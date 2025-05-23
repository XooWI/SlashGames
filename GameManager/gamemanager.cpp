#include "gamemanager.h"

GameManager::GameManager(QSettings *settings, QGridLayout *gamesLayout, QPushButton *addGameButton,  QObject *parent)
    : QObject(parent),
      m_settings(settings),
      m_gamesLayout(gamesLayout),
      m_addGameButton(addGameButton)
{
    // Инициализируем gameCount из настроек
    m_gameCount = m_settings->value("GameCount", GAME_PREINSTALL_COUNT).toInt();
}

void GameManager::saveGame(const QString &name, const QString &iconPath, const QString &executablePath) // Добавили GameManager::
{
    // Сохраняем количество игр
    // Можно убрать!!!
    m_gameCount = m_settings->value("GameCount", GAME_PREINSTALL_COUNT).toInt(); // Используем m_settings и m_gameCount
    m_gameCount++;
    m_settings->setValue("GameCount", m_gameCount);

    // Сохраняем данные конкретной игры
    m_settings->beginGroup(QString("Game_%1").arg(m_gameCount));
    m_settings->setValue("Name", name);
    m_settings->setValue("IconPath", iconPath);
    m_settings->setValue("ExecutablePath", executablePath);
    m_settings->endGroup();
}

void GameManager::refreshGamesLayout() // Добавили GameManager::
{
    m_gamesLayout->removeWidget(m_addGameButton);

    // Очищаем все кнопки
    for (int i = m_gamesLayout->count() - 1; i >= 0; --i) {
        QLayoutItem* item = m_gamesLayout->itemAt(i);
        if (item && item->widget()) {
            if (QPushButton* button = qobject_cast<QPushButton*>(item->widget())) {
                bool isUserGame = button->property("gameIndex").toBool();
                if (isUserGame) {
                    m_gamesLayout->removeWidget(button);
                    button->deleteLater();
                }
            }
        }
    }

    // Загружаем и добавляем игры из настроек
    m_gameCount = m_settings->value("GameCount", GAME_PREINSTALL_COUNT).toInt();

    for (int i = GAME_PREINSTALL_COUNT + 1; i <= m_gameCount; ++i) {
        m_settings->beginGroup(QString("Game_%1").arg(i));

        QString name = m_settings->value("Name").toString();
        QString iconPath = m_settings->value("IconPath").toString();
        QString executablePath = m_settings->value("ExecutablePath").toString();

        m_settings->endGroup();

        QPushButton* gameButton = createGameButton(name, iconPath, executablePath);

        gameButton->setProperty("gameIndex", i);

        // Индекс в сетке:
        int gridIndex = GAME_PREINSTALL_COUNT + (i - (GAME_PREINSTALL_COUNT + 1));
        int row = gridIndex / 2;
        int col = gridIndex % 2;

        m_gamesLayout->addWidget(gameButton, row, col);
    }

    // Обновляем позицию кнопки "+"
    m_gamesLayout->addWidget(m_addGameButton, m_gameCount / 2, m_gameCount % 2);
}

QPushButton* GameManager::createGameButton(const QString &name, const QString &iconPath, const QString &executablePath) // Добавили GameManager::
{
    QPushButton *newGameButton = new QPushButton(m_gamesLayout->parentWidget());
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
    QMenu menu(m_gamesLayout->parentWidget());

    QString gameName = gameButton->property("gameName").toString();
    QString iconPath = gameButton->property("iconPath").toString();
    QString executablePath = gameButton->property("executablePath").toString();
    int gameIndex = gameButton->property("gameIndex").toInt(); // Make sure this property is set when the button is created!

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

    // Извлекаем старые свойства игры из карты (до редактирования)
    QString currentName = gameData.value("Name").toString();
    QString currentIconPath = gameData.value("IconPath").toString();
    QString currentExecutablePath = gameData.value("ExecutablePath").toString();


    GameInputDialog editDialog(true, m_gamesLayout->parentWidget());
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

    for (int i = 1; i <= m_gameCount; ++i) {
        m_settings->beginGroup(QString("Game_%1").arg(i));
        QString entryName = m_settings->value("Name").toString();
        QString entryIconPath = m_settings->value("IconPath").toString();
        QString entryExecutablePath = m_settings->value("ExecutablePath").toString();
        if (entryName == currentName &&
                entryIconPath == currentIconPath &&
                entryExecutablePath == currentExecutablePath) {
            m_settings->setValue("Name", newName);
            m_settings->setValue("IconPath", newIconPath);
            m_settings->setValue("ExecutablePath", newExecutablePath);
            m_settings->endGroup();
            break;
        }
        m_settings->endGroup();
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

    CustomWindow deleteDialog(CustomWindow::DeleteConfirmation, gameName, QString(), m_gamesLayout->parentWidget());
    if (deleteDialog.exec() != QDialog::Accepted) {
        return;
    }

    QVector<QVariantMap> userGamesToKeep;
    int currentTotalGameCount = m_settings->value("GameCount", GAME_PREINSTALL_COUNT).toInt();

    for (int i = GAME_PREINSTALL_COUNT + 1; i <= currentTotalGameCount; ++i) {
        if (i == gameIndexToDelete) {
            continue;
        }

        m_settings->beginGroup(QString("Game_%1").arg(i));
        QString currentName = m_settings->value("Name").toString();
        if (!currentName.isEmpty()) {
            QVariantMap game;
            game["Name"] = currentName;
            game["IconPath"] = m_settings->value("IconPath").toString(); // Ensure toString()
            game["ExecutablePath"] = m_settings->value("ExecutablePath").toString(); // Ensure toString()
            userGamesToKeep.append(game);
        }
        m_settings->endGroup();
    }

    for (int i = GAME_PREINSTALL_COUNT + 1; i <= currentTotalGameCount; ++i) {
        m_settings->remove(QString("Game_%1").arg(i));
    }

    for (int i = 0; i < userGamesToKeep.size(); ++i) {
        int newGameIndex = GAME_PREINSTALL_COUNT + i + 1;
        m_settings->beginGroup(QString("Game_%1").arg(newGameIndex));
        m_settings->setValue("Name", userGamesToKeep[i]["Name"]);
        m_settings->setValue("IconPath", userGamesToKeep[i]["IconPath"]);
        m_settings->setValue("ExecutablePath", userGamesToKeep[i]["ExecutablePath"]);
        m_settings->endGroup();
    }

    m_gameCount = GAME_PREINSTALL_COUNT + userGamesToKeep.size();
    m_settings->setValue("GameCount", m_gameCount);

    refreshGamesLayout();
}

