#ifndef EMPTYFILEENTRYENGINE_H
#define EMPTYFILEENTRYENGINE_H

#include <QIO/AbstractDirEngine>

class QIO_EXPORT EmptyFileEntryEngine : public AbstractDirEngine
{
public:
    EmptyFileEntryEngine();

    ListJob list(QDir::Filters filters, QDir::SortFlags sortFlags) Q_DECL_OVERRIDE;
    InfoListJob infoList(QDir::Filters filters, QDir::SortFlags sortFlags) Q_DECL_OVERRIDE;
    FileJob mkdir(const QString &fileName, bool createParents) Q_DECL_OVERRIDE;
    FileJob rmdir(const QString &fileName, bool removeEmptyParents) Q_DECL_OVERRIDE;
    FileJob remove(const QString &fileName) Q_DECL_OVERRIDE;
    FileJob rename(const QString &oldName, const QString &newName) Q_DECL_OVERRIDE;
    FileJob setPermissions(const QString &fileName,
                                       QFileDevice::Permissions permissions) Q_DECL_OVERRIDE;
    StatJob stat(const QString &fileName) Q_DECL_OVERRIDE;
};

#endif // EMPTYFILEENTRYENGINE_H
