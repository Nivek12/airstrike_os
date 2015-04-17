/*
 * pwm.c
 *
 *  Created on: Mar 10, 2015
 *      Author: Kevin
 */

#include "pwm.h"

//
// The PWM works based on the following settings:
//     Timer reload interval -> determines the time period of one cycle
//     Timer match value -> determines the duty cycle
//                          range [0, timer reload interval]
// The computation of the timer reload interval and dutycycle granularity
// is as described below:
// Timer tick frequency = 80 Mhz = 80000000 cycles/sec
// For a time period of 0.5 ms,
//      Timer reload interval = 80000000/2000 = 40000 cycles
// To support steps of duty cycle update from [0, 255]
//      duty cycle granularity = ceil(40000/255) = 157
// Based on duty cycle granularity,
//      New Timer reload interval = 255*157 = 40035
//      New time period = 0.5004375 ms
//      Timer match value = (update[0, 255] * duty cycle granularity)
//
#define PRESCALE 25	//20ms period
#define TIMER_INTERVAL_RELOAD   65535 /* =(255*157) */
#define DUTYCYCLE_GRANULARITY   257 //Lower this for more accurate timing

#define CC3200_CLOCK_RATE 	80000000
#define TICS_US 			CC3200_CLOCK_RATE / 1000000
#define DEFAULT_PULSE 		0
#define SERVO_REFRESH_RATE 	50	//20ms periods = 50Hz
#define LASER_REFRESH_RATE	50

//****************************************************************************
//
//! Update the dutycycle of the PWM timer
//!
//! \param ulBase is the base address of the timer to be configured
//! \param ulTimer is the timer to be setup (TIMER_A or  TIMER_B)
//! \param ucLevel translates to duty cycle settings (0:255)
//!
//! This function
//!    1. The specified timer is setup to operate as PWM
//!
//! \return None.
//
//****************************************************************************
void UpdateDutyCycle(unsigned long ulBase, unsigned long ulTimer, unsigned int pulse_us)
{
    //
    // Match value is updated to reflect the new dutycycle settings
    //
	uint32_t period = TICS_US*pulse_us;
	uint8_t extender = period >> 16;
	period &= 0xFFFF;

    MAP_TimerPrescaleMatchSet(ulBase, ulTimer, extender);
    MAP_TimerMatchSet(ulBase,ulTimer, period);
}

//****************************************************************************
//
//! Setup the timer in PWM mode
//!
//! \param ulBase is the base address of the timer to be configured
//! \param ulTimer is the timer to be setup (TIMER_A or  TIMER_B)
//! \param ulConfig is the timer configuration setting
//! \param ucInvert is to select the inversion of the output
//!
//! This function
//!    1. The specified timer is setup to operate as PWM
//!
//! \return None.
//
//****************************************************************************
void SetupTimerPWMMode(unsigned long ulBase, unsigned long ulTimer,
                       unsigned long ulConfig, unsigned char ucInvert, unsigned int refreshRate)
{



    // set refresh rate
    uint32_t period1 = CC3200_CLOCK_RATE / refreshRate; /*Hz*/
    uint8_t extender1 = period1 >> 16;
    period1 &= 0xFFFF;

   	//Set default
	uint32_t period2 = TICS_US*DEFAULT_PULSE;
	uint8_t extender2 = period2 >> 16;
	period2 &= 0xFFFF;

    //
    // Set GPT - Configured Timer in PWM mode.
    //
    MAP_TimerConfigure(ulBase,ulConfig);
    MAP_TimerPrescaleSet(ulBase,ulTimer, extender1);

    //
    // Inverting the timer output if required
    //
    MAP_TimerControlLevel(ulBase,ulTimer,ucInvert);

    //
    // Load value set to 20 ms time period
    //
    MAP_TimerLoadSet(ulBase,ulTimer,period1);

    //
    // Match value set so as to output level 0
    //
    MAP_TimerPrescaleMatchSet(ulBase, ulTimer, extender2);
    MAP_TimerMatchSet(ulBase,ulTimer, period2);

}

//****************************************************************************
//
//! Sets up the identified timers as PWM to drive the peripherals
//!
//! \param none
//!
//! This function sets up the folowing
//!    1. TIMERA2 (TIMER B) as RED of RGB light
//!    2. TIMERA3 (TIMER B) as YELLOW of RGB light
//!    3. TIMERA3 (TIMER A) as GREEN of RGB light
//!
//! \return None.
//
//****************************************************************************
void InitPWMModules()
{
    //
    // Initialization of timers to generate PWM output
    //
    MAP_PRCMPeripheralClkEnable(PRCM_TIMERA2, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_TIMERA3, PRCM_RUN_MODE_CLK);

    //
    // TIMERA2 (TIMER B) as Pitch Servo. GPIO 9 --> PWM_5
    //
    SetupTimerPWMMode(TIMERA2_BASE, TIMER_B,
            (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PWM), 1, SERVO_REFRESH_RATE);

    //
    // TIMERA3 (TIMER B) as Yaw Servo. GPIO 10 --> PWM_6
    //
    SetupTimerPWMMode(TIMERA3_BASE, TIMER_A,
            (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM), 1, SERVO_REFRESH_RATE);
    //
    // TIMERA3 (TIMER A) as Laser control. GPIO 11 --> PWM_7
    //
    SetupTimerPWMMode(TIMERA3_BASE, TIMER_B,
            (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM), 1, LASER_REFRESH_RATE);

    MAP_TimerEnable(TIMERA2_BASE,TIMER_B);
    MAP_TimerEnable(TIMERA3_BASE,TIMER_A);
    MAP_TimerEnable(TIMERA3_BASE,TIMER_B);

}

//****************************************************************************
//
//! Disables the timer PWMs
//!
//! \param none
//!
//! This function disables the timers used
//!
//! \return None.
//
//****************************************************************************
void DeInitPWMModules()
{
    //
    // Disable the peripherals
    //
    MAP_TimerDisable(TIMERA2_BASE, TIMER_B);
    MAP_TimerDisable(TIMERA3_BASE, TIMER_A);
    MAP_TimerDisable(TIMERA3_BASE, TIMER_B);
    MAP_PRCMPeripheralClkDisable(PRCM_TIMERA2, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkDisable(PRCM_TIMERA3, PRCM_RUN_MODE_CLK);
}




