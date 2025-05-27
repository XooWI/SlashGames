#include "slotwindow.h"
#include "ui_slotwindow.h"
#include <QRandomGenerator>
#include <QSettings>

SlotWindow::SlotWindow(QSettings *settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SlotWindow)
{
    ui->setupUi(this);
    int balance = settings->value("balance", 0).toInt();
    ui->balanceLabel->setText(QLocale(QLocale::English).toString(balance).replace(",", " ") + "💲");
}

SlotWindow::~SlotWindow()
{
    delete ui;
}

QString randomizer(){
    QString val1 = "background-image: url(:/resources/shrimpBar200200.jpg);";
    QString val2 = "background-image: url(:/resources/shrimpCherry200200.jpg);";
    QString val3 = "background-image: url(:/resources/shrimpLemon200200.jpg);";
    QString val4 = "background-image: url(:/resources/shrimpSeven200200.jpg);";
    QString val;
    int chance = qrand() % 4;
    switch (chance) {
        case 0:
            val = val1;
            break;
        case 1:
            val = val2;
            break;
        case 2:
            val = val3;
            break;
        case 3:
            val = val4;
            break;
    }
    return val;
}

void SlotWindow::on_pushButton_clicked()
{
    QString slotValue1 = randomizer();
    QString slotValue2 = randomizer();
    QString slotValue3 = randomizer();
    if ((slotValue1 == slotValue2) && (slotValue2 == slotValue3)){
       printf("g");
    }
    ui->slot1->setStyleSheet(slotValue1);
    ui->slot2->setStyleSheet(slotValue2);
    ui->slot3->setStyleSheet(slotValue3);
}

//void SlotWindow::saveBalance()
//{
//    settings->setValue("balance", balance);
//    ui->balanceLabel->setText(QLocale(QLocale::English).toString(balance).replace(",", " ") + "💲");
//}
