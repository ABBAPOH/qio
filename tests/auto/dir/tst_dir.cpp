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
    void remove();
    void removeRecursively();

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
    const QString fileName = "file_touch";
    const QString path = dir.path() + "/" + fileName;

    QVERIFY(!QFileInfo(path).exists());
    Dir d(dirUrl);
    auto future = d.touch(fileName);
    future.waitForFinished();
    QVERIFY(QFileInfo(path).exists());
    QVERIFY(!QFileInfo(path).isDir());
    QVERIFY(QFileInfo(path).isFile());
}

void tst_Dir::remove()
{
    const QString fileName = "file_remove";
    const QString path = dir.path() + "/" + fileName;

    QVERIFY(!QFileInfo(path).exists());
    Dir d(dirUrl);
    qDebug("touch");
    auto future = d.touch(fileName);
    future.waitForFinished();
    qDebug("remove");
    future = d.remove(fileName);
    qDebug("waitForFinished");
    future.waitForFinished();
    QVERIFY(!QFileInfo(path).exists());
}

static void makeHierarchy(const QString &path, const QString &folderName)
{
    QDir dir(path);
    dir.mkdir(folderName);
    dir.cd(folderName);

    dir.mkdir("folder1");
    dir.cd("folder1");

    dir.mkdir("subfolder1");
    dir.cd("subfolder1");
    Dir::touch(QUrl::fromLocalFile(dir.absolutePath() + "/" + "file1"));
    Dir::touch(QUrl::fromLocalFile(dir.absolutePath() + "/" + "file2"));
    dir.cdUp();

    dir.mkdir("subfolder2");
    dir.cd("subfolder2");
    Dir::touch(QUrl::fromLocalFile(dir.absolutePath() + "/" + "file1"));
    Dir::touch(QUrl::fromLocalFile(dir.absolutePath() + "/" + "file2"));
    dir.cdUp();

    Dir::touch(QUrl::fromLocalFile(dir.absolutePath() + "/" + "file1"));
    Dir::touch(QUrl::fromLocalFile(dir.absolutePath() + "/" + "file2"));
    Dir::touch(QUrl::fromLocalFile(dir.absolutePath() + "/" + "file3"));
}

void tst_Dir::removeRecursively()
{
    const QString folderName = "folder_recursively";
    const QString path = dir.path() + "/" + folderName;
    QVERIFY(!QFileInfo(path).exists());
    makeHierarchy(dir.path(), "folder_recursively");
    QVERIFY(QFileInfo(path).exists());
    auto future = Dir::removeRecursively(QUrl::fromLocalFile(path));
    future.waitForFinished();
    const bool ok = future.result();
    QVERIFY(ok);
    QVERIFY(!QFileInfo(path).exists());
}

QTEST_MAIN(tst_Dir)
#include "tst_dir.moc"
