#ifndef FAQWINDOW_H
#define FAQWINDOW_H

#include <QDialog>

namespace Ui {
class FAQWindow;
}

class FAQWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FAQWindow(QWidget *parent = nullptr);
    ~FAQWindow();

private slots:
    void on_telegramSocial_clicked();
    void on_githubSocial_clicked();

private:
    Ui::FAQWindow *ui;
};

#endif // FAQWINDOW_H
