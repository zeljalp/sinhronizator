#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionQuit_triggered()
{
    close();
}


void MainWindow::on_syncButton_clicked()
{
    FileSyncMonster f(QUrl("api.staging.hive5.app"), ui->userLine->text(), ui->passwordLine->text());
    f.login();
    //syncT = new SyncThread("api.staging.hive5.app", ui->userLine->text(), ui->passwordLine->text());
    //syncT->start();
}

