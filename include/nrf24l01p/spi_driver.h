/**
 * File:   spi_driver.h
 *  \brief     This is the header file for the class spi_driver.
 *  \author    Andrea Corna
 *  \author    Lorenzo Fontana
 *  \version   1.0
 *  \date      03/01/2014
 */

#ifndef SPI_DRIVER_H
#define	SPI_DRIVER_H

class spi_driver {
public:
    /**
     * Default construct
     */
    spi_driver();
    spi_driver(const spi_driver& orig);
    
    /**
     * Destructor
     */
    virtual ~spi_driver();
    
    /**
     * Method to write a command with the spi 
     * @param command - command's integer coding
     */
    void write(int command);
    
    /**
     * This method uses a dummy write in order to catch the slave's status register
     * @return status register integer coding
     */
    int read();
private:

};

#endif	/* SPI_DRIVER_H */

