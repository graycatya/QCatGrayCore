#include "QCatGrayHttpParam.h"
#include <QFile>
#include <QFileInfo>


QCatGrayHttpParam::QCatGrayHttpParam(QObject *parent)
    : QObject(parent)
{
    m_yParts.clear();
}

QHttpPart QCatGrayHttpParam::AddHttpParam(const QString &key, const QString &value)
{
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"%1\"").arg(key)));
    part.setBody(value.toUtf8());
    m_yParts.push_back(part);
    return part;
}

QHttpPart QCatGrayHttpParam::AddHttpFileParam(QHttpMultiPart *multipart, const QString &key, const QString &filepath)
{
    QHttpPart filePart;
    if(multipart)
    {
        QFileInfo fileinfo;
        fileinfo.setFile(filepath);
        filePart.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"%1\";filename=\"%2\";").arg(key).arg(fileinfo.fileName())));
        QFile *file = new QFile(filepath);
        file->open(QIODevice::ReadOnly);
        filePart.setBodyDevice(file);
        file->setParent(multipart);
    }
    m_yParts.push_back(filePart);
    return filePart;
}

QHttpPart QCatGrayHttpParam::AddHttpImageParam(QHttpMultiPart *multipart, const QString &key, const QString &imagepath, const QString &type)
{
    QHttpPart imagePart;
    if(multipart)
    {
        QFileInfo fileinfo;
        fileinfo.setFile(imagepath);
        imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(type));
        imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"%1\"").arg(key)));
        QFile *file = new QFile(imagepath);
        file->open(QIODevice::ReadOnly);
        imagePart.setBodyDevice(file);
        file->setParent(multipart);
    }
    m_yParts.push_back(imagePart);
    return imagePart;
}

QVector<QHttpPart> QCatGrayHttpParam::GetHttpParams()
{
    return m_yParts;
}

void QCatGrayHttpParam::ClearParams()
{
    m_yParts.clear();
}
