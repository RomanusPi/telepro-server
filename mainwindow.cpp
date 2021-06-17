#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    ui->setupUi(this);
    //QObject::connect(ui->pushButton,&QPushButton::clicked,this,&MainWindow::createButtons);
    nDate= QDate::currentDate();


    //print about current day
    strDate=QDate::currentDate().toString("yyyy/M/dd");
    ui->label_2->setText(strDate);
    if (DateMap[QDate::currentDate().toString("yyyy/M/dd")].isReserved==false){
            ui->label_3->setText("Termin możliwy do rejestracji");
    }
    else { ui->label_3->setText("Termin został zarezerwowany");}

    QDir cat;
    FilePath=cat.currentPath();//take current directory


   //sms
    ui->lineEdit->setText(strDate +" rezerwuje" );
    SmsR.smstext=ui->lineEdit->text();
    //tcp
    client = new ClientStuff("localhost", 6547);//ip and port server

    setStatus(client->getStatus());

    connect(client, &ClientStuff::hasReadSome, this, &MainWindow::receivedSomething);//if server sent something
    connect(client, &ClientStuff::statusChanged, this, &MainWindow::setStatus);//if server change the status

    connect(client->tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
          this, SLOT(gotError(QAbstractSocket::SocketError)));
}



MainWindow::~MainWindow()
{

    delete ui;
    delete client;//tcp
}

void MainWindow::on_calendarWidget_selectionChanged()
{

   ui->label_4->setText("");
   nDate=ui->calendarWidget->selectedDate();//nDate for selected on calender day
   strDate=nDate.toString("yyyy/M/dd");

   if (!(DateMap.contains(strDate))||(DateMap[strDate].isReserved==false)){
           ui->label_3->setText("Termin możliwy do rejestracji");
   }
   else { ui->label_3->setText("Termin został zarezerwowany");}

   QMapIterator<QString,rdata>Iter(DateMap);
   while(Iter.hasNext()){ Iter.next(); qDebug()<<Iter.key()<<" "<<Iter.value().isReserved;};

   ui->label_2->setText(strDate);

   client->send_to_server("isfree@"+strDate);
}
//----

void MainWindow::createButtons(){//used in the future
    qDebug()<<"create";
  
  /*  QVBoxLayout *layout=new QVBoxLayout();
   layout=ui->verticalLayout;
    QString buttontext=tr("Buttont: #%1").arg(layout->count());
    QPushButton *przyc =new QPushButton(buttontext, ui->frame_2);
    layout->insertWidget(0,przyc);
    przyc->show();
    QObject::connect( przyc,&QPushButton::clicked,this,&MainWindow::onRemoveWidget);
//zrobic layout nf veritivcal i pole textowe

*/

}
void MainWindow::onRemoveWidget(){//use in the future

    QPushButton* przyc =  qobject_cast<QPushButton*>(sender());
    delete przyc;

}

void MainWindow::on_pushButton_clicked()
{

    if (DateMap[strDate].isReserved==false){
        DateMap[strDate].isReserved=true;
        ui->label_4->setText("Zarezerwowales ten termin");
    }
    else { ui->label_4->setText("Nie możesz ponownie zarezerwować");}

   client->send_to_server("reserve@"+strDate);

}

void MainWindow::on_pushButton_2_clicked()
{

    if (DateMap[strDate].isReserved==true){
      DateMap[strDate].isReserved=false;
      ui->label_4->setText("Odwołałeś ten termin");
    }
    else { ui->label_4->setText("Nie masz jeszcze rezerwacji ");}

    client->send_to_server("release@"+strDate);
}

void MainWindow::on_commandLinkButton_clicked()//button with months
{

    arrowDate=nDate.addMonths(1);//one month later
    ui->calendarWidget->setSelectedDate(arrowDate);
    QString strMonth=arrowDate.toString("M");
    bool ok;
    int k = strMonth.toInt(&ok);
    if (ok ==false) {qDebug()<<"notconverted";}
    strMonth= QDate::longMonthName(k);
    ui->label->setText(strMonth);
}
void MainWindow::on_commandLinkButton_2_clicked()
{

    arrowDate=nDate.addMonths(-1);//one month earlier
    ui->calendarWidget->setSelectedDate(arrowDate);
    QString strMonth=arrowDate.toString("M");
    bool ok;
    int k = strMonth.toInt(&ok);
    if (ok ==false) {qDebug()<<"notconverted";}
    strMonth= QDate::longMonthName(k);
    ui->label->setText(strMonth);
    //MainWindow::createButtons();//use later
}






void MainWindow::on_pushButton_4_clicked()
{

 SmsR.smstext=ui->lineEdit->text();
 QString strDate= SmsR.smstext.left(9);
 QString Rezor = SmsR.smstext.mid(10,17);
 if(Rezor=="rezerwuje"){

     if (DateMap[strDate].isReserved==false){
         DateMap[strDate].isReserved=true;
         ui->label_4->setText("Zarezerwowales ten termin");
     }
     else { ui->label_4->setText("Nie możesz ponownie zarezerwować");}
  client->send_to_server("reserve@"+strDate);
 }
 if(Rezor=="odwołuje"){
     if (DateMap[strDate].isReserved==true)
     {
       DateMap[strDate].isReserved=false;
       ui->label_4->setText("Odwołałeś ten termin");
     }
     else { ui->label_4->setText("Nie masz jeszcze rezerwacji ");}
     client->send_to_server("release@"+strDate);
 }


 qDebug()<<strDate<<Rezor;
}


//tcp
void MainWindow::setStatus(bool newStatus)
{
    if(newStatus)
    {
        ui->label_status->setText(
                    tr("<font color=\"green\">CONNECTED</font>"));
        ui->pushButton_connect->setVisible(false);
        ui->pushButton_disconnect->setVisible(true);
    }
    else
    {
        ui->label_status->setText(
                tr("<font color=\"red\">DISCONNECTED</font>"));
        ui->pushButton_connect->setVisible(true);
        ui->pushButton_disconnect->setVisible(false);
    }
}

void MainWindow::receivedSomething(QString Msgs)
{


    ui->textEdit_log->append("otrzymalem: "+Msgs);
    qDebug()<<Msgs<<"::";


   QString Order="",Date="";
   bool NextInfo=false;
   for(int i=0;i<Msgs.length();i++){
        if (Msgs[i] !='@' && !NextInfo){
           Order+=Msgs[i];
          // qDebug()<<":"<<Msgs[i];
           continue;
        }
        if(Msgs[i]=='@'){ NextInfo=true;continue;}
        Date+=Msgs[i];

    }
        qDebug()<<Order<<Date;


   if(Order=="notreserved"){
       ui->label_3->setText("Server mowi: Termin możliwy do rejestracji");
       ui->label_2->setText(Date);
   }
   if(Order=="isreserved" ) {
       ui->label_3->setText("Server mowi: Termin został zarezerwowany");
       ui->label_2->setText(Date);
   }

   if(Order=="Ireserved"){
       ui->label_3->setText("Server mowi: Zarezerwowałes ten termin");
       ui->label_2->setText(Date);
   }
   if(Order=="Inotreserved" ) {
       ui->label_3->setText("Server mowi: Termin został juz wczesniej zarezerwowany");
       ui->label_2->setText(Date);
        //add during a time reserved
   }

   if(Order=="isreleased"){
       ui->label_3->setText("Server mowi: Odwołałes ten termin");
       ui->label_2->setText(Date);
   }
   if(Order=="isnotreleased" ) {
       ui->label_3->setText("Server mowi: Nie było rezerwacji");
       ui->label_2->setText(Date);
       //add during a time reserved
   }


   if (Order!="notreserved"&& Order!="isreserved"&&Order!="Ireserved"&& Order!="Inotreserved"&&Order!="isnotreleased"&& Order!="isreleased")
       qDebug()<<"don't now answer";

}

void MainWindow::gotError(QAbstractSocket::SocketError err)
{
    //qDebug() << "got error";
    QString strError = "unknown";
    switch (err)
    {
        case 0:
            strError = "Connection was refused";
            break;
        case 1:
            strError = "Remote host closed the connection";
            break;
        case 2:
            strError = "Host address was not found";
            break;
        case 5:
            strError = "Connection timed out";
            break;
        default:
            strError = "Unknown error";
    }

    ui->textEdit_log->append(strError);
}


void MainWindow::on_pushButton_connect_clicked()
{
     client->connect2host();
}

void MainWindow::on_pushButton_disconnect_clicked()
{
    client->closeConnection();
}


void MainWindow::on_pushButton_send_clicked()
{   QString as="as";
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    //out.setVersion(QDataStream::Qt_5_10);
    out << quint16(0) << ui->lineEdit_message->text();


    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    client->tcpSocket->write(arrBlock);

    //same as
   // client->send_to_server(ui->lineEdit_message->text());
}



