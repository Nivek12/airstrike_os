//*****************************************************************************
// rom_pin_mux_config.c
//
// configure the device pins for different signals
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

// This file was automatically generated on 3/17/2015 at 11:40:26 AM
// by TI PinMux version 3.0.625 
//
//*****************************************************************************

#include "pinmux.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_gpio.h"
#include "pin.h"
#include "gpio.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"

//*****************************************************************************
void PinMuxConfig(void)
{
    //
    // Enable Peripheral Clocks 
    //
    MAP_PRCMPeripheralClkEnable(PRCM_ADC, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_TIMERA2, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_TIMERA3, PRCM_RUN_MODE_CLK);

    //
    // Configure PIN_57 for ADC0 ADC_CH0
    //
    MAP_PinTypeADC(PIN_57, PIN_MODE_255);

    //
    // Configure PIN_58 for ADC0 ADC_CH1
    //
    MAP_PinTypeADC(PIN_58, PIN_MODE_255);

    //
    // Configure PIN_05 for SPI0 GSPI_CLK
    //
    MAP_PinTypeSPI(PIN_05, PIN_MODE_7);

    //
    // Configure PIN_06 for SPI0 GSPI_MISO
    //
    MAP_PinTypeSPI(PIN_06, PIN_MODE_7);

    //
    // Configure PIN_07 for SPI0 GSPI_MOSI
    //
    MAP_PinTypeSPI(PIN_07, PIN_MODE_7);

    //
    // Configure PIN_03 for GPIO Output
    //
    MAP_PinTypeGPIO(PIN_03, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA1_BASE, 0x10, GPIO_DIR_MODE_OUT);

    //
    // Configure PIN_04 for GPIO Output
    //
    MAP_PinTypeGPIO(PIN_04, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA1_BASE, 0x20, GPIO_DIR_MODE_OUT);

    //
    // Configure PIN_64 for TimerPWM5 GT_PWM05
    //
    MAP_PinTypeTimer(PIN_64, PIN_MODE_3);

    //
    // Configure PIN_01 for TimerPWM6 GT_PWM06
    //
    MAP_PinTypeTimer(PIN_01, PIN_MODE_3);

    //
    // Configure PIN_02 for TimerPWM7 GT_PWM07
    //
    MAP_PinTypeTimer(PIN_02, PIN_MODE_3);
}
