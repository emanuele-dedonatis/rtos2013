#Pedometer for STM32F4

This project is a part of a collaborative [project for the class of Real-Time Operating Systems](http://home.deib.polimi.it/bellasi/lib/exe/fetch.php?media=teaching:2013:201311_rtos_projectpresentation.pdf "RTOS Project Presentation") at Polytechnic University of Milan. The goal of this project is to develop a pedometer using the MEMS motion sensor on the STM32F4DISCOVERY board.
The device provides the user with statistics about its training activity.

Download [Report.pdf](https://github.com/emanuele-dedonatis/rtos2013-report/blob/grp12-report/Report.pdf?raw=true)

##Features
+ STM32 F4 Discovery
⋅⋅[Product Page]("http://www.st.com/web/catalog/tools/FM116/SC959/SS1532/PF252419")
⋅⋅32-bit ARM Cortex-M4F core

+ LIS302DL ST MEMS
⋅⋅[Datasheet]("http://www.st.com/web/catalog/sense_power/FM89/SC444/PF152913")
⋅⋅3-axis accelerometer

+ MIOSIX embedded OS
⋅⋅[http://miosix.org]("http://miosix.org")
⋅⋅Open source POSIX compliant solution C++ coded for optimal performances

##Goals
+ Count daily number of steps
+ Recognize training mode (STEADY/WALK/RUN)
+ Calculate approximately distance and speed
+ Calculate burned calories
