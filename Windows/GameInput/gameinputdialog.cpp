#include "gameinputdialog.h"
#include "ui_gameinputdialog.h"

GameInputDialog::GameInputDialog(bool isEditMode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameInputDialog)
{
    ui->setupUi(this);

    // Убираем знак вопроса
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    if (isEditMode) {
        this->setWindowTitle("Редактировать игру");
        ui->acceptButton->setText("Сохранить");
    }
}

GameInputDialog::~GameInputDialog()
{
    delete ui;
}

void GameInputDialog::on_browseIconButton_clicked()
{
    ui->iconPathEdit->setStyleSheet("border: 1px solid dark-gray;");
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите иконку игры", "", "Иконка (*.png *.jpg *.jpeg *.bmp)");
    if (!fileName.isEmpty()) {
        QImage image(fileName);

        if (image.width() < MIN_WIDTH_ICON || image.height() < MIN_HEIGHT_ICON) {
            CustomWindow warningDialog(CustomWindow::GeneralInfo,
                         QString("Изображение слишком маленькое.\nМинимальный размер: %1x%2 пикселей.").arg(MIN_WIDTH_ICON, MIN_HEIGHT_ICON),
                         "Ошибка", this);
            warningDialog.exec();
            return;
        }
        ui->iconPathEdit->setText(fileName);
        ui->iconPathEdit->setStyleSheet("");
    }
}

void GameInputDialog::on_browseFileButton_clicked()
{
    ui->filePathEdit->setStyleSheet("border: 1px solid dark-gray;");
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите файл игры", "",
                       "Испольняемые файлы (*.exe);;Все файлы (*.*)");
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
        return;
    }

    accept();
}

void GameInputDialog::on_cancelButton_clicked()
{
    reject();
}

QString GameInputDialog::getName()
{
    return ui->nameEdit->text();
}

QString GameInputDialog::getIconPath()
{
    return ui->iconPathEdit->text();
}

QString GameInputDialog::getFilePath()
{
    return ui->filePathEdit->text();
}


void GameInputDialog::setName(const QString& name)
{
    ui->nameEdit->setText(name);
}

void GameInputDialog::setIconPath(const QString& path)
{
    ui->iconPathEdit->setText(path);
}

void GameInputDialog::setFilePath(const QString& path)
{
    ui->filePathEdit->setText(path);
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

