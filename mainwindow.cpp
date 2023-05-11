#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QIcon>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    createActions();
    createTrayIcon();

    setWindowIcon(QIcon(":/images/tray.png"));
    trayIcon->setIcon(QIcon(":/images/tray.png"));
    trayIcon->show();

    f = new FileSyncMonster(QUrl("api.staging.hive5.app"));

    setWindowFlags(Qt::Dialog);
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
    //d.doDownload();
    f->setUserAndPassword(ui->userLine->text(), ui->passwordLine->text());
    f->startSyncMachine();
    //f->login();
    //syncT = new SyncThread("api.staging.hive5.app", ui->userLine->text(), ui->passwordLine->text());
    //syncT->start();
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(syncAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    syncAction = new QAction(tr("&Sync"), this);
    connect(syncAction, &QAction::triggered, this, &MainWindow::on_syncButton_clicked);

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void MainWindow::closeEvent(QCloseEvent *ev)
{

#ifdef Q_OS_OSX
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
    if (trayIcon->isVisible()) {
        QMessageBox::information(this, tr("Synchronizator"),
                                 tr("The program will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Quit</b> in the context menu "
                                    "of the system tray entry."));
        hide();
        ev->ignore();
    }

}

