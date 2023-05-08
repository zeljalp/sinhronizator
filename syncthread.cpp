#include "syncthread.h"

SyncThread::SyncThread(QString url, QString user, QString password)
{
    _url = url;
    downloader = new FileSyncMonster(_url, user, password);
}

void SyncThread::run()
{
    downloader->login();
}
