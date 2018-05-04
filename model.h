#ifndef MODEL_H
#define MODEL_H

#include "includes.h"
//#include "driver.h"
//#include "statistic.h"
//#include <QVector>

/*
 * Класс МОДЕЛЬ
 * Содержит в себе состояния объектов информационной модели
 * Объектами выступают:
 *    1-я бензоколонка, содержащая 2 места
 *    2-я бензоколонка, содержащая 2 места
 *    Очередь, содержащая 3 места
*/

class Model : public QObject
{
    Q_OBJECT

public:

    Model(QObject *parent = 0);
    virtual ~Model();

    void setGasState(int station, int place, bool state);   // Метод задания состояния бензоколонки
    void setQueueState(QVector<carPlace> queue);            // Метод задания состояния очереди на колонки

    bool getGasState(int station, int place);               // Метод получения состояния бензоколонки
    QVector<carPlace> getQueueState();                      // Метод, возвращающий очередь
    void resetAll();                                        // Метод сброса всего


signals:

    void gasStateChanged();                                 // Сигнал, посылаемый при изменении состояния бензоколонки
    void queueStateChanged();                               // Сигнал, посылаемый при изменении состояния очереди на бензоколонку

private:

    bool gas[2][2];                                         // Бензоколонка
    QVector<carPlace> queue;                                // Очередь на бензоколонку
};

#endif // MODEL_H
