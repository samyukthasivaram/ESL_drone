/*------------------------------------------------------------------
 *  RS232.h -- defines, globals, function prototypes
 *
 *  Samyuktha Sivaram
 *  Embedded Software Lab
 *
 *  16.06.2018
 *------------------------------------------------------------------
 */

#ifndef _RS232_H_
#define _RS232_H_

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#define POLY 0x8005
#define DataLen 10

void rs232_write();
void rs232_init(void);
void rs232_read();
bool check_CRC(int8_t c1, int8_t c2, int8_t q[]);
void store_data(int8_t p[DataLen]);
int16_t calc_crc(int8_t frame[],int count,int start);

#endif
