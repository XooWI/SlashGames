#include "mainwindow.h"
#include "animFunc.h"
#include "./ui_mainwindow.h"
#include <QTimer>
#include <QDebug>

Animator slotAnimations[3];
QWidget* slotWidgets[3];
int slotValue[3];
int bet;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(framePerSecond()));
    timer->start(1000/60); // И запустим таймер
    slotWidgets[0]=ui->slotW;
    slotWidgets[1]=ui->slotW_2;
    slotWidgets[2]=ui->slotW_3;
    for (int i=0;i<3;i++){slotAnimations[i].t=100;}
    for (int i=0;i<3;i++){slotWidgets[i]->resize(135,1350);}
    ui->widget->resize(2000,2000);

}
void MainWindow::setPos(int x, int y, QWidget* obj){
    obj->move(ui->centralwidget->geometry().center().x()-obj->width()/2+x,ui->centralwidget->geometry().center().y()-obj->height()/2-y);
}
void MainWindow::resizeEvent(QResizeEvent* event)
{
    //Положение объектов на
    int slY=135*3.5-18;
    setPos(0,-220,ui->pushButton);
    setPos(0,slY,ui->slotW_2);
    setPos(-133,slY,ui->slotW);
    setPos(133,slY,ui->slotW_3);
    setPos(0,0,ui->widget);
    setPos(385,-190,ui->bet);
    setPos(41,150,ui->bonus);
    setPos(30,200,ui->youWin);
    setPos(30,200,ui->youWin_2);
    setPos(350,-192,ui->horizontalSlider);
    setPos(355,-220,ui->betValue);
    setPos(-203,-192,ui->balance);
    setPos(-203,-196,ui->balanceValue);
    ui->bonus->hide();
    ui->youWin->hide();

    for (int i=0;i<3;i++){
        slotAnimations[i].startPos=slotWidgets[i]->pos();
        slotAnimations[i].init(slotWidgets[i]);
    }
}
void MainWindow::framePerSecond(){
    int acceleration=3000;
    for (int i=0;i<3;i++){slotAnimations[i].slot(slotWidgets[i],acceleration,slotValue[i]);}
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->youWin_2->show();
    ui->youWin->hide();
    for (int i=0;i<3;i++){slotValue[i]=rand()%7;}
    if (slotValue[0]==slotValue[1] and slotValue[1]==slotValue[2]){
        ui->youWin_2->hide();
        ui->youWin->show();}
    for (int i=0;i<3;i++){slotAnimations[i].t=0;}
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    if (position<10){bet=2;}
    else{bet=position;}
    ui->betValue->setText(QString::number(bet));
}

