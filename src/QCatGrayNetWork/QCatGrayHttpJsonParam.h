#pragma once

#include <QObject>
#include <QJsonObject>

class QCatGrayHttpJsonParam : public QObject
{
    Q_OBJECT
public:
    explicit QCatGrayHttpJsonParam(QObject *parent = nullptr);

    void AddHttpParam(const QString &key, const QJsonValue &value);
    void AddHttpParamObject(const QString &key, const QJsonObject &value);
    void AddHttpParamArray(const QString &key, const QJsonArray &value);
    QByteArray GetHttpParams();

private:
    QJsonObject m_yParamObject;
};


