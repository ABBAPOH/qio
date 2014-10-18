#ifndef FILERESULT_H
#define FILERESULT_H

#include <QtCore/QCoreApplication>

class FileResult
{
    Q_DECLARE_TR_FUNCTIONS(FileResult)
    typedef bool (FileResult::*RestrictedBool)() const;
public:
    enum class Error { Unknown = -1, None = 0, NoEntry = 2 };

    FileResult();
    explicit FileResult(Error error);

    bool hasError() const;
    Error error() const;
    QString errorString() const;

    void swap(FileResult &other);

    inline operator RestrictedBool() const
    {
        return hasError() ? Q_NULLPTR : &FileResult::hasError;
    }
    inline bool operator !() const { return hasError(); }

private:
    static QString errorToString(Error error);

    Error m_error;
};

inline bool operator ==(FileResult lhs, FileResult rhs) { return lhs.error() == rhs.error(); }
inline bool operator !=(FileResult lhs, FileResult rhs) { return lhs.error() != rhs.error(); }

#endif // FILERESULT_H
