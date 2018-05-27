#include "in4073.h"

void panicmode()

{//slow down motor with counter delay

  uint16_t pcounter = 200;
int msec = 0, trigger = 5000;
printf("\n\nPanic Mode\n\n");
clock_t before = clock();

do {
for(int mm=0;mm<4;mm++)
ae[mm]=200;
  clock_t difference = clock() - before;
  msec = difference * 1000 / CLOCKS_PER_SEC;
} while ( msec < trigger );


  for(pcounter=200;pcounter>0;pcounter--)
{
	ae[0]=pcounter;
	ae[1]=pcounter;
	ae[2]=pcounter;
	ae[3]=pcounter;
printf("m1=%d|%d|%d|%d|\n",ae[0],ae[1],ae[2],ae[3]);
}

safe_mode();

}

void safe_mode()
{
//printf("Safe Mode\n");
for(int i=0;i<4;i++)
	ae[i]=0;

if(roll!=0||pitch!=0||yaw!=0)
	{printf("Warning: Illegal Initial conditions");
	prev_mode=0xFF;
         printf("%d|%d|%d|%d|",lift,roll,pitch,yaw);}
//else if(bat_volt<1100)
	//prev_mode=0xFA;
else prev_mode=0;

}
