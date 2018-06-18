/*------------------------------------------------------------------
 *  in4073.c -- mode switching and control
 *				update ae[0-3], uart rx queue
 *
 *  
 *  Embedded Software Lab
 *
 *  16.06.2018
 *------------------------------------------------------------------
 */

#include "in4073.h"

/*------------------------------------------------------------------
 * main -- Drone / Flight Controller
 *------------------------------------------------------------------
 */
int main(void)
{
	//uint32_t start_fullloop = get_time_us();
	//	uint32_t start_rstime,full_loop, stop_rstime,start_logtime,stop_logtime,start_controltime,stop_controltime,start_senstime,stop_senstime;
	//	full_loop = get_time_us();
	uart_init();
	gpio_init();
	timers_init();
	adc_init();
	twi_init();
	imu_init(true, 100);	
	baro_init();
	spi_flash_init();
	ble_init();
	/*Intialisation of flags and global variables*/
	keyboard=0xF0;
	static int state_mode=0,panic_flag=0,calib_flag=0;
	uint32_t counter = 0;
	demo_done = false;
	prev_mode=0;
	int trigger = 300000;
	uint32_t previous = get_time_us();
	p_yaw=10;P1=40;P2=40;
	packet_drop=0;rec_counter=0;
	lift_key=0;roll_key=0;pitch_key=0;yaw_key=0;
	flag_logging = 0;calib_flag=0;
	for(int i=0;i<51;i++)
		data_buffer[i]=0;	

	/*Control Loop*/
	while (!demo_done)
	
	{	
		// start_fullloop = get_time_us();
		//  start_rstime = get_time_us();
		rs232_read();
	 	//stop_rstime = get_time_us() - start_rstime ;
		if(keyboard!=0xF0) keyboard=0xF0;
	//start_senstime = get_time_us();
	  if (check_timer_flag()) 
		{
			if (counter++%20 == 0) nrf_gpio_pin_toggle(BLUE);

			adc_request_sample();
            read_baro(); 
			clear_timer_flag();
		}
	// stop_senstime = get_time_us() - start_senstime ;	
	/*Mode switching*/
			switch(state_mode)
			{
				/*Intial state - safe mode*/
				case 0: 
					safe_mode(); 
					if(roll!=0||pitch!=0||yaw!=0||lift!=0)
					state_mode=0;
					else if (bat_volt<1100)
					state_mode=0;
					else if(mode==0||((mode==1)&&(panic_flag==1))||(calib_flag==1 && mode==3))
					state_mode=0;
					else if(mode==1&&panic_flag==0)
					demo_done=true;
					else if(mode>=2&&mode<=6)
					{state_mode=1;prev_mode=mode;}
					else state_mode=0;
					break;

				/*Mode Switching*/
				case 1: if(prev_mode==mode)
						{panic_flag=0; 
							switch(mode)
								{
									case 2: manual_mode_sqrt();
									break;
									case 3: callibration_mode();
											state_mode=0;calib_flag=1;
											break;
									case 4:  
											if(calib_flag==1)
											yaw_control();
											else state_mode=0;
											break;
									case 5: 
											if(calib_flag==1)
											full_control();
											else state_mode=0;
											break;
									case 6:if(calib_flag==1)
											{	
											kalmanFilter();
											butterWorth_2ndOrder();
											raw_mode();
											}
											else state_mode=0;
											break;
									default: state_mode=0;
								}
					}
					else 
					{state_mode=2;
					prev_mode=mode;
					}
					break;

				/*Panic Mode*/
				case 2: panicmode();
						state_mode=0;
						if(mode==1)
						panic_flag=1;
						break;
				default: state_mode=0;

			}
	//start_controltime = get_time_us();
	if (check_sensor_int_flag()) 
		{
			get_dmp_data();
			//bat_chk();	
			run_filters_and_control();
		}
	//stop_controltime = get_time_us() - start_controltime ;


	/*Telemetry and Data logging every 300ms*/
	uint32_t current = get_time_us();
 	uint32_t difference=current-previous;

	if ( difference > trigger )
    { previous=current;

	/*Data logging*/
	if(flag_logging==1)
		{	
			//start_logtime = get_time_us();
			save_data_in_flash();
			//stop_logtime = get_time_us() - start_logtime ;
			//printf("%10ld|",stop_logtime);
		}
	/*Telemetry*/
		//start_logtime = get_time_us();
		rs232_write();	
		//stop_logtime = get_time_us() - start_logtime ;
		//printf("%10ld|",stop_logtime);

		//printf("%d|%10ld|%10ld|%10ld|%10ld|%d|%d|%d|%d|lift=%d|roll=%d|pitch=%d|yaw=%d|\n",rec_counter,full_loop,stop_rstime,stop_controltime,stop_senstime,ae[0],ae[1],ae[2],ae[3],lift,roll,pitch,yaw);
		//printf("tele:%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|\n",mode,ae[0],ae[1],ae[2],ae[3],bat_volt,phi,theta,psi,sp,sq,sr,sax,say,saz,lift,roll,pitch,yaw,p_yaw,P1,P2);

	}
	//full_loop = get_time_us() - start_fullloop;

		/*Exit control loop if communication is disrupted*/
		if(packet_drop>10000||rec_counter>1000) 
			demo_done=true;	 	

	}
	
	printf("\n\t Goodbye \n\n");
	nrf_delay_ms(100);

	NVIC_SystemReset();

}
