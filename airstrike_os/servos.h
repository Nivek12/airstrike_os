/*
 * servos.h
 *
 *  Created on: Mar 19, 2015
 *      Author: Kevin
 */

#ifndef SERVOS_H_
#define SERVOS_H_

#include "pwm.h"

void SetPitchAngle(float degrees);
void SetYawAngle(float degrees);
int DegreesToWidth(float degrees);

#endif /* SERVOS_H_ */
