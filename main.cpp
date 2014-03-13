
#include <cstdio>
#include "miosix.h"
#include "LIS302.h"

using namespace std;
using namespace miosix;

volatile int x, y, z;

int main()
{
    LIS302::init();
    
    for(;;) {
        x = LIS302::readX();
        y = LIS302::readY();
        z = LIS302::readZ();
        
        iprintf("%d %d %d \n", x, y, y);
        
        usleep(20000);
    }
}
