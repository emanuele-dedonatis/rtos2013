/**
* File:   pedometer.cpp
*  \brief     Pedometer functionalities using LIS302 accelerometer.
*  \author    Emanuele De Donatis
*  \author    Bruno Pistone
*  \version   1.0
*  \date      11/03/2014
*/

#include <stdio.h>
#include <ctime>
#include "pedometer.h"
#include "LIS302.h"
#include "miosix.h"


using namespace std;
using namespace miosix;

#define FILTER_SAMPLE           4
#define PRECISION               100
#define SAMPLE_PERIOD           20000
#define MAX_VALUE               255
#define MIN_INTERVAL            10
#define MAX_INTERVAL            100
#define MIN_TIMEWIN				200
#define MAX_TIMEWIN				2000
#define MIN_TIMEWALK			500

volatile signed char x[FILTER_SAMPLE], y[FILTER_SAMPLE], z[FILTER_SAMPLE];
volatile int x_max, x_min, y_max, y_min, z_max, z_min;
volatile int x_th, y_th, z_th;
volatile int x_new, x_old, y_new, y_old, z_new, z_old;
volatile int x_result, y_result, z_result;
volatile int x_accel, y_accel, z_accel;
volatile int counter;
volatile int steps;
volatile int x_th_abs, y_th_abs, z_th_abs;

long old_time, new_time, current_time;
bool active;
int mode;
float height, weight;
float length_run, length_walk , dist, speed, calories;

Pedometer& Pedometer::instance() {
    static Pedometer singleton;
    return singleton;
}

void Pedometer::init(float usr_height, float usr_weight) {
    LIS302::init();
    height = usr_height;
    weight = usr_weight;
    length_walk = height/6.0;  //length in m
    length_run = height/3.0;    //length in m
    restart();
}

void Pedometer::start() {
    active = true;
    for (;;) {
        if (active) {
            /*SUM FILTERING*/

            /*NOT USED A LOOP FOR STM STUDIO*/
            x[0] = LIS302::readX();
            y[0] = LIS302::readY();
            z[0] = LIS302::readZ();

            x[1] = LIS302::readX();
            y[1] = LIS302::readY();
            z[1] = LIS302::readZ();

            x[2] = LIS302::readX();
            y[2] = LIS302::readY();
            z[2] = LIS302::readZ();

            x[3] = LIS302::readX();
            y[3] = LIS302::readY();
            z[3] = LIS302::readZ();

            x_result = x[0] + x[1] + x[2] + x[3];
            y_result = y[0] + y[1] + y[2] + y[3];
            z_result = z[0] + z[1] + z[2] + z[3];
            /*END SUM FILTERING*/

            /*FIND MAX AND MIN VALUE*/
            if (x_result > x_max) x_max = x_result;
            if (x_result < x_min) x_min = x_result;

            if (y_result > y_max) y_max = y_result;
            if (y_result < y_min) y_min = y_result;

            if (z_result > z_max) z_max = z_result;
            if (z_result < z_min) z_min = z_result;
            /*END FIND MAX AND MIN VALUE*/

            counter++;

            if (counter > 50) {
                /*COMPUTE THRESHOLD*/
                counter = 0;
                x_th = (x_max + x_min) / 2;
                y_th = (y_max + y_min) / 2;
                z_th = (z_max + z_min) / 2;

                /*INIT VALUE*/
                x_max = -MAX_VALUE;
                x_min = MAX_VALUE;
                y_max = -MAX_VALUE;
                y_min = MAX_VALUE;
                z_max = -MAX_VALUE;
                z_min = MAX_VALUE;
                /*END INIT VALUE*/
            }

            x_old = x_new;
            y_old = y_new;
            z_old = z_new;
            /*DISCARD SMALL ACCEL*/
            if (abs(x_new - x_result) > PRECISION)
                x_new = x_result;
            if (abs(y_new - y_result) > PRECISION)
                y_new = y_result;
            if (abs(z_new - z_result) > PRECISION)
                z_new = z_result;



            /*FIND LARGEST ACCEL AXIS*/
            x_th_abs = abs(x_th);
            y_th_abs = abs(y_th);
            z_th_abs = abs(z_th);

            if (x_th_abs > y_th_abs) {
                /*x > y*/
                if (x_th_abs > z_th_abs) {

                    /*x is the largest*/
                    if (x_old > x_th && x_new < x_th)
                        newStep();

                } else {

                    /*z is the largest*/
                    if (z_old > z_th && z_new < z_th)
                        newStep();

                }
            } else {
                /*y > x*/
                if (y_th_abs > z_th_abs) {
                    /*y is the largest*/
                    if (y_old > y_th && y_new < y_th)
                        newStep();
                } else {
                    /*z is the largest*/
                    if (z_old > z_th && z_new < z_th)
                        newStep();
                }
            }
        }
        usleep(SAMPLE_PERIOD);
    }
}

int Pedometer::getSteps() {
    return steps;
}

void Pedometer::pause() {
    if (active)
        active = false;
    else
        active = true;
}

void Pedometer::restart() {
    mode = Pedometer::MODE_STEADY;
    steps = 0;
    counter = 0;
    dist = 0;
    speed = 0;
    x_max = -MAX_VALUE;
    x_min = MAX_VALUE;
    y_max = -MAX_VALUE;
    y_min = MAX_VALUE;
    z_max = -MAX_VALUE;
    z_min = MAX_VALUE;
}

void Pedometer::newStep() {
    old_time = new_time; //time in msec of last step
    new_time = getTick(); //time in msec of new step
    float time = new_time - old_time; //time in msec between last two steps

    if (time >= MIN_TIMEWIN && time <= MAX_TIMEWIN) { //human range for step
        steps++;
        if (time < MIN_TIMEWALK) {
            mode = Pedometer::MODE_RUN;
            dist += length_run;
            speed = length_run / time; //instant speed in m/msec
        } else {
            mode = Pedometer::MODE_WALK;
            dist += length_walk;
            speed = length_walk / time; //instant speed in m/msec
        }
    }
}

int Pedometer::getMode() {
    current_time = getTick();
    if (current_time - new_time > MAX_TIMEWIN) {
        mode = Pedometer::MODE_STEADY;
        speed = 0;
    }
    return mode;
}

void Pedometer::setHeight(float m) {
    height = m;
}

void Pedometer::setWeight(float kg) {
    weight = kg;
}

float Pedometer::getDistance() {
    return dist/1000.0; //from meters to kmeters
}

float Pedometer::getSpeed() {
    return speed*3600.0;        //from m/ms to km/h
}

int Pedometer::getCalories() {
    return (dist/1000.0) * weight;
}
