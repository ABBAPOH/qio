#ifndef DIR_H
#define DIR_H

#include "fileinfo.h"

#include <QtCore/QFuture>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QUrl>

class DirData;

class QIO_EXPORT Dir
{
public:
    Dir();
    Dir(const QUrl &url);
    Dir(const Dir &);
    Dir &operator=(const Dir &);
    ~Dir();

    QFuture<QString> list();
    QFuture<FileInfo> entryList();
    QFuture<bool> mkdir(const QString &fileName);
    QFuture<bool> remove(const QString &fileName);
    QFuture<FileInfo> stat();
    QFuture<FileInfo> stat(const QString &fileName);

private:
    QSharedDataPointer<DirData> d;
};

#endif // DIR_H
