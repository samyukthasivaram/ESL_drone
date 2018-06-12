#include "in4073.h"

void height_control()
{
int16_t h_error=0,new_lift=0,new_pressure=0;

	if(check_sensor_int_flag())
    {
			get_dmp_data();
	}
	read_baro();

h_error=pressure-new_pressure;
new_lift=lift+h_error;
update_motors(new_lift,roll,pitch,yaw);




}