#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>

bool MainWindow::myTern {false};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    auto nb = new Piece(this);
    auto sh = new StepHint(this, 11);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::init() {

}
