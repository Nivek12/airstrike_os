/*
 * pixy.h
 *
 *  Created on: Mar 14, 2015
 *      Author: CharmedLabs
 *      Edited by: Chris
 */

#ifndef PIXY_H_
#define PIXY_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "rom.h"
#include "rom_map.h"

#include "spi_interface.h"

// Are you using an SPI interface?  if so, uncomment this line
#define SPI

#define PIXY_ARRAYSIZE              20
#define PIXY_START_WORD             0xaa55
#define PIXY_START_WORD_CC          0xaa56
#define PIXY_START_WORDX            0x55aa
#define PIXY_SERVO_SYNC             0xff
#define PIXY_CAM_BRIGHTNESS_SYNC    0xfe
#define PIXY_LED_SYNC               0xfd
#define PIXY_OUTBUF_SIZE            32

#define PIXY_SYNC_BYTE              0x5a
#define PIXY_SYNC_BYTE_DATA         0x5b

// data types
typedef enum
{
    NORMAL_BLOCK,
    CC_BLOCK // color code block
} BlockType;

typedef struct
{
  uint16_t signature;
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
  uint16_t angle; // angle is only available for color coded blocks
} Block;


#ifndef SPI //////////// for I2C and UART

extern uint8_t getByte(void);
extern int sendByte(uint8_t byte);
uint16_t getWord(void);}

int pxy_send(uint8_t *data, int len);
#else ///////////// SPI routines

// SPI sends as it receives so we need a getByte routine that
// takes an output data argument
// uint8_t getByte(uint8_t out);

// variables for a little circular queue
static uint8_t g_outBuf[PIXY_OUTBUF_SIZE];
static uint8_t g_outLen = 0;
static uint8_t g_outWriteIndex = 0;
static uint8_t g_outReadIndex = 0;

uint16_t getWord();
int pxy_send(uint8_t *data, int len);

#endif //////////////// end SPI routines

extern int g_skipStart;
static BlockType g_blockType;
extern Block *g_blocks;
extern uint8_t g_numBlocks;

void init();
int getStart(void);
Block * getGBlocks(void);
uint16_t getBlocks(uint16_t maxBlocks);
int setServos(uint16_t s0, uint16_t s1);
int setBrightness(uint8_t brightness);
int setLED(uint8_t r, uint8_t g, uint8_t b);
#endif /* PIXY_H_ */
