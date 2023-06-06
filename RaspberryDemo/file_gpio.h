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

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define PIN  24 /* P1-18 */
#define POUT 4  /* P1-07 */


int GPIOExport(int pin);

int GPIOUnexport(int pin);

int GPIODirection(int pin, int dir);
int GPIORead(int pin);
int GPIOWrite(int pin, int value);


#ifdef __cplusplus
}
#endif
#endif // FILE_GPIO_H
