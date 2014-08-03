#include "fileinfo.h"
#include "fileinfo_p.h"

#include <QFileInfo>

FileInfo::FileInfo() :
    d(new FileInfoData)
{
}

FileInfo::FileInfo(const FileInfo &other) :
    d(other.d)
{
}

FileInfo::FileInfo(FileInfoData *dd) :
    d(dd)
{
}

FileInfo &FileInfo::operator=(const FileInfo &other)
{
    if (this != &other)
        d.operator=(other.d);
    return *this;
}

FileInfo::~FileInfo()
{
}

QUrl FileInfo::url() const
{
    return d->url;
}

bool FileInfo::exists() const
{
    return d->exists;
}

bool FileInfo::isDir() const
{
    return d->isDir;
}

QString FileInfo::fileName() const
{
    return QFileInfo(d->url.path()).fileName();
}

qint64 FileInfo::size() const
{
    return d->size;
}

QDateTime FileInfo::created() const
{
    return d->created;
}

QDateTime FileInfo::lastModified() const
{
    return d->lastModified;
}

QDateTime FileInfo::lastRead() const
{
    return d->lastRead;
}

QString FileInfo::mimeTypeName() const
{
    return d->mimeTypeName;
}
