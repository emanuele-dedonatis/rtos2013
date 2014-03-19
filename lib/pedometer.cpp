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
        lcd.clear();
        x = LIS302::readX();
        y = LIS302::readY();
        z = LIS302::readZ();
        
        lcd.go(0,0);
        lcd.printf("%d", x);
        lcd.go(5,0);
        lcd.printf("%d", y);
        lcd.go(3,1);
        lcd.printf("%d", z);
        usleep(20000);
        }
}

