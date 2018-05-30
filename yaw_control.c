#include <stdlib.h>
#include "in4073.h"
//include manual mode function of calculating square root.

/*
 *Assumption: main function uses state machine, every time the execution of this mode is over,
 *it will start again according to the command from PC.
 *Joystick passes the expected r.
 *The values of p and error are passed by setting them as static variations. To avoid being refreshed
 *every time.
 */

//static int16_t p=10;	//initial value to be decided
static int16_t error = 0;

void yaw_control()
{
int16_t t_roll=0,t_pitch=0,t_yaw=0,t_lift=0;
//if(keyboard==8) p++;
//else if(keyboard==9) p--;
	//Does p has a range?
	
	//sr = get_sensor_data();
	error = yaw - (700*sr);
if(error>100)
	t_yaw = p_yaw * error;
//printf("error=%d\n",p_yaw);	

t_roll=roll/16;t_lift=(lift/16)+8000;t_yaw=t_yaw/16;t_pitch=pitch/16;
motor_control(t_roll,t_yaw, t_pitch, t_lift);

 //printf("m1=%d|m2=%d|",ae[0],ae[1]);
//printf("sr=%6d| y=%d|\n ", sr,yaw);	

}

 

