
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

/*
 *   |0 |1 |2 |3 |4 |5 |6 |7 |8 |9 |10|11|12|13|14|15|16|17|18|19| 
 * 0 |S |T |E |P |S |: |  |* |* |* |* |  |  |  |  |  |  |  |  |  |
 * 1 |M |O |D |E |: |  |* |* |* |* |* |* |  |  |  |  |  |  |  |  |
 * 
 *   |20|21|22|23|24|25|26|27|28|29|30|31|32|33|34|35|36|37|38|39|
 * 0 |S |P |E |E |D |: |  |* |* |. |* |  |K |m |/ |h |  |  |  |  |
 * 1 |K |M |: |  |* |* |. |* |  |K |C |A |L |: |  |* |* |* |* |  |
 */

typedef Gpio<GPIOE_BASE,7>  rs;         //pin4
typedef Gpio<GPIOE_BASE,8>  e;          //pin6
typedef Gpio<GPIOE_BASE,9>  d4;         //pin11
typedef Gpio<GPIOE_BASE,10>  d5;        //pin12
typedef Gpio<GPIOE_BASE,11>  d6;        //pin13
typedef Gpio<GPIOE_BASE,12>  d7;        //pin14

typedef Gpio<GPIOA_BASE,0>  button;        //board button

#define LCD_ROW         4
#define LCD_COL         40

Lcd44780 lcd(rs::getPin(), e::getPin(), d4::getPin(), d5::getPin(), d6::getPin(), d7::getPin(), LCD_ROW, LCD_COL);

void pedometerTask(void *argv) {
    Pedometer::instance().init();
    Pedometer::instance().start();
}

void initLcd() {
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
    lcd.go(29,1);
    lcd.printf("KCAL:");
}

int main()
{
    initLcd();
    Thread *pedometer_t;
    pedometer_t = Thread::create(pedometerTask, 2048, 1, NULL, Thread::JOINABLE);
    
    Pedometer& pedo = Pedometer::instance();
    for(;;){
        //STEPS
        lcd.go(7,0);
        lcd.printf("%d", pedo.getSteps());
        //MODE
        lcd.go(6,1);
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
        lcd.go(27,0);
        lcd.printf("    ");
        lcd.go(27,0);
        lcd.printf("%.1f", pedo.getSpeed());
        //DISTANCE
        lcd.go(24,1);
        lcd.printf("%.1f", pedo.getDistance());
        //CALORIES
        lcd.go(35,1);
        lcd.printf("%.1f", pedo.getCalories());
        if(button::value())
             Pedometer::instance().restart();
        usleep(50000);
    }
}
