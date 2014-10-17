#ifndef FILEENTRYENGINE_H
#define FILEENTRYENGINE_H

#include <QIO/AbstractSyncFileEntryEngine>

class FileEntryEngine : public AbstractSyncFileEntryEngine
{
public:
    QStringList list(QDir::Filters filters) Q_DECL_OVERRIDE;
    QList<FileInfo> entryList(QDir::Filters filters) Q_DECL_OVERRIDE;
    bool mkdir(const QString &dirName, bool createParents) Q_DECL_OVERRIDE;
    bool rmdir(const QString &dirName, bool removeEmptyParents) Q_DECL_OVERRIDE;
    bool remove(const QString &fileName) Q_DECL_OVERRIDE;
    bool setPermissions(const QString &fileName, QFile::Permissions permissions) Q_DECL_OVERRIDE;
    FileInfo stat(const QString &fileName) Q_DECL_OVERRIDE;
};

#endif // FILEENTRYENGINE_H
