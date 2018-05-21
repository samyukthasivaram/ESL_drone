#include "in4073.h"
#include "RS232.h"
#include "crc.h"
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#define DataSize 10
#define POLY 0x8005


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
/*
//CRC calculation with table
uint16_t cal_crc(unsigned int *ptr, unsigned int len) {
 uint16_t crc;
 unsigned int da;
 static const unsigned int crc_ta[16]={ 
 0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
 };
 crc=0;
 while(len--!=0) {
 da=((uint8_t)(crc/256))/16; 
 crc<<=4; 
 crc^=crc_ta[da^(*ptr/16)]; 
 da=((uint8_t)(crc/256))/16; 
 crc<<=4; 
 crc^=crc_ta[da^(*ptr&0x0f)];
 ptr++;
 }
 return(crc);
} */

//CRC calculation without table
int16_t calc_crc(int frame[])

{
    int16_t crc,byte;
    crc = 0;
    
    for (int i=0; i< 10; i++)
    {
            byte=frame[i];
            crc ^=byte<<8;
	    for(int k=8;k>0;k--)
		{if(crc & 0x8000)
			crc^=POLY;
		 crc=crc<<1;
		}
    
    }
    return crc;
}
bool check_crc(int8_t c1, int8_t c2, int q[])
{
   int16_t w= calc_crc(q);
   int16_t new_c = ((c2<<8) & 0xFF00) | (c1 & 0x00FF);
//printf("w=%d",w);
//printf("new crc=%d|%d|%d\n",new_c,c1,c2);
  if (w==new_c)

     return true;
  else
    return false;
}
int frame[DataSize];
uint8_t size=0;
uint8_t count=0;
int rx_wrong[10];
int8_t StartByte=0xFF;
void rs232_read()
{
	static int state = 0;
	int8_t data = 0;
	int i=0,j = 0;
	static int8_t check1 = 0,check2 = 0;	//store CRC
	//uint16_t crc=0;
	//queue *rx_buffer;
	//int rx_wrong[13];	//store data when CRC result goes wrong <TO DO> 
	//int p[DataSize];
	
		if(rx_queue.count>0)
	{		data = dequeue(&rx_queue); 
			//printf("%d|  ",data);
		switch(state)
		{
			case 0:
				//printf("CASE0");
				if(data == StartByte)
				{
					read1 = true;
					size=0;
					state = 1;
				}
				else	state = 0;
				break;
				
			case 1:
				//printf("CASE1");
				if(count >=DataSize)
				{
					check1 = data;
					count = 0;
					state = 2;
				}
				else
				{
					frame[count] = data;
					count++;
					//printf("count=%d \t \t \t \n",count);
					state = 1;
				}
				break;
			

			case 2:
				//printf("CASE2");
				check2 = data;
				//printf("check1=%d",check1);
				if(check_crc(check1,check2,frame))//check CRC <TO DO>
				{
					//printf("CRC right");
					read1 = false;
					state = 0;
					store_data(frame);
					//usleep(100000);
				}
				else    //if CRC is wrong check for start byte to start over
				{    

					printf("CRC wrong");

					if(check1 == StartByte)
					{
						frame[0] = check2;
						count = 1;
						state = 1;
						//printf("check1 == StartByte");
					}
					else if(check2 == StartByte)
					{
						count = 0;
						state = 1;
						//printf("check2 == StartByte");
					}
					else
					{	
						//printf("data == StartByte");
						for(i=0; i<10; i++)  //DataLen=12 (?)
						{
							if(frame[i] == StartByte)
							{
								int k=0;
								for(j=i+1; j<10; j++)
									rx_wrong[k++] = frame[j];
								rx_wrong[k++] = check1;
								rx_wrong[k++] = check2;
								state = 1;
								for(j=0; j<k; j++)
									frame[j]=rx_wrong[j];
							}
							else	state = 0;
							break;
						}
					}
				}

				break;
			
			default: printf("deafult case");
					break;
		}
	}
}


void store_data(int p[DataSize])
{	//int16_t temp_1, temp_2;
	//int16_t temp_3;
for(int k=0; k<10; k++)
	{
			//tx_buffer[k]=0xFF;
			//rs232_putchar(tx_buffer[k]);
			//printf("tx_buffer[%d]=%d | ",k,p[k]);
	}
//if(mode!=p[0])
printf("%d \n",keyboard);
	mode= p[0];
	
	lift = (p[2]<<8)+p[1];

	roll = (p[4]<<8)+p[3];
	pitch = (p[6]<<8)+p[5];
	
	yaw = (p[8]<<8)+p[7];
	
	keyboard = p[9];

//for(int ii=0;ii<DataSize;ii++) 
 
manual_mode_withoutsqrt();
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
