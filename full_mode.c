#include"in4073.h"
#include"joystick.h"




/* Local variables */
static uint16_t cmd_thrust = 0;                     // The thrust command
static int16_t cmd_roll, cmd_pitch, cmd_yaw = 0;    // The roll, pitch, yaw command
static uint16_t sp_thrust = 0;                      // The thrust setpoint
static int16_t sp_roll, sp_pitch, sp_yaw = 0;       // The roll, pitch, yaw setpoint

static uint8_t gyaw_d = 0;                          // The yaw rate control gains
static uint8_t g_angle_d = 0;                       // The attitude control gains
static uint8_t g_rate_d = 0;                        // The attitude rate control gains
static uint32_t current_panic = 0;


void set_control_gains(uint8_t yaw_d, uint8_t g_angle, uint8_t g_rate) {
	P = yaw_d;
	P1 = g_angle;
	P2 = g_rate;
}

void mode_FULL() {
	
		if (init_raw == true) //raw control
		{
			roll_control = (roll - (phi))*P1   - estimated_p*P2;
			pitch_control = (pitch - (theta))*P1 - estimated_q*P2 ;
			yaw_control = (yaw  - r_butter )* P;
		}
		else
			// do the dmp control
		{
			roll_control = (roll - (phi - ave_phi))*P1 - (sp - ave_gyro[0])*P2;
			pitch_control = (pitch - (theta - ave_theta) )*P1 + ((sq - ave_gyro[1]))*P2;
			yaw_control = (yaw  + (sr - ave_gyro[2]))* P;
		}
	//motor_mixing(cmd_thrust, cmd_roll, cmd_pitch, cmd_yaw);




}