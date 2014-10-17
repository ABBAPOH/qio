#ifndef SYNCDIRENGINEWRAPPER_H
#define SYNCDIRENGINEWRAPPER_H

#include <QIO/AbstractDirEngine>
#include <QIO/AbstractSyncDirEngine>

class QIO_EXPORT SyncDirEngineWrapper : public AbstractDirEngine
{
public:
    explicit SyncDirEngineWrapper(AbstractSyncDirEngine *engine);

    QFuture<QString> list(QDir::Filters filters) Q_DECL_OVERRIDE;
    QFuture<FileInfo> entryList(QDir::Filters filters) Q_DECL_OVERRIDE;
    QFuture<bool> mkdir(const QString &dirName) Q_DECL_OVERRIDE;
    QFuture<bool> rmdir(const QString &dirName) Q_DECL_OVERRIDE;
    QFuture<bool> remove(const QString &fileName) Q_DECL_OVERRIDE;
    QFuture<bool> setPermissions(const QString &fileName, QFileDevice::Permissions permissions) Q_DECL_OVERRIDE;
    QFuture<FileInfo> stat(const QString &fileName) Q_DECL_OVERRIDE;

private:
    struct SharedState
    {
        SharedState(AbstractSyncDirEngine *engine) : engine(engine) {}

        QMutex mutex;
        QScopedPointer<AbstractSyncDirEngine> engine;
    };
    typedef QSharedPointer<SharedState> SharedStatePointer;

    SharedStatePointer m_state;

    friend struct FileOperation;
};

#endif // SYNCDIRENGINEWRAPPER_H
