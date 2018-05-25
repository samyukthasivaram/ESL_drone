#include<stdio.h>
#include<math.h>
#include<stdint.h>
#include"in4073.h"




#define NUM_DATA 40
#define MAX_ADDR   0xFFFFFFFF
#define MIN_ADDR   0x00000000

struct read_data
{
	float time;
	int mode;
	int incoming;
	float phi, theta, psi;
	float sax, say, saz, sp, sq, sr;
	float pressure, temperature;
	float ae[4];

}r;





void update_data(uint8_t *data_buffer)
{   
	
   //flash_chip_erase()
	uint32_t system_time;
	system_time = get_time_us();
	printf("update  data in flash.\n");
	//system time,4 bytes
	data_buffer[0] = (system_time >> 24) & 0xFF;
	data_buffer[1] = (system_time >> 16) & 0xFF;
	data_buffer[2] = (system_time >> 8) & 0xFF;
	data_buffer[3] = system_time  & 0xFF;
	//system_mode
	data_buffer[4] = mode & 0xFF;
	//incoming joystick/keyboard
	data_buffer[5] =0; //?????? & 0xFF;
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


	/*intermediate data from the sensor signal processing chain?????????????*/
	printf("update data successful\n");
}

void save_data_in_flash() 
{ 	uint8_t data_buffer[45];
	static uint32_t addr;
	printf("save data in flash.\n");
	update_data(data_buffer);
		if (addr == MAX_ADDR)
		{
			printf("The flash is full and can not write data in it.\n");
			if (flash_chip_erase()) 
			{
				addr = MIN_ADDR;
			}
			else 
			{
				printf("flash_chip_erase:error");
			}
		}
	
	if (addr += NUM_DATA > MAX_ADDR)
	{
		printf("can not write all the data in flash");
		if (flash_chip_erase())
		{
			addr = MIN_ADDR;
		}
		else
		{
			printf("flash_chip_erase:error");
		}

	}else
	{
		if(flash_write_bytes(addr, data_buffer, NUM_DATA))
		{
			addr = addr + NUM_DATA;
		}
		else 
		{
			printf("Flash write fail.");
		}
	}
}


void read_from_flash()//read and save 
{	uint8_t read_buffer[45];
	static uint32_t addr;
	printf("read data in flash.\n");
	//uint32_t r_time,r_pressure,r_temperature;
	//uint8_t r_mode, r_incoming;//incoming: joystick or keyboard
	//uint16_t r_phi, r_theta, r_psi, r_sax, r_say, r_saz, r_sp, r_sq, r_sr;
	//uint16_t r_ae[4];
	//struct read_data r;
	if (addr += NUM_DATA > MAX_ADDR) 
	{
		printf("the address is our of bounds ");
		addr = 0x00000000;
	}
	else 
	{
		if (!flash_read_bytes(addr, read_buffer, NUM_DATA) )
		{
			printf("read flash failed");
		}
		else 
		{
			r.time = (read_buffer[0] << 24) + (read_buffer[1] << 16) + (read_buffer[2] << 8) + read_buffer[3];
			r.mode = read_buffer[4];
			r.incoming = read_buffer[5];
			r.phi = (read_buffer[6] << 8 )+ read_buffer[7];
			r.theta = (read_buffer[8] << 8) + read_buffer[9];
			r.psi = (read_buffer[10] << 8) + read_buffer[11];
			r.sax = (read_buffer[12] << 8 )+ read_buffer[13];
			r.say = (read_buffer[14] << 8 )+ read_buffer[15];
			r.saz = (read_buffer[16] << 8 )+ read_buffer[17];
			r.sp= (read_buffer[18]<< 8) + read_buffer[19];
			r.sq = (read_buffer[20] << 8) + read_buffer[21];
			r.sr = (read_buffer[22] << 8) + read_buffer[23];
			r.pressure = (read_buffer[24] << 24 )+ (read_buffer[25] << 16) + (read_buffer[26] << 8) + read_buffer[27];
			r.temperature = (read_buffer[28] << 24) +( read_buffer[29] << 16) + (read_buffer[30] << 8 )+ read_buffer[31];
			r.ae[0] = (read_buffer[32] << 8 )+ read_buffer[33];
			r.ae[1] = (read_buffer[34] << 8) + read_buffer[35];
			r.ae[2] = (read_buffer[36] << 8) + read_buffer[37];
			r.ae[3] =( read_buffer[38] << 8 )+ read_buffer[39];
		
			/*FILE *fp;
			fp = fopen("data.txt", "w+");
			if (!fp) 
			{
				printf("creat file error");
			}
			else 
			{
				fwrite(&r, sizeof(r), 1, fp);
				fclose(fp);
			}
			*/
		printf("%d",read_buffer[30]);
		
		
		}
		addr += NUM_DATA;
		printf("read data successful");
		//
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

