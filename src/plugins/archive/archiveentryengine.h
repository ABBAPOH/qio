#ifndef ARCHIVEENTRYENGINE_H
#define ARCHIVEENTRYENGINE_H

#include <QIO/AbstractSyncDirEngine>
#include <KF5/KArchive/KArchive>

class ArchiveEntryEngine : public AbstractSyncDirEngine
{
public:
    ArchiveEntryEngine();

    void setUrl(const QUrl &url);

    QStringList list(QDir::Filters filters) Q_DECL_OVERRIDE;
    QList<FileInfo> entryList(QDir::Filters filters) Q_DECL_OVERRIDE;
    bool mkdir(const QString &fileName) Q_DECL_OVERRIDE;
    bool rmdir(const QString &fileName) Q_DECL_OVERRIDE;
    bool remove(const QString &fileName) Q_DECL_OVERRIDE;
    bool setPermissions(const QString &fileName,
                        QFileDevice::Permissions permissions) Q_DECL_OVERRIDE;
    FileInfo stat(const QString &fileName) Q_DECL_OVERRIDE;

private:
    FileInfo fromEntry(const KArchiveEntry *entry, const QString &name);

    QScopedPointer<KArchive> m_archive;
    QString m_relativePath;
};

#endif // ARCHIVEENTRYENGINE_H
