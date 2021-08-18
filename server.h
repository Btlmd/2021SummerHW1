#ifndef SERVER_H
#define SERVER_H
#define MAX_SCK 2

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QString>
#include <QTime>
#include <QRandomGenerator>


class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(int Port, QObject *parent = nullptr);
    void init_game();
    void init_server();
    int get_port() {
        return port;
    }
    ~Server();

public slots:
    void send(int client_index, const QString& msg);
    void read_and_dispatch(int client_index);
signals:

private:
    int sck_ptr {0};
    int port {9999};
    qint32 ip[MAX_SCK];
    QTcpServer* svr;
    QTcpSocket* sck[MAX_SCK + 1];
    QString random_layout();
    const int to_be_allocated_type[50] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    const int to_be_allocated_piec[50] = {0, 0, 0, 1, 1, 1, 2, 2, 2, 10, 10, 10, 3, 3, 4, 4, 5, 5, 6, 6, 9, 9, 7, 8, 11,
                                          0, 0, 0, 1, 1, 1, 2, 2, 2, 10, 10, 10, 3, 3, 4, 4, 5, 5, 6, 6, 9, 9, 7, 8, 11};
    bool is_allocated[50] = {false, false, false, false, false, false, false, false, false, false,
                             false, false, false, false, false, false, false, false, false, false,
                             false, false, false, false, false, false, false, false, false, false,
                             false, false, false, false, false, false, false, false, false, false,
                             false, false, false, false, false, false, false, false, false, false};
    int get_rand(int moduler);
};

#endif // SERVER_H
