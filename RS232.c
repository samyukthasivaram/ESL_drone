#include <in4073.h>
#include "pc_terminal.c"
#include <RS232.h>

static volatile bool read = false;

void rs232_init(void)
{
	uart_init();
	write = false;
	read = false;
}

/*---------------------------------------------------------
 *Data format: | StartByte | Data | CRC1 | CRC2 | 14 bytes
  ---------------------------------------------------------
 */
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

bool check_crc(int c1, int c2, queue q)
{
   w= cal_crc(q,10);
   new_c = (c2>>8)|c1;
  if (~w&new_c)
     return false;
  else
    return true;
}

bool rs232_read(int p[DataSize])
{
	static int state = 0;
	int data = 0;
	int i = j = 0;
	int check1 = check2 = 0;	//store CRC
	queue rx_buffer;
	queue rx_wrong;	//store data when CRC result goes wrong
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
				else
				{
					init_queue(rx_buffer);

					if(check1 == StartByte)
					{
						rx_buffer.Data[0] = check2;
						rx_buffer.count = 1;
						state = 1;
					}
					else if(check2 == StartByte)
					{
						rx_buffer.count = 0;
						state = 1;
					}
					else
					{
						for(i=0; i<DataLen; i++)
						{
							if(rx_buffer.Data[i] == StartByte)
							{
								rx_buffer.count = i + 1;
								count = DataLen + 1 - i;
								init_queue(rx_wrong);
								for(j=0; j<count; j++)
									rx_wrong.Data[j] = rx_buffer.Data[i+1+j];
								rx_wrong.Data[count] = check1;
								rx_wrong.Data[count+1] = check2;
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


int store_data(int p[DataSize], queue q)
{
	for(i=0; i<DataSize; i++)
		p[i] = q.Data[i];
	return p;
}

