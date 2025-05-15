#include "slotwindow.h"
#include "ui_slotwindow.h"
#include <QRandomGenerator>

SlotWindow::SlotWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SlotWindow)
{
    ui->setupUi(this);
}

SlotWindow::~SlotWindow()
{
    delete ui;
}

void SlotWindow::on_pushButton_clicked()
{
    QString val1 = "background-image: url(:/resources/shrimpThvg200200.png);";
    QString val2 = "background-image: url(:/resources/shrimpThvg200200hover.png);";
    QString val;
    int chance = qrand() % ((1 + 1) - 0) + 0;
    if (chance == 0){
        val = val1;
    }else{
        val = val2;
    }
    ui->label->setStyleSheet(val);
    ui->label_2->setStyleSheet(val);
    ui->label_3->setStyleSheet(val);
}

