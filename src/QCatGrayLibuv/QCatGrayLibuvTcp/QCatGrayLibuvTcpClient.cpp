#include "QCatGrayLibuvTcpClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include <QDebug>

#include <QHostAddress>
#include <QDataStream>
#include <QCatGrayLibuvEventDispatcher.h>


#define MAX_STRING_LEN 256

QHash<uv_tcp_t*, QCatGrayLibuvTcpClient*> QCatGrayLibuvTcpClient::m_pTcpSocketToTcpClient = QHash<uv_tcp_t*, QCatGrayLibuvTcpClient*>();

QCatGrayLibuvTcpClient::QCatGrayLibuvTcpClient(const QString &ip, quint16 port, QObject *parent)
    : QObject(parent)
    , m_Ip(ip)
    , m_Port(port)
    , m_Start(false)
    , m_Thread(nullptr)
    , m_isConnect(false)
{

    m_Thread = new QThread;
    m_Thread->setEventDispatcher(new QCatGrayLibuvEventDispatcher);
    this->moveToThread(m_Thread);
    m_Thread->start();
}

QCatGrayLibuvTcpClient::~QCatGrayLibuvTcpClient()
{
    CloseSocket();
}

void QCatGrayLibuvTcpClient::SendData(const QByteArray &data)
{
    if(m_Start == true)
    {
        QMetaObject::invokeMethod(this,
                                  "SendDataPrivate",
                                  Q_ARG(const QByteArray &, data));
    }
}

void QCatGrayLibuvTcpClient::OpenSocket()
{
    if(m_Start == false
        && m_Thread->isRunning()
        )
    {
        m_Start = true;

        QHostAddress hostaddress(m_Ip);
        m_NetworkLayerProtocol = hostaddress.protocol();
        if(m_NetworkLayerProtocol == QAbstractSocket::NetworkLayerProtocol::UnknownNetworkLayerProtocol)
        {
            m_Start = false;
        }

        if(m_Start != true)
        {
            return;
        }

        QCatGrayLibuvEventDispatcher *dispatcher = static_cast<QCatGrayLibuvEventDispatcher *>(
            QAbstractEventDispatcher::instance());
        uv_loop_t *loop = dispatcher->uvLoop();
        m_Tcp_socket = QSharedPointer<uv_tcp_t>(new uv_tcp_t, [](uv_tcp_t *p){
            //qDebug() << "delete m_Tcp_socket";
            delete p;
        });
        m_Tcp_connect = QSharedPointer<uv_connect_t>(new uv_connect_t, [](uv_connect_t *p){
            //qDebug() << "delete m_Tcp_connect";
            delete p;
        });
        m_Tcp_write = QSharedPointer<uv_write_t>(new uv_write_t, [](uv_write_t *p){
            //qDebug() << "delete m_Tcp_write";
            delete p;
        });
        uv_handle_set_data((uv_handle_t *) m_Tcp_socket.get(), this);

        uv_tcp_init(loop, m_Tcp_socket.get());

        m_pTcpSocketToTcpClient.insert(m_Tcp_socket.data(), this);

        if(m_NetworkLayerProtocol == QAbstractSocket::NetworkLayerProtocol::IPv4Protocol)
        {
            struct sockaddr_in addr;
            uv_ip4_addr(m_Ip.toStdString().c_str(), m_Port, &addr);
            uv_tcp_connect(m_Tcp_connect.data(), m_Tcp_socket.data(), (const struct sockaddr*)&addr, Connect_Cb);
        } else if(m_NetworkLayerProtocol == QAbstractSocket::NetworkLayerProtocol::IPv6Protocol)
        {
            struct sockaddr_in6 addr;
            uv_ip6_addr(m_Ip.toStdString().c_str(), m_Port, &addr);
            uv_tcp_connect(m_Tcp_connect.data(), m_Tcp_socket.data(), (const struct sockaddr*)&addr, Connect_Cb);
        }


    }
}

void QCatGrayLibuvTcpClient::CloseSocketed()
{
    qDebug() << "CloseSocketed this: " << this;
    uv_close((uv_handle_t*)m_Tcp_socket.data(), Close_Cb);
}

void QCatGrayLibuvTcpClient::SendDataPrivate(const QByteArray &data)
{
    uv_buf_t buf = uv_buf_init(const_cast<char*>(data.data()), data.size());
    uv_write(m_Tcp_write.data(), (uv_stream_t*)m_Tcp_socket.data(), &buf, 1, Write_Cb);
}

void QCatGrayLibuvTcpClient::CloseSocket()
{
    if(m_Start)
    {
        qDebug() << "close this: " << this;
        QMetaObject::invokeMethod(this,
                                  "CloseSocketed");
        QMutexLocker locker(&m_CloseMutex);
        m_CloseCondition.wait(&m_CloseMutex);
    }
}

void QCatGrayLibuvTcpClient::Read_Cb(uv_stream_t* stream,
                                     ssize_t nread,
                                     const uv_buf_t* buf)
{
    if (nread > 0) {

        QByteArray receivedData(buf->base, nread);
        emit m_pTcpSocketToTcpClient[(uv_tcp_t*)stream]->DataReceived(receivedData);
        return;
    }
    if (nread < 0) {
        if (nread != UV_EOF)
        {
            emit m_pTcpSocketToTcpClient[(uv_tcp_t*)stream]->Errored(nread, "read error");
        }
        uv_close((uv_handle_t*)stream, Close_Cb);
    }
    if(buf->base)
    {
        free(buf->base);
    }
}

void QCatGrayLibuvTcpClient::Alloc_Cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{

    buf->base = (char*)malloc(suggested_size);
    buf->len = suggested_size;
}

void QCatGrayLibuvTcpClient::Connect_Cb(uv_connect_t *req, int status)
{
    int r;

    if(status != 0)
    {
        emit m_pTcpSocketToTcpClient[(uv_tcp_t*)req->handle]->Errored(status, "connect error");
        return;
    }
    qDebug() << "Connect_Cb";
    r = uv_read_start(req->handle, Alloc_Cb, Read_Cb);
    if(r != 0)
    {
        emit m_pTcpSocketToTcpClient[(uv_tcp_t*)req->handle]->Errored(-1, "connect read start error");
    }
    m_pTcpSocketToTcpClient[(uv_tcp_t*)req->handle]->m_isConnect = true;
    emit m_pTcpSocketToTcpClient[(uv_tcp_t*)req->handle]->Connected();
    //r = uv_shutdown(ThisClient->m_Tcp_shutdown.data(), req->handle, Stutdown_Cb);

}

void QCatGrayLibuvTcpClient::Write_Cb(uv_write_t *req, int status)
{
    if(status != 0)
    {
        emit m_pTcpSocketToTcpClient[(uv_tcp_t*)req->handle]->Errored(status, "write error");
        return;
    }
    if(req)
    {

    }
}

void QCatGrayLibuvTcpClient::Stutdown_Cb(uv_shutdown_t *req, int status)
{
    if(req)
    {
        qDebug() << "Stutdown_Cb";
        uv_close((uv_handle_t*)req->handle, Close_Cb);
    }
}

void QCatGrayLibuvTcpClient::Close_Cb(uv_handle_t *handle)
{
    if(handle)
    {
        qDebug() << "Close_Cb";
        qDebug() << "QCatGrayLibuvTcpClient this: " << m_pTcpSocketToTcpClient[(uv_tcp_t*)handle];
        if(m_pTcpSocketToTcpClient[(uv_tcp_t*)handle]->m_Start)
        {
            m_pTcpSocketToTcpClient[(uv_tcp_t*)handle]->m_Start = false;
            m_pTcpSocketToTcpClient[(uv_tcp_t*)handle]->m_Tcp_connect.clear();
            m_pTcpSocketToTcpClient[(uv_tcp_t*)handle]->m_Tcp_socket.clear();
            m_pTcpSocketToTcpClient[(uv_tcp_t*)handle]->m_Tcp_write.clear();
        }
        m_pTcpSocketToTcpClient[(uv_tcp_t*)handle]->m_isConnect = false;
        QMutexLocker locker(&m_pTcpSocketToTcpClient[(uv_tcp_t*)handle]->m_CloseMutex);
        m_pTcpSocketToTcpClient[(uv_tcp_t*)handle]->m_CloseCondition.wakeAll();
        emit m_pTcpSocketToTcpClient[(uv_tcp_t*)handle]->Closeed();
        m_pTcpSocketToTcpClient.remove((uv_tcp_t*)handle);

    }
}
