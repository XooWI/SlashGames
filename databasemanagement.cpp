#include "databasemanagement.h"

DatabaseManagement::DatabaseManagement(QSettings* settings) :
    settings(settings),
    connectionName("local_sqlite_connection")
{
    qDebug() << "Запуск БД";
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
    } else {
        qDebug() << "База данных успешно открыта!";
    }

    QSqlQuery query(db);
    bool success = query.exec("CREATE TABLE IF NOT EXISTS users ("
                              "username TEXT NOT NULL, " // Имя пользователя
                              "login TEXT UNIQUE NOT NULL, " // Логин
                              "password_hash BLOB NOT NULL, " // Хеш пароля
                              "balance REAL DEFAULT 0.00, " // Баланс пользователя
                              "registration_date DATETIME, " // Дата регистрации
                              "token TEXT UNIQUE)"); // Уникальный токен

    if (!success) {
        qDebug() << "Не удалось создать таблицу" << query.lastError().text();
    } else {
        qDebug() << "Таблица создана!";
    }
}


bool DatabaseManagement::checkLogin(const QString &login)
{
    if (!db.isOpen()) {
        qDebug() << "БД закрыта";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE login = :login");
    query.bindValue(":login", login);

    if (!query.exec()) {
        qDebug() << "checkLogin Query failed:" << query.lastError().text();
        return false; // Ошибка выполнения запроса
    }

    if (query.next() && query.value(0).toInt() > 0) {
        qDebug() << "Логин: " << login << " уже существует";
        return true;
    }

    qDebug() << "Логин: " << login << " не найден";
    return false; // Логин не найден
}


bool DatabaseManagement::registredUser(const QString &username, const QString &login, const QByteArray &hashedPassword)
{
    if (!db.isOpen()) {
        qDebug() << "БД закрыта";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, login, password_hash, balance, registration_date, token) "
                  "VALUES (:username, :login, :password_hash, :balance, :registration_date, :token)");

    query.bindValue(":username", username);
    query.bindValue(":login", login);
    query.bindValue(":password_hash", hashedPassword);
    query.bindValue(":balance", 0.00);
    query.bindValue(":registration_date", QDateTime::currentDateTime());
    query.bindValue(":token", "0");


    if (query.exec()) {
        qDebug() << "Успешно зарегистрирован. Логин: " << login;
        return true;
    } else {
        qDebug() << "не удалось зарегистрировать. Логин: " << login << " Ошибка:" << query.lastError().text();
        return false;
    }
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


bool DatabaseManagement::checkUser(const QString &login, const QByteArray &hashedPassword)
{
    if (!db.isOpen()) {
        qDebug() << "БД закрыта";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT password_hash FROM users WHERE login = :login");
    query.bindValue(":login", login);

    if (!query.exec()) {
        qDebug() << "checkUser Ошибка: " << query.lastError().text();
        return false;
    }

    if (query.next()) {
        // Пользователь с таким логином найден
        QByteArray storedPasswordHash = query.value("password_hash").toByteArray();

        // Сравниваем хеш введенного пароля с хешем из базы данных
        if (hashedPassword == storedPasswordHash) {
            qDebug() << "Успешный вход: " << login;

            // Генерация локального ID
            // Вынести в отдельную функцию?!
            QCryptographicHash local_token_hash(QCryptographicHash::Sha512);
            local_token_hash.addData(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toUtf8());
            local_token_hash.addData(QSysInfo::productType().toUtf8());
            QString local_id = local_token_hash.result().toHex();
            settings->setValue("ID", local_id);
            qDebug() << "Local_id" << local_id;

            // Генерация токена для БД
            // Вынести в отдельную функцию?!
            QCryptographicHash token_hash(QCryptographicHash::Sha256);
            token_hash.addData(local_id.toUtf8());
            token_hash.addData(getUniqueID().toUtf8());
            QString token = token_hash.result().toHex();
            qDebug() << "Token" << token;

            // Обновление токена в базе данных
             QSqlQuery updateQuery(db);
             updateQuery.prepare("UPDATE users SET token = :token WHERE login = :login");
             updateQuery.bindValue(":token", token);
             updateQuery.bindValue(":login", login);

             if (!updateQuery.exec()) {
                    qDebug() << "checkUser Ошибка: " << updateQuery.lastError().text();
                    return false;
             } else{
                    return true;
             }
        } else {
            // Неверный пароль
            qDebug() << "Неверный пароль Логин:" << login;
            return false;
        }
    } else {
        // Логин не найден
        qDebug() << "Логин не найден: " << login;
        return false;
    }
}

bool DatabaseManagement::checkToken(){
    if (!db.isOpen()) {
        qDebug() << "БД закрыта";
        return false;
    }

    // Нет локального ID
    if (settings->value("ID", "0").toString() == "0") {
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT login FROM users WHERE token = :token");
    query.bindValue(":token", getToken());
    if (!query.exec()) {
        qDebug() << "checkToken Ошибка:" << query.lastError().text();
        return false;
    }

    if (query.next() && query.value(0).toInt() > 0) {
        return true;
    }
    return false;
}


QString DatabaseManagement::getToken(){
    QString local_ID = settings->value("ID", "0").toString();
    if (local_ID == "0"){
        return "0";
    }

    QCryptographicHash token_hash(QCryptographicHash::Sha256);
    token_hash.addData(local_ID.toUtf8());
    token_hash.addData(getUniqueID().toUtf8());
    QString token = token_hash.result().toHex();

    return token;
}


int DatabaseManagement::getBalance(){
    if (!db.isOpen()) {
        qDebug() << "БД закрыта";
        return 0;
    }
    if (getToken() == "0") {
        return 0;
    }

    QSqlQuery query(db);
    query.prepare("SELECT balance FROM users WHERE token = :token");
    query.bindValue(":token", getToken());
    if (!query.exec()) {
        qDebug() << "getBalance Ошибка:" << query.lastError().text();
        return 0;
    }

    if (query.next()) {
        return query.value("balance").toInt();
    }
    return 0;
}

QString DatabaseManagement::getUsername(){
    if (!db.isOpen()) {
        qDebug() << "БД закрыта";
        return "None";
    }
    if (getToken() == "0") {
        return "None";
    }

    QSqlQuery query(db);
    query.prepare("SELECT username FROM users WHERE token = :token");
    query.bindValue(":token", getToken());
    if (!query.exec()) {
        qDebug() << "getUsername Ошибка:" << query.lastError().text();
        return "None";
    }

    if (query.next()) {
        return query.value("username").toString();
    }
    return "None";
}



DatabaseManagement::~DatabaseManagement()
{
    // Закрываем соединение с базой данных
    if (db.isOpen()) {
        qDebug() << "Закрытие соединения с БД: " << connectionName;
        db.close();
    }

}
