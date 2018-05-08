#include <in4073.h>
#include "pc_terminal.c"
#include <RS232.h>

static volatile bool read = false;

unsigned cal_crc(unsigned int *ptr, unsigned int len) {
 unsigned int crc;
 unsigned char da;
 unsigned int crc_ta[16]={ 
 0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
 }
 crc=0;
 while(len--!=0) {
 da=((uint)(crc/256))/16; 
 crc<<=4; 
 crc^=crc_ta[da^(*ptr/16)]; 
 da=((uchar)(crc/256))/16; 
 crc<<=4; 
 crc^=crc_ta[da^(*ptr&0x0f)];
 ptr++;
 }
 return(crc);
}

bool checl_crc(int c1, int c2, queue q, int length)
{
   w= cal_crc(q,length);
   new_c = (c2>>8)|c1;
  if (~w&new_c)
     return false;
  else
    return true;
}

void rs232_init(void)
{
	uart_init();
	write = false;
	read = false;
}

/*---------------------------------------------------------
 *Data format: | SYNC(header) | StartByte | Data | CRC1 | CRC2 | StopByte | 
  ---------------------------------------------------------
 */

bool rs232_read(int p[DataSize])
{
	static int state = 0;
	int data = 0;
	int check1 = check2 = 0;	//store CRC
	queue rx_buffer;

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
					rx_buffer.last = DataLen;
					read = true;
					state = 1;
				}
				else	state = 0;
				break;
				
			case 1:
				if(rx_buffer.count >=DataLen)
				{
					check1 = data;
					rx_buffer.count = 0;
					state = 2;
				}
				else
				{
					rx_buffer.Data[rx_buffer.count] = data;
					rx_buffer.count++;
					state = 1;
				}
				break;
				
			case 2:
				check2 = data;
				if(check_CRC(check1, check2, rx_buffer))
				{
					read = false;
					store_data(p[], rx_buffer);
					state = 0;
					usleep(10000);
				}
				else state = 0;
				break;
			
			default: break;
		}
	}
}



int store_data(int p[DataSize], queue q)
{
	for(i=0; i<DataSize; i++)
		p[i] = q.Data[i];
	return p;
}

