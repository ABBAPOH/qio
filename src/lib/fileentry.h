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
    QFuture<bool> rmdir(const QString &fileName = QString());
    QFuture<bool> remove(const QString &fileName = QString());
    QFuture<FileInfo> stat(const QString &fileName = QString());
    QFuture<bool> touch(const QString &fileName = QString());
    QFuture<bool> removeRecursively(const QString &fileName = QString());

    static QUrl absoluteUrl(const QUrl &parentUrl, const QString &relativePath);

protected:
    AbstractDirEngine *engine() const;

private:
    QSharedDataPointer<FileEntryData> d;
};

#endif // FILEENTRY_H
