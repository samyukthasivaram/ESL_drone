#include "in4073.h"

void panicmode()

{//slow down motor with counter delay

uint16_t pcounter = 200;
int  trigger_p = 2000000;
uint32_t difference_p;
uint32_t before_p = get_time_us();
for(int mo=0;mo<4;mo++)
ae[mo]=200;
do {
  difference_p = get_time_us() - before_p;
} while ( difference_p < trigger_p );


printf("Panic Mode\n");


  for(pcounter=200;pcounter>0;pcounter--)
{
	ae[0]=pcounter;
	ae[1]=pcounter;
	ae[2]=pcounter;
	ae[3]=pcounter;
//printf("m1=%d|%d|%d|%d|\n",ae[0],ae[1],ae[2],ae[3]);
}

//safe_mode();

}
void safe_mode()
{
//printf("Safe Mode\n");
for(int i=0;i<4;i++)
	ae[i]=0;
}
