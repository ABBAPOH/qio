#ifndef FILEENTRYENGINE_H
#define FILEENTRYENGINE_H

#include <QIO/AbstractSyncFileEntryEngine>

class FileEntryEngine : public AbstractSyncFileEntryEngine
{
public:
    QList<FileInfo> entryList(QDir::Filters filters) Q_DECL_OVERRIDE;
    FileResult mkdir(const QString &dirName, bool createParents) Q_DECL_OVERRIDE;
    FileResult rmdir(const QString &dirName, bool removeEmptyParents) Q_DECL_OVERRIDE;
    FileResult remove(const QString &fileName) Q_DECL_OVERRIDE;
    FileResult rename(const QString &oldName, const QString &newName) Q_DECL_OVERRIDE;
    FileResult setPermissions(const QString &fileName,
                              QFile::Permissions permissions) Q_DECL_OVERRIDE;
    FileInfo stat(const QString &fileName) Q_DECL_OVERRIDE;
};

#endif // FILEENTRYENGINE_H
