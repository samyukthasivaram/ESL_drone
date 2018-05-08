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
				if(rx_buffer.count >=DataLen)	state = 2;
				else
				{
					rx_buffer.Data[rx_buffer.count] = data;
					rx_buffer.count++;
					state = 1;
				}
				break;

			case 2:
				check1 = data;
				state = 3;
				break;
				
			case 3:
				check2 = data;
				if(check_CRC(check1, check2, rx_buffer))
				{
					read = false;
					store_data(p[], rx_buffer);
					state = 0;
				}
				else state = 0;
				break;
			
			default: break;
		}
	}
}

bool check_CRC(int c1, int c2, queue q)

int store_data(int p[DataSize], queue q)
{
	for(i=0; i<DataSize; i++)
		p[i] = q.Data[i];
	return p;
}

