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
}

ClientSettings::~ClientSettings()
{
    delete ui;
}

void ClientSettings::on_SubmitButton_clicked()
{
    if(is_input_legal()){
        emit server_config_done(ui->IPInput->text(), ui->PortInput->text().toInt());
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

