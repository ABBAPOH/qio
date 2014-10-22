#include <QtTest/QtTest>

#include <QIO/File>
#include <QIO/FileEntry>

#include <QtCore/QFile>
#include <QtCore/QTemporaryDir>

static bool copyFile(const QString &from, const QString &to)
{
    QFile file(from);
    bool ok = file.copy(to);
    if (!ok) {
        qWarning() << "Can't copy file:" << file.errorString();
        return false;
    }
    file.setFileName(to);
    ok = file.setPermissions(file.permissions() | QFileDevice::WriteOwner);
    if (!ok) {
        qWarning() << "Can't set permissions:" << file.errorString();
        return false;
    }

    return true;
}

class tst_Archive : public QObject
{
    Q_OBJECT
public:
    tst_Archive();

private slots:
    void list_data();
    void list();
    void infoList_data();
    void infoList();

private:
};

tst_Archive::tst_Archive()
{
}

void tst_Archive::list_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QStringList>("list");

    QTest::newRow("empty") << QString("empty.zip") << (QStringList());
    QTest::newRow("plain") << QString("plain.zip") << (QStringList() << "freedesktop.org.xml" << "medium.txt" << "small.txt");
}

void tst_Archive::list()
{
    QTemporaryDir tempDir;

    QFETCH(QString, fileName);
    QFETCH(QStringList, list);
    const QString filePath = tempDir.path() + "/" + fileName;

    QVERIFY(::copyFile(":/" + fileName, filePath));

    FileEntry entry(QString("zip://%1").arg(filePath));
    auto future = entry.list(QDir::NoDotAndDotDot | QDir::AllEntries, QDir::Name);
    future.waitForFinished();
    QCOMPARE(future.result(), list);
}

struct Info
{
    Info() {}
    Info(bool isDir, const QString &name) : isDir(isDir), name(name) {}

    bool isDir;
    QString name;
};

typedef QVector<Info> InfoList;

Q_DECLARE_METATYPE(Info)

void tst_Archive::infoList_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<InfoList>("list");

    QTest::newRow("empty") << QString("empty.zip") << (InfoList());
    QTest::newRow("plain") << QString("plain.zip") << (InfoList()
                                                       << Info(false, "freedesktop.org.xml")
                                                       << Info(false, "medium.txt")
                                                       << Info(false, "small.txt"));
}

static bool compareInfos(const FileInfo &fileInfo, const Info &info)
{
    return fileInfo.isDir() == info.isDir
            && fileInfo.fileName() == info.name;
}

static bool compareInfos(const FileInfoList &fileInfos, const InfoList &infos)
{
    if (infos.count() != fileInfos.count())
        return false;

    for (int i = 0; i < infos.count(); ++i) {
        if (!compareInfos(fileInfos.at(i), infos.at(i)))
            return false;
    }
    return true;
}

void tst_Archive::infoList()
{
    QTemporaryDir tempDir;

    QFETCH(QString, fileName);
    QFETCH(InfoList, list);
    const QString filePath = tempDir.path() + "/" + fileName;

    QVERIFY(::copyFile(":/" + fileName, filePath));

    FileEntry entry(QString("zip://%1").arg(filePath));
    auto future = entry.infoList(QDir::NoDotAndDotDot | QDir::AllEntries, QDir::Name);
    future.waitForFinished();
    QVERIFY(compareInfos(future.result(), list));
}

QTEST_MAIN(tst_Archive)
#include "tst_archive.moc"
