#ifndef EMPTYFILEENGINE_H
#define EMPTYFILEENGINE_H

#include <QIO/AbstractFileEngine>

class QIO_EXPORT EmptyFileEngine : public AbstractFileEngine
{
    Q_OBJECT
    Q_DISABLE_COPY(EmptyFileEngine)
public:
    inline EmptyFileEngine() {}

    void open(QIODevice::OpenMode mode) Q_DECL_OVERRIDE;
    bool waitForOpened(int msecs) Q_DECL_OVERRIDE;
    void close() Q_DECL_OVERRIDE;

    bool seek(qint64 pos) Q_DECL_OVERRIDE;
    qint64 size() const  Q_DECL_OVERRIDE;

    void read(qint64 maxlen) Q_DECL_OVERRIDE;
    void write(const QByteArray &data) Q_DECL_OVERRIDE;

    bool waitForBytesWritten(int msecs = -1) Q_DECL_OVERRIDE;
    bool waitForReadyRead(int msecs = -1) Q_DECL_OVERRIDE;
};

#endif // EMPTYFILEENGINE_H
