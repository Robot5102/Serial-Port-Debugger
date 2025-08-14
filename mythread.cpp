#include "mythread.h"

MyThread::MyThread(QObject *parent)
    : QThread{parent}
{}

void MyThread::run()
{
    while (true)
    {
        msleep(sleepTime);
        emit timeOut();
    }
}

void MyThread::setSleepTime(int time)
{
    sleepTime = time;
}


