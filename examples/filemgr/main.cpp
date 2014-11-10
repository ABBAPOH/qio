#include "filemanager.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    FileManager app(argc, argv);

    MainWindow w;

    app.connect(&w, &MainWindow::commandMkDir, &app, &FileManager::mkdir);
    app.connect(&app, &FileManager::updateUi, &w, &MainWindow::onUpdate);

    w.show();

    return app.exec();
}
