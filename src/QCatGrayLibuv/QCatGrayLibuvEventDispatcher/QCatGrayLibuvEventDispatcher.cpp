#include "QCatGrayLibuvEventDispatcher.h"
#include "QCatGrayLibuvEventDispatcherPrivate.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QSocketNotifier>
#include <QtCore/QThread>
#include <QtCore/QTimerEvent>

QCatGrayLibuvEventDispatcher::QCatGrayLibuvEventDispatcher(QObject *parent)
    : QAbstractEventDispatcher(parent)
    , d_ptr(new QCatGrayLibuvEventDispatcherPrivate(this))
{

}

QCatGrayLibuvEventDispatcher::~QCatGrayLibuvEventDispatcher() = default;

bool QCatGrayLibuvEventDispatcher::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    Q_D(QCatGrayLibuvEventDispatcher);
    return d->processEvents(flags);
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
bool QCatGrayLibuvEventDispatcher::hasPendingEvents(void)
{
    extern uint qGlobalPostedEventsCount();
    return qGlobalPostedEventsCount() > 0;
}
#endif
void QCatGrayLibuvEventDispatcher::registerSocketNotifier(QSocketNotifier *notifier)
{
    Q_D(QCatGrayLibuvEventDispatcher);
    d->registerSocketNotifier(notifier);
}

void QCatGrayLibuvEventDispatcher::unregisterSocketNotifier(QSocketNotifier *notifier)
{
    Q_D(QCatGrayLibuvEventDispatcher);
    d->unregisterSocketNotifier(notifier);
}

bool QCatGrayLibuvEventDispatcher::unregisterTimer(int timerId)
{
    Q_D(QCatGrayLibuvEventDispatcher);
    return d->unregisterTimer(timerId);
}

bool QCatGrayLibuvEventDispatcher::unregisterTimers(QObject *object)
{
    Q_D(QCatGrayLibuvEventDispatcher);
    return d->unregisterTimers(object);
}

QList<QAbstractEventDispatcher::TimerInfo> QCatGrayLibuvEventDispatcher::registeredTimers(QObject *object) const
{
    Q_D(const QCatGrayLibuvEventDispatcher);
    return d->registeredTimers(object);
}

int QCatGrayLibuvEventDispatcher::remainingTime(int timerId)
{
    Q_D(const QCatGrayLibuvEventDispatcher);
    return d->remainingTime(timerId);
}
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#if defined(Q_OS_WIN) || defined(Q_CLANG_QDOC)
bool QCatGrayLibuvEventDispatcher::registerEventNotifier(QWinEventNotifier *notifier)
{
    Q_UNUSED(notifier)
    Q_UNIMPLEMENTED();
    return false;
}

void QCatGrayLibuvEventDispatcher::unregisterEventNotifier(QWinEventNotifier *notifier)
{
    Q_UNUSED(notifier)
    Q_UNIMPLEMENTED();
}
#endif
#endif


void QCatGrayLibuvEventDispatcher::wakeUp()
{
    Q_D(QCatGrayLibuvEventDispatcher);
    if (d->m_Wakeups.testAndSetAcquire(0, 1)) {
        uv_async_send(&d->m_Wakeup);
    }
}

void QCatGrayLibuvEventDispatcher::interrupt()
{
    Q_D(QCatGrayLibuvEventDispatcher);
    d->m_Interrupt = true;
    wakeUp();
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void QCatGrayLibuvEventDispatcher::flush()
{

}
#endif

void QCatGrayLibuvEventDispatcher::startingUp()
{

}

void QCatGrayLibuvEventDispatcher::closingDown()
{

}

uv_loop_t *QCatGrayLibuvEventDispatcher::uvLoop() const
{
    Q_D(const QCatGrayLibuvEventDispatcher);
    return d->m_Base;
}

QCatGrayLibuvEventDispatcher::QCatGrayLibuvEventDispatcher(QCatGrayLibuvEventDispatcherPrivate &d_private, QObject *parent)
    : QAbstractEventDispatcher(parent)
    , d_ptr(&d_private)
{

}


#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void QCatGrayLibuvEventDispatcher::registerTimer(int timerId, int interval, Qt::TimerType timerType, QObject *object)
{
    Q_D(QCatGrayLibuvEventDispatcher);
    d->registerTimer(timerId, interval, timerType, object);
}
#else
void QCatGrayLibuvEventDispatcher::registerTimer(int timerId, qint64 interval, Qt::TimerType timerType, QObject *object)
{
    Q_D(QCatGrayLibuvEventDispatcher);
    d->registerTimer(timerId, interval, timerType, object);
}
#endif
