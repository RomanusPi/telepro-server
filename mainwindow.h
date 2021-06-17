#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDate>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCalendarWidget>
#include "data.h"
#include "sms.h"
#include<QDir>
#include"clientStuff.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_commandLinkButton_clicked();
    void on_calendarWidget_selectionChanged();

    void createButtons();//create new buttons (not used yet)
    void onRemoveWidget();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_commandLinkButton_2_clicked();



    void on_pushButton_4_clicked();

    void on_pushButton_connect_clicked();

    void on_pushButton_disconnect_clicked();

    void on_pushButton_send_clicked();

    //void send_to_s(QString msg);

public slots:
    void setStatus(bool newStatus);
    void receivedSomething(QString msg);
    void gotError(QAbstractSocket::SocketError err);

private:
    Ui::MainWindow *ui;
    QDate nDate;//selected date for date oparations
    QDate arrowDate;//date for months from arrows
    QMap<QString,rdata> DateMap;//all termins reserved
    QString strDate;//for save string of qdate
    Sms SmsR;

    QString FilePath="";//string to path of project
   //tcp
    ClientStuff *client;
};

#endif // MAINWINDOW_H


//obsluga bladnej daty
