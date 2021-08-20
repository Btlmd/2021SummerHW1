#ifndef BOARDLABEL_H
#define BOARDLABEL_H

#include <QLabel>
#include <QObject>
#include <QMouseEvent>

class BoardLabel : public QLabel
{
    Q_OBJECT
public:
    BoardLabel(QWidget* parent);

protected:
    void mousePressEvent(QMouseEvent *e) override;
};

#endif // BOARDLABEL_H
