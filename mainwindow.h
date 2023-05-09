#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    FileSyncMonster *f;
    Ui::MainWindow *ui;
    QString userName;
    QString password;
};
#endif // MAINWINDOW_H
