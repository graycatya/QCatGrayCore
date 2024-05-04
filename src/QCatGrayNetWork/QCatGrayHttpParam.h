#pragma once

#include <QObject>
#include <QHttpPart>
#include <QVector>

class QCatGrayHttpParam : public QObject
{
    Q_OBJECT
public:
    explicit QCatGrayHttpParam(QObject *parent = nullptr);

    QHttpPart AddHttpParam(const QString &key, const QString &value);
    QHttpPart AddHttpFileParam(QHttpMultiPart *multipart, const QString &key = "iamge", const QString &filepath = "");
    QHttpPart AddHttpImageParam(QHttpMultiPart *multipart, const QString &key = "iamge", const QString &imagepath = "", const QString &type = "image/png");
    QVector<QHttpPart> GetHttpParams();
    void ClearParams();

private:
    QVector<QHttpPart> m_yParts;
};

