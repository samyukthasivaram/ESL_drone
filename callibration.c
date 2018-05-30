#include <stdio.h>
#include <math.h>
#include <time.h>
#include "in4073.h"

void callibration_mode()
{
	safe_mode();
    printf("callibrating");
	bool flag_init = check_timer_flag();
	int i = 0;
	float sum_accel[3] = { 0 };
	float sum_gyro[3] = { 0 };
	int num = 0;

	
	while (i<10)
	{
		if (!flag_init && check_timer_flag())
		{
			i++;
		}
		sum_accel[0] = sum_accel[0] + sax;
		sum_accel[1] = sum_accel[1] + say;
		sum_accel[2] = sum_accel[2] + saz;
		sum_gyro[0] = sum_gyro[0] + sp;
		sum_gyro[1] = sum_gyro[1] + sq;
		sum_gyro[2] = sum_gyro[2] + sr;
		num++;
	}
	int16_t ave_accx = sum_accel[0] / num;
	int16_t ave_accy = sum_accel[1] / num;
	int16_t ave_accz = sum_accel[2] / num;
	int16_t ave_gyx = sum_gyro[0] / num;
	int16_t ave_gyy = sum_gyro[1] / num;
	int16_t ave_gyz = sum_gyro[2] / num;
    printf("%d|%d|%d|%d|%d|%d|",ave_accx,ave_accy,ave_accz,ave_gyx,ave_gyy,ave_gyz);

}


