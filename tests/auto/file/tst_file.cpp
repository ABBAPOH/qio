#include <QtTest/QtTest>

#include <QIO/File>

#include <QtCore/QFile>
#include <QtCore/QTemporaryDir>

static bool copyFile(const QString &from, const QString &to)
{
    bool ok = QFile::copy(from, to);
    if (ok) {
        QFile file(to);
        ok = file.setPermissions(file.permissions() | QFileDevice::WriteOwner);
    }
    return ok;
}

class tst_File : public QObject
{
    Q_OBJECT
public:
    tst_File();

private slots:
    void initTestCase();
    void open();
    void read_data();
    void read();
    void seek_data();
    void seek();
    void readBench_data();
    void readBench();
    void readQFileBench();

private:
    QTemporaryDir dir;

    QString filePath;
    QString notExistPath;
    QStringList files;
    QStringList paths;
};

tst_File::tst_File()
{
}

void tst_File::initTestCase()
{
//    files << "small.txt" << freedesktop.org.xml;
//    filePath = dir.path() + "/" + "freedesktop.org.xml";
//    notExistPath = dir.path() + "/" + "notexisr.txt";
//    QFile in(":/freedesktop.org.xml");
//    QVERIFY(in.open(QIODevice::ReadOnly));
//    QFile out(filePath);
//    QVERIFY(out.open(QIODevice::WriteOnly));

//    while (!in.atEnd()) {
//        const QByteArray data = in.read(4*1024);
//        qint64 written = out.write(data);
//        QVERIFY(written == data.size());
//    }
}

void tst_File::open()
{
    const QString path = dir.path() + "/" + "tst_open.txt";
    QVERIFY(copyFile(":/small.txt", path));
    {
        File f(QUrl::fromLocalFile(path));
        QVERIFY(f.open(QIODevice::ReadOnly));
        f.close();
    }

    {
        File f(QUrl::fromLocalFile("notexist.txt"));
        QVERIFY(!f.open(QIODevice::ReadOnly));
    }

    QVERIFY(QFile::remove(path));
}

void tst_File::read_data()
{
    QTest::addColumn<QString>("fileName");

    QTest::newRow("1") << QString("small.txt");
    QTest::newRow("2") << QString("medium.txt");
    QTest::newRow("3") << QString("freedesktop.org.xml");
}

void tst_File::read()
{
    QFETCH(QString, fileName);
    const QString path = dir.path() + "/" + fileName;
    QVERIFY(copyFile(":/" + fileName, path));
    QFile qFile(path);
    File file(QUrl::fromLocalFile(path));
    QVERIFY(qFile.open(QIODevice::ReadOnly));
    QVERIFY(file.open(QIODevice::ReadOnly));

    while (!file.atEnd()) {
        QVERIFY(file.waitForReadyRead());
        qint64 bytes = file.bytesAvailable();
        const QByteArray data1 = file.read(bytes);
        const QByteArray data2 = qFile.read(data1.size());
        if (data1 != data2) {
            qDebug() << "data1" << data1.left(100);
            qDebug() << "data2" << data2.left(100);
        }
        QCOMPARE(file.pos(), qFile.pos());
        QVERIFY(data1 == data2);
        QVERIFY(file.pos() == qFile.pos());
    }

    QVERIFY(qFile.atEnd());
    qFile.remove();
}

void tst_File::seek_data()
{
    typedef QList<qint64> PosList;
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<PosList>("offsets");

    QTest::newRow("1") << QString("small.txt") << (PosList() << 0);
    QTest::newRow("2") << QString("small.txt") << (PosList() << 100);
    QTest::newRow("3") << QString("small.txt") << (PosList() << 100 << 200 << 300);
    QTest::newRow("4") << QString("small.txt") << (PosList() << 300 << 100 << 200);
    QTest::newRow("5") << QString("small.txt") << (PosList() << 1958); // at end
    QTest::newRow("6") << QString("small.txt") << (PosList() << 2000); // past end
    QTest::newRow("7") << QString("small.txt") << (PosList() << 2000 << 0);
}

void tst_File::seek()
{
    QFETCH(QString, fileName);
    QFETCH(QList<qint64>, offsets);
    const QString path = dir.path() + "/" + fileName;
    QVERIFY(copyFile(":/" + fileName, path));
    QFile qFile(path);
    File file(QUrl::fromLocalFile(path));
    QVERIFY(qFile.open(QIODevice::ReadOnly));
    QVERIFY(file.open(QIODevice::ReadOnly));

    foreach (qint64 offset, offsets) {
        QCOMPARE(file.seek(offset), qFile.seek(offset));
        QCOMPARE(file.pos(), qFile.pos());
        QVERIFY(file.waitForReadyRead());
        qint64 bytes = file.bytesAvailable();
        const QByteArray data1 = file.read(bytes);
        const QByteArray data2 = qFile.read(data1.size());
        if (data1 != data2) {
            qDebug() << "data1" << data1.left(100);
            qDebug() << "data2" << data2.left(100);
        }
        QVERIFY(data1 == data2);
    }

    qFile.remove();
}

void tst_File::readBench_data()
{
    QString fileName = "medium.txt";
    const QString path = dir.path() + "/" + fileName + "1";
    copyFile(":/" + fileName, path);
}

void tst_File::readBench()
{
    QString fileName = "medium.txt";
    const QString path = dir.path() + "/" + fileName + "1";
//    QFile qFile(path);
    File file(QUrl::fromLocalFile(path));
//    QVERIFY(qFile.open(QIODevice::ReadOnly));
    QVERIFY(file.open(QIODevice::ReadOnly));

    QByteArray data;

    QBENCHMARK {
        while (!file.atEnd()) {
            QVERIFY(file.waitForReadyRead());
            qint64 bytes = file.bytesAvailable();
            const QByteArray data1 = file.read(bytes);
            data.append(data1);
        }
    }
}

void tst_File::readQFileBench()
{
    QString fileName = "medium.txt";
    const QString path = dir.path() + "/" + fileName + "1";
    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly));

    QByteArray data;

    QBENCHMARK {
        while (!file.atEnd()) {
            qint64 bytes = 4096;
            const QByteArray data1 = file.read(bytes);
            data.append(data1);
        }
    }
}

QTEST_MAIN(tst_File)
#include "tst_file.moc"
