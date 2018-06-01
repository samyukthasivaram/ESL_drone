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
int16_t yaw_roll=0,yaw_pitch=0,yaw_yaw=0,yaw_lift=0,yaw_sr;

yaw_lift=lift;
yaw_roll=roll;
yaw_yaw=yaw;
yaw_pitch=pitch;
	
	yaw_sr=(sr-sr_c)>>6;
	error = yaw - yaw_sr;
	if(error>100)
	yaw_yaw = p_yaw * error;


motor_control(yaw_lift,yaw_roll,yaw_pitch,yaw_yaw);

}

void full_control()
{int16_t full_roll=0,full_pitch=0,full_yaw=0,full_lift=0,full_sr,full_sp,full_sq;
 int16_t error_yaw,error_pitch,error_roll;
full_lift=lift;
full_roll=roll;
full_yaw=yaw;
full_pitch=pitch;
	
	full_sr=(sr-sr_c)>>6;
	error_yaw = yaw - full_sr;

	full_sp=(sp-sp_c)>>6;
	error_roll = roll - full_sp;	

	full_sq=(sq-sq_c)>>6;
	error_pitch = pitch - full_sq;

	if(error_yaw>100)
	full_yaw = p_yaw * error_yaw;

	
	full_roll = (roll - (phi - sax_c))*P1 - error_roll*P2;
	full_pitch = (pitch - (theta - say_c) )*P1 + (error_pitch)*P2;


motor_control(full_lift,full_roll,full_pitch,full_yaw);
}
 

