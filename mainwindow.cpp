#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <string>
#include <chrono>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_depButton_clicked()
{
    if(ui->timeLabel->text() == "00:00:00"){
        QString balance = ui->balanceLabel->text();
        double newBalance = balance.toDouble() + 3000;
        QString newBalanceString = QString::number(newBalance);
        ui->balanceLabel->setText(newBalanceString);

        ui->timeLabel->setText("03:00:00");

//        QTimer *objTimer = new QTimer ();
//        QTime *objTime  = new QTime  (0, minutes, seconds);
//        objTime->setHMS (0, objTime->addSecs (-1).minute (), objTime->addSecs (-1).second ());
//        display (objTime->toString ());


    }
}

