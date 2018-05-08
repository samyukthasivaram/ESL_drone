#ifndef _RS232_H_
#define _RS232_H_

#define StartByte 0xFF
#define CRC1
#define CRC2
#define StopByte  0xFF
#define DataLen 10
#define DataSize	10

void rs232_init(void);
bool rs232_read(int p[DataSize]);
bool check_CRC(int c1, int c2);
int store_data(int p[DataSize], queue q);

#endif
