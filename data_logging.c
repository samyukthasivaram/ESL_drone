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



	/*intermediate data from the sensor sign(saz >> 8) & 0xFF;
	data_buffer[17] = saz & 0xFF;
	data_buffer[18] = (sp >> 8) & 0xFF;
	data_buffer[19] = sp & 0xFF;
	data_buffer[20] = (sq >> 8) & 0xFF;
	data_buffer[21] = sq & 0xFF;
	data_buffer[22] = (sr >> 8) & 0xFF;
	data_bufal processing chain?????????????*/
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


void read_from_flash(uint8_t *read_buffer)//read and save 
{
	//static uint32_t addr;
	uint32_t r_time=0,r_pressure=0,r_temperature=0;
	int8_t r_mode=0, r_incoming=0;//incoming: joystick or keyboard
	int16_t r_phi=0, r_theta=0, r_psi=0, r_sax=0, r_say=0, r_saz=0, r_sp=0, r_sq=0, r_sr=0, r_lift=0,r_roll=0,r_pitch=0,r_yaw=0;
	int16_t r_ae[4]={0};
	//struct read_data r;
	while ((addr -NUM_DATA) > MIN_ADDR) 
	{
		
	
		if (!flash_read_bytes(addr, read_buffer, NUM_DATA) )
		{
			printf("read flash failed");
		}
		else 
		{
			r_time = (read_buffer[0] << 24) + (read_buffer[1] << 16) + (read_buffer[2] << 8) + read_buffer[3];
			r_mode = read_buffer[4];
			r_incoming = read_buffer[5];
			r_phi = (read_buffer[6] << 8 )+ read_buffer[7];
			r_theta = (read_buffer[8] << 8) + read_buffer[9];
			r_psi = (read_buffer[10] << 8 )+ read_buffer[11];
			r_sax = (read_buffer[12] << 8 )+ read_buffer[13];
			r_say =( read_buffer[14] << 8) + read_buffer[15];
			r_saz = (read_buffer[16] << 8 )+ read_buffer[17];
			r_sp= (read_buffer[18]<< 8) + read_buffer[19];
			r_sq = (read_buffer[20] << 8 )+ read_buffer[21];
			r_sr = (read_buffer[22] << 8) + read_buffer[23];
			r_pressure = (read_buffer[24] << 24) + (read_buffer[25] << 16) + (read_buffer[26] << 8) + read_buffer[27];
			r_temperature = (read_buffer[28] << 24) + (read_buffer[29] << 16) + (read_buffer[30] << 8 )+ read_buffer[31];
			r_ae[0] = (read_buffer[32] << 8) + read_buffer[33];
			r_ae[1] = (read_buffer[34] << 8) + read_buffer[35];
			r_ae[2] = (read_buffer[36] << 8) + read_buffer[37];
			r_ae[3] = (read_buffer[38] << 8) + read_buffer[39];
			r_lift = (read_buffer[40] << 8) + read_buffer[41];
			r_roll = (read_buffer[42] << 8) + read_buffer[43];
			r_pitch = (read_buffer[44] << 8) + read_buffer[45];
			r_yaw = (read_buffer[46] << 8) + read_buffer[47];
		
    

		/*	FILE *fp;
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
		}
		printf("%10ld|%10ld|%10ld|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|",r_time,r_pressure,r_temperature,r_mode,r_incoming,r_phi,r_theta,r_psi,r_sax,r_say,r_saz,r_sp,r_sq,r_sr,r_ae[0],r_ae[1],r_ae[2],r_ae[3],r_lift,r_roll,r_pitch,r_yaw);

		addr -= NUM_DATA;
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
