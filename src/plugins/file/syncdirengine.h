#ifndef SYNCDIRENGINE_H
#define SYNCDIRENGINE_H

#include <QIO/AbstractSyncDirEngine>

class SyncDirEngine : public AbstractSyncDirEngine
{
public:
    QStringList list(QDir::Filters filters) Q_DECL_OVERRIDE;
    QList<FileInfo> entryList(QDir::Filters filters) Q_DECL_OVERRIDE;
    bool mkdir(const QString &dirName) Q_DECL_OVERRIDE;
    bool rmdir(const QString &dirName) Q_DECL_OVERRIDE;
    bool remove(const QString &fileName) Q_DECL_OVERRIDE;
    bool setPermissions(const QString &fileName, QFile::Permissions permissions) Q_DECL_OVERRIDE;
    FileInfo stat(const QString &fileName) Q_DECL_OVERRIDE;
};

#endif // SYNCDIRENGINE_H
