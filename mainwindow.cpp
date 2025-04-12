#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDateTime>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      bonusTimer(new QTimer(this)),
      settings(new QSettings("SlashGames", "Menu", this))
{
    ui->setupUi(this);

    // Загрузка сохраненных данных
    loadTheme();
    loadBalance();
    checkBonusAvailability();

    connect(bonusTimer, &QTimer::timeout, this, &MainWindow::updateBonusTimer);
    connect(ui->getBonusButton, &QPushButton::clicked, this, &MainWindow::claimBonus);

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
    apply_theme(isDarkTheme);
}


void MainWindow::on_FaQButton_clicked()
{
    FAQWindow faqWindow(this);
    faqWindow.exec();
}


void MainWindow::on_themeButton_clicked()
{
    apply_theme(!isDarkTheme);
}


void MainWindow::on_addGameButton_clicked()
{
   QMessageBox::information(this, "В разработке...", "Кнопка добавления новых игр находится в разработке\n\n(GitHub: MrAnonim114).");
}


void MainWindow::on_accountButton_clicked()
{
    AuthorizationWindow authWindow(this);
    authWindow.exec();
}


void MainWindow::on_rouletteButton_clicked()
{
    QMessageBox::information(this, "В разработке...", "Кнопка запуска рулетки");
}


void MainWindow::on_slotsButton_clicked()
{
    QMessageBox::information(this, "В разработке...", "Кнопка запуска слотов");
}


void MainWindow::apply_theme(bool darkTheme)
{
    isDarkTheme = darkTheme;
    QString themeIconPath, faqIconPath, accountIconPath;

    if (isDarkTheme) {
        setStyleSheet(CustomStyle::getDarkThemeStyle());

        themeIconPath = ":/resources/theme_for_dark.png";
        faqIconPath = ":/resources/faq_for_dark.png";
        accountIconPath = ":/resources/user_for_dark.png";
    } else {
        setStyleSheet(CustomStyle::getLightThemeStyle());

        themeIconPath = ":/resources/theme_for_light.png";
        faqIconPath = ":/resources/faq_for_light.png";
        accountIconPath = ":/resources/user_for_light.png";
    }

    ui->themeButton->setIcon(QIcon(themeIconPath));
    ui->FaQButton->setIcon(QIcon(faqIconPath));
    ui->accountButton->setIcon(QIcon(accountIconPath));

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


void MainWindow::claimBonus()
{
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




