/*------------------------------------------------------------
 * - pc_terminal.c
 * 
 * -This file has 2 parts - Data processing (Data logging 
 *  and telemetry)and Input processing
 *
 * Developed and tested by Samyuktha Sivaram
 * Embedded Software Lab
 * Date - 16.06.2018
 *------------------------------------------------------------
 */

#include "pc_terminal.h"


int serial_device = 0;
int fd_RS232;  							/*file for writing joystick values - 'data.csv' */
FILE *fp;								/*file for data logging - 'data.csv' */
FILE *ftele;
static int Datasize=0; 					/*size of payload for data processing*/
int frame[51];							/* payload received */
uint8_t count=0;						/* count the number of bytes received via RS232*/
int rx_wrong[51];						/* temporary buffer to store data during header synchronisation*/
int8_t StartByte=0xFF;					/* start byte of Pc to drone frame */
static int8_t check1 = 0,check2 = 0;	/*store the 2 bytes of CRC received via RS232 */

/*------------------------------------------------------------
 * Function crc_pc() - compute CRC for an array of values
 * Return value - 2 bytes of calculated CRC
 * Arguments : Frame[] - Array for which the CRC needs to be computed
 * 			   Count - The number of elements in the array
 * 
 *------------------------------------------------------------
 */
int16_t crc_pc(int frame[],int count_crc)

{	
    int16_t crc,byte;
    crc = 0;    
    for (int i=0; i< count_crc; i++)
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

/*----------------------------------------------------------------------------------------------------------
 * Function check_crc() - compute CRC for an array of values and check if it is equal to the received CRC 
 * Return value - 1 if the received and calculated CRC are equal
 *				- 0 if the received and calculated CRC are not equal
 * Arguments : c1 - First byte of the received CRC
 * 			   c2 - Second byte of the received CRC
 * 			   q[]- The array of data for which the CRC needs to be computed
 *----------------------------------------------------------------------------------------------------------
 */
 
int check_crc(int8_t c1, int8_t c2, int q[])
{
   int16_t w= crc_pc(q,Datasize);
   int16_t new_c = ((c2<<8) & 0xFF00) | (c1 & 0x00FF);

  if (w==new_c) 
	 return 1;
  else
   return 0;
}

/*----------------------------------------------------------------------------------------------------------
 * Function rs232_pc() - Decode the data received from drone via the RS232 
 *                        using state machine (telemetry and logging)
 * Return value - void
 * Arguments : c - 1 byte of the received data via RS232
 *----------------------------------------------------------------------------------------------------------
*/

void rs232_pc(uint8_t c)
{
	static int state = 0;	
	
		switch(state)
		{
			/* State 0 - Initial state till the start byte is received*/
			case 0:
				if(c == 0xFA)
				{
					state = 1;
					Datasize = 40;  	//telemetry payload = 40 
				}
				else if(c==0xFB)
				{
					state = 1;
					Datasize = 51;  	//logging payload = 51
					
				}
				else	state = 0;
				break;
				
			/*State 1 - Receive the payload and the first byte of CRC */	
			case 1:
				if(count >=Datasize)
				{
					check1 = (int8_t)c;
					count = 0;
					state = 2;
				}
				else
				{					
					frame[count] = (int8_t)c;
					count++;
					state = 1;
				}
				break;
			
			/*State 2 - Receive the second byte of CRC and store the payload if CRC matches*/
			case 2:
				check2 = (int8_t)c;
				if(check_crc(check1,check2,frame))			
				{
					state = 0;
					log_data(frame);
				}
				
				/*if CRC is wrong go to state 0*/
				else    
				   state=0;  
				break;			
			default: printf("deafult case");
					break;
		}		
}

/*----------------------------------------------------------------------------------------------------------
 * Function log_data() - Print the payload on the screen for telemetry and 
 *							store the payload in a file for data logging
 * Return value - void
 * Arguments : read_buffer[50] - Payload for telemerty and data processing
 *----------------------------------------------------------------------------------------------------------
*/

void log_data(int read_buffer[50])
{
	uint32_t r_time=0,r_pressure=0,r_temperature=0;
	int8_t r_mode=0, r_incoming=0;
	int16_t r_phi=0, r_theta=0, r_psi=0, r_sax=0, r_say=0, r_saz=0, r_sp=0, r_sq=0, r_sr=0, r_lift=0,r_roll=0,r_pitch=0,r_yaw=0;
	int16_t r_ae[4]={0}, r_p=0, r_p1=0, r_p2=0;
	uint16_t  r_bat=0;
	
	/* Data for Data logging*/		
	if(Datasize==51)
	{		
			r_time = ((uint32_t)(read_buffer[0] << 24)&(0xFF000000)) + ((uint32_t)(read_buffer[1] << 16)&(0x00FF0000)) + ((uint32_t)(read_buffer[2] << 8)&(0x0000FF00)) +((uint32_t)read_buffer[3]&(0x000000FF));
			r_mode = read_buffer[4];
			r_incoming = read_buffer[5];//keyboard
			r_phi = ((read_buffer[6] << 8 )&(0xFF00))+ (read_buffer[7]& 0x00FF);
			r_theta = ((read_buffer[8] << 8)&(0xFF00)) + (read_buffer[9]& 0x00FF);
			r_psi = ((read_buffer[10] << 8 )&(0xFF00))+ (read_buffer[11]& 0x00FF);
			r_sax = ((read_buffer[12] << 8 )&(0xFF00))+ (read_buffer[13]& 0x00FF);
			r_say =( (read_buffer[14] << 8)&(0xFF00)) + (read_buffer[15]& 0x00FF);
			r_saz = ((read_buffer[16] << 8 )&(0xFF00))+ (read_buffer[17]& 0x00FF);
			r_sp= ((read_buffer[18]<< 8)&(0xFF00))+(read_buffer[19]& 0x00FF);
			r_sq = ((read_buffer[20] << 8 )&(0xFF00))+ (read_buffer[21]& 0x00FF);
			r_sr = ((read_buffer[22] << 8) &(0xFF00))+ (read_buffer[23]& 0x00FF);
			r_pressure = ((uint32_t)(read_buffer[24] << 24)&(0xFF000000)) + ((uint32_t)(read_buffer[25] << 16)&(0x00FF0000)) + ((uint32_t)(read_buffer[26] << 8)&(0x0000FF00)) +((uint32_t)read_buffer[27]&(0x000000FF));
			r_temperature = ((uint32_t)(read_buffer[28] << 24)&(0xFF000000)) + ((uint32_t)(read_buffer[29] << 16)&(0x00FF0000)) + ((uint32_t)(read_buffer[30] << 8)&(0x0000FF00)) +((uint32_t)read_buffer[31]&(0x000000FF));
			r_ae[0] = ((read_buffer[32] << 8)&(0xFF00)) + (read_buffer[33]& 0x00FF);
			r_ae[1] = ((read_buffer[34] << 8)&(0xFF00)) + (read_buffer[35]& 0x00FF);
			r_ae[2] = ((read_buffer[36] << 8)&(0xFF00)) + (read_buffer[37]& 0x00FF);
			r_ae[3] = ((read_buffer[38] << 8)&(0xFF00)) + (read_buffer[39]& 0x00FF);	
			r_lift = ((read_buffer[40] << 8)&(0xFF00)) + (read_buffer[41]& 0x00FF);
			r_roll = ((read_buffer[42] << 8)&(0xFF00)) + (read_buffer[43]& 0x00FF);
			r_pitch = ((read_buffer[44] << 8)&(0xFF00)) + (read_buffer[45]& 0x00FF);
			r_yaw = ((read_buffer[46] << 8)&(0xFF00)) + (read_buffer[47]& 0x00FF);
			r_p = read_buffer[48];
			r_p1 = read_buffer[49]>>2;
			r_p2 = read_buffer[50]>>2;

			printf("log:%10u|%10u|%10u|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|\n",r_time,r_pressure,r_temperature,r_mode,r_incoming,r_phi,r_theta,r_psi,r_sax,r_say,r_saz,r_sp,r_sq,r_sr,r_ae[0],r_ae[1],r_ae[2],r_ae[3],r_lift,r_roll,r_pitch,r_yaw,r_p,r_p1,r_p2);
		
			fp = fopen("data.csv", "a");
			if (!fp) 
			{
				printf("create file error");
			}
			else 
			{			
				fprintf(fp,"%10u,%10u,%10u,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",r_time,r_pressure,r_temperature,r_mode,r_incoming,r_phi,r_theta,r_psi,r_sax,r_say,r_saz,r_sp,r_sq,r_sr,r_ae[0],r_ae[1],r_ae[2],r_ae[3],r_lift,r_roll,r_pitch,r_yaw,r_p,r_p1,r_p2);
			    fprintf(fp, "%s","\n");
				fclose(fp);
			}
	}

	/* Data for Telemetry*/	
	else if(Datasize==40)
	{
			r_mode = read_buffer[0];
			r_ae[0] = ((read_buffer[2] << 8)&(0xFF00)) + (read_buffer[1]& 0x00FF);
			r_ae[1] = ((read_buffer[4] << 8)&(0xFF00)) + (read_buffer[3]& 0x00FF);
			r_ae[2] = ((read_buffer[6] << 8)&(0xFF00)) + (read_buffer[5]& 0x00FF);
			r_ae[3] = ((read_buffer[8] << 8)&(0xFF00)) + (read_buffer[7]& 0x00FF);	
			r_bat = ((read_buffer[10] << 8)&(0xFF00)) + (read_buffer[9]& 0x00FF);
			r_phi = ((read_buffer[12] << 8 )&(0xFF00))+ (read_buffer[11]& 0x00FF);
			r_theta = ((read_buffer[14] << 8)&(0xFF00)) + (read_buffer[13]& 0x00FF);
			r_psi = ((read_buffer[16] << 8 )&(0xFF00))+ (read_buffer[15]& 0x00FF);
			r_sp= ((read_buffer[18]<< 8) &(0xFF00))+ (read_buffer[17]& 0x00FF);
			r_sq = ((read_buffer[20] << 8 )&(0xFF00))+ (read_buffer[19]& 0x00FF);
			r_sr = ((read_buffer[22] << 8)&(0xFF00)) + (read_buffer[21]& 0x00FF);
			r_sax = ((read_buffer[24] << 8 )&(0xFF00))+ (read_buffer[23]& 0x00FF);
			r_say =( (read_buffer[26] << 8)&(0xFF00)) + (read_buffer[25]& 0x00FF);
			r_saz = ((read_buffer[28] << 8 )&(0xFF00))+ (read_buffer[27]& 0x00FF);
			r_lift = ((read_buffer[30] << 8)&(0xFF00)) + (read_buffer[29]& 0x00FF);
			r_roll = ((read_buffer[32] << 8)&(0xFF00)) + (read_buffer[31]& 0x00FF);
			r_pitch = ((read_buffer[34] << 8)&(0xFF00)) + (read_buffer[33]& 0x00FF);
			r_yaw = ((read_buffer[36] << 8)&(0xFF00))+ (read_buffer[35]& 0x00FF);
			r_p = read_buffer[37];
			r_p1 = read_buffer[38]>>2;
			r_p2 = read_buffer[39]>>2;
			
	
	
	
	printf("tele:%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|\n",r_mode,r_ae[0],r_ae[1],r_ae[2],r_ae[3],r_bat,r_phi,r_theta,r_psi,r_sp,r_sq,r_sr,r_sax,r_say,r_saz,r_lift,r_roll,r_pitch,r_yaw,r_p,r_p1,r_p2);
	/*ftele = fopen("data_print.txt", "a");
	if (!ftele) 
		{
		printf("creat file error");
		}
	else 
		{
		fprintf( ftele,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n ",r_mode,r_ae[0],r_ae[1],r_ae[2],r_ae[3],r_bat,r_phi,r_theta,r_psi,r_sp,r_sq,r_sr,r_sax,r_say,r_saz,r_lift,r_roll,r_pitch,r_yaw,r_p,r_p1,r_p2);
		fprintf(ftele, "%s","\n");
		fclose(ftele);
		}
		*/
		
	}	

}	
		
/*----------------------------------------------------------------------------------------------------------
 * Function calc_crc() - compute CRC for an array of values to be sent to the drone
 * Return value - 2 byte of calculated CRC
 * Arguments : frame[]- The array of data for which the CRC needs to be computed
 *----------------------------------------------------------------------------------------------------------
 */
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
	char c;										/*Store byte of data received via rs232 */
	static int8_t mode = 0;											
	int16_t roll=0, pitch=0, yaw=0,lift=0;							
	int	axis[6];								/*Joystick axes values */
	int	button[12];								/*Joystick buttons values*/
	int tx_buffer[13];							/*Transmit buffer */
	struct js_event js;
	struct termios options;
	int8_t start=0xFF; 
	int8_t keyboard=0xF0;						/*key press by user */
	int16_t crc=0x0000;							/* store CRC of the transmit frame */
	int flag_mode = 0;							/* Flag to check if aborted by user via Joystick */
	int msec = 0, trigger = 14;					/* Clock to transmit frame every 14 ms*/
	int r_msec = 0, r_trigger = 4;				/* Clock to receive byte every 3 ms*/
	clock_t r_previous = clock();
	clock_t previous = clock();
	term_puts("\nTerminal program - Embedded Real-Time Systems\n");
	term_puts("Type ^C to exit\n");
    
/*Open file for storing joystick values */

		int fd;
        #define JS_DEV	"/dev/input/js0"
	if ((fd = open(JS_DEV, O_RDONLY)) < 0) {
		perror("joystick error");
		exit(1);
	}

	term_initio();
	rs232_open();

	/* discard any incoming text
	 */
	while ((c = rs232_getchar_nb()) != -1)
		{
			fputc(c,stderr);
		}
	

	/*lift = 1000;
	roll = 1000;
	pitch = 1000;
	yaw = 1000;*/
		
	for (;;)
	{	
	flag_mode = 0;		
	uint8_t key_press=0;	
	keyboard=0xF0;
	uint8_t c = 0;
	
	/* input from joystick */

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
		// Abort using Joystick
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

         /*clock to transmit frame every 14ms*/
		clock_t current = clock();
 		clock_t difference=current-previous;
  		msec = difference * 1000 / CLOCKS_PER_SEC;

		if ( msec > trigger )
		{
			previous=current;
	
		/* keyboard press */
		
		if(flag_mode!=1) // does not take input from the keyboard if aborted in the joystick
		{				
				int temp[3];
				for(int i=0; i<3; i++)
				{temp[i]= getchar ();
				}
				if(temp[0]== 0x1B && (temp[1])== 0x5B)
					key_press = temp[2];
				else 
					key_press = temp[0];
				
			switch (key_press)
			{
				case 0x30: 			 //0 - safe mode
					mode = 0;
				break;
				case 0x31:			 //1 - panic mode
					mode = 1;
				break;
				case 0x32: 			 //2 - Manual mode
					mode = 2;
				break;
				case 0x33:			 //3 - Calibaration mode
					mode = 3;
				break;
				case 0x34:			 //4 - Yaw Control 
					mode = 4;
				break;
				case 0x35: 			 //5 - Full control
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
				case 0x1B: 			   //ecs - abort
					mode = 1;
				break;
				case 0x61: 			   //a - lift up
					keyboard = 0x00;
				break;
				case 0x7A: 			   //z - lift down
					keyboard = 0x01;
				break;
				case 0x44: 			   //left arrow - Roll up
					keyboard = 0x02;
				break;
				case 0x43:     		   //right arrow - Roll down
					keyboard = 0x03;
				break;
				case 0x41: 			   //up arrow - Pitch down
					keyboard = 0x04;
				break;
				case 0x42: 			   //down arrow - Pitch Up
					keyboard = 0x05;
				break;
				case 0x71:			   //q - Yaw down
					keyboard = 0x06;
				break;
				case 0x77: 			   //w - Yaw up
					keyboard = 0x07;
				break;
				case 0x75: 			   //u - Increase P
					keyboard = 0x08;
				break;
				case 0x6A: 			   // j - Decrease P
					keyboard = 0x09;
				break;
				case 0x69: 			   //i - Increase P1
					keyboard = 0x0A;
				break;
				case 0x6B: 			   //k - Decrease P1
					keyboard = 0x0B;
				break;
				case 0x6F: 			   //o - Increase P2
					keyboard = 0x0C;
				break;
				case 0x6C:			   //l - Decrease P2
					keyboard = 0x0D;
				break;
				case 0x66:             //f  (write to flash)
					keyboard = 0x0E;
				break;
				case 0x72  :           //r (read from flash)
					keyboard = 0x0F;
				break;
				default:
					keyboard = 0xF0;
			}
										
		}
		else
		{
			keyboard = 0xF0;
		}	

		
/*---------------------------------------------------------
 *Data format: | StartByte | Data | CRC1 | CRC2 | 13 bytes
  ---------------------------------------------------------
 */
			
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
			crc=calc_crc(tx_buffer);      
			tx_buffer[11]= (int8_t)crc;
			tx_buffer[12]=(int8_t)(crc>>8);
			
		/* Transmit frame via RS232 */ 	
		for(int k=0; k<13; k++)
		{
			rs232_putchar(tx_buffer[k]);
		}
	}
			
		
		if ((c = rs232_getchar_nb()) != -1)  
		{
		/*clock to receive 1 byte from the drone every 3ms*/
		clock_t r_current = clock();
 		clock_t r_difference=r_current-r_previous;
  		r_msec = r_difference * 1000 / CLOCKS_PER_SEC;
		if ( r_msec > r_trigger )
		{
			r_previous=r_current;
		if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c>='0' && c<='9')||c=='\n'||c=='-'||c=='|')
	    term_putchar(c);
		}
		#ifdef TELEMETRY
		rs232_pc(c);
		#else
		
		#endif
		}	
	}					
	term_exitio();
	rs232_close();
	term_puts("\n<exit>\n");

	return 0;
}
