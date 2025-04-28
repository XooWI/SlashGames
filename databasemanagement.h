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
    explicit DatabaseManagement(QSettings* settings);
    ~DatabaseManagement();

    bool registredUser(const QString &username, const QString &login, const QByteArray &hashedPassword);

    bool checkLogin(const QString &login);
    bool checkUser(const QString &login, const QByteArray &hashedPassword);
    bool checkToken();

    int getBalance();
    QString getToken();
    QString getUsername();

private:
    void initializeDatabase();
    QString getUniqueID();

    QSqlDatabase db;
    QString connectionName;
    QSettings *settings;
};

#endif // DATABASEMANAGEMENT_H
