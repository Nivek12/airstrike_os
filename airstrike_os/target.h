/*
 * target.h
 *
 *  Created on: Mar 14, 2015
 *      Author: Chris
 */

#ifndef TARGET_H_
#define TARGET_H_

#include <stdbool.h>
#include <math.h>
#include "hw_types.h"
#include "timer.h"
#include "pixy.h"

#define TRACKFRAMES 5
#define MS_PER_FRAME 10
#define M_PI 3.14159265358979323846
#define RANGE 10
#define PIXY_PIXEL_WIDTH  320
#define PIXY_PIXEL_HEIGHT 200
#define PIXY_PIXEL_AZ 75/320
#define PIXY_PIXEL_EL 47/200
#define S2P_X 0.2708333
#define S2P_Y 0
#define S2P_Z -0.166667
#define VELFRAMES 	   2
#define LASER_TOL      5
#define TRACK_TOL     10

typedef struct {
	uint16_t coordinates[TRACKFRAMES][4];
	uint8_t  viableFrames;
	uint8_t  start;
	float vel[2];
	float accel[2];
} Target_Track;

bool foundTarget(void);
bool onTarget(void);
float findTargetChangePercent(uint16_t, uint16_t, uint16_t, uint16_t);
void InitTargetModule(void);
char* getTargetString(char*);
uint8_t TargetMainLoopTask(void);
float* getTargetSphericalCoords(float*);
float* getServoAngles(float*);
void updateTarget(void);

extern Target_Track g_target;
extern uint16_t c_triesSinceLastDet;
extern uint16_t last_coor[2];

#endif /* TARGET_H_ */
