#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "faqwindow.h"
#include "customstyle.h"
#include "authorizationwindow.h"
#include"gameinputdialog.h"
#include"deletegamedialog.h"
#include"databasemanagement.h"

#include <QMainWindow>
#include <QSettings>
#include <QTimer>
#include <QDateTime>
#include <QProcess>

// Удалить после обновления!
#include <QMessageBox>
#include <QDebug>


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
    void updateBonusTimer();
    void showBalanceChange(int amount);
    void on_getBonusButton_clicked();
    void on_addGameButton_clicked();
    void on_FaQButton_clicked();
    void on_themeButton_clicked();


    void on_rouletteButton_clicked();
    void on_slotsButton_clicked();

    void menuProfile_clicked();
    void menuEdit_clicked();
    void menuExit_clicked();


    void gameButtonRight_clicked(QPushButton* gameButton, const QPoint& globalPos);
    void menuGameEdit_clicked();
    void menuGameDelete_clicked();

private:
    const int BONUS_RELOAD = 20;    // Время ожидания бонуса в секундах
    const int BONUS_AMOUNT = 3000;  // Количество получаемых монет (3 часа = 10800 секунд)

    Ui::MainWindow *ui;
    QTimer *bonusTimer;
    QSettings *settings;
    QMenu *toolMenu;

    int balance;
    int remainingSeconds;
    bool isDarkTheme;
    QString local_ID;

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



    const int GAME_PREINSTALL_COUNT = 2;
    const int MINIMUM_SIZE_WIDTH = 400;
    const int MINIMUM_SIZE_HEIGHT = 300;
    const int MAXIMUM_SIZE_WIDTH = 1000;
    const int MAXIMUM_SIZE_HEIGHT = 500;

    int gameCount;

    // Работа с играми
    QPushButton* createGameButton(const QString &name, const QString &iconPath, const QString &executablePath);
    void saveGame(const QString &name, const QString &iconPath, const QString &executablePath);
    void refreshGamesLayout();


    DatabaseManagement *dbManager;
};
#endif // MAINWINDOW_H
