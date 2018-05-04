#include "mainwindow.h"
#include "model.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint);
    setFixedWidth(800);
    setFixedHeight(440);
    this->setStyleSheet("background-color: #F8F8FF");
    qApp->setStyleSheet("QProgressBar::chunk:horizontal{ background: green; }");
    this->show();
    mousePressed = false;
    exitLabel = new QLabel;
    QCursor *pointer = new QCursor(Qt::PointingHandCursor);
    ui->exitLabel->setCursor(*pointer);

    progressTime_0_0 = progressTime_0_1 = progressTime_1_0 = progressTime_1_1 = 0;
    diff_0_0 = diff_0_1 = diff_1_0 = diff_1_1 = 0;

    carFront.load(":src/images/car_front.ico");
    carSide.load(":src/images/car_side.ico");
    gasStation.load(":src/images/gas_station.png");
    ui->gas_0->setPixmap(gasStation);
    ui->gas_1->setPixmap(gasStation);


    QObject::connect(&model, SIGNAL(gasStateChanged()), this, SLOT(redrawWidget()));
    QObject::connect(&model, SIGNAL(queueStateChanged()), this, SLOT(redrawWidget()));
    QObject::connect(&driver, SIGNAL(pendingEvent(Event)), this, SLOT(handlePendingEvents(Event)));
    QObject::connect(&driver, SIGNAL(updateProgressBar(int)), this, SLOT(updateProgressBar(int)));
    QObject::connect(&statistic, SIGNAL(meanCarArrivalTimeChanged(double)), this, SLOT(updateMeanCarArrivalTime(double)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event) {

    mousePressed = true;
    if(event->button() == Qt::LeftButton && event->x() > (this->width() - 30) && event->y() < 30) {

        qApp->exit();
    } else {

        xPressCoord = event->x();
        yPressCoord = event->y();
    }
}

void MainWindow::mouseReleaseEvent() {

    mousePressed = false;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {

    if(mousePressed && event->y() < 30) {

        this->move(event->globalX() - xPressCoord, event->globalY() - yPressCoord);
    }
}

/* Отрисовка событий */
void MainWindow::redrawWidget() {

    (model.getGasState(0, 0)) ? (ui->place_0_0->setPixmap(carFront)) : (ui->place_0_0->clear());
    (model.getGasState(0, 1)) ? (ui->place_0_1->setPixmap(carFront)) : (ui->place_0_1->clear());
    (model.getGasState(1, 0)) ? (ui->place_1_0->setPixmap(carFront)) : (ui->place_1_0->clear());
    (model.getGasState(1, 1)) ? (ui->place_1_1->setPixmap(carFront)) : (ui->place_1_1->clear());

    QVector<carPlace> tmp = model.getQueueState();
    switch (tmp.size()) {

    case 1:
        ui->place_2_0->setPixmap(carSide);
        ui->place_2_1->clear();
        ui->place_2_2->clear();
        break;

    case 2:
        ui->place_2_0->setPixmap(carSide);
        ui->place_2_1->setPixmap(carSide);
        ui->place_2_2->clear();
        break;

    case 3:
        ui->place_2_0->setPixmap(carSide);
        ui->place_2_1->setPixmap(carSide);
        ui->place_2_2->setPixmap(carSide);
        break;

    default:
        ui->place_2_0->clear();
        ui->place_2_1->clear();
        ui->place_2_2->clear();
        break;
    }
}

/* Нажата кнопка начала симуляции */
void MainWindow::on_startSimulationButton_clicked() {

    // Считываем все переменные в объект класса ДВИЖОК
    int tmp = ui->value_T->text().toInt();
    driver.setT(tmp*60);
    driver.setM(ui->value_M->text().toInt());
    driver.setZ(ui->value_Z->text().toInt());
    driver.setP(ui->value_P->text().toDouble());
    driver.setQ(ui->value_Q1->text().toInt(), ui->value_Q2->text().toInt());
    driver.setV(ui->value_V->text().toInt());

    if(!driver.checkParameters()) {

        QMessageBox box;
        box.setText("Warning! Some parameters are invalid.\nCheck if all the fields are filled and\nvalues are greater than 0.");
        box.exec();
        return;
    }
    ui->simulationProgress->setRange(0, ui->value_V->text().toInt()*60);

    // Запускаем движок
    driver.startDriver();
}

/* Нажата кнопка приостановки симуляции */
void MainWindow::on_pauseSimulationButton_clicked() {

    if(driver.getTimerState()) {

       ui->pauseSimulationButton->setText("Resume simulation");
       driver.setTimerState(false);
    } else {

        ui->pauseSimulationButton->setText("Pause simulation");
        driver.setTimerState(true);
    }
}

/* Нажата кнопка окончания симуляции */
void MainWindow::on_stopSimulationButton_clicked() {

    progressTime_0_0 = progressTime_0_1 = progressTime_1_0 = progressTime_1_1 = 0;
    diff_0_0 = diff_0_1 = diff_1_0 = diff_1_1 = 0;
    driver.setTimerState(false);

    driver.resetAll();
    model.resetAll();
    statistic.resetAll();

    ui->simulationProgress->setValue(0);
    ui->simulationProgress->update();
    ui->progress_0_0->setValue(0);
    ui->progress_0_0->update();
    ui->progress_0_1->setValue(0);
    ui->progress_0_1->update();
    ui->progress_1_0->setValue(0);
    ui->progress_1_0->update();
    ui->progress_1_1->setValue(0);
    ui->progress_1_1->update();
}

/* Обработка поступившего события */
void MainWindow::handlePendingEvents(Event event) {

    switch(event) {

    case(carArrived): {

        /* Определяем fuelType */
        int fuelType = rand()%2;

        /* Определяем Q */
        int fuelAmount = driver.uniformRand(ui->value_Q1->text().toInt(), ui->value_Q2->text().toInt());

        /* Определяем V */
        int refuelingSpeed = ui->value_V->text().toInt();
        if(refuelingSpeed != 1) {

            int s = rand()%3 - 1;       // {-1, 0, 1}
            refuelingSpeed += s;
        }

        /* Определяем время требуемое на заправку В СЕКУНДАХ */
        int refuelingTime = (int)((double)fuelAmount*60.0/(double)refuelingSpeed);

        /* Если есть свободная колонка занимаем ее */
        if(!model.getGasState(fuelType, 0)) {

            /* Вызываем обработчик будто поступил сигнал in_M_N */
            if(fuelType == 0) {

                handlePendingEvents(in_0_0);
                progressTime_0_0 = driver.getElapsedTime() + refuelingTime;
                diff_0_0 = refuelingTime;
            } else if (fuelType == 1) {

                handlePendingEvents(in_1_0);
                progressTime_1_0 = driver.getElapsedTime() + refuelingTime;
                diff_1_0 = refuelingTime;
            }

            Event ev;
            (fuelType == 0) ? (ev = out_0_0) : (ev = out_1_0);

            /* Отправляем в очередь событие освобождения колонки через найденное время */
            driver.pushNewEvent(driver.getElapsedTime() + refuelingTime, ev);

        } else if(!model.getGasState(fuelType, 1)) {

            /* Вызываем обработчик будто поступил сигнал in_M_N */
            if(fuelType == 0) {

                handlePendingEvents(in_0_1);
                progressTime_0_1 = driver.getElapsedTime() + refuelingTime;
                diff_0_1 = refuelingTime;
            } else if (fuelType == 1) {

                handlePendingEvents(in_1_1);
                progressTime_1_1 = driver.getElapsedTime() + refuelingTime;
                diff_1_1 = refuelingTime;
            }

            Event ev;
            (fuelType == 0) ? (ev = out_0_1) : (ev = out_1_1);

            /* Отправляем в очередь событие освобождения колонки через найденное время*/
            driver.pushNewEvent(driver.getElapsedTime() + refuelingTime, ev);

        } else {

            /* Свободной колонки не найдено. Проверим, есть ли место в очереди
             * Если есть, поставим туда машину. Если нет, то ничего не будем делать
            */
            QVector<carPlace> tmp = model.getQueueState();
            if(tmp.size() < 3) {    // Если в очереди есть хотя бы одно свободное место

                carPlace a;
                a.fuelType = fuelType;
                a.fuelAmount = fuelAmount;
                a.refuelingSpeed = refuelingSpeed;
                a.refuelingTime = refuelingTime;
                tmp.push_back(a);
                model.setQueueState(tmp);   // Заносим машину в очередь. Автоматическая перерисовка
            }
        }

        /* Задаем время приезда следующей машины В СЕКУНДАХ */
        int nextCarTime = driver.getElapsedTime() + driver.exponentialRand(ui->value_Z->text().toInt());

        /* Отправляем приезд новой машины в очередь, если не закончилось время */
        if(driver.getTimerState()) {

            driver.pushNewEvent(nextCarTime, carArrived);
            statistic.setMeanCarArrivalTime(nextCarTime - driver.getElapsedTime());
        }
        break;
    }

    case(in_0_0): {

        model.setGasState(0, 0, true);
        break;
    }

    case(in_0_1): {

        model.setGasState(0, 1, true);
        break;
    }

    case(in_1_0): {

        model.setGasState(1, 0, true);
        break;
    }

    case(in_1_1): {

        model.setGasState(1, 1, true);
        break;
    }

    case(out_0_0): {

        model.setGasState(0, 0, false);                 // Освободили колонку
        ui->progress_0_0->setValue(0);
        ui->progress_0_0->update();

        /* Проверим, нет ли в очереди подходящих машин */
        QVector<carPlace> tmp = model.getQueueState();  // Получили состояние очереди

        if(!tmp.empty()) {                              // Если очередь не пуста

            for(int i = 0; i < tmp.size(); i++) {       // Проходимся по всем ее элементам

                if(tmp.at(i).fuelType == 0) {           // Если встретили подходящую машину

                    //handlePendingEvents(in_0_0);        // Заняли для нее колонку

                    /* Отправили в очередь событие освобождения этой колонки через известное время */
                    driver.pushNewEvent(driver.getElapsedTime() + tmp.at(i).refuelingTime, out_0_0);
                    progressTime_0_0 = driver.getElapsedTime() + tmp.at(i).refuelingTime;
                    diff_0_0 = tmp.at(i).refuelingTime;

                    tmp.erase(tmp.begin() + i);         // Извлекли из очереди машину
                    model.setQueueState(tmp);           // Обновили состояние очереди
                    handlePendingEvents(in_0_0);        // Заняли для нее колонку
                    break;                              // Выходим из цикла просмотра очереди
                }
            }
        }
        break;
    }

    case(out_0_1): {

        model.setGasState(0, 1, false);                 // Освободили колонку
        ui->progress_0_1->setValue(0);
        ui->progress_0_1->update();

        /* Проверим, нет ли в очереди подходящих машин */
        QVector<carPlace> tmp = model.getQueueState();  // Получили состояние очереди

        if(!tmp.empty()) {                              // Если очередь не пуста

            for(int i = 0; i < tmp.size(); i++) {       // Проходимся по всем ее элементам

                if(tmp.at(i).fuelType == 0) {           // Если встретили подходящую машину

                    //handlePendingEvents(in_0_1);        // Заняли для нее колонку

                    /* Отправили в очередь событие освобождения этой колонки через известное время */
                    driver.pushNewEvent(driver.getElapsedTime() + tmp.at(i).refuelingTime, out_0_1);
                    progressTime_0_1 = driver.getElapsedTime() + tmp.at(i).refuelingTime;
                    diff_0_1 = tmp.at(i).refuelingTime;

                    tmp.erase(tmp.begin() + i);         // Извлекли из очереди машину
                    model.setQueueState(tmp);           // Обновили состояние очереди
                    handlePendingEvents(in_0_1);        // Заняли для нее колонку
                    break;                              // Выходим из цикла просмотра очереди
                }
            }
        }
        break;
    }

    case(out_1_0): {

        model.setGasState(1, 0, false);                 // Освободили колонку
        ui->progress_1_0->setValue(0);
        ui->progress_1_0->update();

        /* Проверим, нет ли в очереди подходящих машин */
        QVector<carPlace> tmp = model.getQueueState();  // Получили состояние очереди

        if(!tmp.empty()) {                              // Если очередь не пуста

            for(int i = 0; i < tmp.size(); i++) {       // Проходимся по всем ее элементам

                if(tmp.at(i).fuelType == 1) {           // Если встретили подходящую машину

                    //handlePendingEvents(in_1_0);        // Заняли для нее колонку

                    /* Отправили в очередь событие освобождения этой колонки через известное время */
                    driver.pushNewEvent(driver.getElapsedTime() + tmp.at(i).refuelingTime, out_1_0);
                    progressTime_1_0 = driver.getElapsedTime() + tmp.at(i).refuelingTime;
                    diff_1_0 = tmp.at(i).refuelingTime;

                    tmp.erase(tmp.begin() + i);         // Извлекли из очереди машину
                    model.setQueueState(tmp);           // Обновили состояние очереди
                    handlePendingEvents(in_1_0);        // Заняли для нее колонку
                    break;                              // Выходим из цикла просмотра очереди
                }
            }
        }
        break;
    }

    case(out_1_1): {

        model.setGasState(1, 1, false);                 // Освободили колонку
        ui->progress_1_1->setValue(0);
        ui->progress_1_1->update();

        /* Проверим, нет ли в очереди подходящих машин */
        QVector<carPlace> tmp = model.getQueueState();  // Получили состояние очереди

        if(!tmp.empty()) {                              // Если очередь не пуста

            for(int i = 0; i < tmp.size(); i++) {       // Проходимся по всем ее элементам

                if(tmp.at(i).fuelType == 1) {           // Если встретили подходящую машину

                    //handlePendingEvents(in_1_1);        // Заняли для нее колонку

                    /* Отправили в очередь событие освобождения этой колонки через известное время */
                    driver.pushNewEvent(driver.getElapsedTime() + tmp.at(i).refuelingTime, out_1_1);
                    progressTime_1_1 = driver.getElapsedTime() + tmp.at(i).refuelingTime;
                    diff_1_1 = tmp.at(i).refuelingTime;

                    tmp.erase(tmp.begin() + i);         // Извлекли из очереди машину
                    model.setQueueState(tmp);           // Обновили состояние очереди
                    handlePendingEvents(in_1_1);        // Заняли для нее колонку
                    break;                              // Выходим из цикла просмотра очереди
                }
            }
        }
        break;
    }

    case(in_2): {

        /*
         * Фактически не востребована, потому что в обработчиках других событий уже осуществляются
         * все необходимые изменеия очереди на заправку
        */
        break;
    }

    case(out_2): {

        /*
         * Фактически не востребована, потому что в обработчиках других событий уже осуществляются
         * все необходимые изменеия очереди на заправку
        */
        break;
    }
    }
}

/* Обновление progressBar */
void MainWindow::updateProgressBar(int time) {

    ui->simulationProgress->setValue(time);
    ui->simulationProgress->update();

    if(model.getGasState(0, 0)) {

        ui->progress_0_0->setValue((int)(100.0 - 100.0*(double)(progressTime_0_0 - time)/(double)diff_0_0));
    }

    if(model.getGasState(0, 1)) {

        ui->progress_0_1->setValue((int)(100.0 - 100.0*(double)(progressTime_0_1 - time)/(double)diff_0_1));
    }

    if(model.getGasState(1, 0)) {

        ui->progress_1_0->setValue((int)(100.0 - 100.0*(double)(progressTime_1_0 - time)/(double)diff_1_0));
    }

    if(model.getGasState(1, 1)) {

        ui->progress_1_1->setValue((int)(100.0 - 100.0*(double)(progressTime_1_1 - time)/(double)diff_1_1));

    }
}

/* Обновление среднего времени приезда машин */
void MainWindow::updateMeanCarArrivalTime(double value) {

    ui->meanCarArrivalTimeLabel->setText(QString::number(value));
}

/* Генерация случайной выборки из равномерного распределения */
void MainWindow::on_generateRandomUniformButton_clicked() {

    if(driver.getElapsedTime() == 0 && ui->value_Q1->text() != "" && ui->value_Q2->text() != "") {      // Гарантирует что ничего не собьется

        int len = ui->uniformRandomSampleLength->text().toInt();
        int arr[len];
        double mean = 0, std = 0;
        for(int i = 0; i < len; i++) {

            arr[i] = driver.uniformRand(ui->value_Q1->text().toInt(), ui->value_Q2->text().toInt());
            mean += arr[i];
        }
        mean /= len;
        for(int i = 0; i < len; i++) {

            std += pow((arr[i] - mean), 2);
        }
        std /= len;
        std = sqrt(std);

        ui->expectationUniformLabel->setText(QString::number(mean));
        ui->stdUniformLabel->setText(QString::number(std));
    } else {
        QMessageBox box;
        box.setText("Error!\nSpecify Q1 and Q2 first.");
        box.exec();
    }
}

/* Генерация случайной выборки из экспоненциального распределения */
void MainWindow::on_generateRandomExponentialButton_clicked() {

    if(driver.getElapsedTime() == 0 && ui->value_Z->text() != "") {      // Гарантирует, что ничего не собьется

        int len = ui->exponentialRandomSampleLength->text().toInt();
        int arr[len];
        double mean = 0, std = 0;
        for(int i = 0; i < len; i++) {

            arr[i] = driver.exponentialRand(ui->value_Z->text().toInt());
            mean += arr[i];
        }
        mean /= len;
        for(int i = 0; i < len; i++) {

            std += pow((arr[i] - mean), 2);
        }
        std /= len;
        std = sqrt(std);

        ui->expectationExponentialLabel->setText(QString::number(mean));
        ui->stdExponentialLabel->setText(QString::number(std));
    } else {

        QMessageBox box;
        box.setText("Error!\nSpecify Z first.");
        box.exec();
    }
}
