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

#define NAME_LENGTH 128
#define POLY 0x8005
int charsWaiting (int fd)
{
  int count ;

  if (ioctl (fd, FIONREAD, &count) == -1)
  {
    perror ("Something went wrong") ;
    exit (EXIT_FAILURE) ;
  }
  return count ;
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
/*
//CRC calculation with LUT
unsigned cal_crc(unsigned int *ptr, unsigned int len) {
 unsigned int crc;
 unsigned char da;
 unsigned int crc_ta[16]={ 
 0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
 };
 crc=0;
 while(len--!=0) {
 da=((uint)(crc/256))/16; 
 crc<<=4; 
 crc^=crc_ta[da^(*ptr/16)]; 
 da=((uint)(crc/256))/16; 
 crc<<=4; 
 crc^=crc_ta[da^(*ptr&0x0f)];
 ptr++;
 }
 return(crc);
}*/

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
#include <termios.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

int serial_device = 0;
int fd_RS232;

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
	char	c;
	//int *tx_frame;
	term_puts("\nTerminal program - Embedded Real-Time Systems\n");
	int fd;
        #define JS_DEV	"/dev/input/js0"
	if ((fd = open(JS_DEV, O_RDONLY)) < 0) {
		perror("joystick error");
		exit(1);
	}

	term_initio();
	rs232_open();

	term_puts("Type ^C to exit\n");

	/* discard any incoming text
	 */
	while ((c = rs232_getchar_nb()) != -1)
		fputc(c,stderr);

	/* send & receive
	 */
	for (;;)
	{
	int16_t roll=0, pitch=0, yaw=0,lift=0;
	int abort;
	//int8_t temp_1,temp_2;
	int flag_mode = 0;
	static int8_t mode = 0;
	int8_t start=0xFF; 
	uint8_t c=0;
	struct termios options;
	int count=0;
	int8_t keyboard=0xF0;
	int	axis[6];
	int	button[12];
	int tx_buffer[13];
	int16_t crc=0x0000;
	struct js_event js;
	
	// input from joystick 


	// non-blocking mode
	 
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
			{mode= 0x09;
			flag_mode = 1;
			}	
		else 
		{
		pitch = axis[0];
		roll = axis[1];
		yaw = axis[2];
		lift = axis[3];
		}

             
		//lift = -15000;
		//roll = -20000;
		//pitch = -32000;
		//yaw = -10000;
                //printf("%d|%d|%d|%d|\t\n",pitch,roll,yaw,lift);
			
		//keyboard press 
		if(flag_mode!=1) // does not take input from the keyboard if aborted in the joystick
		{		
			/*tcgetattr (fileno (stdin), &options) ;
			cfmakeraw (&options) ;	// Note - also affects output
			tcsetattr (fileno (stdin), TCSANOW, &options) ;
			count = charsWaiting (fileno (stdin)) ;
			c=getchar();*/
			
				int temp[3];
				for(int i=0; i<3; i++)
				{temp[i]= getchar ();
				//printf("temp[%d]=%d \t",i,temp[i]);
				}
				if(temp[0]== 0x1B && (temp[1])== 0x5B)
					c = temp[2];
				else 
					c = temp[0];
			        //printf("c=%d\n",c);
				
			switch (c)
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
				case 0x1B:
					mode = 9;
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
				break;
				default:
					
					keyboard = 0xF0;
			}
										
		}
		else
		{
			keyboard = 0xF0;
		}	
			//printf("mode =%d |KEYBOARD=%d \t",mode,keyboard);
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
			
			/*tx_buffer[0]= start;
			tx_buffer[1]= 85;
			tx_buffer[2]= 20;
			tx_buffer[3]= 21;
			tx_buffer[4]= 22;
			tx_buffer[5]= 23;
			tx_buffer[6]= 24;
			tx_buffer[7]= 25;
			tx_buffer[8]= 26;
			tx_buffer[9]= 27;
			tx_buffer[10]=28;*/
			//update crc
			crc=calc_crc(tx_buffer);      //calculate crc without LUT
			//crc=cal_crc(&tx_buffer,10);      //with LUT
			//crc=0xFFFF;
			//printf("crc=%d\t",crc);
			//temp_1= (uint8_t)crc;
			//temp_2=(uint8_t)(crc>>8); 
			tx_buffer[11]= (int8_t)crc;
			tx_buffer[12]=(int8_t)(crc>>8);; 

		
	
	for(int k=0; k<13; k++)
	{
			//tx_buffer[k]=0xFF;
			rs232_putchar(tx_buffer[k]);//usleep(10);
			//printf("tx[%d]=%d | \t ",k,tx_buffer[k]);
	}
	
	for(int kk=0; kk<22; kk++)
	{
		if ((c = rs232_getchar_nb()) != -1)  //possible cause of delay
			//term_putchar(c);
			{
				printf("%d | ", c);
			}
	}
		printf("\n");
	}

	term_exitio();
	rs232_close();
	term_puts("\n<exit>\n");

	return 0;
}
