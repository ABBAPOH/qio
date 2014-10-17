#include "fileentryengine.h"

#include <QIO/FileEntry>
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
    d->permissions = info.permissions();
    d->mimeTypeName = QMimeDatabase().mimeTypeForUrl(d->url).name();

    return FileInfo(d);
}

QStringList FileEntryEngine::list(QDir::Filters filters)
{
    return QDir(url().toLocalFile()).entryList(filters);
}

QList<FileInfo> FileEntryEngine::entryList(QDir::Filters filters)
{
    QList<FileInfo> result;

    auto infos = QDir(url().toLocalFile()).entryInfoList(filters);
    foreach (const auto &info, infos)
        result.append(fromQFileInfo(info));
    return result;
}

bool FileEntryEngine::mkdir(const QString &dirName)
{
    return QDir(url().toLocalFile()).mkdir(dirName);
}

bool FileEntryEngine::rmdir(const QString &dirName)
{
    return QDir().rmdir(FileEntry::absoluteUrl(url(), dirName).toLocalFile());
}

bool FileEntryEngine::remove(const QString &fileName)
{
    return QDir().remove(FileEntry::absoluteUrl(url(), fileName).toLocalFile());
}

bool FileEntryEngine::setPermissions(const QString &fileName, QFileDevice::Permissions permissions)
{
    return QFile::setPermissions(fileName, permissions);
}

FileInfo FileEntryEngine::stat(const QString &fileName)
{
    return fromQFileInfo(QFileInfo(FileEntry::absoluteUrl(url(), fileName).toLocalFile()));
}
