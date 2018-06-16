#include "in4073.h"
#include "RS232.h"
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#define DataSize 10
#define POLY 0x8005

/*---------------------------------------------------------
 *Data format: | StartByte | Data | CRC1 | CRC2 | 14 bytes
  ---------------------------------------------------------
 */

//CRC calculation without table
int16_t calc_crc(int8_t frame[],int count,int start)

{	
    int16_t crc,byte;
    crc = 0;
    
    for (int i=start; i< count; i++)
    {
            byte=frame[i];
            crc ^=byte<<8;
	    for(int k=8;k>0;k--)
		{if(crc & 0x8000)
			crc^=POLY;
		 crc=crc<<1;
		}
    
    }
    return crc;
}

bool check_crc(int8_t c1, int8_t c2, int8_t q[])
{
   int16_t w= calc_crc(q,10,0);
   int16_t new_c = ((c2<<8) & 0xFF00) | (c1 & 0x00FF);
  if (w==new_c)
     return true;
  else
    return false;
}

int8_t frame[DataSize];
uint8_t count=0;
int rx_wrong[10];
int8_t StartByte=0xFF;
static int8_t check1 = 0,check2 = 0;	//store CRC

void rs232_read()
{
	static int state = 0;
	int8_t data = 0;
	int i=0,j = 0;	

		
		if(rx_queue.count>0)
	{		data = dequeue(&rx_queue); rec_counter=0;
			//printf("%d| \n ",data);
		switch(state)
		{
			case 0:
				if(data == StartByte)
				{
					state = 1;
				}
				else	state = 0;
				break;
				
			case 1:
				if(count >=DataSize)
				{
					check1 = data;
					count = 0;
					state = 2;
				}
				else
				{
					frame[count] = data;
					count++;
					state = 1;
				}
				break;
			

			case 2:
				check2 = data;
				if(check_crc(check1,check2,frame))//check CRC <TO DO>
				{
					state = 0;packet_drop=0;
					store_data(frame);
				}
				else    //if CRC is wrong check for start byte to start over
				{    
					//printf("crc wrong");
					if(check1 == StartByte)
					{
						frame[0] = check2;
						count = 1;
						state = 1;
					}
					else if(check2 == StartByte)
					{
						count = 0;
						state = 1;
					}
					else
					{	int crcflag=0;
						for(i=0; i<10; i++)  
						{
							if(frame[i] == StartByte)
							{	crcflag=1; 
								int k=0;
								for(j=i+1; j<10; j++)
									rx_wrong[k++] = frame[j];
								rx_wrong[k++] = check1;
								rx_wrong[k++] = check2;
								state = 1;
								for(j=0; j<k; j++)
									frame[j]=rx_wrong[j];
							}
							else	state = 0;
							break;
						}
					if(crcflag==0) packet_drop++;

					}


				}

				break;
			
			default: printf("deafult case");
					break;
		}
	}
	else rec_counter++;      //to check continuous communication
	
}

void store_data(int8_t p[DataSize])
{
	
	int16_t temp_lift, temp_roll, temp_pitch, temp_yaw;

	mode= p[0];	
	temp_lift = (p[2]<<8)+p[1];
	temp_roll = (p[4]<<8)+p[3];
	temp_pitch = (p[6]<<8)+p[5];	
	temp_yaw = (p[8]<<8)+p[7];	
	keyboard = p[9];

	switch (keyboard)
	{
		case 0://lift up 'a'
			lift_key+=1;
			
			break;
		case 1://lift down 'z'
			lift_key-=1;
			
			break;
		case 2://roll up '<'
			roll_key+=1;
			break;
		case 3://roll down '>'
		roll_key-=1;
			break;
		case 4://pitch down '^'
			pitch_key-=1;
			break;
		case 5://pitch up 'down arrow'
		pitch_key+=1;
			break;
		case 6:// yaw clockwise 'q'
				yaw_key+=1;
				break;
		case 7://yaw ccw  'w'
			    yaw_key-=1;
				break;
		case 8: p_yaw+=1;break; // 'u'
		case 9: p_yaw-=1;       //'j'
				if(p_yaw<1) p_yaw=1;
				break;
		case 10:P1+=1;break;   //'i'
		case 11:P1-=1;			//'k'
				if(P1<1) P1=1;
				break;
		case 12:P2+=1;break; //'o'
		case 13:P2-=1;		//'l'
				if(P2<1) P2=1;
				break;
		case 14: //f
				flag_logging = 1;
				break;
		case 15: //r
				read_from_flash();
				break;
	}
	
	lift = (-1*((temp_lift)>>6))+lift_key+507;
	roll = ((temp_roll)>>6)+roll_key;
	pitch = ((temp_pitch)>>6)+pitch_key;
	yaw = ((temp_yaw)>>6)+yaw_key;
	
	//printf("man=%d|%d|%d|%d|lift=%d|roll=%d|pitch=%d|yaw=%d|\n",temp_lift, temp_roll, temp_pitch, temp_yaw,lift,roll,pitch,yaw);
	//printf("mode=%d|key=%d\n",mode,keyboard);
	//printf("lift_key=%d\n",lift_key);

}

//transmit frame telemetry - ae[0] | ae[1] | ae[2] | ae[3] | bat_volt | phi | theta | psi | sp | sq | sr
void rs232_write()
{
	int8_t tx_buff[43];
	int16_t bat_volt_temp = bat_volt - 32767;
	
	int16_t crc;
	
	tx_buff[0] 		= 0xFA;
	tx_buff[1] 		= mode;
	tx_buff[2] 		= (int8_t)ae[0];
	tx_buff[3] 		= (int8_t)(ae[0]>>8);
	tx_buff[4] 		= (int8_t)ae[1];
	tx_buff[5] 		= (int8_t)(ae[1]>>8);
	tx_buff[6] 		= (int8_t)ae[2];
	tx_buff[7] 		= (int8_t)(ae[2]>>8);
	tx_buff[8] 		= (int8_t)ae[3];
	tx_buff[9] 		= (int8_t)(ae[3]>>8);
	tx_buff[10]		= (int8_t)bat_volt_temp;
	tx_buff[11]		 = (int8_t)(bat_volt_temp>>8);
	tx_buff[12]		 = (int8_t)phi;
	tx_buff[13]		 = (int8_t)(phi>>8);
	tx_buff[14]		 = (int8_t)theta;
	tx_buff[15]		 = (int8_t)(theta>>8);
	tx_buff[16]		 = (int8_t)psi;
	tx_buff[17]		 = (int8_t)(psi>>8);
	tx_buff[18]		 = (int8_t)sp;
	tx_buff[19]		 = (int8_t)(sp>>8);
	tx_buff[20]		 = (int8_t)sq;
	tx_buff[21]		 = (int8_t)(sq>>8);
	tx_buff[22]      = (int8_t)sr;
	tx_buff[23]		 = (int8_t)(sr>>8);
	tx_buff[24]      = (int8_t)sax;
	tx_buff[25]      = (int8_t)(sax >> 8);
	tx_buff[26]      = (int8_t)say ;
	tx_buff[27]      = (int8_t)(say >> 8) ;
	tx_buff[28]      = (int8_t)saz;
	tx_buff[29]      = (int8_t)(saz >> 8) ;
	tx_buff[30]      = (int8_t)lift;
	tx_buff[31]      = (int8_t)(lift >> 8);
	tx_buff[32]      = (int8_t)roll ;
	tx_buff[33]      = (int8_t)(roll >> 8);
	tx_buff[34]      = (int8_t)pitch;
	tx_buff[35]      = (int8_t)(pitch >> 8);
	tx_buff[36]	     = (int8_t)yaw;
	tx_buff[37]      = (int8_t)(yaw >> 8);
	tx_buff[38]		 = (int8_t) p_yaw;
	tx_buff[39] 	 = (int8_t)P1;
	tx_buff[40]		 = (int8_t) P2;

	crc = calc_crc(tx_buff,41,1);
	
	tx_buff[41]= (int8_t)crc;
	tx_buff[42]= (int8_t)(crc>>8);
	
	
	for(int i=0; i<43; i++)
		uart_put(tx_buff[i]);	
	
}






