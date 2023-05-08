#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->remoteLine->setText("https://s3.eu-west-2.amazonaws.com/labdeck/updates/mathdeck1");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionQuit_triggered()
{
    close();
}


void MainWindow::on_localBrowseButton_clicked()
{
    localDir = QFileDialog::getExistingDirectory(this, tr("Select local directory"), QDir::homePath());
    if (!localDir.isEmpty())
    {
        ui->localLine->setText(localDir);
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    FileDownloader* downloader = new FileDownloader();
    connect(downloader, SIGNAL(downloaded(FileDownloader*)), this, SLOT(fileDownloaded(FileDownloader*)));
    connect(downloader, SIGNAL(downloadError(FileDownloader*)), this, SLOT(error(FileDownloader*)));
    downloader->download(ui->remoteLine->text());
}

