#include "emptyfileentryengine.h"

EmptyFileEntryEngine::EmptyFileEntryEngine()
{
}

QFuture<QString> EmptyFileEntryEngine::list(QDir::Filters filters)
{
    Q_UNUSED(filters);
    return QFuture<QString>();
}

QFuture<FileInfo> EmptyFileEntryEngine::entryList(QDir::Filters filters)
{
    Q_UNUSED(filters);
    return QFuture<FileInfo>();
}

QFuture<bool> EmptyFileEntryEngine::mkdir(const QString &fileName, bool createParents)
{
    Q_UNUSED(fileName);
    return QFuture<bool>();
}

QFuture<bool> EmptyFileEntryEngine::rmdir(const QString &fileName, bool removeEmptyParents)
{
    Q_UNUSED(fileName);
    return QFuture<bool>();
}

QFuture<bool> EmptyFileEntryEngine::remove(const QString &fileName)
{
    Q_UNUSED(fileName);
    return QFuture<bool>();
}

QFuture<bool> EmptyFileEntryEngine::rename(const QString &oldName, const QString &newName)
{
    Q_UNUSED(oldName);
    Q_UNUSED(newName);
    return QFuture<bool>();
}

QFuture<bool> EmptyFileEntryEngine::setPermissions(const QString &fileName,
                                             QFileDevice::Permissions permissions)
{
    Q_UNUSED(fileName);
    Q_UNUSED(permissions);
    return QFuture<bool>();
}

QFuture<FileInfo> EmptyFileEntryEngine::stat(const QString &fileName)
{
    Q_UNUSED(fileName);
    return QFuture<FileInfo>();
}
