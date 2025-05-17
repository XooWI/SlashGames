#include "authorizationwindow.h"
#include "ui_authorizationwindow.h"


AuthorizationWindow::AuthorizationWindow(QSettings* settings, DatabaseManagement *dbManager, QWidget *parent) :
    QDialog(parent),
    settings(settings),
    dbManager(dbManager),
    ui(new Ui::AuthWindow)

{
    ui->setupUi(this);

    // Убирает вкладки
    ui->tabWidget->tabBar()->setVisible(false);

    // Начинаем с окна входа
    on_switchToLogin_clicked();

    // Убираем знак вопроса
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

AuthorizationWindow::~AuthorizationWindow()
{
    delete ui;
}

// Переключаем на вкладку входа
void AuthorizationWindow::on_switchToLogin_clicked()
{
    this->setMinimumSize(400, 380);
    this->setMaximumSize(400, 380);
    ui->tabWidget->setCurrentIndex(0);
    ui->loginEdit->clear();
    ui->passwordEdit->clear();
    ui->errorLabel->setText("");
}


// Переключаем на вкладку регистрации
void AuthorizationWindow::on_switchToRegister_clicked()
{
    this->setMinimumSize(400, 550);
    this->setMaximumSize(400, 550);
    ui->tabWidget->setCurrentIndex(1);
    ui->regUsernameEdit->clear();
    ui->regLoginEdit->clear();
    ui->regPasswordEdit->clear();
    ui->confirmPasswordEdit->clear();
    ui->regErrorLabel->setText("");
}


void AuthorizationWindow::on_loginButton_clicked()
{
    QString login = ui->loginEdit->text(), password = ui->passwordEdit->text();

    if(login.isEmpty() || password.isEmpty()) {
        ui->errorLabel->setText("Заполните все поля!");
        if (login.isEmpty()) ui->loginEdit->setStyleSheet("border: 1px solid red;");
        if (password.isEmpty())ui->passwordEdit->setStyleSheet("border: 1px solid red;");
        return;
    }

    // Хешируем введенный пользователем пароль для сравнения с хешем в БД
    QByteArray enteredPasswordHash = hashPassword(password);

    if (dbManager->checkUser(login, enteredPasswordHash)){
        // Успешный вход
        ui->errorLabel->setText("");
        ui->loginEdit->clear();
        ui->passwordEdit->clear();

        emit loginSuccessful(); // Сигнал об успешной регистрации

        // Отображаем сообщение об успехе
        // Заменить на кастомное окно!
        //QMessageBox::information(this, "Успех", "Вход выполнен успешно!");

        this->accept();
    } else {
        // Неверный логин или пароль
        ui->errorLabel->setText("Неверный логин или пароль.");
        ui->passwordEdit->clear();
        ui->loginEdit->setFocus(); // Ставим курсор на ввод логина
        return;
    }
}


void AuthorizationWindow::on_registerButton_clicked()
{
    QString username = ui->regUsernameEdit->text();
    QString login = ui->regLoginEdit->text();
    QString password = ui->regPasswordEdit->text();
    QString confirmPassword = ui->confirmPasswordEdit->text();

    ui->regErrorLabel->setText("");

    if(username.isEmpty() || login.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        if (username.isEmpty()) ui->regUsernameEdit->setStyleSheet("border: 1px solid red;");
        if (login.isEmpty()) ui->regLoginEdit->setStyleSheet("border: 1px solid red;");
        if (password.isEmpty()) ui->regPasswordEdit->setStyleSheet("border: 1px solid red;");
        if (confirmPassword.isEmpty()) ui->confirmPasswordEdit->setStyleSheet("border: 1px solid red;");
        ui->regErrorLabel->setText("Заполните все поля!");
        return;
    }
    if(login.length() < 8) {
        ui->regErrorLabel->setText("Логин должен быть не меньше 8 символов!");
        ui->regLoginEdit->setStyleSheet("border: 1px solid red;");
        return;
    }

    if(dbManager->checkLogin(login)) {
        ui->regErrorLabel->setText("Пользователь с таким логином уже существует.");
        ui->regLoginEdit->setStyleSheet("border: 1px solid red;");
        return;
    }

    if(password != confirmPassword) {
        ui->regErrorLabel->setText("Пароли не совпадают!");
        ui->regPasswordEdit->setStyleSheet("border: 1px solid red;");
        ui->confirmPasswordEdit->setStyleSheet("border: 1px solid red;");
        return;
    }

    switch(password_strength(password)){
    case 1: ui->regErrorLabel->setText("Пароль должен быть не меньше 10 символов!"); return;
    case 2: ui->regErrorLabel->setText("Пароль не должен содержать пробелов!"); return;
    case 3: ui->regErrorLabel->setText("Пароль должен содержать заглавные буквы!"); return;
    case 4: ui->regErrorLabel->setText("Пароль должен содержать строчные буквы!"); return;
    case 5: ui->regErrorLabel->setText("Пароль должен содержать цифры!"); return;
    case 6: ui->regErrorLabel->setText("Пароль должен содержать английские и русские буквы!"); return;
    case 7: ui->regErrorLabel->setText("Пароль должен содержать специальные символы!"); return;
    case 0: ui->regErrorLabel->setText("");
    }

    QByteArray hashedPassword = hashPassword(password);

    if(dbManager->registredUser(username, login, hashedPassword)){
        // Успешная регистрация
        ui->regUsernameEdit->clear();
        ui->regLoginEdit->clear();
        ui->regPasswordEdit->clear();
        ui->confirmPasswordEdit->clear();

        // Отображаем сообщение об успехе
        // Заменить на кастомное окно!
        QMessageBox::information(this, "Успех", "Регистрация выполнена успешно!");

        // Переключаемся на вкладку входа
        on_switchToLogin_clicked();
    } else {
        ui->regErrorLabel->setText("Ошибка при регистрации пользователя.");
    }
}

// Проверка сложности пароля
int AuthorizationWindow::password_strength(QString &password)
{
    // Длина пароля
    if (password.length() < 10) return 1;

    // Отсутствие пробелов
    if (password.contains(' ')) return 2;

    // Содержит заглавные буквы
    if (password == password.toLower()) return 3;

    // Содержит строчные буквы
    if (password == password.toUpper()) return 4;

    // Содержит цифры
    if (password.count(QRegularExpression("[0-9]")) == 0) return 5;

    // Содержит русские и английские буквы
    if ( ((password.count(QRegularExpression("[a-zA-Z]"))>0) + (password.count(QRegularExpression("[а-яА-Я]"))>0)) <2) return 6;

    // Содержит спецсимволы
    if (password.count(QRegularExpression("[^a-zA-Zа-яА-Я0-9]")) == 0) return 7;

    return 0;
}


// Хеширование пароля
// Добавить salt!
QByteArray AuthorizationWindow::hashPassword(const QString &password){
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
}

void AuthorizationWindow::on_loginEdit_textEdited(const QString &arg1)
{
    ui->errorLabel->setText("");
    ui->loginEdit->setStyleSheet("");
}


void AuthorizationWindow::on_passwordEdit_textEdited(const QString &arg1)
{
    ui->errorLabel->setText("");
    ui->passwordEdit->setStyleSheet("");
}


void AuthorizationWindow::on_regUsernameEdit_textEdited(const QString &arg1)
{
    ui->regErrorLabel->setText("");
    ui->regUsernameEdit->setStyleSheet("");
}


void AuthorizationWindow::on_regLoginEdit_textEdited(const QString &arg1)
{
    ui->regErrorLabel->setText("");
    ui->regLoginEdit->setStyleSheet("");
}

void AuthorizationWindow::on_regPasswordEdit_textEdited(const QString &arg1)
{
    ui->regErrorLabel->setText("");
    ui->regPasswordEdit->setStyleSheet("");
    QString password = ui->regPasswordEdit->text();
    int count = password_strength(password);
    if (count==0) {
       ui->regPasswordEdit->setStyleSheet("border: 1px solid green;");
       return;
    }
    if (count<=3) {
        ui->regPasswordEdit->setStyleSheet("border: 1px solid red;");
        return;
    }
    if (count<=5) {
       ui->regPasswordEdit->setStyleSheet("border: 1px solid yellow;");
       return;
    }
    if (count<=7) {
       ui->regPasswordEdit->setStyleSheet("border: 1px solid orange;");
       return;
    }

}

void AuthorizationWindow::on_confirmPasswordEdit_textEdited(const QString &arg1)
{
    ui->regErrorLabel->setText("");
    ui->confirmPasswordEdit->setStyleSheet("");

    QString password = ui->confirmPasswordEdit->text();
    int count = password_strength(password);
    if (count==0) {
       ui->confirmPasswordEdit->setStyleSheet("border: 1px solid green;");
       return;
    }
    if (count<=3) {
        ui->confirmPasswordEdit->setStyleSheet("border: 1px solid red;");
        return;
    }
    if (count<=5) {
       ui->confirmPasswordEdit->setStyleSheet("border: 1px solid yellow;");
       return;
    }
    if (count<=7) {
       ui->confirmPasswordEdit->setStyleSheet("border: 1px solid orange;");
       return;
    }

}

// Показ пароля при нажатии кнопки с глазом
void AuthorizationWindow::on_loginShowPassword_pressed()
{
    ui->passwordEdit->setEchoMode(QLineEdit::Normal);
    ui->loginShowPassword->setIcon(QIcon(":/resources/open_eye_for_light.png"));
}

void AuthorizationWindow::on_loginShowPassword_released()
{
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    ui->loginShowPassword->setIcon(QIcon(":/resources/hide_eye_for_light.png"));
}

void AuthorizationWindow::on_regShowPassword_pressed()
{
    ui->regPasswordEdit->setEchoMode(QLineEdit::Normal);
    ui->regShowPassword->setIcon(QIcon(":/resources/open_eye_for_light.png"));
}

void AuthorizationWindow::on_regShowPassword_released()
{
    ui->regPasswordEdit->setEchoMode(QLineEdit::Password);
    ui->regShowPassword->setIcon(QIcon(":/resources/hide_eye_for_light.png"));
}

void AuthorizationWindow::on_regShowConfirmPassword_pressed()
{
    ui->confirmPasswordEdit->setEchoMode(QLineEdit::Normal);
    ui->regShowConfirmPassword->setIcon(QIcon(":/resources/open_eye_for_light.png"));
}

void AuthorizationWindow::on_regShowConfirmPassword_released()
{
    ui->confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    ui->regShowConfirmPassword->setIcon(QIcon(":/resources/hide_eye_for_light.png"));
}






