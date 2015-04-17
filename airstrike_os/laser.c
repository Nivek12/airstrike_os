/*
 * laser.c
 *
 *  Created on: Mar 28, 2015
 *      Author: Kevin
 */

#include "laser.h"

#define PULSE_WIDTH_US	20000

float g_LaserPowerLevel = 0;

void SetLaserPower(float percent)
{
	g_LaserPowerLevel = percent;
	int width = (int)(PULSE_WIDTH_US*percent);
	UpdateDutyCycle(TIMERA3_BASE, TIMER_B, width);
}

float GetLaserPower()
{
	return g_LaserPowerLevel;
}

