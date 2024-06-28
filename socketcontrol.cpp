#include "socketcontrol.h"
#include <QDebug>

//defined protocol start bytes
#define PROTO_START1 0x44
#define PROTO_START2 0x55


//define packet protocol type
#define ETHERNET_TYPE 0x20
#define GET_INTERFACE_TYPE 0x23
#define GET_INTERFACE_SETTING_TYPE 0x24
#define GET_INTERFACE_WORK_TYPE 0x25

//define unknow packet code
#define UNKNOW_TYPE 0xDF
#define UNKNOW_INTERFACE 0x46

//define send packet buffer size
#define SEND_BUFF_SIZE 0x0FFF

//define interface type
#define USART_TYPE 0x01
#define SPI_TYPE 0x02
#define I2C_TYPE 0x03
#define CAN_TYPE 0x04
#define GPIO_TYPE 0x05
#define PWM_TYPE 0x06

//define interface type
#define CONFIG_PACK 0x01
#define CTRL_PACK 0x02
#define DATA_WRDATA_PACK 0x03
#define DATA_GETD_PACK 0x04
#define DATA_RDATA_PACK 0x05

//define return value
#define CONFIG_OK 0x02
#define CONFIG_ERR 0x01

#define CTRL_ON 0x02
#define CTRL_OFF 0x01

#define SEND_OK 0x02
#define SEND_ERR 0x01

#define READ_OK 0x02
#define READ_ERR 0x01


SocketControl::SocketControl()
{
    cSocket = new QTcpSocket(nullptr);
    QObject::connect(cSocket, &QTcpSocket::connected, this, &SocketControl::connected);
    QObject::connect(cSocket, &QTcpSocket::disconnected, this, &SocketControl::disconnected);
    QObject::connect(cSocket, &QTcpSocket::readyRead, this, &SocketControl::readdata);
}

void SocketControl::connect()
{
    cSocket->connectToHost(ip, port);
    if(cSocket->waitForConnected(3000)){
        //connect(cSocket, &QIODevice::readyRead, this, &WorkerThread::readSocket);
        //socketDescriptor = cSocket->socketDescriptor();
        qDebug() << "connected ok";
    }
    else{
        qDebug() << "not connected err";
    }
}


void SocketControl::writeData(QByteArray arr)
{
    w_arr = arr;

    //w_arr.push_back("$KE,REL,1,1");
    //DU%

    cSocket->write(w_arr);
    if(cSocket->waitForBytesWritten(500)){
        qDebug() << "wite data";
    }
    else{
        qDebug() << "wite data error";
    }
}

void SocketControl::readData()
{
    //if(cSocket->waitForReadyRead(500)){
        qDebug() << "socket readd: " << cSocket->bytesAvailable();
        if(cSocket->bytesAvailable() > 0)
        {
            r_arr.clear();
            r_arr.append(cSocket->readAll());

            //qDebug() << QString(r_arr);

            //if(QString(r_arr) == ""){
            //    int i = 0;
            //}


            /*QByteArray arr;
            for(int i = 3; i < r_arr.size(); i++){
                arr.append(r_arr[i]);
            }*/

            emit getInterfaceSignal(r_arr);

            qDebug() << "sadasdasd protocol start = " << (uint8_t)r_arr[0] << "ASDa = " << (uint8_t)r_arr[1];
            if((uint8_t)r_arr[0] == PROTO_START1 && (uint8_t) r_arr[1] == PROTO_START2)
            {
                qDebug() << "sadasdasd protocol start";
                if((uint8_t)r_arr[2] == ETHERNET_TYPE
                        && cmd_code == ETHERNET_TYPE)
                {

                }
                else if((uint8_t)r_arr[2] == GET_INTERFACE_TYPE
                        && cmd_code == GET_INTERFACE_TYPE)
                {
                    qDebug() << "sadasdasd protocol packet";
                    QByteArray arr;
                    for(int i = 3; i < r_arr.size(); i++){
                        arr.append(r_arr[i]);
                    }

                    emit getInterfaceSignal(arr);
                }
                else if((uint8_t)r_arr[2] == GET_INTERFACE_SETTING_TYPE
                        && cmd_code == GET_INTERFACE_SETTING_TYPE)
                {

                }
                else if((uint8_t)r_arr[2] == GET_INTERFACE_WORK_TYPE
                        && cmd_code == GET_INTERFACE_WORK_TYPE)
                {
                    emit receivedata(r_arr);
                }
                else
                {

                }

                //emit receivedata(r_arr);


                qDebug() << "protocol packet";
                //emit receivedata(r_arr);

                //emit receivedata(r_arr);
            }
            else
            {
                qDebug() << "not protocol packet";
            }
        }
        else{
            qDebug() << "bytesAvailable() = 0";
        }
    //}
}

void SocketControl::senddataSlot(QByteArray arr)
{
    QByteArray arr1;

    arr1.append(PROTO_START1);
    arr1.append(PROTO_START2);
    arr1.append(GET_INTERFACE_WORK_TYPE);
    arr1.append(DATA_WRDATA_PACK);

    uint16_t len = arr.size();
    arr1.append(len);

    arr1.append(arr);

    cmd_code = GET_INTERFACE_WORK_TYPE;
    pack_code = DATA_WRDATA_PACK;

    writeData(arr1);
}

void SocketControl::connected()
{
    qDebug() << "connected socket connection";
    emit connectedSignal();
}

void SocketControl::disconnected()
{
    qDebug() << "disconnect socket connection";
    emit disconnectedSignal();
    /*QObject::disconnect(cSocket, &QTcpSocket::connected, this, &SocketControl::connected);
    QObject::disconnect(cSocket, &QTcpSocket::disconnected, this, &SocketControl::disconnected);
    QObject::disconnect(cSocket, &QTcpSocket::readyRead, this, &SocketControl::readdata);
    delete cSocket;*/
}

void SocketControl::readdata()
{
    qDebug() << "socket readd: " << cSocket->bytesAvailable();
    if(cSocket->bytesAvailable() > 0)
    {
        r_arr.clear();
        r_arr.append(cSocket->readAll());
        qDebug() << QString(r_arr);
        /*if(QString(r_arr) == "#REL,OK"){
            int i = 0;
        }*/

        //qDebug() << QString(r_arr);

        //if(QString(r_arr) == ""){
        //    int i = 0;
        //}


        /*QByteArray arr;
        for(int i = 3; i < r_arr.size(); i++){
            arr.append(r_arr[i]);
        }*/

        //emit getInterfaceSignal(r_arr);

        qDebug() << "sadasdasd protocol start = " << (uint8_t)r_arr[0] << "ASDa = " << (uint8_t)r_arr[1];
        if((uint8_t)r_arr[0] == PROTO_START1 && (uint8_t) r_arr[1] == PROTO_START2)
        {
            qDebug() << "sadasdasd protocol start";
            if((uint8_t)r_arr[2] == ETHERNET_TYPE
                    && cmd_code == ETHERNET_TYPE)
            {

            }
            else if((uint8_t)r_arr[2] == GET_INTERFACE_TYPE
                    && cmd_code == GET_INTERFACE_TYPE)
            {
                qDebug() << "GET_INTERFACE_TYPE protocol packet";
                QByteArray arr;
                for(int i = 3; i < r_arr.size(); i++){
                    arr.append(r_arr[i]);
                }

                emit getInterfaceSignal(arr);
            }
            else if((uint8_t)r_arr[2] == GET_INTERFACE_SETTING_TYPE
                    && cmd_code == GET_INTERFACE_SETTING_TYPE)
            {

            }
            else if((uint8_t)r_arr[2] == GET_INTERFACE_WORK_TYPE
                    && cmd_code == GET_INTERFACE_WORK_TYPE)
            {
                qDebug() << "GET_INTERFACE_WORK_TYPE protocol packet";
                if((uint8_t)r_arr[3] == CONFIG_PACK
                        && pack_code == CONFIG_PACK)
                {
                    qDebug() << "CONFIG_PACK protocol packet";
                    QByteArray arr;

                    for(int i = 4; i < r_arr.size(); i++){
                        arr.append(r_arr[i]);
                    }

                    emit getConfigSignal(arr);
                }
                else if((uint8_t)r_arr[3] == CTRL_PACK
                        && pack_code == CTRL_PACK)
                {
                    qDebug() << "CTRL_PACK protocol packet";
                    QByteArray arr;

                    for(int i = 4; i < r_arr.size(); i++){
                        arr.append(r_arr[i]);
                    }

                    emit getControlSignal(arr);
                }
                else if((uint8_t)r_arr[3] == DATA_WRDATA_PACK
                        && pack_code == DATA_WRDATA_PACK)
                {
                    qDebug() << "DATA_WRDATA_PACK protocol packet";
                    QByteArray arr;

                    for(int i = 4; i < r_arr.size(); i++){
                        arr.append(r_arr[i]);
                    }

                    emit getDataSendSignal(arr);
                }
                else if((uint8_t)r_arr[3] == DATA_GETD_PACK
                        && pack_code == DATA_GETD_PACK)
                {
                    qDebug() << "DATA_GETD_PACK protocol packet";
                    QByteArray arr;

                    for(int i = 4; i < r_arr.size(); i++){
                        arr.append(r_arr[i]);
                    }

                    emit getDataSignal(arr);
                }
                else if((uint8_t)r_arr[3] == DATA_RDATA_PACK)
                        //&& pack_code == DATA_RDATA_PACK)
                {
                    qDebug() << "DATA_RDATA_PACK protocol packet";
                    QByteArray arr;

                    for(int i = 4; i < r_arr.size(); i++){
                        arr.append(r_arr[i]);
                    }

                    emit getDataSignal(arr);
                }

            }
            else
            {

            }

            //emit receivedata(r_arr);


            qDebug() << "protocol packet";
            //emit receivedata(r_arr);

            //emit receivedata(r_arr);
        }
        else
        {
            qDebug() << "not protocol packet";
        }
    }
    else{
        qDebug() << "bytesAvailable() = 0";
    }
}

void SocketControl::connectSlot(QString ip, quint16 port)
{
    this->ip = ip;
    this->port = port;
    connect();
}

void SocketControl::checkconnect()
{

}

void SocketControl::disconnectSlot()
{
    cSocket->disconnectFromHost();
}

//periph proto handlers

void SocketControl::getInterfaceSlot()
{
    QByteArray arr;

    arr.append(PROTO_START1);
    arr.append(PROTO_START2);

    arr.append(GET_INTERFACE_TYPE);

    cmd_code = GET_INTERFACE_TYPE;

    writeData(arr);

    qDebug() << "bytesAvailable() = 0";
}

void SocketControl::getConfigSlot()
{
    QByteArray arr;

    arr.push_front(GET_INTERFACE_SETTING_TYPE);
    arr.push_front(PROTO_START2);
    arr.push_front(PROTO_START1);

    cmd_code = GET_INTERFACE_SETTING_TYPE;

    writeData(arr);
}

void SocketControl::setConfigSlot(QByteArray config)
{
    QByteArray arr;

    arr.append(PROTO_START1);
    arr.append(PROTO_START2);
    arr.append(GET_INTERFACE_WORK_TYPE);
    arr.append(CONFIG_PACK);

    arr.append(config);

    cmd_code = GET_INTERFACE_WORK_TYPE;
    pack_code = CONFIG_PACK;

    writeData(arr);
}

void SocketControl::setControlSlot(QByteArray ctrl)
{
    QByteArray arr;

    arr.append(PROTO_START1);
    arr.append(PROTO_START2);
    arr.append(GET_INTERFACE_WORK_TYPE);
    arr.append(ctrl);

    cmd_code = GET_INTERFACE_WORK_TYPE;
    pack_code = CTRL_PACK;

    writeData(arr);
}
