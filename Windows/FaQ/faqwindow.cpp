#include "faqwindow.h"
#include "ui_faqwindow.h"

FAQWindow::FAQWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FAQWindow)
{
    ui->setupUi(this);

    // Отключаем знак вопроса
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Установка текста
    ui->rulesLabel->setText(CustomStyle::getTextRules());
    ui->faqLabel->setText(CustomStyle::getTextFAQ());
}


void FAQWindow::on_telegramSocial_clicked()
{
    QDesktopServices::openUrl(QUrl(telegramSocialPath));
}


void FAQWindow::on_githubSocial_clicked()
{
    QDesktopServices::openUrl(QUrl(githubSocialPath));
}


FAQWindow::~FAQWindow()
{
    delete ui;
}

