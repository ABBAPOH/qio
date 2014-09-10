#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class FileSystemModel;
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

private:
    void setupToolBar();

private slots:
    void onRootUrlChanged(const QUrl &url);
    void onTextEdited(const QString &text);
    void onMkdirFinished();

private:
    Ui::MainWindow *ui;
    QLineEdit *m_lineEdit;
    FileSystemModel *m_model;
};

#endif // MAINWINDOW_H
