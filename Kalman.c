/*
 * Kalman filter
 * filter every time a data is sampled
 * for roll ( sp, sq, sr ): 16.4 LSB/deg/s (+-2000deg/s)
 * for pitch ( sax, say, saz ): 16384 LSB/g (+-2g)
 */
 
#include "in4073.h"
//iteration times
int32_t C1=7,C2=17;
//#define C2 17
#define P2PHI	50

void kalmanFilter()
{
	int32_t sp_t = sp << 8;
	int32_t sq_t = sq << 8;
	int32_t sax_t = sax << 8;
	int32_t say_t = say << 8;

	static int32_t pbias =0, qbias = 0;
	static int32_t estimate_p =0, estimate_q = 0;
	static int32_t estimate_phi =0, estimate_theta = 0;

	//roll
	estimate_p = sp_t - pbias;
	estimate_phi = sax_t + estimate_p * P2PHI;
	estimate_phi = estimate_phi - ((estimate_phi - sax_t) >> C1);
	pbias = pbias + (((estimate_phi - sax_t) / P2PHI) >> C2);
	p_kalman = estimate_p >> 8;
	phi_kalman = estimate_phi >> 8;

	//pitch
	estimate_q = sq_t - qbias;
	estimate_theta = say_t + estimate_q * P2PHI;
	estimate_theta = estimate_theta - ((estimate_theta - say_t) >> C1);
	qbias = qbias + (((estimate_theta - say_t) / P2PHI) >> C2);
	q_kalman = estimate_q >> 8;
	theta_kalman = estimate_theta >> 8;	
    //printf("%d|%d|%d|%d|\n",p_kalman,sp,q_kalman,sq);
} 
