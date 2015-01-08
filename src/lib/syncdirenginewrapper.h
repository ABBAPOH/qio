#ifndef SYNCDIRENGINEWRAPPER_H
#define SYNCDIRENGINEWRAPPER_H

#include <QIO/AbstractDirEngine>
#include <QIO/AbstractSyncFileEntryEngine>

#include <QtCore/QSharedPointer>

class QIO_EXPORT SyncDirEngineWrapper : public AbstractDirEngine
{
public:
    explicit SyncDirEngineWrapper(AbstractSyncFileEntryEngine *engine);

    ListJob list(QDir::Filters filters, QDir::SortFlags sortFlags) Q_DECL_OVERRIDE;
    InfoListJob infoList(QDir::Filters filters, QDir::SortFlags sortFlags) Q_DECL_OVERRIDE;
    FileJob mkdir(const QString &dirName, bool createParents) Q_DECL_OVERRIDE;
    FileJob rmdir(const QString &dirName, bool removeEmptyParents) Q_DECL_OVERRIDE;
    FileJob remove(const QString &fileName) Q_DECL_OVERRIDE;
    FileJob rename(const QString &oldName, const QString &newName) Q_DECL_OVERRIDE;
    FileJob setPermissions(const QString &fileName, QFileDevice::Permissions permissions) Q_DECL_OVERRIDE;
    StatJob stat(const QString &fileName) Q_DECL_OVERRIDE;

private:
    struct SharedState
    {
        SharedState(AbstractSyncFileEntryEngine *engine) : engine(engine) {}

        QMutex mutex;
        QScopedPointer<AbstractSyncFileEntryEngine> engine;
    };
    typedef QSharedPointer<SharedState> SharedStatePointer;

    SharedStatePointer m_state;

    friend struct FileOperation;
};

#endif // SYNCDIRENGINEWRAPPER_H
