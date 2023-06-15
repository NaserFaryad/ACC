#ifndef FILE_GPIO_H
#define FILE_GPIO_H
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bcm2835.h"

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define PIN  24 /* P1-18 */
#define POUT 4  /* P1-07 */

/* Relay GPIO*/
#define WARN_LED_PIN    15
#define SENSOR_OK_PIN   23
#define PWR_5V_OK_PIN   4
#define PWR_15V_OK_PIN  3
#define SQR_SIN_SEL_PIN 6
#define PWR_15V_SW_PIN  14

int GPIOExport(int pin);

int GPIOUnexport(int pin);

int GPIODirection(int pin, int dir);
int GPIORead(int pin);
int GPIOWrite(int pin, int value);

int relay_init();
void sensor_ok_on();
void sensor_ok_off();
void pwr_15v_ok_on();
void pwr_15v_ok_off();
void toggle_warning(int ms_delay);
void pwr_5v_ok_on();
void pwr_5v_ok_off();
void pwr_15v_on();
void pwr_15v_off();
void relays_off();


#ifdef __cplusplus
}
#endif
#endif // FILE_GPIO_H
