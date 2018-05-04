#include "driver.h"

/* Конструктор класса ДВИЖОК. Обнуляем все переменные */
Driver::Driver(QObject *parent) : QObject(parent) {

    timerState = false;
    elapsedTime = 0;
    T = M = Z = Q1 = Q2 = V = 0;
    P = 0.0;
    srand(time(NULL));
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(on_timer_timeout()));
}

Driver::~Driver() {

}

/* Метод возврата значения таймера. "true" если таймер активен, "false" если таймер остановлен */
bool Driver::getTimerState() {

    return timerState;
}

/* Метод задания состояния таймера */
void Driver::setTimerState(bool state) {

    timerState = state;
    if(timerState) {
        timer.start(1000/M);
    } else {
        timer.stop();
    }
}

/* Метод задания переменной T */
void Driver::setT(int value) {

    T = value;
}

/* Метод задания переменной M */
void Driver::setM(int value) {

    M = value;
}

/* Метод задания переменной Z */
void Driver::setZ(int value) {

    Z = value;
}

/* Метод задания переменной P */
void Driver::setP(double value) {

    P = value;
}

/* Метод задания переменных Q1 и Q2 */
void Driver::setQ(int value_1, int value_2) {

    Q1 = value_1;
    Q2 = value_2;
}

/* Метод задания переменной V */
void Driver::setV(int value) {

    V = value;
}

/* Старт работы движка */
void Driver::startDriver() {

    if(!timerState) {   // Костыль для обработки программного даблтапа

        int time = exponentialRand(Z);          // Задаем время приезда первой машины
        mainQueue.insert(time, carArrived);     // Кладем это событие в очередь
        timer.start(1000/M);                    // Запускаем таймер с таймаутом 1/M секунд
        timerState = true;                      // Обновляем состояние таймера
    }
}

/* Экспоненциальное распределение
 * Возвращает количество секунд, через которое произойдет событие
*/
int Driver::exponentialRand(int Z) {

    int ret = (int)((-1)*log(1 - (double)rand()/(double)RAND_MAX)/((double)Z/60.0));
    if(ret != 0) { // Обеспечивает отсутствие нулевых значений
        return ret;
    } else {
        exponentialRand(Z);
    }
}

/* Равномерное распределение
 * Возвращает целочисленное значение в диапазоне [Q1; Q2]
*/
int Driver::uniformRand(int Q1, int Q2) {

    return (Q1 + rand()%(Q2-Q1));
}

/* Таймаут таймера */
void Driver::on_timer_timeout() {

    elapsedTime++;
    if(elapsedTime == T) {
        timerState = false;
    }
    emit updateProgressBar(elapsedTime);

    if(mainQueue.contains(elapsedTime)) {               // Если в очереди есть событие на этот момент

        Event ev = mainQueue.value(elapsedTime);
        QMap<int, Event>::iterator it = mainQueue.find(elapsedTime);
        mainQueue.erase(it);
        emit pendingEvent(ev);                          // Послать сигнал о событии
    }
}

/* Метод занесения в очередь нового события (используется другими классами) */
void Driver::pushNewEvent(int time, Event event) {

    mainQueue.insert(time, event);
}

/* Метод, возвращающий прошедшее время от момента начала симуляции */
int Driver::getElapsedTime() {

    return elapsedTime;
}

/* Метод проверки задания всех переменных */
bool Driver::checkParameters() {

    if(T > 0 && M > 0 && Z > 0 && P > 0.0 && Q1 > 0 && Q2 > 0 && V > 0) {

        return true;
    } else {

        return false;
    }
}

/* Метод сброса данных */
void Driver::resetAll() {

    timer.stop();
    timerState = false;
    mainQueue.clear();
    elapsedTime = 0;
    T = M = Z = Q1 = Q2 = V = 0;
    P = 0.0;
}
