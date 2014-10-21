#ifndef ARCHIVEENTRYENGINE_H
#define ARCHIVEENTRYENGINE_H

#include <QIO/AbstractSyncFileEntryEngine>
#include <KF5/KArchive/KArchive>

class ArchiveEntryEngine : public AbstractSyncFileEntryEngine
{
public:
    ArchiveEntryEngine();

    void setUrl(const QUrl &url);

    QStringList list(QDir::Filters filters) Q_DECL_OVERRIDE;
    QList<FileInfo> entryList(QDir::Filters filters) Q_DECL_OVERRIDE;
    FileResult mkdir(const QString &dirName, bool createParents) Q_DECL_OVERRIDE;
    FileResult rmdir(const QString &dirName, bool removeEmptyParents) Q_DECL_OVERRIDE;
    FileResult remove(const QString &fileName) Q_DECL_OVERRIDE;
    FileResult rename(const QString &oldName, const QString &newName) Q_DECL_OVERRIDE;
    FileResult setPermissions(const QString &fileName,
                              QFileDevice::Permissions permissions) Q_DECL_OVERRIDE;
    FileInfo stat(const QString &fileName) Q_DECL_OVERRIDE;

private:
    FileInfo fromEntry(const KArchiveEntry *entry, const QString &name);

    QScopedPointer<KArchive> m_archive;
    QString m_relativePath;
};

#endif // ARCHIVEENTRYENGINE_H
