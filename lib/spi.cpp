/* 
 * File:   spi.cpp
 * Author: user
 * 
 * Created on March 13, 2014, 2:47 PM
 */

#include "spi.h"
#include "miosix.h"

using namespace miosix;

#define SPI_READ_MASK           0x80
#define SPI_DUMMY               0x00

typedef Gpio<GPIOA_BASE,5>  SPI_sclk;
typedef Gpio<GPIOA_BASE,7>  SPI_miso;
typedef Gpio<GPIOA_BASE,6>  SPI_mosi;
typedef Gpio<GPIOE_BASE,3>  SPI_CS;



        void SPI::init(){    
            RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOEEN;
            
            SPI1->CR1 =  SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;
            
            SPI_sclk::mode(Mode::ALTERNATE);
            SPI_sclk::alternateFunction(5);
            SPI_sclk::speed(Speed::_50MHz);
            
            SPI_miso::mode(Mode::ALTERNATE);
            SPI_miso::alternateFunction(5);
            SPI_miso::speed(Speed::_50MHz);
            
            SPI_mosi::mode(Mode::ALTERNATE);
            SPI_mosi::alternateFunction(5);
            SPI_mosi::speed(Speed::_50MHz);
            
            SPI_CS::mode(Mode::OUTPUT);
            SPI_CS::speed(Speed::_50MHz);
            SPI_CS::high();
            
            SPI1->CR1 |= SPI_CR1_SPE;
        }
    
        unsigned char SPI::send(unsigned char address, unsigned char data)
        {    
            volatile unsigned char dummy;
            SPI_CS::low();
            SPI1->DR = address;
            while((SPI1->SR & SPI_SR_TXE) == 0);
            while((SPI1->SR & SPI_SR_RXNE) == 0);
            dummy = SPI1->DR;

            SPI1->DR = data;
            while((SPI1->SR & SPI_SR_TXE) == 0);
            while((SPI1->SR & SPI_SR_RXNE) == 0);
            SPI_CS::high();
            return SPI1->DR;
        }
        
        unsigned char SPI::read(unsigned char address){
            return SPI::send(SPI_READ_MASK | address, SPI_DUMMY);
        }


