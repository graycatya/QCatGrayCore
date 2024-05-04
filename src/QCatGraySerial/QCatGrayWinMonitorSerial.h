#pragma once

#include <QObject>

#include <QMutex>

#include <QMap>

class QCatGrayWinMonitorSerial : public QObject
{
    Q_OBJECT
public:
    static QCatGrayWinMonitorSerial* Instance() noexcept
    {
        if(_instance == nullptr)
        {
            m_pMutex->lock();
            if(_instance == nullptr)
            {
                _instance = new QCatGrayWinMonitorSerial;
            }
            m_pMutex->unlock();
        }
        return _instance;
    }

    static void Delete( void ) noexcept
    {
        if(_instance != nullptr)
        {
            if(m_pMutex != nullptr)
            {
                delete m_pMutex;
                m_pMutex = nullptr;
            }
            delete _instance;
            _instance = nullptr;
        }
    }

    static QStringList Serials();

    QMap<QString, QString> m_pSerials;

private:
    QCatGrayWinMonitorSerial();
    ~QCatGrayWinMonitorSerial();

signals:
    // 端口插入
    void AddSerial(qint64 pid, qint64 vid, QString serial);
    // 端口拔出
    void DeleteSerial(qint64 pid, qint64 vid, QString serial);
    // 端口插入
    //void AddSerial(QSerialPortInfo);
    // 端口拔出


private:
    static QCatGrayWinMonitorSerial* _instance;

    static QMutex* m_pMutex;

};


