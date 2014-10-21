#include "archivefileengine.h"

#include <QIO/RunExtensions>
#include <KF5/KArchive/KZip>
#include <QtCore/QFutureWatcher>

ArchiveFileEngine::ArchiveFileEngine(QObject *parent) :
    AbstractFileEngine(parent),
    openWatcher(new QFutureWatcher<bool>(this)),
    m_mode(QIODevice::NotOpen)
{
    connect(openWatcher, &QFutureWatcherBase::finished, this, &ArchiveFileEngine::onOpenFinished);
}

void ArchiveFileEngine::open(QIODevice::OpenMode mode)
{
    typedef void (*func)(QFutureInterface<bool> &, SharedState *, QIODevice::OpenMode, QUrl);
    func f = [](QFutureInterface<bool> &future, SharedState *data, QIODevice::OpenMode mode, QUrl url) {
        if (url.scheme() == "zip") {
            const QString path = url.path();
            const int index = path.indexOf(".zip");
            const QString archivePath = path.mid(0, index + 4);
            QMutexLocker l(&data->mutex);
            data->relativePath = path.mid(index + 4);
            data->archive.reset(new KZip(archivePath));
            bool ok = data->archive->open(mode);
            if (ok) {
                const KArchiveFile *file = data->archive->directory()->file(data->relativePath);
                if (!file) {
                    future.reportResult(false);
                    return;
                }
                data->data = file->data();
            }
            future.reportResult(ok);
        } else {
            future.reportResult(false);
        }
    };
    m_mode = mode;
    openWatcher->setFuture(QtConcurrent::run(f, &m_state, mode, url()));
}

bool ArchiveFileEngine::waitForOpened(int msecs)
{
    Q_UNUSED(msecs);
    auto future = openWatcher->future();
    if (future == QFuture<bool>())
        return false;
    future.waitForFinished();
    onOpenFinished();
    return true;
}

void ArchiveFileEngine::close()
{
    m_mode = QIODevice::NotOpen;
    m_buffer.close();
    m_buffer.setData(QByteArray());
}

bool ArchiveFileEngine::seek(qint64 pos)
{
    Q_UNUSED(pos);
    Q_UNIMPLEMENTED();
    return false;
}

qint64 ArchiveFileEngine::size() const
{
    Q_UNIMPLEMENTED();
    return 0;
}

void ArchiveFileEngine::read(qint64 maxlen)
{
    Q_UNUSED(maxlen);
    Q_UNIMPLEMENTED();
    const QByteArray data = m_buffer.read(maxlen);
    // TODO: this leads to a recursion
    readFinished(data.data(), data.size());
}

void ArchiveFileEngine::write(const QByteArray &data)
{
    Q_UNUSED(data);
    Q_UNIMPLEMENTED();
}

bool ArchiveFileEngine::waitForBytesWritten(int msecs)
{
    Q_UNUSED(msecs);
    Q_UNIMPLEMENTED();
    return false;
}

bool ArchiveFileEngine::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs);
    Q_UNIMPLEMENTED();
    return true;
}

void ArchiveFileEngine::onOpenFinished()
{
    auto future = openWatcher->future();
    if (future == QFuture<bool>())
        return;
    const bool ok = (future.result());
    if (ok) {
        QMutexLocker l(&m_state.mutex);
        m_buffer.setData(m_state.data);
        m_buffer.open(m_mode);
    }
    openFinished(ok);
    openWatcher->setFuture(QFuture<bool>());
}
