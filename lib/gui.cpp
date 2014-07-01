/* 
 * File:   gui.cpp
*  \brief     pedometer GUI
*  \author    Emanuele De Donatis
*  \author    Bruno Pistone
 */

#include "gui.h"
#include "miosix.h"
#include "util/lcd44780.h"
#include "pedometer.h"

using namespace miosix;

typedef Gpio<GPIOE_BASE,7>  rs;         //pin4
typedef Gpio<GPIOE_BASE,8>  e;          //pin6
typedef Gpio<GPIOE_BASE,9>  d4;         //pin11
typedef Gpio<GPIOE_BASE,10>  d5;        //pin12
typedef Gpio<GPIOE_BASE,11>  d6;        //pin13
typedef Gpio<GPIOE_BASE,12>  d7;        //pin14

typedef Gpio<GPIOE_BASE,13>  btn_UP;
typedef Gpio<GPIOE_BASE,14>  btn_OK;
typedef Gpio<GPIOE_BASE,15> btn_DOWN;

#define LCD_ROW         4
#define LCD_COL         40

Lcd44780 lcd(rs::getPin(), e::getPin(), d4::getPin(), d5::getPin(), d6::getPin(), d7::getPin(), LCD_ROW, LCD_COL);

float usr_height = 1.75;
float usr_weight = 85;

void GUI::init() {
    
    //INITIAL SCREENSHOT
    lcd.clear();
    lcd.go(2,1);
    lcd.printf("PERSONAL TRAINER");
    lcd.go(26,0);
    lcd.printf("RTOS2013");
    sleep(5);
    
    btn_UP::mode(Mode::INPUT_PULL_UP);
    btn_DOWN::mode(Mode::INPUT_PULL_UP);
    btn_OK::mode(Mode::INPUT_PULL_UP);
    
    //HEIGHT INPUT
    lcd.clear();
    lcd.go(0,0);
    lcd.printf("< - >  < OK >  < + >");
    lcd.go(27,0);
    lcd.printf("HEIGHT");
    bool height_saved = false;
    while(!height_saved){
           if(btn_UP::value()==0) usr_height = usr_height + 0.01;
           if(btn_DOWN::value()==0) usr_height = usr_height - 0.01;
           if(btn_OK::value()==0) height_saved = true;
           lcd.go(27,1);
           lcd.printf("%.2f m ", usr_height);
           Thread::sleep(100);
    }
    
	lcd.clear();
	Thread::sleep(100);
    lcd.go(9,1);
    lcd.printf("OK");
               lcd.go(27,0);
           lcd.printf("%.2f m ", usr_height);
    Thread::sleep(1000);
	lcd.clear();
	Thread::sleep(100);
	
	//WEIGHT INPUT
    lcd.clear();
    lcd.go(0,0);
    lcd.printf("< - >  < OK >  < + >");
    lcd.go(27,0);
    lcd.printf("WEIGHT");
    bool weight_saved = false;
    while(!weight_saved){
           if(btn_UP::value()==0) usr_weight = usr_weight + 1;
           if(btn_DOWN::value()==0) usr_weight = usr_weight - 1;
           if(btn_OK::value()==0) weight_saved = true;
           lcd.go(27,1);
           lcd.printf("%.0f kg ", usr_weight);
           Thread::sleep(100);
    }
    
	lcd.clear();
	Thread::sleep(100);
    lcd.go(9,1);
    lcd.printf("OK");
           lcd.go(27,0);
           lcd.printf("%.0f kg ", usr_weight);
    Thread::sleep(1000);
	lcd.clear();
	Thread::sleep(100);
	
        //TEMPLATE
            lcd.clear();
    lcd.go(0,0);
    lcd.printf("STEPS:");
    lcd.go(0,1);
    lcd.printf("MODE:");
    lcd.go(20,0);
    lcd.printf("SPEED:");
    lcd.go(32,0);
    lcd.printf("Km/h");
    lcd.go(20,1);
    lcd.printf("KM:");
    lcd.go(30,1);
    lcd.printf("KCAL:");  
    return;  
}

void GUI::steps(int steps){
        lcd.go(7,0);
        lcd.printf("%d", steps);    
}

void GUI::mode(int mode){
        lcd.go(6,1);
        switch(mode) {
            case Pedometer::MODE_STEADY:
                lcd.printf("STEADY");
                break;
            case Pedometer::MODE_WALK:
                lcd.printf(" WALK ");
                break;            
            case Pedometer::MODE_RUN:
                lcd.printf(" RUN  ");
                break; 
            default: lcd.printf("      ");
        }
}

void GUI::speed(float speed){
        lcd.go(27,0);
        lcd.printf("    ");
        lcd.go(27,0);
        lcd.printf("%.1f ", speed);
}

void GUI::distance(float distance){
        lcd.go(24,1);
        lcd.printf("%.2f", distance);
}

void GUI::calories(int calories){
        lcd.go(36,1);
        lcd.printf("%d", calories);
}

float GUI::getHeight(){
    return usr_height;
}

float GUI::getWeight(){
    return usr_weight;
}


