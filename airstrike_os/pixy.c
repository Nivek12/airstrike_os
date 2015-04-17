/*
 * pixy.c
 *
 *  Created on: Mar 19, 2015
 *      Author: Chris
 */

#include "pixy.h"

Block *g_blocks;
uint8_t g_numBlocks;
int g_skipStart = 0;

#ifndef SPI //////////// for I2C and UART

extern uint8_t getByte(void);
extern int sendByte(uint8_t byte);

uint16_t getWord(void)
{
  // this routine assumes little endian
  uint16_t w;
  uint8_t c;
  c = readByte();
  w = readByte();
  w <<= 8;
  w |= c;
  return w;
}

int pxy_send(uint8_t *data, int len)
{
  int i;
  for (i=0; i<len; i++)
    sendByte(data[i]);

  return len;
}

#else ///////////// SPI routines

// SPI sends as it receives so we need a getByte routine that
// takes an output data argument
//uint8_t getByte(uint8_t out);
uint16_t getWord()
{

  // ordering is big endian because Pixy is sending 16 bits through SPI
  uint16_t w;
  uint8_t c, cout = 0;

  if (g_outLen)
  {
    w = getByte(PIXY_SYNC_BYTE_DATA);
    cout = g_outBuf[g_outReadIndex++];
    g_outLen--;
    if (g_outReadIndex==PIXY_OUTBUF_SIZE)
      g_outReadIndex = 0;
  }
  else
  {
	  w = getByte(PIXY_SYNC_BYTE); // send out sync byte
  }
  w <<= 8;
  c = getByte(cout); // send out data byte
  w |= c;
  return w;
}

int pxy_send(uint8_t *data, int len)
{
  int i;

  // check to see if we have enough space in our circular queue
  if (g_outLen+len>PIXY_OUTBUF_SIZE)
    return -1;

  g_outLen += len;
  for (i=0; i<len; i++)
  {
    g_outBuf[g_outWriteIndex++] = data[i];
    if (g_outWriteIndex==PIXY_OUTBUF_SIZE)
      g_outWriteIndex = 0;
  }
  return len;
}

#endif //////////////// end SPI routines

void init()
{
  g_blocks = (Block *)malloc(sizeof(Block)*PIXY_ARRAYSIZE);
  memset(g_blocks, 0, sizeof(Block)*PIXY_ARRAYSIZE);
}

int getStart(void)
{
  uint16_t w, lastw;

  lastw = 0xffff;

  while(1)
  {
    w = getWord();
    if (w==0 && lastw==0)
      return 0; // no start code
    else if (w==PIXY_START_WORD && lastw==PIXY_START_WORD)
    {
      g_blockType = NORMAL_BLOCK;
      return 1; // code found!
    }
    else if (w==PIXY_START_WORD_CC && lastw==PIXY_START_WORD)
    {
      g_blockType = CC_BLOCK; // found color code block
      return 1;
    }
    else if (w==PIXY_START_WORDX)
    {
    	printf("\nSYNC ERROR\n\n");
    }
//#ifdef SPI
//      getByte(0); // we're out of sync! (backwards)
//#else
//      getByte(); // we're out of sync! (backwards)
//#endif
    lastw = w;
  }
}

Block * getGBlocks(void) {
	return g_blocks;
}

uint16_t getBlocks(uint16_t maxBlocks)
{
  uint8_t i;
  uint16_t w, blockCount, checksum, sum;
  Block *block;

  if (!g_skipStart)
  {
    if (getStart()==0)
      return 0;
  }
  else
    g_skipStart = 0;

  for(blockCount=0; blockCount<maxBlocks && blockCount<PIXY_ARRAYSIZE;)
  {
    checksum = getWord();
    if (checksum==PIXY_START_WORD) // we've reached the beginning of the next frame
    {
      g_skipStart = 1;
      g_blockType = NORMAL_BLOCK;
      return blockCount;
    }
    else if (checksum==PIXY_START_WORD_CC)
    {
      g_skipStart = 1;
      g_blockType = CC_BLOCK;
      return blockCount;
    }
    else if (checksum==0)
      return blockCount;

    block = g_blocks + blockCount;

    for (i=0, sum=0; i<sizeof(Block)/sizeof(uint16_t); i++)
    {
      if (g_blockType==NORMAL_BLOCK && i>=5) // no angle for normal block
      {
        block->angle = 0;
        break;
      }
      w = getWord();
      sum += w;
      *((uint16_t *)block + i) = w;


    }

    // check checksum
    if (checksum==sum)
      blockCount++;
    else
      printf("checksum error!\n");

    w = getWord();
    if (w==PIXY_START_WORD)
      g_blockType = NORMAL_BLOCK;
    else if (w==PIXY_START_WORD_CC)
      g_blockType = CC_BLOCK;
    else
      return blockCount;
  }

  return blockCount;
}

int setServos(uint16_t s0, uint16_t s1)
{
  uint8_t outBuf[6];

  outBuf[0] = 0x00;
  outBuf[1] = PIXY_SERVO_SYNC;
  *(uint16_t *)(outBuf + 2) = s0;
  *(uint16_t *)(outBuf + 4) = s1;

  return pxy_send(outBuf, 6);
}

int setBrightness(uint8_t brightness)
{
  uint8_t outBuf[3];

  outBuf[0] = 0x00;
  outBuf[1] = PIXY_CAM_BRIGHTNESS_SYNC;
  outBuf[2] = brightness;

  return pxy_send(outBuf, 3);
}

int setLED(uint8_t r, uint8_t g, uint8_t b)
{
  uint8_t outBuf[5];

  outBuf[0] = 0x00;
  outBuf[1] = PIXY_LED_SYNC;
  outBuf[2] = r;
  outBuf[3] = g;
  outBuf[4] = b;

  return pxy_send(outBuf, 5);
}



