#include "boardlabel.h"
#include "piece.h"

BoardLabel::BoardLabel(QWidget* parent) : QLabel(parent)
{
    setObjectName(QString::fromUtf8("BackgroundLabel"));
    setGeometry(QRect(50, 80 + 21, 541, 781));
    setPixmap(QPixmap(QString::fromUtf8(":/pic/Resource/background.png")));
    setScaledContents(false);
}

void BoardLabel::mousePressEvent(QMouseEvent* e) {
    if(e->button() == Qt::LeftButton){
        Piece::remove_hint_renders();
    }
}
