#ifndef SOCKETCONTROL_H
#define SOCKETCONTROL_H

#include <QtNetwork/QTcpSocket>

class SocketControl:public QObject
{
    Q_OBJECT
public:
    SocketControl();

    void connect();
    void writeData(QByteArray arr);
    void readData();
    void checkconnect();

signals:
    void receivedata(QByteArray arr);
    void receiveconfig(QByteArray arr);
    //void receiveconfig(QByteArray arr);

    void connectedSignal();
    void disconnectedSignal();

    void getInterfaceSignal(QByteArray arr);
    void getConfigSignal(QByteArray arr);
    void getControlSignal(QByteArray arr);
    void getDataSignal(QByteArray arr);
    void getDataSendSignal(QByteArray arr);

public slots:
    void senddataSlot(QByteArray arr);
    void connectSlot(QString ip, quint16 port);
    void disconnectSlot();
    void readdata();
    void connected();
    void disconnected();
    void getConfigSlot();

    void setConfigSlot(QByteArray arr);
    void setControlSlot(QByteArray arr);

    void getInterfaceSlot();
private:
    QByteArray w_arr;
    QByteArray r_arr;
    QString ip;
    int port;
    QTcpSocket* cSocket;

    uint8_t cmd_code;
    uint8_t pack_code;
};

#endif // SOCKETCONTROL_H
