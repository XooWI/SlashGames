#include "databasemanagement.h"

DatabaseManagement::DatabaseManagement(QSettings* settings) :
    settings(settings)
{
    initializeDatabase();
}

DatabaseManagement::~DatabaseManagement()
{
    if (db.isOpen()) db.close();
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
        qDebug() << "Не удалось открыть базу данных" ;
        return;
    }

    QSqlQuery query(db);
    bool success = query.exec("CREATE TABLE IF NOT EXISTS users ("
                                  "username BLOB NOT NULL, "         // в зашифрованном виде
                                  "login BLOB UNIQUE NOT NULL, "     // в хешированном виде
                                  "password_hash BLOB NOT NULL, "    // в хешированном виде
                                  "balance BLOB DEFAULT '', "        // в зашифрованном виде
                                  "registration_date DATETIME, "     // в зашифрованном виде
                                  "auth_token_hash BLOB UNIQUE)");   // хэш токена авторизации

    if (!success) {
        qDebug() << "Не удалось создать таблицу" << query.lastError().text();
    }
}

bool DatabaseManagement::checkUser(const QString &login, const QByteArray &hashedPassword)
{
    if (!db.isOpen()) return false;
    QSqlQuery query(db);
    query.prepare("SELECT password_hash FROM users WHERE login = :login");
    query.bindValue(":login", hash(login));
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
            authTokenHash.addData(getUniqueID().toUtf8()); // Уникальный номер устройства
            authTokenHash.addData(hash(login.toUtf8()));
            QByteArray rawAuthToken = authTokenHash.result();
            QByteArray authTokenHashForDb = hash(rawAuthToken); // Токен для БД

            // Формируем строку с данными токена (дата_истечения_токена|дата_последнего_захода|уникальный_токен)
            QString localTokenString = tokenExpiryTime.toString(Qt::ISODate) + "|" +
                    lastLogin.toString(Qt::ISODate) + "|" +
                    QString(rawAuthToken.toHex());
            QByteArray encryptedLocalTokenData = localEncrypt(localTokenString.toUtf8());

            settings->setValue("ID", encryptedLocalTokenData.toHex());

            // Обновляем хэш токена авторизации в БД
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

// Шифрование для локального токена
QByteArray DatabaseManagement::localEncrypt(const QByteArray &data)
{
    QByteArray encrypted;
    for (int i = 0; i < data.size(); ++i) {
        encrypted.append(data[i] ^ LOCAL_ENCRYPTION_KEY[i % LOCAL_ENCRYPTION_KEY.size()]);
    }
    return encrypted;
}

// Для шифрования в БД
QByteArray DatabaseManagement::dbEncrypt(const QByteArray &data)
{
    QByteArray encrypted;
    for (int i = 0; i < data.size(); ++i) {
        encrypted.append(data[i] ^ DB_ENCRYPTION_KEY[i % DB_ENCRYPTION_KEY.size()]);
    }
    return encrypted;
}

// Хэширование
QByteArray DatabaseManagement::hash(const QString &data)
{
    return QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Sha256);
}

bool DatabaseManagement::checkLogin(const QString &login)
{
    if (!db.isOpen()) return false;
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE login = :login");
    query.bindValue(":login", hash(login));
    if (!query.exec()) {
        qDebug() << "checkLogin Query failed:" << query.lastError().text();
        return false;
    }
    return (query.next() && query.value(0).toInt() > 0);
}


bool DatabaseManagement::checkToken()
{
    if (settings->value("ID") == "") return false;
    if (!db.isOpen()) return false;

    QByteArray encryptedLocalTokenData = QByteArray::fromHex(settings->value("ID").toByteArray());

    QByteArray decryptedLocalTokenData = localEncrypt(encryptedLocalTokenData);
    QString localTokenString = QString::fromUtf8(decryptedLocalTokenData);
    QStringList tokenParts = localTokenString.split("|");

    if (tokenParts.size() != 3) {
        settings->setValue("ID", "");
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
                return true;
            } else {
                settings->setValue("ID", "");
                settings->setValue("balance", 0);
                return false;
            }
        } else {
            settings->setValue("ID", "");
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
                settings->setValue("ID", QByteArray().toHex());
                settings->setValue("balance", 0);
                return QString();
            }
            return rawAuthTokenHex;
        } else {
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
        return decryptedBalance.toDouble();
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
        return "None";
    }

    if (query.next()) {
        QByteArray encryptedUsername = query.value("username").toByteArray();
        return QString(dbEncrypt(encryptedUsername));
    }
    return "None";
}

QString DatabaseManagement::getLogin()
{
    if (!db.isOpen()) return "None";
    QString authTokenHex = getToken();
    if (authTokenHex.isEmpty()) return "None";
    QByteArray authTokenToCheck = QByteArray::fromHex(authTokenHex.toUtf8());

    QSqlQuery query(db);
    query.prepare("SELECT login FROM users WHERE auth_token_hash = :auth_token_hash");
    query.bindValue(":auth_token_hash", hash(authTokenToCheck));
    if (!query.exec()) {
        return "None";
    }

    if (query.next()) {
        QByteArray encryptedLogin = query.value("login").toByteArray();
        QByteArray decryptedLogin = localEncrypt(encryptedLogin);
        return QString::fromUtf8(decryptedLogin);
    }
    return "None";
}

QDateTime DatabaseManagement::getTokenExpiryTime()
{
    QByteArray encryptedLocalTokenDataHex = settings->value("ID").toByteArray();
    QByteArray encryptedLocalTokenData = QByteArray::fromHex(encryptedLocalTokenDataHex);

    if (encryptedLocalTokenData.isEmpty()) {
        return QDateTime();
    }

    QByteArray decryptedLocalTokenData = localEncrypt(encryptedLocalTokenData);
    QString localTokenString = QString::fromUtf8(decryptedLocalTokenData);
    QStringList tokenParts = localTokenString.split("|");

    if (tokenParts.size() == 3) {
        return QDateTime::fromString(tokenParts[0], Qt::ISODate);
    } else {
        return QDateTime();
    }
}


QDateTime DatabaseManagement::getRegistredTime()
{
    if (!db.isOpen()) return QDateTime();
    QString authTokenHex = getToken();
    if (authTokenHex.isEmpty()) return QDateTime();
    QByteArray authTokenToCheck = QByteArray::fromHex(authTokenHex.toUtf8());

    QSqlQuery query(db);
    query.prepare("SELECT registration_date FROM users WHERE auth_token_hash = :auth_token_hash");
    query.bindValue(":auth_token_hash", hash(authTokenToCheck));
    if (!query.exec()) {
        return QDateTime();
    }

    if (query.next()) {
        QByteArray encryptedRegDate = query.value("registration_date").toByteArray();
        QByteArray decryptedRegDate = dbEncrypt(encryptedRegDate);

        QString regDateString = QString::fromUtf8(decryptedRegDate);
        return QDateTime::fromString(regDateString, Qt::ISODate);
    }
    return QDateTime();
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

// Проверка сложности пароля
int DatabaseManagement::password_strength(QString &password)
{
    // Длина пароля
    if (password.length() < 10) return 1;

    // Отсутствие пробелов
    if (password.contains(' ')) return 2;

    // Содержит заглавные буквы
    if (password == password.toLower()) return 3;

    // Содержит строчные буквы
    if (password == password.toUpper()) return 4;

    // Содержит цифры
    if (password.count(QRegularExpression("[0-9]")) == 0) return 5;

    // Содержит русские и английские буквы
    if ( ((password.count(QRegularExpression("[a-zA-Z]"))>0) + (password.count(QRegularExpression("[а-яА-Я]"))>0)) <2) return 6;

    // Содержит спецсимволы
    if (password.count(QRegularExpression("[^a-zA-Zа-яА-Я0-9]")) == 0) return 7;

    return 0;
}

