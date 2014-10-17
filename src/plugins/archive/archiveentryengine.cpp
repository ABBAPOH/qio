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

QStringList ArchiveEntryEngine::list(QDir::Filters filters)
{
    const KArchiveDirectory *dir = getDirectory(m_archive.data(), m_relativePath);
    if (!dir)
        return QStringList();
    return dir->entries();
}

QList<FileInfo> ArchiveEntryEngine::entryList(QDir::Filters filters)
{
    QList<FileInfo> result;
    const KArchiveDirectory *dir = getDirectory(m_archive.data(), m_relativePath);
    if (!dir)
        return result;

    foreach (const QString &name, list(filters)) {
        const KArchiveEntry *entry = dir->entry(name);
        result.append(fromEntry(entry, name));
    }
    return result;
}

bool ArchiveEntryEngine::mkdir(const QString &fileName, bool createParents)
{
    return false;
}

bool ArchiveEntryEngine::rmdir(const QString &fileName, bool removeEmptyParents)
{
    return false;
}

bool ArchiveEntryEngine::remove(const QString &fileName)
{
    return false;
}

bool ArchiveEntryEngine::setPermissions(const QString &fileName,
                                        QFileDevice::Permissions permissions)
{
    Q_UNUSED(fileName);
    Q_UNUSED(permissions);
    return false;
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
