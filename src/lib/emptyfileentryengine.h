#ifndef EMPTYFILEENTRYENGINE_H
#define EMPTYFILEENTRYENGINE_H

#include <QIO/AbstractDirEngine>

class QIO_EXPORT EmptyFileEntryEngine : public AbstractDirEngine
{
public:
    EmptyFileEntryEngine();

    QFuture<QString> list(QDir::Filters filters) Q_DECL_OVERRIDE;
    QFuture<FileInfo> entryList(QDir::Filters filters) Q_DECL_OVERRIDE;
    QFuture<bool> mkdir(const QString &fileName) Q_DECL_OVERRIDE;
    QFuture<bool> rmdir(const QString &fileName) Q_DECL_OVERRIDE;
    QFuture<bool> remove(const QString &fileName) Q_DECL_OVERRIDE;
    QFuture<bool> setPermissions(const QString &fileName,
                                 QFileDevice::Permissions permissions) Q_DECL_OVERRIDE;
    QFuture<FileInfo> stat(const QString &fileName) Q_DECL_OVERRIDE;
};


#endif // EMPTYFILEENTRYENGINE_H
