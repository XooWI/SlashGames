#include "gameinputdialog.h"
#include "ui_gameinputdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>

GameInputDialog::GameInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameInputDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

GameInputDialog::~GameInputDialog()
{
    delete ui;
}

void GameInputDialog::on_browseIconButton_clicked()
{
    ui->iconPathEdit->setStyleSheet("border: 1px solid dark-gray;");
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите иконку", "",
                       "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!fileName.isEmpty()) {
        ui->iconPathEdit->setText(fileName);
        ui->iconPathEdit->setStyleSheet("");
    }
}

void GameInputDialog::on_browseFileButton_clicked()
{
    ui->filePathEdit->setStyleSheet("border: 1px solid dark-gray;");
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите файл игры", "",
                       "Executable files (*.exe);;All files (*.*)");
    if (!fileName.isEmpty()) {
        ui->filePathEdit->setText(fileName);
        ui->filePathEdit->setStyleSheet("");
    }
}

void GameInputDialog::on_acceptButton_clicked()
{
    bool hasError = false;

    if (ui->nameEdit->text().isEmpty()) {
        ui->nameEdit->setStyleSheet("border: 1px solid red;");
        hasError = true;
    }

    if (ui->iconPathEdit->text().isEmpty() || !QFile::exists(ui->iconPathEdit->text())) {
        ui->iconPathEdit->setStyleSheet("border: 1px solid red;");
        hasError = true;
    }

    if (ui->filePathEdit->text().isEmpty() || !QFile::exists(ui->filePathEdit->text())) {
        ui->filePathEdit->setStyleSheet("border: 1px solid red;");
        hasError = true;
    }

    if (hasError) {
        //QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните все поля корректно");
        return;
    }

    accept();
}

QString GameInputDialog::getName()
{
    return ui->nameEdit->text();
}

QString GameInputDialog::getIconPath()
{
    return ui->iconPathEdit->text();
}

QString GameInputDialog::getExecutablePath()
{
    return ui->filePathEdit->text();
}

void GameInputDialog::on_cancelButton_clicked()
{
    reject();
}

void GameInputDialog::on_nameEdit_textChanged(const QString &arg1)
{
    ui->nameEdit->setStyleSheet("border: 1px solid dark-gray;");
}


void GameInputDialog::on_iconPathEdit_textChanged(const QString &arg1)
{
    ui->iconPathEdit->setStyleSheet("border: 1px solid dark-gray;");
}


void GameInputDialog::on_filePathEdit_textChanged(const QString &arg1)
{
    ui->filePathEdit->setStyleSheet("border: 1px solid dark-gray;");
}

