/* 
 * File:   pedometer.cpp
 * Author: user
 * 
 * Created on March 12, 2014, 1:12 AM
 */

#include <stdio.h>
#include "pedometer.h"
#include "LIS302.h"
#include "miosix.h"

using namespace std;
using namespace miosix;

int step;
signed char x, y, z;

Pedometer& Pedometer::instance(){
    static Pedometer singleton;
    return singleton;
}

void Pedometer::init() {
    LIS302::init();
    step = 0;
}

void Pedometer::start() {
    for(;;){
        x = LIS302::readX();
        y = LIS302::readY();
        z = LIS302::readZ();
        
        iprintf("%d %d %d \n", x, y, y);
        
        usleep(20000);
        }
}

