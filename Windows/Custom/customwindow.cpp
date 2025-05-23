#include "customwindow.h"
#include "ui_customwindow.h"

#include <QLabel>
#include <QPushButton>

CustomWindow::CustomWindow(WindowType type, const QString& mainText, const QString& windowTitle, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::CustomWindow),
      m_windowType(type)
{
    ui->setupUi(this);

    // Отключаем знак вопроса
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupWindow(type, mainText, windowTitle);
}

CustomWindow::~CustomWindow()
{
    delete ui;
}

void CustomWindow::setupWindow(WindowType type, const QString& mainText, const QString& windowTitle)
{
    m_windowType = type;

    QLabel* contentLabel = findChild<QLabel*>("gameNameLabel");
    QPushButton* deleteButton = findChild<QPushButton*>("deleteButton");
    QPushButton* cancelButton = findChild<QPushButton*>("cancelButton");
    QPushButton* contunieButton = findChild<QPushButton*>("contunieButton");

    switch (m_windowType) {
        case DeleteConfirmation:
            setWindowTitle("Подтверждение удаления");
            contentLabel->setText(QString("Удалить игру '%1'?").arg(mainText));
            deleteButton->show();
            cancelButton->show();
            contunieButton->hide();
            break;

        case UserInfo:
            setWindowTitle("Информация о пользователе");
            contentLabel->setText(mainText);
            deleteButton->hide();
            cancelButton->hide();
            contunieButton->show();
            break;

        case GeneralInfo:
            if (!windowTitle.isEmpty()) {
                setWindowTitle(windowTitle);
            } else {
                setWindowTitle("Информация");
            }
            contentLabel->setText(mainText);
            deleteButton->hide();
            cancelButton->hide();
            contunieButton->show();
            break;

        default:
            contentLabel->setText("Ошибка: Неизвестный тип окна.");
            deleteButton->hide();
            cancelButton->hide();
            contunieButton->show();
            setWindowTitle("Ошибка");
            break;
    }

}

void CustomWindow::on_deleteButton_clicked()
{
    QDialog::accept();
}

void CustomWindow::on_cancelButton_clicked()
{
    QDialog::reject();
}

void CustomWindow::on_contunieButton_clicked()
{
    QDialog::reject();
}

