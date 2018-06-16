/*
 * Function: Low-pass Butterworth filter (10Hz)
 * 1st order and 2nd order
 * use matlab to caiculate according to fs and fc
 * sampling frequency fs= 500Hz
 * cut-off frequency fc= 25Hz
 * sensor data is passed by a signed 16-bits integer
 * 16 bits, 8 bits fraction
 * sr-yaw, r_bf -ouput of butterworth filter
 */

#include <stdio.h>
#include <stdlib.h>
#include "in4073.h"

static float b[3] = {0.0461, 0.0923, 0.0461};
static float a[3] = {1.0000, -1.3073, 0.4918};
static float bz[2] = {0};
static float az[3] = {0};
static int16_t init[2] = {0};		// for 1st-order
static int32_t initx[3] = {0};		// for 2nd-order 
static int32_t inity[2] = {0};		// for 2nd-order 

//x0 is initialized as 0 and updated inside the loop
void butterworth_1stOrder()
{
	init[1] = sr ;
	r_bf = (int16_t)(init[1] + init[0]) / 2;
	init[0] = init[1];
}

void butterWorth_2ndOrder()
{
	initx[2] = sr << 8;
	int32_t y = r_bf << 8;
	az[0] = float2fix(a[0]);
	az[1] = float2fix(a[1]);
	az[2] = float2fix(a[2]);
	bz[0] = float2fix(b[0]);
	bz[1] = float2fix(b[1]);
	
	//y[n] = a0x[n] + a1x[n-1] + a2x[n-2] - b0y[n-1] - b1y[n-2]
	y = mul(az[0], initx[2]) + mul(az[1], initx[1]) + mul(az[2], initx[0]) 
		- mul(bz[0], inity[1]) - mul(bz[1], inity[0]);
	
	//update initials
	initx[0] = initx[1];	//x0 = x1;
	initx[1] = initx[2];	//x1 = x;
	inity[0] = inity[1];	//y0 = y1;
	inity[1] = y;			//y1 = y;
	r_bf = y >> 8;
	
}

//16 bits, 8 bits fraction
int mul(float x, float y)
{
	int result = 0;
	result = x * y;
	result = result >> 8;
	return result;
}

/*----------------------------------------------------------------
 * float2fix -- convert float to fixed point 8+8 bits
 * Reference: fixed_order2_butterworth10.c
 *----------------------------------------------------------------
 */
int	float2fix(double x)
{
	int	y;

	y = x * (1 << 8);
	return y;
}
