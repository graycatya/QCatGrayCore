#include "QCatGrayHttpJsonParam.h"
#include <QJsonDocument>
#include <QJsonArray>

QCatGrayHttpJsonParam::QCatGrayHttpJsonParam(QObject *parent)
    : QObject(parent)
{

}

void QCatGrayHttpJsonParam::AddHttpParam(const QString &key, const QJsonValue &value)
{
    m_yParamObject.insert(key, value);
}

void QCatGrayHttpJsonParam::AddHttpParamObject(const QString &key, const QJsonObject &value)
{
    m_yParamObject.insert(key, value);
}

void QCatGrayHttpJsonParam::AddHttpParamArray(const QString &key, const QJsonArray &value)
{
    m_yParamObject.insert(key, value);
}

QByteArray QCatGrayHttpJsonParam::GetHttpParams()
{
    QJsonDocument document = QJsonDocument(m_yParamObject);
    QByteArray params = document.toJson();
    return params;
}
