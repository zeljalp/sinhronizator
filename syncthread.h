#ifndef SYNCTHREAD_H
#define SYNCTHREAD_H
#include <QThread>
#include "filedownloader.h"

class SyncThread : public QThread
{
public:
    SyncThread(QString url, QString user, QString password);
    void run();
    QString _url;
signals:
    void syncFinished();
private:
    FileSyncMonster* downloader;
};

#endif // SYNCTHREAD_H
