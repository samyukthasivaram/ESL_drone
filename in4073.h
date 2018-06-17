/*------------------------------------------------------------------
 *  in4073.h -- defines, globals, function prototypes
 *
 *  Samyuktha Sivaram
 *  Embedded Software Lab
 *
 *  16.06.2018
 *------------------------------------------------------------------
 */

#ifndef IN4073_H__
#define IN4073_H__

#include<stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "ml.h"
#include "app_util_platform.h"
#include <math.h>
#include "RS232.h"
#define RED		22
#define YELLOW		24
#define GREEN		28
#define BLUE		30
#define INT_PIN		5
#define Motor_const 13

//global variables
bool demo_done;
int packet_drop,rec_counter;  /*packet_drop - count the number of consecutive packets dropped, rec_counter - count the number of consecutive incorrect crc */
uint8_t p_yaw,P1,P2;
int16_t sax_c, say_c, saz_c;
int16_t sp_c, sq_c, sr_c;
int16_t phi_c,theta_c,psi_c; 
int16_t roll,pitch,yaw,lift,lift_key,roll_key,pitch_key,yaw_key;
int16_t motor[4],ae[4];
int8_t keyboard,mode,prev_mode;


// Control and modes
void manual_mode_sqrt();
void motor_control(int16_t t_lift, int16_t t_roll,int16_t t_pitch, int16_t t_yaw);
void full_control();
void run_filters_and_control();
void yaw_control();
void raw_mode();

//safety 
void callibration_mode();
void safe_mode();
void panicmode();
void bat_chk();

//filters
int mul(float x, float y);
int16_t r_bf,p_kalman,q_kalman,phi_kalman,theta_kalman;
int	float2fix(double x);
void butterWorth_2ndOrder();
void butterWorth_1stOrder();
void kalmanFilter();

//data logging
int flag_logging;

// Timers
#define TIMER_PERIOD	50 //50ms=20Hz (MAX 23bit, 4.6h)
void timers_init(void);
uint32_t get_time_us(void);
bool check_timer_flag(void);
void clear_timer_flag(void);

// GPIO
void gpio_init(void);

// Queue
#define QUEUE_SIZE 256
typedef struct {
	uint8_t Data[QUEUE_SIZE];
	uint16_t first,last;
  	uint16_t count; 
} queue;
void init_queue(queue *q);
void enqueue(queue *q, char x);
char dequeue(queue *q);

// UART
#define RX_PIN_NUMBER  16
#define TX_PIN_NUMBER  14
queue rx_queue;
queue tx_queue;
uint32_t last_correct_checksum_time;
void uart_init(void);
void uart_put(uint8_t);

// TWI
#define TWI_SCL	4
#define TWI_SDA	2
void twi_init(void);
bool i2c_write(uint8_t slave_addr, uint8_t reg_addr, uint8_t length, uint8_t const *data);
bool i2c_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t length, uint8_t *data);

// MPU wrapper
int16_t phi, theta, psi;
int16_t sp, sq, sr;
int16_t sax, say, saz;
uint8_t sensor_fifo_count;
void imu_init(bool dmp, uint16_t interrupt_frequency); // if dmp is true, the interrupt frequency is 100Hz - otherwise 32Hz-8kHz
void get_dmp_data(void);
void get_raw_sensor_data(void);
bool check_sensor_int_flag(void);
void clear_sensor_int_flag(void);

// Barometer
int32_t pressure;
int32_t temperature;
void read_baro(void);
void baro_init(void);

// ADC
uint16_t bat_volt;
void adc_init(void);
void adc_request_sample(void);
void bat_chk();

// Flash
bool spi_flash_init(void);
bool flash_chip_erase(void);
bool flash_write_byte(uint32_t address, uint8_t data);
bool flash_write_bytes(uint32_t address, uint8_t *data, uint32_t count);
bool flash_read_byte(uint32_t address, uint8_t *buffer);
bool flash_read_bytes(uint32_t address, uint8_t *buffer, uint32_t count);

void save_data_in_flash();
void update_data();
void read_from_flash();
uint8_t data_buffer[51];

// BLE
queue ble_rx_queue;
queue ble_tx_queue;
volatile bool radio_active;
void ble_init(void);
void ble_send(void);

#endif // IN4073_H__
