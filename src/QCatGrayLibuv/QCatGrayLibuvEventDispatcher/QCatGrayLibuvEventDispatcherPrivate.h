#ifndef QCATGRAYLIBUVEVENTDISPATCHERPRIVATE_H
#define QCATGRAYLIBUVEVENTDISPATCHERPRIVATE_H

#include <QtCore/QAbstractEventDispatcher>
#include <QtCore/QAtomicInt>
#include <QtCore/QHash>
#include <QtCore/QPointer>
#include <uv.h>

struct TimerInfo
{
    QObject *m_pObject = nullptr;
    uv_timer_t m_Uv_timer_t;
    int m_TimerId;
    qint64 m_Interval;
    Qt::TimerType m_Type;
};

struct ZeroTimer
{
    QObject *m_pObject = nullptr;
    bool m_Active;
};

class QCatGrayLibuvEventDispatcher;

class Q_DECL_HIDDEN QCatGrayLibuvEventDispatcherPrivate
{
public:
    explicit QCatGrayLibuvEventDispatcherPrivate(QCatGrayLibuvEventDispatcher *const q_libuveventdispatcher);
    ~QCatGrayLibuvEventDispatcherPrivate();

    bool processEvents(QEventLoop::ProcessEventsFlags flags);
    void registerSocketNotifier(QSocketNotifier *notifier);
    void unregisterSocketNotifier(QSocketNotifier *notifier);
    void registerTimer(int timerId, qint64 interval, Qt::TimerType type, QObject *object);
    void registerZeroTimer(int timerId, QObject *object);
    bool unregisterTimer(int timerId);
    bool unregisterTimers(QObject *object);
    QList<QAbstractEventDispatcher::TimerInfo> registeredTimers(QObject *object) const;
    int remainingTime(int timerId) const;

    typedef QHash<QSocketNotifier *, uv_poll_t *> SocketNotifierHash;
    typedef QHash<int, TimerInfo *> TimerHash;
    typedef QPair<QPointer<QObject>, QEvent *> PendingEvent;
    typedef QList<PendingEvent> EventList;
    typedef QHash<int, ZeroTimer> ZeroTimerHash;

private:
    Q_DISABLE_COPY(QCatGrayLibuvEventDispatcherPrivate)
    Q_DECLARE_PUBLIC(QCatGrayLibuvEventDispatcher)
    QCatGrayLibuvEventDispatcher *const q_ptr;

    bool m_Interrupt;
    uv_loop_t *m_Base = nullptr;
    uv_async_t m_Wakeup;
    QAtomicInt m_Wakeups;
    SocketNotifierHash m_Notifiers;
    TimerHash m_Timers;
    EventList m_Event_list;
    ZeroTimerHash m_Zero_timers;
    bool m_Awaken;

    static void socket_notifier_callback(uv_poll_t *w, int status, int events);
    static void timer_callback(uv_timer_t *w);
    static void wake_up_handler(uv_async_t *w);

    bool disableSocketNotifiers(bool disable);
    void killSocketNotifiers();
    bool disableTimers(bool disable);
    void killTimers();
    bool processZeroTimers();
};

#endif
