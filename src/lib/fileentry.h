#ifndef FILEENTRY_H
#define FILEENTRY_H

#include <QIO/FileInfo>
#include <QIO/FileResult>
#include <QIO/Jobs>

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

    ListJob list(QDir::Filters filters = QDir::NoFilter, QDir::SortFlags sortFlags = QDir::NoSort);
    InfoListJob infoList(QDir::Filters filters = QDir::NoFilter,
                         QDir::SortFlags sortFlags = QDir::NoSort);

    FileJob mkdir(const QString &fileName = QString());
    FileJob rmdir(const QString &fileName = QString());

    FileJob mkpath(const QString &dirPath = QString());
    FileJob rmpath(const QString &dirPath = QString());

    FileJob touch(const QString &fileName = QString());
    FileJob remove(const QString &fileName = QString());

    FileJob rename(const QString &newName);
    FileJob rename(const QString &oldName, const QString &newName);

    FileJob link(const QString &linkPath);

    StatJob stat(const QString &fileName = QString());

    FileJob setPermissions(const QString &fileName, QFileDevice::Permissions permissions);

    FileJob removeRecursively(const QString &fileName = QString());
    FileJob copy(const QUrl &destUrl);
    FileJob copy(const QString &fileName, const QUrl &destUrl);
    FileJob move(const QUrl &destUrl);
    FileJob move(const QString &fileName, const QUrl &destUrl);

    static QUrl absoluteUrl(const QUrl &parentUrl, const QString &relativePath);

protected:
    AbstractDirEngine *engine() const;

private:
    QSharedDataPointer<FileEntryData> d;
};

#endif // FILEENTRY_H
