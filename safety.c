#include "in4073.h"

void panicmode()

{//slow down motor with counter delay

uint16_t panic_speed = 200;
int  trigger_p = 1000000;
uint32_t difference_p;


while(panic_speed>0)
{
for(int mo=0;mo<4;mo++)
ae[mo]=panic_speed;
run_filters_and_control();
uint32_t before_p = get_time_us();
do {
  difference_p = get_time_us() - before_p;		
} while ( difference_p < trigger_p );
panic_speed-=50;
}
//printf("Panic Mode\n");

safe_mode();

}


void safe_mode()
{
//printf("Safe Mode\n");
for(int i=0;i<4;i++)
	ae[i]=0;
}
