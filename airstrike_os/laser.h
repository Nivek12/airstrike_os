/*
 * laser.h
 *
 *  Created on: Mar 28, 2015
 *      Author: Kevin
 */

#ifndef LASER_H_
#define LASER_H_

#include "pwm.h"

//percent is a value between 0 and 1
void SetLaserPower(float percent);

float GetLaserPower();

extern float g_LaserPowerLevel;

#endif /* LASER_H_ */
