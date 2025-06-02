#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QSettings>
#include <QGridLayout>
#include <QPushButton>
#include <QProcess>
#include <QMenu>
#include <QAction>
#include <QVariantMap>

#include "Style/customstyle.h"
#include "Windows/GameInput/gameinputdialog.h"
#include "Windows/Custom/customwindow.h"

class GameManager : public QObject
{
    Q_OBJECT

public:
    GameManager(QSettings *settings, QGridLayout *gamesLayout, QPushButton *addGameButton, QObject *parent = nullptr);

    void refreshGamesLayout(int column = 2);
    void saveGame(const QString &name, const QString &iconPath, const QString &executablePath);

private slots:
    void gameButtonRight_clicked(QPushButton* gameButton, const QPoint& globalPos);
    void menuGameEdit_clicked();
    void menuGameDelete_clicked();

private:
    QPushButton* createGameButton(const QString &name, const QString &iconPath, const QString &executablePath);

    QSettings *settings;
    QGridLayout *gamesLayout;
    QPushButton *addGameButton;

    int gameCount;
    const int MINIMUM_SIZE_WIDTH = 400;
    const int MINIMUM_SIZE_HEIGHT = 300;
    const int MAXIMUM_SIZE_WIDTH = 1000;
    const int MAXIMUM_SIZE_HEIGHT = 500;
    const int GAME_PREINSTALL_COUNT = 2;
    const int MAX_CUSTOM_GAME_COUNT = 20; // -1 - неограничено

};

#endif // GAMEMANAGER_H
