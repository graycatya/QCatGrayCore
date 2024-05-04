#pragma once


#include <QObject>
#include <QQueue>
#include <QThread>
#include <QMutex>
#include "QCatGrayThread.h"

class QCatGrayThreadPool : public QObject
{
    Q_OBJECT
public:
    explicit QCatGrayThreadPool(QObject *parent = nullptr, int InitialThreadCount = QThread::idealThreadCount());
    ~QCatGrayThreadPool();

public:
    int GetThreadCount( void );
    int GetThreadWorkCount( void );

    template<class F, class... Args>
    void AddTask(F&& f, Args&&... args)
    {
        if(GetThreadCount() == GetThreadWorkCount())
        {
            QCatGrayThread *thread = new QCatGrayThread(this);
            m_pHandleMutex.lock();
            m_pThreadList.push_back(thread);
            thread->Start(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
            m_pHandleMutex.unlock();
            return;
        } else {
            for(auto thread : m_pThreadList)
            {
                if(!thread->GetWork())
                {
                    thread->Start(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
                    return;
                }
            }
        }
    }

private:
    void InitHandleNonWorkTimer( void );
    int HandleNonWork( void );

signals:
    // 定时发送处理不工作线程数
    void handthreadsed(int);

private:
    int m_iDefaultThreadCount;
    int m_iThreadWorkCount;
    QList<QCatGrayThread*> m_pThreadList;
    bool m_bHandle_NonWork_Threads;
    QCatGrayThread *m_pHandle_NonWork_Threads; //  处理不工作线程
    QMutex m_pHandleMutex;
    QMutex m_pWrokMutex;

};

