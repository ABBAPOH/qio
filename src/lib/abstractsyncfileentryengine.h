#ifndef ABSTRACTSYNCFILEENTRYENGINE_H
#define ABSTRACTSYNCFILEENTRYENGINE_H

#include <QIO/FileInfo>
#include <QIO/FileResult>

#include <QtCore/QDir>
#include <QtCore/QUrl>

class QIO_EXPORT AbstractSyncFileEntryEngine
{
public:
    AbstractSyncFileEntryEngine();
    virtual ~AbstractSyncFileEntryEngine();

    QUrl url() const;
    virtual void setUrl(const QUrl &url);

    // TODO: use iterator
    virtual QList<FileInfo> entryList(QDir::Filters filters) = 0;
    virtual FileResult mkdir(const QString &dirName, bool createParents) = 0;
    virtual FileResult rmdir(const QString &dirName, bool removeEmptyParents) = 0;
    virtual FileResult remove(const QString &fileName) = 0;
    virtual FileResult rename(const QString &oldName, const QString &newName) = 0;
    virtual FileResult setPermissions(const QString &fileName, QFile::Permissions permissions) = 0;
    virtual FileInfo stat(const QString &fileName) = 0;

private:
    QUrl m_url;
};

#endif // ABSTRACTSYNCFILEENTRYENGINE_H
