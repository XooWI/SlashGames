#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent),
        ui(new Ui::MainWindow),
        bonusTimer(new QTimer(this)),
        tokenCheckTimer(new QTimer(this)),
        settings(new QSettings(QSettings::IniFormat,  QSettings::UserScope, "SlashGames", "Menu", this)),
        // Настройки хранятся в C:\Users\<NAME_USER>\AppData\Roaming\SlashGames\Menu.ini
        // Перенести в системный реестр settings(new QSettings("SlashGames", "Menu", this))

      dbManager(new DatabaseManagement(settings)),
      toolMenu(new QMenu(this)) // Инициализация toolMenu
{
    ui->setupUi(this);
    gameManager = new GameManager(settings, ui->gamesGridLayout, ui->addGameButton, this);

    // Загрузка сохраненных данных
    qDebug() << "Install token timer";
    if (dbManager->checkToken()) {
            QDateTime expiryTime = dbManager->getTokenExpiryTime();
            qint64 timeToExpiry = QDateTime::currentDateTime().msecsTo(expiryTime);
            tokenCheckTimer->setInterval(timeToExpiry);
            tokenCheckTimer->start();
            qDebug() << "Таймер выхода (автоматический вход) запущен. Сработает в:" << expiryTime.toString(Qt::ISODate);
            updateAccountButtonState();

    } else {
        qDebug() << "Install token timer Токена нет";
        updateAccountButtonState(); // Установить состояние "Гость", если нет токена или он недействителен
        }

    loadBalance();
    loadTheme();
    checkBonusAvailability();
    gameManager->refreshGamesLayout();

    connect(bonusTimer, &QTimer::timeout, this, &MainWindow::updateBonusTimer);
    connect(tokenCheckTimer, &QTimer::timeout, this, &MainWindow::checkAndUpdateAccountState);

    ui->supportLabel->setText(CustomStyle::getTextSupportLabel());
    ui->supportLabel->setOpenExternalLinks(true);

}


void MainWindow::checkAndUpdateAccountState()
{
    QDateTime expiryTime = dbManager->getTokenExpiryTime();
    if (expiryTime.isValid() && QDateTime::currentDateTime() >= expiryTime) {
        logoutAccount();
        CustomWindow General(CustomWindow::GeneralInfo, "Время вашего сеанса истекло!\nНеобходимо заново авторизироваться.","Сеанс истек!", this);
        General.exec();
    } else if (!dbManager->checkToken()) {
        // Дополнительная проверка, если токен стал недействителен по другой причине
        logoutAccount();
        QMessageBox::information(this, "Сеанс истек2", "Ваш сеанс истек. Необходимо заново авторизироваться.");
            } else {
                updateAccountButtonState();
            }
}



void MainWindow::updateAccountButtonState() {
    qDebug() << "updateAccountButtonState checkToken";
    if (dbManager->checkToken()) {
        disconnect(ui->accountButton, &QPushButton::clicked, this, &MainWindow::openAuthorizationWindow);
        ui->accountButton->setMenu(toolMenu);
        ui->usernameLabel->setText(dbManager->getUsername());
        return;
    }
    ui->accountButton->setMenu(nullptr); // Удаляем поведение выпадающего меню
    connect(ui->accountButton, &QPushButton::clicked, this, &MainWindow::openAuthorizationWindow); // Подключаем сигнал clicked
}


void MainWindow::logoutAccount()
{
    qDebug() << "logoutAccount Выход из аккаунта";
    settings->setValue("ID", "");
    settings->setValue("balance", 0);
    ui->usernameLabel->setText("Гость");
    loadBalance();
    updateAccountButtonState();
    if (tokenCheckTimer->isActive()) {
        tokenCheckTimer->stop();
        qDebug() << "logoutAccount Таймер проверки токена остановлен при выходе.";
    }
}


void MainWindow::loadBalance()
{
    qDebug() << "Balance checkToken";
    if (dbManager->checkToken()){
        settings->setValue("balance", dbManager->getBalance());
        qDebug() << "You login! Username: " << dbManager->getUsername() << " Balance from DB:"<<dbManager->getBalance();
    } else{ui->usernameLabel->setText("Гость");}
    balance = settings->value("balance", 0).toInt();
    ui->balanceLabel->setText(QLocale(QLocale::English).toString(balance).replace(",", " ") + "💲");
}


void MainWindow::loadTheme()
{
    isDarkTheme = settings->value("darkTheme", false).toBool();
    applyTheme(isDarkTheme);
}


void MainWindow::saveBalance()
{
    settings->setValue("balance", balance);
    qDebug() << "saveBalance checkToken";

    if (dbManager->checkToken()){
        if (dbManager->updateBalance(balance)){
            qDebug() << "Update balance from DB";
        } else{
            qDebug() << "Error update balance from DB";
        }
    }
    ui->balanceLabel->setText(QLocale(QLocale::English).toString(balance).replace(",", " ") + "💲");
}

void MainWindow::on_addGameButton_clicked() // Добавили GameManager::
{
    // Используем m_mainWindowParent в качестве родителя для диалога
    GameInputDialog gameInputDialog(false, this);
    if (gameInputDialog.exec() != QDialog::Accepted) {
        return;
    }

    QString gameName = gameInputDialog.getName();
    QString iconPath = gameInputDialog.getIconPath();
    QString executablePath = gameInputDialog.getFilePath();

    // Сохраняем данные игры
    gameManager->saveGame(gameName, iconPath, executablePath);
    gameManager->refreshGamesLayout();
}


void MainWindow::showBalanceChange(int amount)
{
    QString text, color;

        if (amount > 0) {
            text = QString("+%1").arg(QLocale(QLocale::English).toString(amount).replace(",", " "));
            color = "#4CAF50";
        } else {
            text = QString("%1").arg(QLocale(QLocale::English).toString(amount).replace(",", " "));
            color = "#f44336";
        }

        ui->balanceChangeLabel->setText(text);
        ui->balanceChangeLabel->setStyleSheet(QString("color: %1;").arg(color));

        // Очистка текста через 2 секунды
        QTimer::singleShot(2000, this, [this]() {
                if (ui && ui->balanceChangeLabel) {
                    ui->balanceChangeLabel->setText("");
                    ui->balanceChangeLabel->setStyleSheet("");
                }
        });
}


void MainWindow::checkBonusAvailability()
{
    QDateTime lastBonusTime = settings->value("lastBonusTime").toDateTime();

    if (lastBonusTime.isValid()) {
        // Вычисляем, сколько секунд прошло с момента последнего получения бонуса
        int secondsPassed = lastBonusTime.secsTo(QDateTime::currentDateTime());

        // Вычисляем, сколько секунд осталось до следующего бонуса
        int remainingSeconds = BONUS_RELOAD - secondsPassed;

        if (remainingSeconds > 0) {
            startBonusReload(remainingSeconds);
            return;
        }
    }

    enableBonusButton();
}

void MainWindow::startBonusReload(int seconds)
{
    remainingSeconds = seconds;
    bonusTimer->start(1000);
    updateBonusButtonAppearance();
}


void MainWindow::updateBonusTimer()
{
    if (--remainingSeconds <= 0) {
        enableBonusButton();
        return;
    }

    updateBonusButtonAppearance();
}


// Обновление внешнего вида кнопки с прогрессом
void MainWindow::updateBonusButtonAppearance()
{
    double progress = qBound(0.0, (double)(BONUS_RELOAD - remainingSeconds) / BONUS_RELOAD, 1.0);

    int hours = remainingSeconds / 3600;
    int minutes = (remainingSeconds % 3600) / 60;
    int seconds = remainingSeconds % 60;
    QString timeText = QString("%1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));

    QString style = QString(CustomStyle::getActionBonusButtonStyle()).arg(progress).arg(progress + 0.01);

    ui->getBonusButton->setText(timeText);
    ui->getBonusButton->setStyleSheet(style);
    ui->getBonusButton->setEnabled(false);
}


void MainWindow::enableBonusButton()
{
    bonusTimer->stop();
    ui->getBonusButton->setText("Получить Бонус!");
    ui->getBonusButton->setEnabled(true);

    ui->getBonusButton->setStyleSheet(CustomStyle::getEnableBonusButtonStyle());
}


void MainWindow::on_getBonusButton_clicked()
{
    loadBalance();
    balance += BONUS_AMOUNT;
    saveBalance();
    showBalanceChange(BONUS_AMOUNT);

    settings->setValue("lastBonusTime", QDateTime::currentDateTime());
    startBonusReload(BONUS_RELOAD);
}



// Кнопки и меню профиль
void MainWindow::showMenuProfileInfo_clicked()
{
    qDebug() << "Пользователь авторизован.";
    CustomWindow General(CustomWindow::GeneralInfo, "Вы авторизованы!", dbManager->getUsername(), this);
    General.exec();
}


void MainWindow::menuEditProfile_clicked()
{
    qDebug() << "You enter button menu edit!";
    CustomWindow UserInfo(CustomWindow::UserInfo, "You edit!",QString(), this);
    UserInfo.exec();
    return;
}

void MainWindow::menuExitProfile_clicked()
{
    qDebug() << "menuExitProfile checkToken";
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение",
                                "Вы действительно хотите выйти?\nВаш прогресс будет потерян!",
                                QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        logoutAccount();
        qDebug() << "You exit from profile ";
    }
}


void MainWindow::on_rouletteButton_clicked()
{
    QMessageBox::information(this, "В разработке...", "Кнопка запуска рулетки");
}


void MainWindow::on_slotsButton_clicked()
{
    QMessageBox::information(this, "В разработке...", "Кнопка запуска слотов");
}


void MainWindow::handleLoginSuccessful()
{
    updateAccountButtonState();

    loadBalance();
    QDateTime expiryTime = dbManager->getTokenExpiryTime();
        if (expiryTime.isValid()) {
            QDateTime currentTime = QDateTime::currentDateTime();
            if (expiryTime > currentTime) {
                qint64 timeToExpiry = currentTime.msecsTo(expiryTime);
                tokenCheckTimer->setInterval(timeToExpiry);
                tokenCheckTimer->start();
                qDebug() << "handleLoginSuccessful Таймер выхода запущен. Сработает в:" << expiryTime.toString(Qt::ISODate);
            } else {
                qDebug() << "Срок действия токена уже истек при входе.";
                logoutAccount();
                QMessageBox::information(this, "Сеанс истек", "Время вашего сеанса истекло. Необходимо заново авторизоваться.");
            }
        } else {
            qDebug() << "Не удалось получить срок действия токена.";
            tokenCheckTimer->start(TOKEN_CHECK_INTERVAL * 1000);
        }
    CustomWindow UserInfo(CustomWindow::UserInfo, "Вы авторизованы!", dbManager->getUsername(), this);
    UserInfo.exec();
}


void MainWindow::applyTheme(bool darkTheme)
{
    isDarkTheme = darkTheme;
    QString themeIconPath, faqIconPath, accountButtonIconPath, myProfileIconPath, editProfileIconPath, exitIconPath;

    if (isDarkTheme) {
        setStyleSheet(CustomStyle::getDarkThemeStyle());

        themeIconPath = ":/icons/dark_mode/button_toggle_theme.png";
        faqIconPath = ":/icons/dark_mode/button_faq.png";
        accountButtonIconPath = ":/icons/dark_mode/button_profile.png";
        myProfileIconPath = ":/icons/dark_mode/menu_my_profile.png";
        editProfileIconPath = ":/icons/dark_mode/menu_edit.png";
        exitIconPath = ":/icons/universal/menu_exit.png";
    } else {
        setStyleSheet(CustomStyle::getLightThemeStyle());

        themeIconPath = ":/icons/light_mode/button_toggle_theme.png";
        faqIconPath = ":/icons/light_mode/button_faq.png";
        accountButtonIconPath = ":/icons/light_mode/button_profile.png";
        myProfileIconPath = ":/icons/light_mode/menu_my_profile.png";
        editProfileIconPath = ":/icons/light_mode/menu_edit.png";
        exitIconPath = ":/icons/universal/menu_exit.png";
    }

    // Обновляем иконки кнопок
    ui->themeButton->setIcon(QIcon(themeIconPath));
    ui->FaQButton->setIcon(QIcon(faqIconPath));
    ui->accountButton->setIcon(QIcon(accountButtonIconPath)); // Устанавливаем иконку главной кнопки

    if (!toolMenu) {
            toolMenu = new QMenu(this);
        } else {
            toolMenu->clear();
        }

        // Создаем действия меню и назначаем им иконки
        QAction *profileAction = toolMenu->addAction(QIcon(myProfileIconPath), "Мой профиль");
        connect(profileAction, &QAction::triggered, this, &MainWindow::showMenuProfileInfo_clicked);

        QAction *editAction = toolMenu->addAction(QIcon(editProfileIconPath), "Редактировать");
        connect(editAction, &QAction::triggered, this, &MainWindow::menuEditProfile_clicked);

        QAction *exitAction = toolMenu->addAction(QIcon(exitIconPath), "Выход");
        connect(exitAction, &QAction::triggered, this, &MainWindow::menuExitProfile_clicked);


    settings->setValue("darkTheme", isDarkTheme);
}


void MainWindow::openAuthorizationWindow()
{
    AuthorizationWindow authWindow(settings, dbManager, this);
    connect(&authWindow, &AuthorizationWindow::loginSuccessful, this, &MainWindow::handleLoginSuccessful);
    authWindow.exec();
}


void MainWindow::on_FaQButton_clicked()
{
    FAQWindow faqWindow(this);
    faqWindow.exec();
}


void MainWindow::on_themeButton_clicked()
{
    applyTheme(!isDarkTheme);
}


MainWindow::~MainWindow()
{
    if (tokenCheckTimer->isActive()) {
        tokenCheckTimer->stop();
    }
    delete ui;
    delete dbManager;
}




