#ifndef FILERESULT_H
#define FILERESULT_H

#include <QtCore/QCoreApplication>

class FileResult
{
    Q_DECLARE_TR_FUNCTIONS(FileResult)
    typedef bool (FileResult::*RestrictedBool)() const;
public:
    enum class Error {
        Unknown = -1,
        None = 0,
        NoEntry,
        NoAccess,
        Exist,
        NotImplemented
    };

    inline Q_DECL_CONSTEXPR FileResult() : m_error(Error::None) {}
    inline Q_DECL_CONSTEXPR FileResult(Error error) : m_error(error) {}

    inline Q_DECL_CONSTEXPR bool hasError() const { return m_error != Error::None; }
    inline Error Q_DECL_CONSTEXPR error() const { return m_error; }
    inline QString errorString() const { return errorToString(m_error); }

    inline void swap(FileResult &other) { qSwap(m_error, other.m_error); }

    inline Q_DECL_CONSTEXPR operator RestrictedBool() const
    {
        return hasError() ? Q_NULLPTR : &FileResult::hasError;
    }
    inline bool Q_DECL_CONSTEXPR operator !() const { return hasError(); }

private:
    static QString errorToString(Error error);

    Error m_error;
};

inline bool operator ==(FileResult lhs, FileResult rhs) { return lhs.error() == rhs.error(); }
inline bool operator !=(FileResult lhs, FileResult rhs) { return lhs.error() != rhs.error(); }

#endif // FILERESULT_H
