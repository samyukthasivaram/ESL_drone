/*----------------------------------------------------------------------------------------------------------
 *This file ontains functions that enable us to write and read from flash
 *
 * Developed and tested by Samyuktha Sivaram
 * Embedded Software Lab
 * Date - 16.06.2018
 ----------------------------------------------------------------------------------------------------------
*/
#include"in4073.h"

#define NUM_DATA   51
#define MAX_ADDR   0x1FFF
#define MIN_ADDR   0x0000

static uint32_t addr=MIN_ADDR;


/*----------------------------------------------------------------------------------------------------------
 * Function update_data() - updata the data that is to be stored in the flash in an array
 * Return value - void
 * Arguments : null
 *----------------------------------------------------------------------------------------------------------
 */
 
void update_data()
{   
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
	
}

/*----------------------------------------------------------------------------------------------------------
 * Function save_data_in_flash() - write the data in the flash
 * Return value - void
 * Arguments : null
 *----------------------------------------------------------------------------------------------------------
 */
 
void save_data_in_flash() 
{ 

	update_data();
	
	/* if the flash memory is full clear flash and move addr to MIN_ADDR */
	if ((addr + NUM_DATA) > MAX_ADDR)
	{
		flag_logging=0;
		printf("\ncan not write all the data in flash");
		if (flash_chip_erase())
		{
			addr = MIN_ADDR;
		}
		else
		{
			printf("\nflash_chip_erase:error");
		}

	}
	
	/*write data to flash and move addr by NUM_DATA bytes */
	else
	{
		if(flash_write_bytes(addr, data_buffer, NUM_DATA))
		{
			addr = addr + NUM_DATA;
		}
		else 
		{
			printf("\nFlash write fail.");
		}
	}
}

/*----------------------------------------------------------------------------------------------------------
 * Function read_from_flash() - read the data from flash and send it to PC for logging via UART
 * Return value - void
 * Arguments : null
 *----------------------------------------------------------------------------------------------------------
 */
 
void read_from_flash()
{	
	static uint32_t lower_address = MIN_ADDR;
	int start_byte=0xFB;
	uint8_t read_flash[NUM_DATA];
	uint8_t *read_buffer;int8_t read_flashnew[NUM_DATA+3];
	int16_t crc;
	int  trigger_p = 100000;
	uint32_t difference_p;
	read_buffer=read_flash;

	while ((lower_address + NUM_DATA) <= addr) 
	{
		if (!flash_read_bytes(lower_address, read_buffer, NUM_DATA) )
		{
			printf("read flash failed");
		}
		else 
		{	read_flashnew[0]=start_byte;
			for(int i=NUM_DATA-1; i>=0; i--)
			{read_flashnew[i+1]=(int8_t)read_flash[i];
			}
			
			crc=calc_crc(read_flashnew,NUM_DATA+1,1);
				
			read_flashnew[52] = (int8_t)(crc);
			read_flashnew[53] = (int8_t)(crc>>8);
		
			

		uint32_t before_p = get_time_us();
			do {
			  	difference_p = get_time_us() - before_p;		
				} while ( difference_p < trigger_p );


		for(int i=0; i<NUM_DATA+3; i++)
		{			
					uart_put(read_flashnew[i]);
		}
					
		}
		lower_address += NUM_DATA;
	}
	/* clear flash after reading */
	if (flash_chip_erase())
		{
			addr = MIN_ADDR;
		}
		else
		{
			printf("\nflash_chip_erase:error");
		}

	lower_address = MIN_ADDR;

}


 
