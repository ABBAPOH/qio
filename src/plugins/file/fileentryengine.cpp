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

FileResult FileEntryEngine::mkdir(const QString &dirName, bool createParents)
{
    QDir dir(url().toLocalFile());
    if (!dir.exists())
        return FileResult::Error::NoEntry;

    const bool ok = createParents ? dir.mkpath(dirName) : dir.mkdir(dirName);
    if (!ok)
        return FileResult::Error::Unknown;
    return FileResult();
}

FileResult FileEntryEngine::rmdir(const QString &dirName, bool removeEmptyParents)
{
    const QString localPath = FileEntry::absoluteUrl(url(), dirName).toLocalFile();
    const bool ok = removeEmptyParents ? QDir().rmpath(localPath) : QDir().rmdir(localPath);
    if (!ok)
        return FileResult::Error::Unknown;
    return FileResult();
}

FileResult FileEntryEngine::remove(const QString &fileName)
{
    const bool ok = QDir().remove(FileEntry::absoluteUrl(url(), fileName).toLocalFile());
    return ok ? FileResult() : FileResult::Error::Unknown;
}

FileResult FileEntryEngine::rename(const QString &oldName, const QString &newName)
{
    const QUrl oldUrl = FileEntry::absoluteUrl(url(), oldName);
    const QUrl newUrl = FileEntry::absoluteUrl(url(), newName);
    bool ok = QFile::rename(oldUrl.toLocalFile(), newUrl.toLocalFile());
    return ok ? FileResult() : FileResult::Error::Unknown;
}

FileResult FileEntryEngine::setPermissions(const QString &fileName, QFileDevice::Permissions permissions)
{
    return QFile::setPermissions(fileName, permissions) ? FileResult() : FileResult::Error::Unknown;
}

FileInfo FileEntryEngine::stat(const QString &fileName)
{
    return fromQFileInfo(QFileInfo(FileEntry::absoluteUrl(url(), fileName).toLocalFile()));
}
