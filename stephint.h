#ifndef STEPHINT_H
#define STEPHINT_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>

class Piece;

class StepHint : public QLabel
{
    Q_OBJECT
public:
    StepHint(QWidget* parent, int loc = 0, Piece* from = nullptr);
    void on_click();
private:
    int location;
    Piece* call_back;
protected:
    void mousePressEvent(QMouseEvent*) override;
};

#endif // STEPHINT_H
