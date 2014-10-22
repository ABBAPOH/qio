#include "emptyfileentryengine.h"

EmptyFileEntryEngine::EmptyFileEntryEngine()
{
}

QFuture<QStringList> EmptyFileEntryEngine::list(QDir::Filters filters)
{
    Q_UNUSED(filters);
    return QFuture<QStringList>();
}

QFuture<FileInfoList> EmptyFileEntryEngine::entryList(QDir::Filters filters)
{
    Q_UNUSED(filters);
    return QFuture<FileInfoList>();
}

QFuture<FileResult> EmptyFileEntryEngine::mkdir(const QString &fileName, bool createParents)
{
    Q_UNUSED(fileName);
    Q_UNUSED(createParents);
    return QFuture<FileResult>();
}

QFuture<FileResult> EmptyFileEntryEngine::rmdir(const QString &fileName, bool removeEmptyParents)
{
    Q_UNUSED(fileName);
    Q_UNUSED(removeEmptyParents);
    return QFuture<FileResult>();
}

QFuture<FileResult> EmptyFileEntryEngine::remove(const QString &fileName)
{
    Q_UNUSED(fileName);
    return QFuture<FileResult>();
}

QFuture<FileResult> EmptyFileEntryEngine::rename(const QString &oldName, const QString &newName)
{
    Q_UNUSED(oldName);
    Q_UNUSED(newName);
    return QFuture<FileResult>();
}

QFuture<FileResult> EmptyFileEntryEngine::setPermissions(const QString &fileName,
                                             QFileDevice::Permissions permissions)
{
    Q_UNUSED(fileName);
    Q_UNUSED(permissions);
    return QFuture<FileResult>();
}

QFuture<FileInfo> EmptyFileEntryEngine::stat(const QString &fileName)
{
    Q_UNUSED(fileName);
    return QFuture<FileInfo>();
}
