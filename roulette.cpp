#include "roulette.h"
#include "ui_roulette.h"

roulette::roulette(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::roulette)
{
    ui->setupUi(this);
}

roulette::~roulette()
{
    delete ui;
}
