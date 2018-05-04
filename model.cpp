#include "model.h"

/* Конструктор класса
 * Изначально все колонки и очередь на заправку свободны, поэтому инициализируем все как false
*/
Model::Model(QObject *parent) : QObject(parent)
{
    gas[0][0] = gas[0][1] = gas[1][0] = gas[1][1] = false;
}

Model::~Model() {

}

/* Метод задания состояния бензоколонки */
void Model::setGasState(int station, int place, bool state) {

    gas[station][place] = state;
    emit gasStateChanged();
}

/* Метод задания состояния очереди на бензоколонки */
void Model::setQueueState(QVector<carPlace> queue) {

    this->queue = queue;
    emit queueStateChanged();
}

/* Метод получения текущего состояния бензоколонки
 * Возвращает "true", если место занято, "false", если место свободно
*/
bool Model::getGasState(int station, int place) {

    return gas[station][place];
}

/* Метод получения текущего состояния очереди на заправке */
QVector<carPlace> Model::getQueueState() {

    return queue;
}

/* Метод сброса */
void Model::resetAll() {

    gas[0][0] = gas[0][1] = gas[1][0] = gas[1][1] = false;
    queue.clear();
    emit gasStateChanged();
    emit queueStateChanged();
}
