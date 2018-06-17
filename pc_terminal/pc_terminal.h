/*------------------------------------------------------------------
 *  pc_terminal.h - defines, function prototypes
 *
 *  Samyuktha Sivaram
 *  Embedded Software Lab
 *
 *  Date -16.06.2018
 *------------------------------------------------------------------
 */

#ifndef PC_TERMINAL_H__
#define PC_TERMINAL_H__

#include <inttypes.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <sys/stat.h>
#include <termios.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#define NAME_LENGTH 128
#define POLY 0x8005
#define TELEMETRY
#include "joystick.h"

int16_t calc_crc(int frame[]);
int16_t crc_pc(int frame[],int count);
int check_crc(int8_t c1, int8_t c2, int q[]);
void rs232_pc(uint8_t c);
void log_data(int read_buffer[50]);

#endif // PC_TERMINAL_H__