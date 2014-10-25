#ifndef ABSTRACTDIRENGINE_H
#define ABSTRACTDIRENGINE_H

#include <QIO/FileInfo>
#include <QIO/FileResult>
#include <QIO/Jobs>

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

    virtual ListJob list(QDir::Filters filters, QDir::SortFlags sortFlags) = 0;
    virtual InfoListJob infoList(QDir::Filters filters, QDir::SortFlags sortFlags) = 0;
    virtual FileJob mkdir(const QString &dirName, bool createParents) = 0;
    virtual FileJob rmdir(const QString &dirName, bool removeEmptyParents) = 0;
    virtual FileJob remove(const QString &fileName) = 0;
    virtual FileJob rename(const QString &oldName, const QString &newName) = 0;
    virtual FileJob setPermissions(const QString &fileName,
                                         QFileDevice::Permissions permissions) = 0;
    virtual StatJob stat(const QString &fileName) = 0;

    static AbstractDirEngine *emptyEngine();

private:
    QUrl m_url;
};

#endif // ABSTRACTDIRENGINE_H
