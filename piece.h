#ifndef PIECE_H
#define PIECE_H

#include <QPushButton>
#include <QObject>
#include <QWidget>
#include <QMessageBox>
#include <string>
#include "chess.h"


class StepHint;
class MainWindow;
class Chess;

class Piece : public QPushButton
{
    Q_OBJECT
public:
    Piece(QWidget* parent, int loc, int tm, int id, Chess* g);

    QList<int> get_available();

public slots:
    void on_click();

    void move_to(int loc, bool self = true);

    void flip(bool self = true);

    int get_location() const {
        return location;
    }

    int get_id() const {
        return identity;
    }

    bool is_clear() const {
        return clear;
    }

    int get_team() const {
        return team;
    }

    Chess* get_current_game() const {
        return game;
    }
public: signals:
    void make_movement();

private:
    Chess* game;
    int location {0};
    int identity {0x7fffffff};
    bool clear {false};
    int team {-1};
    int search_table[60];

    bool operable(int);

    void disappear();

    void search_along(QList<int>&, int);

    void search_further(QList<int>&, int);

    QList<int>& go_along(QList<int>& , int cursor, int (*next_opeartion)(int), bool (*condition)(int));

    void place();

};

#endif // PIECE_H
