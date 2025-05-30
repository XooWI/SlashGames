#ifndef FAQWINDOW_H
#define FAQWINDOW_H

#include <QDialog>
#include <QDesktopServices>
#include <QUrl>

#include "Style/customstyle.h"

namespace Ui {
class FAQWindow;
}

class FAQWindow : public QDialog
{
    Q_OBJECT

public:
    FAQWindow(QWidget *parent = nullptr);
    ~FAQWindow();

private slots:
    void on_telegramSocial_clicked();
    void on_githubSocial_clicked();

private:
    Ui::FAQWindow *ui;

    const QString telegramSocialPath = "https://t.me/SlashGames_support_bot";
    const QString githubSocialPath = "https://github.com/XooWI/SlashGames";
};

#endif // FAQWINDOW_H
