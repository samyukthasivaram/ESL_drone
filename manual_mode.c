//http://www.microchip.com/forums/m577584.aspx

#include "in4073.h"
#define Motor_const 13
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

int16_t man_roll=0,man_pitch=0,man_yaw=0,man_lift=0;

man_lift=lift;
man_roll=roll;
man_yaw=yaw;
man_pitch=pitch;

motor_control(man_lift,man_roll,man_pitch,man_yaw);

}



void motor_control(int16_t t_lift, int16_t t_roll,int16_t t_pitch, int16_t t_yaw)
{   int16_t w1_sqr = 0;
	int16_t w2_sqr = 0;
	int16_t w3_sqr = 0;
	int16_t w4_sqr = 0;

		
    if(t_lift<277)
    {
	ae[0] = squareroot(t_lift) * Motor_const ;
	ae[1] = squareroot(t_lift) * Motor_const ;
	ae[2] = squareroot(t_lift) * Motor_const ;
	ae[3] = squareroot(t_lift) * Motor_const ;

    }

    else
    {

    w1_sqr= (int16_t) (-t_yaw + t_lift + 2*t_pitch) ;
	w3_sqr= (int16_t) (-t_yaw + t_lift  - 2*t_pitch);
	w2_sqr= (int16_t) (t_yaw + t_lift  - 2*t_roll) ;	
	w4_sqr= (int16_t) (t_yaw + t_lift  + 2*t_roll) ;
	ae[0] = squareroot(w1_sqr) * Motor_const ;
	ae[1] = squareroot(w2_sqr) * Motor_const ;
	ae[2] = squareroot(w3_sqr) * Motor_const ;
	ae[3] = squareroot(w4_sqr) * Motor_const ;
	
 //printf("manual/n");
 if (ae[0] < 200) ae[0] = 200; 
 if (ae[0] > 600) ae[0] = 600;
 if (ae[1] < 200) ae[1] = 200; 
 if (ae[1] > 600) ae[1] = 600;
 if (ae[2] < 200) ae[2] = 200; 
 if (ae[2] > 600) ae[2] = 600;
 if (ae[3] < 200) ae[3] = 200; 
 if (ae[3] > 600) ae[3] = 600;
    }
}







