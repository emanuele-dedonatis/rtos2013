/* 
 * File:   spi.h
 * Author: user
 *
 * Created on March 13, 2014, 2:47 PM
 */

#ifndef SPI_H
#define	SPI_H


class SPI {

public: 
        static void init();
    
        static unsigned char send(unsigned char address, unsigned char data);
        
        static unsigned char read(unsigned char address);
};

#endif	/* SPI_H */

