#include "faqwindow.h"
#include "ui_faqwindow.h"
#include <QDesktopServices>
#include <QUrl>

FAQWindow::FAQWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FAQWindow)
{
    ui->setupUi(this);

    // Отключаем знак вопроса
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Установка текста
        ui->rulesLabel->setText(
            "<h2>📋 Основные правила</h2>"
            "<p><b style='color: #3498db;'>1.</b> Минимальная ставка: <b>100 монет</b></p>"
            "<p><b style='color: #3498db;'>2.</b> Максимальная ставка: <b>10 000 монет</b></p>"
            "<p><b style='color: #3498db;'>3.</b> Время на игру: <b>3 минуты</b></p>"
            "<p><b style='color: #3498db;'>4.</b> При проигрыше ставка <span style='color: #e74c3c;'>не возвращается</span></p>"
            "<p><b style='color: #3498db;'>5.</b> При выигрыше ставка <span style='color: #2ecc71;'>удваивается</span></p>"
            "<p><b style='color: #3498db;'>6.</b> Бонус можно получать каждые <b>3 часа</b></p>"
            "<p><b style='color: #3498db;'>7.</b> Запрещено использование скриптов и ботов</p>"
        );

        ui->faqLabel->setText(
            "<h2>❓ Часто задаваемые вопросы</h2>"
            "<p><b style='color: #3498db;'>Q: Как начать игру?</b></p>"
            "<p style='margin-left: 10px;'>A: Выберите игру из списка, установите желаемую ставку (от 100 до 10 000 монет) и нажмите кнопку \"Играть\".</p>"
            "<p><b style='color: #3498db;'>Q: Как получить бонус?</b></p>"
            "<p style='margin-left: 10px;'>A: Нажмите кнопку \"Получить Бонус!\" в главном меню. Бонус доступен каждые 3 часа.</p>"
            "<p><b style='color: #3498db;'>Q: Что делать при технических проблемах?</b></p>"
            "<p style='margin-left: 10px;'>A: Обратитесь в поддержку через Telegram (@SlashGames_support_bot)</p>"
            "<p><b style='color: #3498db;'>Q: Как предложить новую игру?</b></p>"
            "<p style='margin-left: 10px;'>A: Напишите нам в поддержку с описанием вашей идеи. Лучшие предложения будут реализованы!</p>"
            "<p><b style='color: #3498db;'>Q: Где мои выигрыши?</b></p>"
            "<p style='margin-left: 10px;'>A: Все выигрыши автоматически зачисляются на ваш баланс. Вы можете увидеть изменение суммы в верхней части экрана.</p>"
        );
}

void FAQWindow::on_telegramSocial_clicked()
{
    QDesktopServices::openUrl(QUrl("https://t.me/SlashGames_support_bot"));
}


void FAQWindow::on_githubSocial_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/XooWI/SlashGames"));
}


FAQWindow::~FAQWindow()
{
    delete ui;
}

