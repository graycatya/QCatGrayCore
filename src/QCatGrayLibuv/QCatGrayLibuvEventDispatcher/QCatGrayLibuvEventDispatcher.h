#ifndef QCATGRAYLIBUVEVENTDISPATCHER_H
#define QCATGRAYLIBUVEVENTDISPATCHER_H

#include <QtCore/QAbstractEventDispatcher>
#include <uv.h>

class QCatGrayLibuvEventDispatcherPrivate;

class QCatGrayLibuvEventDispatcher : public QAbstractEventDispatcher
{
    Q_OBJECT
public:
    explicit QCatGrayLibuvEventDispatcher(QObject *parent = nullptr);
    ~QCatGrayLibuvEventDispatcher() override;

    bool processEvents(QEventLoop::ProcessEventsFlags flags) override;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    bool hasPendingEvents(void) override;
#endif
    void registerSocketNotifier(QSocketNotifier *notifier) override;
    void unregisterSocketNotifier(QSocketNotifier *notifier) override;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    void registerTimer(int timerId, int interval, Qt::TimerType timerType, QObject *object) override;
#else
    void registerTimer(int timerId, qint64 interval, Qt::TimerType timerType, QObject *object) override;
#endif
    bool unregisterTimer(int timerId) override;
    bool unregisterTimers(QObject *object) override;
    QList<QAbstractEventDispatcher::TimerInfo> registeredTimers(QObject *object) const override;
    int remainingTime(int timerId) override;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#if defined(Q_OS_WIN) || defined(Q_CLANG_QDOC)
    bool registerEventNotifier(QWinEventNotifier *notifier) override;
    void unregisterEventNotifier(QWinEventNotifier *notifier) override;
#endif
#endif

    void wakeUp() override;
    void interrupt() override;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    void flush() override;
#endif
    void startingUp() override;
    void closingDown() override;

    uv_loop_t *uvLoop() const;

protected:
    QCatGrayLibuvEventDispatcher(QCatGrayLibuvEventDispatcherPrivate &d_private, QObject *parent = nullptr);

private:
    Q_DISABLE_COPY(QCatGrayLibuvEventDispatcher)
    Q_DECLARE_PRIVATE(QCatGrayLibuvEventDispatcher)
    QScopedPointer<QCatGrayLibuvEventDispatcherPrivate> d_ptr;


};

#endif
