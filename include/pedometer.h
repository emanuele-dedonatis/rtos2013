/**
* File:   pedometer.h
*  \brief     Pedometer functionalities using LIS302 accelerometer.
*  \author    Emanuele De Donatis
*  \author    Bruno Pistone
*  \version   1.0
*  \date      11/03/2014
*/

#ifndef PEDOMETER_H
#define	PEDOMETER_H

/**
* Interface class for pedometer
*/
class Pedometer {
public:

   /**
    * @brief Steady mode contant used by getMode
   */
   static const int MODE_STEADY = 1;
   /**
    * @brief Walk mode contant used by getMode
   */   
   static const int MODE_WALK = 2;
   /**
    * @brief Run mode contant used by getMode
   */   
   static const int MODE_RUN = 3;
   
   /**
    * \return an instance of the pedometer (singleton)
    */
   static Pedometer& instance();    
   
   /**
    * Initialize SPI and accelerometer sensor LIS302
    */
   void init();
   
   /**
    * Start the counting 
    */
   void start();
   
   /**
    * Pause the counting
    */
   void pause();
   
   /**
    * Restart the counting
    */
   void restart();
   
   /**
    * \return current number of steps
    */
   int getSteps();
  
   /**
    * \return the current training mode 
    */
   int getMode();
   
   /**
    * \return true if is steady mode 
    */
   bool isSteady();
    
   /**
    * \return true if is walk mode 
    */   
   bool isWalk();
    
   /**
    * \return true if is run mode 
    */   
   bool isRun();
   
   /**
    * Set the user's height
    * 
    * \param m Height in meters
    */
   void setHeight(float m);
   
   /**
    * Set the user's weight
    * 
    * \param kg Weight in kilos
    */
   void setWeight(float kg);
   
   /**
    * \return current distance [km]
    */
   float getDistance();
   
   /**
    * \return current speed [km/sec]
    */
   float getSpeed();
   
   /**
    * \return current calories burned 
    */
   float getCalories();
   
private:
    void newStep();

};

#endif	/* PEDOMETER_H */

