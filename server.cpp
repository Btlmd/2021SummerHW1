#include "server.h"
#include <QTimer>

Server::Server(int Port, QObject *parent) : QObject(parent), port{Port}
{

}

QString Server::random_layout() {
    QString layout {};
    for(int i = 50; i > 0; --i) {
            int rnd = get_rand(i) + 1;
            int num = 0;
            while(rnd > 0){
                if(!is_allocated[num]){
                    rnd--;
                }
                ++num;
            }
            is_allocated[--num] = true;

        layout += QString::number(to_be_allocated_type[num]);
        layout += " ";
        layout += QString::number(to_be_allocated_piec[num]);
        layout += "\n";
    }
    return layout;
}

int Server::get_rand(int moduler) {
    return QRandomGenerator::global()->bounded(moduler);
}

void Server::init_game(){
    //2-player version
    QString msg[2]= {{"I"}, {"I"}};
    int Client_0_first {get_rand(2)};
    if(Client_0_first == 1){
        msg[0] += "1";
        msg[1] += "0";
    } else {
        msg[0] += "0";
        msg[1] += "1";
    }

    auto layout = random_layout();

    msg[0] += layout;
    msg[1] += layout;

    send(0, msg[0].toUtf8());
    send(1, msg[1].toUtf8());
}

void Server::init_server() {
    svr = new QTcpServer(this);
    svr->listen(QHostAddress::Any, port);
    connect(svr, &QTcpServer::newConnection, [&](){
        qDebug()<<"conn detected.";
        sck[sck_ptr] = svr->nextPendingConnection();
        if(sck_ptr == MAX_SCK) {
            delete sck[sck_ptr];
            return;
        }

        ip[sck_ptr] = sck[sck_ptr]->peerAddress().toIPv4Address();
        int ptr = sck_ptr;
        connect(sck[sck_ptr], &QTcpSocket::readyRead, [=](){
            read_and_dispatch(ptr);
        });
        sck_ptr++;// always point to the place to be written

        if(sck_ptr == 2)
            QTimer::singleShot(1000, [&](){
                init_game();
            });

    });
}

void Server::read_and_dispatch(int index) {
    auto bytes = sck[index]->readAll();

    qDebug()<<"Server Read"<<QString(bytes);

    //2-player situation
    if(index == 1)
        send(0, bytes);
    else
        send(1, bytes);


}

Server::~Server(){
    delete svr;
}

void Server::send(int client_index, const QByteArray& bytes){
    sck[client_index]->write(bytes.data());
    sck[client_index]->waitForBytesWritten();
}
