#ifndef SLOTWINDOW_H
#define SLOTWINDOW_H

#include <QWidget>

namespace Ui {
class SlotWindow;
}

class SlotWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SlotWindow(QWidget *parent = nullptr);
    ~SlotWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::SlotWindow *ui;
};

#endif // SLOTWINDOW_H
