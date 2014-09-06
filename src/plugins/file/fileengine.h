#ifndef FILEENGINE_H
#define FILEENGINE_H

#include <QIO/AbstractFileEngine>

class FileEngine : public AbstractFileEngine
{
public:
    FileEngine();

    QFuture<QString> list() Q_DECL_OVERRIDE;
    QFuture<FileInfo> entryList() Q_DECL_OVERRIDE;
    QFuture<bool> mkdir(const QString &dirName) Q_DECL_OVERRIDE;
    // TODO: separate rmdir?
    QFuture<bool> remove(const QString &fileName) Q_DECL_OVERRIDE;
    QFuture<FileInfo> stat(const QString &fileName) Q_DECL_OVERRIDE;
};

#endif // FILEHANDLER_H
