#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include "clientsettings.h"
#include <QNetworkInterface>
#include <QByteArray>
#include "boardlabel.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::stop_and_renew_timer() {
    stop_game_timer();
    set_timer_init();
}

void MainWindow::timer_second_up_check() {
    game_timer->deleteLater();
    game_timer = nullptr;
    --time_left;
    emit time_update(time_left);

    if(time_left == 0) {
        if(Piece::our_tern){
            ++Piece::our_timeout_cnt;

            if(Piece::our_timeout_cnt == 3){
                sck_write(QString("S").toUtf8());
                Piece::end("You lose!\n You run out of time for 3 times.", "You lose!  |  You run out of time for 3 times.");
            } else {
                information("<font color=\"red\"><b>YOUR TIME RUN OUT!&nbsp;</b></font><font color=\"black\"><b>Now it's you opponent's turn.</b></font>");
                sck_write(QString("T").toUtf8());
                Piece::turn_switch(false);
                set_timer_init();
            }
        } else {
            other_possibly_timeout_network_check();
        }
    } else {
        new_one_second();
    }
}

void MainWindow::other_possibly_timeout_network_check() {
    qDebug()<<"Begin syncing network check";
    network_satus_sync_timer = new QTimer(this);
    network_satus_sync_timer->setInterval(1000);
    network_satus_sync_timer->setTimerType(Qt::PreciseTimer);
    connect(network_satus_sync_timer, &QTimer::timeout, [&](){
        sck_disconnected_event();
    });
    network_satus_sync_timer->start();
}

void MainWindow::init() {
    setMinimumSize(size());
    setMaximumSize(size());
    setWindowFlags(Qt::WindowCloseButtonHint);
    ui->actionAdmit_defeat->setEnabled(false);
    ui->actionStart->setEnabled(false);
    ui->actionDisconnect->setEnabled(false);
    information("Click <b>Connect</b> to launch a new connection!", true);
    ui->TimeLeftLCD->hide();
    connect(this, SIGNAL(time_update(int)), ui->TimeLeftLCD, SLOT(display(int)));
    back_ground = new BoardLabel(this);
}

void MainWindow::set_timer_init(){
    if(Piece::ended)
        return;
    time_left = time_out;
    ui->TimeLeftLCD->display(time_left);
    ui->TimeLeftLCD->show();
    new_one_second();
}

void MainWindow::new_one_second(){
    // suppose the memory have been disposed of corectly, so that there's no memory leaking for game_timer
    game_timer = new QTimer(this);
    game_timer->setInterval(1000);
    game_timer->setTimerType(Qt::PreciseTimer);
    connect(game_timer, &QTimer::timeout, this, &MainWindow::timer_second_up_check);
    game_timer->start();
}

void MainWindow::connect_to_server(QString IP, int Port, bool from_server) {
    if(conn != nullptr)
        delete conn;
    conn = new QTcpSocket;
    information("Connecting to server...", true);
    conn->connectToHost(IP, Port);
    connect(conn, &QTcpSocket::connected, [=](){
        qDebug()<<"Client socket connected";
        if(from_server){
            information("Waiting for the other play to join...", true);
        } else {
            information("Server connected!", true);
            ui->actionConnect_to_a_server->setEnabled(false);
            ui->actionCreate_connection_as_a_server->setEnabled(false);
        }
    });
    connect(conn, &QTcpSocket::readyRead, this, &MainWindow::read_dispatcher);
    connect(conn, &QTcpSocket::disconnected, this, &MainWindow::sck_disconnected_event);
}

void MainWindow::sck_disconnected_event() {
    if(!Piece::ended){
        QString msg {"You have disconnect with your partner."};
        if(game_started)
            Piece::end(msg);
        else
        {
            QMessageBox::information(this, "Military Chess", msg);
            information(msg, true);
            disable_all_action();
        }
    }
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
    msg += ".\nDepending on your network environment, possible IP address are:\n";
    auto l = QNetworkInterface::allInterfaces().at(0).allAddresses();
    for(QHostAddress ip : l) {
        if(ip.toIPv4Address()){
            msg += ip.toString() + "\n";
            local_possible_ip = ip.toString();
        }

    }
    server = new Server(9999, this);
    server->init_server();
    connect_to_server(local_possible_ip, server->get_port(), true);
    QMessageBox::information(this, "Create server", msg);
    ui->actionCreate_connection_as_a_server->setEnabled(false);
    ui->actionConnect_to_a_server->setEnabled(false);
}

void MainWindow::read_dispatcher(){
    QString content {conn->readAll()};

    switch(content[0].toLatin1()) {
    case 'A': // A
        Piece::end("You win!\nYour opponent admitted defeat!", "You win!  |  Your opponent admitted defeat!");
        break;

    case 'I': // I01 11\n 0 8\n 1 8\n ...
        init_info = content;
        ui->actionStart->setEnabled(true);
        ui->actionCreate_connection_as_a_server->setDisabled(false);
        information("Now, get <b>ready</b>!", true);
        ui->actionDisconnect->setEnabled(true);
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
        stop_network_sync_timer();
        stop_and_renew_timer();
        information("<font color=\"red\">Opponent time out!</font>");
        Piece::turn_switch(true);
        break;

    case 'S':// S
        stop_game_timer();
        stop_network_sync_timer();
        Piece::end("You win!\nYour opponent run out of time for 3 times.", "You win!  |  Your opponent run out of time for 3 times.");
        break;

    case 'R': // R
        other_ready = true;
        if(ready){
            game_start();
        } else {
            information("Your opponent is ready. Click <b>ready</b> to start the game.", true);
        }
        break;

    case 'D':
        conn->disconnectFromHost();
        break;
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
    //ui->actionAdmit_defeat->setEnabled(true);
    ui->actionStart->setEnabled(false);
    ui->actionConnect_to_a_server->setEnabled(false);
    ui->actionCreate_connection_as_a_server->setEnabled(false);
    set_timer_init();
    game_started = true;
}

void MainWindow::turn(bool our_turn){
    if(our_turn){
        ui->TernLabel->setText("<b>You</b>");
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
    if(Piece::step_cnt >= 20)
        ui->actionAdmit_defeat->setEnabled(true);

    QString msg {"Mine to kill: "};
    msg += QString::number(Piece::opponent_mine_left);
    msg += "   Total steps: ";
    msg += QString::number(Piece::step_cnt);
    if(Piece::our_timeout_cnt != 0)
        msg += "   Timeout count: "+QString::number(Piece::our_timeout_cnt);

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
    conn->write(bytes.data());
    conn->waitForBytesWritten();
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
    ui->actionDisconnect->setEnabled(false);
}


void MainWindow::stop_game_timer(){
    if(game_timer){
        if(game_timer->isActive())
            game_timer->stop();
        game_timer->deleteLater();
        game_timer = nullptr;
    }
}

void MainWindow::stop_network_sync_timer(){
    if(network_satus_sync_timer){
        if(network_satus_sync_timer->isActive())
            network_satus_sync_timer->stop();
        network_satus_sync_timer->deleteLater();
        network_satus_sync_timer = nullptr;
    }
}

void MainWindow::end_game_window_actions(){
    disable_all_action();
    stop_game_timer();
    stop_network_sync_timer();
    ui->TernLabel->hide();
}

void MainWindow::on_actionDisconnect_triggered()
{
    if(conn){
        if(conn->isWritable())sck_write(QString("D").toUtf8());
        conn->disconnectFromHost();
    }
}

