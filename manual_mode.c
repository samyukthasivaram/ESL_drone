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
         printf("%d",Root);		 
		return Root;
}

void manual_mode_sqrt()
{
int w1_sqr = 0;
int w2_sqr = 0;
int w3_sqr = 0;
int w4_sqr = 0;
int ae1, ae2, ae3, ae4;
w1_sqr= -yaw - lift + pitch + pitch;
ae1 = squareroot(w1_sqr);
w3_sqr= -yaw - lift - pitch - pitch;
ae3 = squareroot(w3_sqr);
w2_sqr= yaw - lift - roll - roll;
ae2 = squareroot(w2_sqr);
w4_sqr= yaw - lift + roll + roll;
ae4 = squareroot(w4_sqr);

 if (ae1 < 0) ae1 = 0; 
 if (ae1 > 500) ae1 = 500;
 if (ae2 < 0) ae2 = 0; 
 if (ae2 > 500) ae2 = 500;
 if (ae3 < 0) ae3 = 0; 
 if (ae3 > 500) ae3 = 500;
 if (ae4 < 0) ae4 = 0; 
 if (ae4 > 500) ae4 = 500;

}

void manual_mode_withoutsqrt()
{

ae[0]= -yaw - lift + pitch + pitch;
ae[1]= -yaw - lift - pitch - pitch;
ae[2]= yaw - lift - roll - roll;
ae[3]= yaw - lift + roll + roll;



 if (ae[0] < 0) ae[0] = 0; 
 if (ae[0] > 500) ae[0] = 500;
 if (ae[1] < 0) ae[1] = 0; 
 if (ae[1] > 500) ae[1] = 500;
 if (ae[2] < 0) ae[2] = 0; 
 if (ae[2] > 500) ae[2] = 500;
 if (ae[3] < 0) ae[3] = 0; 
 if (ae[3] > 500) ae[3] = 500;
 printf("%d|%d|%d|%d|",ae[0],ae[1],ae[2],ae[3]);
}

void manual_mode_lookup()
{


}
