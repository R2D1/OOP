#ifndef DRIVER_H
#define DRIVER_H

#include "includes.h"

/*
 * Класс ДВИЖОК
 * Управляет работой программы, взаимодействуя с объектом класса МОДЕЛЬ
 * Управляет очередью событий, организует работу таймера
*/

class Driver : public QObject
{
    Q_OBJECT

public:

    Driver(QObject *parent = 0);                // Конструктор класса
    virtual ~Driver();

    bool getTimerState();                       // Метод, возвращающий состояние таймера
    int getElapsedTime();                       // Метод, возвращающий значение таймера
    bool checkParameters();                     // Метод, возвращающий "true", если все параметры заданы
                                                // и отличны от нуля, и "false" иначе

    void setTimerState(bool state);             // Метод задания состояния таймера
    void setT(int value);                       // ...
    void setM(int value);                       // ...
    void setZ(int value);                       // ...
    void setP(double value);                    // Методы задания переменных
    void setQ(int value_1, int value_2);        // ...
    void setV(int value);                       // ...

    void startDriver();                         // Метод запуска движка в начале работы программы
    int exponentialRand(int Z);                 // Экспоненциальное распределение
    int uniformRand(int Q1, int Q2);            // Равномерное распределение
    void pushNewEvent(int time, Event event);   // Метод занесения в очередь нового события
    void resetAll();                            // Метод сброса

public slots:

    void on_timer_timeout();                    // Слот таймаута таймера

signals:

    void pendingEvent(Event);                   // Сигнал, посылаемый при обнаружении события в очереди
    void updateProgressBar(int);                // Сигнал, посылаемый таймером для обновления progressBar

private:

    QMap<int, Event> mainQueue;                 // Очередь событий
    QTimer timer;                               // Таймер
    bool timerState;                            // Состояние таймера
    int elapsedTime;                            // Время от начала симуляции

    int T;                                      // [minutes]
    int M;                                      // Scale 1 : M
    int Z;                                      // Flow [cars/minute]
    double P;                                   // Probability [0;1]
    int Q1;                                     // [liters]
    int Q2;                                     // [liters]
    int V;                                      // [liters/minute]
};

#endif // DRIVER_H
