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

int16_t yaw_control(int16_t roll, int16_t pitch, int16_t yaw, int16_t lift, int16_t keyboard)
{
	int16_t w1_sqr = 0;
	int16_t w2_sqr = 0;
	int16_t w3_sqr = 0;
	int16_t w4_sqr = 0;
	
	if(keyboard == 8)	p++;
	else if(keyboard == 9)	p--;
	
	//Does p has a range?
	
	sr = get_sensor_data();
	error = yaw - sr;
	yaw = p * error;
	
	w1_sqr= -yaw - lift + pitch + pitch;
	ae[0] = squareroot(w1_sqr);
	w3_sqr= -yaw - lift - pitch - pitch;
	ae[2] = squareroot(w3_sqr);
	w2_sqr= yaw - lift - roll - roll;
	ae[1] = squareroot(w2_sqr);
	w4_sqr= yaw - lift + roll + roll;
	ae[3] = squareroot(w4_sqr);

	if (ae[0] < 0) ae[0] = 0; 
	if (ae[0] > 500) ae[0] = 500;
	if (ae[1] < 0) ae[1] = 0; 
	if (ae[1] > 500) ae[1] = 500;
	if (ae[2] < 0) ae[2] = 0; 
	if (ae[2] > 500) ae[2] = 500;
	if (ae[3] < 0) ae[3] = 0; 
	if (ae[3] > 500) ae[3] = 500;	
	
	return ae[4];
}

 
