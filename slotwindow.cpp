#include "slotwindow.h"
#include "ui_slotwindow.h"

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

}

