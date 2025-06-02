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
                                  "auth_token_hash BLOB UNIQUE,"   // хэш токена авторизации
                                  "last_bonus_time DATETIME DEFAULT NULL)"); // в зашифрованном виде

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
    if (!db.isOpen()) {
        qDebug() << "registredUser: Database is not open.";
        return false;
    }
    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, login, password_hash, balance, registration_date, auth_token_hash, last_bonus_time) "
                    "VALUES (:username, :login, :password_hash, :balance, :registration_date, :auth_token_hash, :last_bonus_time)"); // <-- Изменено здесь

    query.bindValue(":username", dbEncrypt(username.toUtf8(), hash(login)));
    query.bindValue(":login", hash(login.toUtf8())); // Логин хешируется при регистрации
    query.bindValue(":password_hash", hashedPassword);
    query.bindValue(":balance", dbEncrypt(QByteArray::number(0.00), hash(login)));
    query.bindValue(":registration_date", dbEncrypt(QDateTime::currentDateTime().toString(Qt::ISODate).toUtf8(), hash(login)));
    query.bindValue(":auth_token_hash", QByteArray());
    query.bindValue(":last_bonus_time", dbEncrypt(QByteArray(), hash(login)));

    if (!query.exec()) {
        qDebug() << "registredUser: Failed to register user:" << query.lastError().text();
        return false;
    }
    qDebug() << "registredUser: User registered successfully.";
    return true;
}

// Шифрование для локального токена
QByteArray DatabaseManagement::localEncrypt(const QByteArray &data)
{
    // Ключ для шифрования локальных данных
    QByteArray encrypted, LOCAL_ENCRYPTION_KEY = hash("Local" + getUniqueID());
    for (int i = 0; i < data.size(); ++i) {
        encrypted.append(data[i] ^ LOCAL_ENCRYPTION_KEY[i % LOCAL_ENCRYPTION_KEY.size()]);
    }
    return encrypted;
}

// Для шифрования в БД
QByteArray DatabaseManagement::dbEncrypt(const QByteArray &data, const QByteArray &login)
{
    // Ключ для шифрования данных в БД (имя, баланс, даты)
    QByteArray encrypted, DB_ENCRYPTION_KEY = hash("Database" + login);
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

bool DatabaseManagement::checkPassword(const QString &password)
{
    if (!db.isOpen()) {
        qDebug() << "checkPassword: Database is not open.";
        return false;
    }

    QString authTokenHex = getToken();
    if (authTokenHex.isEmpty()) {
        qDebug() << "checkPassword: Authentication token is empty. Cannot verify password.";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT password_hash FROM users WHERE auth_token_hash = :auth_token_hash");
    query.bindValue(":auth_token_hash", hash(QByteArray::fromHex(authTokenHex.toUtf8())));

    if (!query.exec()) {
        qDebug() << "checkPassword Query failed:" << query.lastError().text();
        return false;
    }
    if (query.next()) {
        QByteArray storedPasswordHash = query.value("password_hash").toByteArray();
        bool passwordMatches = (hash(password) == storedPasswordHash);
        qDebug() << "CheckPassword: Password matches =" << passwordMatches;
        return passwordMatches;
    } else {
        qDebug() << "checkPassword: No user found for the current authentication token.";
        return false;
    }
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
        QByteArray decryptedBalance = dbEncrypt(encryptedBalance, getLogin());
        return decryptedBalance.toDouble();
    }
    return 0;
}

void DatabaseManagement::updateBalanceLocal(int amount)
{
    settings->setValue("balance", getBalance()- amount);
    settings->value("balance", 0).toInt();
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
        return QString(dbEncrypt(encryptedUsername, getLogin()));
    }
    return "None";
}

QByteArray DatabaseManagement::getLogin()
{
    if (!db.isOpen()) return "None";
    QString authTokenHex = getToken();
    if (authTokenHex.isEmpty()) return "None";
    QByteArray authTokenToCheck = QByteArray::fromHex(authTokenHex.toUtf8());

    QSqlQuery query(db);
    query.prepare("SELECT login FROM users WHERE auth_token_hash = :auth_token_hash");
    query.bindValue(":auth_token_hash", hash(authTokenToCheck));
    if (!query.exec()) {
        return QByteArray();
    }

    if (query.next()) {
        return query.value("login").toByteArray();
    }
    return QByteArray();
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
        QByteArray decryptedRegDate = dbEncrypt(encryptedRegDate, getLogin());

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
    QByteArray encryptedBalance = dbEncrypt(QByteArray::number(balance), getLogin());

    QSqlQuery updateQuery(db);
    updateQuery.prepare("UPDATE users SET balance = :balance WHERE auth_token_hash = :auth_token_hash");
    updateQuery.bindValue(":balance", encryptedBalance);
    updateQuery.bindValue(":auth_token_hash", hash(authTokenToCheck));

    return updateQuery.exec() && updateQuery.numRowsAffected() > 0;
}

bool DatabaseManagement::updateUsername(const QString &username)
{
    if (!db.isOpen()) {
        qDebug() << "updateUsername: Database is not open.";
        return false;
    }

    QString authTokenHex = getToken();
    if (authTokenHex.isEmpty()) {
        qDebug() << "updateUsername: Authentication token is empty. Cannot update username.";
        return false;
    }

    QSqlQuery updateQuery(db);
    updateQuery.prepare("UPDATE users SET username = :username WHERE auth_token_hash = :auth_token_hash");

    updateQuery.bindValue(":username", dbEncrypt(username.toUtf8(), getLogin()));

    updateQuery.bindValue(":auth_token_hash", hash(QByteArray::fromHex(authTokenHex.toUtf8())));

    if (!updateQuery.exec()) {
        qDebug() << "updateUsername: Query failed:" << updateQuery.lastError().text();
        return false;
    }

    bool success = updateQuery.numRowsAffected() > 0;
    qDebug() << "updateUsername: Update successful =" << success;
    return success;
}

bool DatabaseManagement::updatePassword(const QString &password)
{
    if (!db.isOpen()) {
        qDebug() << "updatePassword: Database is not open.";
        return false;
    }

    QString authTokenHex = getToken();
    if (authTokenHex.isEmpty()) {
        qDebug() << "updatePassword: Authentication token is empty. Cannot update password.";
        return false;
    }

    QSqlQuery updateQuery(db);
    updateQuery.prepare("UPDATE users SET password_hash = :password_hash WHERE auth_token_hash = :auth_token_hash");

    updateQuery.bindValue(":password_hash", hash(password));
    updateQuery.bindValue(":auth_token_hash", hash(QByteArray::fromHex(authTokenHex.toUtf8())));

    if (!updateQuery.exec()) {
        qDebug() << "updatePassword: Query failed:" << updateQuery.lastError().text();
        return false;
    }
    bool success = updateQuery.numRowsAffected() > 0;
    qDebug() << "updatePassword: Update successful =" << success;
    return success;
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


// Функция для сохранения времени последнего бонуса в БД
bool DatabaseManagement::saveLastBonusTime(const QDateTime &dateTime)
{
    if (!db.isOpen()) {
        return false;
    }

    QString authTokenHex = getToken();
    if (authTokenHex.isEmpty()) {
        return false;
    }

    QSqlQuery updateQuery(db);
    updateQuery.prepare("UPDATE users SET last_bonus_time = :last_bonus_time WHERE auth_token_hash = :auth_token_hash");
    updateQuery.bindValue(":last_bonus_time", dbEncrypt(dateTime.toString(Qt::ISODate).toUtf8(), getLogin()));
    updateQuery.bindValue(":auth_token_hash", hash(QByteArray::fromHex(authTokenHex.toUtf8())));

    if (!updateQuery.exec()) {
        return false;
    }

    return updateQuery.numRowsAffected() > 0;
}


// Функция для получения времени последнего бонуса из БД
QDateTime DatabaseManagement::getLastBonusTime()
{
    if (!db.isOpen()) {
        return QDateTime();
    }

    QString authTokenHex = getToken();
    if (authTokenHex.isEmpty()) {
        return QDateTime();
    }

    QSqlQuery query(db);
    query.prepare("SELECT last_bonus_time FROM users WHERE auth_token_hash = :auth_token_hash");
    query.bindValue(":auth_token_hash", hash(QByteArray::fromHex(authTokenHex.toUtf8())));

    if (!query.exec()) {
        return QDateTime();
    }

    if (query.next()) {
        QByteArray encryptedBonusTime = query.value("last_bonus_time").toByteArray();
        QByteArray decryptedBonusTime = dbEncrypt(encryptedBonusTime, getLogin());

        QString bonusTimeString = QString::fromUtf8(decryptedBonusTime);

        return QDateTime::fromString(bonusTimeString, Qt::ISODate);
    }

    return QDateTime();
}
