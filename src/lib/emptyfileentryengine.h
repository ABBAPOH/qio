#ifndef EMPTYFILEENTRYENGINE_H
#define EMPTYFILEENTRYENGINE_H

#include <QIO/AbstractDirEngine>

class QIO_EXPORT EmptyFileEntryEngine : public AbstractDirEngine
{
public:
    EmptyFileEntryEngine();

    QFuture<QStringList> list(QDir::Filters filters, QDir::SortFlags sortFlags) Q_DECL_OVERRIDE;
    QFuture<FileInfoList> infoList(QDir::Filters filters,
                                   QDir::SortFlags sortFlags) Q_DECL_OVERRIDE;
    QFuture<FileResult> mkdir(const QString &fileName, bool createParents) Q_DECL_OVERRIDE;
    QFuture<FileResult> rmdir(const QString &fileName, bool removeEmptyParents) Q_DECL_OVERRIDE;
    QFuture<FileResult> remove(const QString &fileName) Q_DECL_OVERRIDE;
    QFuture<FileResult> rename(const QString &oldName, const QString &newName) Q_DECL_OVERRIDE;
    QFuture<FileResult> setPermissions(const QString &fileName,
                                 QFileDevice::Permissions permissions) Q_DECL_OVERRIDE;
    QFuture<FileInfo> stat(const QString &fileName) Q_DECL_OVERRIDE;
};


#endif // EMPTYFILEENTRYENGINE_H
