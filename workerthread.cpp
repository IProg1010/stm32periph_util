#include "workerthread.h"
#include <iostream>
#include "socketcontrol.h"
#include "mainwindow.h"

WorkerThread::WorkerThread(QObject *parent)
{

}

void WorkerThread::run()
{

    /*QTcpSocket* cSocket = new QTcpSocket(nullptr);

    cSocket->connectToHost("192.168.1.34", 47000);
    if(cSocket->waitForConnected(1000)){
        //connect(cSocket, &QIODevice::readyRead, this, &WorkerThread::readSocket);
        //socketDescriptor = cSocket->socketDescriptor();
        qDebug() << "connected = 0";
    }
    else{
        qDebug() << "not connected = 0";
    }

    bool rep = true;
    QByteArray block;
    int i = 0;
    while(rep)
    {
        while(i < 1){
            block.clear();

            block.push_back("$KE,REL,1,1");

            cSocket->write(block);
            if(cSocket->waitForBytesWritten(50)){
                qDebug() << "wite data";
            }
            else{
                qDebug() << "wite data error";
            }

            i++;
        }

        if(cSocket->waitForReadyRead(500)){
            qDebug() << "socket readd: " << cSocket->bytesAvailable();
            if(cSocket->bytesAvailable() > 0)
            {
                block.clear();
                block.append(cSocket->readAll());
                qDebug() << QString(block);
                if(QString(block) == "#REL,OK"){
                    i = 0;
                }
            }
            else{
                qDebug() << "bytesAvailable() = 0";
            }
        }
        //rep = false;
    }*/

    SocketControl* soc = new SocketControl();
    QObject::connect(mainptr, &MainWindow::connectSignal, soc, &SocketControl::connectSlot);
    QObject::connect(soc, &SocketControl::connectedSignal , mainptr, &MainWindow::connected);


    QObject::connect(mainptr, &MainWindow::disconnectSignal, soc, &SocketControl::disconnectSlot);
    QObject::connect(soc, &SocketControl::disconnectedSignal , mainptr, &MainWindow::disconnected);

    QObject::connect(mainptr, &MainWindow::senddata, soc, &SocketControl::senddataSlot);
    QObject::connect(soc, &SocketControl::getDataSendSignal , mainptr, &MainWindow::getDataSendSlot);

    QObject::connect(mainptr, &MainWindow::getInterfaceSignal, soc, &SocketControl::getInterfaceSlot);
    QObject::connect(soc, &SocketControl::getInterfaceSignal , mainptr, &MainWindow::getInterfaceSlot);

    //QObject::connect(mainptr, &MainWindow::getConfigSignal, soc, &SocketControl::getConfigSlot);
    QObject::connect(mainptr, &MainWindow::setConfigSignal, soc, &SocketControl::setConfigSlot);
    QObject::connect(soc, &SocketControl::getConfigSignal , mainptr, &MainWindow::getConfigSlot);

    QObject::connect(mainptr, &MainWindow::setControlSignal, soc, &SocketControl::setControlSlot);
    QObject::connect(soc, &SocketControl::getControlSignal , mainptr, &MainWindow::getControlSlot);

    QObject::connect(soc, &SocketControl::getDataSignal, mainptr, &MainWindow::receivedata);

    QObject::connect(soc, &SocketControl::receivedata , mainptr, &MainWindow::receivedata);

    //delete soc;
    exec();
}
void WorkerThread::senddata(char* ch, int size){
    /*if(cSocket->state() == QAbstractSocket::ConnectedState)
    {
        cSocket->write(ch, size);
        cSocket->waitForBytesWritten();

        emit message("Write data is ok");
    }*/
}
void WorkerThread::readSocket()
{
    //emit message("ready data");
    qDebug() << QString(m_dataread);
    //emit receivedata(m_dataread);
}
