#ifndef FILE_P_H
#define FILE_P_H

#include "file.h"

#include "abstractfileengine.h"

class FilePrivate
{
    Q_DECLARE_PUBLIC(File)
    File *q_ptr;

public:
    explicit FilePrivate(File *qq) : q_ptr(qq) {}
    void init();

    void openFinished(bool ok, qint64 size);

    AbstractFileEngine *engine;
    QUrl url;

    File::State state;
    QIODevice::OpenMode openMode;
    qint64 size;
};

#endif // FILE_P_H
