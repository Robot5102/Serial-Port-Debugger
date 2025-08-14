#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QString>
#include <QMessageBox>
#include <QTimer>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QThread>
#include "mythread.h"




QT_BEGIN_NAMESPACE
namespace Ui {
class App;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public:
    void paintEvent(QPaintEvent *event);
    void on_serialPort_ReadyRead();
    void on_timer_timeout();
    void TimeShow();
    void findSerial();
    void on_command_clicked();
    void on_Thread_timeout();




private slots:
    void on_btnCloseOrOpenSerial_clicked();
    void on_btnSend_clicked();
    void on_checkBoxTime_clicked(bool checked);
    void on_btnClear_clicked();
    void on_btnSave_clicked();
    void on_checkBhexShow_clicked(bool checked);
    void on_checkBTime_clicked(bool checked);
    void on_btnHideTable_clicked(bool checked);
    void on_btnHideHistory_clicked(bool checked);
    void on_checkBoxLoop_clicked(bool checked);
    void on_pushButton_11_clicked();
    void on_btnSave2_clicked();
    void on_pushButton_10_clicked();




private:
    Ui::App *ui;
    QSerialPort *serialPort;
    QTimer *timer;
    QTimer *timer2;
    QTimer *timer3;
    QString Time;
    bool TimeShowFlag = false;
    bool hexShowFlag = false;
    QList<QPushButton*> buttons;
    MyThread *thread;
};
#endif // WIDGET_H
