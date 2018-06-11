
#include "crc.h"
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


#define POLY 0x8005

static int Datasize=0;
/*---------------------------------------------------------
 *Data format: | StartByte | Data | CRC1 | CRC2 | 14 bytes
  ---------------------------------------------------------
 */

//CRC calculation without table
int16_t calc_crc(int frame[],int count)

{	
    int16_t crc,byte;
    crc = 0;
    
    for (int i=0; i< count; i++)
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

bool check_crc(int8_t c1, int8_t c2, int q[])
{
   int16_t w= calc_crc(q,Datasize);
   int16_t new_c = ((c2<<8) & 0xFF00) | (c1 & 0x00FF);
  if (w==new_c)
     return true;
  else
    return false;
}

int frame[50];
uint8_t count=0;
int rx_wrong[50];
int8_t StartByte=0xFF;
static int8_t check1 = 0,check2 = 0;	//store CRC

void rs232_read()
{
	static int state = 0;
	int8_t c = 0;
	int i=0,j = 0;	
	
		if (read(fd_RS232, &c, 1))
	{		//data = dequeue(&rx_queue); rec_counter=0;
			//printf("%d| \n ",data);
		switch(state)
		{
			case 0:
				if(c == 0xFA)
				{
					state = 1;
					Datasize = 40;
				}
				else if(c==0xFB)
				{
					state = 1;
					Datasize = 50;
				}
				else	state = 0;
				break;
				
			case 1:
				if(count >=DataSize)
				{
					check1 = c;
					count = 0;
					state = 2;
				}
				else
				{
					frame[count] = c;
					count++;
					state = 1;
				}
				break;
			

			case 2:
				check2 = c;
				if(check_crc(check1,check2,frame))//check CRC <TO DO>
				{
					state = 0;packet_drop=0;
					log_data(frame);
				}
				else    //if CRC is wrong check for start byte to start over
				{    
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



void log_data(int8_t read_buffer[50])
{
	uint32_t r_time=0,r_pressure=0,r_temperature=0;
	int8_t r_mode=0, r_incoming=0;//incoming: joystick or keyboard
	int16_t r_phi=0, r_theta=0, r_psi=0, r_sax=0, r_say=0, r_saz=0, r_sp=0, r_sq=0, r_sr=0, r_lift=0,r_roll=0,r_pitch=0,r_yaw=0;
	int16_t r_ae[4]={0};
	
	
			r_time = (read_buffer[0] << 24) + (read_buffer[1] << 16) + (read_buffer[2] << 8) + read_buffer[3];
			r_mode = read_buffer[4];
			r_incoming = read_buffer[5];
			r_phi = (read_buffer[6] << 8 )+ read_buffer[7];
			r_theta = (read_buffer[8] << 8) + read_buffer[9];
			r_psi = (read_buffer[10] << 8 )+ read_buffer[11];
			r_sax = (read_buffer[12] << 8 )+ read_buffer[13];
			r_say =( read_buffer[14] << 8) + read_buffer[15];
			r_saz = (read_buffer[16] << 8 )+ read_buffer[17];
			r_sp= (read_buffer[18]<< 8) + read_buffer[19];
			r_sq = (read_buffer[20] << 8 )+ read_buffer[21];
			r_sr = (read_buffer[22] << 8) + read_buffer[23];
			r_pressure = (read_buffer[24] << 24) + (read_buffer[25] << 16) + (read_buffer[26] << 8) + read_buffer[27];
			r_temperature = (read_buffer[28] << 24) + (read_buffer[29] << 16) + (read_buffer[30] << 8 )+ read_buffer[31];
			r_ae[0] = (read_buffer[32] << 8) + read_buffer[33];
			r_ae[1] = (read_buffer[34] << 8) + read_buffer[35];
			r_ae[2] = (read_buffer[36] << 8) + read_buffer[37];
			r_ae[3] = (read_buffer[38] << 8) + read_buffer[39];
	
	if(Datasize==50)
			

	
			FILE *fp;
			fp = fopen("data.csv", "w+");
			if (!fp) 
			{
				printf("creat file error");
			}
			else 
			{
			
				fprintf(fp,"%10ld,%10ld,%10ld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",r_time,r_pressure,r_temperature,r_mode,r_incoming,r_phi,r_theta,r_psi,r_sax,r_say,r_saz,r_sp,r_sq,r_sr,r_ae[0],r_ae[1],r_ae[2],r_ae[3],r_lift,r_roll,r_pitch,r_yaw);
				//fwrite(&r, sizeof(r), 1, fp);
			    //fprintf(f, "%s","\n");
				fclose(fp);
			}
			
	else if(Datasize==40)
			r_lift = (read_buffer[40] << 8) + read_buffer[41];
			r_roll = (read_buffer[42] << 8) + read_buffer[43];
			r_pitch = (read_buffer[44] << 8) + read_buffer[45];
			r_yaw = (read_buffer[46] << 8) + read_buffer[47];
			printf("%10ld|%10ld|%10ld|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|",r_time,r_pressure,r_temperature,r_mode,r_incoming,r_phi,r_theta,r_psi,r_sax,r_say,r_saz,r_sp,r_sq,r_sr,r_ae[0],r_ae[1],r_ae[2],r_ae[3],r_lift,r_roll,r_pitch,r_yaw);
				

	
}	
		