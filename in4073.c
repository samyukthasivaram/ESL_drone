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
		default:
			nrf_gpio_pin_toggle(RED);
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
	static int state_mode=0;
	uint32_t counter = 0;
	demo_done = false;
	prev_mode=0;
	int trigger = 100000;
	uint32_t previous = get_time_us();
	

	while (!demo_done)
	
	{	
		//printf(" |");
		rs232_read();
	if(keyboard!=0xF0)
		process_key(keyboard);
	
	switch(state_mode)
	{case 0: 
			 safe_mode(); 
			 //if(roll!=0||pitch!=0||yaw!=0)
	         //{printf("Warning: Illegal Initial conditions");
			  //state_mode=0;}
			 //if (bat_volt<1100)
			 //{printf("Low battery");
			 //state_mode=0;}
			 if(mode==0)
			 state_mode=0;
			 else if(mode==1)
			 demo_done=true;
			 else if(mode>=2&&mode<=8)
			 {state_mode=1;prev_mode=mode;}
			 else {printf("invalid mode");state_mode=0;}
			 break;
	 case 1: if(prev_mode==mode)
	 		{
	         switch(mode)
	 			{case 2: manual_mode_sqrt();
						break;
		  		case 3: //callibration_mode();
						break;
				case 4: yaw_control();
						break;
				case 5://full control
				case 6://raw control
				case 7://height control
				case 8://wireless
				default: state_mode=0;
				}
			 }
			 else 
			 {state_mode=2;
			 prev_mode=mode;
			 }
			 break;
	 case 2: panicmode();
	 		 state_mode=0;
			 break;
	 default: state_mode=0;

	}

	uint32_t current = get_time_us();
 	uint32_t difference=current-previous;

	if ( difference > trigger )
    {   previous=current;
    //printf("man=%d|%d|%d|%d|lift=%d|roll=%d|pitch=%d|yaw=%d|\n",ae[0],ae[1],ae[2],ae[3],lift,roll,pitch,yaw);
	//printf("%6d %6d %6d | \n", sp, sq, sr);

	
	}
	  if (check_timer_flag()) 
		{
			if (counter++%20 == 0) nrf_gpio_pin_toggle(BLUE);

			adc_request_sample();
                  	
			//read_baro(); 

			/*printf("%10ld | ", get_time_us());
			printf("%3d %3d %3d %3d | ",ae[0],ae[1],ae[2],ae[3]);
			printf("%6d %6d %6d | ", phi, theta, psi);
			printf("%6d %6d %6d | ", sp, sq, sr);
			printf("%4d | %4ld | %6ld \n", bat_volt, temperature, pressure);*/
			//printf("%6d %6d %6d |\n ", sp, sq, sr);

			clear_timer_flag();
		}
		
		if (check_sensor_int_flag()) 
		{
			get_dmp_data();//bat_chk();
			run_filters_and_control();
		}
//rs232_write();
	}	

	printf("\n\t Goodbye \n\n");
	nrf_delay_ms(100);

	NVIC_SystemReset();

}
