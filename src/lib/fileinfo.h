#ifndef FILEINFO_H
#define FILEINFO_H

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
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

inline bool operator ==(const FileInfo &lhs, const FileInfo &rhs)
{
    return lhs.url() == rhs.url();
}

inline uint qHash(const FileInfo &info, uint seed)
{
    return qHash(info.url(), seed);
}

inline QDebug operator<<(QDebug debug, const FileInfo &info)
{
    debug << QString("FileInfo(%1)").arg(info.url().toString());
    return debug;
}

#endif // FILEINFO_H
