
#include <cstdio>
#include "miosix.h"
#include "pedometer.h"
#include "util/lcd44780.h"

using namespace std;
using namespace miosix;

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

typedef Gpio<GPIOE_BASE,7>  rs;         //pin4
typedef Gpio<GPIOE_BASE,8>  e;          //pin6
typedef Gpio<GPIOE_BASE,9>  d4;         //pin11
typedef Gpio<GPIOE_BASE,10>  d5;        //pin12
typedef Gpio<GPIOE_BASE,11>  d6;        //pin13
typedef Gpio<GPIOE_BASE,12>  d7;        //pin14

typedef Gpio<GPIOA_BASE,0>  button;        //board button

#define LCD_ROW         4
#define LCD_COL         20

Lcd44780 lcd(rs::getPin(), e::getPin(), d4::getPin(), d5::getPin(), d6::getPin(), d7::getPin(), LCD_ROW, LCD_COL);

void pedometerTask(void *argv) {
    Pedometer::instance().init();
    Pedometer::instance().start();
}

void initLcd() {
    lcd.clear();
    lcd.go(0,0);
    lcd.printf("S:");
    lcd.go(8,0);
    lcd.printf("M:");
    lcd.go(0,1);
    lcd.printf("V:");
    lcd.go(7,1);
    lcd.printf("K:");
    lcd.go(13,1);
    lcd.printf("C:");
}

int main()
{
    initLcd();
    Thread *pedometer_t;
    pedometer_t = Thread::create(pedometerTask, 2048, 1, NULL, Thread::JOINABLE);
    
    Pedometer& pedo = Pedometer::instance();
    for(;;){
        //STEPS
        lcd.go(3,0);
        lcd.printf("%d", pedo.getSteps());
        //MODE
        lcd.go(10,0);
        switch(Pedometer::instance().getMode()) {
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
        //SPEED
        lcd.go(2,1);
        lcd.printf("    ");
        lcd.go(2,1);
        lcd.printf("%.1f", pedo.getSpeed());
        //DISTANCE
        lcd.go(9,1);
        lcd.printf("%.1f", pedo.getDistance());
        //CALORIES
        lcd.go(15,1);
        lcd.printf("%.1f", pedo.getCalories());
        if(button::value())
             Pedometer::instance().restart();
        usleep(50000);
    }
}
