#include "chess.h"
#include <sstream>
#include "piece.h"
#include "mainwindow.h"

using namespace std;

const int Chess::camp_site[10] = {11, 13, 17, 21, 23, 36, 38, 42, 46, 48};
const int Chess::normal_site[18] = {0, 1, 2, 3, 4, 12, 16, 18, 22, 37, 41, 43, 47, 55, 56, 57, 58 ,59};
const int Chess::railway_site[60] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0};
const int Chess::win_y[12] = {113, 167, 222, 277, 332, 387, 559, 614, 669, 724, 779, 834};
const int Chess::win_x[5] = {56, 168, 280, 393, 506};


Chess::Chess(QObject *parent) : QObject(parent)
{

}

bool Chess::is_null(int location) {
    return board[location] == nullptr;
}

bool Chess::in_normal(int loc) {
    for(int i : normal_site) {
        if(loc == i) {
            return true;
        }
    }
    return false;
}

bool Chess::in_camp(int loc) {
    for(int i : camp_site) {
        if(i == loc)
            return true;
    }
    return false;
}

void Chess::remove_hint_renders(){
    for(int i = 0; i < hinters_cnt; ++i) {
        delete hinters[i];
    }
    if(hinters)
        delete hinters;

    hinters_cnt = 0;
    hinters = nullptr;
}

/* init string's format hint
 *
 * <team> <identity>\n
 * line for camp skipped!!
 */
void Chess::init_board(QString layout, MainWindow* w) {
    qDebug()<<"Begin_init";
    game_window = w;
    istringstream l (layout.toStdString());
    for(int i = 0; i < 60; ++i){
        if(in_camp(i)){
            continue;
            board[i] = nullptr;
        }
        int tm, id;
        l>>tm>>id;
        Piece* p = new Piece(game_window, i, tm, id, this);
        board[i] = p;
        p->show();
    }
}


void Chess::turn_switch(bool ot){
    game_window->turn(ot);

    if(ot == our_turn)
        return;
    Chess::our_turn = ot;

    if(our_turn){
        if(our_team == -1) {
            for(Piece* p : board){
                if(p != nullptr) {
                    if(p->is_clear()){
                        p->setCursor(QCursor(Qt::ForbiddenCursor));
                    } else {
                        p->setCursor(QCursor(Qt::ArrowCursor));
                    }
                }
            }
        } else {
            for(Piece* p : board){
                if(p != nullptr) {
                    if(p->is_clear() == false){
                        p->setCursor(QCursor(Qt::ArrowCursor));
                    } else {
                        if(p->get_team() == our_team)
                            p->setCursor(QCursor(Qt::ArrowCursor));
                        else
                            p->setCursor(QCursor(Qt::ForbiddenCursor));
                    }
                }
            }
        }

    } else {
        cursor_total_disable();
    }
}

void Chess::cursor_total_disable(){
    for(Piece* p : board){
        if(p != nullptr) {
            p->setCursor(QCursor(Qt::ForbiddenCursor));
        }
    }
}

void Chess::end(QString msg_box, QString msg_line){
    ended = true;
    cursor_total_disable();
    remove_hint_renders();
    QMessageBox::information(game_window, "Military Chess", msg_box);
    game_window->information(msg_line == "" ? msg_box : msg_line, true);
    game_window->disable_all_action();

}

void Chess::perform_opponent_flip(int loc) {
    board[loc]->flip(false);
}

void Chess::perform_opponent_move(int from, int to) {
    board[from]->move_to(to, false);
}
