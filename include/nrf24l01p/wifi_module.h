/**
 * File:   wifi_module.h
 *  \brief     This is the header file for the file wifi_module.cpp.
 *  \author    Andrea Corna
 *  \author    Lorenzo Fontana
 *  \version   1.0
 *  \date      03/01/2014
 */

#ifndef WIFI_MODULE_H
#define	WIFI_MODULE_H

/**
 * Method created in order to send data with the wireless module. Max lenght of payload should be
 * 32 bytes the other ones will be truncated.
 * @param payload - payload to be sent
 */
void send(char *payload);

/**
 * Function executed by a thread in order to receive data from other boards.
 * @param arg - useless parameter
 * @return 
 */
void *wifi_receive(void *arg);

/**
 * Function excutes  by a thread in order to send data to other boards.
 * @param arg - useless parameter
 * @return 
 */
void *wifi_transmit(void *arg);

/**
 * Function in order to receive data from module's buffer. 
 * @param payload - char array with size equals to 960
 */
void receive(char *payload);

/**
 * Function which setup GPIO led to debug, powers up the wireless module
 * and set it into the recive mode.
 */
void wifi_init();
#endif	/* WIFI_MODULE_H */

