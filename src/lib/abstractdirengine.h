#ifndef ABSTRACTDIRENGINE_H
#define ABSTRACTDIRENGINE_H

#include <QIO/FileInfo>

#include <QtCore/QDir>
#include <QtCore/QFuture>
#include <QtCore/QUrl>

class QIO_EXPORT AbstractDirEngine
{
public:
    AbstractDirEngine();
    virtual ~AbstractDirEngine();

    QUrl url() const;
    virtual void setUrl(const QUrl &url);

    virtual QFuture<QString> list(QDir::Filters filters) = 0;
    virtual QFuture<FileInfo> entryList(QDir::Filters filters) = 0;
    virtual QFuture<bool> mkdir(const QString &dirName, bool createParents) = 0;
    virtual QFuture<bool> rmdir(const QString &dirName, bool removeEmptyParents) = 0;
    virtual QFuture<bool> remove(const QString &fileName) = 0;
    virtual QFuture<bool> rename(const QString &oldName, const QString &newName) = 0;
    virtual QFuture<bool> setPermissions(const QString &fileName,
                                         QFileDevice::Permissions permissions) = 0;
    virtual QFuture<FileInfo> stat(const QString &fileName) = 0;

    static AbstractDirEngine *emptyEngine();

private:
    QUrl m_url;
};

#endif // ABSTRACTDIRENGINE_H
