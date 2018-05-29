//http://www.microchip.com/forums/m577584.aspx

#include "in4073.h"
#define Motor_const 4
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

	int16_t t_roll=0,t_pitch=0,t_yaw=0,t_lift=0;
	//uint16_t t_lift=lift+32767;

/*if(roll<0) t_roll=-roll; 
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
      t_lift=2*t_pitch+lift;*/

t_roll=roll/16;t_lift=((lift+32767)/16);t_yaw=yaw/16;t_pitch=pitch/16;
motor_control(t_roll,t_yaw, t_pitch, t_lift);
}



void motor_control(int16_t t_roll,int16_t t_yaw,int16_t t_pitch,int16_t t_lift)
{   int16_t w1_sqr = 0;
	int16_t w2_sqr = 0;
	int16_t w3_sqr = 0;
	int16_t w4_sqr = 0;
	w1_sqr= (int16_t) (-2*t_yaw + t_lift + 2*t_pitch) ;
	w3_sqr= (int16_t) (-2*t_yaw + t_lift  - 2*t_pitch);
	w2_sqr= (int16_t) (2*t_yaw + t_lift  - 2*t_roll) ;	
	w4_sqr= (int16_t) (2*t_yaw + t_lift  + 2*t_roll) ;
		
	ae[0] = squareroot(w1_sqr) * Motor_const ;
	ae[1] = squareroot(w2_sqr) * Motor_const ;
	ae[2] = squareroot(w3_sqr) * Motor_const ;//
	ae[3] = squareroot(w4_sqr) * Motor_const ;
	
 printf("man=%d|%d|%d|%d|lift=%d|roll=%d|pitch=%d|yaw=%d|\n",ae[0],ae[1],ae[2],ae[3],t_lift,t_roll,t_pitch,t_yaw);
 //printf("manual/n");
 if (ae[0] < 200) ae[0] = 200; 
 if (ae[0] > 500) ae[0] = 500;
 if (ae[1] < 200) ae[1] = 200; 
 if (ae[1] > 500) ae[1] = 500;
 if (ae[2] < 200) ae[2] = 200; 
 if (ae[2] > 500) ae[2] = 500;
 if (ae[3] < 200) ae[3] = 200; 
 if (ae[3] > 500) ae[3] = 500;
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

