#ifndef AUTHORIZATIONWINDOW_H
#define AUTHORIZATIONWINDOW_H

#include <QDialog>

namespace Ui {
class AuthWindow;
}

class AuthorizationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AuthorizationWindow(QWidget *parent = nullptr);
    ~AuthorizationWindow();

private slots:
    void on_switchToRegister_clicked();
    void on_switchToLogin_clicked();
    void on_loginButton_clicked();
    void on_registerButton_clicked();

private:
    Ui::AuthWindow *ui;
};

#endif // AUTHORIZATIONWINDOW_H
