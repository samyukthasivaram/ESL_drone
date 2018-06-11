#include<stdio.h>
#include<math.h>
#include<stdint.h>
#include"in4073.h"




#define NUM_DATA 50
#define MAX_ADDR   0x1FFF
#define MIN_ADDR   0x0000

/*struct read_data
{
	float time;
	int mode;
	int incoming;
	float phi, theta, psi;
	float sax, say, saz, sp, sq, sr;
	float pressure, temperature;
	float ae[4];

};*/


	static uint32_t addr=MIN_ADDR;


void update_data()
{   
	
   //flash_chip_erase()
	uint32_t system_time;
	system_time = get_time_us();
	//system time,4 bytes
	data_buffer[0] = (system_time >> 24) & 0xFF;
	data_buffer[1] = (system_time >> 16) & 0xFF;
	data_buffer[2] = (system_time >> 8) & 0xFF;
	data_buffer[3] = system_time  & 0xFF;
	//system_mode
	data_buffer[4] = mode & 0xFF;
	//incoming joystick/keyboard
	data_buffer[5] =keyboard;
	//actuator data phi,theta,psi
	data_buffer[6] = (phi >> 8) & 0xFF;
	data_buffer[7] = phi & 0xFF;
	data_buffer[8] = (theta >> 8) & 0xFF;
	data_buffer[9] = theta & 0xFF;
	data_buffer[10] = (psi >> 8) & 0xFF;
	data_buffer[11] = psi & 0xFF;
	//sensor data sax,say,sp,sq,sr
	data_buffer[12] = (sax >> 8) & 0xFF;
	data_buffer[13] = sax & 0xFF;
	data_buffer[14] = (say >> 8) & 0xFF;
	data_buffer[15] = say & 0xFF;
	data_buffer[16] = (saz >> 8) & 0xFF;
	data_buffer[17] = saz & 0xFF;
	data_buffer[18] = (sp >> 8) & 0xFF;
	data_buffer[19] = sp & 0xFF;
	data_buffer[20] = (sq >> 8) & 0xFF;
	data_buffer[21] = sq & 0xFF;
	data_buffer[22] = (sr >> 8) & 0xFF;
	data_buffer[23] = sr & 0xFF;
	//pressure and temperature
	data_buffer[24] = (pressure >> 24) & 0xFF;
	data_buffer[25] = (pressure >> 16) & 0xFF;
	data_buffer[26] = (pressure >> 8) & 0xFF;
	data_buffer[27] = pressure & 0xFF;

	data_buffer[28] = (temperature >> 24) & 0xFF;
	data_buffer[29] = (temperature>> 16) & 0xFF;
	data_buffer[30] = (temperature >> 8) & 0xFF;
	data_buffer[31] = temperature & 0xFF;
	//relevant controller data
	data_buffer[32] = (ae[0] >> 8) & 0xFF;
	data_buffer[33] = ae[0] & 0xFF;
	data_buffer[34] = (ae[1] >> 8) & 0xFF;
	data_buffer[35] = ae[1] & 0xFF;
	data_buffer[36] = (ae[2] >> 8) & 0xFF;
	data_buffer[37] = ae[2] & 0xFF;
	data_buffer[38] = (ae[3] >> 8) & 0xFF;
	data_buffer[39] = ae[3] & 0xFF;
	data_buffer[40] = (lift >> 8) & 0xFF;
	data_buffer[41] = lift & 0xFF;
	data_buffer[42] = (roll >> 8) & 0xFF;
	data_buffer[43] = roll & 0xFF;
	data_buffer[44] = (pitch >> 8) & 0xFF;
	data_buffer[45] = pitch & 0xFF;
	data_buffer[46] = (yaw >> 8) & 0xFF;
	data_buffer[47] = yaw & 0xFF;
	data_buffer[48] = p_yaw;
	data_buffer[49] = P1;
	data_buffer[50] = P2;

	//printf("update data successful\n");
}

void save_data_in_flash() 
{ 

	update_data();
		if (addr == MAX_ADDR)
		{
			printf("\nThe flash is full and can not write data in it.\n");
			if (flash_chip_erase()) 
			{
				addr = MIN_ADDR;
				flag_logging=0;
				read_from_flash();	
			}
			else 
			{
				printf("\nflash_chip_erase:error");
			}
		}
	
	if (addr += NUM_DATA > MAX_ADDR)
	{
		printf("\ncan not write all the data in flash");
		if (flash_chip_erase())
		{
			addr = MIN_ADDR;
		}
		else
		{
			printf("\nflash_chip_erase:error");
		}

	}else
	{
		if(flash_write_bytes(addr, data_buffer, NUM_DATA))
		{
			addr = addr + NUM_DATA;printf("\nwrite success");
		}
		else 
		{
			printf("\nFlash write fail.");
		}
	}
}


void read_from_flash()//read and save 
{
	int start_byte=0xFB;
	//static uint32_t addr;
	//struct read_data r;
	while ((addr -NUM_DATA) > MIN_ADDR) 
	{
		
	
		if (!flash_read_bytes(addr, read_buffer, NUM_DATA) )
		{
			printf("read flash failed");
		}
		else 
		{
			uart_put(start_byte);	
			for(int i=0; i<50; i++)
			uart_put(read_buffer[i]);		
		}
		
		addr -= NUM_DATA;
		printf("read data successful");
		
	}

}

bool flash_clear() 
{
	if (flash_chip_erase())
	{
		printf("The flash is cleared.");
			return true;
	}
	else 
	{
		return false;
	}
}
