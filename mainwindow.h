#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QCloseEvent>
#include "syncthread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();

    void on_syncButton_clicked();

private:
    SyncThread *syncT;
    //Downloader d;
    FileSyncMonster *f;
    Ui::MainWindow *ui;
    QString userName;
    QString password;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QAction *minimizeAction;
    QAction *syncAction;
    QAction *restoreAction;
    QAction *quitAction;
private:
    void createTrayIcon();
    void createActions();

protected:
    void closeEvent(QCloseEvent *evenet) override;
};
#endif // MAINWINDOW_H
