#ifndef CHESS_H
#define CHESS_H

#include <QObject>

class Piece;
class MainWindow;
class StepHint;

class Chess : public QObject
{
    Q_OBJECT
public:
    explicit Chess(QObject *parent = nullptr);

    void init_board(QString layout, MainWindow* w);

    void turn_switch(bool our_turn);

    void cursor_total_disable();

    void remove_hint_renders();

    bool is_null(int);

    void end(QString msg_box, QString msg_line = "");

    bool is_ended() const {
        return ended;
    }

    bool is_our_turn() const {
        return our_turn;
    }

    int get_our_time_out_cnt() const {
        return our_timeout_cnt;
    }

    int get_step_cnt() const {
        return step_cnt;
    }

    int get_opponent_mine_left() const {
        return opponent_mine_left;
    }

    void increase_our_timeout_cnt() {
        ++our_timeout_cnt;
    }

    void perform_opponent_move(int from, int to);

    void perform_opponent_flip(int loc);

    int our_team {-1};
    bool our_turn {false};
    Piece* board[60];
    int opponent_mine_left {3};
    int my_prev_flip_team {-1};
    int opponent_prev_flip_team {-1};
    int our_timeout_cnt {0};
    bool ended {false};
    MainWindow* game_window {nullptr};
    StepHint** hinters {nullptr};
    int hinters_cnt {0};
    int step_cnt {0};

    static bool in_camp(int);

    static bool in_normal(int);

    static const int camp_site[10];
    static const int normal_site[18];
    static const int railway_site[60];
    static const int win_y[12];
    static const int win_x[5];

signals:

};

#endif // CHESS_H
