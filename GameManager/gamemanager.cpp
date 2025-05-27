#include "gamemanager.h"

GameManager::GameManager(QSettings *settings, QGridLayout *gamesLayout, QPushButton *addGameButton,  QObject *parent)
    : QObject(parent),
      settings(settings),
      gamesLayout(gamesLayout),
      addGameButton(addGameButton)
{
    // Инициализируем gameCount из настроек
    gameCount = settings->value("GameCount", GAME_PREINSTALL_COUNT).toInt();
}

void GameManager::saveGame(const QString &name, const QString &iconPath, const QString &executablePath)
{
    gameCount = settings->value("GameCount", GAME_PREINSTALL_COUNT).toInt(); // Используем settings и gameCount
    gameCount++;
    settings->setValue("GameCount", gameCount);

    // Сохраняем данные конкретной игры
    settings->beginGroup(QString("Game_%1").arg(gameCount));
    settings->setValue("Name", name);
    settings->setValue("IconPath", iconPath);
    settings->setValue("ExecutablePath", executablePath);
    settings->endGroup();
}

void GameManager::refreshGamesLayout()
{
    gamesLayout->removeWidget(addGameButton);

    // Очищаем все кнопки
    for (int i = gamesLayout->count() - 1; i >= 0; --i) {
        QLayoutItem* item = gamesLayout->itemAt(i);
        if (item && item->widget()) {
            if (QPushButton* button = qobject_cast<QPushButton*>(item->widget())) {
                bool isUserGame = button->property("gameIndex").toBool();
                if (isUserGame) {
                    gamesLayout->removeWidget(button);
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

        gameButton->setProperty("gameIndex", i);

        int gridIndex = GAME_PREINSTALL_COUNT + (i - (GAME_PREINSTALL_COUNT + 1));
        int row = gridIndex / 2;
        int col = gridIndex % 2;

        gamesLayout->addWidget(gameButton, row, col);
    }

    if (MAX_CUSTOM_GAME_COUNT>0 && MAX_CUSTOM_GAME_COUNT>(gameCount-GAME_PREINSTALL_COUNT)){
        // Обновляем позицию кнопки "+"
        gamesLayout->addWidget(addGameButton, gameCount / 2, gameCount % 2);
    }
}

QPushButton* GameManager::createGameButton(const QString &name, const QString &iconPath, const QString &executablePath) // Добавили GameManager::
{
    QPushButton *newGameButton = new QPushButton(gamesLayout->parentWidget());
    newGameButton->setMinimumSize(QSize(MINIMUM_SIZE_WIDTH, MINIMUM_SIZE_HEIGHT));
    newGameButton->setMaximumSize(QSize(MAXIMUM_SIZE_WIDTH, MAXIMUM_SIZE_HEIGHT));

    newGameButton->setStyleSheet(CustomStyle::getCustomNewButton().arg(iconPath));
    newGameButton->setText(name);
    newGameButton->setProperty("gameName", name);
    newGameButton->setProperty("executablePath", executablePath);
    newGameButton->setProperty("iconPath", iconPath);

    newGameButton->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(newGameButton, &QPushButton::customContextMenuRequested, this, [this, newGameButton](const QPoint& pos){
        QPoint globalPos = newGameButton->mapToGlobal(pos);
        gameButtonRight_clicked(newGameButton, globalPos);
    });

    connect(newGameButton, &QPushButton::clicked, [executablePath]() {
        QProcess::startDetached(executablePath);
    });

    return newGameButton;
}

void GameManager::gameButtonRight_clicked(QPushButton* gameButton, const QPoint& globalPos) {
    QMenu menu(gamesLayout->parentWidget());

    QString gameName = gameButton->property("gameName").toString();
    QString iconPath = gameButton->property("iconPath").toString();
    QString executablePath = gameButton->property("executablePath").toString();
    int gameIndex = gameButton->property("gameIndex").toInt();

    QVariantMap gameData;
    gameData["Name"] = gameName;
    gameData["IconPath"] = iconPath;
    gameData["ExecutablePath"] = executablePath;
    gameData["Index"] = gameIndex;

    QAction* editAction = menu.addAction("Редактировать");
    editAction->setData(gameData);
    connect(editAction, &QAction::triggered, this, &GameManager::menuGameEdit_clicked);

    QAction* deleteAction = menu.addAction("Удалить");
    deleteAction->setData(gameData);;
    connect(deleteAction, &QAction::triggered, this, &GameManager::menuGameDelete_clicked);

    menu.exec(globalPos);
}


void GameManager::menuGameEdit_clicked()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action) return;

    QVariant gameDataVariant = action->data();
    QVariantMap gameData = gameDataVariant.toMap();

    QString currentName = gameData.value("Name").toString();
    QString currentIconPath = gameData.value("IconPath").toString();
    QString currentExecutablePath = gameData.value("ExecutablePath").toString();


    GameInputDialog editDialog(true, gamesLayout->parentWidget());
    editDialog.setName(currentName);
    editDialog.setIconPath(currentIconPath);
    editDialog.setFilePath(currentExecutablePath);

    if (editDialog.exec() != QDialog::Accepted) {
        return;
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


void GameManager::menuGameDelete_clicked()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action) return;

    QVariantMap gameData = action->data().toMap();

    QString gameName = gameData.value("Name").toString();
    int gameIndexToDelete = gameData.value("Index").toInt();

    if (gameName.isEmpty() || gameIndexToDelete == 0) {
        return;
    }

    CustomWindow deleteDialog(CustomWindow::ConfirmationDeleteGame, gameName, QString(), gamesLayout->parentWidget());
    if (deleteDialog.exec() != QDialog::Accepted) {
        return;
    }

    QVector<QVariantMap> userGamesToKeep;
    int currentTotalGameCount = settings->value("GameCount", GAME_PREINSTALL_COUNT).toInt();

    for (int i = GAME_PREINSTALL_COUNT + 1; i <= currentTotalGameCount; ++i) {
        if (i == gameIndexToDelete) {
            continue;
        }

        settings->beginGroup(QString("Game_%1").arg(i));
        QString currentName = settings->value("Name").toString();
        if (!currentName.isEmpty()) {
            QVariantMap game;
            game["Name"] = currentName;
            game["IconPath"] = settings->value("IconPath").toString(); // Ensure toString()
            game["ExecutablePath"] = settings->value("ExecutablePath").toString(); // Ensure toString()
            userGamesToKeep.append(game);
        }
        settings->endGroup();
    }

    for (int i = GAME_PREINSTALL_COUNT + 1; i <= currentTotalGameCount; ++i) {
        settings->remove(QString("Game_%1").arg(i));
    }

    for (int i = 0; i < userGamesToKeep.size(); ++i) {
        int newGameIndex = GAME_PREINSTALL_COUNT + i + 1;
        settings->beginGroup(QString("Game_%1").arg(newGameIndex));
        settings->setValue("Name", userGamesToKeep[i]["Name"]);
        settings->setValue("IconPath", userGamesToKeep[i]["IconPath"]);
        settings->setValue("ExecutablePath", userGamesToKeep[i]["ExecutablePath"]);
        settings->endGroup();
    }

    gameCount = GAME_PREINSTALL_COUNT + userGamesToKeep.size();
    settings->setValue("GameCount", gameCount);

    refreshGamesLayout();
}

