#ifndef AUTHORIZATIONWINDOW_H
#define AUTHORIZATIONWINDOW_H

#include"databasemanagement.h"

#include <QDialog>
#include <QSettings>
#include <QSqlDatabase>
#include <QCryptographicHash>
#include <QByteArray>
#include <QTabBar>
#include <QRegularExpression>

// Убрать после обновления!
#include <QDebug>
#include <QMessageBox>


namespace Ui {
class AuthWindow;
}

class AuthorizationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AuthorizationWindow(QSettings* settings, DatabaseManagement *dbManager, QWidget *parent = nullptr);
    ~AuthorizationWindow();

    int password_strength(QString &password);

private slots:
    void on_switchToRegister_clicked();
    void on_switchToLogin_clicked();
    void on_loginButton_clicked();
    void on_registerButton_clicked();

    void on_loginShowPassword_pressed();
    void on_loginShowPassword_released();

    void on_regShowPassword_released();
    void on_regShowPassword_pressed();

    void on_regShowConfirmPassword_pressed();
    void on_regShowConfirmPassword_released();

private:
    Ui::AuthWindow *ui;
    DatabaseManagement *dbManager;
    QSettings *settings;
    QByteArray hashPassword(const QString &password);
};

#endif // AUTHORIZATIONWINDOW_H
