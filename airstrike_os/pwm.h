/*
 * pwm.h
 *
 *  Created on: Mar 10, 2015
 *      Author: Kevin
 */

#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_apps_rcm.h"
#include "hw_common_reg.h"
#include "interrupt.h"
#include "rom.h"
#include "rom_map.h"
#include "timer.h"
#include "utils.h"
#include "prcm.h"

void UpdateDutyCycle(unsigned long ulBase, unsigned long ulTimer, unsigned int pulse_ms);
void SetupTimerPWMMode(unsigned long ulBase, unsigned long ulTimer, unsigned long ulConfig, unsigned char ucInvert, unsigned int refreshRate);
void InitPWMModules();

#endif /* PWM_H_ */
