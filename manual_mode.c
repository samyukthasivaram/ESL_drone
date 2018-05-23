//http://www.microchip.com/forums/m577584.aspx

#include "in4073.h"
int squareroot(int value)
{
 unsigned Root = 0;
         unsigned Bit;
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
         //printf("%d",Root);		 
		return Root;
}

void manual_mode_sqrt()
{
int w1_sqr = 0;
int w2_sqr = 0;
int w3_sqr = 0;
int w4_sqr = 0;
w1_sqr= -yaw - lift + pitch + pitch;
ae[0] = squareroot(w1_sqr);
w3_sqr= -yaw - lift - pitch - pitch;
ae[2] = squareroot(w3_sqr);
w2_sqr= yaw - lift - roll - roll;
ae[1] = squareroot(w2_sqr);
w4_sqr= yaw - lift + roll + roll;
ae[3] = squareroot(w4_sqr);
 //printf("m1=%d|%d|%d|%d|\n",ae[0],ae[1],ae[2],ae[3]);
if (ae[0] < 50) ae[0] = 50; 
 if (ae[0] > 400) ae[0] = 400;
 if (ae[1] < 50) ae[1] = 50; 
 if (ae[1] > 400) ae[1] = 400;
 if (ae[2] < 50) ae[2] = 50; 
 if (ae[2] > 400) ae[2] = 400;
 if (ae[3] < 50) ae[3] = 50; 
 if (ae[3] > 400) ae[3] = 400;
}

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

void manual_mode_lookup()
{


}
