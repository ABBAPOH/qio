#ifndef FILEENGINE_H
#define FILEENGINE_H

#include <QIO/AbstractDirEngine>

class DirEngine : public AbstractDirEngine
{
public:
    DirEngine();

    QFuture<QString> list(QDir::Filters filters) Q_DECL_OVERRIDE;
    QFuture<FileInfo> entryList(QDir::Filters filters) Q_DECL_OVERRIDE;
    QFuture<bool> mkdir(const QString &dirName) Q_DECL_OVERRIDE;
    QFuture<bool> rmdir(const QString &dirName) Q_DECL_OVERRIDE;
    QFuture<bool> remove(const QString &fileName) Q_DECL_OVERRIDE;
    QFuture<FileInfo> stat(const QString &fileName) Q_DECL_OVERRIDE;
};

#endif // FILEHANDLER_H
