/*----------------------------------------------------------------------------------------------------------
 *This file includes 4  main functions for safety feature implementation
 *
 * Developed and tested by Samyuktha Sivaram
 * Embedded Software Lab
 * Date - 16.06.2018
 ----------------------------------------------------------------------------------------------------------
*/
#include "in4073.h"

/*----------------------------------------------------------------------------------------------------------
 * Function panicmode() - slow down motor from 200rpm to 0 rpm in steps of 50 and wait for 1sec 
 *							and finally go to safe mode
 * Return value - void
 * Arguments : null
 *----------------------------------------------------------------------------------------------------------
 */
void panicmode()
{

uint16_t panic_speed = 200;
int  trigger_p = 1000000;
uint32_t difference_p;

if(lift>277)
	{
		while(panic_speed>0)
		{
			for(int mo=0;mo<4;mo++)
				ae[mo]=panic_speed;
			run_filters_and_control();
			uint32_t before_p = get_time_us();
			do {
				difference_p = get_time_us() - before_p;		
			} while ( difference_p < trigger_p );
			
			panic_speed-=50;
		}
	}
safe_mode();
}

/*----------------------------------------------------------------------------------------------------------
 * Function safe_mode() - make the motor values to 0 rpm
 * Return value - void
 * Arguments : null
 *----------------------------------------------------------------------------------------------------------
 */
void safe_mode()
{
for(int i=0;i<4;i++)
	ae[i]=0;
}

/*----------------------------------------------------------------------------------------------------------
 * Function bat_chk() - The drone goes to panic mode if the battery voltage falls below a threshold
 * Return value - void
 * Arguments : null
 *----------------------------------------------------------------------------------------------------------
 */
 
void bat_chk() //if low battery then go to panic mode
{

if(bat_volt<1100)
   {printf("Battery Low");
	 panicmode();
	 demo_done=true;}

}

/*----------------------------------------------------------------------------------------------------------
 * Function callibration_mode() - take the average of 16 consecutive values of sensor and store as reference value
 * Return value - void
 * Arguments : null
 *----------------------------------------------------------------------------------------------------------
 */
 
void callibration_mode()
{
	safe_mode();
	int16_t sum_accel[3];
    int16_t sum_gyro[3];
	int16_t sum_phi=0,sum_theta=0,sum_psi=0;
	sum_accel[0] = 0;
	sum_accel[1] = 0;
	sum_accel[2] = 0;

	sum_gyro[0] =  0;
	sum_gyro[1] =  0;
	sum_gyro[2] =  0;
	
	int num = 16;
	
	for(int i=0;i<num;)
	{
		if (check_sensor_int_flag()) 
		{get_dmp_data();i++;}

		sum_phi=sum_phi+(phi>>6);
		sum_theta=sum_theta+(theta>>6);
		sum_psi=sum_psi+(psi>>6);
		
		sum_accel[0] = sum_accel[0] + (sax>>6);
		sum_accel[1] = sum_accel[1] + (say>>6);
		sum_accel[2] = sum_accel[2] + (saz>>6);
		sum_gyro[0] = sum_gyro[0] + (sp>>6);
		sum_gyro[1] = sum_gyro[1] + (sq>>6);
		sum_gyro[2] = sum_gyro[2] + (sr>>6);
		
	}

	 phi_c=(sum_phi>>4);
	 theta_c=(sum_theta>>4);
	 psi_c=(sum_psi>>4);	
	 sax_c = sum_accel[0] >> 4;
	 say_c = sum_accel[1] >> 4;
	 saz_c = sum_accel[2] >>4;
	 sp_c = sum_gyro[0] >> 4;
	 sq_c = sum_gyro[1] >> 4;
	 sr_c = sum_gyro[2] >> 4;
    printf("%d|%d|%d|%d|%d|%d|",phi_c,theta_c,psi_c,sp_c,sq_c,sr_c);

}