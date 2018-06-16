/*----------------------------------------------------------------------------------------------------------
 *Assumption: main function uses state machine, every time the execution of this mode is over,
 *it will start again according to the command from PC.
 *Joystick passes the setpoint.
 *
 * Developed and tested by Samyuktha Sivaram
 * Embedded Software Lab
 * Date - 16.06.2018
 *----------------------------------------------------------------------------------------------------------
*/
#include "in4073.h"
void update_motors(void)
{					
	motor[0] = ae[0];
	motor[1] = ae[1];
	motor[2] = ae[2];
	motor[3] = ae[3];
}

void run_filters_and_control()
{

	update_motors();
}

/*----------------------------------------------------------------------------------------------------------
 * Function squareroot() - computes approximate square root of 2 byte data
 * Adopted from - http://www.microchip.com/forums/m577584.aspx
 * Return value - value - value whose square root needs to be computed
 * Arguments : int16_t - computed Squareroot
 *----------------------------------------------------------------------------------------------------------
 */

int16_t squareroot(int16_t value)
{
    unsigned Root = 0;
    unsigned Bit;
    if(value<0)	value = - value;
    for ( Bit = 0x4000; Bit > 0; Bit >>= 2 )
        {
        	unsigned Trial = Root + Bit;
            Root >>= 1;
            if ( Trial <= value )
            {
                Root += Bit;
                value -= Trial;
            }
        }
       // printf("%d",Root);		 
		return Root;
}

/*----------------------------------------------------------------------------------------------------------
 * Function manual_mode_sqrt() - Pass the received set points of roll, pitch, yaw and lift from
 *   							 Joystick to motor_control to calculate motor speed
 * Return value - void
 * Arguments : null
 *----------------------------------------------------------------------------------------------------------
 */
void manual_mode_sqrt()
{

int16_t man_roll=0,man_pitch=0,man_yaw=0,man_lift=0;

man_lift=lift;
man_roll=roll;
man_yaw=yaw;
man_pitch=pitch;

motor_control(man_lift,man_roll,man_pitch,man_yaw);

}

/*----------------------------------------------------------------------------------------------------------
 * Function yaw_control() - compute yaw value using setpoint from joystick and sensor value sr
 *   						Pass the computed value of yaw to motor_control to calculate motor speed
 * Return value - void
 * Arguments : null
 *----------------------------------------------------------------------------------------------------------
 */
 
void yaw_control()
{
int16_t yaw_roll=0,yaw_pitch=0,yaw_yaw=0,yaw_lift=0,yaw_sr,error = 0;

	yaw_lift=lift;
	yaw_roll=roll;
	yaw_yaw=yaw;
	yaw_pitch=pitch;
	
	yaw_sr=(sr>>6)-sr_c;
	error = yaw - yaw_sr;
	yaw_yaw = p_yaw * error;

/*Calculate motor RMP using updated yaw */
motor_control(yaw_lift,yaw_roll,yaw_pitch,yaw_yaw);

}

/*----------------------------------------------------------------------------------------------------------
 * Function full_control() - compute yaw, roll, pitch value using setpoint from joystick and sensor value sr, sp and sq
 *   						Pass the computed value of yaw. roll, pitch to motor_control to calculate motor speed
 * Return value - void
 * Arguments : null
 *----------------------------------------------------------------------------------------------------------
 */
 
void full_control()
{int16_t full_roll=0,full_pitch=0,full_yaw=0,full_lift=0,full_sr,full_sp,full_sq,error_yaw;

	full_lift=lift;
	full_roll=roll;
	full_yaw=yaw;
	full_pitch=pitch;
	
	full_sr=(sr>>6)-sr_c;
	full_sp=(sp>>6)-sp_c;
	full_sq=(sq>>6)-sq_c;
	
	error_yaw = yaw - full_sr;
	full_yaw = p_yaw * error_yaw;
	
	full_roll = (roll - ((phi>>6) - phi_c))*(P1>>2) - full_sp*P2;
	
	full_pitch = (pitch - ((theta>>6) - theta_c) )*(P1>>2) + (full_sq)*P2;

/*Calculate motor RMP using updated roll, pitch and yaw */
motor_control(full_lift,full_roll,full_pitch,full_yaw);
}
 
 /*----------------------------------------------------------------------------------------------------------
 * Function raw_mode() - compute yaw, roll, pitch value using setpoint from joystick and filtered value of sr, sp and sq
 *   						Pass the computed value of yaw. roll, pitch to motor_control to calculate motor speed
 * Return value - void
 * Arguments : null
 *----------------------------------------------------------------------------------------------------------
 */
 
void raw_mode()
{
int16_t raw_roll=0,raw_pitch=0,raw_yaw=0,raw_lift=0,raw_sr,raw_sp,raw_sq;
int16_t error_yaw;
raw_lift=lift;
raw_roll=roll;
raw_yaw=yaw;
raw_pitch=pitch;
	
	raw_sr=(sr>>6)-sr_c;
	error_yaw = yaw - raw_sr;

	raw_sp=(p_kalman>>6);

	raw_sq=(q_kalman>>6);

	raw_yaw = p_yaw * error_yaw;

	
	raw_roll = roll - (phi_kalman>>6)*P1 - raw_sp*P2;
	raw_pitch = pitch - (theta_kalman>>6)*P1 + (raw_sq)*P2;

motor_control(raw_lift,raw_roll,raw_pitch,raw_yaw);

}

/*----------------------------------------------------------------------------------------------------------
 * Function motor_control() - computes the rotor rpm with the given values of lift, roll, pitch and yaw
 *
 * Return value - void
 * Arguments : lift, roll, pitch, yaw received from the mode function
 *----------------------------------------------------------------------------------------------------------
 */
void motor_control(int16_t t_lift, int16_t t_roll,int16_t t_pitch, int16_t t_yaw)
{   int16_t w1_sqr = 0;
	int16_t w2_sqr = 0;
	int16_t w3_sqr = 0;
	int16_t w4_sqr = 0;

		
    if(t_lift<277)
    {
	ae[0] = squareroot(t_lift) * Motor_const ;
	ae[1] = squareroot(t_lift) * Motor_const ;
	ae[2] = squareroot(t_lift) * Motor_const ;
	ae[3] = squareroot(t_lift) * Motor_const ;

    }

    else
    {

    w1_sqr= (int16_t) (-t_yaw + t_lift + 2*t_pitch) ;
	w3_sqr= (int16_t) (-t_yaw + t_lift  - 2*t_pitch);
	w2_sqr= (int16_t) (t_yaw + t_lift  - 2*t_roll) ;	
	w4_sqr= (int16_t) (t_yaw + t_lift  + 2*t_roll) ;
	ae[0] = squareroot(w1_sqr) * Motor_const ;
	ae[1] = squareroot(w2_sqr) * Motor_const ;
	ae[2] = squareroot(w3_sqr) * Motor_const ;
	ae[3] = squareroot(w4_sqr) * Motor_const ;
	
/* Cap the rotor rpm to min of 200 and max of 600 */
 if (ae[0] < 200) ae[0] = 200; 
 if (ae[0] > 600) ae[0] = 600;
 if (ae[1] < 200) ae[1] = 200; 
 if (ae[1] > 600) ae[1] = 600;
 if (ae[2] < 200) ae[2] = 200; 
 if (ae[2] > 600) ae[2] = 600;
 if (ae[3] < 200) ae[3] = 200; 
 if (ae[3] > 600) ae[3] = 600;
    }
	
	
}