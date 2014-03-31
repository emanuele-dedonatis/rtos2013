/**
* File:   spi.h
*  \brief     SPI library on STM32F4
*  \author    Emanuele De Donatis
*  \author    Bruno Pistone
*  \version   1.0
*  \date      13/03/2014
*/

#ifndef SPI_H
#define	SPI_H

/**
* Interface class for SPI
*/
class SPI {

public: 
    /**
     * Initialize the SPI peripheral
     */
        static void init();
    /**
     * Send data to SPI
     * 
     * @param address Destination address
     * @param data Data to send
     * @return Incoming data from SPI
     */
        static unsigned char send(unsigned char address, unsigned char data);
        
        /**
         * Recive data from SPI
         * 
         * @param address Source address 
         * @return Incoming data from SPI
         */
        static unsigned char read(unsigned char address);
};

#endif	/* SPI_H */

