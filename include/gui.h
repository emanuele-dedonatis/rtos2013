/* 
 * File:   gui.h
*  \brief     pedometer GUI
*  \author    Emanuele De Donatis
*  \author    Bruno Pistone
 */

#ifndef GUI_H
#define	GUI_H

/*    LCD44680 PINOUT
 * pin1:        GND
 * pin2:        5V
 * pin3:        Vee
 * pin4:        RS
 * pin5:        RW (WRITE=GND)
 * pin6:        E
 * pin7-14:     DB0-DB7
 * pin15:       LED+
 * pin16:       LED-
 */

/*
 *   |0 |1 |2 |3 |4 |5 |6 |7 |8 |9 |10|11|12|13|14|15|16|17|18|19| 
 * 0 |S |T |E |P |S |: |  |* |* |* |* |  |  |  |  |  |  |  |  |  |
 * 1 |M |O |D |E |: |  |* |* |* |* |* |* |  |  |  |  |  |  |  |  |
 * 
 *   |20|21|22|23|24|25|26|27|28|29|30|31|32|33|34|35|36|37|38|39|
 * 0 |S |P |E |E |D |: |  |* |* |. |* |  |K |m |/ |h |  |  |  |  |
 * 1 |K |M |: |  |* |* |. |* |* |  |K |C |A |L |: |  |* |* |* |* |
 */

/**
* Interface class for GUI
*/
class GUI {

public: 
    /**
     * Initialize GUI
     */
    static void init();
    
    /**
     * Print number of steps
     */
     static void steps(int steps);
 
    /**
     * Print mode
     */
     static void mode(int mode);
 
    /**
     * Print speed
     */
     static void speed(float speed);
 
    /**
     * Print distance
     */
     static void distance(float distance);
 
    /**
     * Print calories
     */
     static void calories(int calories);
     
     /**
      * Get user height
      */
     static float getHeight();
     
     /**
      * Get user weight
      */
     static float getWeight();
};

#endif	/* GUI_H */

