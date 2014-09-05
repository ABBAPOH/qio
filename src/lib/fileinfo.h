#ifndef FILEINFO_H
#define FILEINFO_H

#include <QtCore/QDateTime>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QUrl>

class FileInfoData;

class QIO_EXPORT FileInfo
{
public:
    FileInfo();
    FileInfo(const FileInfo &);
    FileInfo(FileInfoData *dd);
    FileInfo &operator=(const FileInfo &);
    ~FileInfo();

    QUrl url() const;

    bool exists() const;
    bool isDir() const;

    QString fileName() const;

    qint64 size() const;

    QDateTime created() const;
    QDateTime lastModified() const;
    QDateTime lastRead() const;

    QString mimeTypeName() const;

private:
    QSharedDataPointer<FileInfoData> d;
};

#endif // FILEINFO_H
