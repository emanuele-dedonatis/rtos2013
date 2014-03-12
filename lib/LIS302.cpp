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
#include "util/software_spi.h"

using namespace miosix;

typedef Gpio<GPIOA_BASE,5>  SPI_sclk;
typedef Gpio<GPIOA_BASE,7>  SPI_miso;
typedef Gpio<GPIOA_BASE,6>  SPI_mosi;
typedef Gpio<GPIOE_BASE,3>  SPI_CS;

typedef SoftwareSPI<SPI_miso, SPI_mosi, SPI_sclk, SPI_CS, 0>;

LIS302::init() {
}

unsigned char LIS302::readX() {
}

unsigned char LIS302::readY() {
}

unsigned char LIS302::readZ() {
}

