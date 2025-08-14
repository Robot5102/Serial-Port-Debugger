#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>

class MyThread : public QThread
{
    Q_OBJECT

public:
    explicit MyThread(QObject *parent = nullptr);
    void run() override;
    void setSleepTime(int time);

private:
    int sleepTime;

signals:
    void timeOut();
};

#endif // MYTHREAD_H
