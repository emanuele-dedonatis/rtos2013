
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
#define LCD_ROW         2
#define LCD_COL         8

void pedometerTask(void *argv) {
    Pedometer::instance().init();
    Pedometer::instance().start();
}

int main()
{
    Lcd44780 lcd(rs::getPin(), e::getPin(), d4::getPin(), d5::getPin(), d6::getPin(), d7::getPin(), LCD_ROW, LCD_COL);
    lcd.clear();
    lcd.go(3,0);
    lcd.printf("PEDO");
    lcd.go(3,1);
    lcd.printf("METER");
    Thread *pedometer_t;
    pedometer_t = Thread::create(pedometerTask, 2048, 1, NULL, Thread::JOINABLE);
    for(;;){
    }
}
