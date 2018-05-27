#ifndef _RS232_H_
#define _RS232_H_

#define DataLen 10

void rs232_write();
void rs232_init(void);
void rs232_read();
bool check_CRC(int8_t c1, int8_t c2, int q[]);
void store_data(int p[DataLen]);

#endif
