#ifndef CLIENTSETTINGS_H
#define CLIENTSETTINGS_H

#include <QDialog>

namespace Ui {
class ClientSettings;
}

class ClientSettings : public QDialog
{
    Q_OBJECT

public:
    explicit ClientSettings(QWidget *parent = nullptr);
    ~ClientSettings();

private slots:
    void on_SubmitButton_clicked();

public: signals:
    void server_config_done(QString IP, int Port);
private:
    Ui::ClientSettings *ui;
    bool is_input_legal();
};

#endif // CLIENTSETTINGS_H
