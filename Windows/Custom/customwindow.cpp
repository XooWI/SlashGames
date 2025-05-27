#include "customwindow.h"
#include "ui_customwindow.h"

CustomWindow::CustomWindow(WindowType type, const QString& mainText, const QString& windowTitle, QWidget *parent, DatabaseManagement *dbManager)
    : QDialog(parent),
      ui(new Ui::CustomWindow),
      dbManager(dbManager)
{
    ui->setupUi(this);

    // Отключаем знак вопроса & Qt::WindowTitleHint
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupWindow(type, mainText, windowTitle);
}

CustomWindow::~CustomWindow()
{
    delete ui;
}

void CustomWindow::setupWindow(WindowType type, const QString& mainText, const QString& windowTitle)
{
    QLabel* contentLabel = findChild<QLabel*>("gameNameLabel");
    QPushButton* acceptButton = findChild<QPushButton*>("acceptButton");
    QPushButton* cancelButton = findChild<QPushButton*>("cancelButton");
    QPushButton* contunieButton = findChild<QPushButton*>("contunieButton");

    QLineEdit* nameLineEdit = findChild<QLineEdit*>("nameLineEdit");
    QLineEdit* oldPasswordLineEdit = findChild<QLineEdit*>("oldPasswordLineEdit");
    QLineEdit* newPasswordLineEdit = findChild<QLineEdit*>("newPasswordLineEdit");
    QLineEdit* confirmNewPasswordLineEdit = findChild<QLineEdit*>("confirmNewPasswordLineEdit");

    if (!contentLabel || !acceptButton || !cancelButton || !contunieButton || !nameLineEdit || !oldPasswordLineEdit || !newPasswordLineEdit || !confirmNewPasswordLineEdit) {
        return;
    }

    acceptButton->hide();
    cancelButton->hide();
    contunieButton->hide();
    nameLineEdit->hide();
    oldPasswordLineEdit->hide();
    newPasswordLineEdit->hide();
    confirmNewPasswordLineEdit->hide();

    switch (type) {
        case ConfirmationDeleteGame:
            setWindowTitle("Подтверждение удаления");
            contentLabel->setText(QString("Удалить игру '%1'?").arg(mainText));
            acceptButton->setText("Удалить");
            cancelButton->setText("Отмена");
            acceptButton->show();
            cancelButton->show();
            break;

        case UserInfo: {
            setWindowTitle("Информация о пользователе");
            QStringList userInfoParts = mainText.split(',');
            if (userInfoParts.size() >= 4) {
                QString name = userInfoParts.at(0);
                QString balance = QLocale(QLocale::English).toString(userInfoParts.at(1).toInt()).replace(",", " ") + "💲";
                QDate registrationDate = QDate::fromString(userInfoParts.at(2), "yyyy-MM-dd");
                QDateTime tokenExpiryDateTime = QDateTime::fromString(userInfoParts.at(3), Qt::ISODate);

                long long daysRemaining = QDate::currentDate().daysTo(tokenExpiryDateTime.date());

                QString daysRemainingText = "<span style='color: red;'>(-)</span>";
                QString tokenDateColorStyle = "color: red;";

                if (daysRemaining > 0) {
                    daysRemainingText = QString("<span style='color: #4CAF50;'>(%1 дн.)</span>").arg(daysRemaining);
                    tokenDateColorStyle = "color: #4CAF50;";
                } else if (daysRemaining == 0) {
                    daysRemainingText = "<span style='color: orange;'>(сегодня)</span>";
                    tokenDateColorStyle = "color: orange;";
                }

            contentLabel->setText(CustomStyle::userInfoText().arg(
                                  name,
                                  balance,
                                  registrationDate.toString("dd.MM.yyyy"),
                                  tokenExpiryDateTime.toString("dd.MM.yyyy"),
                                  daysRemainingText,
                                  tokenDateColorStyle));
            } else {
                contentLabel->setText("Ошибка");
            }
            contunieButton->setText("ОК");
            contunieButton->show();
            break;
        }

        case GeneralInfo:
            if (!windowTitle.isEmpty()) {
                setWindowTitle(windowTitle);
            } else {
                setWindowTitle("Информация");
            }
            contentLabel->setText(mainText);
            contunieButton->setText("ОК");
            contunieButton->show();
            break;

        case Confirmation:
            if (!windowTitle.isEmpty()) {
                setWindowTitle(windowTitle);
            } else {
                setWindowTitle("Подтверждение");
            }
            contentLabel->setText(mainText);
            acceptButton->setText("Да");
            cancelButton->setText("Нет");
            acceptButton->show();
            cancelButton->show();
            break;

    case EditProfile: {
            setWindowTitle("Редактирование профиля");
            nameLineEdit->show();
            oldPasswordLineEdit->show();
            newPasswordLineEdit->show();
            confirmNewPasswordLineEdit->show();

            contentLabel->setText("Редактирование");
            contentLabel->setStyleSheet("font-size: 30px;");
            ui->oldPasswordLineEdit->setFocus();

            nameLineEdit->setStyleSheet("");
            oldPasswordLineEdit->setStyleSheet("");
            newPasswordLineEdit->setStyleSheet("");
            confirmNewPasswordLineEdit->setStyleSheet("");

            nameLineEdit->setText(dbManager->getUsername());

            acceptButton->setText("Сохранить");
            acceptButton->setStyleSheet("QPushButton#acceptButton {background-color: #4CAF50;}QPushButton#acceptButton:hover {background-color: #45a049;}");
            cancelButton->setText("Отмена");
            cancelButton->setStyleSheet("QPushButton#cancelButton {background-color: #f44336;}QPushButton#cancelButton:hover {background-color: #b71c1c;}");
            acceptButton->show();
            cancelButton->show();

            connect(acceptButton, &QPushButton::clicked, this, [this, nameLineEdit, oldPasswordLineEdit, newPasswordLineEdit, confirmNewPasswordLineEdit]() {
                QString newName = nameLineEdit->text();
                QString oldPassword = oldPasswordLineEdit->text();
                QString newPassword = newPasswordLineEdit->text();
                QString confirmNewPassword = confirmNewPasswordLineEdit->text();


                if (newName.isEmpty()) {
                    nameLineEdit->setStyleSheet("border: 1px solid red;");
                    return;
                }

                if (newPassword != confirmNewPassword) {
                    newPasswordLineEdit->setStyleSheet("border: 1px solid red;");
                    confirmNewPasswordLineEdit->setStyleSheet("border: 1px solid red;");
                    return;
                }
                if (oldPassword.isEmpty()) {
                    oldPasswordLineEdit->setStyleSheet("border: 1px solid red;");
                    return;
                }
                qDebug() << "Успешно";
            });

            connect(cancelButton, &QPushButton::clicked, this, &CustomWindow::on_cancelButton_clicked);

            break;
        }

        default:
            contentLabel->setText("Ошибка: Неизвестный тип окна.");
            contunieButton->setText("ОК");
            contunieButton->show();
            setWindowTitle("Ошибка");
            break;
    }

    // Перерасчет оптимального размера окна
    adjustSize();
}

void CustomWindow::on_acceptButton_clicked()
{
    QDialog::accept();
}

void CustomWindow::on_cancelButton_clicked()
{
    QDialog::reject();
}

void CustomWindow::on_contunieButton_clicked()
{
    QDialog::accept();
}

