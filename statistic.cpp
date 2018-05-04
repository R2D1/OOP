#include "statistic.h"

Statistic::Statistic(QObject *parent) : QObject(parent)
{
    meanCarArrivalTime = 0;
    carCount = 0;
}

Statistic::~Statistic() {

}

/* Метод пересчета значения среднего времени приезда */
void Statistic::setMeanCarArrivalTime(int value) {

    carCount++;
    meanCarArrivalTime = ((double)(carCount-1)*meanCarArrivalTime + (double)value)/(double)carCount;
    emit meanCarArrivalTimeChanged(meanCarArrivalTime);
}

/* Метод, возвращающий среднее время приезда машин */
double Statistic::getMeanCarArrivalTime() {

    return meanCarArrivalTime;
}

/* Метод сброса */
void Statistic::resetAll() {

    meanCarArrivalTime = 0.0;
    carCount = 0;
    emit meanCarArrivalTimeChanged(0.0);
}
