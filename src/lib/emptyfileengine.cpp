#include "emptyfileengine.h"

void EmptyFileEngine::open(QIODevice::OpenMode mode)
{
    Q_UNUSED(mode);
}

bool EmptyFileEngine::waitForOpened(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}

void EmptyFileEngine::close()
{

}

bool EmptyFileEngine::seek(qint64 pos)
{
    Q_UNUSED(pos);
    return false;
}

qint64 EmptyFileEngine::size() const
{
    return -1;
}

void EmptyFileEngine::read(qint64 maxlen)
{
    Q_UNUSED(maxlen);
}

void EmptyFileEngine::write(const QByteArray &data)
{
    Q_UNUSED(data);
}

bool EmptyFileEngine::waitForBytesWritten(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}

bool EmptyFileEngine::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}
