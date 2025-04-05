#include "roulette.h"

RouletteWidget::RouletteWidget(QWidget *parent) : QWidget(parent), m_angle(0) {}

qreal RouletteWidget::angle() const { return m_angle; }

void RouletteWidget::setAngle(qreal angle) {
    m_angle = angle;
    update();
}

void RouletteWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRectF rect = QRectF(10, 10, width()-20, height()-20);

    qreal startAngle = 90;
    qreal sectorAngle = 360.0 / 37;

    for(int i = 0; i < 37; ++i) {
        QColor color = (i == 0) ? Qt::green : (i % 2 == 1) ? Qt::red : Qt::black;
        painter.setBrush(color);
        painter.drawPie(rect, startAngle * 16, sectorAngle * 16);
        startAngle += sectorAngle;
    }

    painter.setBrush(Qt::blue);
    painter.translate(rect.center());
    painter.rotate(m_angle);
    painter.drawPolygon(QPolygonF() << QPointF(0, -rect.height()/2 + 10)
                                    << QPointF(-5, 0)
                                    << QPointF(5, 0));
}


void Roulette::placeBet() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if(!button || betAmount->value() <= 0) return;

    QString betType = button->property("betType").toString();
    int amount = betAmount->value();

    if(amount > balance) {
        QMessageBox::warning(this, "Ошибка", "Недостаточно средств!");
        return;
    }

    balance -= amount;
    totalBet += amount;
    bets[betType] += amount;

    updateDisplay();
}

void Roulette::spin() {
    if(totalBet == 0) {
        QMessageBox::warning(this, "Ошибка", "Сделайте ставку!");
        return;
    }

    int rounds = 5 + QRandomGenerator::global()->bounded(5);
    qreal targetAngle = 360 * rounds + QRandomGenerator::global()->bounded(360);

    animation->setStartValue(roulette->angle());
    animation->setEndValue(targetAngle);
    animation->setDuration(3000);
    animation->start();
}

void Roulette::animationFinished() {
    qreal finalAngle = roulette->angle() - 90;
    while(finalAngle < 0) finalAngle += 360;
    int sector = static_cast<int>((360 - fmod(finalAngle, 360)) / (360.0 / 37)) % 37;

    QString result = QString::number(sector);
    QString color = (sector == 0) ? "green" : (sector % 2 == 1) ? "red" : "black";

    int payout = 0;
    if(bets.contains(result)) payout += bets[result] * 35;
    if(bets.contains(color)) payout += bets[color] * 2;

    balance += payout;
    resultLabel->setText(QString("Выпало: %1 (%2)\nВыигрыш: %3")
                             .arg(sector).arg(color).arg(payout));

    totalBet = 0;
    bets.clear();
    updateDisplay();
}

void Roulette::clearBets() {
    balance += totalBet;
    totalBet = 0;
    bets.clear();
    updateDisplay();
}

void Roulette::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);

    roulette = new RouletteWidget;
    roulette->setMinimumSize(400, 400);
    layout->addWidget(roulette);

    QGridLayout *betsLayout = new QGridLayout;
    QStringList betTypes = {"0", "red", "black"};
    for(int i = 0; i < betTypes.count(); ++i) {
        QPushButton *btn = new QPushButton(betTypes[i]);
        btn->setProperty("betType", betTypes[i]);
        betsLayout->addWidget(btn, 0, i);
    }
    layout->addLayout(betsLayout);

    betAmount = new QSpinBox;
    betAmount->setRange(1, 1000);
    QPushButton *spinBtn = new QPushButton("Крутить");
    QPushButton *clearBtn = new QPushButton("Очистить");

    QHBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->addWidget(betAmount);
    controlLayout->addWidget(spinBtn);
    controlLayout->addWidget(clearBtn);
    layout->addLayout(controlLayout);

    balanceLabel = new QLabel;
    resultLabel = new QLabel;
    layout->addWidget(balanceLabel);
    layout->addWidget(resultLabel);

    animation = new QPropertyAnimation(roulette, "angle");
    animation->setEasingCurve(QEasingCurve::OutQuint);
    connect(animation, &QPropertyAnimation::finished, this, &Roulette::animationFinished);

    updateDisplay();
}

void Roulette::setupConnections() {
    foreach(QPushButton *btn, findChildren<QPushButton*>()) {
        if(btn->property("betType").isValid())
            connect(btn, &QPushButton::clicked, this, &Roulette::placeBet);
    }
    connect(findChild<QPushButton*>("Крутить"), &QPushButton::clicked, this, &Roulette::spin);
    connect(findChild<QPushButton*>("Очистить"), &QPushButton::clicked, this, &Roulette::clearBets);
}

void Roulette::updateDisplay() {
    balanceLabel->setText(QString("Баланс: $%1\nТекущая ставка: $%2").arg(balance).arg(totalBet));
}
