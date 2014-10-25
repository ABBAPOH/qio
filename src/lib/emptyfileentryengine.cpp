#include "emptyfileentryengine.h"

EmptyFileEntryEngine::EmptyFileEntryEngine()
{
}

ListJob EmptyFileEntryEngine::list(QDir::Filters filters, QDir::SortFlags sortFlags)
{
    Q_UNUSED(filters);
    Q_UNUSED(sortFlags);
    return QFuture<QStringList>();
}

InfoListJob EmptyFileEntryEngine::infoList(QDir::Filters filters,
                                                     QDir::SortFlags sortFlags)
{
    Q_UNUSED(filters);
    Q_UNUSED(sortFlags);
    return QFuture<FileInfoList>();
}

FileJob EmptyFileEntryEngine::mkdir(const QString &fileName, bool createParents)
{
    Q_UNUSED(fileName);
    Q_UNUSED(createParents);
    return FileJob();
}

FileJob EmptyFileEntryEngine::rmdir(const QString &fileName, bool removeEmptyParents)
{
    Q_UNUSED(fileName);
    Q_UNUSED(removeEmptyParents);
    return FileJob();
}

FileJob EmptyFileEntryEngine::remove(const QString &fileName)
{
    Q_UNUSED(fileName);
    return FileJob();
}

FileJob EmptyFileEntryEngine::rename(const QString &oldName, const QString &newName)
{
    Q_UNUSED(oldName);
    Q_UNUSED(newName);
    return FileJob();
}

FileJob EmptyFileEntryEngine::setPermissions(const QString &fileName,
                                                         QFileDevice::Permissions permissions)
{
    Q_UNUSED(fileName);
    Q_UNUSED(permissions);
    return FileJob();
}

StatJob EmptyFileEntryEngine::stat(const QString &fileName)
{
    Q_UNUSED(fileName);
    return QFuture<FileInfo>();
}
