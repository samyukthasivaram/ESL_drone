#ifndef _RS232_H_
#define _RS232_H_

#define StartByte 0xFF
#define DataLen 10

void rs232_init(void);
bool rs232_read(int p[DataLen]);
bool check_CRC(int c1, int c2, queue q);
int store_data(int p[DataLen], queue q);

#endif
