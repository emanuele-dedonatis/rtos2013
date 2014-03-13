/**
* File:   pedometer.h
*  \brief     This is the header file for the file LIS302.cpp
*  \author    Emanuele De Donatis
*  \author    Bruno Pistone
*  \version   1.0
*  \date      12/03/2014
*/

#include "miosix.h"
#include "LIS302.h"
#include "spi.h"

#define LIS302_Ctrl_Reg1        0x20
#define LIS302_PD               0x40
#define LIS302_Zen              0x04
#define LIS302_Yen              0x02
#define LIS302_Xen              0x01
#define LIS302_OutX             0x29
#define LIS302_OutY             0x2B
#define LIS302_OutZ             0x2D

void LIS302::init() {
    SPI::init();
    SPI::send(LIS302_Ctrl_Reg1, LIS302_PD | LIS302_Xen | LIS302_Yen | LIS302_Zen);
}

     signed char LIS302::readX() {
        return SPI::read(LIS302_OutX);
    }
    
     signed char LIS302::readY() {
        return SPI::read(LIS302_OutY);
    }
    
     signed char LIS302::readZ() {
        return SPI::read(LIS302_OutZ);
    }

