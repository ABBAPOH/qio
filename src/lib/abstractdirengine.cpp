#include "abstractdirengine.h"

AbstractDirEngine::AbstractDirEngine()
{
}

AbstractDirEngine::~AbstractDirEngine()
{
}

QUrl AbstractDirEngine::url() const
{
    return m_url;
}

void AbstractDirEngine::setUrl(const QUrl &url)
{
    m_url = url;
}

class EmptyDirEngine : public AbstractDirEngine
{
public:
    inline EmptyDirEngine() {}

    QFuture<QString> list(QDir::Filters filters) Q_DECL_OVERRIDE;
    QFuture<FileInfo> entryList(QDir::Filters filters) Q_DECL_OVERRIDE;
    QFuture<bool> mkdir(const QString &fileName) Q_DECL_OVERRIDE;
    QFuture<bool> rmdir(const QString &fileName) Q_DECL_OVERRIDE;
    QFuture<bool> remove(const QString &fileName) Q_DECL_OVERRIDE;
    QFuture<bool> setPermissions(const QString &fileName,
                                 QFileDevice::Permissions permissions) Q_DECL_OVERRIDE;
    QFuture<FileInfo> stat(const QString &fileName) Q_DECL_OVERRIDE;
};

QFuture<QString> EmptyDirEngine::list(QDir::Filters filters)
{
    Q_UNUSED(filters);
    return QFuture<QString>();
}

QFuture<FileInfo> EmptyDirEngine::entryList(QDir::Filters filters)
{
    Q_UNUSED(filters);
    return QFuture<FileInfo>();
}

QFuture<bool> EmptyDirEngine::mkdir(const QString &fileName)
{
    Q_UNUSED(fileName);
    return QFuture<bool>();
}

QFuture<bool> EmptyDirEngine::rmdir(const QString &fileName)
{
    Q_UNUSED(fileName);
    return QFuture<bool>();
}

QFuture<bool> EmptyDirEngine::remove(const QString &fileName)
{
    Q_UNUSED(fileName);
    return QFuture<bool>();
}

QFuture<bool> EmptyDirEngine::setPermissions(const QString &fileName,
                                             QFileDevice::Permissions permissions)
{
    Q_UNUSED(fileName);
    Q_UNUSED(permissions);
    return QFuture<bool>();
}

QFuture<FileInfo> EmptyDirEngine::stat(const QString &fileName)
{
    Q_UNUSED(fileName);
    return QFuture<FileInfo>();
}

Q_GLOBAL_STATIC(EmptyDirEngine, empty_engine)

AbstractDirEngine *AbstractDirEngine::emptyEngine()
{
    return empty_engine();
}
