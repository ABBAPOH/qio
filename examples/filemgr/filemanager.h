#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "filemanagertask.h"

#include <QtWidgets/QApplication>

class FileManager : public QApplication
{
    Q_OBJECT

public:
    explicit FileManager(int &argc, char* argv[]);

public slots:
    void update();
    //
    void mkdir(const QString &srcPath, const QString &dirName);

signals:
    void updateUi();

private:
    QList<FileManagerTask*> m_tasks;

};

#endif // FILEMANAGER_H
