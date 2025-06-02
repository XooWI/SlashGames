#ifndef SLOTSGAME_H
#define SLOTSGAME_H

#include <QDialog>
#include <QResizeEvent>
#include <QTimer>
#include <QTime>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "animFunc.h"
#include "ui_slotsgame.h"


class ExitConfirmDialog;

class slotsgame : public QDialog
{
    Q_OBJECT

public:
    slotsgame(QWidget *parent = nullptr);
    ~slotsgame();

private:
    Ui::slotsgame *ui;
    QTimer *timer;

    Animator slotAnimations[3];
    QWidget* slotWidgets[3];
    int slotValue[3];
    int bet;

    void setPos(int x, int y, QWidget* obj);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void resizeEvent(QResizeEvent* event) override;
    void framePerSecond();
    void on_pushButton_clicked();
    void on_horizontalSlider_sliderMoved(int position);
};

class ExitConfirmDialog : public QDialog
{
    Q_OBJECT
public:
    ExitConfirmDialog( QWidget *parent = nullptr);

private:
    QLabel *messageLabel;
    QLabel *balanceLabel;
    QPushButton *yesButton;
    QPushButton *noButton;


};

#endif // SLOTSGAME_H
