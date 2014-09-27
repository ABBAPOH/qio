#ifndef RUNNER_H
#define RUNNER_H

#include <QtCore/QObject>

class QRunnable;
class QThreadPool;

class QIO_EXPORT Runner : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Runner)

    Q_PROPERTY(int expiryTimeout READ expiryTimeout WRITE setExpiryTimeout)
public:
    explicit Runner(QObject *parent = 0);

public:
    int expiryTimeout() const;
    void setExpiryTimeout(int arg);

    void start(QRunnable *runnable, int priority = 0);
    bool tryStart(QRunnable *runnable);
    void waitForDone(int msecs = -1);

private:
    QThreadPool *pool;
};

#endif // RUNNER_H
