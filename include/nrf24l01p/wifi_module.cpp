#include <cstdio>
#include "miosix.h"
#include "spi_driver.h"
#include "nRF24L01P.h"
#include <miosix/kernel/scheduler/scheduler.h>
#include "pedometer.h"

#define BUFFER_TRANSMIT_SIZE            32
#define BUFFER_CELL_SIZE                32
#define BUFFER_NUMBER_CELLS             3
#define BUFFER_RECEIVE_SIZE             960

using namespace std;
using namespace miosix;

/*Variable for context switch*/
static Thread *waiting=0;

/*Buffers and counters*/
static char buffer_transmit[BUFFER_TRANSMIT_SIZE];
static char buffer_receive[BUFFER_RECEIVE_SIZE];
static int counter_tx = 0;
static int counter_rx = 0;

/*Wireless Module*/
static nRF24L01P *wifi;

/*Thread variables*/
static pthread_t receive_thread;
static pthread_t transmit_thread;

/*Buffers and module mutex*/
static pthread_mutex_t buff_tx=PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t buff_rx=PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t spi=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond=PTHREAD_COND_INITIALIZER;

/*Leds Gpio*/
typedef Gpio<GPIOD_BASE,12> greenLed;
typedef Gpio<GPIOD_BASE,14> redLed;
typedef Gpio<GPIOD_BASE,15> blueLed;
typedef Gpio<GPIOD_BASE,13> orangeLed;


void send(char* payload){
    int i=0;
    pthread_mutex_lock(&buff_tx);
    if(counter_tx == BUFFER_TRANSMIT_SIZE){
        counter_tx=0;
    }
    for(;i<BUFFER_CELL_SIZE-1;i++){
        buffer_transmit[i+counter_tx] = payload[i];
    }
    buffer_transmit[i+counter_tx]='\0';
    counter_tx = counter_tx + BUFFER_CELL_SIZE;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&buff_tx);
}


void __attribute__((naked)) EXTI1_IRQHandler(){
    saveContext();
    asm volatile("bl _Z16EXTI1HandlerImplv");
    restoreContext();
}

void __attribute__((used))EXTI1HandlerImpl(){
    EXTI->PR=EXTI_PR_PR1;
    if(waiting==0) return;
    waiting->IRQwakeup(); 
    if(waiting->IRQgetPriority()>Thread::IRQgetCurrentThread()->IRQgetPriority())
        Scheduler::IRQfindNextThread();
    waiting=0;
}

void waitForModule(){
    FastInterruptDisableLock dLock; 
    waiting=Thread::IRQgetCurrentThread();
    while(waiting)
    {
        Thread::IRQwait(); 
        FastInterruptEnableLock eLock(dLock); 
        Thread::yield(); 
    }
}

/**
 * setup the interrupt
 */
void configureModuleInterrupt()
{
    SYSCFG->EXTICR[1] = SYSCFG_EXTICR1_EXTI1_PA;
    /*device which handle external interrupt, it's for GPIO 1*/
    EXTI->IMR |= EXTI_IMR_MR1; 
    
    EXTI->RTSR &= ~EXTI_RTSR_TR1; 
    EXTI->FTSR |= EXTI_FTSR_TR1;
    NVIC_EnableIRQ(EXTI1_IRQn);
    /*Configure low priority*/
    NVIC_SetPriority(EXTI1_IRQn,15); 
    
}


void *wifi_receive(void *arg){
    char data[BUFFER_CELL_SIZE];
    int steps;
    for(;;){
       orangeLed::high();
       waitForModule();
       pthread_mutex_lock(&spi);
       wifi->set_receive_mode();
       while(wifi->packet_in_pipe0()){
                 orangeLed::low();
                 wifi->reset_interrupt();
                 wifi->receive(data,BUFFER_CELL_SIZE);
                 
                 printf("<RECEIVE> %s\n",data);
                 steps= atoi(data);
                 if(steps != 0){
                     Pedometer::instance().compareSteps(steps);
                     printf("Chiamo il pedometro perchè ho ricevuto %d passi\n",steps);
                 }pthread_mutex_lock(&buff_rx);
                 if(counter_rx<BUFFER_RECEIVE_SIZE){
                        for(int i=0;i<BUFFER_CELL_SIZE;i++){
                                buffer_receive[i+counter_rx] = data[i];
                        }
                        counter_rx += BUFFER_CELL_SIZE;
                 }
                 else{
                     printf("Receive buffer is full\n");
                 }
                 pthread_mutex_unlock(&buff_rx);
                 redLed::low();
                 
       }
       pthread_mutex_unlock(&spi);
              
    }
    
}  

void *wifi_transmit(void *arg){
     char payload[BUFFER_CELL_SIZE];
     payload[0]='h';
     payload[1]='\0';
    for(;;){
        greenLed::high();
        pthread_mutex_lock(&buff_tx);
        while(counter_tx == 0)
          pthread_cond_wait(&cond,&buff_tx);
        for(int j=0;j<counter_tx/BUFFER_CELL_SIZE;j++){
           for(int i = 0;i< BUFFER_CELL_SIZE;i++){
               payload[i]=buffer_transmit[i+BUFFER_CELL_SIZE*j];
                      
            }
            printf("<TRASMIT> %s\n",payload);
            pthread_mutex_lock(&spi);
            wifi->transmit(BUFFER_CELL_SIZE,payload);
            pthread_mutex_unlock(&spi);
            greenLed::low();
            usleep(400000);
            greenLed::high();
        }
        counter_tx = 0;
        pthread_mutex_unlock(&buff_tx); 
        usleep(20000000);
   }
        
}      



void receive(char *payload){
    pthread_mutex_lock(&buff_rx);
    if(counter_rx == 0){
        pthread_mutex_unlock(&buff_rx);
        return;
    }
    for(int i=0;i<counter_rx/BUFFER_CELL_SIZE;i++){
        for(int j=0;j<BUFFER_CELL_SIZE;j++){
            payload[j] = buffer_receive[j+BUFFER_CELL_SIZE*i];
            buffer_receive[j+BUFFER_CELL_SIZE*i] = 0;
        }
    }
    counter_rx=0;
    pthread_mutex_unlock(&buff_rx);
}


void wifi_init(){
    orangeLed::mode(Mode::OUTPUT);
    redLed::mode(Mode::OUTPUT);
    blueLed::mode(Mode::OUTPUT);
    greenLed::mode(Mode::OUTPUT);
    wifi = new nRF24L01P();
    wifi->power_up();
    wifi->set_receive_mode();
    configureModuleInterrupt();
    blueLed::high();
    pthread_create(&transmit_thread,NULL,&wifi_transmit,NULL);
    pthread_create(&receive_thread,NULL,&wifi_receive,NULL);

}