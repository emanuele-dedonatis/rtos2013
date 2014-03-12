/* 
 * File:   pedometer.cpp
 * Author: user
 * 
 * Created on March 12, 2014, 1:12 AM
 */

#include "pedometer.h"

Pedometer& Pedometer::instance(){
    static Pedometer singleton;
    return singleton;
}

