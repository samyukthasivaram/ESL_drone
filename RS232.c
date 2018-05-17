#include <in4073.h>
#include <RS232.h>
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

#define DataSize 14
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
}

//CRC calculation without table
uint16_t calc_crc(int frame[],int DLEN)

{
    uint16_t crc,byte;
    crc = 0;
    
    for (int i=1; i< DLEN+1; i++)
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
bool check_crc(int c1, int c2, queue *q)
{
   unsigned int w= cal_crc(q,10);
   unsigned int new_c = (c2>>8)|c1;
  if (~w&new_c)
     return false;
  else
    return true;
}*/
uint8_t frame[DataSize];
uint8_t size=0;
void rs232_read()
{
	static int state = 0;
	int8_t data = 0;
	int i=0,j = 0;
	int check1 = 0,check2 = 0;	//store CRC
	queue *rx_buffer;
	queue *rx_wrong;	//store data when CRC result goes wrong <TO DO> 
	int count;
	int p[DataSize];
	
	while(1)
	{
		if(rx_queue.count>0)
	{		data = dequeue(&rx_queue); 
		switch(state)
		{
			case 0:
				if(data == StartByte)
				{
					read1 = true;
					size=0;
					state = 1;
				}
				else	state = 0;
				break;
				
			case 1:
				check1=data;
				state=2;

			case 2:
				check2 = data;
				if(1==1)//check CRC <TO DO>
				{
					read1 = false;
					state = 0;
					frame[size++]=data;
					for(int ii=0;ii<DataSize;ii++) { printf("0x%02X ", p[ii]);}
					//usleep(10000);
				}
				else    //if CRC is wrong check for start byte to start over
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
}}


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
