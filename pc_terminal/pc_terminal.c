/*------------------------------------------------------------
 * Simple pc terminal in C
 *
 * Arjan J.C. van Gemund (+ mods by Ioannis Protonotarios)
 *
 * read more: http://mirror.datenwolf.net/serial/
 *------------------------------------------------------------
 */

#include <inttypes.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <sys/stat.h>
#include "joystick.h"
#include "crc.h"
#include <termios.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

int serial_device = 0;
int fd_RS232;

#define NAME_LENGTH 128
#define POLY 0x8005




static int Datasize=0;
/*---------------------------------------------------------
 *Data format: | StartByte | Data | CRC1 | CRC2 | 14 bytes
  ---------------------------------------------------------
 */

//CRC calculation without table
int16_t crc_pc(int frame[],int count)

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

int check_crc(int8_t c1, int8_t c2, int q[])
{
   int16_t w= crc_pc(q,Datasize);
   int16_t new_c = ((c2<<8) & 0xFF00) | (c1 & 0x00FF);
  if (w==new_c)
     return 1;
  else
    return 0;
}

int frame[50];
uint8_t count=0;
int rx_wrong[50];
int8_t StartByte=0xFF;
static int8_t check1 = 0,check2 = 0;	//store CRC

void rs232_pc(uint8_t c)
{
	static int state = 0;
	int i=0,j = 0;	
	
	
			//data = dequeue(&rx_queue); rec_counter=0;
			//printf("drone");
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
					Datasize = 51;
					
				}
				else	state = 0;
				break;
				
			case 1:
				if(count >=Datasize)
				{
					//printf("hi123\n");
					check1 = c;
					count = 0;
					state = 2;
				}
				else
				{
					//printf("hi\n");
					frame[count] = c;
					count++;
					state = 1;
				}
				break;
			

			case 2:
				check2 = c;
				if(check_crc(check1,check2,frame))//check CRC <TO DO>
				{
					
					state = 0;
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
					{	
						for(i=0; i<10; i++)  
						{
							if(frame[i] == StartByte)
							{	
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
			

					}


				}

				break;
			
			default: printf("deafult case");
					break;
		}
	
	
}



void log_data(int read_buffer[50])
{
	/*for(int i=0;i<37;i++)
					printf("%d|", read_buffer[i]);
					printf("\n");*/
	
	
	uint32_t r_time=0,r_pressure=0,r_temperature=0;
	int8_t r_mode=0, r_incoming=0;//incoming: joystick or keyboard
	int16_t r_phi=0, r_theta=0, r_psi=0, r_sax=0, r_say=0, r_saz=0, r_sp=0, r_sq=0, r_sr=0, r_lift=0,r_roll=0,r_pitch=0,r_yaw=0;
	int16_t r_ae[4]={0}, r_bat=0, r_p=0, r_p1=0, r_p2=0;
	
	
			
	if(Datasize==51)
	{		
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
			r_lift = (read_buffer[40] << 8) + read_buffer[41];
			r_roll = (read_buffer[42] << 8) + read_buffer[43];
			r_pitch = (read_buffer[44] << 8) + read_buffer[45];
			r_yaw = (read_buffer[46] << 8) + read_buffer[47];
			r_p = read_buffer[48];
			r_p1 = read_buffer[49];
			r_p2 = read_buffer[50];

		printf("%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|\n",r_mode,r_ae[0],r_ae[1],r_ae[2],r_ae[3],r_phi,r_theta,r_psi,r_sax,r_say,r_saz,r_sp,r_sq,r_sr,r_lift,r_roll,r_pitch,r_yaw,r_bat,r_p,r_p1,r_p2);
	
		/*	FILE *fp;
			fp = fopen("data.csv", "w+");
			if (!fp) 
			{
				printf("creat file error");
			}
			else 
			{
			
				fprintf(fp,"%10ld,%10ld,%10ld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",r_time,r_pressure,r_temperature,r_mode,r_incoming,r_phi,r_theta,r_psi,r_sax,r_say,r_saz,r_sp,r_sq,r_sr,r_ae[0],r_ae[1],r_ae[2],r_ae[3],r_lift,r_roll,r_pitch,r_yaw,r_p,r_p1,r_p2);
				//fwrite(&r, sizeof(r), 1, fp);
			    //fprintf(f, "%s","\n");
				fclose(fp);
			}*/
	}		
	else if(Datasize==40)
	{
			r_mode = read_buffer[0];
			r_ae[0] = (read_buffer[2] << 8) | read_buffer[1];
			r_ae[1] = (read_buffer[4] << 8) | read_buffer[3];
			r_ae[2] = (read_buffer[6] << 8) | read_buffer[5];
			r_ae[3] = (read_buffer[8] << 8) | read_buffer[7];	
			r_bat = (read_buffer[10] << 8) | read_buffer[9];
			r_phi = (read_buffer[12] << 8 )| read_buffer[11];
			r_theta = (read_buffer[14] << 8) | read_buffer[13];
			r_psi = (read_buffer[16] << 8 )| read_buffer[15];
			r_sp= (read_buffer[18]<< 8) | read_buffer[17];
			r_sq = (read_buffer[20] << 8 )| read_buffer[19];
			r_sr = (read_buffer[22] << 8) | read_buffer[21];
			r_sax = (read_buffer[24] << 8 )| read_buffer[23];
			r_say =( read_buffer[26] << 8) | read_buffer[25];
			r_saz = (read_buffer[28] << 8 )| read_buffer[27];
			r_lift = (read_buffer[30] << 8) | read_buffer[29];
			r_roll = (read_buffer[32] << 8) | read_buffer[31];
			r_pitch = (read_buffer[34] << 8) | read_buffer[33];
			r_yaw = (read_buffer[36] << 8) | read_buffer[35];
			r_p = read_buffer[37];
			r_p1 = read_buffer[38];
			r_p2 = read_buffer[39];
			//printf("%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|\n",r_mode,r_ae[0],r_ae[1],r_ae[2],r_ae[3],r_phi,r_theta,r_psi,r_sax,r_say,r_saz,r_sp,r_sq,r_sr,r_lift,r_roll,r_pitch,r_yaw,r_bat,r_p,r_p1,r_p2);
	}		

	
}	
		
//CRC calculation without LUT
int16_t calc_crc(int frame[])

{
    int16_t crc,byte;
    crc = 0;
    
    for (int i=1; i< 11; i++)
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


/*------------------------------------------------------------
 * console I/O
 *------------------------------------------------------------
 */
struct termios 	savetty;

void	term_initio()
{
	struct termios tty;

	tcgetattr(0, &savetty);
	tcgetattr(0, &tty);

	tty.c_lflag &= ~(ECHO|ECHONL|ICANON|IEXTEN);
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = 0;

	tcsetattr(0, TCSADRAIN, &tty);
}

void	term_exitio()
{
	tcsetattr(0, TCSADRAIN, &savetty);
}

void	term_puts(char *s)
{
	fprintf(stderr,"%s",s);
}

void	term_putchar(char c)
{
	putc(c,stderr);
}

int	term_getchar_nb()
{
        static unsigned char 	line [2];

        if (read(0,line,1)) // note: destructive read
        		return (int) line[0];

        return -1;
}

int	term_getchar()
{
        int    c;

        while ((c = term_getchar_nb()) == -1)
                ;
        return c;
}

/*------------------------------------------------------------
 * Serial I/O
 * 8 bits, 1 stopbit, no parity,
 * 115,200 baud
 *------------------------------------------------------------
 */


void rs232_open(void)
{
  	char 		*name;
  	int 		result;
  	struct termios	tty;

       	fd_RS232 = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);  // Hardcode your serial port here, or request it as an argument at runtime

	assert(fd_RS232>=0);

  	result = isatty(fd_RS232);
  	assert(result == 1);

  	name = ttyname(fd_RS232);
  	assert(name != 0);

  	result = tcgetattr(fd_RS232, &tty);
	assert(result == 0);

	tty.c_iflag = IGNBRK; /* ignore break condition */
	tty.c_oflag = 0;
	tty.c_lflag = 0;

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; /* 8 bits-per-character */
	tty.c_cflag |= CLOCAL | CREAD; /* Ignore model status + read input */

	cfsetospeed(&tty, B115200);
	cfsetispeed(&tty, B115200);

	tty.c_cc[VMIN]  = 0;
	tty.c_cc[VTIME] = 0; // added timeout   <if 1 adds 100ms delay>

	tty.c_iflag &= ~(IXON|IXOFF|IXANY);

	result = tcsetattr (fd_RS232, TCSANOW, &tty); /* non-canonical */

	tcflush(fd_RS232, TCIOFLUSH); /* flush I/O buffer */
}


void 	rs232_close(void)
{
  	int 	result;

  	result = close(fd_RS232);
  	assert (result==0);
}


int	rs232_getchar_nb()
{
	int 		result;
	unsigned char 	c;

	result = read(fd_RS232, &c, 1);

	if (result == 0)
		return -1;

	else
	{
		assert(result == 1);
		return (int) c;
	}
}


int 	rs232_getchar()
{
	int 	c;

	while ((c = rs232_getchar_nb()) == -1)
		;
	return c;
}


int 	rs232_putchar(char c)
{
	int result;

	do {
		result = (int) write(fd_RS232, &c, 1);
	} while (result == 0);

	assert(result == 1);
	return result;
}


/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */
int main(int argc, char **argv)
{      
	char c;
	static int8_t mode = 0;
	int16_t roll=0, pitch=0, yaw=0,lift=0;
	int	axis[6];
	int	button[12];
	int tx_buffer[13];
	struct js_event js;
	struct termios options;
	int8_t start=0xFF; 
	int8_t keyboard=0xF0;
	int16_t crc=0x0000;
	int flag_mode = 0;
	int msec = 0, trigger = 14;
	int r_msec = 0, r_trigger = 4;
	clock_t r_previous = clock();
	clock_t previous = clock();
	term_puts("\nTerminal program - Embedded Real-Time Systems\n");
	term_puts("Type ^C to exit\n");

	/*int fd;
        #define JS_DEV	"/dev/input/js0"
	if ((fd = open(JS_DEV, O_RDONLY)) < 0) {
		perror("joystick error");
		exit(1);
	}*/

	term_initio();
	rs232_open();

	/* discard any incoming text
	 */
	while ((c = rs232_getchar_nb()) != -1)
		fputc(c,stderr);

	/* send & receive
	 */

		lift = 32767;
		roll = 32767;
		pitch = 32767;
		yaw = 32767;
	for (;;)
	{	
	flag_mode = 0;		
	uint8_t key_press=0;	
	keyboard=0xF0;
	uint8_t c = 0;
	// input from joystick 

	// non-blocking mode
/*	 
	fcntl(fd, F_SETFL, O_NONBLOCK);
		
			unsigned int	t, i;
	
			while (read(fd, &js, sizeof(struct js_event)) == 
		       			sizeof(struct js_event))  {

			// register data
			 
			// fprintf(stderr,".");
			switch(js.type & ~JS_EVENT_INIT) {
				case JS_EVENT_BUTTON:
					button[js.number] = js.value;
					break;
				case JS_EVENT_AXIS:
					axis[js.number] = js.value;
					break;
			}
		}
		if (errno != EAGAIN) {
			perror("\njs: error reading (EAGAIN)");
			exit (1);
		}		
	
		if(button[0]==1)
			{mode= 1;
			flag_mode = 1;
			}	
		else 
		{
		roll = axis[0];
		pitch = axis[1];
		yaw = axis[2];
		lift = axis[3];
		}
*/
       /*   for(lift=-32767;lift<32767;lift+=10000)
		  	   for(roll=-32767;roll<32767;lift+=10000)
			       for(pitch=-32767;pitch<32767;pitch+=10000)
			  		for(yaw=-32767;yaw<32767;yaw+=10000)
		{*/

        //printf("%d|%d|%d|%d|\t\n",pitch,roll,yaw,lift);
		clock_t current = clock();
 		clock_t difference=current-previous;
  		msec = difference * 1000 / CLOCKS_PER_SEC;

		if ( msec > trigger )
		{
			previous=current;


	
		//keyboard press 
		if(flag_mode!=1) // does not take input from the keyboard if aborted in the joystick
		{				
				int temp[3];
				for(int i=0; i<3; i++)
				{temp[i]= getchar ();
				//printf("temp[%d]=%d \t",i,temp[i]);
				}
				if(temp[0]== 0x1B && (temp[1])== 0x5B)
					key_press = temp[2];
				else 
					key_press = temp[0];
			        //printf("c=%d\n",c);
				
			switch (key_press)
			{
				case 0x30:

					mode = 0;
				break;
				case 0x31:
					mode = 1;
				break;
				case 0x32:
					mode = 2;
				break;
				case 0x33:
					mode = 3;
				break;
				case 0x34:
					mode = 4;
				break;
				case 0x35:
					mode = 5;
				break;
				case 0x36:
					mode = 6;
				break;
				case 0x37:
					mode = 7;
				break;
				case 0x38:
					mode = 8;
				break;
				case 0x1B://ecs
					mode = 1;
				break;
				case 0x61: //a
					keyboard = 0x00;
				break;
				case 0x7A: //z
					keyboard = 0x01;
				break;
				case 0x44: //left arrow
					keyboard = 0x02;
				break;
				case 0x43: // right arrow
					keyboard = 0x03;
				break;
				case 0x41: //up arrow
					keyboard = 0x04;
				break;
				case 0x42: // down arrow
					keyboard = 0x05;
				break;
				case 0x71: //q
					keyboard = 0x06;
				break;
				case 0x77: // w
					keyboard = 0x07;
				break;
				case 0x75:  //u
					keyboard = 0x08;
				break;
				case 0x6A: // j
					keyboard = 0x09;
				break;
				case 0x69: //i
					keyboard = 0x0A;
				break;
				case 0x6B: //k
					keyboard = 0x0B;
				break;
				case 0x6F: //o
					keyboard = 0x0C;
				break;
				case 0x6C: //l
					keyboard = 0x0D;
				case 0x66: //f  (flash)
					keyboard = 0x0E;
				break;
				default:
					
					keyboard = 0xF0;
			}
										
		}
		else
		{
			keyboard = 0xF0;
		}	

		
			//printf("mode =%d |KEYBOARD=%d \t\n",mode,keyboard);
			// frame update
			
			tx_buffer[0]= start;
			tx_buffer[1]= mode;
			tx_buffer[2]= (int8_t)lift;
			tx_buffer[3]= (int8_t)(lift>>8);
			tx_buffer[4]= (int8_t)roll;
			tx_buffer[5]= (int8_t)(roll>>8);
			tx_buffer[6]= (int8_t)pitch;
			tx_buffer[7]= (int8_t)(pitch>>8);
			tx_buffer[8]= (int8_t)yaw;
			tx_buffer[9]= (int8_t)(yaw>>8);
			tx_buffer[10]= keyboard;			
			
			//update crc
			crc=calc_crc(tx_buffer);      //calculate crc without LUT
			tx_buffer[11]= (int8_t)crc;
			tx_buffer[12]=(int8_t)(crc>>8);
			
		for(int k=0; k<13; k++)
		{
			rs232_putchar(tx_buffer[k]);
			//printf("tx[%d]=%d | \t ",k,tx_buffer[k]);
		}


		/*lift += 1000;
		roll+= 1000;
		pitch+=1000;
		yaw+=1000;*/

				

	}

		clock_t r_current = clock();
 		clock_t r_difference=r_current-r_previous;
  		r_msec = r_difference * 1000 / CLOCKS_PER_SEC;

		if ( r_msec > r_trigger )
		{
			r_previous=r_current;	
	if ((c = rs232_getchar_nb()) != -1)  //possible cause of delay
		{rs232_pc(c);
			//printf("%d",c);
	    //term_putchar(c);
		}
		}
	
	}
					
	term_exitio();
	rs232_close();
	term_puts("\n<exit>\n");

	return 0;
}
