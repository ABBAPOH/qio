#include "archiveentryengine.h"

#include <QIO/FileInfoData>
#include <KF5/KArchive/KZip>
#include <QtCore/QMimeDatabase>

static const KArchiveEntry *getEntry(KArchive *archive, const QString &name)
{
    if (name.isEmpty())
        return archive->directory();
    return archive->directory()->entry(name);
}

static const KArchiveDirectory *getDirectory(KArchive *archive, const QString &name)
{
    return static_cast<const KArchiveDirectory *>(getEntry(archive, name));
}

ArchiveEntryEngine::ArchiveEntryEngine()
{
}

void ArchiveEntryEngine::setUrl(const QUrl &url)
{
    const QUrl oldUrl = this->url();
    AbstractSyncFileEntryEngine::setUrl(url);
    if (url != oldUrl) {
        if (url.scheme() == "zip") {
            const QString path = url.path();
            const int index = path.indexOf(".zip");
            const QString archivePath = path.mid(0, index + 4);
            m_relativePath = path.mid(index + 4);
            m_archive.reset(new KZip(archivePath));
            m_archive->open(QIODevice::ReadOnly);
        }
    }
}

FileInfoList ArchiveEntryEngine::entryList(QDir::Filters filters)
{
    Q_UNUSED(filters);
    FileInfoList result;
    const KArchiveDirectory *dir = getDirectory(m_archive.data(), m_relativePath);
    if (!dir)
        return result;

    foreach (const QString &name, dir->entries()) {
        const KArchiveEntry *entry = dir->entry(name);
        result.append(fromEntry(entry, name));
    }
    return result;
}

FileResult ArchiveEntryEngine::mkdir(const QString &dirName, bool createParents)
{
    Q_UNUSED(dirName);
    Q_UNUSED(createParents);
    Q_UNIMPLEMENTED();
    return FileResult::Error::NotImplemented;
}

FileResult ArchiveEntryEngine::rmdir(const QString &dirName, bool removeEmptyParents)
{
    Q_UNUSED(dirName);
    Q_UNUSED(removeEmptyParents);
    Q_UNIMPLEMENTED();
    return FileResult::Error::NotImplemented;
}

FileResult ArchiveEntryEngine::remove(const QString &fileName)
{
    Q_UNUSED(fileName);
    return FileResult::Error::NotImplemented;
}

FileResult ArchiveEntryEngine::rename(const QString &oldName, const QString &newName)
{
    Q_UNUSED(oldName);
    Q_UNUSED(newName);
    return FileResult::Error::NotImplemented;
}

FileResult ArchiveEntryEngine::setPermissions(const QString &fileName,
                                              QFileDevice::Permissions permissions)
{
    Q_UNUSED(fileName);
    Q_UNUSED(permissions);
    return FileResult::Error::NotImplemented;
}

FileInfo ArchiveEntryEngine::stat(const QString &fileName)
{
    return fromEntry(getEntry(m_archive.data(), m_relativePath + "/" + fileName), fileName);
}

FileInfo ArchiveEntryEngine::fromEntry(const KArchiveEntry *entry, const QString &name)
{
    FileInfoData *d = new FileInfoData();

    QUrl url = this->url();
    url.setPath(url.path() + "/" + name);
    d->url = url;
    d->exists = true;
    d->isDir = entry->isDirectory();
    if (entry->isFile())
        d->size = static_cast<const KArchiveFile *>(entry)->size();
    else
        d->size = 0;
    d->created = entry->date();
    d->lastModified = entry->date();
    d->lastRead = entry->date();
    // TODO: get permissions
//    d->mimeTypeName = QMimeDatabase().mimeTypesForFileName(name).first().name();

    return FileInfo(d);
}
