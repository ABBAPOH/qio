#include <QtWidgets/QApplication>

#include <QDebug>
#include <QDir>
#include <QIO/Dir>
#include <QIO/File>
#include <QIO/FileSystemModel>

#include <QTreeView>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

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
