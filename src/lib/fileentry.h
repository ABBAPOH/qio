#ifndef FILEENTRY_H
#define FILEENTRY_H

#include "fileinfo.h"

#include <QtCore/QDir>
#include <QtCore/QFuture>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QUrl>

class AbstractDirEngine;

class FileEntryData;
class QIO_EXPORT FileEntry
{
public:
    FileEntry();
    FileEntry(const QUrl &url);
    FileEntry(const FileEntry &);
    FileEntry &operator=(const FileEntry &);
    ~FileEntry();

    QUrl url() const;
    void setUrl(const QUrl &url);

    QFuture<QString> list(QDir::Filters filters = QDir::NoFilter);
    QFuture<FileInfo> entryList(QDir::Filters filters = QDir::NoFilter);
    QFuture<bool> mkdir(const QString &fileName);
    QFuture<bool> rmdir(const QString &fileName);
    QFuture<bool> rmdir(const QUrl &url);

    QFuture<bool> remove(const QString &fileName);
    static QFuture<bool> remove(const QUrl &url);

    QFuture<FileInfo> stat();
    QFuture<FileInfo> stat(const QString &fileName);
    static QFuture<FileInfo> stat(const QUrl &url);

    QFuture<bool> touch(const QString &fileName);
    static QFuture<bool> touch(const QUrl &url);
    static QFuture<bool> removeRecursively(const QUrl &url);

    static QUrl absoluteUrl(const QUrl &parentUrl, const QString &relativePath);

protected:
    AbstractDirEngine *engine() const;

private:
    QSharedDataPointer<FileEntryData> d;
};

#endif // FILEENTRY_H
