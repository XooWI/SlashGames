#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QTimer>
#include <QDateTime>

#include <QDebug>

#include "Windows/FaQ/faqwindow.h"
#include "Style/customstyle.h"
#include "Windows/Authorization/authorizationwindow.h"
#include "Windows/GameInput/gameinputdialog.h"
#include "Windows/Custom/customwindow.h"
#include "Database/databasemanagement.h"
#include "GameManager/gamemanager.h"
#include "Windows/Balance/balancewindow.h"
#include "Slots/slotsgame.h"

#include <QClipboard>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateBalance(const int &amount);

private slots:
    void checkAndUpdateAccountState();

    void updateBonusTimer();
    void showBalanceChange(int amount);

    void on_getBonusButton_clicked();
    void on_addGameButton_clicked();
    void on_FaQButton_clicked();
    void on_themeButton_clicked();

    void on_rouletteButton_clicked();
    void on_slotsButton_clicked();

    void showMenuProfileInfo_clicked();
    void menuEditProfile_clicked();
    void menuExitProfile_clicked();

    void openAuthorizationWindow();
    void handleLoginSuccessful();

    void on_balanceLabel_clicked();

    void resizeEvent(QResizeEvent *event);

private:
    void initializeTimers();

    Ui::MainWindow *ui;
    QTimer *bonusTimer;
    QTimer *tokenCheckTimer;
    QSettings *settings;
    QMenu *toolMenu;
    DatabaseManagement *dbManager;
    GameManager *gameManager;

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
    void applyTheme(bool darkTheme);
    void loadTheme();

    void updateAccountButtonState();
    void logoutAccount();

    const int BONUS_RELOAD = 3*60*60;    // Время ожидания бонуса в секундах (3 часа = 10800 секунд)
    const int BONUS_AMOUNT = 3000;  // Количество получаемых монет

    int balance;
    int remainingSeconds; // Секунд до получения следующего бонуса
    bool isDarkTheme;
    QString local_ID;

};
#endif // MAINWINDOW_H
