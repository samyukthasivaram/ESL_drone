//http://www.microchip.com/forums/m577584.aspx

#include "in4073.h"
#define Motor_const 3
int16_t squareroot(int16_t value)
{
    unsigned Root = 0;
    unsigned Bit;
    if(value<0)	value = - value;
    for ( Bit = 0x4000; Bit > 0; Bit >>= 2 )
        {
        	unsigned Trial = Root + Bit;
            Root >>= 1;
            if ( Trial <= value )
            {
                Root += Bit;
                value -= Trial;
            }
        }
       // printf("%d",Root);		 
		return Root;
}

void manual_mode_sqrt()
{
	int16_t w1_sqr = 0;
	int16_t w2_sqr = 0;
	int16_t w3_sqr = 0;
	int16_t w4_sqr = 0;
	int16_t t_roll=0,t_pitch=0,t_yaw=0,t_lift=0;

if(roll<0) t_roll=-roll; 
       else t_roll=roll;
    if(lift < t_roll) 
      t_lift=2*t_roll+lift;


     if(yaw<0) t_yaw=-yaw; 
       else t_yaw=yaw;
    if(lift < t_yaw) 
      t_lift=2*t_yaw+lift;

	     if(pitch<0) t_pitch=-pitch; 
       else t_pitch=pitch;
    if(lift < t_pitch) 
      t_lift=2*t_pitch+lift;


	w1_sqr= (int16_t) (-t_yaw + t_lift + t_pitch) / 2;
	w3_sqr= (int16_t) (-t_yaw + t_lift  - t_pitch) / 2;
	w2_sqr= (int16_t) (t_yaw + t_lift  - t_roll) / 2;	
	w4_sqr= (int16_t) (t_yaw + t_lift  + t_roll) / 2;
		
	ae[0] = squareroot(w1_sqr) * Motor_const ;
	ae[1] = squareroot(w2_sqr) * Motor_const ;
	ae[2] = squareroot(w3_sqr) * Motor_const ;
	ae[3] = squareroot(w4_sqr) * Motor_const ;
	
 //printf("man=%d|%d|%d|%d|lift=%d|roll=%d|pitch=%d|yaw=%d|\n",ae[0],ae[1],ae[2],ae[3],lift,roll,pitch,yaw);
// printf("manual");
	if (ae[0] < 200) ae[0] = 200; 
	if (ae[0] > 700) ae[0] = 700;
	if (ae[1] < 200) ae[1] = 200; 
	if (ae[1] > 700) ae[1] = 700;
	if (ae[2] < 200) ae[2] = 200; 
	if (ae[2] > 700) ae[2] = 700;
	if (ae[3] < 200) ae[3] = 200; 
	if (ae[3] > 700) ae[3] = 700;
}
/*
void manual_mode_withoutsqrt()
{

ae[0]= (int16_t)(-yaw - lift + pitch )/50;
ae[2]= (int16_t)(-yaw - lift - pitch )/50;
ae[1]= (int16_t)(yaw - lift - roll )/50;
ae[3]= (int16_t)(yaw - lift + roll )/50;

 printf("m1=%d|%d|%d|%d|\n",ae[0],ae[1],ae[2],ae[3]);

 if (ae[0] < 200) ae[0] = 200; 
 if (ae[0] > 700) ae[0] = 700;
 if (ae[1] < 200) ae[1] = 200; 
 if (ae[1] > 700) ae[1] = 700;
 if (ae[2] < 200) ae[2] = 200; 
 if (ae[2] > 700) ae[2] = 700;
 if (ae[3] < 200) ae[3] = 200; 
 if (ae[3] > 700) ae[3] = 700;
}
*/

void manual_mode_lookup()
{


}

