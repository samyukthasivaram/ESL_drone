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

static int16_t p=10;	//initial value to be decided
static int16_t error = 0;

void yaw_control()
{
	
if(keyboard==8) p++;
else if(keyboard==9) p--;
	//Does p has a range?
	
	//sr = get_sensor_data();
	error = yaw - (700*sr);
if(error>100)
	yaw = p * error;
	
ae[0]=(int16_t) (-yaw - lift)/4 ;
ae[2]= (int16_t)(-yaw - lift )/4;
ae[1]= (int16_t)(yaw - lift )/4;
ae[3]=(int16_t) (yaw - lift )/4;



 if (ae[0] < 100) ae[0] = 100; 
 if (ae[0] > 300) ae[0] = 300;
 if (ae[1] < 100) ae[1] = 100; 
 if (ae[1] > 300) ae[1] = 300;
 if (ae[2] < 100) ae[2] = 100; 
 if (ae[2] > 300) ae[2] = 300;
 if (ae[3] < 100) ae[3] = 100; 
 if (ae[3] > 300) ae[3] = 300;
 printf("m1=%d|m2=%d|",ae[0],ae[1]);
printf("sr=%6d| y=%d|\n ", sr,yaw);	

}

 

