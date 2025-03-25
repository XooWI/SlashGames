#ifndef ROULETTE_H
#define ROULETTE_H

#include <QWidget>

namespace Ui {
class roulette;
}

class roulette : public QWidget
{
    Q_OBJECT

public:
    explicit roulette(QWidget *parent = nullptr);
    ~roulette();

private:
    Ui::roulette *ui;
};

#endif // ROULETTE_H
