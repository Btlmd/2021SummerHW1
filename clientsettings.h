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

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_10_clicked();

public: signals:
    void server_config_done(QString IP, int Port, int from_server);
private:
    Ui::ClientSettings *ui;
    bool is_input_legal();
};

#endif // CLIENTSETTINGS_H
