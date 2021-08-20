#ifndef PIECE_H
#define PIECE_H

#include <QPushButton>
#include <QObject>
#include <QWidget>
#include <QMessageBox>
#include <string>


class StepHint;
class MainWindow;

class Piece : public QPushButton
{
    Q_OBJECT
public:
    Piece(QWidget* parent, int loc, int tm, int id);
    QList<int> get_available();
public slots:
    void test();
    void on_click();
    void move_to(int loc, bool self = true);
    void flip(bool self = true);

    int get_location() const {
        return location;
    }

    int getId() const {
        return identity;
    }

public: signals:
    void make_movement();
private:
    int location {0};
    int identity {0x7fffffff};
    bool clear {false};
    int team {-1};
    int search_table[60];
    bool operable(int);
    void disappear();

    void search_along(QList<int>&, int);
    void search_further(QList<int>&, int);
    QList<int>& go_along(QList<int>& available_list, int cursor, int (*next_opeartion)(int), bool (*condition)(int));
    void place();

public:
    static int our_team;
    static bool our_tern;
    static Piece* board[60];
    static int camp_site[10];
    static int normal_site[18];
    static int railway_site[60];
    inline static int locate(int x, int y) {
        return y * 5 + x;
    }
    static int opponent_mine_left;
    static bool in_camp(int);
    static void init_board(QString layout, MainWindow* w);
    static int win_y[12];
    static int win_x[5];
    static int my_prev_flip_team;
    static int opponent_prev_flip_team;
    static MainWindow* win;
    static void turn_switch(bool our_turn);
    static bool is_null(int);
    static bool in_normal(int);
    static void remove_hint_renders();
    static StepHint** Hinters;
    static int Hinters_cnt;
    static int step_cnt;
    static QLabel* last_step_hinter;
    static void cursor_total_disable();
    static bool ended;
    static void end(QString msg_box, QString msg_line = "");
    static int our_timeout_cnt;
    static void render_hint_at(int);
    static void remove_last_step_hinter();
    static void render_step_hinters(Piece*);
};

#endif // PIECE_H
