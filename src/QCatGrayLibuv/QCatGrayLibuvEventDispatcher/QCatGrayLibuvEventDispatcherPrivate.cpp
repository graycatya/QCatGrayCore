#include "QCatGrayLibuvEventDispatcherPrivate.h"
#include "QCatGrayLibuvEventDispatcher.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QSocketNotifier>
#include <QtCore/QTimerEvent>

QCatGrayLibuvEventDispatcherPrivate::QCatGrayLibuvEventDispatcherPrivate(QCatGrayLibuvEventDispatcher * const q_libuveventdispatcher)
    : q_ptr(q_libuveventdispatcher)
    , m_Interrupt(false)
    , m_Base(new uv_loop_t)
    //, m_Wakeup(0)
    , m_Awaken(false)
{
    uv_loop_init(m_Base);
    m_Base->data = this;
    uv_async_init(m_Base, &m_Wakeup, wake_up_handler);
}

QCatGrayLibuvEventDispatcherPrivate::~QCatGrayLibuvEventDispatcherPrivate()
{
    if(m_Base) {
        killTimers();
        killSocketNotifiers();
        uv_close(reinterpret_cast<uv_handle_t *>(&m_Wakeup), nullptr);
        uv_run(m_Base, UV_RUN_DEFAULT);
        uv_loop_close(m_Base);
        delete m_Base;
        m_Base = nullptr;
    }
}

bool QCatGrayLibuvEventDispatcherPrivate::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    Q_Q(QCatGrayLibuvEventDispatcher);

    const bool exclude_notifiers = (flags & QEventLoop::ExcludeSocketNotifiers);
    const bool exclude_timers = (flags & QEventLoop::X11ExcludeTimers);


    exclude_notifiers &&disableSocketNotifiers(true);
    exclude_timers &&disableTimers(true);

    m_Interrupt = false;
    m_Awaken = false;

    bool result = !m_Event_list.isEmpty() || !m_Zero_timers.isEmpty() || !m_Timers.isEmpty()
                  || !m_Notifiers.isEmpty();

    Q_EMIT q->awake();
    QCoreApplication::sendPostedEvents();

    bool can_wait = !m_Interrupt && (flags & QEventLoop::WaitForMoreEvents) && !result;
    uv_run_mode mode = UV_RUN_NOWAIT;

    if (!m_Interrupt) {
        if (!exclude_timers && !m_Zero_timers.isEmpty()) {
            result |= processZeroTimers();
            if (result) {
                can_wait = false;
            }
        }

        if (can_wait) {
            Q_EMIT q->aboutToBlock();
            mode = UV_RUN_ONCE;
        }

        uv_run(m_Base, mode);

        EventList list;
        m_Event_list.swap(list);
        result |= (!list.isEmpty() || m_Awaken);

        for (const PendingEvent &e : list) {
            if (!e.first.isNull()) {
                QCoreApplication::sendEvent(e.first, e.second);
            }
            delete e.second;
        }
    }

    exclude_notifiers &&disableSocketNotifiers(false);
    exclude_timers &&disableTimers(false);

    return result;
}

void QCatGrayLibuvEventDispatcherPrivate::registerSocketNotifier(QSocketNotifier *notifier)
{
    int events = 0;
    switch (notifier->type()) {
    case QSocketNotifier::Read:
        events = UV_READABLE;
        break;
    case QSocketNotifier::Write:
        events = UV_WRITABLE;
        break;
    case QSocketNotifier::Exception:
        return;
    }

    auto poll_handle = new uv_poll_t;
    uv_poll_init_socket(m_Base, poll_handle, notifier->socket());
    poll_handle->data = notifier;
    m_Notifiers.insert(notifier, poll_handle);
    uv_poll_start(poll_handle, events, socket_notifier_callback);
}

void QCatGrayLibuvEventDispatcherPrivate::unregisterSocketNotifier(QSocketNotifier *notifier)
{
    auto it = m_Notifiers.find(notifier);
    if (it != m_Notifiers.end()) {
        uv_poll_t *handle = it.value();
        m_Notifiers.erase(it);
        uv_close(reinterpret_cast<uv_handle_t *>(handle),
                 [](uv_handle_t *h) { delete reinterpret_cast<uv_poll_t *>(h); });
    }
}

void QCatGrayLibuvEventDispatcherPrivate::registerTimer(int timerId, qint64 interval, Qt::TimerType type, QObject *object)
{
    auto info = new TimerInfo{object, {}, timerId, interval, type};
    uv_timer_init(m_Base, &info->m_Uv_timer_t);
    info->m_Uv_timer_t.data = info;
    m_Timers.insert(timerId, info);
    uv_timer_start(&info->m_Uv_timer_t, timer_callback, interval, 0);
}

void QCatGrayLibuvEventDispatcherPrivate::registerZeroTimer(int timerId, QObject *object)
{
    ZeroTimer timer{object, true};
    m_Zero_timers.insert(timerId, timer);
}

bool QCatGrayLibuvEventDispatcherPrivate::unregisterTimer(int timerId)
{
    auto it = m_Timers.find(timerId);
    if (it != m_Timers.end()) {
        TimerInfo *info = it.value();
        m_Timers.erase(it);
        uv_timer_stop(&info->m_Uv_timer_t);
        uv_close(reinterpret_cast<uv_handle_t *>(&info->m_Uv_timer_t),
                 [](uv_handle_t *h) { delete static_cast<TimerInfo *>(h->data); });
        return true;
    }
    return m_Zero_timers.remove(timerId) > 0;
}

bool QCatGrayLibuvEventDispatcherPrivate::unregisterTimers(QObject *object)
{
    bool result = false;
    for (auto it = m_Timers.begin(); it != m_Timers.end();) {
        TimerInfo *info = it.value();
        if (info->m_pObject == object) {
            uv_timer_stop(&info->m_Uv_timer_t);
            uv_close(reinterpret_cast<uv_handle_t *>(&info->m_Uv_timer_t),
                     [](uv_handle_t *h) { delete static_cast<TimerInfo *>(h->data); });
            it = m_Timers.erase(it);
            result = true;
        } else {
            ++it;
        }
    }
    for (auto it = m_Zero_timers.begin(); it != m_Zero_timers.end();) {
        if (it.value().m_pObject == object) {
            it = m_Zero_timers.erase(it);
            result = true;
        } else {
            ++it;
        }
    }
    return result;
}

QList<QAbstractEventDispatcher::TimerInfo> QCatGrayLibuvEventDispatcherPrivate::registeredTimers(QObject *object) const
{
    QList<QAbstractEventDispatcher::TimerInfo> result;
    for (const auto &info : m_Timers) {
        if (info->m_pObject == object) {
            result.append({info->m_TimerId, static_cast<int>(info->m_Interval), info->m_Type});
        }
    }
    for (auto it = m_Zero_timers.begin(); it != m_Zero_timers.end(); ++it) {
        if (it.value().m_pObject == object) {
            result.append({it.key(), 0, Qt::PreciseTimer});
        }
    }
    return result;
}

int QCatGrayLibuvEventDispatcherPrivate::remainingTime(int timerId) const
{
    auto it = m_Timers.find(timerId);
    if (it != m_Timers.end()) {
        TimerInfo *info = it.value();
        return static_cast<int>(info->m_Interval);
    }
    return -1;
}

void QCatGrayLibuvEventDispatcherPrivate::socket_notifier_callback(uv_poll_t *w, int status, int events)
{
    QSocketNotifier *notifier = static_cast<QSocketNotifier *>(w->data);
    QCatGrayLibuvEventDispatcherPrivate *self = static_cast<QCatGrayLibuvEventDispatcherPrivate *>(w->loop->data);

    if (status < 0) {
        qWarning("Socket notifier error: %s", uv_strerror(status));
        return;
    }

    QSocketNotifier::Type type = notifier->type();
    if ((type == QSocketNotifier::Read && (events & UV_READABLE))
        || (type == QSocketNotifier::Write && (events & UV_WRITABLE))) {
        // 使用 QSocketNotifier 的 event() 方法来创建事件
        QEvent *event = new QEvent(QEvent::SockAct);
        self->m_Event_list.append(PendingEvent(notifier, event));
    }
}

void QCatGrayLibuvEventDispatcherPrivate::timer_callback(uv_timer_t *w)
{
    TimerInfo *info = static_cast<TimerInfo *>(w->data);
    QCatGrayLibuvEventDispatcherPrivate *self = static_cast<QCatGrayLibuvEventDispatcherPrivate *>(w->loop->data);
    self->m_Event_list.append(PendingEvent(info->m_pObject, new QTimerEvent(info->m_TimerId)));
}

void QCatGrayLibuvEventDispatcherPrivate::wake_up_handler(uv_async_t *w)
{
    QCatGrayLibuvEventDispatcherPrivate *self = static_cast<QCatGrayLibuvEventDispatcherPrivate *>(w->loop->data);
    self->m_Awaken = true;
    self->m_Wakeups.storeRelease(0);
}

bool QCatGrayLibuvEventDispatcherPrivate::disableSocketNotifiers(bool disable)
{
    for (auto it = m_Notifiers.begin(); it != m_Notifiers.end(); ++it) {
        uv_poll_t *handle = it.value();
        if (disable) {
            uv_poll_stop(handle);
        } else {
            QSocketNotifier *notifier = it.key();
            int events = 0;
            switch (notifier->type()) {
            case QSocketNotifier::Read:
                events = UV_READABLE;
                break;
            case QSocketNotifier::Write:
                events = UV_WRITABLE;
                break;
            default:
                continue;
            }
            uv_poll_start(handle, events, socket_notifier_callback);
        }
    }
    return true;
}

void QCatGrayLibuvEventDispatcherPrivate::killSocketNotifiers()
{
    for (auto it = m_Notifiers.begin(); it != m_Notifiers.end(); ++it) {
        uv_poll_t *handle = it.value();
        uv_close(reinterpret_cast<uv_handle_t *>(handle),
                 [](uv_handle_t *h) { delete reinterpret_cast<uv_poll_t *>(h); });
    }
    m_Notifiers.clear();
}

bool QCatGrayLibuvEventDispatcherPrivate::disableTimers(bool disable)
{
    for (auto it = m_Timers.begin(); it != m_Timers.end(); ++it) {
        TimerInfo *info = it.value();
        if (disable) {
            uv_timer_stop(&info->m_Uv_timer_t);
        } else {
            uv_timer_start(&info->m_Uv_timer_t, timer_callback, info->m_Interval, 0);
        }
    }
    return true;
}

void QCatGrayLibuvEventDispatcherPrivate::killTimers()
{
    for (auto it = m_Timers.begin(); it != m_Timers.end(); ++it) {
        TimerInfo *info = it.value();
        uv_timer_stop(&info->m_Uv_timer_t);
        uv_close(reinterpret_cast<uv_handle_t *>(&info->m_Uv_timer_t),
                 [](uv_handle_t *h) { delete static_cast<TimerInfo *>(h->data); });
    }
    m_Timers.clear();
    m_Zero_timers.clear();
}

bool QCatGrayLibuvEventDispatcherPrivate::processZeroTimers()
{
    bool result = false;
    QList<int> ids = m_Zero_timers.keys();

    for (int timerId : ids) {
        auto it = m_Zero_timers.find(timerId);
        if (it != m_Zero_timers.end()) {
            ZeroTimer &data = it.value();
            if (data.m_Active) {
                data.m_Active = false;
                QTimerEvent event(timerId);
                QCoreApplication::sendEvent(data.m_pObject, &event);
                result = true;

                it = m_Zero_timers.find(timerId);
                if (it != m_Zero_timers.end()) {
                    it.value().m_Active = true;
                }
            }
        }
    }

    return result;
}
