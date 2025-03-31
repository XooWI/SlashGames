#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSettings>
#include <QTimer>
#include <QDateTime>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      balance(0),
      bonusTimer(new QTimer(this)),
      settings(new QSettings("SlashGames", "Menu", this))
{
    ui->setupUi(this);

    connect(bonusTimer, &QTimer::timeout, this, &MainWindow::updateBonusTimer);

    // Загрузка сохраненных данных
    loadBalance();
    checkBonusAvailability();


    connect(ui->getBonusButton, &QPushButton::clicked, this, &MainWindow::claimBonus);

    ui->supportLabel->setText("Нет нужной игры? "
                              "<a href='https://t.me/SlashGames_support_bot' style='color: #2a7ae9; text-decoration: none;'>"
                              "Напишите нам!");
    ui->supportLabel->setOpenExternalLinks(true);
}

void MainWindow::loadBalance()
{
    balance = settings->value("balance", 0).toInt();
    ui->balanceLabel->setText(QString("%1💲").arg(balance));
}

void MainWindow::saveBalance()
{
    settings->setValue("balance", balance);
    ui->balanceLabel->setText(QString("%1💲").arg(balance));
}


void MainWindow::showBalanceChange(int amount)
{
    QString text;
    QString color;

        if (amount > 0) {
            text = QString("+%1").arg(amount);
            color = "#4CAF50";
        } else {
            text = QString("%1").arg(amount);
            color = "#f44336";
        }

        ui->balanceChangeLabel->setText(text);
        ui->balanceChangeLabel->setStyleSheet(QString("color: %1;").arg(color));

        // Очистка текста
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

void MainWindow::claimBonus()
{
    const int BONUS_AMOUNT = 3000;
    balance += BONUS_AMOUNT;
    saveBalance();
    showBalanceChange(BONUS_AMOUNT);

    settings->setValue("lastBonusTime", QDateTime::currentDateTime());
    startBonusReload(BONUS_RELOAD);
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

    QString timeText = QString("00:%1").arg(remainingSeconds, 2, 10, QChar('0'));
    QString style = QString(
        "QPushButton#getBonusButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                              stop:0 #4CAF50,"
        "                              stop:%1 #4CAF50,"
        "                              stop:%2 #808080);"
        "    color: #ffffff;"
        "    border: 2px solid #45a049;"
        "    border-radius: 25px;"
        "    padding: 15px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
    ).arg(progress).arg(progress + 0.01);

    ui->getBonusButton->setText(timeText);
    ui->getBonusButton->setStyleSheet(style);
    ui->getBonusButton->setEnabled(false);
}


void MainWindow::enableBonusButton()
{
    bonusTimer->stop();
    ui->getBonusButton->setText("Получить Бонус!");
    ui->getBonusButton->setEnabled(true);

    QString style = "QPushButton#getBonusButton {"
                   "    background-color: #4CAF50;"
                   "    color: #ffffff;"
                   "    border: 2px solid #45a049;"
                   "    border-radius: 25px;"
                   "    padding: 15px;"
                   "    font-size: 14px;"
                   "    font-weight: bold;"
                   "}"
                   "QPushButton#getBonusButton:hover {"
                   "    background-color: #45a049;"
                   "}"
                   "QPushButton#getBonusButton:pressed {"
                   "    background-color: #3d8b40;"
                   "}";

    ui->getBonusButton->setStyleSheet(style);
}


void MainWindow::on_addGameButton_clicked()
{
    QMessageBox::information(this, "В разработке...", "Это окно находится в разработке (GitHub: MrAnonim114).");
}

MainWindow::~MainWindow()
{
    delete ui;
}

