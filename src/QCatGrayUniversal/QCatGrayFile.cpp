#include "QCatGrayFile.h"
#include <QFile>
#include <QString>
#include <QVariant>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QFileInfo>

QCatGrayFile::QCatGrayFile()
{

}

QCatGrayFile::~QCatGrayFile()
{

}

bool QCatGrayFile::ReadFile(const QString &filePath, QByteArray &content)
{
    QFile file(filePath);
    if(!file.open(QFile::ReadOnly))
    {
        return false;
    }
    content = file.readAll();
    file.close();
    return true;
}


bool QCatGrayFile::WriteFile(const QString &filePath, QByteArray &content)
{
    QFile file(filePath);
    if(!file.open(QFile::WriteOnly))
    {
        return false;
    }
    file.write(content);
    file.close();
    return true;
}

bool QCatGrayFile::Existe(const QString &filePath, bool newfile)
{
    QFileInfo fileinfo(filePath);
    if(!fileinfo.exists())
    {
        bool ret = false;
        if(newfile)
        {
            QDir dir;
            ret = dir.mkpath(fileinfo.filePath());
        }
        return ret;
    }
    return true;
}
