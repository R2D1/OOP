#ifndef STATISTIC_H
#define STATISTIC_H

#include "includes.h"

/*
 * Класс СТАТИСТИК
 * Используется для хранения среднего времени, через которое приезжает машина
*/

class Statistic : public QObject
{
    Q_OBJECT

public:

    Statistic(QObject *parent = 0);
    virtual ~Statistic();

    void setMeanCarArrivalTime(int value);                  // Метод задания значения meanCarArrivalTime.
                                                            // В value содержится значение времени для пересчета
    double getMeanCarArrivalTime();                         // Метод, возвращающий значение переменной meanCarArrivalTime
    void resetAll();                                        // Метод сброса всего

signals:

    void meanCarArrivalTimeChanged(double);                 // Сигнал, вызываемый при изменении значения meanCarArrivalTime

private:

    double meanCarArrivalTime;                              // Переменная для хранения среднего времени приезда машин
    int carCount;                                           // Хранит количество машин, учтенных при пересчете значения среднего времени
};

#endif // STATISTIC_H
