#include "faqwindow.h"
#include "ui_faqwindow.h"
#include <QDesktopServices>
#include <QUrl>

FAQWindow::FAQWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FAQWindow)
    //isDarkTheme(false)
{
    ui->setupUi(this);

    // Отключаем знак вопроса
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(ui->telegramSocial, &QPushButton::clicked, []() {
        QDesktopServices::openUrl(QUrl("https://t.me/SlashGames_support_bot"));
    });

    connect(ui->githubSocial, &QPushButton::clicked, []() {
        QDesktopServices::openUrl(QUrl("https://github.com/XooWI/SlashGames"));
    });
}

FAQWindow::~FAQWindow()
{
    delete ui;
}

