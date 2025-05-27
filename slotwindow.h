#ifndef SLOTWINDOW_H
#define SLOTWINDOW_H

#include <QWidget>
#include <QSettings>

namespace Ui {
class SlotWindow;
}

class SlotWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SlotWindow(QSettings *settings, QWidget *parent = nullptr);
    ~SlotWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::SlotWindow *ui;

    void saveBalance();
};

#endif // SLOTWINDOW_H
