#include <QtTest/QtTest>

#include <QIO/FileEntry>

class tst_FileEntry : public QObject
{
    Q_OBJECT
public:
    tst_FileEntry();

private slots:
    void initTestCase();
    void mkdir();
    void rmdir();
    void touch();
    void remove();
    void rename();
    void removeRecursively();
    void copy();
    void move();

private:
    QTemporaryDir dir;
    QUrl dirUrl;
};

tst_FileEntry::tst_FileEntry()
{
}

void tst_FileEntry::initTestCase()
{
    dirUrl = QUrl::fromLocalFile(dir.path());
}

void tst_FileEntry::mkdir()
{
    const QString folderName = "folder_mkdir";
    const QString path = dir.path() + "/" + folderName;

    QVERIFY(!QFileInfo(path).exists());
    FileEntry d(dirUrl);
    auto future = d.mkdir(folderName);
    future.waitForFinished();
    QVERIFY(QFileInfo(path).exists());
    QVERIFY(QFileInfo(path).isDir());
    QVERIFY(!QFileInfo(path).isFile());
}

void tst_FileEntry::rmdir()
{
    const QString folderName = "folder_rmdir";
    const QString path = dir.path() + "/" + folderName;

    QVERIFY(!QFileInfo(path).exists());
    FileEntry d(dirUrl);
    auto future = d.mkdir(folderName);
    future.waitForFinished();
    future = d.rmdir(folderName);
    future.waitForFinished();
    QVERIFY(!QFileInfo(path).exists());
}

void tst_FileEntry::touch()
{
    const QString fileName = "file_touch";
    const QString path = dir.path() + "/" + fileName;

    QVERIFY(!QFileInfo(path).exists());
    FileEntry d(dirUrl);
    auto future = d.touch(fileName);
    future.waitForFinished();
    QVERIFY(QFileInfo(path).exists());
    QVERIFY(!QFileInfo(path).isDir());
    QVERIFY(QFileInfo(path).isFile());
}

void tst_FileEntry::remove()
{
    const QString fileName = "file_remove";
    const QString path = dir.path() + "/" + fileName;

    QVERIFY(!QFileInfo(path).exists());
    FileEntry d(dirUrl);
    qDebug("touch");
    auto future = d.touch(fileName);
    future.waitForFinished();
    qDebug("remove");
    future = d.remove(fileName);
    qDebug("waitForFinished");
    future.waitForFinished();
    QVERIFY(!QFileInfo(path).exists());
}

void tst_FileEntry::rename()
{
    const QString fileName = "file_rename";
    const QString newfileName = "newfile_rename";
    const QString path = dir.path() + "/" + fileName;
    const QString newPath = dir.path() + "/" + newfileName;

    QVERIFY(!QFileInfo(path).exists());
    QVERIFY(!QFileInfo(newPath).exists());
    FileEntry entry(dirUrl);
    auto future = entry.touch(fileName);
    future.waitForFinished();
    QVERIFY(future.result());
    QVERIFY(QFileInfo(path).exists());
    future = entry.rename(fileName, newfileName);
    future.waitForFinished();
    QVERIFY(future.result());
    QVERIFY(!QFileInfo(path).exists());
    QVERIFY(QFileInfo(newPath).exists());

    entry = FileEntry(QUrl::fromLocalFile(newPath));
    future = entry.rename(fileName);
    future.waitForFinished();
    QVERIFY(future.result());
    QVERIFY(QFileInfo(path).exists());
    QVERIFY(!QFileInfo(newPath).exists());
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
    FileEntry(QUrl::fromLocalFile(dir.absolutePath() + "/" + "file1")).touch();
    FileEntry(QUrl::fromLocalFile(dir.absolutePath() + "/" + "file2")).touch();
    dir.cdUp();

    dir.mkdir("subfolder2");
    dir.cd("subfolder2");
    FileEntry(QUrl::fromLocalFile(dir.absolutePath() + "/" + "file1")).touch();
    FileEntry(QUrl::fromLocalFile(dir.absolutePath() + "/" + "file2")).touch();
    dir.cdUp();

    FileEntry(QUrl::fromLocalFile(dir.absolutePath() + "/" + "file1")).touch();
    FileEntry(QUrl::fromLocalFile(dir.absolutePath() + "/" + "file2")).touch();
    FileEntry(QUrl::fromLocalFile(dir.absolutePath() + "/" + "file3")).touch();
}

void tst_FileEntry::removeRecursively()
{
    const QString folderName = "folder_recursively";
    const QString path = dir.path() + "/" + folderName;
    QVERIFY(!QFileInfo(path).exists());
    makeHierarchy(dir.path(), "folder_recursively");
    QVERIFY(QFileInfo(path).exists());
    auto future = FileEntry(QUrl::fromLocalFile(path)).removeRecursively();
    future.waitForFinished();
    const bool ok = future.result();
    QVERIFY(ok);
    QVERIFY(!QFileInfo(path).exists());
}

static bool compareInfos(const FileInfo &lhs, const FileInfo &rhs)
{
    return lhs.isDir() == rhs.isDir() && lhs.fileName() == rhs.fileName();
}

static bool compareDirectories(const QUrl &lhs, const QUrl &rhs)
{
    FileEntry leftEntry(lhs);
    FileEntry rigthEntry(rhs);

    auto leftFuture = leftEntry.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
    auto rightFuture = rigthEntry.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
    leftFuture.waitForFinished();
    rightFuture.waitForFinished();

    const FileInfoList leftList = leftFuture.result();
    const FileInfoList rightList = rightFuture.result();

    if (leftList.count() != rightList.count())
        return false;

    for (int i = 0; i < leftList.count(); ++i) {
        const FileInfo leftInfo = leftList.at(i);
        const FileInfo rightInfo = rightList.at(i);
        if (!compareInfos(leftInfo, rightInfo))
            return false;
        if (leftInfo.isDir()) {
            if (!compareDirectories(leftInfo.url(), rightInfo.url()))
                return false;
        }
    }

    return true;
}

void tst_FileEntry::copy()
{
    const QString folderName = "folder_copy";
    const QString path = dir.path() + "/" + folderName;
    const QString destPath = dir.path() + "/" + "folder2_copy";
    QVERIFY(!QFileInfo(path).exists());
    makeHierarchy(dir.path(), "folder_copy");
    QVERIFY(QFileInfo(path).exists());

    auto future = FileEntry(QUrl::fromLocalFile(path)).copy(QUrl::fromLocalFile(destPath));
    future.waitForFinished();
    QVERIFY(future.result());
    QVERIFY(compareDirectories(QUrl::fromLocalFile(path), QUrl::fromLocalFile(destPath)));
}

void tst_FileEntry::move()
{
    const QString folderName = "folder_move";
    const QString path = dir.path() + "/" + folderName;
    const QString testFolderName = "testfolder_move";
    const QString testFolderPath = dir.path() + "/" + testFolderName;
    const QString destPath = dir.path() + "/" + "folder2_move";
    QVERIFY(!QFileInfo(path).exists());
    makeHierarchy(dir.path(), folderName);
    makeHierarchy(dir.path(), testFolderName);
    QVERIFY(QFileInfo(path).exists());

    auto future = FileEntry(QUrl::fromLocalFile(path)).move(QUrl::fromLocalFile(destPath));
    future.waitForFinished();
    QVERIFY(future.result());
    QVERIFY(!QFileInfo(path).exists());
    QVERIFY(QFileInfo(destPath).exists());
    QVERIFY(compareDirectories(QUrl::fromLocalFile(testFolderPath), QUrl::fromLocalFile(destPath)));
}

QTEST_MAIN(tst_FileEntry)
#include "tst_fileentry.moc"
