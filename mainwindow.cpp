#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include "clientsettings.h"
#include <QNetworkInterface>
#include <QByteArray>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::init() {

}

void MainWindow::connect_to_server(QString IP, int Port) {
    conn = new QTcpSocket;
    information("Connecting to server...");
    conn->connectToHost(IP, Port);
    connect(conn, &QTcpSocket::connected, [&](){
        information("Waiting for the other side to join...");
    });
    connect(conn, &QTcpSocket::readyRead, this, &MainWindow::read_dispatcher);

}

void MainWindow::on_actionConnect_to_a_server_triggered()
{
    auto setting_window = new ClientSettings(this);
    connect(setting_window, &ClientSettings::server_config_done, this, &MainWindow::connect_to_server);
    setting_window->show();
}


void MainWindow::on_actionCreate_connection_as_a_server_triggered()
{
    //setup server
    QString msg {"A TCP server has been setup locally on port "};
    QString local_possible_ip {};
    msg += QString::number(9999);
    msg += ".\n Depending on your network environment, possible IP address are:\n";
    auto l = QNetworkInterface::allInterfaces().at(0).allAddresses();
    qDebug()<<l.size();
    for(QHostAddress ip : l) {
        if(ip.toIPv4Address()){
            msg += ip.toString() + "\n";
            local_possible_ip = ip.toString();
        }

    }
    qDebug()<<"traverse done";
    if(server){
        delete server;
    }
    server = new Server(9999);
    server->init_server();
    connect_to_server(local_possible_ip, server->get_port());
    //QMessageBox::information(this, "Create server", msg);
}

void MainWindow::read_dispatcher(){
    QString content {conn->readAll()};

    qDebug()<<content;

    switch(content[0].toLatin1()) {
    case 'A': // A
        win(true);
        break;

    case 'I': // I01 11\n 0 8\n 1 8\n ...
        qDebug()<<"case I";
        Piece::init_board(content.mid(2), this);
        if(content[1] == QChar('1')) {
            Piece::turn_switch(true);
        } else {
            Piece::turn_switch(false);
        }
        break;

    case 'M':// M11 23     M7 22
        int from, to;
        if(content[2] == QChar(' ')) {
            from = content.mid(1,1).toInt();
            to = content.mid(3).toInt();
        } else {
            from = content.mid(1,2).toInt();
            to = content.mid(4).toInt();
        }
        qDebug()<<"M: from, to"<<from<<to;
        Piece::board[from]->move_to(to, false); // use move_to to update the board configuration
        Piece::turn_switch(true);
        break;

    case 'F':// F54
        Piece::board[content.mid(1).toInt()]->flip(false);
        Piece::turn_switch(true);
        break;

    case 'T':// T
        information("Opponent time out!");
        Piece::turn_switch(true);
        break;

    }
}

void MainWindow::win(bool from_admit) {
    if(from_admit)
        QMessageBox::information(this, "Military Chess", "You win!\nYour opponent admitted failure!");
    else
        QMessageBox::information(this, "Military Chess", "You win!");
}

void MainWindow::lose(bool from_admit) {
    if(from_admit)
        QMessageBox::information(this, "Military Chess", "You admitted defeat!");
    else
        QMessageBox::information(this, "Military Chess", "You lose!");
}

void MainWindow::turn(bool our_turn){
    if(our_turn){
        ui->TernLabel->setText("Now it's your turn!");
    } else {
        ui->TernLabel->setText("Now it's the other player's turn");
    }
}

void MainWindow::our_team_determined(int team) {
    QString msg {"Your Side:\n"};
    if(team == 0)
        ui->TeamLabel->setText(msg + "<font color=\"red\">RED</font>");
    else
        ui->TeamLabel->setText(msg + "<font color=\"blue\">BLUE</font>");
    ui->TeamLabel->setTextFormat(Qt::RichText);
}

void MainWindow::information(QString msg) {
    ui->InfoLabel->setText(msg);
}

void MainWindow::send_move(int from, int to) {
    QString msg {"M"};
    msg += QString::number(from) + " " + QString::number(to);
    sck_write(msg.toUtf8());
}

void MainWindow::send_flip(int loc) {
    QString msg {"F"};
    msg += QString::number(loc);
    sck_write(msg.toUtf8());
}

//obsolete: functions replaced by Piece::turn_switch(bool)
void MainWindow::user_step_done() {
    //cancel the status?? maybe Piece should do this
    //change the words

}

void MainWindow::sck_write(const QByteArray& bytes) {

    qDebug()<<"Client Send" << QString(bytes);
    conn->write(bytes.data());
    conn->waitForBytesWritten();
    qDebug()<<"written!";
}
