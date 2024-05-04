﻿#pragma once

#include <QThread>
#include <QNetworkReply>
#include <QMutex>
#include <QWaitCondition>
#include <QFile>

class QCatGrayNetWorkHttp : public QThread
{
    Q_OBJECT
    Q_ENUMS(CATNETWORKERROR)
    Q_ENUMS(HTTPSTATE)
public:
    enum CATNETWORKERROR {
        NORMAL,
        ISSTART,
        DOWNLOADPATHERROR,
        FILEOPENERROR,
        URLERROR
    };
    enum HTTPSTATE {
        NONE,
        DOWNLOAD,
        HTTPGET,
        HTTPPOST,
        HTTPMULTIPARTPOST,
        HTTPPUS
    };
    explicit QCatGrayNetWorkHttp(QObject *parent = nullptr);
    ~QCatGrayNetWorkHttp();

public:
    int DownLoad(QUrl url, QString downloaddir, bool ssl = false);
    int HttpGet(QUrl url, QVariantHash heads, QUrlQuery query, bool ssl = false);
    int HttpPost(QUrl url, QVariantHash heads, QUrlQuery query, QByteArray& data, bool ssl = false);
    int HttpPost(QUrl url, QVariantHash heads, QUrlQuery query, QHttpMultiPart* data, bool ssl = false);

protected:
    void run() override;

private:
    void InitHttpDownLoad(QNetworkAccessManager *m_pManager);
    void InitHttpGet(QNetworkAccessManager *m_pManager);
    void InitHttpPost(QNetworkAccessManager *m_pManager);
    void InitHttpMultiPartPost(QNetworkAccessManager *m_pManager);
    void SetHttpState(HTTPSTATE state);

signals:
    void DownLoadFinished(QString filePath);
    void DownLoadError();
    void HttpPostError();
    void DownLoadProgress(qint64, qint64);
    void NetWorkError();
    void ReplyDataed(QByteArray data);
    void ExitTasked();
    void UpdateHttpState();



public slots:
    void ExitTask();

private slots:
    void httpDownFinished();
    void httpDownReadyRead();
    void httpError(QNetworkReply::NetworkError);
    void updateDataReadProgress(qint64, qint64);

private:
    bool m_bStart;
    bool m_bWork;

    QVariant m_pVar;

    QFile *m_pFile;
    HTTPSTATE m_yState;
};

