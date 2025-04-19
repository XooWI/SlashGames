#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDateTime>
#include <QMessageBox>


#include <QDebug>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      bonusTimer(new QTimer(this)),
      settings(new QSettings("SlashGames", "Menu", this))
{
    ui->setupUi(this);
    toolMenu = new QMenu(this);

    // Загрузка сохраненных данных
    loadTheme();
    loadBalance();
    checkBonusAvailability();

    connect(bonusTimer, &QTimer::timeout, this, &MainWindow::updateBonusTimer);

    ui->supportLabel->setText("Нет нужной игры? "
                              "<a href='https://t.me/SlashGames_support_bot' style='color: #2a7ae9; text-decoration: none;'>"
                              "Напишите нам!");
    ui->supportLabel->setOpenExternalLinks(true);
}


void MainWindow::loadBalance()
{
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

void MainWindow::on_addGameButton_clicked()
{

    GameInputDialog gameInputDialog(this);
    if (gameInputDialog.exec() != QDialog::Accepted) {
        return; // Пользователь отменил ввод
    }

    QString gameName = gameInputDialog.getName();
    QString iconPath = gameInputDialog.getIconPath();
    QString executablePath = gameInputDialog.getExecutablePath();

    // Создаем новую кнопку игры
    QPushButton *newGameButton = new QPushButton(ui->gamesContainer);
    newGameButton->setMinimumSize(QSize(400, 300));
    newGameButton->setMaximumSize(QSize(600, 300));
    newGameButton->setCursor(Qt::PointingHandCursor);


    newGameButton->setStyleSheet(CustomStyle::getCustomNewButton().arg(iconPath));
    newGameButton->setText(gameName);

    newGameButton->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(newGameButton, &QPushButton::customContextMenuRequested, [this, newGameButton](const QPoint&){
        QPoint centerPos = newGameButton->rect().center();
        QPoint globalPos = newGameButton->mapToGlobal(centerPos);
        QString gameName = newGameButton->property("gameName").toString();
        gameButtonRight_clicked(gameName, globalPos);
    });

    // Вычисляем позицию для новой кнопки
    int row = gameCount / 2;  // Учитываем 2 стандартные игры
    int column = gameCount % 2;

    // Добавляем новую кнопку в сетку
    ui->gamesGridLayout->addWidget(newGameButton, row, column);
    gameCount++;

    // Перемещаем кнопку "+" в следующую позицию
    ui->gamesGridLayout->removeWidget(ui->addGameButton);
    int newRow = gameCount / 2;
    int newCol = gameCount % 2;
    ui->gamesGridLayout->addWidget(ui->addGameButton, newRow, newCol);

    // Подключаем обработчик клика для запуска игры
    connect(newGameButton, &QPushButton::clicked, [executablePath]() {QProcess::startDetached(executablePath);});

}



void MainWindow::gameButtonRight_clicked(const QString& gameName, const QPoint& globalPos)
{
    QMenu menu(this);

    QAction* editAction = menu.addAction("Редактировать");
    connect(editAction, &QAction::triggered, this, &MainWindow::menuGameEdit_clicked);

    QAction* deleteAction = menu.addAction("Удалить");
    connect(deleteAction, &QAction::triggered, this, &MainWindow::menuGameDelete_clicked);

    menu.exec(globalPos);
}


void MainWindow::menuProfile_clicked()
{
       AuthorizationWindow authWindow(this);
       authWindow.exec();
}

void MainWindow::menuGameEdit_clicked()
{
    qDebug() << "Press edit game button";
}

void MainWindow::menuGameDelete_clicked()
{
    qDebug() << "Press delete game button";
}

void MainWindow::menuEdit_clicked()
{
    qDebug() << "Press edit button";
}

void MainWindow::menuExit_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение",
                                "Вы действительно хотите выйти?",
                                QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        qApp->quit();
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


void MainWindow::applyTheme(bool darkTheme)
{
    isDarkTheme = darkTheme;
    QString themeIconPath, faqIconPath, accountIconPath, myProfileIconPath, editProfileIconPath;
    toolMenu->clear();

    if (isDarkTheme) {
        setStyleSheet(CustomStyle::getDarkThemeStyle());

        themeIconPath = ":/resources/theme_for_dark.png";
        faqIconPath = ":/resources/faq_for_dark.png";
        accountIconPath = ":/resources/user_for_dark.png";
        myProfileIconPath = ":/resources/my_profile_for_dark.png";
        editProfileIconPath = ":/resources/edit_for_dark.png";
    } else {
        setStyleSheet(CustomStyle::getLightThemeStyle());

        themeIconPath = ":/resources/theme_for_light.png";
        faqIconPath = ":/resources/faq_for_light.png";
        accountIconPath = ":/resources/user_for_light.png";
        myProfileIconPath = ":/resources/my_profile_for_light.png";
        editProfileIconPath = ":/resources/edit_for_light.png";
    }

    ui->themeButton->setIcon(QIcon(themeIconPath));
    ui->FaQButton->setIcon(QIcon(faqIconPath));
    ui->accountButton->setIcon(QIcon(accountIconPath));

    // Меню для кнопки профиля
    QAction *profileAction = toolMenu->addAction(QIcon(myProfileIconPath), "Мой профиль");
    connect(profileAction, &QAction::triggered, this, &MainWindow::menuProfile_clicked);
    QAction *editAction = toolMenu->addAction(QIcon(editProfileIconPath), "Редактировать");
    connect(editAction, &QAction::triggered, this, &MainWindow::menuEdit_clicked);
    QAction *exitAction = toolMenu->addAction(QIcon(":/resources/exit.png"), "Выход");
    connect(exitAction, &QAction::triggered, this, &MainWindow::menuExit_clicked);
    ui->accountButton->setMenu(toolMenu);

    settings->setValue("darkTheme", isDarkTheme);
}


void MainWindow::saveBalance()
{
    settings->setValue("balance", balance);
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
}



