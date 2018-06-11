int16_t calc_crc(int frame[]);
int16_t crc_pc(int frame[],int count);
int check_crc(int8_t c1, int8_t c2, int q[]);

void rs232_pc(uint8_t c);

void log_data(int read_buffer[50]);