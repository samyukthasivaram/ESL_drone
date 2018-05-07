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
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <sys/stat.h>
#include <joystick.h>

#define NAME_LENGTH 128

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
	tty.c_cc[VTIME] = 1; // added timeout

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
	int *tx_frame;
	term_puts("\nTerminal program - Embedded Real-Time Systems\n");

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
		sint16_t roll=0, pitch=0, yaw=0;
	int abort;
	int flag_mode = 0;
	int mode = 0;
	int start=0xF0; 
	int stop=0xF0;
	int c=0;
	struct termios options;
	int count=0;
	int fd;
	int keyboard=0xF0;
	unsigned char axes = 2;
	unsigned char buttons = 2;
	int version = 0x000800;
	int tx_buffer[14];
	char name[NAME_LENGTH] = "Unknown";
	
	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(NAME_LENGTH), name);

	printf("Joystick (%s) has %d axes and %d buttons. Driver version is %d.%d.%d.\n",
		name, axes, buttons, version >> 16, (version >> 8) & 0xff, version & 0xff);
	printf("Testing ... (interrupt to exit)\n");
	
	struct js_event js;

		fcntl(fd, F_SETFL, O_NONBLOCK);

			// printf("no events ..\n");

			switch(js.type & ~JS_EVENT_INIT) {
			case JS_EVENT_BUTTON:
				//buttons_state ^= (1 << js_event.number);
				abort = js.value;
				if(abort==1)
				{	
				mode = 9;
				flag_mode=1;
				}
			break;
			case JS_EVENT_AXIS:
				if(js.type%3==0)
					roll = js.value;
				else if(js.type%3==1)
					pitch = js.value;
				else if (js.type%3==2)
					yaw = js.value;
			break;
			}
			
			while (read(fd, &js, sizeof(struct js_event)) == sizeof(struct js_event))  {
				printf("Event: type %d, time %d, number %d, value %d\n",
					js.type, js.time, js.number, js.value);
			}
			

			if (errno != EAGAIN) {
				perror("\njstest: error reading");
				exit (1);
			}
			
		//keyboard press 
		if(flag_mode!=1) // does not take input from the keyboard if aborted in the joystick
		{		
			tcgetattr (fileno (stdin), &options) ;
			cfmakeraw (&options) ;	// Note - also affects output
			tcsetattr (fileno (stdin), TCSANOW, &options) ;
			count = charsWaiting (fileno (stdin)) ;
			if(count==1)
			{ 
				c = getchar () ;
			}
			else if (count==3)
			{
				int temp[3];
				for(int i=0; i<3; i++)
				temp[i]= getchar ();
				if(temp[0]== 0x1B && (temp[1])== 0x5B)
				c = temp[2];
			}	
			else 
				c = 0x00;
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
			// frame update
			
			tx_buffer[0]= start;
			tx_buffer[1]= mode;
			tx_buffer[2]= (uint8)lift;
			tx_buffer[3]= (uint8)(lift>>8);
			tx_buffer[4]= (uint8)roll;
			tx_buffer[5]= (uint8)(roll>>8);
			tx_buffer[6]= (uint8)pitch;
			tx_buffer[7]= (uint8)(pitch>>8);
			tx_buffer[8]= (uint8)yaw;
			tx_buffer[9]= (uint8)(yaw>>8);
			tx_buffer[10]= keyboard;
			tx_buffer[11]= (uint8)crc;
			tx_buffer[12]=(uint8)(crc>>8);
			tx_buffer[13]= stop;			
	
	for(int k=0; k<14; k++)
	{
			rs232_putchar(tx_frame);
	}
	
		if ((c = rs232_getchar_nb()) != -1)
			term_putchar(c);
	
	usleep(10000);
		
	}

	term_exitio();
	rs232_close();
	term_puts("\n<exit>\n");

	return 0;
}

