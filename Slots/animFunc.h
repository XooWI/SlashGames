#ifndef ANIMFUNC_H
#define ANIMFUNC_H
#include <math.h>
#include <qrandom.h>
#include <QDebug>
#include <QLabel>

//Класс для подсчета состояния виджета в следующем кадре
class Animator{
    // t - время с начала анимации (кол-во вызовов/60 кадров)
private:
    int loopsY=0;
    int loopsX=0;
    int aCalcY=0;
    int picSX=135;
    int picSY=1350;
    int x,y;
public:
    int slotLoops =rand()%3;
    //скорость в пикс/сек
    float t=0;
    float fps=60;
    float Vy;
    float Vx=0;
    QPoint startPos;

    void init(QWidget* obj){
        y=startPos.y();
        x=startPos.x();
    }
    void clear(){
        Vy=0;
        Vx=0;
    }
    void accelMoveY(QWidget* obj,float ay){

        if(!t){clear();}
        t=t+1/fps;
        Vy=Vy+ay/fps;
        y=y+Vy/fps;
        if (y>startPos.y()+picSY-3*picSX){
            y=startPos.y();
            loopsY=loopsY+1;
        }
        obj->move(obj->x(),y);

    }

    void slot(QWidget* obj,float ay, float val){
        if (t<1){accelMoveY(obj,ay);
        loopsY=0;}
        else{
            if(loopsY<slotLoops){
                accelMoveY(obj,0);
            }
            else{
                if(loopsY==slotLoops){
                    loopsY=loopsY+1;
                    int distance=0;
                    //Выравние дефекта цифры "7"
                    if(val==5){distance =picSX*7+ val*picSX;}
                    else{distance =picSX*7+ val*picSX +20 + (val*val*val)/12;}
                    //расчитываем ускорения при s=distance, t=1
                    aCalcY=-(Vy*Vy)/(2*distance);
                }
                if (Vy>0){accelMoveY(obj,aCalcY);}
            }
        }

    }

};

#endif // ANIMFUNC_H
