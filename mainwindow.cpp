﻿#include "mainwindow.h"
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
    setMinimumSize(size());
    setMaximumSize(size());
    setWindowFlags(Qt::WindowCloseButtonHint);
    ui->actionAdmit_defeat->setEnabled(false);
    ui->actionStart->setEnabled(false);
    information("Click <b>Connect</b> to launch a new connection!", true);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::init() {

}

void MainWindow::connect_to_server(QString IP, int Port, bool from_server) {
    if(conn != nullptr)
        delete conn;
    conn = new QTcpSocket;
    information("Connecting to server...", true);
    conn->connectToHost(IP, Port);
    connect(conn, &QTcpSocket::connected, [=](){
        if(from_server){
            information("Waiting for the other play to join...", true);
        } else {
            information("Server connected!", true);
            ui->actionConnect_to_a_server->setEnabled(false);
            ui->actionCreate_connection_as_a_server->setEnabled(false);
        }
    });
    connect(conn, &QTcpSocket::readyRead, this, &MainWindow::read_dispatcher);
//    if(!from_server) //Crash! Howcome??
//        connect(conn, &QTcpSocket::disconnected, this, &MainWindow::sck_disconnected_event);
}

void MainWindow::sck_disconnected_event() {
    Piece::end("Network failure!\nGame ended.", "Network failure!  |  Game ended.");
}

void MainWindow::on_actionConnect_to_a_server_triggered()
{
    auto setting_window = new ClientSettings(this);
    connect(setting_window, &ClientSettings::server_config_done, this, &MainWindow::connect_to_server);
    setting_window->show();
}


void MainWindow::on_actionCreate_connection_as_a_server_triggered()
{
    if(server){
        delete server;
        server = nullptr;
        ui->actionCreate_connection_as_a_server->setText("Create connection as a server");
        ui->actionConnect_to_a_server->setEnabled(true);
        return;
    }

    //setup server
    QString msg {"A TCP server has been setup locally on port "};
    QString local_possible_ip {};
    msg += QString::number(9999);
    msg += ".\nDepending on your network environment, possible IP address are:\n";
    auto l = QNetworkInterface::allInterfaces().at(0).allAddresses();
    for(QHostAddress ip : l) {
        if(ip.toIPv4Address()){
            msg += ip.toString() + "\n";
            local_possible_ip = ip.toString();
        }

    }
    server = new Server(9999);
    server->init_server();
    connect_to_server(local_possible_ip, server->get_port(), true);
    QMessageBox::information(this, "Create server", msg);
    ui->actionCreate_connection_as_a_server->setText("Disable Server");
    ui->actionConnect_to_a_server->setEnabled(false);
}

void MainWindow::read_dispatcher(){
    QString content {conn->readAll()};

    switch(content[0].toLatin1()) {
    case 'A': // A
        Piece::end("You win!\nYour opponent admitted defeat!", "You win!  |  Your opponent admitted failure!");
        break;

    case 'I': // I01 11\n 0 8\n 1 8\n ...
        init_info = content;
        ui->actionStart->setEnabled(true);
        information("Now, get <b>ready</b>!", true);
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
        information("<font color=\"red\">Opponent time out!</font>");
        Piece::turn_switch(true);
        break;

    case 'R': // R
        other_ready = true;
        if(ready){
            game_start();
        } else {
            information("Your opponent is ready. Click <b>ready</b> to start the game.", true);
        }
    }
}

void MainWindow::game_start(){
    Piece::init_board(init_info.mid(2), this);
    if(init_info[1] == QChar('1')) {
        Piece::turn_switch(true);
    } else {
        Piece::turn_switch(false);
    }
    ui->TeamLabel->setText("Unknown");
    information("Now game begin!");
    ui->actionAdmit_defeat->setEnabled(true);
    ui->actionStart->setEnabled(false);
    ui->actionConnect_to_a_server->setEnabled(false);
    ui->actionCreate_connection_as_a_server->setEnabled(false);
}

void MainWindow::turn(bool our_turn){
    if(our_turn){
        ui->TernLabel->setText("You");
    } else {
        ui->TernLabel->setText("Opponent");
    }
}

void MainWindow::our_team_determined(int team) {
    if(team == 0)
        ui->TeamLabel->setText("<font color=\"red\">RED</font>");
    else
        ui->TeamLabel->setText("<font color=\"blue\">BLUE</font>");
    ui->TeamLabel->setTextFormat(Qt::RichText);
}

void MainWindow::information(QString msg, bool lasting) {
    if(info_show_timer){
        if(info_show_timer->isActive())
            info_show_timer->stop();
        info_show_timer->deleteLater();
    }
    if(lasting){
        info_show_timer = nullptr;
    } else {
        info_show_timer = new QTimer(this);
        connect(info_show_timer, &QTimer::timeout, this, &MainWindow::set_info_default);
        info_show_timer->start(2000);
    }

    ui->InfoLabel->setText(msg);
}

void MainWindow::set_info_default() {
    QString msg {"Mine to kill: "};
    msg += QString::number(Piece::opponent_mine_left);
    msg += "  Total steps: ";
    msg += QString::number(Piece::step_cnt);
    if(Piece::our_timeout_cnt != 0)
        msg += " Timeout count: "+QString::number(Piece::our_timeout_cnt);

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


void MainWindow::sck_write(const QByteArray& bytes) {

    qDebug()<<"Client Send" << QString(bytes);
    conn->write(bytes.data());
    conn->waitForBytesWritten();
    qDebug()<<"written!";
}

void MainWindow::on_actionAdmit_defeat_triggered()
{
    ui->actionAdmit_defeat->setEnabled(false);
    sck_write(QString("A").toUtf8());
    Piece::end("You admitted defeat.");
}


void MainWindow::on_actionStart_triggered()
{
    ready = true;
    ui->actionStart->setEnabled(false);
    sck_write(QString("R").toUtf8());
    if(other_ready){
        game_start();
    } else {
        information("Waiting for you opponent to get ready ...", true);
    }
}

void MainWindow::disable_all_action(){
    ui->actionAdmit_defeat->setEnabled(false);
    ui->actionConnect_to_a_server->setEnabled(false);
    ui->actionCreate_connection_as_a_server->setEnabled(false);
    ui->actionStart->setEnabled(false);
}
