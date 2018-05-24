#include "in4073.h"

void panicmode()

{//slow down motor with counter delay

  uint16_t pcounter = 1000;

for(int mm=0;m<3;m++)
ae[mm]=200;
  for(;pcounter>0;pcounter--)
{
	if(ae[0]>0) ae[0]--;
	if(ae[1]>0) ae[1]--;
	if(ae[2]>0) ae[2]--;
	if(ae[3]>0) ae[3]--;
printf("m1=%d|%d|%d|%d|\n",ae[0],ae[1],ae[2],ae[3]);
}

}

