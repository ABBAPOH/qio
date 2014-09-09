#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QIO/Dir>
#include <QIO/FileSystemModel>

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QInputDialog>
#include <QtCore/QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_model(new FileSystemModel(this))
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

    Dir dir(m_model->rootUrl());
    dir.mkdir(name);
}

void MainWindow::setupToolBar()
{
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
