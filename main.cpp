
#include <cstdio>
#include "miosix.h"
#include "pedometer.h"
#include "gui.h"
#include "audio/slice-and-play.h"
#include "nrf24l01p/wifi_module.h"

using namespace std;
using namespace miosix;

/*  BUTTON GPIO
 * Gpio<GPIOE_BASE,13>  btn_UP;
 * Gpio<GPIOE_BASE,14>  btn_OK;
 * Gpio<GPIOE_BASE,15> btn_DOWN;
 * 
 *  LCD GPIO
 * Gpio<GPIOE_BASE,7>   rs;        //pin4
 * Gpio<GPIOE_BASE,8>    e;        //pin6
 * Gpio<GPIOE_BASE,9>   d4;        //pin11
 * Gpio<GPIOE_BASE,10>  d5;        //pin12
 * Gpio<GPIOE_BASE,11>  d6;        //pin13
 * Gpio<GPIOE_BASE,12>  d7;        //pin14
 *  
 * WIFI GPIO
 * Gpio<GPIOB_BASE,11> CE;
 * Gpio<GPIOB_BASE,12> CS;
 * Gpio<GPIOB_BASE,13> SCK;
 * Gpio<GPIOB_BASE,14> MISO;
 * Gpio<GPIOB_BASE,15> MOSI;
 * Gpio<GPIOA_BASE,1> IRQ;
 */

#define SOUND_DELAY     30
#define LCD_REFRESH     50000

void pedometerTask(void *argv) {
    Pedometer::instance().init(GUI::getHeight(), GUI::getWeight());
    Pedometer::instance().start();
}

void audio_and_wifiTask(void *argv) {
    char stepsCod[32];
    for(;;){
        sleep(SOUND_DELAY);  
        int steps = Pedometer::instance().getSteps();
        if(steps!=0) {
                ring::instance().play_n_of_step(steps,100);
                sprintf(stepsCod, "%d", steps);
                send(stepsCod);
        }
    }
}

int main()
{
    GUI::init();
    
    wifi_init();
    
    Thread *pedometer_t;
    pedometer_t = Thread::create(pedometerTask, 2048, 1, NULL, Thread::JOINABLE);
    
    Thread *audio_t;
    audio_t = Thread::create(audio_and_wifiTask, 2048, 1, NULL, Thread::JOINABLE);
        
    Pedometer& pedo = Pedometer::instance();
    for(;;){
        //STEPS
        GUI::steps(pedo.getSteps());
        //MODE
        GUI::mode(pedo.getMode());
        //SPEED
        GUI::speed(pedo.getSpeed());
        //DISTANCE
        GUI::distance(pedo.getDistance());
        //CALORIES
        GUI::calories(pedo.getCalories());
        usleep(LCD_REFRESH);
    }
}
