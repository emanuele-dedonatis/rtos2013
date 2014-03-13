/**
* File:   pedometer.h
*  \brief     This is the header file for the file LIS302.cpp
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
     * \return value of x-axis
     */
    static signed char readX();

    /**
     * \return value of y-axis
     */    
    static signed char readY();
      
    /**
     * \return value of z-axis
     */  
    static signed char readZ();
private:

};

#endif	/* LIS302_H */

