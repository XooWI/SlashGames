#ifndef SLOTSGAME_H
#define SLOTSGAME_H

#include <QDialog> //
#include <QResizeEvent>
#include <QTimer>
#include <QTime>
#include "./ui_slotsgame.h"


class slotsgame : public QDialog
{
    Q_OBJECT

public:
    slotsgame(QWidget *parent = nullptr);
    ~slotsgame();
    void setPos(int x, int y, QWidget* obj);
    int slotVal1,slotVal2,slotVal3;
private:
    QTimer *timer;
    Ui::slotsgame *ui;
private slots:
    void resizeEvent(QResizeEvent* event);
    void framePerSecond();
    //void on_pushButton_clicked(bool checked);
    void on_pushButton_clicked();
    //void on_pushButton_released();
    //void on_horizontalSlider_actionTriggered(int action);
    void on_horizontalSlider_sliderMoved(int position);
};
#endif // SLOTSGAME_H
