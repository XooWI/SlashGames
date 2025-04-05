#ifndef ROULETTE_H
#define ROULETTE_H

#include <QtWidgets>
#include <QPropertyAnimation>
#include <QRandomGenerator>

class RouletteWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal angle READ angle WRITE setAngle)
public:
    RouletteWidget(QWidget *parent = nullptr);
    qreal angle() const;
    void setAngle(qreal angle);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    qreal m_angle;
};

class Roulette : public QWidget {
    Q_OBJECT

public:
    Roulette(QWidget *parent = nullptr);

private slots:
    void placeBet();
    void spin();
    void animationFinished();
    void clearBets();

private:
    void setupUI();
    void setupConnections();
    void updateDisplay();

    RouletteWidget *roulette;
    QSpinBox *betAmount;
    QLabel *balanceLabel, *resultLabel;
    QPropertyAnimation *animation;

    int balance;
    int totalBet;
    QMap<QString, int> bets;
};

#endif // ROULETTE_H
