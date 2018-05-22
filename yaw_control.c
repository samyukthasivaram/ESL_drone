#include <stdlib.h>
#include <in4073.h>
//include manual mode function of calculating square root.

/*
 *Assumption: main function uses state machine, every time the execution of this mode is over,
 *it will start again according to the command from PC.
 *Joystick passes the expected r.
 *The values of p and error are passed by setting them as static variations. To avoid being refreshed
 *every time.
 */

static int16_t p = 10;	//initial value to be decided
static int16_t error = 0;

void yaw_control(int16_t roll, int16_t pitch, int16_t yaw, int16_t lift, int8_t keyboard)
{
	int16_t w1_sqr = 0;
	int16_t w2_sqr = 0;
	int16_t w3_sqr = 0;
	int16_t w4_sqr = 0;
	
	if(keyboard == ‘8’)		p++;
	else if(keyboard == ‘9’)	p--;
	
	//Does p has a range?
	
	error = yaw - sr;
	yaw = p * error;
	
	ae[0] = -yaw + lift + pitch + pitch;
	ae[2] = -yaw + lift - pitch - pitch;
	ae[1] = yaw + lift - roll - roll;
	ae[3]= yaw + lift + roll + roll;

	if (ae[0] < 0) ae[0] = 0; 
	if (ae[0] > 500) ae[0] = 500;
	if (ae[1] < 0) ae[1] = 0; 
	if (ae[1] > 500) ae[1] = 500;
	if (ae[2] < 0) ae[2] = 0; 
	if (ae[2] > 500) ae[2] = 500;
	if (ae[3] < 0) ae[3] = 0; 
	if (ae[3] > 500) ae[3] = 500;	

}

 
