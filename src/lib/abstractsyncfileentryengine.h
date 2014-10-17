#ifndef ABSTRACTSYNCFILEENTRYENGINE_H
#define ABSTRACTSYNCFILEENTRYENGINE_H

#include "fileinfo.h"

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
    virtual QStringList list(QDir::Filters filters) = 0;
    virtual QList<FileInfo> entryList(QDir::Filters filters) = 0;
    virtual bool mkdir(const QString &fileName) = 0;
    virtual bool rmdir(const QString &fileName) = 0;
    virtual bool remove(const QString &fileName) = 0;
    virtual bool setPermissions(const QString &fileName, QFile::Permissions permissions) = 0;
    virtual FileInfo stat(const QString &fileName) = 0;

private:
    QUrl m_url;
};

#endif // ABSTRACTSYNCFILEENTRYENGINE_H
