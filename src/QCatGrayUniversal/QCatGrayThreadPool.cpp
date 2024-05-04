﻿#include "QCatGrayThreadPool.h"
#include <QTimer>
#include <QDebug>

QCatGrayThreadPool::QCatGrayThreadPool(QObject *parent, int InitialThreadCount)
    : QObject(parent)
    , m_iDefaultThreadCount(InitialThreadCount)
    , m_iThreadWorkCount(0)
    , m_bHandle_NonWork_Threads(false)
    , m_pHandle_NonWork_Threads(nullptr)
{
    //[0] 初始化
    m_pThreadList.clear();
    //[1] 线程池里的线程
    for(int i = 0; i < InitialThreadCount; i++)
    {
        QCatGrayThread *thread = new QCatGrayThread(this);
        m_pThreadList.push_back(thread);
    }
    //[2] 处理不工作线程
    InitHandleNonWorkTimer();
}

QCatGrayThreadPool::~QCatGrayThreadPool()
{
    if(m_pHandle_NonWork_Threads != nullptr)
    {
        m_bHandle_NonWork_Threads = false;
        m_pHandle_NonWork_Threads->deleteLater();
        m_pHandle_NonWork_Threads = nullptr;
    }

    for(auto thread : m_pThreadList)
    {
        delete thread;
        m_pThreadList.removeOne(thread);
        thread = nullptr;
    }
}

int QCatGrayThreadPool::GetThreadCount()
{
    m_pHandleMutex.lock();
    int ThreadCount = m_pThreadList.size();
    m_pHandleMutex.unlock();
    return ThreadCount;
}

int QCatGrayThreadPool::GetThreadWorkCount()
{
    int ThreadWorkCount = 0;
    m_pHandleMutex.lock();
    for(auto thread : m_pThreadList)
    {
        if(thread->GetWork())
        {
            ThreadWorkCount++;
        }
    }
    m_pHandleMutex.unlock();
    return ThreadWorkCount;
}

void QCatGrayThreadPool::InitHandleNonWorkTimer()
{
    m_pHandle_NonWork_Threads = new QCatGrayThread;
    m_pHandle_NonWork_Threads->Start([=](){
        m_bHandle_NonWork_Threads = true;
        while(m_bHandle_NonWork_Threads)
        {
            int handlenonwork = HandleNonWork();
            emit handthreadsed(handlenonwork);
            QThread::sleep(600);
        }
    });
}

int QCatGrayThreadPool::HandleNonWork()
{
    int handnum = 0;

    for(auto thread : m_pThreadList)
    {
        if(!thread->GetWork() && (GetThreadCount() > m_iDefaultThreadCount))
        {
            m_pThreadList.removeOne(thread);
            handnum++;
        }
    }

    return handnum;
}
