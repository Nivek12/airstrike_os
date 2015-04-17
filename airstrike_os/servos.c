/*
 * servos.c
 *
 *  Created on: Mar 19, 2015
 *      Author: Kevin
 */

#include "servos.h"

#define SERVO_A			0.01282051282
#define SERVO_B			10.8974358974
#define SERVO_C			1500
#define ROUND_OFFSET 	0.5

//2ms = 25.5
//1ms = 12.75

//Pitch pin 64 pwm 05
void SetPitchAngle(float degrees)
{
	int width = DegreesToWidth(-degrees);
	UpdateDutyCycle(TIMERA2_BASE, TIMER_B, width);
}

//Yaw pin 1 pwm 06
void SetYawAngle(float degrees)
{
	int width = DegreesToWidth(degrees);
	UpdateDutyCycle(TIMERA3_BASE, TIMER_A, width);
	//("%d\n", width);
}

int DegreesToWidth(float degrees)
{

	float a = SERVO_A*degrees*degrees;
	float b = SERVO_B*degrees;
	int width = (int)(a + b + SERVO_C + ROUND_OFFSET);
	return width;
}

