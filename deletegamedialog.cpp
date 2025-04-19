#include "deletegamedialog.h"
#include "ui_deletegamedialog.h"

DeleteGameDialog::DeleteGameDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::DeleteGameDialog)
{
    ui->setupUi(this);
    // Отключаем знак вопроса
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->gameNameLabel->setText(QString("Удалить игру '%1'?").arg(gameName));

}

DeleteGameDialog::~DeleteGameDialog()
{
    delete ui;
}

void DeleteGameDialog::on_deleteButton_clicked()
{
    QDialog::reject();
}


void DeleteGameDialog::on_cancelButton_clicked()
{
    QDialog::accept();
}

