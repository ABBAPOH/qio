#ifndef SYNCDIRENGINEWRAPPER_H
#define SYNCDIRENGINEWRAPPER_H

#include <QIO/AbstractDirEngine>
#include <QIO/AbstractSyncFileEntryEngine>

class QIO_EXPORT SyncDirEngineWrapper : public AbstractDirEngine
{
public:
    explicit SyncDirEngineWrapper(AbstractSyncFileEntryEngine *engine);

    QFuture<QStringList> list(QDir::Filters filters, QDir::SortFlags sortFlags) Q_DECL_OVERRIDE;
    QFuture<FileInfoList> infoList(QDir::Filters filters,
                                   QDir::SortFlags sortFlags) Q_DECL_OVERRIDE;
    QFuture<FileResult> mkdir(const QString &dirName, bool createParents) Q_DECL_OVERRIDE;
    QFuture<FileResult> rmdir(const QString &dirName, bool removeEmptyParents) Q_DECL_OVERRIDE;
    QFuture<FileResult> remove(const QString &fileName) Q_DECL_OVERRIDE;
    QFuture<FileResult> rename(const QString &oldName, const QString &newName) Q_DECL_OVERRIDE;
    QFuture<FileResult> setPermissions(const QString &fileName, QFileDevice::Permissions permissions) Q_DECL_OVERRIDE;
    QFuture<FileInfo> stat(const QString &fileName) Q_DECL_OVERRIDE;

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
