#pragma once


#include <QObject>

class QCatGrayFont : public QObject
{
    Q_OBJECT
public:
    explicit QCatGrayFont(QObject *parent = nullptr);
    ~QCatGrayFont();

    static QStringList SystemFontFamilys( void );

private:

};

