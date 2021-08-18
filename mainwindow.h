#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <piece.h>
#include <stephint.h>
#include <server.h>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // game operations
    void our_team_determined(int team);

    void information(QString msg);

    void send_move(int from, int to);

    void send_flip(int loc);

    void win(bool from_admit = false);

    void lose(bool from_admit = false);

    void turn(bool our_turn);
private slots:
    void on_actionConnect_to_a_server_triggered();

    void on_actionCreate_connection_as_a_server_triggered();

    void user_step_done();

    void connect_to_server(QString IP, int Port);

    void read_dispatcher();

private:
    Ui::MainWindow *ui;

    Server* server {nullptr};

    QTcpSocket* conn {nullptr};

    void init();






};
#endif // MAINWINDOW_H
