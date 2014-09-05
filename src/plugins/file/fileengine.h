#ifndef FILEENGINE_H
#define FILEENGINE_H

#include <QIO/AbstractFileEngine>

class FileEngine : public AbstractFileEngine
{
public:
    FileEngine();

    QFuture<bool> open(QIODevice::OpenMode mode) Q_DECL_OVERRIDE;
    void close() Q_DECL_OVERRIDE;

    bool seek(qint64 pos) Q_DECL_OVERRIDE;

    bool waitForBytesWritten(int msecs = -1) Q_DECL_OVERRIDE;
    bool waitForReadyRead(int msecs = -1) Q_DECL_OVERRIDE;

    QFuture<QString> list() Q_DECL_OVERRIDE;
    QFuture<FileInfo> entryList() Q_DECL_OVERRIDE;
    QFuture<bool> mkdir(const QString &dirName) Q_DECL_OVERRIDE;
    // TODO: separate rmdir?
    QFuture<bool> remove(const QString &fileName) Q_DECL_OVERRIDE;
    QFuture<FileInfo> stat(const QString &fileName) Q_DECL_OVERRIDE;
};

#endif // FILEHANDLER_H
