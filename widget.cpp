#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::App)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    timer2 = new QTimer(this);
    timer3 = new QTimer(this);
    timer3->start(1000);
    timer2->start(1000);
    thread = new MyThread(this);
    // 初始化串口
    serialPort = new QSerialPort(this);

    // 设置窗口背景透明（关键：让图片的透明区域生效）
    setAttribute(Qt::WA_TranslucentBackground, true);

    // 初始化串口
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports)
    {
        ui->comboBox_serialNum->addItem(port.portName());
    }

    // 初始化ui
    ui->comboBox_boautrate->setCurrentIndex(6);
    ui->comboBox_databit->setCurrentIndex(3);
    ui->btnSend->setEnabled(false);
    ui->checkBoxTime->setEnabled(false);
    ui->labelTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    ui->checkBautoLine->setChecked(true);
    ui->checkBTime->setChecked(true);
    ui->pushButton_1->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->pushButton_4->setEnabled(false);
    ui->pushButton_5->setEnabled(false);
    ui->pushButton_6->setEnabled(false);
    ui->pushButton_7->setEnabled(false);
    ui->pushButton_8->setEnabled(false);
    ui->pushButton_9->setEnabled(false);


        // 连接信号槽
    connect(serialPort, &QSerialPort::readyRead, this, &Widget::on_serialPort_ReadyRead);
    connect(timer, &QTimer::timeout, this, &Widget::on_timer_timeout);
    connect(timer2, &QTimer::timeout, this, &Widget::TimeShow);
    connect(timer3, &QTimer::timeout, this, &Widget::findSerial);
    connect(thread, &MyThread::timeOut, this, &Widget::on_Thread_timeout);


    for(int i = 1; i < 10; i++)
    {
        QString btname = QString("pushButton_%1").arg(i);
        QPushButton* button = findChild<QPushButton*>(btname);
        if(button)
        {
            button->setProperty("index", i);
            buttons.append(button);
            connect(button, &QPushButton::clicked, this,&Widget::on_command_clicked);
        }
    }
}

Widget::~Widget()
{
    // 关闭串口
    if (serialPort->isOpen())
    {
        serialPort->close();
    }
    delete ui;
}

// 重写绘制事件，绘制背景图片
void Widget::paintEvent(QPaintEvent *event)
{
    // 先调用父类方法处理其他绘制
    QWidget::paintEvent(event);
    QPainter painter(this);
    // 加载背景图片（替换为你的图片路径）
    QPixmap bgPixmap(":/imgs/skl.png");
    if (bgPixmap.isNull())
    {
        qDebug() << "图片加载失败";
        return; // 图片加载失败则不绘制
    }
    // 拉伸图片铺满整个窗口
    painter.drawPixmap(rect(), bgPixmap.scaled(
                                   size(),
                                   Qt::IgnoreAspectRatio,
                                   Qt::SmoothTransformation // 平滑缩放
                                   ));
}

void Widget::on_btnCloseOrOpenSerial_clicked()
{

    if (serialPort->isOpen())
    {
        serialPort->close();
        qDebug() << "串口关闭成功";
        ui->btnCloseOrOpenSerial->setText("打开串口");
        ui->comboBox_serialNum->setEnabled(true);
        ui->comboBox_boautrate->setEnabled(true);
        ui->comboBox_databit->setEnabled(true);
        ui->comboBox_jiaoyan->setEnabled(true);
        ui->comboBox_stopbit->setEnabled(true);
        ui->comboBox_liukong->setEnabled(true);
        ui->btnSend->setEnabled(false);
        ui->checkBoxTime->setEnabled(false);
        ui->pushButton_1->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
        ui->pushButton_4->setEnabled(false);
        ui->pushButton_5->setEnabled(false);
        ui->pushButton_6->setEnabled(false);
        ui->pushButton_7->setEnabled(false);
        ui->pushButton_8->setEnabled(false);
        ui->pushButton_9->setEnabled(false);
    }
    else
    {
        // 选择端口号
        serialPort->setPortName(ui->comboBox_serialNum->currentText());

        // 选择波特率
        serialPort->setBaudRate(ui->comboBox_boautrate->currentText().toInt());

        // 选择数据位
        serialPort->setDataBits(static_cast<QSerialPort::DataBits>(ui->comboBox_databit->currentText().toUInt()));

        // 选择校验位
        QString str = ui->comboBox_jiaoyan->currentText();
        if (str == "No")
        {
            serialPort->setParity(QSerialPort::NoParity);
        }
        else if (str == "Odd")
        {
            serialPort->setParity(QSerialPort::OddParity);
        }
        else if (str == "Even")
        {
            serialPort->setParity(QSerialPort::EvenParity);
        }
        else if (str == "Mark")
        {
            serialPort->setParity(QSerialPort::MarkParity);
        }
        else if (str == "Space")
        {
            serialPort->setParity(QSerialPort::SpaceParity);
        }
        // 选择停止位
        str = ui->comboBox_stopbit->currentText();
        if (str == "One")
        {
            serialPort->setStopBits(QSerialPort::OneStop);
        }
        else if (str == "OneAndHalf")

        {
            serialPort->setStopBits(QSerialPort::OneAndHalfStop);
        }
        else if (str == "Two")
        {
            serialPort->setStopBits(QSerialPort::TwoStop);
        }
        // 选择流控制
        str = ui->comboBox_liukong->currentText();
        if (str == "No")
        {
            serialPort->setFlowControl(QSerialPort::NoFlowControl);
        }
        else if (str == "Hard")
        {
            serialPort->setFlowControl(QSerialPort::HardwareControl);
        }
        else if (str == "Soft")
        {
            serialPort->setFlowControl(QSerialPort::SoftwareControl);
        }
        // 打开串口
        if (serialPort->open(QIODevice::ReadWrite))
        {
            ui->btnCloseOrOpenSerial->setText("关闭串口");
            ui->comboBox_serialNum->setEnabled(false);
            ui->comboBox_boautrate->setEnabled(false);
            ui->comboBox_databit->setEnabled(false);
            ui->comboBox_jiaoyan->setEnabled(false);
            ui->comboBox_stopbit->setEnabled(false);
            ui->comboBox_liukong->setEnabled(false);
            ui->btnSend->setEnabled(true);
            ui->checkBoxTime->setEnabled(true);
            ui->pushButton_1->setEnabled(true);
            ui->pushButton_2->setEnabled(true);
            ui->pushButton_3->setEnabled(true);
            ui->pushButton_4->setEnabled(true);
            ui->pushButton_5->setEnabled(true);
            ui->pushButton_6->setEnabled(true);
            ui->pushButton_7->setEnabled(true);
            ui->pushButton_8->setEnabled(true);
            ui->pushButton_9->setEnabled(true);
        }
        else
        {
            QMessageBox::warning(this, "警告", "串口打开失败");
        }
    }
}

void Widget::on_btnSend_clicked()
{
    static int SendBits = 0;
    static QString SendBack;
    if (ui->lineEditSend->text() == "")
    {
        ui->labelSendStatus->setText("Input Empty!");
        return;
    }
    if (SendBits == 0)
    {
        ui->textEditRecord->clear();
    }
    const char *sendData = ui->lineEditSend->text().toStdString().c_str();
    int writeBits = 0;
    if (ui->checkBoxHEX->isChecked())
    {
        QString temp = ui->lineEditSend->text();
        QByteArray tmpArray = temp.toLocal8Bit();
        if (tmpArray.size() % 2 != 0)
        {
            ui->labelSendStatus->setText("Error Input!");
            return;
        }
        for (char c : tmpArray)
        {
            if (!std::isxdigit(c))
            {
                ui->labelSendStatus->setText("Error Input!");
                return;
            }
        }
        QByteArray sendArray = QByteArray::fromHex(tmpArray);
        if (ui->checkBoxLine->isChecked())
        {
            writeBits = serialPort->write(sendArray.append("\r\n"));
        }
        else
        {
            writeBits = serialPort->write(sendArray);
        }
    }
    else
    {
        if (ui->checkBoxLine->isChecked())
        {
            QByteArray sendArray = QByteArray::fromStdString(sendData);
            writeBits = serialPort->write(sendArray.append("\r\n"));
        }
        else
        {
            writeBits = serialPort->write(sendData);
        }
    }
    if (writeBits == -1)
    {
        ui->labelSendStatus->setText("Send Error!");
    }
    else
    {
        SendBits += writeBits;
        ui->labelSendStatus->setText("Send OK!");
        ui->labelSendBits->setText("SendBits: " + QString::number(SendBits));
        if (SendBack != ui->lineEditSend->text())
        {
            ui->textEditRecord->append(sendData);
            SendBack = ui->lineEditSend->text();
        }
    }
}

void Widget::on_serialPort_ReadyRead()
{
    static int RevBits = 0;
    if (RevBits == 0)
    {
        ui->textEditRev->clear();
    }
    QByteArray receiveData = serialPort->readAll();

    QString str;
    if (hexShowFlag)
    {
        QString hexString = receiveData.toHex();
        str = hexString.toUpper();
    }
    else
    {
        str = QString::fromUtf8(receiveData);
    }
    RevBits += receiveData.size();
    ui->textEditRev->moveCursor(QTextCursor::End);
    ui->textEditRev->ensureCursorVisible();

    if (ui->checkBTime->isChecked())
    {
        if (ui->checkBautoLine->isChecked())
        {
            ui->textEditRev->insertPlainText(QDateTime::currentDateTime().toString("[hh:mm:ss]") + " " + str + "\r\n");
        }
        else
        {
            ui->textEditRev->insertPlainText(QDateTime::currentDateTime().toString("[hh:mm:ss]") + " " + str);
        }
    }
    else
    {
        if (ui->checkBautoLine->isChecked())
        {
            ui->textEditRev->insertPlainText(str + "\r\n");
        }
        else
        {
            ui->textEditRev->insertPlainText(str);
        }
    }

    ui->labelRevBits->setText("RevBits: " + QString::number(RevBits));
}

void Widget::on_checkBoxTime_clicked(bool checked)
{
    if (checked)
    {
        timer->start(ui->lineEditTime->text().toInt());
        ui->lineEditTime->setEnabled(false);
        ui->lineEditSend->setEnabled(false);
        ui->btnSend->setEnabled(false);
        ui->btnCloseOrOpenSerial->setEnabled(false);
    }
    else
    {
        timer->stop();
        ui->lineEditTime->setEnabled(true);
        ui->lineEditSend->setEnabled(true);
        ui->btnSend->setEnabled(true);
        ui->btnCloseOrOpenSerial->setEnabled(true);
    }
}

void Widget::on_timer_timeout()
{
    on_btnSend_clicked();
}

void Widget::on_btnClear_clicked()
{
    ui->textEditRev->clear();
}

void Widget::on_btnSave_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存文件", "", "Text (*.txt)");
    if (fileName.isEmpty())
    {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream out(&file);
    out << ui->textEditRev->toPlainText();
    file.close();
}

void Widget::TimeShow()
{
    ui->labelTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

}

void Widget::on_checkBhexShow_clicked(bool checked)
{
    if (checked)
    {
        hexShowFlag = true;
    }
    else
    {
        hexShowFlag = false;
    }
    QString originalText = ui->textEditRev->toPlainText();
    QString convertedText;
    QStringList lines = originalText.split("\n");
    for (const QString &line : lines)
    {
        // 跳过空行
        if (line.isEmpty())
        {
            convertedText += "\n";
            continue;
        }
        QString processedLine;
        if (line[0] == '[') // 处理带前缀的行
        {
            // 安全获取前缀，确保不会越界
            int prefixLength = qMin(11, line.length());
            QString prefix = line.left(prefixLength);
            processedLine += prefix;
            // 处理剩余部分
            if (line.length() > prefixLength)
            {
                QString content = line.mid(prefixLength);
                if (checked)
                {
                    QString result = QString::fromUtf8(content.toUtf8().toHex().toUpper());
                    for (int i = 0; i < result.length(); i++)
                    {
                        processedLine += result[i];
                        if ((i + 1) % 2 == 0)
                        {
                            processedLine += " ";
                        }
                    }
                    // 文本转十六进制
                }
                else
                {
                    // 十六进制转文本
                    processedLine += QString::fromUtf8(QByteArray::fromHex(content.remove(" ").toUtf8()));
                }
            }
        }
        else // 处理普通行
        {
            if (checked)
            {
                QString result = QString::fromUtf8(line.toUtf8().toHex().toUpper());
                for (int i = 0; i < result.length(); i++)
                {
                    processedLine += result[i];
                    if ((i + 1) % 2 == 0)
                    {
                        processedLine += " ";
                    }
                }
                // 文本转十六进制
            }
            else
            {
                QString result = line;
                // 十六进制转文本
                processedLine += QString::fromUtf8(QByteArray::fromHex(result.remove(" ").toUtf8()));
            }
        }
        convertedText += processedLine + "\n";
    }
    convertedText.chop(1);
    ui->textEditRev->setText(convertedText);
}

void Widget::on_checkBTime_clicked(bool checked)
{
    if (checked)
    {
        TimeShowFlag = true;
    }
    else
    {
        TimeShowFlag = false;
    }
}

void Widget::on_btnHideTable_clicked(bool checked)
{
    if(checked)
    {
        ui->groupBoxTexts->hide();
        ui->btnHideTable->setText("显示面板");
    }
    else
    {
        ui->groupBoxTexts->show();
        ui->btnHideTable->setText("隐藏面板");
    }
}


void Widget::on_btnHideHistory_clicked(bool checked)
{
    if(checked)
    {
        ui->groupBoxRecord->hide();
        ui->btnHideHistory->setText("显示历史");
    }
    else
    {
        ui->groupBoxRecord->show();
        ui->btnHideHistory->setText("隐藏历史");
    }
}

void Widget::findSerial()
{
    ui->comboBox_serialNum->clear();
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports)
    {
        ui->comboBox_serialNum->addItem(port.portName());
    }
}

void Widget::on_command_clicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    int num = button->property("index").toInt();
    QCheckBox* checkBox = findChild<QCheckBox*>(QString("checkBox_%1").arg(num));
    QLineEdit* lineEdit = findChild<QLineEdit*>(QString("lineEdit_%1").arg(num));
    ui->lineEditSend->setText(lineEdit->text());
    ui->checkBoxHEX->setChecked(checkBox->isChecked());
    this->on_btnSend_clicked();
}
void Widget::on_checkBoxLoop_clicked(bool checked)
{
    if(checked)
    {
        ui->spinBox->setEnabled(false);
        thread->setSleepTime(ui->spinBox->text().toInt());
        thread->start();
        ui->pushButton_1->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
        ui->pushButton_4->setEnabled(false);
        ui->pushButton_5->setEnabled(false);
        ui->pushButton_6->setEnabled(false);
        ui->pushButton_7->setEnabled(false);
        ui->pushButton_8->setEnabled(false);
        ui->pushButton_9->setEnabled(false);
        ui->btnSend->setEnabled(false);

    }
    else
    {
        ui->spinBox->setEnabled(true);
        thread->terminate();
        ui->pushButton_1->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        ui->pushButton_4->setEnabled(true);
        ui->pushButton_5->setEnabled(true);
        ui->pushButton_6->setEnabled(true);
        ui->pushButton_7->setEnabled(true);
        ui->pushButton_8->setEnabled(true);
        ui->pushButton_9->setEnabled(true);
        ui->btnSend->setEnabled(true);
    }
}

void Widget::on_Thread_timeout()
{
    static int index = 0;
    if(index >= buttons.size())
    {
        index = 0;
    }
    qDebug() << index;
    emit buttons[index]->clicked();
    index++;
}


void Widget::on_pushButton_11_clicked()
{
    int ret = QMessageBox::information(this, "提示", "重置列表不可逆，是否确认重置？", QMessageBox::Yes | QMessageBox::No);
    if(ret == QMessageBox::Yes)
    {
        for(int i = 1 ;i <= buttons.size();i++)
        {
            QCheckBox *checkBox = findChild<QCheckBox *>(QString("checkBox_%1").arg(i));
            QLineEdit *lineEdit = findChild<QLineEdit *>(QString("lineEdit_%1").arg(i));
            checkBox->setChecked(false);
            lineEdit->clear();
        }
    }
}


void Widget::on_btnSave2_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存文件", "", "Text (*.txt)");
    if (fileName.isEmpty())
    {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream out(&file);
    for (int i = 1; i <= buttons.size(); i++)
    {
        QCheckBox *checkBox = findChild<QCheckBox *>(QString("checkBox_%1").arg(i));
        QLineEdit *lineEdit = findChild<QLineEdit *>(QString("lineEdit_%1").arg(i));
        out << checkBox->isChecked() << " " << lineEdit->text() << "\n";
    }
    file.close();
    QMessageBox::information(this, "提示", "保存成功");

}


void Widget::on_pushButton_10_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "打开文件", "", "Text (*.txt)");
    if (fileName.isEmpty())
    {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream in(&file);
    QString line;
    int i = 1;
    while (in.readLineInto(&line))
    {
        QStringList parts = line.split(" ");
        if (parts.size() == 2)
        {
            QCheckBox *checkBox = findChild<QCheckBox *>(QString("checkBox_%1").arg(i));
            QLineEdit *lineEdit = findChild<QLineEdit *>(QString("lineEdit_%1").arg(i));
            checkBox->setChecked(parts[0]=="1");
            lineEdit->setText(parts[1]);
            i++;
        }
    }
    file.close();
    QMessageBox::information(this, "提示", "加载成功");

}

