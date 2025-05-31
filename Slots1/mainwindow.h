#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QResizeEvent>
#include <QTimer>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setPos(int x, int y, QWidget* obj);
    int slotVal1,slotVal2,slotVal3;
private:
    QTimer *timer;
    Ui::MainWindow *ui;
private slots:
    void resizeEvent(QResizeEvent* event);
    void framePerSecond();
    void on_pushButton_clicked(bool checked);
    void on_pushButton_clicked();
    void on_pushButton_released();
    void on_horizontalSlider_actionTriggered(int action);
    void on_horizontalSlider_sliderMoved(int position);
};
#endif // MAINWINDOW_H
