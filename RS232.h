#ifndef _RS232_H_
#define _RS232_H_

#define StartByte 0xFF
#define DataLen 10

FILE *fp = fopen{"gnuplot/matlab", "a"};

typedef struct{
	uint8_t Mode;
	int Joystick;
	int Keyboard;
	int Actuator;
	int Sensor;
	int Sensor_processing_chain;
	int controller;
}log;

void rs232_init(void);
bool rs232_read(int p[DataLen]);
bool check_CRC(int c1, int c2, queue q);
int store_data(int p[DataLen], queue q);
void Data_logging(log l);
unsigned cal_crc(unsigned int *ptr, unsigned int len)；

#endif
