#ifndef ABSTRACTDIRENGINE_H
#define ABSTRACTDIRENGINE_H

#include <QIO/FileInfo>
#include <QIO/FileResult>

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

    virtual QFuture<QStringList> list(QDir::Filters filters) = 0;
    virtual QFuture<FileInfoList> entryList(QDir::Filters filters) = 0;
    virtual QFuture<FileResult> mkdir(const QString &dirName, bool createParents) = 0;
    virtual QFuture<FileResult> rmdir(const QString &dirName, bool removeEmptyParents) = 0;
    virtual QFuture<FileResult> remove(const QString &fileName) = 0;
    virtual QFuture<FileResult> rename(const QString &oldName, const QString &newName) = 0;
    virtual QFuture<FileResult> setPermissions(const QString &fileName,
                                         QFileDevice::Permissions permissions) = 0;
    virtual QFuture<FileInfo> stat(const QString &fileName) = 0;

    static AbstractDirEngine *emptyEngine();

private:
    QUrl m_url;
};

#endif // ABSTRACTDIRENGINE_H
