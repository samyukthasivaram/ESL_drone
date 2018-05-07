#include <in4073.h>
#include "pc_terminal.c"
#include <RS232.h>

static volatile bool read = false;
queue rx_queue;
queue rx_buffer;

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

void rs232_read(Param p, int c)
{
	static int state = 0;
	int i = 0;
	int data = 0;
	int check1 = check2 = 0;	//store CRC1
	
	init_queue(rx_queue);
	init_queue(rx_buffer);
	init_queue(rx_param);
	p = {0};
	
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
						check1 = data;
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
					if(p.count>=size)	p = {0};
					store_data(p, rx_buffer, p.count);
					p.count++;
					read = false;
					state = 0;
				}
				else	state = 0;
				break;
			
			default: break;
		}
	}
}

bool check_CRC(int c1, int c2)

Param store_data(Param p, queue q, int i)
{
	p.Mode[i] = q.Data[0];
	p.Lift[i] = 0xF0 & q.Data[1] + 0x0F & q.Data[2];
	p.Roll[i] = 0xF0 & q.Data[3] + 0x0F & q.Data[4];
	p.Pitch[i] = 0xF0 & q.Data[5] + 0x0F & q.Data[6];
	p.Yaw[i] = 0xF0 & q.Data[7] + 0x0F & q.Data[8];
	p.Keyboard[i] = q.Data[9];
	return p;
}

