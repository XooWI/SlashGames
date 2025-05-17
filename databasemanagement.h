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

// Убрать после обновления!
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>

class DatabaseManagement
{
public:
    explicit DatabaseManagement(QSettings* settings = nullptr);
    ~DatabaseManagement();

    bool checkUser(const QString &login, const QByteArray &hashedPassword);
    bool checkLogin(const QString &login);
    bool checkToken();
    bool registredUser(const QString &username, const QString &login, const QByteArray &hashedPassword);

    bool updateBalance(int &balance);

    int getBalance();
    QString getToken();
    QString getUsername();

private:
    QSettings *settings;
    QSqlDatabase db;
    QString connectionName;

    void initializeDatabase();
    QString getUniqueID();


    // Методы шифрования и хэширования
    QByteArray localEncrypt(const QByteArray &data);
    QByteArray dbEncrypt(const QByteArray &data);
    QByteArray hash(const QByteArray &data);


    const qint64 TOKEN_LIFETIME = 120; // 7 * 24 * 60 * 60 7 дней
    // Ключ для шифрования локальных данных
    const QByteArray LOCAL_ENCRYPTION_KEY = "LocalSecretEncryptionKey";

    // Ключ для шифрования данных в БД (баланс)
    const QByteArray DB_ENCRYPTION_KEY = "DatabaseSecretEncryptionKey";
};

#endif // DATABASEMANAGEMENT_H
