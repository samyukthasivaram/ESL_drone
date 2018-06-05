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

	int16_t sum_phi=0,sum_theta=0,sum_psi=0;

	int num = 16;

	

	
	//while (i<10)
	for(int i=0;i<num;)
	{
		//if (!flag_init && check_timer_flag())
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
		//printf("cal=%6d |%6d| %6d |%6d| %6d| %6d|\n ",sum_accel[0],sum_accel[1],sum_accel[2],sum_gyro[0],sum_gyro[1],sum_gyro[2]);
		
	}

	 phi_c=(sum_phi>>num);
	 theta_c=(sum_theta>>num);
	 psi_c=(sum_psi>>num);	
	 sax_c = sum_accel[0] >> num;
	 say_c = sum_accel[1] >> num;
	 saz_c = sum_accel[2] >>num;
	 sp_c = sum_gyro[0] >> num;
	 sq_c = sum_gyro[1] >> num;
	 sr_c = sum_gyro[2] >> num;
    printf("calibration = %d|%d|%d|%d|%d|%d|",phi_c,theta_c,psi_c,sp_c,sq_c,sr_c);

}


