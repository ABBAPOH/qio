#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QDir>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_model(new FileSystemModel(this)),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupToolBar();

    ui->treeView->setModel(m_model);

    connect(m_model, &FileSystemModel::rootUrlChanged, this, &MainWindow::onRootUrlChanged);
    m_model->setRootUrl(QUrl::fromLocalFile(QDir::homePath()));

    connect(ui->actionNewFolder, &QAction::triggered, this, &MainWindow::mkdir);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mkdir()
{
    const QString name = QInputDialog::getText(this,
                                               tr("New folder"),
                                               tr("Enter folder name"),
                                               QLineEdit::Normal,
                                               m_model->getNonExistentFileName(tr("New folder")));
    if (name.isEmpty())
        return;

    emit commandMkDir(m_model->rootUrl().toString(), name);
}

void MainWindow::setupToolBar()
{
    ui->actionNewFolder->setIcon(QIcon(":/qt-project.org/styles/commonstyle/images/newdirectory-128.png"));

    m_lineEdit = new QLineEdit(ui->toolBar);
    connect(m_lineEdit, &QLineEdit::textEdited, this, &MainWindow::onTextEdited);

    ui->toolBar->addWidget(m_lineEdit);
}

void MainWindow::onRootUrlChanged(const QUrl &url)
{
    m_lineEdit->setText(url.toString());
}

void MainWindow::onTextEdited(const QString &text)
{
    m_model->setRootUrl(QUrl::fromUserInput(text));
}

void MainWindow::onUpdate()
{
    m_model->refresh(QModelIndex());
}
