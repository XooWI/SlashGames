#include "authorizationwindow.h"
#include "ui_authorizationwindow.h"

#include <QMessageBox>
#include <QDebug>
#include <QTabBar>

AuthorizationWindow::AuthorizationWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AuthWindow)
{
    ui->setupUi(this);

    this->setMinimumSize(400, 380);
    this->setMaximumSize(400, 380);

    ui->tabWidget->tabBar()->setVisible(false); // Убирает вкладки
    ui->tabWidget->setCurrentIndex(0);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // Убираем знак вопроса
}


// Переключаем на вкладку входа
void AuthorizationWindow::on_switchToLogin_clicked()
{
    this->setMinimumSize(400, 380);
    this->setMaximumSize(400, 380);
    ui->tabWidget->setCurrentIndex(0);
}


// Переключаем на вкладку регистрации
void AuthorizationWindow::on_switchToRegister_clicked()
{
    this->setMinimumSize(400, 550);
    this->setMaximumSize(400, 550);
    ui->tabWidget->setCurrentIndex(1);
}


void AuthorizationWindow::on_loginButton_clicked()
{
    QString login = ui->loginEdit->text();
    QString password = ui->passwordEdit->text();

    if(login.isEmpty() || password.isEmpty()) {
        ui->errorLabel->setText("Заполните все поля!");
        return;
    }

    qDebug() << "Попытка входа. Логин:" << login << "Пароль:" << password;

    ui->errorLabel->setText("");
    ui->loginEdit->clear();
    ui->passwordEdit->clear();
    QMessageBox::information(this, "Успех", "Вход выполнен успешно!");

}


void AuthorizationWindow::on_registerButton_clicked()
{
    QString username = ui->regUsernameEdit->text();
    QString login = ui->regLoginEdit->text();
    QString password = ui->regPasswordEdit->text();
    QString confirmPassword = ui->confirmPasswordEdit->text();

    if(username.isEmpty() || login.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        ui->regErrorLabel->setText("Заполните все поля!");
        return;
    }

    if(password != confirmPassword) {
        ui->regErrorLabel->setText("Пароли не совпадают!");
        return;
    }

    if(password.length() < 6) {
        ui->regErrorLabel->setText("Пароль должен содержать минимум 6 символов!");

        return;
    }

    qDebug() << "Попытка регистрации. Логин:" << login << "Пароль:" << password;

    QMessageBox::information(this, "Успех", "Регистрация выполнена успешно!");

    // Очищаем поля регистрации
    ui->regLoginEdit->clear();
    ui->regPasswordEdit->clear();
    ui->confirmPasswordEdit->clear();
    ui->regErrorLabel->setText("");

    on_switchToLogin_clicked();
}


AuthorizationWindow::~AuthorizationWindow()
{
    delete ui;
}
