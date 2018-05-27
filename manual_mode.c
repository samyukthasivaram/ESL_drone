//http://www.microchip.com/forums/m577584.aspx

#include "in4073.h"
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
	w1_sqr= (int16_t) (-yaw + lift + pitch + pitch) / 2;
	w3_sqr= (int16_t) (-yaw + lift - pitch - pitch) / 2;
	w2_sqr= (int16_t) (yaw + lift - roll - roll) / 2;	
	w4_sqr= (int16_t) (yaw + lift + roll + roll) / 2;
		
	ae[0] = squareroot(w1_sqr) * 3;
	ae[1] = squareroot(w2_sqr) * 3;
	ae[2] = squareroot(w3_sqr) * 3;
	ae[3] = squareroot(w4_sqr) * 3;
	
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

