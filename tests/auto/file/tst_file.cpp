#include <QtTest/QtTest>

#include <QIO/File>

#include <QtCore/QFile>
#include <QtCore/QTemporaryDir>

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
    {
        const QString path = dir.path() + "/" + "small.txt";
        QVERIFY(QFile::copy(":/small.txt", path));
        File f(QUrl::fromLocalFile(path));
        QVERIFY(f.open(QIODevice::ReadOnly));
        QVERIFY(QFile::remove(path));
    }

    {
        File f(QUrl::fromLocalFile("notexist.txt"));
        QVERIFY(!f.open(QIODevice::ReadOnly));
    }
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
    QVERIFY(QFile::copy(":/" + fileName, path));
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
}

QTEST_MAIN(tst_File)
#include "tst_file.moc"
