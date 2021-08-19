#include "clientsettings.h"
#include "ui_clientsettings.h"
#include <QMessageBox>
#include <regex>

using namespace std;

ClientSettings::ClientSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClientSettings)
{
    ui->setupUi(this);
    setMinimumSize(size());
    setMaximumSize(size());
}

ClientSettings::~ClientSettings()
{
    delete ui;
}

void ClientSettings::on_SubmitButton_clicked()
{
    if(is_input_legal()){
        emit server_config_done(ui->IPInput->text(), ui->PortInput->text().toInt(), false);
        this->close();
    } else {
        QMessageBox::warning(this, "Warning", "Invalid input!");
    }
}

bool ClientSettings::is_input_legal(){
    if(regex_match(ui->IPInput->text().toStdString(), regex("[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+")))
        return true;
    return false;
}


void ClientSettings::on_pushButton_clicked()
{
    ui->IPInput->setText(ui->IPInput->text() + "1");
}


void ClientSettings::on_pushButton_2_clicked()
{
    ui->IPInput->setText(ui->IPInput->text() + "2");
}


void ClientSettings::on_pushButton_3_clicked()
{
    ui->IPInput->setText(ui->IPInput->text() + "3");
}


void ClientSettings::on_pushButton_4_clicked()
{
    ui->IPInput->setText(ui->IPInput->text() + "4");
}


void ClientSettings::on_pushButton_5_clicked()
{
    ui->IPInput->setText(ui->IPInput->text() + "5");
}


void ClientSettings::on_pushButton_6_clicked()
{
    ui->IPInput->setText(ui->IPInput->text() + "6");
}


void ClientSettings::on_pushButton_7_clicked()
{
    ui->IPInput->setText(ui->IPInput->text() + "7");
}


void ClientSettings::on_pushButton_8_clicked()
{
    ui->IPInput->setText(ui->IPInput->text() + "8");
}


void ClientSettings::on_pushButton_9_clicked()
{
    ui->IPInput->setText(ui->IPInput->text() + "9");
}


void ClientSettings::on_pushButton_11_clicked()
{
    ui->IPInput->setText(ui->IPInput->text() + "0");
}


void ClientSettings::on_pushButton_12_clicked()
{
    ui->IPInput->setText(ui->IPInput->text() + ".");
}


void ClientSettings::on_pushButton_10_clicked()
{
    if(ui->IPInput->text().length() != 0)
        ui->IPInput->setText(ui->IPInput->text().mid(0,ui->IPInput->text().length() - 1));
}

