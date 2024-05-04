#pragma once


#include <QObject>

class QCatGrayFile
{

public:
    QCatGrayFile();
    ~QCatGrayFile();

    static bool ReadFile(const QString &filePath, QByteArray &content);

    static bool WriteFile(const QString &filePath, QByteArray &content);

    static bool Existe(const QString &filePath, bool newfile = false);


};

