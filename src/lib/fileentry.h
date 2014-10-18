#ifndef FILEENTRY_H
#define FILEENTRY_H

#include <QIO/FileInfo>
#include <QIO/FileResult>

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

    QFuture<FileResult> mkdir(const QString &fileName);
    QFuture<FileResult> rmdir(const QString &fileName = QString());

    QFuture<FileResult> mkpath(const QString &dirPath = QString());
    QFuture<FileResult> rmpath(const QString &dirPath = QString());

    QFuture<FileResult> touch(const QString &fileName = QString());
    QFuture<FileResult> remove(const QString &fileName = QString());

    QFuture<FileResult> rename(const QString &newName);
    QFuture<FileResult> rename(const QString &oldName, const QString &newName);

    QFuture<FileResult> link(const QString &linkPath);

    QFuture<FileInfo> stat(const QString &fileName = QString());

    QFuture<FileResult> setPermissions(const QString &fileName, QFileDevice::Permissions permissions);

    QFuture<bool> removeRecursively(const QString &fileName = QString());

    static QUrl absoluteUrl(const QUrl &parentUrl, const QString &relativePath);

protected:
    AbstractDirEngine *engine() const;

private:
    QSharedDataPointer<FileEntryData> d;
};

#endif // FILEENTRY_H
