#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSettings>

#include "faqwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void claimBonus();
    void updateBonusTimer();
    void showBalanceChange(int amount);
    void on_addGameButton_clicked();
    void on_FaQButton_clicked();
    void on_themeButton_clicked();


private:
    static constexpr int BONUS_RELOAD = 20;    // Время ожидания бонуса в секундах
    static constexpr int BONUS_AMOUNT = 3000;  // Количество получаемых монет

    Ui::MainWindow *ui;
    QTimer *bonusTimer;
    QSettings *settings;

    int balance;
    int remainingSeconds;

    // Управление балансом
    void loadBalance();
    void saveBalance();
    void updateBalanceDisplay();

    // Работа с бонусом
    void checkBonusAvailability();
    void startBonusReload(int seconds);
    void enableBonusButton();
    void updateBonusButtonAppearance();

    // Смена темы
    QString getDarkThemeStyle();
    QString getLightThemeStyle();
    bool isDarkTheme = false;
};
#endif // MAINWINDOW_H
