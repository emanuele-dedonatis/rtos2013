/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010 by Terraneo Federico                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   As a special exception, if other files instantiate templates or use   *
 *   macros or inline functions from this file, or you compile this file   *
 *   and link it with other works to produce a work based on this file,    *
 *   this file does not by itself cause the resulting work to be covered   *
 *   by the GNU General Public License. However the source code for this   *
 *   file must still be made available in accordance with the GNU General  *
 *   Public License. This exception does not invalidate any other reasons  *
 *   why a work based on this file might be covered by the GNU General     *
 *   Public License.                                                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

/************************************************************************
* Part of the Miosix Embedded OS. This is the testsuite used during Miosix
* development to check for regressions.
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <algorithm>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>

#include "miosix.h"
#include "kernel/priority_queue.h"
#include "config/miosix_settings.h"
#include "interfaces/console.h"
#include "board_settings.h"

using namespace miosix;

//Functions common to all tests
static void test_name(const char *name);
static void pass();
static void fail(const char *cause);
//Kernel est functions
static void test_1();
static void test_2();
static void test_3();
static void test_4();
static void test_5();
static void test_6();
static void test_7();
static void test_8();
static void test_9();
static void test_10();
static void test_11();
static void test_12();
static void test_13();
static void test_14();
static void test_15();
static void test_16();
//Filesystem test functions
#ifdef WITH_FILESYSTEM
static void fs_test_1();
static void fs_test_2();
static void fs_test_3();
#endif //WITH_FILESYSTEM
//Benchmark functions
static void benchmark_1();
static void benchmark_2();
static void benchmark_3();

//main(), calls all tests
int main()
{
    srand(0);
    //The priority of the test thread must be 0
    Thread::setPriority(0);
    for(;;)
    {
        iprintf("Type 't' for kernel test, 'f' for filesystem test or 'b' for"
        " benchmarks or 's' for shutdown\n");
        char c;
        for(;;)
        {
            c=getchar();
            if(c!='\n') break;
        }
        switch(c)
        {
            case 't':
                //for(;;){ //Testing
                ledOn();
                test_1();
                test_2();
                test_3();
                test_4();
                test_5();
                test_6();
                test_7();
                test_8();
                test_9();
                test_10();
                test_11();
                test_12();
                test_13();
                test_14();
                test_15();
                test_16();
                
                ledOff();
                Thread::sleep(500);//Ensure all threads are deleted.
                iprintf("\n*** All tests were successful\n\n");
                //} //Testing
                break;
            case 'f':
                ledOn();
                #ifdef WITH_FILESYSTEM
                fs_test_1();
                fs_test_2();
                fs_test_3();
                #else //WITH_FILESYSTEM
                iprintf("Error, filesystem support is disabled\n");
                #endif //WITH_FILESYSTEM
                ledOff();
                Thread::sleep(500);//Ensure all threads are deleted.
                iprintf("\n*** All tests were successful\n\n");
                break;
            case 'b':
                ledOn();
                benchmark_1();
                benchmark_2();
                benchmark_3();

                ledOff();
                Thread::sleep(500);//Ensure all threads are deleted.
                break;
            case 's':
                iprintf("Shutting down\n");
                while(!Console::txComplete()) ;
                shutdown();
            default:
                iprintf("Unrecognized option\n");
        }
    }
}

/**
 * Called @ the beginning of a test
 * \param name test name
 */
static void test_name(const char *name)
{
    iprintf("Testing %s...\n",name);
}

/**
 * Called @ the end of a successful test
 */
static void pass()
{
    iprintf("Ok.\n");
}

/**
 * Called if a test fails
 * \param cause cause of the failure
 */
static void fail(const char *cause)
{
    //Can't use iprintf here because fail() may be used in threads
    //with 256 bytes of stack, and iprintf may cause stack overflow
    Console::write("Failed:\r\n");
    Console::write(cause);
    Console::write("\r\n");
    while(!Console::txComplete()) /*wait*/;
    reboot();
}

//
// Test 1
//
/*
tests:
Thread::create
Thread::yield
Thread::terminate
Thread::testTerminate
Thread::exists
Thread::getStackBottom
also test thread termination by exiting the entry point
and test argv pointer passing
*/

static volatile bool t1_v1;

static void t1_p1(void *argv)
{
    for(;;)
    {
        if(Thread::testTerminate()) break;
        t1_v1=true;
    }
}

static void t1_p2(void *argv)
{
    for(;;)
    {
        if(Thread::testTerminate()) break;
        t1_v1=true;
    }
    //Leave entry point = terminate
}

static void t1_p3(void *argv)
{
    if(reinterpret_cast<unsigned int>(argv)!=0xdeadbeef) fail("argv passing");
}

static void t1_f1(Thread *p)
{
    // This test is very strict and sometimes might fail. To see why consider
    // this: yield() moves control flow to the second thread, but a preemption
    // happens immediately before it has the chance to set t1_v1 to true.
    // In this case t1_v1 will remain false and the test will fail.
    // The Thread::sleep(5) is added to make this possibility as unlikely as
    //possible.

    //If the thread exists, it should modify t1_v1, and exist() must return true
    for(int i=0;i<10;i++) //testing 10 times
    {
        Thread::sleep(5);
        t1_v1=false;
        Thread::yield();//run t1_p1
        if(t1_v1==false) fail("thread not created");
        if(Thread::exists(p)==false) fail("Thread::exists (1)");
    }
    p->terminate();
    Thread::yield();//Give time to the second thread to terminate
    //If the thread doesn't exist, it can't modify t1_v1, and exist() must
    //return false
    for(int i=0;i<10;i++) //testing 10 times
    {
        t1_v1=false;
        Thread::yield();//run t1_p1
        Thread::yield();//Double yield to be extra sure.
        if(t1_v1==true) fail("thread not deleted");
        if(Thread::exists(p)==true) fail("Thread::exists (2)");
    }
}

static void test_1()
{
    test_name("thread creation/deletion");
    //Testing getStackBottom()
    //const int *y=Thread::getStackBottom();
    //if(*y!=(int)STACK_FILL) fail("getStackBottom() (1)");
    //y--;//Now should point to last word of watermark
    //if(*y!=(int)WATERMARK_FILL) fail("getStackBottom() (2)");
    //Testing thread termination
    Thread *p;
    p=Thread::create(t1_p1,STACK_MIN,0,NULL);
    t1_f1(p);
    //Testing thread termination by leaving entry point
    p=Thread::create(t1_p2,STACK_MIN,0,NULL);
    t1_f1(p);
    //Testing argv passing
    p=Thread::create(t1_p3,STACK_MIN,0,reinterpret_cast<void*>(0xdeadbeef));
    Thread::sleep(5);
    if(Thread::exists(p)) fail("thread not deleted (2)");
    pass();
}

//
// Test 2
//
/*
Tests:
pauseKernel
restartKernel
Thread::getCurrentThread
*/

static volatile bool t2_v1;
static Thread *t2_p_v1;

static void t2_p1(void *argv)
{
    for(;;)
    {
        if(Thread::testTerminate()) break;
        if(Thread::getCurrentThread()!=t2_p_v1)
            fail("Thread::getCurrentThread()");
        t2_v1=true;
    }
}

static void test_2()
{
    test_name("pause/restart kernel");
    t2_p_v1=Thread::create(t2_p1,STACK_MIN,0,NULL);
    pauseKernel();//
    t2_v1=false;
    //If the kernel is stopped, t2_v1 must not be updated
    for(int i=0;i<10;i++)
    {
        delayMs(20);
        if(t2_v1==true) { restartKernel(); fail("pauseKernel"); }
    }
    restartKernel();//
    //If the kernel is started, t2_v1 must be updated
    for(int i=0;i<10;i++)
    {
        t2_v1=false;
        delayMs(20);
        if(t2_v1==false) fail("restartKernel");
    }
    t2_p_v1->terminate();
    pass();
}

//
// Test 3
//
/*
tests:
Thread::sleep()
Thread::sleepUntil()
getTick()
also tests creation of multiple instances of the same thread
*/

static void t3_p1(void *argv)
{
    const int SLEEP_TIME=100;
    for(;;)
    {
        if(Thread::testTerminate()) break;
        //Test that Thread::sleep sleeps the desired number of ticks
        long long x=getTick();
        Thread::sleep(SLEEP_TIME);
        if(abs(((SLEEP_TIME*TICK_FREQ)/1000)-(getTick()-x))>5)
            fail("Thread::sleep() or getTick()");
    }
}

static volatile bool t3_v2;//Set to true by t3_p2
static volatile bool t3_deleted;//Set when an instance of t3_p2 is deleted

static void t3_p2(void *argv)
{
    const int SLEEP_TIME=15;
    for(;;)
    {
        t3_v2=true;
        Thread::sleep(SLEEP_TIME);
        if(Thread::testTerminate())
        {
            t3_deleted=true;
            break;
        }
    }
}

static void test_3()
{
    test_name("tick and sleep");
    Thread *p=Thread::create(t3_p1,STACK_MIN,0,NULL);
    for(int i=0;i<4;i++)
    {
        //The other thread is running tick() test
        Thread::sleep((101*1000)/TICK_FREQ);
    }
    p->terminate();
    Thread::sleep(200); //make sure the other thread does terminate*/
    //Testing Thread::sleep() again
    t3_v2=false;
    p=Thread::create(t3_p2,STACK_MIN,0,NULL);
    //t3_p2 sleeps for 15ms, then sets t3_v2. We sleep for 20ms so t3_v2 should
    //be true
    Thread::sleep(20);
    if(t3_v2==false) fail("Thread::sleep() (2)");
    t3_v2=false;
    //t3_p2 is sleeping for other 15ms and will set t3_v2 @ t=30ms
    //but we check variable @ t=25ms, so it should be false
    Thread::sleep(5);
    if(t3_v2==true) fail("Thread::sleep() (3)");
    //Giving time to t3_p2 to set t3_v2
    Thread::sleep(10);
    if(t3_v2==false) fail("Thread::sleep() (2)");
    t3_v2=false;
    //Creating another instance of t3_p2 to check what happens when 3 threads
    //are sleeping
    Thread *p2=Thread::create(t3_p2,STACK_MIN,0,NULL);
    //p will wake @ t=45, main will wake @ t=47 and p2 @ t=50ms
    //this will test proper sorting of sleeping_list in the kernel
    Thread::sleep(12);
    if(t3_v2==false) fail("Thread::sleep() (2)");
    //Deleting the two instances of t3_p2
    t3_deleted=false;
    p->terminate();
    Thread::sleep(20);
    if(Thread::exists(p)) fail("multiple instances (1)");
    if(t3_deleted==false) fail("multiple instances (2)");
    t3_deleted=false;
    p2->terminate();
    Thread::sleep(20);
    if(Thread::exists(p)) fail("multiple instances (3)");
    if(t3_deleted==false) fail("multiple instances (4)");
    //Testing Thread::sleepUntil()
    long long tick;
    const int period=static_cast<int>(TICK_FREQ*0.01); //10ms
    {
        InterruptDisableLock lock; //Making these two operations atomic.
        tick=getTick();
        tick+=period;
    }
    for(int i=0;i<4;i++)
    {
        Thread::sleepUntil(tick);
        if(tick!=getTick()) fail("Thread::sleepUntil()");
        tick+=period;
    }
    pass();
}

//
// Test 4
//
/*
tests:
disableInterrupts
enableInterrupts
Thread::getPriority
Thread::setPriority
Thread::IRQgetCurrentThread
Thread::IRQgetPriority
*/

static volatile bool t4_v1;

static void t4_p1(void *argv)
{
    for(;;)
    {
        if(Thread::testTerminate()) break;
        t4_v1=true;
    }
}

static void test_4()
{
    test_name("disableInterrupts and priority");
    Thread *p=Thread::create(t4_p1,STACK_MIN,0,NULL);
    //Check getPriority()
    if(p->getPriority()!=0) fail("getPriority (0)");
    //Check that getCurrentThread() and IRQgetCurrentThread() return the
    //same value
    Thread *q=Thread::getCurrentThread();
    disableInterrupts();//
    if(q!=Thread::IRQgetCurrentThread()) fail("IRQgetCurrentThread");
    //Check IRQgetPriority
    if(p->IRQgetPriority()!=0) fail("IRQgetPriority");
    //Check that tick is not incremented and t4_v1 is not updated
    int i;
    long long tick=getTick();
    t4_v1=false;
    for(i=0;i<4;i++)
    {
        delayMs(100);
        if((t4_v1==true)||(tick!=getTick()))
        {
            enableInterrupts();//
            fail("disableInterrupts");
        }
    }
    enableInterrupts();//
    Thread::yield();
    //Should not happen, since already tested
    if(t4_v1==false) fail("variable not updated");
    //Checking get_priority
    if(Thread::getCurrentThread()->getPriority()!=0)
        fail("getPriority (1)");
    //Checking setPriority
    Thread::setPriority(1);
    if(Thread::getCurrentThread()->getPriority()!=1)
        fail("setPriority (0)");
    #ifndef SCHED_TYPE_CONTROL_BASED
    //Since priority is higher, the other thread must not run
    //Of course this is not true for the control based scheduler
    t4_v1=false;
    for(i=0;i<4;i++)
    {
        Thread::yield();//must return immediately
        delayMs(100);
        if(t4_v1==true) fail("setPriority (1)");
    }

    #endif //SCHED_TYPE_CONTROL_BASED
    //Restoring original priority
    Thread::setPriority(0);
    if(Thread::getCurrentThread()->getPriority()!=0)
        fail("setPriority (2)");
    p->terminate();
    pass();
}

//
// Test 5
//
/*
tests:
Thread::wait
Thread::wakeup
Thread::IRQwait
Thread::IRQwakeup
*/

static volatile bool t5_v1;
static volatile bool t5_v2;//False=testing Thread::wait() else Thread::IRQwait()

static void t5_p1(void *argv)
{
    for(;;)
    {
        if(Thread::testTerminate()) break;
        t5_v1=true;
        if(t5_v2) Thread::wait();
        else {
            disableInterrupts();
            Thread::IRQwait();
            enableInterrupts();
            Thread::yield();
        }
    }
}

static void test_5()
{
    test_name("wait, wakeup, IRQwait, IRQwakeup");
    t5_v2=false;//Testing wait
    t5_v1=false;
    Thread *p=Thread::create(t5_p1,STACK_MIN,0,NULL);
    //Should not happen, since already tested
    Thread::sleep(5);
    if(t5_v1==false) fail("variable not updated");
    int i;
    //Since the other thread is waiting, must not update t5_v1
    t5_v1=false;
    for(i=0;i<4;i++)
    {
        Thread::sleep(100);
        if(t5_v1==true) fail("Thread::wait");
    }
    t5_v1=false;
    t5_v2=true;//Test IRQwait()
    p->wakeup();
    //Now that is still running, must update the variable
    Thread::sleep(5);
    if(t5_v1==false) fail("Thread::wakeup");
    //Since the other thread is waiting, must not update t5_v1
    t5_v1=false;
    for(i=0;i<4;i++)
    {
        Thread::sleep(100);
        if(t5_v1==true) fail("Thread::IRQwait");
    }
    disableInterrupts();
    p->IRQwakeup();
    t5_v1=false;
    enableInterrupts();
    //Now that is still running, must update the variable
    Thread::sleep(5);
    if(t5_v1==false) fail("Thread::IRQwakeup");
    p->terminate();
    p->wakeup();//Wake thread so that it can terminate
    pass();
}

//
// Test 6
//
/*
tests:
Mutex::lock
Mutex::unlock
Mutex::tryLock
Lock
*/

class Sequence
{
    public:
    Sequence()
    {
        s[0]='\0';
    }

    void add(const char c)
    {
        if(strlen(s)>=3) fail("error in Sequence class");
        char t[2];
        t[0]=c; t[1]='\0';
        strcat(s,t);
    }

    char *read()
    {
        return s;
    }

    void clear()
    {
        s[0]='\0';
    }

    private:
    char s[4];
};

static Sequence seq;
static Mutex t6_m1;

static void t6_p1(void *argv)
{
    t6_m1.lock();
    seq.add('1');
    Thread::sleep(100);
    //Testing priority inheritance. Priority is 2 because inherits priority
    //from t6_p3
    if(Thread::getCurrentThread()->getPriority()!=2)
        fail("priority inheritance (1)");
    t6_m1.unlock();
}

static void t6_p2(void *argv)
{
    t6_m1.lock();
    seq.add('2');
    Thread::sleep(100);
    //Testing priority inheritance. Priority is 1 because enters after t6_p3
    if(Thread::getCurrentThread()->getPriority()!=1)
        fail("priority inheritance (2)");
    t6_m1.unlock();
}

static void t6_p3(void *argv)
{
    t6_m1.lock();
    seq.add('3');
    Thread::sleep(100);
    //Testing priority inheritance. Original priority 2 must not change
    if(Thread::getCurrentThread()->getPriority()!=2)
        fail("priority inheritance (3)");
    t6_m1.unlock();
}

static void t6_p4(void *argv)
{
    t6_m1.lock();
    for(;;)
    {
        if(Thread::testTerminate())
        {
            t6_m1.unlock();
            break;
        }
        Thread::yield();
    }
}

static volatile bool t6_v1;
static Mutex t6_m2;

static void t6_p5(void *argv)
{
    for(;;)
    {
        if(Thread::testTerminate()) break;

        {
            Lock l(t6_m2);
            t6_v1=true;
        }

        Thread::sleep(10);
    }
}

static Mutex t6_m3;
static Mutex t6_m4;

static void t6_p6(void *argv)
{
    Mutex *m=reinterpret_cast<Mutex*>(argv);
    {
        Lock l(*m);
        Thread::sleep(1);
    }
}

static Mutex t6_m5(Mutex::RECURSIVE);

static void test_6()
{
    test_name("Mutex class");
    seq.clear();
    //Create first thread
    Thread::create(t6_p1,STACK_MIN,0,NULL);
    Thread::sleep(20);
    //Create second thread
    Thread::create(t6_p2,STACK_MIN,1,NULL);
    Thread::sleep(20);
    //Create third thread
    Thread::create(t6_p3,STACK_MIN,2,NULL);
    Thread::sleep(20);
    t6_m1.lock();
    /*
    Now there will be 4 threads on the Mutex, the first one, with priority 0,
    waiting 100ms the second with priority 1, the third with priority 2 and the
    fourth (this) with priority 0. The first will go first (because it entered
    first), and will inherit priority 1 when the second enters the waiting list,
    and priority 2 when the third enters the waiting list. When the first
    leaves, the third enters (list is ordered by priority), then the second and
    last the fourth.
    */
    //Testing priority inheritance. Original priority 0 must not change
    if(Thread::getCurrentThread()->getPriority()!=0)
        fail("priority inheritance (4)");
    if(strcmp(seq.read(),"132")!=0)
    {
        //iprintf("%s\n",seq.read());
        fail("incorrect sequence");
    }
    t6_m1.unlock();

    Thread::sleep(350);//Ensure all threads are deleted
    //
    // Testing tryLock
    //

    //This thread will hold the lock until we terminate it
    Thread *t=Thread::create(t6_p4,STACK_MIN,0);
    Thread::yield();
    if(t6_m1.tryLock()==true) fail("Mutex::tryLock() (1)");
    Thread::sleep(10);
    if(t6_m1.tryLock()==true) fail("Mutex::tryLock() (2)");
    t->terminate();
    Thread::yield();
    Thread::yield();//Ensuring the other thread is deleted
    if(t6_m1.tryLock()==false) fail("Mutex::tryLock() (3)");
    t6_m1.unlock();
    t6_v1=false;
    Thread *t2=Thread::create(t6_p5,STACK_MIN,1);
    Thread::sleep(30);
    if(t6_v1==false) fail("Thread not created");
    {
        Lock l(t6_m2);
        t6_v1=false;
        Thread::sleep(30);
        if(t6_v1==true) fail("Lock (1)");
    }
    Thread::sleep(30);
    if(t6_v1==false) fail("Lock (2)");
    t2->terminate();
    Thread::sleep(10);
    //
    // Testing full priority inheritance algorithm
    //
    Thread *t3;
    Thread *t4;
    {
        Lock l1(t6_m3);
        {
            Lock l2(t6_m4);
            //Check initial priority
            if(Thread::getCurrentThread()->getPriority()!=0)
                fail("priority inheritance (5)");
            //This thread will increase priority to 1
            t=Thread::create(t6_p6,STACK_MIN,1,reinterpret_cast<void*>(
                    &t6_m4),Thread::JOINABLE);
            Thread::sleep(10);
            if(Thread::getCurrentThread()->getPriority()!=1)
                fail("priority inheritance (6)");
            //This thread will do nothing
            t2=Thread::create(t6_p6,STACK_MIN,1,reinterpret_cast<void*>(
                    &t6_m3),Thread::JOINABLE);
            Thread::sleep(10);
            if(Thread::getCurrentThread()->getPriority()!=1)
                fail("priority inheritance (7)");
            //This thread will increase priority to 2
            t3=Thread::create(t6_p6,STACK_MIN,2,reinterpret_cast<void*>(
                    &t6_m4),Thread::JOINABLE);
            Thread::sleep(10);
            if(Thread::getCurrentThread()->getPriority()!=2)
                fail("priority inheritance (8)");
            //This will do nothing
            Thread::setPriority(1);
            if(Thread::getCurrentThread()->getPriority()!=2)
                fail("priority inheritance (9)");
            //This will increase priority
            Thread::setPriority(3);
            if(Thread::getCurrentThread()->getPriority()!=3)
                fail("priority inheritance (10)");
            //This will reduce priority to 2, not 0
            Thread::setPriority(0);
            if(Thread::getCurrentThread()->getPriority()!=2)
                fail("priority inheritance (11)");
        }
        //Unlocking t6_m4, only the thread waiting on t6_m3 will cause
        //priority inheritance, which is only t2 which has priority 1
        if(Thread::getCurrentThread()->getPriority()!=1)
                fail("priority inheritance (12)");
        //This will make the priority of 1 hold even when unlocking the mutex
        Thread::setPriority(1);
        //This thread will increase priority to 2
        t4=Thread::create(t6_p6,STACK_MIN,2,reinterpret_cast<void*>(
                &t6_m3),Thread::JOINABLE);
        Thread::sleep(10);
        if(Thread::getCurrentThread()->getPriority()!=2)
            fail("priority inheritance (13)");

    }
    if(Thread::getCurrentThread()->getPriority()!=1)
        fail("priority inheritance (14)");
    //Restore original priority
    Thread::setPriority(0);
    if(Thread::getCurrentThread()->getPriority()!=0)
        fail("priority inheritance (14)");
    t->join();
    t2->join();
    t3->join();
    t4->join();
    Thread::sleep(10);
    
    //
    // Testing recursive mutexes
    //
    {
        Lock l(t6_m5);
        {
            Lock l(t6_m5);
            {
                Lock l(t6_m5);
                t=Thread::create(t6_p6,STACK_MIN,0,reinterpret_cast<void*>(
                    &t6_m5));
                Thread::sleep(10);
                //If thread does not exist it means it has locked the mutex
                //even if we locked it first
                if(Thread::exists(t)==false) fail("recursive mutex (1)");
            }
            Thread::sleep(10);
            //If thread does not exist the mutex was unlocked too early
            if(Thread::exists(t)==false) fail("recursive mutex (2)");
        }
        Thread::sleep(10);
        //If thread does not exist the mutex was unlocked too early
        if(Thread::exists(t)==false) fail("recursive mutex (3)");
    }
    Thread::sleep(10);
    //If thread exists the mutex was not unlocked
    if(Thread::exists(t)==true) fail("recursive mutex (4)");
    
    pass();
}

//
// Test 7
//
/*
tests:
Timer::start
Timer::stop
Timer::isRunning
Timer::interval
Timer::clear
*/

static void test_timer(Timer *t)
{
    //Testing interval behaviour when timer never started
    if(t->interval()!=-1) fail("interval (1)");
    //Testing isRunning
    if(t->isRunning()==true) fail("isRunning (1)");
    t->start();
    //Testing interval behaviour when timer not stopped
    if(t->interval()!=-1) fail("interval (2)");
    //Testing isRunning
    if(t->isRunning()==false) fail("isRunning (2)");
    Thread::sleep(100);
    t->stop();
    //Testing interval precision
    if(t->interval()==-1) fail("interval (3)");
    if(abs(t->interval()-((100*TICK_FREQ)/1000))>4) fail("not precise");
    //Testing isRunning
    if(t->isRunning()==true) fail("isRunning (1)");
}

static void test_7()
{
    test_name("Timer class");
    Timer t;
    //Testing a new timer
    test_timer(&t);
    t.clear();
    //Testing after clear
    test_timer(&t);
    //Testing copy constructor and operator = with a stopped timer
    t.clear();
    t.start();
    Thread::sleep(100);
    t.stop();
    Timer w(t);
    if(w.interval()==-1) fail("interval (copy 1)");
    if(abs(w.interval()-((100*TICK_FREQ)/1000))>4) fail("not precise (copy 1)");
    if(w.isRunning()==true) fail("isRunning (copy 1)");
    Timer q;
    q=t;
    if(q.interval()==-1) fail("interval (= 1)");
    if(abs(q.interval()-((100*TICK_FREQ)/1000))>4) fail("not precise (= 1)");
    if(q.isRunning()==true) fail("isRunning (= 1)");
    //Testing copy constructor and operator = with a running timer
    t.clear();
    t.start();
    Thread::sleep(100);
    Timer x(t);//copy constructor called when running
    x.stop();
    if(x.interval()==-1) fail("interval (copy 2)");
    if(abs(x.interval()-((100*TICK_FREQ)/1000))>4) fail("not precise (copy 2)");
    if(x.isRunning()==true) fail("isRunning (copy 2)");
    Timer y;
    y=t;//Operator = called when running
    y.stop();
    if(y.interval()==-1) fail("interval (= 2)");
    if(abs(y.interval()-((100*TICK_FREQ)/1000))>4) fail("not precise (= 2)");
    if(y.isRunning()==true) fail("isRunning (= 2)");
    //Testing concatenating time intervals
    t.clear();//Calling clear without calling stop. done on purpose
    t.start();
    Thread::sleep(100);
    t.stop();
    t.start();
    Thread::sleep(150);
    t.stop();
    if(t.interval()==-1) fail("interval (= 2)");
    if(abs(t.interval()-((250*TICK_FREQ)/1000))>4) fail("not precise (= 2)");
    pass();
}

//
// Test 8
//
/*
tests:
Queue::isFull()
Queue::isEmpty()
Queue::put()
Queue::get()
Queue::reset()
Queue::IRQput()
Queue::waitUntilNotFull()
Queue::IRQget()
Queue::waitUntilNotEmpty()
FIXME: The overloaded versions of IRQput and IRQget are not tested
*/

static Queue<char,4> t8_q1;
static Queue<char,4> t8_q2;

static void t8_p1(void *argv)
{
    for(;;)
    {
        if(Thread::testTerminate()) break;
        char c;
        t8_q1.get(c);
        t8_q2.put(c);
    }
}

static void test_8()
{
    test_name("Queue class");
    //A newly created queue must be empty
    if(t8_q1.isEmpty()==false) fail("isEmpty (1)");
    if(t8_q1.isFull()==true) fail("isFull (1)");
    //Adding elements
    t8_q1.put('0');
    if(t8_q1.isEmpty()==true) fail("isEmpty (2)");
    if(t8_q1.isFull()==true) fail("isFull (2)");
    t8_q1.put('1');
    if(t8_q1.isEmpty()==true) fail("isEmpty (2)");
    if(t8_q1.isFull()==true) fail("isFull (2)");
    t8_q1.put('2');
    if(t8_q1.isEmpty()==true) fail("isEmpty (3)");
    if(t8_q1.isFull()==true) fail("isFull (3)");
    t8_q1.put('3');
    if(t8_q1.isEmpty()==true) fail("isEmpty (4)");
    if(t8_q1.isFull()==false) fail("isFull (4)");
    //Removing elements
    char c;
    t8_q1.get(c);
    if(c!='0') fail("get (1)");
    if(t8_q1.isEmpty()==true) fail("isEmpty (5)");
    if(t8_q1.isFull()==true) fail("isFull (5)");
    t8_q1.get(c);
    if(c!='1') fail("get (2)");
    if(t8_q1.isEmpty()==true) fail("isEmpty (6)");
    if(t8_q1.isFull()==true) fail("isFull (6)");
    t8_q1.get(c);
    if(c!='2') fail("get (3)");
    if(t8_q1.isEmpty()==true) fail("isEmpty (7)");
    if(t8_q1.isFull()==true) fail("isFull (7)");
    t8_q1.get(c);
    if(c!='3') fail("get (4)");
    if(t8_q1.isEmpty()==false) fail("isEmpty (8)");
    if(t8_q1.isFull()==true) fail("isFull (8)");
    //Testing reset
    t8_q1.put('a');
    t8_q1.put('b');
    t8_q1.reset();
    if(t8_q1.isEmpty()==false) fail("reset (isEmpty)");
    if(t8_q1.isFull()==true) fail("reset (isFull)");
    //Test queue between threads
    t8_q1.reset();
    t8_q2.reset();
    Thread *p=Thread::create(t8_p1,STACK_MIN,0,NULL);
    int i,j;
    char write='A', read='A';
    for(i=1;i<=8;i++)
    {
        for(j=0;j<i;j++)
        {
            t8_q1.put(write);
            write++;//Advance to next char, to check order
        }
        for(j=0;j<i;j++)
        {
            char c;
            t8_q2.get(c);
            if(c!=read) fail("put or get (1)");
            read++;
        }
    }
    //Test waitUntilNotFull and IRQput
    t8_q1.reset();
    t8_q2.reset();
    write='A';
    read='A';
    disableInterrupts();//
    for(j=0;j<8;j++)
    {
        if(t8_q1.isFull())
        {
            enableInterrupts();//
            t8_q1.waitUntilNotFull();
            disableInterrupts();//
        }
        if(t8_q1.isFull()) fail("waitUntilNotFull()");
        if(t8_q1.IRQput(write)==false) fail("IRQput (1)");
        write++;//Advance to next char, to check order
    }
    enableInterrupts();//
    for(j=0;j<8;j++)
    {
        char c;
        t8_q2.get(c);
        if(c!=read) fail("IRQput (2)");
        read++;
    }
    //Test waitUntilNotEmpty and IRQget
    t8_q1.reset();
    t8_q2.reset();
    write='A';
    read='A';
    for(j=0;j<8;j++)
    {
        disableInterrupts();//
        t8_q1.IRQput(write);
        write++;
        if(t8_q2.isEmpty()==false) fail("Unexpected");
        enableInterrupts();//
        t8_q2.waitUntilNotEmpty();
        disableInterrupts();//
        if(t8_q2.isEmpty()==true) fail("waitUntilNotEmpty()");
        char c='\0';
        if(t8_q2.IRQget(c)==false) fail("IRQget (1)");
        if(c!=read) fail("IRQget (2)");
        read++;
        enableInterrupts();//
    }
    p->terminate();
    t8_q1.put('0');
    //Make sure the queue is empty in case the testsuite is run again
    Thread::sleep(5);
    t8_q1.reset();
    t8_q2.reset();
    pass();
}

//
// Test 9
//
/*
tests:
isKernelRunning()
areInterruptsEnabled()
*/

static void test_9()
{
    test_name("isKernelRunning and save/restore interrupts");
    //Testing kernel_running with nested pause_kernel()
    if(isKernelRunning()==false) fail("isKernelRunning() (1)");
    pauseKernel();//1
    if(isKernelRunning()==true)
    {
        restartKernel();
        fail("isKernelRunning() (2)");
    }
    pauseKernel();//2
    if(isKernelRunning()==true)
    {
        restartKernel();
        restartKernel();
        fail("isKernelRunning() (3)");
    }
    restartKernel();//2
    if(isKernelRunning()==true)
    {
        restartKernel();
        fail("isKernelRunning() (4)");
    }
    restartKernel();//1
    if(isKernelRunning()==false) fail("isKernelRunning() (5)");
    //Testing nesting of disableInterrupts()
    long long i;
    if(areInterruptsEnabled()==false) fail("areInterruptsEnabled() (1)");
    disableInterrupts();//Now interrupts should be disabled
    i=getTick();
    delayMs(100);
    if(i!=getTick())
    {
        enableInterrupts();
        fail("disableInterrups() nesting (1)");
    }
    if(areInterruptsEnabled()==true)
    {
        enableInterrupts();
        fail("areInterruptsEnabled() (2)");
    }
    disableInterrupts();//Interrupts already disabled
    i=getTick();
    delayMs(100);
    if(i!=getTick())
    {
        enableInterrupts();
        fail("disableInterrups() nesting (2)");
    }
    if(areInterruptsEnabled()==true)
    {
        enableInterrupts();
        fail("areInterruptsEnabled() (3)");
    }
    enableInterrupts();//Now interrupts should remain disabled
    i=getTick();
    delayMs(100);
    if(i!=getTick())
    {
        enableInterrupts();
        fail("enableInterrupts() nesting (1)");
    }
    if(areInterruptsEnabled()==true)
    {
        enableInterrupts();
        fail("areInterruptsEnabled() (4)");
    }
    enableInterrupts();//Now interrupts should be enabled
    i=getTick();
    delayMs(100);
    if(i==getTick())
    {
        enableInterrupts();
        fail("enableInterrupts() nesting (2)");
    }
    if(areInterruptsEnabled()==false) fail("areInterruptsEnabled() (5)");
    pass();
}

//
// Test 10
//
/*
tests exception propagation throug entry point of a thread
*/

void t10_f1()
{
    throw(std::logic_error("Ok, exception test worked"));
}

void t10_f2()
{
    try {
        t10_f1();
        fail("Exception not thrown (1)");
    } catch(std::logic_error& e)
    {
        throw;
    }
}

void t10_p1(void *argv)
{
    t10_f2();
    fail("Exception not thrown");
}

void test_10()
{
    test_name("Exception handling");
    Thread::sleep(10);
    Thread *t=Thread::create(t10_p1,1024+512,0,NULL);
    Thread::sleep(40);
    if(Thread::exists(t)) fail("Thread not deleted");
    pass();
}

//
// Test 11
//
/*
tests class MemoryProfiling
*/

static volatile unsigned int t11_v1;//Free heap after spawning thread

void t11_p1(void *argv)
{
    if(MemoryProfiling::getStackSize()!=STACK_MIN) fail("getStackSize (2)");
    //Check that getCurrentFreeHeap returns the same value from different
    //threads if no heap allocation happened in between
    Thread::sleep(10);//Give time to other thread to set variable
    if(MemoryProfiling::getCurrentFreeHeap()!=t11_v1)
        fail("getCurrentFreeHeap (2)");
}

void test_11()
{
    test_name("MemoryProfiling class");
    MemoryProfiling::print();
    //Stack size
    unsigned int stackSize=MemoryProfiling::getStackSize();
    if(stackSize!=MAIN_STACK_SIZE) fail("getStackSize (1)");
    unsigned int curFreeStack=MemoryProfiling::getCurrentFreeStack();
    if(curFreeStack>stackSize) fail("getCurrentFreeStack (1)");
    if(MemoryProfiling::getAbsoluteFreeStack()>stackSize)
        fail("getAbsoluteFreeStack (1)");
    if(MemoryProfiling::getAbsoluteFreeStack()>curFreeStack-4)
        fail("getAbsoluteFreeStack (2)");
        unsigned int heapSize=MemoryProfiling::getHeapSize();
    if(MemoryProfiling::getCurrentFreeHeap()>heapSize)
        fail("getCurrentFreeHeap");
    if(MemoryProfiling::getAbsoluteFreeHeap()>heapSize)
        fail("getAbsoluteFreeHeap");
    //Multithread test
    Thread *t=Thread::create(t11_p1,STACK_MIN,0,0,Thread::JOINABLE);
    t11_v1=MemoryProfiling::getCurrentFreeHeap();
    t->join(0);
    Thread::sleep(10); //Give time to the idle thread for deallocating resources
    pass();
}

//
// Test 12
//
/*
Test data structures:
 PriorityQueue
*/

void t12_f1(miosix::PriorityQueue<int>& queue)
{
    if(queue.empty()==false) fail("empty (1)");
    if(queue.size()!=0) fail("size (1)");
    const unsigned int size=128;
    int data[size];
    for(unsigned int i=0;i<size;i++)
    {
        data[i]=rand();
        queue.push(data[i]);
    }
    std::sort(data,data+size);
    if(queue.empty()) fail("empty (2)");
    if(queue.size()!=size) fail("size (2)");
    for(unsigned int i=0;i<size;i++)
    {
        if(data[i]!=queue.top())
        {
            iprintf("Error i=%d data[i]=%d top()=%d\n",i,data[i],queue.top());
            fail("PriorityQueue");
        }
        queue.pop();
    }
    if(queue.empty()==false) fail("empty (3)");
    if(queue.size()!=0) fail("size (3)");
}

void test_12()
{
    test_name("PriorityQueue");
    miosix::PriorityQueue<int> queue;
    for(int i=0;i<10;i++) t12_f1(queue);
    pass();
}

//
// Test 13
//
/*
Tests:
printing through stderr
*/

void test_13()
{
    test_name("stderr");
    //Test stderr, as it used to fail
    int res=fiprintf(stderr,"This string is printed through stderr. %d\n",0);
    if(res<=0) fail("stderr");
    pass();
}

//
// Test 14
//
/*
Tests:
- Thread::create with void * entry point
- Thread::create with joinable threads
- Thread::isDetached
- Thread::join
- Thread::detach
*/

void *t14_p1(void *argv)
{
    for(;;)
    {
        if(Thread::testTerminate()) break;
        Thread::yield();
    }
    return argv;
}

void *t14_p2(void *argv)
{
    Thread::sleep(50);
    return argv;
}

void t14_p3(void *argv)
{
    Thread::sleep(20);
    Thread *t=reinterpret_cast<Thread *>(argv);
    t->detach();
}

void test_14()
{
    test_name("joinable threads");
    //
    // Memory leak tests. These tests make sure that memory of joinable threads
    // is always deallocated. Since during tests MemoryStatistics.print() is
    // called, it is possible to run the test repeatedly and check for memory
    // leaks.
    //

    //Test 1: detached thread that returns void*
    Thread *t=Thread::create(t14_p1,STACK_MIN,0,0);
    t->terminate();
    Thread::sleep(10);
    if(Thread::exists(t)) fail("detached thread");
    //Test 2: joining a not deleted thread
    t=Thread::create(t14_p2,STACK_MIN,0,0,Thread::JOINABLE);
    t->join();
    Thread::sleep(10);
    //Test 3: detaching a thread while not deleted
    t=Thread::create(t14_p2,STACK_MIN,0,0,Thread::JOINABLE);
    Thread::yield();
    t->detach();
    Thread::sleep(10);
    //Test 4: detaching a deleted thread
    t=Thread::create(t14_p1,STACK_MIN,0,0,Thread::JOINABLE);
    t->terminate();
    Thread::sleep(10);
    t->detach();
    Thread::sleep(10);
    //Test 5: detaching a thread on which some other thread called join
    t=Thread::create(t14_p2,STACK_MIN,0,0,Thread::JOINABLE);
    Thread::create(t14_p3,STACK_MIN,0,reinterpret_cast<void*>(t));
    if(t->join()==true) fail("thread not detached (1)");

    //
    // Consistency tests, these make sure that joinable threads are correctly
    // implemented.
    //

    //Test 1: join on joinable, not already deleted
    void *result=0;
    t=Thread::create(t14_p2,STACK_MIN,0,(void*)0xdeadbeef,Thread::JOINABLE);
    Thread::yield();
    if(t->join(&result)==false) fail("Thread::join (1)");
    if(Thread::exists(t)) fail("Therad::exists (1)");
    if(reinterpret_cast<unsigned>(result)!=0xdeadbeef) fail("join result (1)");
    Thread::sleep(10);

    //Test 2: join on joinable, but detach called before
    t=Thread::create(t14_p2,STACK_MIN,0,0,Thread::JOINABLE);
    Thread::yield();
    t->detach();
    if(t->join()==true) fail("Thread::join (2)");
    Thread::sleep(60);

    //Test 3: join on joinable, but detach called after
    t=Thread::create(t14_p1,STACK_MIN,0,0,Thread::JOINABLE);
    Thread::create(t14_p3,STACK_MIN,0,reinterpret_cast<void*>(t));
    if(t->join()==true) fail("Thread::join (3)");
    t->terminate();
    Thread::sleep(10);

    //Test 4: join on detached, not already deleted
    t=Thread::create(t14_p1,STACK_MIN,0,0);
    if(t->join()==true) fail("Thread::join (4)");
    t->terminate();
    Thread::sleep(10);

    //Test 5: join on self
    if(Thread::getCurrentThread()->join()==true)
        fail("Thread:: join (5)");

    //Test 6: join on already deleted
    result=0;
    t=Thread::create(t14_p1,STACK_MIN,0,(void*)0xdeadbeef,Thread::JOINABLE);
    t->terminate();
    Thread::sleep(10);
    if(Thread::exists(t)==false) fail("Therad::exists (2)");
    if(t->join(&result)==false) fail("Thread::join (6)");
    if(Thread::exists(t)) fail("Therad::exists (3)");
    if(reinterpret_cast<unsigned>(result)!=0xdeadbeef) fail("join result (2)");
    Thread::sleep(10);

    //Test 7: join on already detached and deleted
    t=Thread::create(t14_p1,STACK_MIN,0,0,Thread::JOINABLE);
    t->detach();
    t->terminate();
    Thread::sleep(10);
    if(Thread::exists(t)) fail("Therad::exists (4)");
    if(t->join()==true) fail("Thread::join (7)");
    Thread::sleep(10);
    
    pass();
}

//
// Test 15
//
/*
Tests:
- Condition variables
*/

static volatile bool t15_v1;
static volatile bool t15_v2;
static int t15_v3;
static ConditionVariable t15_c1;
static Mutex t15_m1;

void t15_p1(void *argv)
{
    for(int i=0;i<10;i++)
    {
        Lock l(t15_m1);
        t15_c1.wait(l);
        t15_v1=true;
        t15_v3++;
    }
}

void t15_p2(void *argv)
{
    for(int i=0;i<10;i++)
    {
        Lock l(t15_m1);
        t15_c1.wait(l);
        t15_v2=true;
        t15_v3++;
    }
}

static void test_15()
{
    test_name("Condition variables");
    //Test signal
    t15_v1=false;
    t15_v2=false;
    t15_v3=0;
    Thread *p1=Thread::create(t15_p1,STACK_MIN,0);
    Thread *p2=Thread::create(t15_p2,STACK_MIN,0);
    for(int i=0;i<20;i++)
    {
        Thread::sleep(10);
        if(t15_v1 || t15_v2) fail("Spurious wakeup (1)");
        t15_c1.signal();
        Thread::sleep(10);
        {
            Lock l(t15_m1);
            if(t15_v1 && t15_v2) fail("Both threads wake up");
            if(t15_v3!=i+1) fail("Mutple wakeup (1)");
        }
        t15_v1=false;
        t15_v2=false;
    }
    Thread::sleep(10);
    if(Thread::exists(p1) || Thread::exists(p2)) fail("Threads not deleted (1)");
    //Test broadcast
    t15_v1=false;
    t15_v2=false;
    t15_v3=0;
    p1=Thread::create(t15_p1,STACK_MIN,0);
    p2=Thread::create(t15_p2,STACK_MIN,0);
    for(int i=0;i<10;i++)
    {
        Thread::sleep(10);
        if(t15_v1 || t15_v2) fail("Spurious wakeup (2)");
        t15_c1.broadcast();
        Thread::sleep(10);
        {
            Lock l(t15_m1);
            if((t15_v1 && t15_v2)==false) fail("Not all thread woken up");
            if(t15_v3!=2*(i+1)) fail("Mutple wakeup (2)");
        }
        t15_v1=false;
        t15_v2=false;
    }
    Thread::sleep(10);
    if(Thread::exists(p1) || Thread::exists(p2)) fail("Threads not deleted (2)");
    pass();
}

//
// Test 16
//
/*
tests:
posix threads API
 pthread_attr_init
 pthread_attr_destroy
 pthread_attr_getstacksize
 pthread_attr_setstacksize
 pthread_attr_getdetachstate
 pthread_atthr_setdetachstate
 pthread_detach
 pthread_create
 pthread_join
 pthread_self
 pthread_equal
 pthread_mutex_init
 pthread_mutex_destroy
 pthread_mutex_lock
 pthread_mutex_unlock
 pthread_mutex_trylock
 pthread_cond_init
 pthread_cond_destroy
 pthread_cond_wait
 pthread_cond_signal
 pthread_cond_broadcast
*/

inline Thread *toThread(pthread_t t)
{
    return reinterpret_cast<Thread*>(t);
}

void *t16_p1(void *argv)
{
    toThread(pthread_self())->wait();
    return argv;
}

volatile bool t16_v1;
pthread_mutex_t t16_m1=PTHREAD_MUTEX_INITIALIZER;

void *t16_p2(void *argv)
{
    pthread_mutex_t *mutex=(pthread_mutex_t*)argv;
    if(pthread_mutex_lock(mutex)!=0) fail("mutex_lock (2)");
    t16_v1=true;
    Thread::sleep(50);
    if(pthread_mutex_unlock(mutex)!=0) fail("mutex unlock (2)");
    return NULL;
}

pthread_cond_t t16_c1=PTHREAD_COND_INITIALIZER;

void *t16_p3(void *argv)
{
    Thread::sleep(30);
    if(pthread_mutex_trylock(&t16_m1)!=0)
        fail("cond_wait did not release mutex"); //<---
    t16_v1=true;
    if(pthread_cond_signal(&t16_c1)!=0) fail("signal");
    if(pthread_mutex_unlock(&t16_m1)!=0) fail("cond mutex unlock"); //<---
    return NULL;
}

pthread_cond_t t16_c2;

void *t16_p4(void *argv)
{
    Thread::sleep(30);
    if(pthread_mutex_trylock(&t16_m1)!=0)
        fail("cond_wait did not release mutex (2)"); //<---
    t16_v1=true;
    //testing pthread_cond_destroy while a thread is waiting
    if(pthread_cond_destroy(&t16_c2)==0) fail("cond destroy while waiting");
    //testing broadcast
    if(pthread_cond_broadcast(&t16_c2)!=0) fail("broadcast");
    if(pthread_mutex_unlock(&t16_m1)!=0) fail("cond mutex unlock (2)"); //<---
    return NULL;
}

static void test_16()
{
    test_name("posix threads");
    //
    // Test pthread_attr_*
    //
    pthread_attr_t attr;
    if(pthread_attr_init(&attr)!=0) fail("attr_init return value");
    int temp;
    size_t temp2;
    if(pthread_attr_getstacksize(&attr,&temp2)!=0)
        fail("attr_getstacksize return value (1)");
    if(temp2!=STACK_DEFAULT_FOR_PTHREAD) fail("attr_getstacksize (1)");
    if(pthread_attr_getdetachstate(&attr,&temp)!=0)
        fail("attr_getdetachstate (1)");
    if(temp!=PTHREAD_CREATE_JOINABLE) fail("attr_getdetahstate (1)");
    //setstacksize should fail for anything below STACK_MIN
    if(pthread_attr_setstacksize(&attr,STACK_MIN-4)!=EINVAL)
        fail("stacks < STACK_MIN allowed");
    if(pthread_attr_getstacksize(&attr,&temp2)!=0)
        fail("attr_getstacksize return value (2)");
    if(temp2!=STACK_DEFAULT_FOR_PTHREAD) fail("attr_getstacksize (2)");
    //set a valid stack size
    if(pthread_attr_setstacksize(&attr,STACK_MIN)!=0)
        fail("attr_setstacksize return value");
    if(pthread_attr_getstacksize(&attr,&temp2)!=0)
        fail("attr_getstacksize return value (3)");
    if(temp2!=STACK_MIN) fail("attr_getstacksize (3)");
    //now try setting detach state
    if(pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED)!=0)
        fail("attr_stedetachstate return value (1)");
    if(pthread_attr_getdetachstate(&attr,&temp)!=0)
        fail("attr_getdetachstate (2)");
    if(temp!=PTHREAD_CREATE_DETACHED) fail("attr_getdetahstate (2)");
    //
    // Test pthread_create, pthread_self, pthread_equal, pthread_attr_destroy
    //
    pthread_t thread;
    if(pthread_create(&thread,&attr,t16_p1,NULL)!=0)
        fail("pthread_create (1)");
    Thread *t=toThread(thread);
    if(Thread::exists(t)==false) fail("thread not created");
    if(t->isDetached()==false) fail("attr_setdetachstate not considered");
    pthread_t self=pthread_self();
    if(Thread::getCurrentThread()!=toThread(self)) fail("pthread_self");
    if(pthread_equal(thread,self)) fail("pthread_equal (1)");
    if(!pthread_equal(thread,thread)) fail("pthread_equal (2)");
    if(pthread_attr_destroy(&attr)!=0) fail("attr_destroy return value");
    Thread::sleep(10); //Give the other thread time to call wait()
    t->wakeup();
    Thread::sleep(10);
    if(Thread::exists(t)) fail("unexpected");
    //
    // testing pthread_join and pthread_detach
    //
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr,STACK_MIN);
    if(pthread_create(&thread,&attr,t16_p1,(void*)0xdeadbeef)!=0)
        fail("pthread_create (2)");
    t=toThread(thread);
    if(Thread::exists(t)==false) fail("thread not created (2)");
    if(t->isDetached()==true) fail("detached by mistake (1)");
    Thread::sleep(10); //Give the other thread time to call wait()
    t->wakeup();
    void *res;
    if(pthread_join(thread,&res)!=0) fail("join return value");
    if(((unsigned)res)!=0xdeadbeef) fail("entry point return value");
    if(Thread::exists(t)) fail("not joined");
    Thread::sleep(10);
    //Testing create with no pthread_attr_t
    if(pthread_create(&thread,NULL,t16_p1,NULL)!=0) fail("pthread_create (3)");
    t=toThread(thread);
    if(Thread::exists(t)==false) fail("thread not created (3)");
    if(t->isDetached()==true) fail("detached by mistake (2)");
    if(pthread_detach(thread)!=0) fail("detach return value");
    if(t->isDetached()==false) fail("detach");
    Thread::sleep(10); //Give the other thread time to call wait()
    t->wakeup();
    Thread::sleep(10);
    if(Thread::exists(t)) fail("unexpected (2)");
    //
    // testing pthread_mutex_*
    //
    if(pthread_mutex_lock(&t16_m1)!=0) fail("mutex lock"); //<---
    t16_v1=false;
    if(pthread_create(&thread,NULL,t16_p2,(void*)&t16_m1)!=0)
        fail("pthread_create (4)");
    t=toThread(thread);
    if(Thread::exists(t)==false) fail("thread not created (4)");
    if(t->isDetached()==true) fail("detached by mistake (3)");
    Thread::sleep(10);
    if(t16_v1==true) fail("mutex fail");
    if(pthread_mutex_unlock(&t16_m1)!=0) fail("mutex unlock"); //<---
    Thread::sleep(10);
    if(t16_v1==false) fail("mutex fail (2)");
    pthread_join(thread,NULL);
    Thread::sleep(10);
    //testing trylock, after the thread is created it will lock the mutex
    //for 50ms
    if(pthread_create(&thread,NULL,t16_p2,(void*)&t16_m1)!=0)
        fail("pthread_create (5)");
    Thread::sleep(10);
    if(pthread_mutex_trylock(&t16_m1)==0) fail("trylock");
    pthread_join(thread,NULL);
    if(pthread_mutex_trylock(&t16_m1)!=0) fail("trylock (2)"); //<---
    if(pthread_mutex_unlock(&t16_m1)!=0) fail("mutex unlock (3)"); //<---
    Thread::sleep(10);
    //now testing pthread_mutex_init
    pthread_mutex_t mutex;
    if(pthread_mutex_init(&mutex,NULL)!=0) fail("mutex init");
    if(pthread_mutex_lock(&mutex)!=0) fail("mutex lock (4)"); //<---
    t16_v1=false;
    if(pthread_create(&thread,NULL,t16_p2,(void*)&mutex)!=0)
        fail("pthread_create (6)");
    t=toThread(thread);
    if(Thread::exists(t)==false) fail("thread not created (5)");
    if(t->isDetached()==true) fail("detached by mistake (4)");
    Thread::sleep(10);
    if(t16_v1==true) fail("mutex fail (3)");
    if(pthread_mutex_unlock(&mutex)!=0) fail("mutex unlock (4)"); //<---
    Thread::sleep(10);
    if(t16_v1==false) fail("mutex fail (4)");
    pthread_join(thread,NULL);
    Thread::sleep(10);
    //testing pthread_mutex_destroy
    if(pthread_create(&thread,NULL,t16_p2,(void*)&mutex)!=0)
        fail("pthread_create (7)");
    t=toThread(thread);
    if(Thread::exists(t)==false) fail("thread not created (6)");
    if(t->isDetached()==true) fail("detached by mistake (5)");
    Thread::sleep(10);
    if(pthread_mutex_destroy(&mutex)==0) fail("mutex destroy");
    pthread_join(thread,NULL);
    Thread::sleep(10);
    if(pthread_mutex_destroy(&mutex)!=0) fail("mutex destroy (2)");
    //
    // Testing pthread_cond_*
    //
    if(pthread_create(&thread,NULL,t16_p3,NULL)!=0) fail("pthread_create (8)");
    t=toThread(thread);
    if(Thread::exists(t)==false) fail("thread not created (7)");
    if(t->isDetached()==true) fail("detached by mistake (6)");
    if(pthread_mutex_lock(&t16_m1)!=0) fail("mutex lock (5)"); //<---
    t16_v1=false;
    if(pthread_cond_wait(&t16_c1,&t16_m1)!=0) fail("wait");
    if(t16_v1==false) fail("did not really wait");
    if(pthread_mutex_unlock(&t16_m1)!=0) fail("mutex unlock (5)"); //<---
    pthread_join(thread,NULL);
    Thread::sleep(10);
    //now testing pthread_cond_init
    if(pthread_cond_init(&t16_c2,NULL)!=0) fail("cond_init");
    if(pthread_create(&thread,NULL,t16_p4,NULL)!=0) fail("pthread_create (9)");
    t=toThread(thread);
    if(Thread::exists(t)==false) fail("thread not created (8)");
    if(t->isDetached()==true) fail("detached by mistake (7)");
    if(pthread_mutex_lock(&t16_m1)!=0) fail("mutex lock (6)"); //<---
    t16_v1=false;
    if(pthread_cond_wait(&t16_c2,&t16_m1)!=0) fail("wait (2)");
    if(t16_v1==false) fail("did not really wait (2)");
    if(pthread_mutex_unlock(&t16_m1)!=0) fail("mutex unlock (6)"); //<---
    pthread_join(thread,NULL);
    Thread::sleep(10);
    if(pthread_cond_destroy(&t16_c2)!=0) fail("cond destroy");
    pass();
}

#ifdef WITH_FILESYSTEM
//
// Filesystem test 1
//
/*
tests:
mkdir()
fopen()
fclose()
fread()
fwrite()
fprintf()
fgets()
fgetc()
fseek()
ftell()
remove()
Also tests concurrent write by opening and writing 3 files from 3 threads
*/
static volatile bool p1end,p2end,p3end,fs_1_error;

static void fs_t1_p1(void *argv)
{
    FILE *f;
    if((f=fopen("/testdir/file_1.txt","w"))==NULL)
    {
        fs_1_error=true;
        p1end=true;
        return;
    }
    setbuf(f,NULL);
    char *buf=new char[512];
    memset(buf,'1',512);
    int i,j;
    for(i=0;i<512;i++)
    {
        j=fwrite(buf,1,512,f);
        if(j!=512)
        {
            iprintf("Written %d bytes instead of 512\n",j);
            delete[] buf;
            fs_1_error=true;
            p1end=true;
            fclose(f);
            return;
        }
    }
    delete[] buf;
    if(fclose(f)!=0)
    {
        fs_1_error=true;
        p1end=true;
        return;
    }
    p1end=true;
}

static void fs_t1_p2(void *argv)
{
    FILE *f;
    if((f=fopen("/testdir/file_2.txt","w"))==NULL)
    {
        fs_1_error=true;
        p2end=true;
        return;
    }
    setbuf(f,NULL);
    char *buf=new char[512];
    memset(buf,'2',512);
    int i,j;
    for(i=0;i<512;i++)
    {
        j=fwrite(buf,1,512,f);
        if(j!=512)
        {
            iprintf("Written %d bytes instead of 512\n",j);
            delete[] buf;
            fs_1_error=true;
            p2end=true;
            fclose(f);
            return;
        }
    }
    delete[] buf;
    if(fclose(f)!=0)
    {
        fs_1_error=true;
        p2end=true;
        return;
    }
    p2end=true;
}

static void fs_t1_p3(void *argv)
{
    FILE *f;
    if((f=fopen("/testdir/file_3.txt","w"))==NULL)
    {
        fs_1_error=true;
        p3end=true;
        return;
    }
    setbuf(f,NULL);
    char *buf=new char[512];
    memset(buf,'3',512);
    int i,j;
    for(i=0;i<512;i++)
    {
        j=fwrite(buf,1,512,f);
        if(j!=512)
        {
            iprintf("Written %d bytes instead of 512\n",j);
            delete[] buf;
            fs_1_error=true;
            p3end=true;
            fclose(f);
            return;
        }
    }
    delete[] buf;
    if(fclose(f)!=0)
    {
        fs_1_error=true;
        p3end=true;
        return;
    }
    p3end=true;
}

static void fs_test_1()
{
    test_name("C standard library file functions + mkdir()");
    iprintf("Please wait (long test)\n");
	//Test mkdir (if possible)
	int result=mkdir("/testdir",0);
	switch(result)
	{
            case 0: break;
            case -2:
                iprintf("Directory test not made because directory"
                        " already exists\n");
                break;
            default:
                iprintf("mkdir returned %d\n",result);
                fail("Directory::mkdir()");
	}
	//Test concurrent file write access
    p1end=false;
    p2end=false;
    p3end=false;
    fs_1_error=false;
    Thread::create(fs_t1_p1,2048+512,1,NULL);
    Thread::create(fs_t1_p2,2048+512,1,NULL);
    Thread::create(fs_t1_p3,2048+512,1,NULL);
    while((!p1end)&(!p2end)&(!p3end)) Thread::yield();
    Thread::sleep(50);
    if(fs_1_error) fail("Concurrent write");
    //Testing file read
    char *buf=new char[1024];
    int i,j,k;
    FILE *f;
    //file_1.txt
    if((f=fopen("/testdir/file_1.txt","r"))==NULL)
        fail("can't open file_1.txt");
    setbuf(f,NULL);
    i=0;
    for(;;)
    {
        j=fread(buf,1,1024,f);
        if(j==0) break;
        i+=j;
        for(k=0;k<j;k++) if(buf[k]!='1')
            fail("read or write error on file_1.txt");
    }
    if(i!=512*512) fail("file_1.txt : size error");
    if(fclose(f)!=0) fail("Can't close file file_1.txt");
    //file_2.txt
    if((f=fopen("/testdir/file_2.txt","r"))==NULL)
        fail("can't open file_2.txt");
    setbuf(f,NULL);
    i=0;
    for(;;)
    {
        j=fread(buf,1,1024,f);
        if(j==0) break;
        i+=j;
        for(k=0;k<j;k++) if(buf[k]!='2')
            fail("read or write error on file_2.txt");
    }
    if(i!=512*512) fail("file_2.txt : size error");
    if(fclose(f)!=0) fail("Can't close file file_2.txt");
    //file_3.txt
    if((f=fopen("/testdir/file_3.txt","r"))==NULL)
        fail("can't open file_3.txt");
    setbuf(f,NULL);
    i=0;
    for(;;)
    {
        j=fread(buf,1,1024,f);
        if(j==0) break;
        i+=j;
        for(k=0;k<j;k++) if(buf[k]!='3')
            fail("read or write error on file_3.txt");
    }if(i!=512*512) fail("file_3.txt : size error");
    if(fclose(f)!=0) fail("Can't close file file_3.txt");
    delete[] buf;
    //Testing fprintf
    if((f=fopen("/testdir/file_4.txt","w"))==NULL)
        fail("can't open w file_4.txt");
    fprintf(f,"Hello world line 001\n");
    if(fclose(f)!=0) fail("Can't close w file_4.txt");
    //Testing append
    if((f=fopen("/testdir/file_4.txt","a"))==NULL)
        fail("can't open a file_4.txt");
    for(i=2;i<=128;i++) fprintf(f,"Hello world line %03d\n",i);
    if(fclose(f)!=0) fail("Can't close a file_4.txt");
    //Reading to check (only first 2 lines)
    if((f=fopen("/testdir/file_4.txt","r"))==NULL)
        fail("can't open r file_4.txt");
    char line[80];
    fgets(line,sizeof(line),f);
    if(strcmp(line,"Hello world line 001\n")) fail("file_4.txt line 1 error");
    fgets(line,sizeof(line),f);
    if(strcmp(line,"Hello world line 002\n")) fail("file_4.txt line 2 error");
    if(fclose(f)!=0) fail("Can't close r file_4.txt");
    //Test fseek and ftell. When reaching this point file_4.txt contains:
    //Hello world line 001\n
    //Hello world line 002\n
    //  ...
    //Hello world line 128\n
    if((f=fopen("/testdir/file_4.txt","r"))==NULL)
        fail("can't open r2 file_4.txt");
    if(ftell(f)!=0) fail("File opend but cursor not @ address 0");
    fseek(f,-4,SEEK_END);//Seek to 128\n
    if((fgetc(f)!='1')|(fgetc(f)!='2')|(fgetc(f)!='8')) fail("fgetc SEEK_END");
    if(ftell(f)!=(21*128-1))
    {
        iprintf("ftell=%ld\n",ftell(f));
        fail("ftell() 1");
    }
    fseek(f,21+17,SEEK_SET);//Seek to 002\n
    if((fgetc(f)!='0')|(fgetc(f)!='0')|(fgetc(f)!='2')|(fgetc(f)!='\n'))
        fail("fgetc SEEK_SET");
    if(ftell(f)!=(21*2))
    {
        iprintf("ftell=%ld\n",ftell(f));
        fail("ftell() 2");
    }
    fseek(f,21*50+17,SEEK_CUR);//Seek to 053\n
    if((fgetc(f)!='0')|(fgetc(f)!='5')|(fgetc(f)!='3')|(fgetc(f)!='\n'))
        fail("fgetc SEEK_CUR");
    if(ftell(f)!=(21*53))
    {
        iprintf("ftell=%ld\n",ftell(f));
        fail("ftell() 2");
    }
    if(fclose(f)!=0) fail("Can't close r2 file_4.txt");
    //Testing remove()
    if((f=fopen("/testdir/deleteme.txt","w"))==NULL)
        fail("can't open deleteme.txt");
    if(fclose(f)!=0) fail("Can't close deleteme.txt");
    remove("/testdir/deleteme.txt");
    if((f=fopen("/testdir/deleteme.txt","r"))!=NULL) fail("remove() error");
    pass();
}

//
// Filesystem test 2
//
/*
tests:
Directory::open()
Directtory::next()
Directory::exists()
*/

struct fs_2_file
{
    char filename[Directory::FILENAME_LEN];
    bool found;
};

static fs_2_file fs_2_names[4];

static void ls_check_valid(Directory& d, bool last)
{
    char line[Directory::FILENAME_LEN];
    unsigned int len;
    unsigned char a;
    if(last)
    {
        if(d.next(line,len,a)==true) fail("next() (0)");
    } else {
        if(d.next(line,len,a)==false) fail("next() (1)");
        if(len==0) fail("next() (2)");
        int i;
        for(i=0;i<4;i++)
        {
            if(!strcmp(line,fs_2_names[i].filename))
            {
                if(fs_2_names[i].found==true)
                {
                    fail("next() (2)");
                } else {
                    fs_2_names[i].found=true;
                    return;
                }
            }
        }
        fail("next() (3)");
    }
}

static void fs_test_2()
{
    test_name("Directory class");
    //Testing exists()
    if(Directory::exists("/testdir")==false) fail("exist() (1)");
    //Testing exist() with a nonexistent dirctory
    if(Directory::exists("/noexist")==true) fail("exist() (2)");
    Directory d;
    //Testing Directory::open()
    if(d.open("/testdir")!=0) fail("open() (1)");
    strcpy(fs_2_names[0].filename,"file_1.txt");
    fs_2_names[0].found=false;
    strcpy(fs_2_names[1].filename,"file_2.txt");
    fs_2_names[1].found=false;
    strcpy(fs_2_names[2].filename,"file_3.txt");
    fs_2_names[2].found=false;
    strcpy(fs_2_names[3].filename,"file_4.txt");
    fs_2_names[3].found=false;
    //Testing Directory::next()
    ls_check_valid(d,false);
    ls_check_valid(d,false);
    ls_check_valid(d,false);
    ls_check_valid(d,false);
    ls_check_valid(d,true);
    pass();
}

//
// Filesystem test 3
//
/*
tests:
is_mounted()
mount_filesystem()
umount_filesystem()
*/

static void fs_test_3()
{
    test_name("Filesystem mount/umount");
    //When starting, filesystem should be mounted
    Filesystem& fs=Filesystem::instance();
    if(fs.isMounted()==false) fail("is_mounted() (1)");
    fs.umount();
    if(fs.isMounted()==true) fail("umount_filesystem() (1)");
    if(fs.mount()!=0) fail("mount_filesystem() (1)");
    if(fs.isMounted()==false) fail("is_mounted() (2)");
    //Filesystem already mounted, try mounting again, should report error
    if(fs.mount()!=3) fail("mount_filesystem() (2)");
    pass();
}
#endif //WITH_FILESYSTEM

//
// Benchmark 1
//
/*
tests:
serial write speed
*/

static void benchmark_1()
{
    Timer t;
    t.start();
    extern char _data asm("_data");
    #ifdef _ARCH_CORTEXM3_STM32
    memDump(&_data,2048);
    t.stop();
    //every line dumps 16 bytes, and is 81 char long (considering \r\n)
    //so (2048/16)*81=10368
    iprintf("Time required to print 10368 char is %dms\n",(
            t.interval()*1000)/TICK_FREQ);
    unsigned int baudrate=10368*10000/((t.interval()*1000)/TICK_FREQ);
    iprintf("Effective baud rate =%u\n",baudrate);
    #else //_ARCH_CORTEXM3_STM32
    memDump(&_data,32768);
    t.stop();
    //every line dumps 16 bytes, and is 81 char long (considering \r\n)
    //so (32768/16)*81=165888
    iprintf("Time required to print 165888 char is %dms\n",(
            t.interval()*1000)/TICK_FREQ);
    unsigned int baudrate=165888*10000/((t.interval()*1000)/TICK_FREQ);
    iprintf("Effective baud rate =%u\n",baudrate);
    #endif //_ARCH_CORTEXM3_STM32
}

//
// Benchmark 2
//
/*
tests:
context switch speed
*/

static void b2_p1(void *argv)
{
    for(;;)
    {
        if(Thread::testTerminate()) break;
        Thread::yield();
    }
}

static int b2_f1()
{
    int i=0;
    Timer t;
    t.start();
    for(;;)
    {
        //Since calling interval() on a running timer is not allowed,
        //we need to make a copy of the timer and stop the copy.
        Timer k(t);
        k.stop();
        if((unsigned int)k.interval()>=TICK_FREQ) break;
        i+=2;
        Thread::yield();
    }
    t.stop();
    return i;
}

static void benchmark_2()
{
    //Test context switch time at maximum priority
    Thread::setPriority(3);//Using max priority
    Thread *p=Thread::create(b2_p1,STACK_MIN,3,NULL);
    int i=b2_f1();
    p->terminate();
    iprintf("%d context switch per second (max priority)\n",i);
    Thread::sleep(10);
    Thread::setPriority(0);//Restoring original priority
    p=Thread::create(b2_p1,STACK_MIN,0,NULL);
    i=b2_f1();
    p->terminate();
    iprintf("%d context switch per second (min priority)\n",i);
    Thread::sleep(10);
}

//
// Benchmark 3
//
/*
tests:
Fisesystem write speed and latency
makes a 1MB file and measures time required to read/write it.
*/

static void benchmark_3()
{
    //Write benchmark
    const char FILENAME[]="/speed.txt";
    const int BUFSIZE=1024;
    char *buf=new char[BUFSIZE];
    memset ((void*)buf,'0',BUFSIZE);
    FILE *f;
    if((f=fopen(FILENAME,"w"))==NULL)
    {
        iprintf("Filesystem write benchmark not made. Can't open file\n");
        delete[] buf;
        return;
    }
    setbuf(f,NULL);
    Timer total,part;
    int i,max=0;
    total.start();
    for(i=0;i<1024;i++)
    {
        part.start();
        fwrite(buf,BUFSIZE,1,f);
        part.stop();
        if(part.interval()>max) max=part.interval();
        part.clear();
    }
    total.stop();
    if(fclose(f)!=0) iprintf("Error in fclose 1\n");
    iprintf("Filesystem write benchmark\n");
    unsigned int writeTime=(total.interval()*1000)/TICK_FREQ;
    unsigned int writeSpeed=static_cast<unsigned int>(1024000.0/writeTime);
    iprintf("Total write time = %dms (%dKB/s)\n",writeTime,writeSpeed);
    iprintf("Max filesystem latency = %dms\n",(max*1000)/TICK_FREQ);
    //Read benchmark
    max=0;
    total.clear();
    if((f=fopen(FILENAME,"r"))==NULL)
    {
        iprintf("Filesystem read benchmark not made. Can't open file\n");
        delete[] buf;
        return;
    }
    setbuf(f,NULL);
    total.start();
    for(i=0;i<1024;i++)
    {
        part.start();
        fread(buf,BUFSIZE,1,f);
        part.stop();
        if(part.interval()>max) max=part.interval();
        part.clear();
        int j;
        for(j=0;j<BUFSIZE;j++) if(buf[j]!='0')
        {
            iprintf("Read error\n");
            goto quit;
        }
    }
    quit:
    total.stop();
    if(fclose(f)!=0) iprintf("Error in fclose 2\n");
    iprintf("Filesystem read test\n");
    unsigned int readTime=(total.interval()*1000)/TICK_FREQ;
    unsigned int readSpeed=static_cast<unsigned int>(1024000.0/readTime);
    iprintf("Total read time = %dms (%dKB/s)\n",readTime,readSpeed);
    iprintf("Max filesystem latency = %dms\n",(max*1000)/TICK_FREQ);
    delete[] buf;
}