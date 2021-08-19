#include "piece.h"
#include <QPixmap>
#include <QPalette>
#include <QBrush>
#include <QDebug>
#include <QChar>
#include "mainwindow.h"
#include <sstream>
#include <QMovie>
#include <QTimer>


using namespace std;

int Piece::our_team {-1};
int Piece::camp_site[10] = {11, 13, 17, 21, 23, 36, 38, 42, 46, 48};
int Piece::normal_site[18] = {0, 1, 2, 3, 4, 12, 16, 18, 22, 37, 41, 43, 47, 55, 56, 57, 58 ,59};
int Piece::opponent_mine_left {3};
int Piece::railway_site[60] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1,
                               1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0};
Piece* Piece::board[60] = {nullptr};
bool Piece::our_tern {false};
int Piece::win_y[12] = {113, 167, 222, 277, 332, 387, 559, 614, 669, 724, 779, 834};
int Piece::win_x[5] = {56, 168, 280, 393, 506};
int Piece::my_prev_flip_team {-1};
int Piece::opponent_prev_flip_team {-1};
MainWindow* Piece::win {nullptr};
int Piece::Hinters_cnt {0};
StepHint** Piece::Hinters {nullptr};
int Piece::step_cnt {0};
bool Piece::ended {false};
int Piece::our_timeout_cnt {0};


bool Piece::is_null(int location) {
    return board[location] == nullptr;
}

bool Piece::operable(int location){
    //It should be previously judged if the piece is movable!!!
    qDebug()<< "";
    qDebug()<<"BEGIN LOCATION @"<<location<<"as"<<identity;
    if(is_null(location))
        return true;
    qDebug()<<"is_null pass";
    if(in_camp(location))
        return false;
    qDebug()<<"in_camp pass";
    if(board[location]->team == team)
        return false;
    qDebug()<<"same_team pass";
    if(!board[location]->clear)
        return false;
    qDebug()<<"Clearance pass";
    int other = board[location]->identity;

    if(identity <= 8 && other <= 8)
        return identity >= other;
    qDebug()<<"military_comparison pass with"<<identity<<other;

    if(identity == 9 || other == 9)
        if(other != 11)
            return true;

    qDebug()<<"bomb pass";
    if(other == 11) {
        qDebug()<<"Trial on flag, left mine(s) "<<opponent_mine_left;
        if(opponent_mine_left == 0) {
            return true;
        } else {
            return false;
        }
    }

    if(other == 10) {
        if(identity == 0) {
            return true;
        } else {
            return false;
        }
    }
    qDebug()<<"Fatal Err: Unreturned operability judgement! (this, that) ="<<identity<<other;
    return false;
}

Piece::Piece(QWidget* parent, int loc, int tm, int id): QPushButton(parent),  identity{id},team{tm}
{
    connect(this, &QPushButton::clicked, this, &Piece::on_click);
    setStyleSheet("border-image:url(:/pic/Resource/unknown) 0px 0px no-repeat;");
    resize(75, 38);
    location = loc;
    place();
    setCursor(QCursor(Qt::ForbiddenCursor));

}

void Piece::test(){
    this->setStyleSheet("border-image:url(:/pic/Resource/" + QString::number(team) + "/" + QString::number(identity)+ ") 0px 0px no-repeat;");
    this->clear = true;
}

void Piece::on_click() {
    remove_hint_renders();

    if(ended)
        return;

    if(!our_tern){
        win->information("Now it's not our term.");
        return;
    }

    if(our_team == -1 ) {
        if(clear)
            return;
        flip();
        return;
    }


    if(clear) {
        if(team != our_team) {
            win->information("This is not your team member.");
            return;
        }
        if(identity == 10) {
            win->information("Mine cannot move.");
            return;
        }
        if(identity == 11){
            win->information("Flag cannot move.");
            return;
        }
        QList<int> render_list {get_available()};
        qDebug()<<"Clear: Try to render possible locations"<<render_list.size();

        Hinters_cnt = render_list.size();
        Hinters = new StepHint*[Hinters_cnt];
        for(int i = 0; i < Hinters_cnt; ++i) {
            Hinters[i] = new StepHint(qobject_cast<QWidget*>(win), render_list[i], this);
        }
    } else {
        flip();
    }
}

bool Piece::in_normal(int loc) {
    for(int i : normal_site) {
        if(loc == i) {
            return true;
        }
    }
    return false;
}

QList<int> Piece::get_available() {
    QList<int> av {};

    if(in_camp(location)) {
        qDebug()<<"piece in camp";
        int possible_delta[8] = {-6, -5, -4, -1, 1, 4, 5, 6};
        for(int j = 0; j < 8; ++j) {
            if(operable(location + possible_delta[j]))
                av.push_back(location + possible_delta[j]);
        }
        return av;
    }

    if(in_normal(location)){
        int possible_delta[4] = {-5, -1, 1, 5};
        switch(location){
        case 1:
        case 2:
        case 3:
            possible_delta[0] = 0;
            break;
        case 0:
            possible_delta[0] = 0;
            possible_delta[1] = 0;
            break;
        case 4:
            possible_delta[0] = 0;
            possible_delta[2] = 0;
            break;
        case 56:
        case 57:
        case 58:
            possible_delta[3] = 0;
            break;
        case 55:
            possible_delta[1] = 0;
            possible_delta[3] = 0;
            break;
        case 59:
            possible_delta[2] = 0;
            possible_delta[3] = 0;
            break;
        default:
            break;
        }
        for(int delta : possible_delta) {
            if(delta == 0)
                continue;
            if(operable(location + delta))
                av.push_back(location + delta);
        }
        return av;
    }


    //first get the positions on the railway
    if(identity == 0){
        memset(search_table, 0, sizeof(int) * 60);
        search_table[location] = 1;
        search_along(av, location);
    }
    else
    {
        //paralle situation1: left & right
        int para1[14] = {6, 7, 8, 26, 28, 31, 33, 51, 52, 53, 5, 9, 50, 54};
        for(int loc : para1) {
            if(location == loc){
                go_along(av, loc - 1, [](int cur){return cur - 1;}, [](int cur){return (cur % 5) != 4;});
                go_along(av, loc + 1, [](int cur){return cur + 1;}, [](int cur){return (cur % 5) != 0;});
            }
        }

        //paralle situation2: up & down
        int para2[16] = {10, 15, 20, 14, 19, 24, 35, 40, 45, 39, 44, 49, 25, 29, 30 ,34};
        for(int loc : para2) {
            if(location == loc){
                go_along(av, loc - 5, [](int cur){return cur - 5;}, [](int cur){return cur > 4;});
                go_along(av, loc + 5, [](int cur){return cur + 5;}, [](int cur){return cur < 55;});
            }
        }

        //crossing situation, compensational for {25, 29, 30, 34, 27, 32}
        switch (location) {
        case 5:
            go_along(av, location + 5, [](int cur){return cur + 5;}, [](int cur){return cur < 55;});
            break;

        case 9:
            go_along(av, location + 5, [](int cur){return cur + 5;}, [](int cur){return cur < 55;});
            break;

        case 50:
            go_along(av, location - 5, [](int cur){return cur - 5;}, [](int cur){return cur > 4;});
            break;

        case 54:
            go_along(av, location - 5, [](int cur){return cur - 5;}, [](int cur){return cur > 4;});
            break;

        case 25:
        case 30:
            go_along(av, location + 1, [](int cur){return cur + 1;}, [](int cur){return (cur % 5) != 0;});
            break;

        case 29:
        case 34:
            go_along(av, location - 1, [](int cur){return cur - 1;}, [](int cur){return (cur % 5) != 4;});
            break;
        case 27:
            if(operable(32))
                av.push_back(32);
            break;
        case 32:
            if(operable(27))
                av.push_back(27);
            break;
        }
    }

    //then get general adjacency
    QList<int> normal_delta {};
    //search 1
    switch(location) {
    case 6:
    case 7:
    case 8:
    case 51:
    case 52:
    case 53:
        normal_delta.push_back(5);
        normal_delta.push_back(-5);
        break;

    case 10:
    case 15:
    case 20:
    case 35:
    case 40:
    case 45:
        normal_delta.push_back(1);
        break;

    case 14:
    case 19:
    case 24:
    case 39:
    case 44:
    case 49:
        normal_delta.push_back(-1);
        break;

    case 50:
    case 25:
        normal_delta.push_back(-4);
        break;

    case 5:
    case 30:
        normal_delta.push_back(6);
        break;

    case 9:
    case 34:
        normal_delta.push_back(4);
        break;

    case 29:
    case 54:
        normal_delta.push_back(-6);
        break;
    }

    //search 2
    switch(location) {
    case 7:
        normal_delta.push_back(6);
        normal_delta.push_back(4);
        break;
    case 52:
        normal_delta.push_back(-6);
        normal_delta.push_back(-4);
        break;
    case 32:
        normal_delta.push_back(6);
        normal_delta.push_back(5);
        normal_delta.push_back(4);
        break;
    case 27:
        normal_delta.push_back(-6);
        normal_delta.push_back(-5);
        normal_delta.push_back(-4);
        break;
    case 15:
    case 40:
        normal_delta.push_back(6);
        normal_delta.push_back(-4);
        break;
    case 19:
    case 44:
        normal_delta.push_back(-6);
        normal_delta.push_back(4);
        break;
    case 26:
    case 28:
    case 5:
    case 9:
        normal_delta.push_back(-5);
        break;
    case 31:
    case 33:
    case 50:
    case 54:
        normal_delta.push_back(5);
        break;

    }

    for(auto i : normal_delta){
        if(operable(location + i))
            av.push_back(location + i);
    }

    return av;
}

QList<int>& Piece::go_along(QList<int>& available_list, int cursor, int (*next_opeartion)(int), bool (*condition)(int)) {
    while(condition(cursor)){
        if(is_null(cursor)){
            available_list.push_back(cursor);
            cursor = next_opeartion(cursor);
        } else {
            if(operable(cursor)) {
                available_list.push_back(cursor);
            }
            break;
        }
    }
    return available_list;
}

void Piece::search_further(QList<int>& av, int loc){
    if(railway_site[loc] == 1 && search_table[loc] == 0){
        search_table[loc] = 1;
        if(is_null(loc)){
            av.push_back(loc);
            search_along(av, loc);
        } else {
            if(operable(loc)){
                av.push_back(loc);
            }
        }
    }
}

void Piece::search_along(QList<int>& av, int loc){
    if(loc % 5 > 0)
        search_further(av, loc - 1);
    if(loc % 5 < 4)
        search_further(av, loc + 1);
    if(loc / 5 > 0)
        if(loc != 31 && loc != 33)
            search_further(av, loc - 5);
    if(loc / 5 < 11)
        if(loc != 26 && loc != 28)
            search_further(av, loc + 5);

}

bool Piece::in_camp(int loc) {
    for(int i : camp_site) {
        if(i == loc)
            return true;
    }
    return false;
}

void Piece::remove_hint_renders(){
    for(int i = 0; i < Hinters_cnt; ++i) {
        delete Hinters[i];
    }
    if(Hinters)
        delete Hinters;

    Hinters_cnt = 0;
    Hinters = nullptr;
}

void Piece::move_to(int loc, bool self) {
    if(self){
        turn_switch(false);
        win->send_move(location, loc);
    }

    ++step_cnt;
    win->set_info_default();

    remove_hint_renders();

    board[location] = nullptr;

    location = loc;



    //Null situation
    if(is_null(loc)) {
        board[loc] = this;
        this->place();
        return;
    }

    int other = board[loc]->identity;

    //when romoving opponent's mine
    if(other == 10 && board[loc]->team != our_team){
        --opponent_mine_left;
        qDebug()<<"Decrease opppnent mine left"<< opponent_mine_left;
    }

    //Annihilation
    if(this->identity == 9 || other == 9 || this->identity == other) {
        board[loc]->hide();
        this->hide();
        board[loc] = nullptr;
        qDebug()<<"init_movie";
        QMovie* mov = new QMovie(":/pic/Resource/explode.gif");
        QLabel* canvas = new QLabel(win);
        canvas->setMovie(mov);
        canvas->resize(75, 38);
        canvas->move(win_x[location % 5] - 5, win_y[location / 5] - 5 + 4);
        QTimer::singleShot(800,[canvas, mov](){
            qDebug()<<"Moive killed";
            canvas->hide();
            canvas->deleteLater();
            delete mov;
        });
        canvas->show();
        mov->start();
        return;
    }

    //Winning situation
    if(other == 11) {
        board[loc]->hide();
        this->place();
        if(board[loc]->team == our_team)
            end("You lose.");
        else
            end("You win!");
    }

    //Eat
        board[loc]->hide();
        board[loc] = this;
        this->place();
    return;

}

void Piece::flip(bool self){
    //render effect
    this->setStyleSheet("border-image:url(:/pic/Resource/" + QString::number(team) + "/" + QString::number(identity)+ ".png) 0px 0px no-repeat;");
    this->clear = true;
    ++step_cnt;
    win->set_info_default();

    //undetermined phase judge logic ONLY
    if(our_team == -1){
        if(self){
            if(my_prev_flip_team == team){
                our_team = team;
                win->our_team_determined(our_team);
            } else {
                my_prev_flip_team = team;
            }
        } else {
            if(opponent_prev_flip_team == team){
                our_team = team == 1 ? 0 : 1;
                win->our_team_determined(our_team);
            }else{
                opponent_prev_flip_team = team;
            }
        }
    }

    //common logic
    if(self){
        turn_switch(false);
        win->send_flip(location);
    }
}

//set location, then move()
void Piece::place() {
    this->move(win_x[location % 5] - 5, win_y[location / 5] - 5 + 4); // mannual adjustemnt -5
}

/* init string's format hint
 *
 * <team> <identity>\n
 * line for camp skipped!!
 */
void Piece::init_board(QString layout, MainWindow* w) {
    win = w;
    istringstream l (layout.toStdString());
    for(int i = 0; i < 60; ++i){
        if(in_camp(i)){
            continue;
            board[i] = nullptr;
        }
        int tm, id;
        l>>tm>>id;
        Piece* p = new Piece(win, i, tm, id);
        board[i] = p;
        p->show();
    }
    qDebug()<<"init board done";
}


void Piece::turn_switch(bool ot){
    win->turn(ot);

    if(ot == our_tern)
        return;
    Piece::our_tern = ot;

    if(our_tern){
        if(our_team == -1) {
            for(Piece* p : board){
                if(p != nullptr) {
                    if(p->clear){
                        p->setCursor(QCursor(Qt::ForbiddenCursor));
                    } else {
                        p->setCursor(QCursor(Qt::ArrowCursor));
                    }
                }
            }
        } else {
            for(Piece* p : board){
                if(p != nullptr) {
                    if(p->clear == false){
                        p->setCursor(QCursor(Qt::ArrowCursor));
                    } else {
                        if(p->team == our_team)
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

void Piece::cursor_total_disable(){
    for(Piece* p : board){
        if(p != nullptr) {
            p->setCursor(QCursor(Qt::ForbiddenCursor));
        }
    }
}

void Piece::end(QString msg_box, QString msg_line){
    ended = true;
    cursor_total_disable();
    QMessageBox::information(win, "Military Chess", msg_box);
    win->information(msg_line == "" ? msg_box : msg_line, true);
    win->disable_all_action();
}
