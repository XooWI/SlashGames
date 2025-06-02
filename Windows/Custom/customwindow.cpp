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

    case ConfirmationPay:
        if (!windowTitle.isEmpty()) {
            setWindowTitle(windowTitle);
        } else {
            setWindowTitle("Подтверждение");
        }
        contentLabel->setText(mainText);
        acceptButton->setText("Подтвердить");
        cancelButton->setText("Отмена");
        acceptButton->setStyleSheet("QPushButton#acceptButton {background-color: #4CAF50;}QPushButton#acceptButton:hover {background-color: #45a049;}");
        cancelButton->setStyleSheet("QPushButton#cancelButton {background-color: #f44336;}QPushButton#cancelButton:hover {background-color: #b71c1c;}");
        acceptButton->show();
        cancelButton->show();
        break;

    case EditProfile: {
        setWindowTitle("Редактирование профиля");

        ui->nameLineEdit->show();
        ui->oldPasswordLineEdit->show();
        ui->newPasswordLineEdit->show();
        ui->confirmNewPasswordLineEdit->show();

        contentLabel->setText("Редактирование");
        contentLabel->setStyleSheet("font-size: 30px;");
        ui->oldPasswordLineEdit->setFocus();

        ui->nameLineEdit->setStyleSheet("");
        ui->oldPasswordLineEdit->setStyleSheet("");
        ui->newPasswordLineEdit->setStyleSheet("");
        ui->confirmNewPasswordLineEdit->setStyleSheet("");

        ui->nameLineEdit->setText(dbManager->getUsername());

        ui->acceptButton->setText("Сохранить");
        ui->acceptButton->setStyleSheet("QPushButton#acceptButton {background-color: #4CAF50;}QPushButton#acceptButton:hover {background-color: #45a049;}");
        ui->cancelButton->setText("Отмена");
        ui->cancelButton->setStyleSheet("QPushButton#cancelButton {background-color: #f44336;}QPushButton#cancelButton:hover {background-color: #b71c1c;}");
        ui->acceptButton->show();
        ui->cancelButton->show();


        disconnect(ui->acceptButton, &QPushButton::clicked, nullptr, nullptr);
        connect(ui->acceptButton, &QPushButton::clicked, this, [this]() {
            QString newName = ui->nameLineEdit->text().trimmed();
            QString oldPassword = ui->oldPasswordLineEdit->text();
            QString newPassword = ui->newPasswordLineEdit->text();
            QString confirmNewPassword = ui->confirmNewPasswordLineEdit->text();

            // Сброс стилей ошибок
            ui->nameLineEdit->setStyleSheet("");
            ui->oldPasswordLineEdit->setStyleSheet("");
            ui->newPasswordLineEdit->setStyleSheet("");
            ui->confirmNewPasswordLineEdit->setStyleSheet("");

            bool hasError = false;
            QString errorMessage = "";

            if (newName.isEmpty()) {
                ui->nameLineEdit->setStyleSheet("border: 1px solid red;");
                errorMessage += "Имя пользователя не может быть пустым.\n";
                hasError = true;
            }

            if (oldPassword.isEmpty()) {
                ui->oldPasswordLineEdit->setStyleSheet("border: 1px solid red;");
                errorMessage += "Пожалуйста, введите старый пароль.\n";
                hasError = true;
            }

            if (!newPassword.isEmpty() && newPassword != confirmNewPassword) {
                ui->newPasswordLineEdit->setStyleSheet("border: 1px solid red;");
                ui->confirmNewPasswordLineEdit->setStyleSheet("border: 1px solid red;");
                errorMessage += "Новый пароль и подтверждение не совпадают.\n";
                hasError = true;
            }

            if (!newPassword.isEmpty() && dbManager->password_strength(newPassword) != 0) {
                ui->newPasswordLineEdit->setStyleSheet("border: 1px solid red;");
                errorMessage += "Новый пароль слишком слабый. Используйте комбинацию букв, цифр и символов.\n";
                hasError = true;
            }

            if (!newPassword.isEmpty() && newPassword==oldPassword) {
                ui->newPasswordLineEdit->setStyleSheet("border: 1px solid red;");
                errorMessage += "Новый пароль совпадает со старым\n";
                hasError = true;
            }

            if (hasError) {
                CustomWindow errorDialog(CustomWindow::GeneralInfo, errorMessage.trimmed(), "Ошибка ввода", this);
                errorDialog.exec();
                return;
            }

            if (!(dbManager->checkPassword(oldPassword))) {
                ui->oldPasswordLineEdit->setStyleSheet("border: 1px solid red;");
                CustomWindow errorDialog(CustomWindow::GeneralInfo, "Неверный старый пароль.", "Ошибка авторизации", this);
                errorDialog.exec();
                return;
            }

            bool updateSuccess = true;
            bool changesMade = false;

            if (newName != dbManager->getUsername()) {
                if (dbManager->updateUsername(newName)) {
                    changesMade = true;
                } else {
                    CustomWindow errorDialog(CustomWindow::GeneralInfo, "Не удалось обновить имя пользователя.", "Ошибка обновления", this); // Возможно, CustomWindow::Error
                    errorDialog.exec();
                    updateSuccess = false;
                }
            }

            if (!newPassword.isEmpty() && updateSuccess) {
                if (dbManager->updatePassword(newPassword)) {
                    changesMade = true;
                } else {
                    CustomWindow errorDialog(CustomWindow::GeneralInfo, "Не удалось обновить пароль.", "Ошибка обновления", this);
                    errorDialog.exec();
                    updateSuccess = false;
                }
            }

            if (updateSuccess && changesMade) {
                CustomWindow infoDialog(CustomWindow::GeneralInfo, "Данные профиля успешно обновлены.", "Успех", this);
                infoDialog.exec();
                QDialog::accept();
            } else if (!changesMade && updateSuccess) {
                CustomWindow infoDialog(CustomWindow::GeneralInfo, "Изменений не обнаружено.", "Информация", this);
                infoDialog.exec();
            }

        });

        connect(ui->cancelButton, &QPushButton::clicked, this, &CustomWindow::on_cancelButton_clicked, Qt::UniqueConnection); // Также с UniqueConnection
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
    setFixedSize(sizeHint());
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

