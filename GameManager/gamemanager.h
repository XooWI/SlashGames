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
#include <QDebug>

#include "Style/customstyle.h"
#include "Windows/GameInput/gameinputdialog.h"
#include "Windows/Custom/customwindow.h"

class GameManager : public QObject
{
    Q_OBJECT

public:
    // Конструктор: теперь принимает QSettings, QGridLayout, QPushButton и опционально MainWindow* для диалогов
    explicit GameManager(QSettings *settings, QGridLayout *gamesLayout,
                         QPushButton *addGameButton, QObject *parent = nullptr);

    //int getGameCount() const;
    void refreshGamesLayout(); // Метод для обновления макета игр
    void saveGame(const QString &name, const QString &iconPath, const QString &executablePath);

private slots:
    // Приватные слоты для внутренней логики GameManager
    //void onGameButtonClicked(); // Обработчик нажатия кнопки игры
    //void onGameButtonCustomContextMenuRequested(const QPoint &pos); // Обработчик контекстного меню

    void gameButtonRight_clicked(QPushButton* gameButton, const QPoint& globalPos); // Обработчик контекстного меню
    void menuGameEdit_clicked();    // Обработчик редактирования игры из меню
    void menuGameDelete_clicked();  // Обработчик удаления игры из меню

private:
    QPushButton* createGameButton(const QString &name, const QString &iconPath, const QString &executablePath);

    QSettings *m_settings; // Используем m_settings вместо settings
    QGridLayout *m_gamesLayout; // Используем m_gamesLayout вместо ui->gamesGridLayout
    QPushButton *m_addGameButton; // Используем m_addGameButton вместо ui->addGameButton

    int m_gameCount; // Используем m_gameCount вместо gameCount
    const int MINIMUM_SIZE_WIDTH = 400;
    const int MINIMUM_SIZE_HEIGHT = 300;
    const int MAXIMUM_SIZE_WIDTH = 1000;
    const int MAXIMUM_SIZE_HEIGHT = 500;
    const int GAME_PREINSTALL_COUNT = 2;

};

#endif // GAMEMANAGER_H
