#include <QtWidgets/QApplication>

#include <QDebug>
#include <QDir>
#include <QThread>
#include <QIO/Dir>
#include <QIO/File>
#include <QIO/FileSystemModel>

#include <QTreeView>

#include "mainwindow.h"

void testFile()
{
//    const char *const filePath = "C:/Users/arch/Programming/asyncfile/freedesktop.org.xml";
    const char *const filePath = "/Users/arch/cardbase.xml";

    qDebug() << "main" << QThread::currentThreadId();

    File file;
    file.setUrl(QUrl::fromLocalFile(filePath));
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Can't open file";
        return;
    }

    qDebug() << "at end" << file.atEnd();
    QByteArray data;
    while (file.pos() < file.size()) {
        bool ok = file.waitForReadyRead(1000);
        qDebug() << "waitForReadyRead" << ok;
        qDebug() << "bytesAvailable" << file.bytesAvailable();
        QByteArray read = file.read(file.bytesAvailable());
        qDebug() << "read" << read;
        data.append(read);
//        qDebug() << "read" << file.read(file.bytesAvailable());
    }
    qDebug() << QString::fromUtf8(data).right(1*1024);
    qDebug() << file.pos() << file.size();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//    testFile();

    MainWindow w;
    w.show();

//    File file;
//    file.setUrl(QUrl::fromLocalFile("/Users/arch/Programming/qt/qio/examples/example/main.cpp"));

//    Dir dir(QUrl::fromLocalFile(QDir::homePath()));
//    auto future = dir.entryList();
//    future.waitForFinished();
//    qDebug() << future.resultCount();
//    for (int i = 0; i < future.resultCount(); ++i) {
//        auto info = future.resultAt(i);
//        qDebug() << info.url() << info.lastRead() << info.mimeTypeName();
//    }

//    QTreeView view;
//    FileSystemModel *model = new FileSystemModel(&view);
//    view.setModel(model);
//    model->setRootUrl(QUrl::fromLocalFile(QDir::homePath()));
//    view.show();

    return app.exec();
}
