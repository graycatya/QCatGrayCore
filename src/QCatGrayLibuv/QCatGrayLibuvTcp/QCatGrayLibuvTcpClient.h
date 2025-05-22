#ifndef QCATGRAYLIBUVTCPCLIENT_H
#define QCATGRAYLIBUVTCPCLIENT_H

#include <QObject>
#include <QAtomicInteger>
#include <QAbstractSocket>
#include <QSharedPointer>
#include <QThread>
#include <QHash>
#include <uv.h>

class QCatGrayLibuvTcpClient : public QObject
{
    Q_OBJECT
public:
    explicit QCatGrayLibuvTcpClient(const QString &ip, quint16 port, QObject *parent = nullptr);
    ~QCatGrayLibuvTcpClient();

    void SendData(const QByteArray &data);

    void CloseSocket();

    bool isConnect() const {
        return m_isConnect;
    }
public slots:
    void OpenSocket();

signals:
    void DataReceived(const QByteArray &data);
    void Errored(int status, QString errorstr);
    void Closeed();
    void Connected();

private slots:
    void CloseSocketed();
    void SendDataPrivate(const QByteArray &data);
private:

    static void Read_Cb(uv_stream_t* stream,
                 ssize_t nread,
                 const uv_buf_t* buf);
    static void Alloc_Cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
    static void Connect_Cb(uv_connect_t* req, int status);
    static void Write_Cb(uv_write_t* req, int status);
    static void Stutdown_Cb(uv_shutdown_t* req, int status);
    static void Close_Cb(uv_handle_t* handle);
private:
    Q_DISABLE_COPY(QCatGrayLibuvTcpClient) //noncopyable QCatGrayLibuvTcpClient
    const QString m_Ip;
    const quint16 m_Port;
    QAtomicInteger<bool> m_Start;
    QAbstractSocket::NetworkLayerProtocol m_NetworkLayerProtocol;
    QSharedPointer<uv_tcp_t> m_Tcp_socket;
    QSharedPointer<uv_connect_t> m_Tcp_connect;
    QSharedPointer<uv_write_t> m_Tcp_write;
    QThread *m_Thread;
    static QHash<uv_tcp_t*, QCatGrayLibuvTcpClient*> m_pTcpSocketToTcpClient;
    bool m_isConnect;

};

#endif
