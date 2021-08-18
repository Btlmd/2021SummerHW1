#include "stephint.h"
#include "QDebug"
#include "piece.h"
#include <QPixmap>
#include "mainwindow.h"


StepHint::StepHint(QWidget* parent, int loc, Piece* from): QLabel{parent}, call_back{from}, location{loc}
{    
    if(Piece::in_camp(loc)) {
        QPixmap bg(":/pic/Resource/circ_filling");
        this->setPixmap(bg.scaled(42, 42, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        this->resize(42, 42);
        //location_mover
    } else {
        QPixmap bg(":/pic/Resource/rect_filling.png");
        this->setPixmap(bg);
        this->resize(bg.width(), bg.height());
    }
    this->show();
}

void StepHint::mousePressEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton){
        this->on_click();
    }
}

void StepHint::on_click(){
    if(call_back){
        call_back->move_to(location);
        Piece::turn_switch(false);
        Piece::window->send_move(call_back->get_location(), location);
    }
}
