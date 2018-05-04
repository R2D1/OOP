#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "includes.h"
#include "model.h"
#include "driver.h"
#include "statistic.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);               // Конструктор
    ~MainWindow();

public slots:

    void redrawWidget();                                    // Слот, вызываемый при изменении состояния
                                                            // колонки или очереди к ней

protected:

    virtual void mousePressEvent(QMouseEvent *event);       // Контроль нажатия мыши
    virtual void mouseReleaseEvent();                       // Контроль отпускания кнопки мыши
    virtual void mouseMoveEvent(QMouseEvent *event);        // Контроль перемещения мыши

private slots:

    void on_startSimulationButton_clicked();                // Нажатие клавиши начала симуляции
    void on_pauseSimulationButton_clicked();                // Нажатие клавиши приостановки симуляции
    void on_stopSimulationButton_clicked();                 // Нажатие клавиши остановки симуляции
    void on_generateRandomUniformButton_clicked();          // Нажатие клавиши теста рандома нормального распределения
    void on_generateRandomExponentialButton_clicked();      // Нажатие клавиши теста рандома экспоненциального распределения
    void handlePendingEvents(Event event);                  // Обработка поступающих событий
    void updateProgressBar(int time);                       // Обновление статуса прогресс бара
    void updateMeanCarArrivalTime(double value);            // Обновление среднего времени приезда машин

private:

    Ui::MainWindow *ui;
    QLabel *exitLabel;
    QPixmap carFront;
    QPixmap carSide;
    QPixmap gasStation;
    bool mousePressed;
    int xPressCoord;
    int yPressCoord;
    int progressTime_0_0;
    int diff_0_0;
    int progressTime_0_1;
    int diff_0_1;
    int progressTime_1_0;
    int diff_1_0;
    int progressTime_1_1;
    int diff_1_1;

    Model model;                                            // Объект класса "МОДЕЛЬ"
    Driver driver;                                          // Объект класса "ДВИЖОК"
    Statistic statistic;                                    // Объект класса "СТАТИСТИК"
};

#endif // MAINWINDOW_H
