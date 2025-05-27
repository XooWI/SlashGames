#ifndef AUTHORIZATIONWINDOW_H
#define AUTHORIZATIONWINDOW_H

#include <QDialog>
#include <QSettings>
#include <QByteArray>
#include <QTabBar>
#include <QCryptographicHash>

#include "Database/databasemanagement.h"
#include "Windows/Custom/customwindow.h"


namespace Ui {
class AuthWindow;
}

class AuthorizationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AuthorizationWindow(QSettings* settings, DatabaseManagement *dbManager, bool isDarkTheme, QWidget *parent = nullptr);
    ~AuthorizationWindow();

signals:
    // Сигнал, испускаемый при успешной авторизации
    void loginSuccessful();

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

    void on_loginEdit_textEdited(const QString &arg1);
    void on_passwordEdit_textEdited(const QString &arg1);

    void on_regUsernameEdit_textEdited(const QString &arg1);
    void on_regLoginEdit_textEdited(const QString &arg1);
    void on_regPasswordEdit_textEdited(const QString &arg1);
    void on_confirmPasswordEdit_textEdited(const QString &arg1);


private:
    Ui::AuthWindow *ui;
    DatabaseManagement *dbManager;
    QSettings *settings;
    bool darkTheme;
    QString getEyeIconPath(bool isOpenEye) const;
};

#endif // AUTHORIZATIONWINDOW_H
