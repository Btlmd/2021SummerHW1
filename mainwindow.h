#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <piece.h>
#include <stephint.h>
#include <server.h>
#include <QTcpSocket>
#include <QTimer>

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

    void information(QString msg, bool lasting = false);

    void game_start();

    void send_move(int from, int to);

    void send_flip(int loc);

    void turn(bool our_turn);

    void set_info_default();

    void disable_all_action();
private slots:
    void on_actionConnect_to_a_server_triggered();

    void on_actionCreate_connection_as_a_server_triggered();

    void connect_to_server(QString IP, int Port, bool from_server = false);

    void read_dispatcher();

    void sck_write(const QByteArray& bytes);

    void on_actionAdmit_defeat_triggered();

    void sck_disconnected_event();

    void on_actionStart_triggered();

    void timer_second_up_check();

    void other_possibly_timeout_network_check();
public slots:
    void stop_and_renew_timer();

    void stop_game_timer();

    void stop_network_sync_timer();

    void end_game_window_actions();
public: signals:
    void time_update(int);

private:
    Ui::MainWindow *ui;

    Server* server {nullptr};

    QTcpSocket* conn {nullptr};

    void init();

    QTimer* info_show_timer {nullptr};

    bool ready {false};

    bool other_ready {false};

    QString init_info {};

    QTimer* game_timer {nullptr};

    QTimer* network_satus_sync_timer {nullptr};

    const int time_out {20};

    int time_left {-1};

    void set_timer_init();

    void new_one_second();

    QLabel* back_ground {nullptr};


};
#endif // MAINWINDOW_H
