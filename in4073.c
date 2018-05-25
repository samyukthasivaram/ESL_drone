/*------------------------------------------------------------------
 *  in4073.c -- test QR engines and sensors
 *
 *  reads ae[0-3] uart rx queue
 *  (q,w,e,r increment, a,s,d,f decrement)
 *
 *  prints timestamp, ae[0-3], sensors to uart tx queue
 *
 *  I. Protonotarios
 *  Embedded Software Lab
 *
 *  June 2016
 *------------------------------------------------------------------
 */

#include "in4073.h"
#include "RS232.h"
/*------------------------------------------------------------------
 * process_key -- process command keys
 *------------------------------------------------------------------
 */
void process_key(uint8_t c)
{
	switch (c)
	{
		case 0://lift up
			ae[0] += 10;
			ae[1] += 10;
			ae[2] += 10;
			ae[3] += 10;
			
			break;
		case 1://lift down
			ae[0] -= 10;
			ae[1] -= 10;
			ae[2] -= 10;
			ae[3] -= 10;
			
			break;
		case 2://roll up
			ae[1] += 10;
			break;
		case 3://roll down
			ae[1] -= 10;
			if (ae[1] < 0) ae[1] = 0;
			break;
		case 4://pitch down
			ae[2] += 10;
			break;
		case 5://pitch up
			ae[2] -= 10;
			if (ae[2] < 0) ae[2] = 0;
			break;
		case 6:// yaw clockwise
			ae[0] += 10;
			ae[2] += 10;
			break;
		case 7://yaw ccw
			ae[1]-=10;
			ae[3] -= 10;
			if (ae[3] < 0) ae[3] = 0;
			if (ae[1] < 0) ae[1] = 0;
			break;
		case 27:
			demo_done = true;
			break;
		//default:
			//nrf_gpio_pin_toggle(RED);
	}
}

void bat_chk() //if low battery then go to panic mode
{

if(bat_volt<1100)
   {printf("Battery Low");
	 panicmode();}

}
/*------------------------------------------------------------------
 * main -- everything you need is here :)
 *------------------------------------------------------------------
 */
int main(void)
{
	uart_init();
	gpio_init();
	timers_init();
	adc_init();
	twi_init();
	imu_init(true, 100);	
	baro_init();
	spi_flash_init();
	ble_init();

	uint32_t counter = 0;
	demo_done = false;



	//bat_chk();

	while (!demo_done)
	
	{	
		rs232_read();
		if(mode==1||mode==9)
			{panicmode();demo_done=1;}
		if(mode==2)
			manual_mode_sqrt();
		if(keyboard!=0xF0)
			process_key(keyboard);
//		if(mode == 9)
//			demo_done=1;
		if(mode == 4)
			yaw_control();
		if (check_timer_flag()) 
		{
			if (counter++%20 == 0) nrf_gpio_pin_toggle(BLUE);

			adc_request_sample();
                  	//printf(" |");
			//read_baro(); 

			/*printf("%10ld | ", get_time_us());
			printf("%3d %3d %3d %3d | ",ae[0],ae[1],ae[2],ae[3]);
			printf("%6d %6d %6d | ", phi, theta, psi);
			printf("%6d %6d %6d | ", sp, sq, sr);
			printf("%4d | %4ld | %6ld \n", bat_volt, temperature, pressure);*/


			clear_timer_flag();
		}
		if (check_sensor_int_flag()) 
		{
			get_dmp_data();
			//bat_chk();
			run_filters_and_control();
			
		}
//rs232_write();
	}	
	//save_data_in_flash();
	//read_from_flash();
	//rs232_write();
	printf("\n\t Goodbye \n\n");
	nrf_delay_ms(100);

	NVIC_SystemReset();

}
