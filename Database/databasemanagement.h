#ifndef DATABASEMANAGEMENT_H
#define DATABASEMANAGEMENT_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QByteArray>
#include <QString>
#include <QSettings>
#include <QDateTime>
#include <QSysInfo>
#include <QCryptographicHash>
#include <QRegularExpression>

#include <QSqlError>
#include <QDebug>

class DatabaseManagement
{
public:
    DatabaseManagement(QSettings* settings = nullptr);
    ~DatabaseManagement();

    bool checkUser(const QString &login, const QByteArray &hashedPassword);
    bool checkLogin(const QString &login);
    bool checkPassword(const QString &password);
    bool checkToken();

    bool registredUser(const QString &username, const QString &login, const QByteArray &hashedPassword);

    int getBalance();
    QString getToken();
    QString getUsername();
    QByteArray getLogin();
    QDateTime getRegistredTime();
    QDateTime getTokenExpiryTime();

    bool updateBalance(int &balance);
    void updateBalanceLocal(int amount);
    bool updateUsername(const QString &username);
    bool updatePassword(const QString &password);
    int password_strength(QString &password);

    QByteArray hash(const QString &data);

    bool saveLastBonusTime(const QDateTime &dateTime);
    QDateTime getLastBonusTime();

private:
    QSettings *settings;
    QSqlDatabase db;
    const QString connectionName = "local_sqlite_connection";

    void initializeDatabase();
    QString getUniqueID();

    // Методы шифрования и хэширования
    QByteArray localEncrypt(const QByteArray &data);
    QByteArray dbEncrypt(const QByteArray &data, const QByteArray &login);

    // Время жизни токена
    const qint64 TOKEN_LIFETIME = 7 * 24 * 60 * 60; // 7 * 24 * 60 * 60 = 7 дней

};

#endif // DATABASEMANAGEMENT_H
