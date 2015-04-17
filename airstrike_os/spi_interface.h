/*
 * spi_interface.h
 *
 *  Created on: Mar 14, 2015
 *      Author: Chris
 */

#ifndef SPI_INTERFACE_H_
#define SPI_INTERFACE_H_

#include <stdio.h>
#include <stdint.h>
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_gpio.h"
#include "pin.h"
#include "gpio.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "spi.h"

#define SPI_IF_BIT_RATE  10000000
#define MAX_BLOCKS       100
#define CAM_A            0
#define CAM_B            1
#define CAM_A_CS         GPIO_PIN_5
#define CAM_B_CS         GPIO_PIN_4

extern uint8_t g_CameraSelect;

uint8_t getByte(uint8_t out);
void enablePixyCS(uint8_t cam);
void disablePixyCS(uint8_t cam);
void InitSPIModule(void);

#endif /* SPI_INTERFACE_H_ */
