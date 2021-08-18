#include "server.h"

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

    msg[0] += random_layout();
    msg[1] += random_layout();

    send(0, msg[0]);
    send(1, msg[1]);
}

void Server::init_server() {
    svr = new QTcpServer(this);
    svr->listen(QHostAddress::Any, port);
    connect(svr, &QTcpServer::newConnection, [&](){
        sck[sck_ptr] = svr->nextPendingConnection();
        if(sck_ptr == MAX_SCK) {
            delete sck[sck_ptr];
            return;
        }

        ip[sck_ptr] = sck[sck_ptr]->peerAddress().toIPv4Address();
        connect(sck[sck_ptr], &QTcpSocket::readyRead, [=](){
            read_and_dispatch(sck_ptr);
        });
        sck_ptr++;// always point to the place to be written
    });
}

void Server::read_and_dispatch(int index) {
    auto bytes = sck[index]->readAll();

    //2-player situation
    if(index == 1){
        sck[0]->write(bytes.data());
        sck[0]->waitForBytesWritten();
    }
    else
    {
        sck[1]->write(bytes.data());
        sck[1]->waitForBytesWritten();
    }


}

bool Server::in_camp(int loc){
    for(int i : camp_site){
        if(loc == i)
            return true;
    }
    return false;
}

Server::~Server(){

}

void Server::send(int client_index, const QString& msg){

}
