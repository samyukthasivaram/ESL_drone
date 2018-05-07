#ifndef _RS232_H_
#define _RS232_H_

#define StartByte 0xFF
#define CRC1
#define CRC2
#define StopByte  0xFF
#define DataLen 10
#define size	5

typedef struct{
	uint8_t Mode[size];
	uint16_t Lift[size];
	uint16_t Roll[size];
	uint16_t Pitch[size];
	uint16_t Yaw[size];
	uint8_t Keyboard[size];
	int count;
}Param;

void rs232_init(void);
void rs232_read(Param p, int c);
bool check_CRC(int c1, int c2);
Param store_data(Param p, queue q, int i);

#endif
