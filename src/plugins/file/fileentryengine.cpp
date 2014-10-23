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

FileInfoList FileEntryEngine::entryList(QDir::Filters filters)
{
    FileInfoList result;

    auto infos = QDir(url().toLocalFile()).entryInfoList(filters);
    foreach (const auto &info, infos)
        result.append(fromQFileInfo(info));
    return result;
}

FileResult FileEntryEngine::mkdir(const QString &dirName, bool createParents)
{
    const QString localPath = FileEntry::absoluteUrl(url(), dirName).toLocalFile();
    const bool ok = createParents ? QDir().mkpath(localPath) : QDir().mkdir(localPath);
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
    const QString oldPath = FileEntry::absoluteUrl(url(), oldName).toLocalFile();
    const QString newPath = oldName.isEmpty()
            ? QFileInfo(oldPath).absolutePath() + "/" + newName
            : FileEntry::absoluteUrl(url(), newName).toLocalFile();
    QFile file(oldPath);
    if (!file.exists())
        return FileResult::Error::NoEntry;
    if (QFileInfo(newPath).exists())
        return FileResult::Error::Exist;
    return file.rename(newPath) ? FileResult() : FileResult::Error::Unknown;
}

FileResult FileEntryEngine::setPermissions(const QString &fileName, QFileDevice::Permissions permissions)
{
    return QFile::setPermissions(fileName, permissions) ? FileResult() : FileResult::Error::Unknown;
}

FileInfo FileEntryEngine::stat(const QString &fileName)
{
    return fromQFileInfo(QFileInfo(FileEntry::absoluteUrl(url(), fileName).toLocalFile()));
}
