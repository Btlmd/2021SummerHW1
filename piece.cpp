#include "piece.h"
#include <QPixmap>
#include <QPalette>
#include <QBrush>
#include <QDebug>
#include <QChar>
#include "mainwindow.h"
#include <QMovie>
#include <QTimer>

bool Piece::operable(int location){
    //It should be previously judged if the piece is movable!!!
    qDebug()<< "";
    qDebug()<<"BEGIN LOCATION @"<<location<<"as"<<identity;
    if(game->is_null(location))
        return true;
    qDebug()<<"is_null pass";
    if(Chess::in_camp(location))
        return false;
    qDebug()<<"in_camp pass";
    if(game->board[location]->team == team)
        return false;
    qDebug()<<"same_team pass";
    if(!game->board[location]->clear)
        return false;
    qDebug()<<"Clearance pass";
    int other = game->board[location]->identity;

    if(identity <= 8 && other <= 8)
        return identity >= other;
    qDebug()<<"military_comparison pass with"<<identity<<other;

    if(identity == 9 || other == 9)
        if(other != 11)
            return true;

    qDebug()<<"bomb pass";
    if(other == 11) {
        qDebug()<<"Trial on flag, left mine(s) "<<game->opponent_mine_left;
        if(game->opponent_mine_left == 0) {
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

Piece::Piece(QWidget* parent, int loc, int tm, int id, Chess* g): QPushButton(parent),  identity{id}, team{tm}, game{g}
{
    connect(this, &QPushButton::clicked, this, &Piece::on_click);
    setStyleSheet("border-image:url(:/pic/Resource/unknown) 0px 0px no-repeat;");
    resize(75, 38);
    location = loc;
    place();
    setCursor(QCursor(Qt::ForbiddenCursor));

    connect(this, &Piece::make_movement, game->game_window, &MainWindow::stop_and_renew_timer);
}

void Piece::on_click() {
    game->remove_hint_renders();

    if(game->is_ended())
        return;

    if(!game->our_turn){
        game->game_window->information("Now it's not our term.");
        return;
    }

    if(game->our_team == -1 ) {
        if(clear)
            return;
        flip();
        return;
    }


    if(clear) {
        if(team != game->our_team) {
            game->game_window->information("This is not your team member.");
            return;
        }
        if(identity == 10) {
            game->game_window->information("Mine cannot move.");
            return;
        }
        if(identity == 11){
            game->game_window->information("Flag cannot move.");
            return;
        }
        QList<int> render_list {get_available()};
        qDebug()<<"Clear: Try to render possible locations"<<render_list.size();

        game->hinters_cnt = render_list.size();
        game->hinters = new StepHint*[game->hinters_cnt];
        for(int i = 0; i < game->hinters_cnt; ++i) {
            game->hinters[i] = new StepHint(qobject_cast<QWidget*>(game->game_window), render_list[i], this);
        }
    } else {
        flip();
    }
}



QList<int> Piece::get_available() {
    QList<int> av {};

    if(Chess::in_camp(location)) {
        qDebug()<<"piece in camp";
        int possible_delta[8] = {-6, -5, -4, -1, 1, 4, 5, 6};
        for(int j = 0; j < 8; ++j) {
            if(operable(location + possible_delta[j]))
                av.push_back(location + possible_delta[j]);
        }
        return av;
    }

    if(Chess::in_normal(location)){
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
        if(game->is_null(cursor)){
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
    if(Chess::railway_site[loc] == 1 && search_table[loc] == 0){
        search_table[loc] = 1;
        if(game->is_null(loc)){
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



void Piece::move_to(int loc, bool self) {
    if(self){
        game->turn_switch(false);
        game->game_window->send_move(location, loc);
    }

    ++game->step_cnt;;
    game->game_window->set_info_default();

    game->remove_hint_renders();

    game->board[location] = nullptr;

    location = loc;

    emit make_movement();

    //Null situation
    if(game->is_null(loc)) {
        game->board[loc] = this;
        this->place();
        return;
    }

    int other = game->board[loc]->identity;

    //when romoving opponent's mine
    if(other == 10 && game->board[loc]->team != game->our_team){
        --game->opponent_mine_left;
        qDebug()<<"Decrease opppnent mine left"<< game->opponent_mine_left;
    }

    //Annihilation
    if(this->identity == 9 || other == 9 || this->identity == other) {
        game->board[loc]->hide();
        this->hide();
        game->board[loc] = nullptr;
        QMovie* mov = new QMovie(":/pic/Resource/explode.gif");
        QLabel* canvas = new QLabel(game->game_window);
        canvas->setMovie(mov);
        canvas->resize(75, 38);
        canvas->move(Chess::win_x[location % 5] - 5, Chess::win_y[location / 5] - 5 + 4);
        QTimer::singleShot(800,[canvas, mov](){
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
        game->board[loc]->hide();
        this->place();
        if(game->board[loc]->team == game->our_team)
            game->end("You lose.");
        else
            game->end("You win!");
    }

    //Eat
        game->board[loc]->hide();
        game->board[loc] = this;
        this->place();
    return;

}

void Piece::flip(bool self){
    //render effect
    this->setStyleSheet("border-image:url(:/pic/Resource/" + QString::number(team) + "/" + QString::number(identity)+ ".png) 0px 0px no-repeat;");
    this->clear = true;
    ++game->step_cnt;;
    game->game_window->set_info_default();

    emit make_movement();

    //undetermined phase judge logic ONLY
    if(game->our_team == -1){
        if(self){
            if(game->my_prev_flip_team == team){
                game->our_team = team;
                game->game_window->our_team_determined(game->our_team);
            } else {
                game->my_prev_flip_team = team;
            }
        } else {
            if(game->opponent_prev_flip_team == team){
                game->our_team = team == 1 ? 0 : 1;
                game->game_window->our_team_determined(game->our_team);
            }else{
                game->opponent_prev_flip_team = team;
            }
        }
    }

    //common logic
    if(self){
        game->turn_switch(false);
        game->game_window->send_flip(location);
    }
}

//set location, then move()
void Piece::place() {
    this->move(Chess::win_x[location % 5] - 5, Chess::win_y[location / 5] - 5 + 4); // mannual adjustemnt -5
}
