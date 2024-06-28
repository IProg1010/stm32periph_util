#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtNetwork/QTcpSocket>
#include <iostream>
#include "socketcontrol.h"
#include <QDebug>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->btnGetConfig->setEnabled(false);
    ui->btnSetConfig->setEnabled(false);
    ui->sendButton->setEnabled(false);
    ui->btnOnOff->setEnabled(false);

    work = new WorkerThread(this);
    //connect(this, &MainWindow::senddata, work->soc, &SocketControl::senddata);
    //connect(this, &MainWindow::connectSignal, work->soc, &SocketControl::connectSlot);
    //connect(work->soc, &SocketControl::receivedata , this, &MainWindow::receivedata);

    work->mainptr = this;
    work->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::message(QString str)
{
    ui->readDataPlain->setPlainText(str);
}

void MainWindow::receivedata(QByteArray arr)
{
    qDebug() << "arr = " << arr << "size  = " << arr.size();

    uint16_t len = arr[3] << 8 | arr[4];
    for(auto it = interfaces.begin(); it != interfaces.end(); it++)
    {
        if((*it)->interface_type == arr[0] && (*it)->interface_num == arr[1])
        {
            printMessage("По интерфейсу "+getTextInterface(arr[0], arr[1])+" приняты данные. Длина = " + QString::number(len));
            for(int i = 2; i < arr.size(); i++){
                (*it)->data_r.append(arr[i]);
            }
        }
    }
    //ui->readDataPlain->setPlainText(QString(arr));
}

void MainWindow::on_btnConnect_clicked()
{
    emit connectSignal(ui->editIP->text(), ui->editPort->text().toInt());
}

void MainWindow::on_btnDisconnect_clicked()
{
    emit disconnectSignal();
}

void MainWindow::connected()
{
    printMessage("Устройство подключено");
    ui->btnGetConfig->setEnabled(true);
    ui->btnSetConfig->setEnabled(true);
    ui->sendButton->setEnabled(true);
    ui->btnOnOff->setEnabled(true);
}

void MainWindow::disconnected()
{
    printMessage("Устройство отключено");
    ui->btnGetConfig->setEnabled(false);
    ui->btnSetConfig->setEnabled(false);
    ui->sendButton->setEnabled(false);
    ui->btnOnOff->setEnabled(false);
}

void MainWindow::on_btnStart_clicked()
{
    work = new WorkerThread(this);
    //connect(this, &MainWindow::senddata, work->soc, &SocketControl::senddata);
    //connect(this, &MainWindow::connectSignal, work->soc, &SocketControl::connectSlot);
    //connect(work->soc, &SocketControl::receivedata , this, &MainWindow::receivedata);

    work->mainptr = this;
    work->start();
}

void MainWindow::on_sendButton_clicked()
{
    QByteArray block;
    getInterface(&block);

    QString s = ui->writeDataPlain->toPlainText();

    block.append(s.toStdString().c_str());
    qDebug() << "block = " << block << "size  = " << block.size();

    if(curr_iface != nullptr)
        curr_iface->data_w.append(block);

    qDebug() << "block = " << block << "size  = " << block.size();

    emit senddata(block);
}

void MainWindow::getDataSendSlot(QByteArray arr)
{
    uint16_t len = arr[3] << 8 | arr[4];
    if((uint8_t)arr[2] == 0x02)
        printMessage("По интерфейсу "+getTextInterface(arr[0], arr[1])+" данные отправлены успешно. Длина = " + QString::number(len));
    else
        printMessage("По интерфейсу "+getTextInterface(arr[0], arr[1])+" ошибка отправки данных");
}

void MainWindow::on_MainWindow_destroyed()
{

}

void MainWindow::on_btnGetConfig_clicked()
{
    for(auto it = interfaces.begin(); it != interfaces.end(); it++)
    {
        delete *it;
    }
    interfaces.clear();
    ui->comboInterface->clear();
    ui->btnGetConfig->setEnabled(false);

    emit getInterfaceSignal();
}

void MainWindow::getInterfaceSlot(QByteArray inter)
{
    //emit getInterfaceSignal();
    qDebug() << "inter = " << inter << "size  = " << inter.size();

    ui->btnGetConfig->setEnabled(true);

    for(int i = 0; i < inter[0]; i++)
    {
        Interface* iface = new Interface();

        iface->interface_type = inter[1+2*i];
        iface->interface_num = inter[1+2*i+1];
        interfaces.push_back(iface);

        QString nameas = getTextInterface(iface);

        if(ui->comboInterface == nullptr)
        {
            ui->comboInterface = new QComboBox(this);
            connect(ui->comboInterface,SIGNAL(currentIndexChanged(const QString&)),
                    this,SLOT(on_comboInterface_currentIndexChanged(const QString&)));
            curr_iface = iface;
        }
        else
            ui->comboInterface->addItem(nameas);
        iface->intface_name = nameas;
    }

    printMessage("Доступные интерфейсы считаны");
    qDebug() << "inter = " << inter << "size  = " << inter.size();
}

void MainWindow::on_btnOnOff_clicked()
{
    QByteArray ctrl;

    getInterface(&ctrl);

    if(ui->btnOnOff->text() == "Включить"){
        ctrl.append(0x01);
    }
    else if(ui->btnOnOff->text() == "Выключить"){
        ctrl.append(0x02);
    }
    ui->btnOnOff->setEnabled(false);
    qDebug() << "ctrl = " << ctrl << "size  = " << ctrl.size();
    emit setControlSignal(ctrl);
}

void MainWindow::on_btnSetConfig_clicked()
{
    QByteArray config;
    getInterface(&config);
    getConfigUART(&config);

    qDebug() << "config = " << config << "size  = " << config.size();
    ui->btnSetConfig->setEnabled(false);

    emit setConfigSignal(config);
}

void MainWindow::getConfigSlot(QByteArray config)
{
    qDebug() << "config = " << config << "size  = " << config.size();
    ui->btnSetConfig->setEnabled(true);
    getTextInterface(config[0], config[1]);
    printMessage("Настройки интерфейса "+getTextInterface(config[0], config[1])+" установлены");
}

void MainWindow::getControlSlot(QByteArray ctrl)
{
    qDebug() << "ctrl = " << ctrl << "size  = " << ctrl.size();

    ui->btnOnOff->setEnabled(true);
    if(ui->btnOnOff->text() == "Включить" && (uint8_t)ctrl[2] == 0x02){
        ui->btnOnOff->setText("Выключить");
        printMessage("Интерфейс "+getTextInterface(ctrl[0], ctrl[1])+" включен");
    }
    else if(ui->btnOnOff->text() == "Выключить" && (uint8_t)ctrl[2] == 0x01){
        ui->btnOnOff->setText("Включить");
        printMessage("Интерфейс "+getTextInterface(ctrl[0], ctrl[1])+" выключен");
    }
}

void MainWindow::getInterface(QByteArray* interface)
{
    //uart interfaces
    qDebug() << "getInterface = ";
    if(ui->comboInterface->currentText() == "UART1")
    {
        interface->append(0x01);
        interface->append(0x01);
    }
    else if(ui->comboInterface->currentText() == "UART2")
    {
        interface->append(0x01);
        interface->append(0x02);
    }
    else if(ui->comboInterface->currentText() == "UART3")
    {
        interface->append(0x01);
        interface->append(0x03);
    }
    else if(ui->comboInterface->currentText() == "UART4")
    {
        interface->append(0x01);
        interface->append(0x04);
    }
    else if(ui->comboInterface->currentText() == "UART5")
    {
        interface->append(0x01);
        interface->append(0x05);
    }

    //spi interfaces
    else if(ui->comboInterface->currentText() == "SPI1")
    {
        interface->append(0x02);
        interface->append(0x01);
    }
    else if(ui->comboInterface->currentText() == "SPI2")
    {
        interface->append(0x02);
        interface->append(0x02);
    }
    else if(ui->comboInterface->currentText() == "SPI3")
    {
        interface->append(0x02);
        interface->append(0x03);
    }

}

QString MainWindow::getTextInterface(Interface* iface)
{
    //uart interfaces
    qDebug() << "getTextInterface = ";
    QString nameas = "";

    if(iface->interface_type == 0x01)
    {
        nameas += "UART";
    }
    else if(iface->interface_type == 0x02)
    {
        nameas += "SPI";
    }

    nameas += QString::number(iface->interface_num);

    return nameas;
}

QString MainWindow::getTextInterface(uchar type, uchar num)
{
    //uart interfaces
    qDebug() << "getTextInterface = ";
    QString nameas = "";

    if(type == 0x01)
    {
        nameas += "UART";
    }
    else if(type == 0x02)
    {
        nameas += "SPI";
    }

    nameas += QString::number(num);

    return nameas;
}

void MainWindow::getConfigUART(QByteArray* config)
{
    qDebug() << "getConfigUART = ";
    //baude rate config
    if(ui->comboBaude->currentText() == "9600")
    {
        config->append(0x01);
    }
    else if(ui->comboBaude->currentText() == "19200")
    {
        config->append(0x02);
    }
    else if(ui->comboBaude->currentText() == "38400")
    {
        config->append(0x03);
    }
    else if(ui->comboBaude->currentText() == "57600")
    {
        config->append(0x04);
    }
    else if(ui->comboBaude->currentText() == "115200")
    {
        config->append(0x05);
    }

    //data bit config
    if(ui->comboBit->currentText() == "7")
    {
        config->append(0x07);
    }
    else if(ui->comboBit->currentText() == "8")
    {
        config->append(0x08);
    }
    else if(ui->comboBit->currentText() == "9")
    {
        config->append(0x09);
    }

    //stop bit config
    if(ui->comboStopBit->currentText() == "0.5")
    {
        config->append(0x01);
    }
    else if(ui->comboStopBit->currentText() == "1")
    {
        config->append(0x02);
    }
    else if(ui->comboStopBit->currentText() == "1.5")
    {
        config->append(0x03);
    }
    else if(ui->comboStopBit->currentText() == "2")
    {
        config->append(0x04);
    }

    //parity bit config
    if(ui->comboParity->currentText() == "Да")
    {
        config->append(0x01);
    }
    else if(ui->comboParity->currentText() == "Нет")
    {
        config->append(0x02);
    }

    //parity type config
    if(ui->comboParityType->currentText() == "Чет")
    {
        config->append(0x02);
    }
    else if(ui->comboParityType->currentText() == "Нечет")
    {
        config->append(0x01);
    }

    //port num config
    if(ui->comboPort->currentText() == "Порт1")
    {
        config->append(0x01);
    }
    else if(ui->comboPort->currentText() == "Порт2")
    {
        config->append(0x02);
    }
    else if(ui->comboPort->currentText() == "Порт3")
    {
        config->append(0x03);
    }
    else if(ui->comboPort->currentText() == "Порт4")
    {
        config->append(0x04);
    }

    qDebug() << "config = " << config->toHex();
}

void MainWindow::on_comboInterface_currentIndexChanged(const QString &arg1)
{
    QByteArray intef;
    getInterface(&intef);

    printMessage("Переключение на " +arg1);


    for(auto it = interfaces.begin(); it != interfaces.end(); it++)
    {
        if((*it)->interface_type == intef[0] && (*it)->interface_num == intef[1])
        {
            curr_iface = *it;

            ui->readDataPlain->clear();
            ui->readDataPlain->setPlainText(curr_iface->data_r);

            ui->writeDataPlain->clear();
            ui->writeDataPlain->setPlainText(curr_iface->data_w);
        }
    }

    if(curr_iface != nullptr)
    {
        if(curr_iface->interface_type != intef[0] || curr_iface->interface_num != intef[1])
        {
            printMessage("Переключение c интерфейса = "+curr_iface->intface_name + "на " +arg1);

            for(auto it = interfaces.begin(); it != interfaces.end(); it++)
            {
                if((*it)->interface_type == intef[0] && (*it)->interface_num == intef[1])
                {
                    curr_iface = *it;

                    ui->readDataPlain->clear();
                    ui->readDataPlain->setPlainText(curr_iface->data_r);

                    ui->writeDataPlain->clear();
                    ui->writeDataPlain->setPlainText(curr_iface->data_w);
                }
            }
        }
    }
}


void MainWindow::on_btnClearRecv_clicked()
{
    if(curr_iface != nullptr)
    {
        curr_iface->data_r.clear();
        ui->readDataPlain->clear();
    }
}

void MainWindow::on_btnClearSend_clicked()
{
    if(curr_iface != nullptr)
    {
        curr_iface->data_w.clear();
        ui->writeDataPlain->clear();
    }
}

void MainWindow::printMessage(QString message)
{
    QString time_mark = QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss");
    //ui->message->setPlainText(ui->message->toPlainText()+'\n'+time_mark+" -- "+message);
    ui->message->insertPlainText('\n'+time_mark+" -- "+message);
}

