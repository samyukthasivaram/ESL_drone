#include <in4073.h>
//#include "pc_terminal.c"
#include <RS232.h>
#include <fcntl.h>
#include <time.h>
//#include "js.c"
#include <stdint.h>
#include <inttypes.h>
//#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
//#include <termios.h>
#include <sys/stat.h>

int rs232_getchar();
int rs232_putchar(char c);
int rs232_getchar_nb();




static volatile bool read1 = false,write1 = false;

void rs232_init(void)
{
	uart_init();
	write1 = false;
	read1 = false;
}

/*---------------------------------------------------------
 *Data format: | StartByte | Data | CRC1 | CRC2 | 14 bytes
  ---------------------------------------------------------
 */
unsigned int cal_crc(unsigned int *ptr, unsigned int len) {
 unsigned int crc;
 unsigned char da;
 unsigned int crc_ta[16]={ 
 0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
 };
 crc=0;
 while(len--!=0) {
 da=((unsigned int)(crc/256))/16; 
 crc<<=4; 
 crc^=crc_ta[da^(*ptr/16)]; 
 da=((unsigned char)(crc/256))/16; 
 crc<<=4; 
 crc^=crc_ta[da^(*ptr&0x0f)];
 ptr++;
 }
 return(crc);
}

bool check_crc(int c1, int c2, queue q)
{
   unsigned int w= cal_crc(q,10);
   unsigned int new_c = (c2>>8)|c1;
  if (~w&new_c)
     return false;
  else
    return true;
}

bool rs232_read(int p[])
{
	static int state = 0;
	int data = 0;
	int i=0,j = 0;
	int check1 = 0,check2 = 0;	//store CRC
	queue *rx_buffer;
	queue *rx_wrong;	//store data when CRC result goes wrong
	int count;

	init_queue(rx_buffer);
	*p = [0];
	
	while(1)
	{
		data = rs232_getchar();
		switch(state)
		{
			case 0:
				if(data == StartByte)
				{
					init_queue(rx_buffer);
					rx_buffer->last = DataLen;
					read1 = true;
					state = 1;
				}
				else	state = 0;
				break;
				
			case 1:
				if(rx_buffer.count >=DataLen)
				{
					check1 = data;
					rx_buffer->count = 0;
					state = 2;
				}
				else
				{
					rx_buffer->Data[rx_buffer->count] = data;
					rx_buffer->count++;
					state = 1;
				}
				break;

			case 2:
				check2 = data;
				if(check_CRC(check1, check2, rx_buffer))
				{
					read1 = false;
					store_data(p[], rx_buffer);
					state = 0;
					usleep(10000);
				}
				else
				{
					init_queue(rx_buffer);

					if(check1 == StartByte)
					{
						rx_buffer->Data[0] = check2;
						rx_buffer->count = 1;
						state = 1;
					}
					else if(check2 == StartByte)
					{
						rx_buffer->count = 0;
						state = 1;
					}
					else
					{
						for(i=0; i<DataLen; i++)
						{
							if(rx_buffer->Data[i] == StartByte)
							{
								rx_buffer->count = i + 1;
								count = DataLen + 1 - i;
								init_queue(rx_wrong);
								for(j=0; j<count-1; j++)
									rx_wrong->Data[j] = rx_buffer->Data[i+1+j];
								rx_wrong->Data[count] = check1;
								rx_wrong->Data[count+1] = check2;
								state = 1;
							}
							else	state = 0;
						}
					}
				}

				break;
			
			default: break;
		}
	}
}


int store_data(int p[DataSize], queue *q)
{
	for(i=0; i<DataSize; i++)
		p[i] = q->Data[i];
	return p;
}

int fd_RS232;
void rs232_open(void)
{
  	char 		*name;
  	int 		result;
  	struct termios	tty;

       	fd_RS232 = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);  // Hardcode your serial port here, or request it as an argument at runtime

	assert(fd_RS232>=0);

  	result = isatty(fd_RS232);
  	assert(result == 1);

  	name = ttyname(fd_RS232);
  	assert(name != 0);

  	result = tcgetattr(fd_RS232, &tty);
	assert(result == 0);

	tty.c_iflag = IGNBRK; /* ignore break condition */
	tty.c_oflag = 0;
	tty.c_lflag = 0;

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; /* 8 bits-per-character */
	tty.c_cflag |= CLOCAL | CREAD; /* Ignore model status + read input */

	cfsetospeed(&tty, B115200);
	cfsetispeed(&tty, B115200);

	tty.c_cc[VMIN]  = 0;
	tty.c_cc[VTIME] = 1; // added timeout

	tty.c_iflag &= ~(IXON|IXOFF|IXANY);

	result = tcsetattr (fd_RS232, TCSANOW, &tty); /* non-canonical */

	tcflush(fd_RS232, TCIOFLUSH); /* flush I/O buffer */
}


void 	rs232_close(void)
{
  	int 	result;

  	result = close(fd_RS232);
  	assert (result==0);
}


int	rs232_getchar_nb()
{
	int 		result;
	unsigned char 	c;

	result = read(fd_RS232, &c, 1);

	if (result == 0)
		return -1;

	else
	{
		assert(result == 1);
		return (int) c;
	}
}


int 	rs232_getchar()
{
	int 	c;

	while ((c = rs232_getchar_nb()) == -1)
		;
	return c;
}


int 	rs232_putchar(char c)
{
	int result;

	do {
		result = (int) write(fd_RS232, &c, 1);
	} while (result == 0);

	assert(result == 1);
	return result;
}

/*
void Data_logging(log l, int p[DataLen])
{
	if(fp == NULL)
		return -1;
	else
	{
		log.Mode = p[0];
		log.Joystick[0] = p[1];
		log.Joystick[1] = p[2];
		log.Joystick[2] = p[3];
		log.Joystick[3] = p[4];
		log.Joystick[4] = p[5];
		log.Joystick[5] = p[6];
		log.Joystick[6] = p[7];
		log.Joystick[7] = p[8];
		log.Keyboard = p[9];
		fprintf(fp, "%d      %d      ", mon_time_ms(), log.Mode);
		for(i=0; i<DataLen; i++)
			fprintf(fp, "%d ", log.Joystick[i]);
		fprintf(fp, "%d      %d      %d      %d      %d\n", \
			log.Keyboard, log.Actuator, log.Sensor, log.Sensor_processing_chain, log.controller);
	}
}
*/
