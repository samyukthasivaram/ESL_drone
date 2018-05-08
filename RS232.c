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
 *Data format: | SYNC(header) | StartByte | Data | CRC1 | CRC2 | StopByte | 
  ---------------------------------------------------------
 */

bool rs232_read(int p[DataSize])
{
	static int state = 0;
	int i = 0;
	int data = 0;
	int check1 = check2 = 0;	//store CRC
	queue rx_queue;
	queue rx_buffer;
	
	init_queue(rx_queue);
	init_queue(rx_buffer);
	p = [0];
	
	for(i=0; i<QUEUE_SIZE; i++)
	{
		rx_queue.Data[i] = rs232_getchar();
		data = rx_queue.Data[i];
		switch(state)
		{
			case 0:
				if(data == StartByte)
				{
					init_queue(rx_queue);
					init_queue(rx_buffer);
					rx_buffer.last = DataLen;
					read = true;
					state = 1;
				}
				else	state = 0;
				break;
				
			case 1:
				if(data == CRC1)	
				{
					if(rx_buffer.count<rx_buffer.last)	state = 1;
					else
					{
						state = 2;
						check = data;
					}
				}
				else
				{
					rx_buffer.Data[rx_buffer.count] = data;
					rx_buffer.count++;
				}
				break;

			case 2:
				if(data == CRC2)
				{
					check2 = data;
					if(!check_CRC(check1, check2))		state = 3;
					else	state = 0;
				}
				break;
				
			case 3:
				if(data == StopByte)
				{
					store_data(p, rx_buffer);
					read = false;
					state = 0;
					return 0;
				}
				else	state = 0;
				break;
			
			default: break;
		}
	}
}

bool check_CRC(int c1, int c2)

Param store_data(int p[DataSize], queue q)
{
	for(i=0; i<DataSize; i++)
		p[i] = q.Data[i];
	return p;
}

