#include <stdio.h>
#include <math.h>
#include <time.h>
#include "in4073.h"

void callibration_mode()
{
	safe_mode();
	int16_t sum_accel[3];
    int16_t sum_gyro[3];
    //printf("callibrating");
	//bool flag_init = check_timer_flag();
	sum_accel[0] = 0;
	sum_accel[1] = 0;
	sum_accel[2] = 0;

	sum_gyro[0] =  0;
	sum_gyro[1] =  0;
	sum_gyro[2] =  0;
	int num = 10;

	

	
	//while (i<10)
	for(int i=0;i<10;i++)
	{
		//if (!flag_init && check_timer_flag())
		
			
		
		sum_accel[0] = sum_accel[0] + sax;
		sum_accel[1] = sum_accel[1] + say;
		sum_accel[2] = sum_accel[2] + saz;
		sum_gyro[0] = sum_gyro[0] + sp;
		sum_gyro[1] = sum_gyro[1] + sq;
		sum_gyro[2] = sum_gyro[2] + sr;
		//printf("cal=%6d |%6d| %6d |%6d| %6d| %6d|\n ",sum_accel[0],sum_accel[1],sum_accel[2],sum_gyro[0],sum_gyro[1],sum_gyro[2]);
		
	}
	 sax_c = sum_accel[0] / num;
	 say_c = sum_accel[1] / num;
	 saz_c = sum_accel[2] / num;
	 sp_c = sum_gyro[0] / num;
	 sq_c = sum_gyro[1] / num;
	 sr_c = sum_gyro[2] / num;
    //printf("%d|%d|%d|%d|%d|%d|",ave_accx,ave_accy,ave_accz,ave_gyx,ave_gyy,ave_gyz);

}


