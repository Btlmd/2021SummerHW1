﻿#include "stephint.h"
#include "QDebug"
#include "piece.h"
#include <QPixmap>
#include "mainwindow.h"
#include "chess.h"

const int StepHint::circ_x[4] = {0, 183, 296, 409};
const int StepHint::circ_y[10] = {0, 0, 199, 253, 306, 0, 0, 592, 647, 702};

StepHint::StepHint(QWidget* parent, int loc, Piece* from): QLabel{parent}, call_back{from}, location{loc}
{    
    if(Chess::in_camp(loc)) {
        QPixmap bg(":/pic/Resource/circ_filling");
        setPixmap(bg.scaled(42, 42, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        resize(42, 42);
        move(circ_x[location % 5] - 5 + 5 - 4 + 2, circ_y[location / 5] - 5 + 23);
    } else if(call_back->get_current_game()->is_null(loc)){
        QPixmap bg(":/pic/Resource/rect_filling.png");
        setPixmap(bg);
        resize(bg.width(), bg.height());
        move(Chess::win_x[location % 5] - 5 + 8 , Chess::win_y[location / 5] - 5 + 8 + 3);
    } else if(call_back->get_current_game()->board[loc]->get_id() == 11) {
        QPixmap bg(":/pic/Resource/flag_filling.png");
        setPixmap(bg);
        resize(bg.width(), bg.height());
        move(Chess::win_x[location % 5] - 5, Chess::win_y[location / 5] - 5 + 4);
    } else {
        QPixmap bg(":/pic/Resource/not_null_filling.png");
        setPixmap(bg);
        resize(bg.width(), bg.height());
        move(Chess::win_x[location % 5] - 5, Chess::win_y[location / 5] - 5 + 4);
    }
    show();
}

void StepHint::mousePressEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton){
        this->on_click();
    }
}

void StepHint::on_click(){
    if(call_back){
        call_back->move_to(location, true);
    }
}
