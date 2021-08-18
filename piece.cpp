#include "piece.h"
#include <QPixmap>
#include <QPalette>
#include <QBrush>
#include <QDebug>
#include <mainwindow.h>
#include <QChar>

int Piece::our_team {-1};
int Piece::camp_site[10] = {11, 13, 17, 21, 23, 36, 38, 42, 46, 48};
int Piece::normal_site[18] = {0, 1, 2, 3, 4, 12, 16, 18, 22, 37, 41, 43, 47, 55, 56, 57, 58 ,59};
int Piece::mine_left {3};
int Piece::railway_site[5][12] = {
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}
};
Piece* Piece::board[60] = {nullptr};

bool Piece::is_null(int location) {
    return board[location] == nullptr;
}

bool Piece::operable(int location){
    //It should be previously judged if the piece is movable!!!

    if(board[location] == nullptr)
        return true;

    if(!board[location]->clear)
        return false;

    int other = board[location]->identity;
    for(int i =0; i < 10; ++i)
        if(other == camp_site[i])
            return false;

    if(identity <= 8 && other <= 8)
        return identity >= other;
    if(identity == 9 || other == 9)
        return true;
    if(other == 11) {
        if(mine_left == 0) {
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

Piece::Piece(QWidget* parent): QPushButton(parent)
{
    connect(this, &QPushButton::clicked, this, &Piece::test);
    this->setStyleSheet("border-image:url(:/pic/Resource/unknown) 0px 0px no-repeat;");
    this->setCursor(QCursor(Qt::ForbiddenCursor));

}

void Piece::test(){
    this->setStyleSheet("border-image:url(:/pic/Resource/0/0) 0px 0px no-repeat;");
    QMessageBox::information(this,"test", "set!");
}

void Piece::on_click() {
    if(!MainWindow::myTern)
        return;// emit not your tern
    if(team != our_team)
        return;// emit not your team
    if(identity == 10 || identity == 11)
        return;// emit not movable


    if(clear) {
        QList<int> render_list {get_available()};
        Hinters_cnt = render_list.size();
        Hinters = new StepHint*[Hinters_cnt];
        for(int i = 0; i < Hinters_cnt; ++i) {
            Hinters[i] = new StepHint(qobject_cast<QWidget*>(this->parent()), render_list[i], this);
        }
    } else {
        flip_render();
        // emit the tern finish signal
    }
}

QList<int> Piece::get_available() {
    QList<int> av {};

    for(int i : camp_site) {
        if(location == i) {
            int possible_delta[8] = {-6, -5, -4, -1, 1, 4, 5, 6};
            for(int j = 0; j < 8; ++j) {
                if(operable(location + possible_delta[j]))
                    av.push_back(location + possible_delta[j]);
            }
            return av;
        }
    }

    for(int i : normal_site) {
        if(location == i) {
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
            for(int j = 0; j < 4; ++j) {
                if(j == 0)
                    continue;
                if(operable(location + possible_delta[j]))
                    av.push_back(location + possible_delta[j]);
            }
        }
    }
    return av;

    //first get the positions on the railway
    if(identity == 0){
        memset(search_table, 0, sizeof(bool) * 60);
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
    int* l = (int*) railway_site;
    if(search_table[loc] == 0 && l[loc] == 1){
        search_table[loc] = 1;
        if(is_null(loc)){
            av.push_back(loc);
            search_along(av, loc);
        } else {
            if(operable(loc))
                av.push_back(loc);
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

void Piece::flip_render() {
    QString team_string {team == 1 ? "1" : "0"};
    this->setStyleSheet("border-image:url(:/pic/Resource/" + team_string + "/" + QString(QChar((char)('0'+identity)))+ ") 0px 0px no-repeat;");
    if(our_team != -1 && our_team != team)
        this->setCursor(QCursor(Qt::ForbiddenCursor));
}

bool Piece::in_camp(int loc) {
    for(int i : camp_site) {
        if(i == loc)
            return true;
    }
    return false;
}

void Piece::move_to(int loc) {
    for(int i = 0; i < Hinters_cnt; ++i) {
        delete Hinters[i];
    }
    delete Hinters;

    //Null situation
    if(is_null(loc)) {
        board[loc] = this;
        //this->move()
        return;
    }

    int other = board[loc]->identity;

    //Annihilation
    if(this->identity == 9 || other == 9 || this->identity == other) {
        board[loc]->hide();
        this->hide();
        board[loc] = nullptr;
        //annihilating movie to play
        return;
    }

    //Winning situation
    if(other == 11) {

    }

    //Eat
        board[loc]->hide();
        //this->move();
    return;

}
