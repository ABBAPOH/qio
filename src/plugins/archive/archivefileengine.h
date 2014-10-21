#ifndef ARCHIVEFILEENGINE_H
#define ARCHIVEFILEENGINE_H

#include <QIO/AbstractFileEngine>
#include <KF5/KArchive/KArchive>
#include <QtCore/QBuffer>

class ArchiveFileEngine : public AbstractFileEngine
{
    Q_OBJECT
public:
    explicit ArchiveFileEngine(QObject *parent = 0);

    void open(QIODevice::OpenMode m_mode);
    bool waitForOpened(int msecs);
    void close();
    bool seek(qint64 pos);
    qint64 size() const;
    void read(qint64 maxlen);
    void write(const QByteArray &data);
    bool waitForBytesWritten(int msecs);
    bool waitForReadyRead(int msecs);

private slots:
    void onOpenFinished();

private:
    struct SharedState {
        QString relativePath;
        QScopedPointer<KArchive> archive;
        QMutex mutex;
        QByteArray data;
    } m_state;

    QFutureWatcher<bool> *openWatcher;
    QIODevice::OpenMode m_mode;
    QBuffer m_buffer;
};

#endif // ARCHIVEFILEENGINE_H
