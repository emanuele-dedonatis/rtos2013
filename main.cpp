
#include <cstdio>
#include "miosix.h"
#include "pedometer.h"

using namespace std;
using namespace miosix;

void pedometerTask(void *argv) {
    Pedometer::instance().init();
    Pedometer::instance().start();
}
int main()
{
    Thread *pedometer_t;
    pedometer_t = Thread::create(pedometerTask, 2048, 1, NULL, Thread::JOINABLE);
    for(;;){
        iprintf("Main\n");
        usleep(50000);
    }
    pedometer_t->join();
}
