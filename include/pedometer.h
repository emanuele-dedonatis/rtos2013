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
    * @brief Get the instance of the pedometer, since it is a singleton
    * 
    * \return an instance of the pedometer (singleton)
    */
   static Pedometer& instance();    
   
   /**
    * @brief Initialize SPI and accelerometer sensor LIS302
    */
   void init(float usr_height, float usr_weights);
   
   /**
    * @brief Start the counting 
    */
   void start();
   
   /**
    * @brief Pause the counting
    */
   void pause();
   
   /**
    * @brief Restart the counting
    */
   void restart();
   
   /**
    * @brief Get the current number of steps
    * 
    * \return current number of steps
    */
   int getSteps();
  
   /**
    * @brief Get the current training mode (STEADY or WALK or RUN)
    * 
    * \return the current training mode 
    */
   int getMode();
   
   /**
    * @brief Ask if is in steady mode
    * 
    * \return true if is steady mode 
    */
   bool isSteady();
    
   /**
    * @brief Ask if is in walk mode
    * 
    * \return true if is walk mode 
    */   
   bool isWalk();
    
   /**
    * @brief Ask if is in run mode
    * 
    * \return true if is run mode 
    */   
   bool isRun();
   
   /**
    * @brief Set the user's height
    * 
    * \param m Height in meters
    */
   void setHeight(float m);
   
   /**
    * @brief Set the user's weight
    * 
    * \param kg Weight in kilos
    */
   void setWeight(float kg);
   
   /**
    * @brief Get the current distance in km
    * 
    * \return current distance [km]
    */
   float getDistance();
   
   /**
    * @brief Get the current speed in km/sec
    * 
    * \return current speed [km/sec]
    */
   float getSpeed();
   
   /**
    * @brief Get the current calories burned in KCal
    * 
    * \return current calories burned [Kcal]
    */
   int getCalories();
   
private:
    void newStep();

};

#endif	/* PEDOMETER_H */

