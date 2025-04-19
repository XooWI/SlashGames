#include "authorizationwindow.h"
#include "ui_authorizationwindow.h"

#include <QMessageBox>
#include <QDebug>
#include <QTabBar>
#include <QRegularExpression>

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
    if(login.length() <10) {
        ui->regErrorLabel->setText("Логин должен быть не меньше 10 символов!");
        return;
    }
    if(password != confirmPassword) {
        ui->regErrorLabel->setText("Пароли не совпадают!");
        return;
    }

    if(AuthorizationWindow::password_strength(password) == 1) {
        // ui->regErrorLabel->setText("Пароль не подходит!");

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

int AuthorizationWindow::password_strength(QString &password)
{
    // Длина пароля
    if (password.length() < 10){
        ui->regErrorLabel->setText("Пароль меньше 10 символов!");
        return 1;
    }

    // Отсутствие пробелов
    if (password.contains(' ')){
        ui->regErrorLabel->setText("Не должно быть пробелов!");
        return 1;
    }

    // Содержит заглавные буквы
    if (password == password.toLower()){
        ui->regErrorLabel->setText("Должны быть заглавные буквы!");
        return 1;
    }

    // Содержит строчные буквы
    if (password == password.toUpper()){
        ui->regErrorLabel->setText("Должны быть прописные буквы!");
        return 1;
    }

    // Содержит цифры
    if (password.count(QRegularExpression("[0-9]"))==0){
        ui->regErrorLabel->setText("Должны быть цифры!");
        return 1;
    }

    // Содержит русские и английские буквы
    if ( ((password.count(QRegularExpression("[a-zA-Z]"))>0) + (password.count(QRegularExpression("[а-яА-Я]"))>0)) <2) {
        ui->regErrorLabel->setText("Должны быть английские и русские буквы!");
        return 1;
    }

    // Содержит спецсимволы
    if (password.count(QRegularExpression("[^a-zа-яA-ZА-Я0-9]"))==0){
        ui->regErrorLabel->setText("Должны быть символы!");
        return 1;
    }

    return 0;
}

AuthorizationWindow::~AuthorizationWindow()
{
    delete ui;
}
