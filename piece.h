#ifndef PIECE_H
#define PIECE_H

#include <QPushButton>
#include <QObject>
#include <QWidget>
#include <QMessageBox>
#include <string>

class StepHint;

class Piece : public QPushButton
{
    Q_OBJECT
public:
    Piece(QWidget* parent);
    QList<int> get_available();
public slots:
    void test();
    void on_click();
    void move_to(int loc);
private:
    int location;
    int x;
    int y;
    int identity;
    bool clear {false};
    bool movable {false};
    int team {-1};
    int search_table[60];
    bool operable(int);
    void disappear();
    bool is_null(int);
    void search_along(QList<int>&, int);
    void search_further(QList<int>&, int);
    QList<int>& go_along(QList<int>& available_list, int cursor, int (*next_opeartion)(int), bool (*condition)(int));
    void flip_render();
    StepHint** Hinters {nullptr};
    int Hinters_cnt {0};
public:
    static int our_team;
    static Piece* board[60];
    static int camp_site[10];
    static int normal_site[18];
    static int railway_site[5][12];
    inline static int locate(int x, int y) {
        return y * 5 + x;
    }
    static int mine_left;
    static bool in_camp(int);


};

#endif // PIECE_H
