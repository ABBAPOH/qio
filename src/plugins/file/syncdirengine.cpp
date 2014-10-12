#include "syncdirengine.h"

#include <QIO/FileInfoData>
#include <QtCore/QMimeDatabase>

static FileInfo fromQFileInfo(const QFileInfo &info)
{
    FileInfoData *d = new FileInfoData();

    d->url = QUrl::fromLocalFile(info.absoluteFilePath());
    d->exists = info.exists();
    d->isDir = info.isDir();
    d->size = info.size();
    d->created = info.created();
    d->lastModified = info.lastModified();
    d->lastRead = info.lastRead();
    d->mimeTypeName = QMimeDatabase().mimeTypeForUrl(d->url).name();

    return FileInfo(d);
}

QStringList SyncDirEngine::list(QDir::Filters filters)
{
    return QDir(url().toLocalFile()).entryList(filters);
}

QList<FileInfo> SyncDirEngine::entryList(QDir::Filters filters)
{
    QList<FileInfo> result;

    auto infos = QDir(url().toLocalFile()).entryList(filters);
    foreach (const auto &info, infos)
        result.append(fromQFileInfo(info));
    return result;
}

bool SyncDirEngine::mkdir(const QString &dirName)
{
    return QDir(url().toLocalFile()).mkdir(dirName);
}

bool SyncDirEngine::rmdir(const QString &dirName)
{
    return QDir(url().toLocalFile()).rmdir(dirName);
}

bool SyncDirEngine::remove(const QString &fileName)
{
    return QDir(url().toLocalFile()).remove(fileName);
}

FileInfo SyncDirEngine::stat(const QString &fileName)
{
    const QString path = url().path() + "/" + fileName;
    return fromQFileInfo(QFileInfo(path));
}
