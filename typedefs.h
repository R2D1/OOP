#ifndef TYPEDEFS_H
#define TYPEDEFS_H

/* Структура, в которой хранится вся необходимая информация о машине, стоящей в очереди на заправку */
struct carPlace {

    int fuelType;               // {0, 1}
    int fuelAmount;             // [liters]
    int refuelingSpeed;         // [liters/minute]
    int refuelingTime;          // [seconds]
};

/*
 * Перечисление, содержащее перечень возможных событий
 * carArrived - машина приехала на заправку и ожидает определения ее параметров
 * in_0_0 - машина заехала на позицию 0_0
 * in_0_1 - ... 0_1
 * in_1_0 - ... 1_0
 * in_1_1 - ... 1_1
 * out_2 - машина вышла из очереди на заправке
 * out_0_0 - машина освободила колонку 0_0
 * out_0_1 - ... 0_1
 * out_1_0 - ... 1_0
 * out_1_1 - ... 1_1
*/
enum Event{carArrived, in_0_0, in_0_1, in_1_0, in_1_1, in_2, out_2, out_0_0, out_0_1, out_1_0, out_1_1/*, NO*/};

typedef struct carPlace carPlace;             // Задаем новые типы для удобства
typedef enum Event Event;

#endif // TYPEDEFS_H
