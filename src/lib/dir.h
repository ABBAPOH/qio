#ifndef DIR_H
#define DIR_H

#include "fileinfo.h"

#include <QtCore/QDir>
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

    QFuture<QString> list(QDir::Filters filters = QDir::NoFilter);
    QFuture<FileInfo> entryList(QDir::Filters filters = QDir::NoFilter);
    QFuture<bool> mkdir(const QString &fileName);
    QFuture<bool> remove(const QString &fileName);
    QFuture<FileInfo> stat();
    QFuture<FileInfo> stat(const QString &fileName);

    static QFuture<bool> touch(const QUrl &url);
    static QFuture<bool> removeRecursively(const QUrl &url);

private:
    QSharedDataPointer<DirData> d;
};

#endif // DIR_H
