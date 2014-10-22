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

QTEST_MAIN(tst_Archive)
#include "tst_archive.moc"
