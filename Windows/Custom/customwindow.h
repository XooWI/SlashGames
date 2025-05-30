#ifndef CUSTOMWINDOW_H
#define CUSTOMWINDOW_H

#include <QDialog>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QDate>
#include <QMessageBox>

#include "Style/customstyle.h"
#include "Database/databasemanagement.h"


namespace Ui {
class CustomWindow;
}

class CustomWindow: public QDialog
{
    Q_OBJECT

public:
    enum WindowType {
        ConfirmationDeleteGame,
        UserInfo,
        GeneralInfo,
        Confirmation,
        ConfirmationPay,
        EditProfile
    };

    CustomWindow(WindowType type,
                          const QString& mainText,
                          const QString& windowTitle = QString(),
                          QWidget *parent = nullptr,
                          DatabaseManagement *dbManager=nullptr);
    ~CustomWindow();

private slots:
    void on_acceptButton_clicked();
    void on_cancelButton_clicked();
    void on_contunieButton_clicked();


private:
    void setupWindow(WindowType type, const QString& mainText, const QString& windowTitle);

    Ui::CustomWindow *ui;
    QWidget *parent = nullptr;
    DatabaseManagement *dbManager=nullptr;
};

#endif // CUSTOMWINDOW_H
