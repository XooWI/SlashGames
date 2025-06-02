#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent),
        ui(new Ui::MainWindow),
        bonusTimer(new QTimer(this)),
        tokenCheckTimer(new QTimer(this)),
        settings(new QSettings(QSettings::IniFormat,  QSettings::UserScope, "SlashGames", "Settings", this)),
        // Настройки хранятся в C:\Users\<NAME_USER>\AppData\Roaming\SlashGames\Menu.ini
        //settings(new QSettings("SlashGames", "Menu", this)),

        dbManager(new DatabaseManagement(settings)),
        toolMenu(new QMenu(this))
{
    ui->setupUi(this);
    gameManager = new GameManager(settings, ui->gamesGridLayout, ui->addGameButton, this);
    initializeTimers();

    updateAccountButtonState();

    loadBalance();
    loadTheme();
    checkBonusAvailability();

    connect(bonusTimer, &QTimer::timeout, this, &MainWindow::updateBonusTimer);
    connect(tokenCheckTimer, &QTimer::timeout, this, &MainWindow::checkAndUpdateAccountState);

    ui->supportLabel->setText(CustomStyle::getTextSupportLabel());
    ui->supportLabel->setOpenExternalLinks(true);


}

void MainWindow::initializeTimers() {
    if (dbManager->checkToken()) {
        QDateTime expiryTime = dbManager->getTokenExpiryTime();
        qint64 timeToExpiry = QDateTime::currentDateTime().msecsTo(expiryTime);
        tokenCheckTimer->setInterval(timeToExpiry);
        tokenCheckTimer->start();
    }
}


MainWindow::~MainWindow()
{
    if (tokenCheckTimer->isActive()) {
        tokenCheckTimer->stop();
    }
    delete ui;
    delete dbManager;
}


void MainWindow::checkAndUpdateAccountState()
{
    logoutAccount();
    CustomWindow General(CustomWindow::GeneralInfo, "Время вашего сеанса истекло!\nНеобходимо заново авторизироваться.","Сеанс истек!", this);
    General.exec();
}


void MainWindow::updateAccountButtonState() {
    if (dbManager->checkToken()) {
        disconnect(ui->accountButton, &QPushButton::clicked, this, &MainWindow::openAuthorizationWindow);
        ui->accountButton->setMenu(toolMenu);
        ui->usernameLabel->setText(dbManager->getUsername());
        return;
    }
    ui->accountButton->setMenu(nullptr); // Удаляем поведение выпадающего меню
    connect(ui->accountButton, &QPushButton::clicked, this, &MainWindow::openAuthorizationWindow);
}


void MainWindow::logoutAccount()
{
    settings->setValue("ID", "");
    settings->setValue("balance", 0);
    settings->setValue("lastBonusTime", QDateTime());
    ui->usernameLabel->setText("Гость");
    loadBalance();
    updateAccountButtonState();
    checkBonusAvailability();
    if (tokenCheckTimer->isActive()) {
        tokenCheckTimer->stop();
    }
}


void MainWindow::loadBalance()
{
    if (dbManager->checkToken()){
        settings->setValue("balance", dbManager->getBalance());
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

    if (dbManager->checkToken()){
        dbManager->updateBalance(balance);
    }
    ui->balanceLabel->setText(QLocale(QLocale::English).toString(balance).replace(",", " ") + "💲");
}

void MainWindow::on_addGameButton_clicked()
{
    GameInputDialog gameInputDialog(false, this);
    if (gameInputDialog.exec() != QDialog::Accepted) {
        return;
    }

    QString gameName = gameInputDialog.getName();
    QString iconPath = gameInputDialog.getIconPath();
    QString executablePath = gameInputDialog.getFilePath();

    // Сохраняем данные игры
    gameManager->saveGame(gameName, iconPath, executablePath);
    gameManager->refreshGamesLayout(this->size().width() > 1600? 3: 2);
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
    QDateTime lastBonusTime;

    if (dbManager->checkToken()) {
        lastBonusTime = dbManager->getLastBonusTime();
    } else {
        lastBonusTime = settings->value("lastBonusTime").toDateTime();
    }

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
    updateBalance(BONUS_AMOUNT);

    QDateTime currentBonusTime = QDateTime::currentDateTime();

    if (dbManager->checkToken()) {
        dbManager->saveLastBonusTime(currentBonusTime);
    } else {
        settings->setValue("lastBonusTime", currentBonusTime);
    }

    startBonusReload(BONUS_RELOAD);
}

void MainWindow::updateBalance(const int &amount)
{
    loadBalance();
    balance += amount;
    saveBalance();
    showBalanceChange(amount);
}

// Кнопки и меню профиль
void MainWindow::showMenuProfileInfo_clicked()
{
    QString name = dbManager->getUsername();
    int balance = dbManager->getBalance();
    QDateTime regDate = dbManager->getRegistredTime();
    QDateTime tokenDate = dbManager->getTokenExpiryTime();

    QString userInfoString = QString("%1,%2,%3,%4")
            .arg(name, QString::number(balance), regDate.toString("yyyy-MM-dd"), tokenDate.toString("yyyy-MM-dd"));

    CustomWindow UserInfo(CustomWindow::UserInfo, userInfoString, QString(), this);
    UserInfo.exec();
}


void MainWindow::menuEditProfile_clicked()
{
    CustomWindow EditProfile(CustomWindow::EditProfile,"", "", this, dbManager);
    EditProfile.exec();
    updateAccountButtonState();
}

void MainWindow::menuExitProfile_clicked()
{
    CustomWindow confirmation(CustomWindow::Confirmation,
                                "Вы действительно хотите выйти?\nВаш прогресс будет потерян!",
                                "Подтверждение выхода", this);
    if (confirmation.exec() == QDialog::Accepted) {
        logoutAccount();
    }
}


void MainWindow::on_rouletteButton_clicked()
{
    CustomWindow rouletteInfo(CustomWindow::GeneralInfo, "Кнопка запуска рулетки", "В разработке...", this);
    rouletteInfo.exec();}


void MainWindow::on_slotsButton_clicked()
{
    this->hide();
    slotsgame *roulette = new slotsgame(this);
    roulette->exec();
    this->show();
    roulette->deleteLater();

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
            } else {
                logoutAccount();
                CustomWindow General(CustomWindow::GeneralInfo, "Время вашего сеанса истекло2!\nНеобходимо заново авторизироваться.","Сеанс истек!", this);
                General.exec();            }
        } else {
            logoutAccount();
        }
    checkBonusAvailability();
    CustomWindow GeneralInfo(CustomWindow::GeneralInfo, "Вы авторизованы!", dbManager->getUsername(), this);
    GeneralInfo.exec();
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
    ui->accountButton->setIcon(QIcon(accountButtonIconPath));

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
    AuthorizationWindow authWindow(settings, dbManager, isDarkTheme, this);
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


void MainWindow::on_balanceLabel_clicked()
{
    BalanceWindow balancewindow(this, dbManager);
    balancewindow.exec();
}


// Реализация события изменения размера
void MainWindow::resizeEvent(QResizeEvent *event)
{
    int currentColumn = (event->size().width())/500;
    if (currentColumn != previousColumn) {
        gameManager->refreshGamesLayout(currentColumn);
        previousColumn = currentColumn;
    }
}
