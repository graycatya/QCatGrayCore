#include <QCoreApplication>
#include <QThread>
#include <QTimer>
#include "QCatGrayLibuvTcpClient.h"
#include <QCatGrayLibuvEventDispatcher.h>
#include <QDebug>
#include <QTimer>


int main(int argc, char *argv[])
{
    qDebug() << "tcpclientlibuvtest main thread tid:" << getpid();
    QCoreApplication a(argc, argv);
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    qDebug() << "current endian is big";
#endif

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    qDebug() << "current endian is little";
#endif

    QCatGrayLibuvTcpClient tcpclient("0.0.0.0", 51739);
    //move to child thread
    QThread thread;
    thread.setObjectName("networkThread");
    thread.setEventDispatcher(new QCatGrayLibuvEventDispatcher);
    tcpclient.moveToThread(&thread);
    thread.start();

    QTimer::singleShot(0, &tcpclient, &QCatGrayLibuvTcpClient::OpenSocket);

    return a.exec();
}
