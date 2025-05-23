#include "databasemanagement.h"

DatabaseManagement::DatabaseManagement(QSettings* settings) :
    settings(settings),
    connectionName("local_sqlite_connection")
{
    initializeDatabase();
}

// Инициализации базы данных
void DatabaseManagement::initializeDatabase()
{
    if (QSqlDatabase::contains(connectionName)) {
        db = QSqlDatabase::database(connectionName);
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName("users.db");
    }


    if (!db.open()) {
        QMessageBox::critical(nullptr, "Ошибка базы данных", "Не удалось открыть базу данных");
        return;
    }

    QSqlQuery query(db);
    bool success = query.exec("CREATE TABLE IF NOT EXISTS users ("
                                  "username BLOB NOT NULL, " // в зашифрованном виде
                                  "login BLOB UNIQUE NOT NULL, " // в хешированном виде
                                  "password_hash BLOB NOT NULL, " // в хешированном виде
                                  "balance BLOB DEFAULT '', " // в зашифрованном виде
                                  "registration_date DATETIME, " // в зашифрованном виде
                                  "auth_token_hash BLOB UNIQUE)"); // Хэш токена авторизации

    if (!success) {
        qDebug() << "Не удалось создать таблицу" << query.lastError().text();
    }
}

bool DatabaseManagement::checkUser(const QString &login, const QByteArray &hashedPassword)
{
    if (!db.isOpen()) return false;
    QSqlQuery query(db);
    query.prepare("SELECT password_hash FROM users WHERE login = :login");
    query.bindValue(":login", hash(login.toUtf8())); // Логин хешируется при запросе
    if (!query.exec()) {
        qDebug() << "checkUser Ошибка: " << query.lastError().text();
        return false;
    }

    if (query.next()) {
        QByteArray storedPasswordHash = query.value("password_hash").toByteArray();

        if (hashedPassword == storedPasswordHash) {

            // Генерация данных локального токена
            QDateTime tokenExpiryTime = QDateTime::currentDateTime().addSecs(TOKEN_LIFETIME);
            QDateTime lastLogin = QDateTime::currentDateTime();
            QCryptographicHash authTokenHash(QCryptographicHash::Sha256);
            authTokenHash.addData(getUniqueID().toUtf8());
            authTokenHash.addData(hash(login.toUtf8()));
            QByteArray rawAuthToken = authTokenHash.result();
            QByteArray authTokenHashForDb = hash(rawAuthToken); // Токен для БД

            // Формируем строку с данными токена (дата_истечения_токена|дата_последнего_захода|уникальный_токен)
            QString localTokenString = tokenExpiryTime.toString(Qt::ISODate) + "|" +
                    lastLogin.toString(Qt::ISODate) + "|" +
                    QString(rawAuthToken.toHex());
            QByteArray localTokenData = localTokenString.toUtf8();
            QByteArray encryptedLocalTokenData = localEncrypt(localTokenData);

            settings->setValue("ID", encryptedLocalTokenData.toHex());

            // Сохраняем хэш токена авторизации в БД
            QSqlQuery updateQuery(db);
            updateQuery.prepare("UPDATE users SET auth_token_hash = :auth_token_hash WHERE login = :login");
            updateQuery.bindValue(":auth_token_hash", authTokenHashForDb);
            updateQuery.bindValue(":login", hash(login.toUtf8()));

            if (!updateQuery.exec()) {
                qDebug() << "Ошибка обновления хэша токена: " << updateQuery.lastError().text();
                return false;
            }

            return true;
        } else {
            qDebug() << "Неверный пароль Логин:" << login;
            return false;
        }
    } else {
        qDebug() << "Логин не найден: " << login;
        return false;
    }
}


bool DatabaseManagement::registredUser(const QString &username, const QString &login, const QByteArray &hashedPassword)
{
    if (!db.isOpen()) return false;
    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, login, password_hash, balance, registration_date, auth_token_hash) "
                    "VALUES (:username, :login, :password_hash, :balance, :registration_date, :auth_token_hash)");
    query.bindValue(":username", dbEncrypt(username.toUtf8()));
    query.bindValue(":login", hash(login.toUtf8())); // Логин хешируется при регистрации
    query.bindValue(":password_hash", hashedPassword);
    query.bindValue(":balance", dbEncrypt(QByteArray::number(0.00)));
    query.bindValue(":registration_date", dbEncrypt(QDateTime::currentDateTime().toString(Qt::ISODate).toUtf8()));
    query.bindValue(":auth_token_hash", QByteArray());

    return query.exec();
}

QByteArray DatabaseManagement::localEncrypt(const QByteArray &data)
{
    QByteArray encrypted;
    for (int i = 0; i < data.size(); ++i) {
        encrypted.append(data[i] ^ LOCAL_ENCRYPTION_KEY[i % LOCAL_ENCRYPTION_KEY.size()]);
    }
    return encrypted;
}

QByteArray DatabaseManagement::dbEncrypt(const QByteArray &data)
{
    QByteArray encrypted;
    for (int i = 0; i < data.size(); ++i) {
        encrypted.append(data[i] ^ DB_ENCRYPTION_KEY[i % DB_ENCRYPTION_KEY.size()]);
    }
    return encrypted;
}

QByteArray DatabaseManagement::hash(const QByteArray &data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256);
}

bool DatabaseManagement::checkLogin(const QString &login)
{
    if (!db.isOpen()) return false;
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE login = :login");
    query.bindValue(":login", hash(login.toUtf8()));
    if (!query.exec()) {
        qDebug() << "checkLogin Query failed:" << query.lastError().text();
        return false;
    }
    return (query.next() && query.value(0).toInt() > 0);
}


bool DatabaseManagement::checkToken()
{
    if (settings->value("ID") == ""){
        //qDebug() << "checkToken1 Локальные данные токена отсутствуют";
        return false;
   }
    if (!db.isOpen()) return false;

    QByteArray encryptedLocalTokenData = QByteArray::fromHex(settings->value("ID").toByteArray());

    QByteArray decryptedLocalTokenData = localEncrypt(encryptedLocalTokenData);
    QString localTokenString = QString::fromUtf8(decryptedLocalTokenData);
    QStringList tokenParts = localTokenString.split("|");

    if (tokenParts.size() != 3) {
        qDebug() << "checkToken Неверный формат локальных данных токена";
        settings->setValue("ID", ""); // Очищаем локальные данные
        settings->setValue("balance", 0);
        return false;
    }

    QDateTime tokenExpiryTime = QDateTime::fromString(tokenParts[0], Qt::ISODate);
    QDateTime lastLogin = QDateTime::fromString(tokenParts[1], Qt::ISODate);
    QByteArray rawAuthToken = QByteArray::fromHex(tokenParts[2].toUtf8());

    QDateTime currentDate = QDateTime::currentDateTime();

    if (currentDate <= tokenExpiryTime && lastLogin <= currentDate) {
            // Токен локально действителен, проверяем хэш в БД
            QSqlQuery query(db);
            query.prepare("SELECT COUNT(*) FROM users WHERE auth_token_hash = :auth_token_hash");
            query.bindValue(":auth_token_hash", hash(rawAuthToken));
            if (!query.exec()) {
                qDebug() << "checkToken Ошибка при запросе к БД:" << query.lastError().text();
                return false;
            }
            if (query.next() && query.value(0).toInt() > 0) {
                // Обновляем локальные данные токена (время последнего захода)
                QDateTime newLastLogin = QDateTime::currentDateTime();
                QString newLocalTokenString = tokenExpiryTime.toString(Qt::ISODate) + "|" +
                                               newLastLogin.toString(Qt::ISODate) + "|" +
                                               QString(rawAuthToken.toHex());
                QByteArray newEncryptedLocalTokenData = localEncrypt(newLocalTokenString.toUtf8());
                settings->setValue("ID", newEncryptedLocalTokenData.toHex());
                //qDebug() << "checkToken Токен истекает: " << tokenExpiryTime;
                return true;
            } else {
                qDebug() << "checkToken Хэш локального токена не найден в БД или не совпадает";
                settings->setValue("ID", ""); // Очищаем локальные данные
                settings->setValue("balance", 0);
                return false;
            }
        } else {
            qDebug() << "checkToken Срок действия локального токена истек. Очистка ID и баланса!";
            settings->setValue("ID", ""); // Очищаем локальные данные
            settings->setValue("balance", 0);
            return false;
        }
    return false;
}

QString DatabaseManagement::getToken()
{
    QByteArray encryptedLocalTokenDataHex = settings->value("ID").toByteArray();
    QByteArray encryptedLocalTokenData = QByteArray::fromHex(encryptedLocalTokenDataHex);

    if (!encryptedLocalTokenData.isEmpty()) {
        QByteArray decryptedLocalTokenData = localEncrypt(encryptedLocalTokenData);
        QString localTokenString = QString::fromUtf8(decryptedLocalTokenData);
        QStringList tokenParts = localTokenString.split("|");

        if (tokenParts.size() == 3) {
            // Время истечения
            QDateTime tokenExpiryTime = QDateTime::fromString(tokenParts[0], Qt::ISODate);
            // Время последнего входа
            QDateTime lastLogin = QDateTime::fromString(tokenParts[1], Qt::ISODate);
            QString rawAuthTokenHex = tokenParts[2];

            QDateTime currentDate = QDateTime::currentDateTime();

            if (currentDate > tokenExpiryTime || lastLogin > currentDate || !(tokenExpiryTime.isValid()) || !(lastLogin.isValid())) {
                qDebug() << "getToken Токен истек! Очистка ID и баланса!";
                settings->setValue("ID", QByteArray().toHex());
                settings->setValue("balance", 0);
                return QString();
            }
            return rawAuthTokenHex;
        } else {
            qDebug() << "getToken Неверный формат локальных данных токена при получении токена";
            settings->setValue("ID", QByteArray().toHex()); // Очищаем локальные данные
            return QString();
        }
    }
    return QString();
}

// Генерация уникального токена устройства
QString DatabaseManagement::getUniqueID()
{
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(QSysInfo::machineUniqueId());
    hash.addData(QSysInfo::kernelVersion().toUtf8());
    QString hashedId = hash.result().toHex();
    return hashedId;
}


int DatabaseManagement::getBalance()
{
    if (!db.isOpen()) return 0;
    QString authTokenHex = getToken();
    if (authTokenHex.isEmpty()) return 0;
    QByteArray authTokenToCheck = QByteArray::fromHex(authTokenHex.toUtf8());

    QSqlQuery query(db);
    query.prepare("SELECT balance FROM users WHERE auth_token_hash = :auth_token_hash");
    query.bindValue(":auth_token_hash", hash(authTokenToCheck));
    if (!query.exec()) {
        qDebug() << "getBalance Ошибка:" << query.lastError().text();
        return 0;
    }

    if (query.next()) {
        QByteArray encryptedBalance = query.value("balance").toByteArray();
        QByteArray decryptedBalance = dbEncrypt(encryptedBalance);
        bool ok;
        int balance = decryptedBalance.toDouble(&ok);
        return ok ? balance : 0;
    }
    return 0;
}

QString DatabaseManagement::getUsername()
{
    if (!db.isOpen()) return "None";
    QString authTokenHex = getToken();
    if (authTokenHex.isEmpty()) return "None";
    QByteArray authTokenToCheck = QByteArray::fromHex(authTokenHex.toUtf8());

    QSqlQuery query(db);
    query.prepare("SELECT username FROM users WHERE auth_token_hash = :auth_token_hash");
    query.bindValue(":auth_token_hash", hash(authTokenToCheck));
    if (!query.exec()) {
        qDebug() << "getUsername Ошибка:" << query.lastError().text();
        return "None";
    }

    if (query.next()) {
        QByteArray encryptedUsername = query.value("username").toByteArray();
        return QString(dbEncrypt(encryptedUsername));
    }
    return "None";
}

QDateTime DatabaseManagement::getTokenExpiryTime()
{
    QByteArray encryptedLocalTokenDataHex = settings->value("ID").toByteArray();
    QByteArray encryptedLocalTokenData = QByteArray::fromHex(encryptedLocalTokenDataHex);

    if (encryptedLocalTokenData.isEmpty()) {
        qDebug() << "getTokenExpiryTime: Локальные данные токена отсутствуют";
        return QDateTime(); // Возвращаем невалидное QDateTime
    }

    QByteArray decryptedLocalTokenData = localEncrypt(encryptedLocalTokenData);
    QString localTokenString = QString::fromUtf8(decryptedLocalTokenData);
    QStringList tokenParts = localTokenString.split("|");

    if (tokenParts.size() == 3) {
        return QDateTime::fromString(tokenParts[0], Qt::ISODate);
    } else {
        qDebug() << "getTokenExpiryTime: Неверный формат локальных данных токена";
        return QDateTime(); // Возвращаем невалидное QDateTime
    }
}


bool DatabaseManagement::updateBalance(int &balance)
{
    if (!db.isOpen()) return false;
    QString authTokenHex = getToken();
    if (authTokenHex.isEmpty()) return false;
    QByteArray authTokenToCheck = QByteArray::fromHex(authTokenHex.toUtf8());
    QByteArray encryptedBalance = dbEncrypt(QByteArray::number(balance));

    QSqlQuery updateQuery(db);
    updateQuery.prepare("UPDATE users SET balance = :balance WHERE auth_token_hash = :auth_token_hash");
    updateQuery.bindValue(":balance", encryptedBalance);
    updateQuery.bindValue(":auth_token_hash", hash(authTokenToCheck));

    return updateQuery.exec() && updateQuery.numRowsAffected() > 0;
}

DatabaseManagement::~DatabaseManagement()
{
    if (db.isOpen()) db.close();
}
