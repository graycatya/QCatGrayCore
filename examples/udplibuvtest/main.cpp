#include <QCoreApplication>
#include <QThread>
#include <QTimer>
#include "UvUdpSocket.h"
#include <QCatGrayLibuvEventDispatcher.h>
#include <QDebug>
//#include <unistd.h>
using namespace shuimo;

int main(int argc, char *argv[])
{
    qDebug() << "QUdpLibuvInefficient main thread tid:" << getpid();
    QCoreApplication a(argc, argv);

    //create socket
    UvUdpSocket socket("0.0.0.0", 9002);
    auto recvCb = [](const QString &addr, quint16 port, const QByteArray &data) {
        qDebug() << QString("recv %1:%2 %3bytes").arg(addr).arg(port).arg(data.size());
    };
    QObject::connect(&socket, &UvUdpSocket::dataReceived, recvCb);

    //move to child thread
    QThread thread;
    thread.setObjectName("networkThread");
    thread.setEventDispatcher(new QCatGrayLibuvEventDispatcher);
    socket.moveToThread(&thread);
    thread.start();

    //excute initSocket() in child thread
    QTimer::singleShot(0, &socket, &UvUdpSocket::initSocket);

    //send data
    QString message("hello world");
    socket.sendData(message.toLocal8Bit(), "127.0.0.1", 9001);

    return a.exec();
}
