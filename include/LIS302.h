/**
* File:   pedometer.h
*  \brief     LIS302 accelerometer
*  \author    Emanuele De Donatis
*  \author    Bruno Pistone
*  \version   1.0
*  \date      12/03/2014
*/

#ifndef LIS302_H
#define	LIS302_H

/**
* Interface class for LIS302
*/
class LIS302 {
public:
    /**
     * Initialize the sensor
     * \return true if initialization ok
     */
    static void init();
    
    /**
     * Read value from x-axis
     * 
     * \return value of x-axis
     */
    static signed char readX();

    /**
     * Read value from y-axis
     * 
     * \return value of y-axis
     */    
    static signed char readY();
      
    /**
     * Read value from z-axis
     * 
     * \return value of z-axis
     */  
    static signed char readZ();
private:

};

#endif	/* LIS302_H */

