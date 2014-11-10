#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QIO/FileSystemModel>

#include <QtWidgets/QMainWindow>

class QLineEdit;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void mkdir();
    void onUpdate();

signals:
    void commandMkDir(const QString &srcPath, const QString &dirName);

private:
    void setupToolBar();

private slots:
    void onRootUrlChanged(const QUrl &url);
    void onTextEdited(const QString &text);

private:
    FileSystemModel *m_model;
    Ui::MainWindow *ui;
    QLineEdit *m_lineEdit;
};

#endif // MAINWINDOW_H
