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
    explicit DatabaseManagement(QSettings* settings = nullptr);
    ~DatabaseManagement();

    bool checkUser(const QString &login, const QByteArray &hashedPassword);
    bool checkLogin(const QString &login);
    bool checkToken();

    bool registredUser(const QString &username, const QString &login, const QByteArray &hashedPassword);

    int getBalance();
    QString getToken();
    QString getUsername();
    QString getLogin();
    QDateTime getRegistredTime();
    QDateTime getTokenExpiryTime();

    bool updateBalance(int &balance);
    int password_strength(QString &password);

    QByteArray hash(const QString &data);

private:
    QSettings *settings;
    QSqlDatabase db;
    const QString connectionName = "local_sqlite_connection";

    void initializeDatabase();
    QString getUniqueID();

    // Методы шифрования и хэширования
    QByteArray localEncrypt(const QByteArray &data);
    QByteArray dbEncrypt(const QByteArray &data);

    // Время жизни токена
    const qint64 TOKEN_LIFETIME = 7 * 24 * 60 * 60; // 7 * 24 * 60 * 60 = 7 дней

    // Ключ для шифрования локальных данных
    const QByteArray LOCAL_ENCRYPTION_KEY = "LocalSecretEncryptionKey";

    // Ключ для шифрования данных в БД (имя, баланс)
    const QByteArray DB_ENCRYPTION_KEY = "DatabaseSecretEncryptionKey";
};

#endif // DATABASEMANAGEMENT_H
