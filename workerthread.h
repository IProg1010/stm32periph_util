#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>
#include <QtNetwork/QTcpSocket>
#include "socketcontrol.h"

class MainWindow;

class WorkerThread: public QThread
{
    Q_OBJECT
public:
    WorkerThread(QObject *parent = nullptr);

    void run() override;


    signals:
        void receivedata(QByteArray arr);
        void connected(uint8_t count);
        void senddataexecute(QString str);
        void message(QString str);
        void error(QTcpSocket::SocketError socketError);

    public slots:
        void senddata(char* ch, int size);
        void readSocket();

public:
    bool term;
    QByteArray m_dataread;
    MainWindow* mainptr;
    int socketDescriptor;
};

#endif // WORKERTHREAD_H
