//http://www.microchip.com/forums/m577584.aspx
int squareroot(int value)
{
 unsigned Root = 0;
         unsigned Bit;
         for ( Bit = 0x4000; Bit > 0; Bit >>= 2 )
           {
             unsigned Trial = Root + Bit;
             Root >>= 1;
             if ( Trial <= Value )
               {
                 Root += Bit;
                 Value -= Trial;
               }
           }
         printf("%d",Root);		 
		return root;
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
int ae1 = 0;
int ae2 = 0;
int ae3 = 0;
int ae4 = 0;
ae1= -yaw - lift + pitch + pitch;
ae2= -yaw - lift - pitch - pitch;
ae3= yaw - lift - roll - roll;
ae4= yaw - lift + roll + roll;



 if (ae1 < 0) ae1 = 0; 
 if (ae1 > 500) ae1 = 500;
 if (ae2 < 0) ae2 = 0; 
 if (ae2 > 500) ae2 = 500;
 if (ae3 < 0) ae3 = 0; 
 if (ae3 > 500) ae3 = 500;
 if (ae4 < 0) ae4 = 0; 
 if (ae4 > 500) ae4 = 500;
 
}

void manual_mode_lookup()
{


}