#ifndef FILEENGINEFALLBACK_H
#define FILEENGINEFALLBACK_H

#include "fileengine.h"

#include <QtCore/QFile>
#include <QtCore/QMutex>

class FileEngineFallback : public FileEngine
{
public:
    FileEngineFallback();

    void setUrl(const QUrl &url) Q_DECL_OVERRIDE;

    QFuture<bool> open(QIODevice::OpenMode mode) Q_DECL_OVERRIDE;
    void close() Q_DECL_OVERRIDE;

private:

    struct ThreadData
    {
        ThreadData() : file(Q_NULLPTR) {}

        QFile *file;
        QMutex mutex;
    } data;
};

#endif // FILEENGINEFALLBACK_H
