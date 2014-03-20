/* 
 * File:   pedometer.cpp
 * Author: user
 * 
 * Created on March 12, 2014, 1:12 AM
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
#define SAMPLE_PERIOD           20000   //      1/50Hz = 20ms
#define MAX_VALUE               255
#define MIN_INTERVAL            10
#define MAX_INTERVAL            100

volatile signed char x[FILTER_SAMPLE], y[FILTER_SAMPLE], z[FILTER_SAMPLE];
volatile int x_max, x_min, y_max, y_min, z_max, z_min;
volatile int x_th, y_th, z_th;
volatile int x_new, x_old, y_new, y_old, z_new, z_old;
volatile int x_result, y_result, z_result;
volatile int x_accel, y_accel, z_accel;
volatile int counter;
volatile int steps;
volatile int x_th_abs, y_th_abs, z_th_abs;
volatile long old_time, new_time, current_time;

bool active;
int mode;
float height, weight;
float dist, speed;

Pedometer& Pedometer::instance(){
    static Pedometer singleton;
    return singleton;
}

void Pedometer::init() {
    LIS302::init();
    restart();
}

void Pedometer::start() {
    active = true;
    for(;;){
        if(active){ 
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
                
                x_result = x[0]+x[1]+x[2]+x[3];
                y_result = y[0]+y[1]+y[2]+y[3];
                z_result = z[0]+z[1]+z[2]+z[3];
          /*END SUM FILTERING*/
                 
           /*FIND MAX AND MIN VALUE*/
                if(x_result>x_max) x_max = x_result;
                if(x_result<x_min) x_min = x_result;
                            
                if(y_result>y_max) y_max = y_result;
                if(y_result<y_min) y_min = y_result;
            
                if(z_result>z_max) z_max = z_result;
                if(z_result<z_min) z_min = z_result;
           /*END FIND MAX AND MIN VALUE*/

                counter++;
                
                if(counter > 50) {
                        /*COMPUTE THRESHOLD*/
                    counter = 0;
                    x_th = (x_max+x_min)/2;
                    y_th = (y_max+y_min)/2;
                    z_th = (z_max+z_min)/2;
                    
                    /*INIT VALUE*/
                    x_max=-MAX_VALUE;
                    x_min=MAX_VALUE;
                    y_max=-MAX_VALUE;
                    y_min=MAX_VALUE;
                    z_max=-MAX_VALUE;
                    z_min=MAX_VALUE;
                    /*END INIT VALUE*/
                }
                               
            x_old = x_new;
            y_old = y_new;
            z_old = z_new;
       /*DISCARD SMALL ACCEL*/
        if(abs(x_new-x_result) > PRECISION)
            x_new = x_result;        
        if(abs(y_new-y_result) > PRECISION)
            y_new = y_result;        
        if(abs(z_new-z_result) > PRECISION)
            z_new = z_result;        
        
        

            /*FIND LARGEST ACCEL AXIS*/
            x_th_abs = abs(x_th);
            y_th_abs = abs(y_th);
            z_th_abs = abs(z_th);
        
            if(x_th_abs > y_th_abs){
                /*x > y*/
                if(x_th_abs > z_th_abs){
                    
                    /*x is the largest*/
                   if(x_old > x_th && x_new < x_th) 
                        newStep(); 
                   
                }else{
                    
                    /*z is the largest*/
                    if(z_old > z_th && z_new < z_th)
                        newStep(); 
                    
                }
            }else{
               /*y > x*/
                if(y_th_abs > z_th_abs){
                    /*y is the largest*/
                    if(y_old > y_th && y_new < y_th)
                        newStep(); 
                }else{
                    /*z is the largest*/
                    if(z_old > z_th && z_new < z_th)
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
    if(active)
        active = false;
    else
        active = true;
}

void Pedometer::restart() {
    mode = Pedometer::MODE_STEADY;
    steps = 0;
    counter = 0;
    height = 1.80;
    weight = 0;
    dist = 0;
    speed = 0;
        x_max=-MAX_VALUE;
        x_min=MAX_VALUE;
        y_max=-MAX_VALUE;
        y_min=MAX_VALUE;
        z_max=-MAX_VALUE;
        z_min=MAX_VALUE;
}

void Pedometer::newStep() {
    steps++;
    old_time = new_time;
    new_time = getTick();
    int rhythm = 2000/(new_time-old_time);
    switch(rhythm) {
        case 1: mode = Pedometer::MODE_WALK;
                dist += height/5000;
                break;
        case 2: mode = Pedometer::MODE_WALK;
                dist += height/4000;
                break;
        case 3: mode = Pedometer::MODE_WALK;
                dist += height/3000;
                break;
        case 4: mode = Pedometer::MODE_RUN;
                dist += height/2000;
                break;
        case 5:;
        case 6:;
        case 7:;
        case 8:;
        case 9:;
        case 10: mode = Pedometer::MODE_RUN;
                dist += height/1000;
                break;
        default: ;
    }
}

int Pedometer::getMode() {
    current_time = getTick();
    if(current_time - new_time > 2000)
        mode = Pedometer::MODE_STEADY;
    return mode;
}

void Pedometer::setHeight(float cm) {
    height = cm;
}

void Pedometer::setWeight(float kg) {
    weight = kg;
}

float Pedometer::getDistance() {
    return dist;
}
