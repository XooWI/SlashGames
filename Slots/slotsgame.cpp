#include "slotsgame.h"
#include "animFunc.h"
#include <QTimer>
#include <QDebug>
#include <cstdlib>

QWidget* slotWidgets[3];
int slotValue[3];
int bet;

slotsgame::slotsgame(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::slotsgame)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &slotsgame::framePerSecond);
    timer->start(1000/60);

    slotWidgets[0]=ui->slotW;
    slotWidgets[1]=ui->slotW_2;
    slotWidgets[2]=ui->slotW_3;

    for (int i=0;i<3;i++){
        slotAnimations[i].t=100;
        slotWidgets[i]->resize(135,1350);
    }
    ui->widget->resize(2000,2000);

    bet = ui->horizontalSlider->value();
    ui->betValue->setText(QString::number(bet));

}

slotsgame::~slotsgame()
{
    delete ui;
}

void slotsgame::setPos(int x, int y, QWidget* obj){
    if (ui->centralwidget) {
        obj->move(ui->centralwidget->geometry().center().x()-obj->width()/2+x,
                  ui->centralwidget->geometry().center().y()-obj->height()/2-y);
    } else {
        obj->move(x,y);
    }
}

void slotsgame::resizeEvent(QResizeEvent* event)
{
    QDialog::resizeEvent(event);

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

    for (int i=0;i<3;i++){
        slotAnimations[i].startPos=slotWidgets[i]->pos();
        slotAnimations[i].init(slotWidgets[i]);
    }
}

void slotsgame::framePerSecond(){
    int acceleration=3000;
    for (int i=0;i<3;i++){
        slotAnimations[i].slot(slotWidgets[i],acceleration,slotValue[i]);
    }
}

void slotsgame::on_pushButton_clicked()
{
    ui->youWin_2->show();
    ui->youWin->hide();
    for (int i=0;i<3;i++){slotValue[i]=rand()%7;}
    if (slotValue[0]==slotValue[1] && slotValue[1]==slotValue[2]){
        ui->youWin_2->hide();
        ui->youWin->show();
    }
    for (int i=0;i<3;i++){slotAnimations[i].t=0;}
}

void slotsgame::on_horizontalSlider_sliderMoved(int position)
{
    if (position < 10){
        bet = 2;
    } else {
        bet = position;
    }
    ui->betValue->setText(QString::number(bet));
}

void slotsgame::closeEvent(QCloseEvent *event)
{
    ExitConfirmDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        event->accept();
    } else {
        event->ignore();
    }
}

ExitConfirmDialog::ExitConfirmDialog(QWidget *parent)
    : QDialog(parent)
{
    setModal(true);
    setFixedSize(650, 400);
    setWindowTitle("Подтвердите выход");
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 40, 50, 40);
    mainLayout->setSpacing(25);

    messageLabel = new QLabel("<html><head/><body>"
                              "<p align='center'><span style=\" font-size:28pt; font-weight:700; color:#FFD700;\">ОСТАНОВИТЕСЬ!</span></p>"
                              "<p align='center'><span style=\" font-size:18pt; color:#FFFFFF;\">Вы уверены, что хотите уйти сейчас?</span></p>"
                              "<p align='center'><span style=\" font-size:18pt; font-weight:600; color:#FFC125;\">90% игроков уходят перед джекпотом!</span></p>"
                              "</body></html>", this);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setWordWrap(true);
    mainLayout->addWidget(messageLabel);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(50);
    buttonLayout->addStretch();

    yesButton = new QPushButton("Да, Выйти", this);
    yesButton->setFixedSize(160, 50);
    yesButton->setStyleSheet("QPushButton {"
                             "background-color: #550000;"
                             "color: #AAAAAA;"
                             "font: bold 16px 'Arial';"
                             "border-radius: 10px;"
                             "border: 1px solid #880000;"
                             "}"
                             "QPushButton:hover { background-color: #880000; color: white; }"
                             "QPushButton:pressed { background-color: #440000; }");
    connect(yesButton, &QPushButton::clicked, this, &QDialog::accept);

    noButton = new QPushButton("Нет, Остаться", this);
    noButton->setFixedSize(220, 70);
    noButton->setStyleSheet("QPushButton {"
                            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #32CD32, stop: 1 #008000);"
                            "color: white;"
                            "font: bold 24px 'Impact';"
                            "border-radius: 15px;"
                            "border: 4px solid #00FF00;"
                            "}"
                            "QPushButton:hover { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #00FF00, stop: 1 #32CD32); }"
                            "QPushButton:pressed { background-color: #006400; }");
    connect(noButton, &QPushButton::clicked, this, &QDialog::reject);

    buttonLayout->addWidget(yesButton);
    buttonLayout->addWidget(noButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    setStyleSheet("QDialog {"
                  "background-color: #1a1a1a;"
                  "border: 5px solid #FFD700;"
                  "border-radius: 25px;"
                  "}"
                  "QLabel { color: white; }"
                  );
}
