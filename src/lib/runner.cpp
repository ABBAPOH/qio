#include "runner.h"

#include <QtCore/QThreadPool>

Runner::Runner(QObject *parent) :
    QObject(parent),
    pool(new QThreadPool(this))
{
    pool->setMaxThreadCount(1);
}

int Runner::expiryTimeout() const
{
    return pool->expiryTimeout();
}

void Runner::setExpiryTimeout(int timeout)
{
    pool->setExpiryTimeout(timeout);
}

bool Runner::tryStart(QRunnable *runnable)
{
    return pool->tryStart(runnable);
}

void Runner::start(QRunnable *runnable, int priority)
{
    pool->start(runnable, priority);
}

void Runner::waitForDone(int msecs)
{
    pool->waitForDone(msecs);
}
