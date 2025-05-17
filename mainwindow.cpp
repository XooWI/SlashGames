#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent),
        ui(new Ui::MainWindow),
        bonusTimer(new QTimer(this)),
        settings(new QSettings(QSettings::IniFormat,  QSettings::UserScope, "SlashGames", "Menu", this)),
        // Настройки хранятся в C:\Users\<NAME_USER>\AppData\Roaming\SlashGames\Menu.ini
        // Перенести в системный реестр settings(new QSettings("SlashGames", "Menu", this))

      dbManager(new DatabaseManagement(settings)),
      toolMenu(nullptr)
{
    ui->setupUi(this);

    // Загрузка сохраненных данных
    local_ID = settings->value("ID", "").toString();
    loadBalance();
    loadTheme();
    checkBonusAvailability();
    refreshGamesLayout();

    connect(bonusTimer, &QTimer::timeout, this, &MainWindow::updateBonusTimer);
    ui->supportLabel->setText(CustomStyle::getTextSupportLabel());
    ui->supportLabel->setOpenExternalLinks(true);
    connect(bonusTimer, &QTimer::timeout, this, &MainWindow::updateBonusTimer);

    ui->supportLabel->setText(CustomStyle::getTextSupportLabel());
    ui->supportLabel->setOpenExternalLinks(true);
}


void MainWindow::checkAndUpdateAccountState()
{
    if (!dbManager->checkToken()) {
        updateAccountButtonState();
        ui->usernameLabel->setText("Гость");
        QMessageBox::information(this, "Сеанс истек", "Ваш сеанс истек. Необходимо заново авторизироваться.");
        //tokenCheckTimer->stop();
        settings->setValue("balance", "0");
    }
}


void MainWindow::loadBalance()
{
    qDebug() << "Balance checkToken";
    if (local_ID != "" && dbManager->checkToken()){
        settings->setValue("balance", dbManager->getBalance());
        qDebug() << "You login! username: " << dbManager->getUsername() << " Balance:"<<dbManager->getBalance();
    }
    balance = settings->value("balance", 0).toInt();
    ui->balanceLabel->setText(QLocale(QLocale::English).toString(balance).replace(",", " ") + "💲");
}


void MainWindow::loadTheme()
{
    isDarkTheme = settings->value("darkTheme", false).toBool();
    applyTheme(isDarkTheme);
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

void MainWindow::on_getBonusButton_clicked()
{
    balance += BONUS_AMOUNT;
    saveBalance();
    showBalanceChange(BONUS_AMOUNT);

    settings->setValue("lastBonusTime", QDateTime::currentDateTime());
    startBonusReload(BONUS_RELOAD);
}


// Управление пользовательскими играми
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
    saveGame(gameName, iconPath, executablePath);
    refreshGamesLayout();
}


void MainWindow::saveGame(const QString &name, const QString &iconPath, const QString &executablePath)
{
    // Сохраняем количество игр
    gameCount = settings->value("GameCount", GAME_PREINSTALL_COUNT).toInt();
    gameCount++;
    settings->setValue("GameCount", gameCount);

    // Сохраняем данные конкретной игры
    settings->beginGroup(QString("Game_%1").arg(gameCount));
    settings->setValue("Name", name);
    settings->setValue("IconPath", iconPath);
    settings->setValue("ExecutablePath", executablePath);
    settings->endGroup();

}

void MainWindow::refreshGamesLayout()
{
    ui->gamesGridLayout->removeWidget(ui->addGameButton);

    // Очищаем все кнопки
    for (int i = ui->gamesGridLayout->count() - 1; i >= 0; --i) {
        QLayoutItem* item = ui->gamesGridLayout->itemAt(i);
        if (item && item->widget()) {
            if (QPushButton* button = qobject_cast<QPushButton*>(item->widget())) {

                bool isUserGame = button->property("isUserGame").toBool();

                if (isUserGame) {
                    ui->gamesGridLayout->removeWidget(button);
                    button->deleteLater();
                }
            }
        }
    }

    // Загружаем и добавляем игры из настроек
    gameCount = settings->value("GameCount", GAME_PREINSTALL_COUNT).toInt();

    for (int i = GAME_PREINSTALL_COUNT + 1; i <= gameCount; ++i) {
            settings->beginGroup(QString("Game_%1").arg(i));

            QString name = settings->value("Name").toString();
            QString iconPath = settings->value("IconPath").toString();
            QString executablePath = settings->value("ExecutablePath").toString();

            settings->endGroup();

            QPushButton* gameButton = createGameButton(name, iconPath, executablePath);

            // Помечаем кнопку как пользовательскую
            gameButton->setProperty("isUserGame", true);

            int gridIndex = GAME_PREINSTALL_COUNT + (i - (GAME_PREINSTALL_COUNT + 1));
            int row = gridIndex / 2;
            int col = gridIndex % 2;

            ui->gamesGridLayout->addWidget(gameButton, row, col);
    }

    // Обновляем позицию кнопки "+"
    ui->gamesGridLayout->addWidget(ui->addGameButton, gameCount / 2, gameCount % 2);

}


QPushButton* MainWindow::createGameButton(const QString &name, const QString &iconPath, const QString &executablePath)
{
    QPushButton *newGameButton = new QPushButton(ui->gamesContainer);
    newGameButton->setMinimumSize(QSize(MINIMUM_SIZE_WIDTH, MINIMUM_SIZE_HEIGHT));
    newGameButton->setMaximumSize(QSize(MAXIMUM_SIZE_WIDTH, MAXIMUM_SIZE_HEIGHT));

    newGameButton->setStyleSheet(CustomStyle::getCustomNewButton().arg(iconPath));
    newGameButton->setText(name);
    newGameButton->setProperty("gameName", name);
    newGameButton->setProperty("executablePath", executablePath);
    newGameButton->setProperty("iconPath", iconPath);

    newGameButton->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(newGameButton, &QPushButton::customContextMenuRequested, [this, newGameButton](const QPoint& pos){
        QPoint globalPos = newGameButton->mapToGlobal(pos);
        gameButtonRight_clicked(newGameButton, globalPos);
    });

    connect(newGameButton, &QPushButton::clicked, [executablePath]() {
        QProcess::startDetached(executablePath);
    });

    return newGameButton;
}


void MainWindow::gameButtonRight_clicked(QPushButton* gameButton, const QPoint& globalPos) {
    QMenu menu(this);

    QString gameName = gameButton->property("gameName").toString();
    QString iconPath = gameButton->property("iconPath").toString();
    QString executablePath = gameButton->property("executablePath").toString();

    QVariantMap gameData;
        gameData["Name"] = gameName;
        gameData["IconPath"] = iconPath;
        gameData["ExecutablePath"] = executablePath;

    QAction* editAction = menu.addAction("Редактировать");
    editAction->setData(gameData);
    connect(editAction, &QAction::triggered, this, &MainWindow::menuGameEdit_clicked);

    QAction* deleteAction = menu.addAction("Удалить");
    deleteAction->setData(gameName);;
    connect(deleteAction, &QAction::triggered, this, &MainWindow::menuGameDelete_clicked);

    menu.exec(globalPos);
}


void MainWindow::menuGameEdit_clicked()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action) return;

    QVariant gameDataVariant = action->data();
    QVariantMap gameData = gameDataVariant.toMap();

    // Извлекаем старые свойства игры из карты (до редактирования)
    QString currentName = gameData.value("Name").toString();
    QString currentIconPath = gameData.value("IconPath").toString();
    QString currentExecutablePath = gameData.value("ExecutablePath").toString();


    GameInputDialog editDialog(true, this);
    editDialog.setName(currentName);
    editDialog.setIconPath(currentIconPath);
    editDialog.setFilePath(currentExecutablePath);

    if (editDialog.exec() != QDialog::Accepted) {
        return; // Пользователь отменил ввод
    }

    QString newName = editDialog.getName();
    QString newIconPath = editDialog.getIconPath();
    QString newExecutablePath = editDialog.getFilePath();

    // Если ничего не поменялось
    if (newName == currentName && newIconPath == currentIconPath && newExecutablePath == currentExecutablePath) {
        return;
    }

    for (int i = 1; i <= gameCount; ++i) {
        settings->beginGroup(QString("Game_%1").arg(i));
        QString entryName = settings->value("Name").toString();
        QString entryIconPath = settings->value("IconPath").toString();
        QString entryExecutablePath = settings->value("ExecutablePath").toString();
        if (entryName == currentName &&
                entryIconPath == currentIconPath &&
                entryExecutablePath == currentExecutablePath) {
            settings->setValue("Name", newName);
            settings->setValue("IconPath", newIconPath);
            settings->setValue("ExecutablePath", newExecutablePath);
            settings->endGroup();
            break;
        }
        settings->endGroup();
    }

    refreshGamesLayout();
}


void MainWindow::menuGameDelete_clicked()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action) return;

    QString gameName = action->data().toString();
    if (gameName.isEmpty()) return;

    CustomWindow deleteDialog(CustomWindow::DeleteConfirmation, gameName, QString(), this);
    if (deleteDialog.exec() != QDialog::Accepted) {
        return; // Пользователь отменил ввод
    }


    QVector<QVariantMap> userGamesToKeep;
    int currentTotalGameCount = settings->value("GameCount", GAME_PREINSTALL_COUNT).toInt();

    for (int i = GAME_PREINSTALL_COUNT + 1; i <= currentTotalGameCount; ++i) {
        settings->beginGroup(QString("Game_%1").arg(i));
        QString currentName = settings->value("Name").toString();
        if (!currentName.isEmpty() && currentName != gameName) {
            QVariantMap game;
            game["Name"] = currentName;
            game["IconPath"] = settings->value("IconPath");
            game["ExecutablePath"] = settings->value("ExecutablePath");
            userGamesToKeep.append(game);
        }
        settings->endGroup();
    }

    // Очищаем все пользовательские игры из настроек
    for (int i = GAME_PREINSTALL_COUNT + 1; i <= currentTotalGameCount; ++i) {
        settings->remove(QString("Game_%1").arg(i));
    }

    for (int i = 0; i < userGamesToKeep.size(); ++i) {
        int newGameIndex = GAME_PREINSTALL_COUNT + 1 + i;
        settings->beginGroup(QString("Game_%1").arg(newGameIndex));
        settings->setValue("Name", userGamesToKeep[i]["Name"]);
        settings->setValue("IconPath", userGamesToKeep[i]["IconPath"]);
        settings->setValue("ExecutablePath", userGamesToKeep[i]["ExecutablePath"]);
        settings->endGroup();
    }

    gameCount = GAME_PREINSTALL_COUNT + userGamesToKeep.size(); // Обновляем член класса
    settings->setValue("GameCount", gameCount);

    // Обновляем поля
    refreshGamesLayout();

}

// Конец управления играми


// Кнопки и меню профиль
void MainWindow::showMenuProfileInfo_clicked()
{
    qDebug() << "Пользователь авторизован.";
    CustomWindow General(CustomWindow::GeneralInfo, "Вы авторизованы!", dbManager->getUsername(), this);
    General.exec();
}


void MainWindow::menuEditProfile_clicked()
{
    qDebug() << "menuEditProfile checkToken";
    if(dbManager->checkToken()){
        qDebug() << "You enter button menu edit!";
        CustomWindow UserInfo(CustomWindow::UserInfo, "You edit!",QString(), this);
        UserInfo.exec();
        return;
    }
}

void MainWindow::menuExitProfile_clicked()
{
    qDebug() << "menuExitProfile checkToken";

    if(dbManager->checkToken()){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Подтверждение",
                                    "Вы действительно хотите выйти?\nВаш прогресс будет потерян!",
                                    QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            settings->setValue("ID", "0");
            settings->setValue("balance", "0");
            local_ID = settings->value("ID", "").toString();
            loadBalance();
            updateAccountButtonState();
            qDebug() << "You 0 ";
        }
        return;
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
    local_ID = settings->value("ID", "").toString();
    loadBalance();
    updateAccountButtonState();
    CustomWindow UserInfo(CustomWindow::UserInfo, "Вы авторизованы!", dbManager->getUsername(), this);
    UserInfo.exec();
}

void MainWindow::applyTheme(bool darkTheme)
{
    isDarkTheme = darkTheme;
    QString themeIconPath, faqIconPath, accountButtonIconPath, myProfileIconPath, editProfileIconPath, exitIconPath;

    if (isDarkTheme) {
        setStyleSheet(CustomStyle::getDarkThemeStyle());

        themeIconPath = ":/resources/theme_for_dark.png";
        faqIconPath = ":/resources/faq_for_dark.png";
        accountButtonIconPath = ":/resources/user_for_dark.png";
        myProfileIconPath = ":/resources/my_profile_for_dark.png";
        editProfileIconPath = ":/resources/edit_for_dark.png";
        exitIconPath = ":/resources/exit.png";
    } else {
        setStyleSheet(CustomStyle::getLightThemeStyle());

        themeIconPath = ":/resources/theme_for_light.png";
        faqIconPath = ":/resources/faq_for_light.png";
        accountButtonIconPath = ":/resources/user_for_light.png";
        myProfileIconPath = ":/resources/my_profile_for_light.png";
        editProfileIconPath = ":/resources/edit_for_light.png";
        exitIconPath = ":/resources/exit.png";
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

    updateAccountButtonState();
}


void MainWindow::updateAccountButtonState() {
    disconnect(ui->accountButton, &QPushButton::clicked, this, &MainWindow::openAuthorizationWindow);
    qDebug() << "updateAccountButtonState checkToken";
    if (dbManager->checkToken()) {
        ui->accountButton->setMenu(toolMenu);
        ui->usernameLabel->setText(dbManager->getUsername());
    } else {
        ui->accountButton->setMenu(nullptr); // Удаляем поведение выпадающего меню
        connect(ui->accountButton, &QPushButton::clicked, this, &MainWindow::openAuthorizationWindow); // Подключаем сигнал clicked
    }
}

void MainWindow::openAuthorizationWindow()
{
    AuthorizationWindow authWindow(settings, dbManager, this);
    connect(&authWindow, &AuthorizationWindow::loginSuccessful, this, &MainWindow::handleLoginSuccessful);
    authWindow.exec();
}


void MainWindow::saveBalance()
{
    settings->setValue("balance", balance);
    qDebug() << "saveBalance checkToken";
    if (dbManager->checkToken()){
        if(dbManager->updateBalance(balance)){
            qDebug() << "Update balance from DB";
        } else{
            qDebug() << "Error update balance from DB";
        }
    }

    ui->balanceLabel->setText(QLocale(QLocale::English).toString(balance).replace(",", " ") + "💲");
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



MainWindow::~MainWindow()
{

    delete ui;
    delete dbManager;
}



