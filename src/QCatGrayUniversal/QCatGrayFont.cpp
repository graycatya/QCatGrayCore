#include "QCatGrayFont.h"
#include <QFontDatabase>

QCatGrayFont::QCatGrayFont(QObject *parent)
    : QObject(parent)
{

}

QCatGrayFont::~QCatGrayFont()
{

}

QStringList QCatGrayFont::SystemFontFamilys()
{
    QFontDatabase database;
    QStringList list;
    foreach (const QString &family, database.families())
    {
         list << family;
    }
    return list;
}
