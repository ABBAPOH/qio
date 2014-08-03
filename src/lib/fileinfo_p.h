#ifndef FILEINFO_P_H
#define FILEINFO_P_H

#include "fileinfo.h"

#include <QtCore/QSharedData>

class FileInfoData : public QSharedData
{
public:
    QUrl url;
    bool exists;
    bool isDir;
    qint64 size;

    QDateTime created;
    QDateTime lastModified;
    QDateTime lastRead;

    QString mimeTypeName;
};

#endif // FILEINFO_P_H
