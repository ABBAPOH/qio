#include <QtTest/QtTest>

#include <QIO/Dir>

class tst_Dir : public QObject
{
    Q_OBJECT
public:
    tst_Dir();

private slots:
    void initTestCase();
    void mkdir();
    void rmdir();
    void touch();

private:
    QTemporaryDir dir;
    QUrl dirUrl;
};

tst_Dir::tst_Dir()
{
}

void tst_Dir::initTestCase()
{
    dirUrl = QUrl::fromLocalFile(dir.path());
}

void tst_Dir::mkdir()
{
    const QString folderName = "folder_mkdir";
    const QString path = dir.path() + "/" + folderName;

    QVERIFY(!QFileInfo(path).exists());
    Dir d(dirUrl);
    auto future = d.mkdir(folderName);
    future.waitForFinished();
    QVERIFY(QFileInfo(path).exists());
    QVERIFY(QFileInfo(path).isDir());
    QVERIFY(!QFileInfo(path).isFile());
}

void tst_Dir::rmdir()
{
    const QString folderName = "folder_rmdir";
    const QString path = dir.path() + "/" + folderName;

    QVERIFY(!QFileInfo(path).exists());
    Dir d(dirUrl);
    auto future = d.mkdir(folderName);
    future.waitForFinished();
    future = d.rmdir(folderName);
    future.waitForFinished();
    QVERIFY(!QFileInfo(path).exists());
}

void tst_Dir::touch()
{
    const QString path = dir.path() + "/" + "file";

    QVERIFY(!QFileInfo(path).exists());
    Dir d(dirUrl);
    auto future = d.touch("file");
    future.waitForFinished();
    QVERIFY(QFileInfo(path).exists());
    QVERIFY(!QFileInfo(path).isDir());
    QVERIFY(QFileInfo(path).isFile());
}

QTEST_MAIN(tst_Dir)
#include "tst_dir.moc"
