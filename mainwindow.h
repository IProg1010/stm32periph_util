#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "workerthread.h"
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Interface{

    public:
        uint8_t interface_type;
        uint8_t interface_num;
        QString intface_name;
        QByteArray config;
        QByteArray ctrl;
        QByteArray data_r;
        QByteArray data_w;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

     WorkerThread* work;
    signals:
     void connectSignal(QString ip, quint16 port);
     void disconnectSignal();
     void senddata(QByteArray arr);
     void getInterfaceSignal();
     void getConfigSignal();
     void setConfigSignal(QByteArray arr);
     void setControlSignal(QByteArray ctrl);

public slots:
    void receivedata(QByteArray arr);
    void connected();
    void disconnected();
    void message(QString str);
    void getConfigSlot(QByteArray config);
    void getControlSlot(QByteArray ctrl);
    void getInterfaceSlot(QByteArray interface);
    void getDataSendSlot(QByteArray arr);

private slots:

    void on_btnConnect_clicked();
    void on_btnStart_clicked();
    void on_sendButton_clicked();
    void on_MainWindow_destroyed();    
    void on_btnGetConfig_clicked();

    void on_btnDisconnect_clicked();

    void on_btnOnOff_clicked();

    void on_btnSetConfig_clicked();

    void on_comboInterface_currentIndexChanged(const QString &arg1);

    void on_btnClearRecv_clicked();

    void on_btnClearSend_clicked();

private:
    void printMessage(QString message);
    void getConfigUART(QByteArray* config);
    void getConfigSPI(QByteArray* config);
    void getInterface(QByteArray* config);

    QString getTextInterface(Interface* iface);
    QString getTextInterface(uchar type, uchar num);

    QVector<Interface*> interfaces;
    Interface* curr_iface;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
