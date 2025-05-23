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
    
    void setTheme(bool isDark);

private:
    Ui::FAQWindow *ui;
};

#endif // FAQWINDOW_H
