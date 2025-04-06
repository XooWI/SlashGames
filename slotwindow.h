#ifndef SLOTWINDOW_H
#define SLOTWINDOW_H

#include <QObject>
#include <QWidget>

class SlotWindow : public QWidget
{
    Q_OBJECT
public:
    explicit SlotWindow(QWidget *parent = nullptr);

signals:

};

#endif // SLOTWINDOW_H
