/*
 * target.c
 *
 *  Created on: Mar 19, 2015
 *      Author: Chris
 */
#include "target.h"

Target_Track g_target;
uint16_t c_triesSinceLastDet = 10;
uint16_t last_coor[2];

bool foundTarget(void) {
	return (g_target.viableFrames > 1);
}

bool onTarget(void) {

	if (g_target.viableFrames < 2) return false;

	uint8_t frame =  (g_target.start + g_target.viableFrames - 1)%TRACKFRAMES;
	//uint8_t lframe = (g_target.start + g_target.viableFrames - 2)%TRACKFRAMES;
	float p;

	p = findTargetChangePercent(last_coor[0], last_coor[1],
			g_target.coordinates[frame][0], g_target.coordinates[frame][1]);

	printf("Percent from target: %f\n", p);

	if (p > LASER_TOL)
		return false;
	else
		return true;
}

float findTargetChangePercent(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	float p = 0.0, dx = 0.0, dy = 0.0, dr = 0.0, dc = 0.0;

	dx = x1 - x0;
	dy = y1 - y0;
	dr = sqrt(dx*dx + dy*dy);
	dc = sqrt(PIXY_PIXEL_WIDTH*PIXY_PIXEL_WIDTH +
		 PIXY_PIXEL_HEIGHT*PIXY_PIXEL_HEIGHT);
	p  = dr/dc * 100;

	printf("dr, dc: %f, %f\np: %f\n", dr, dc, p);
	return p;
}

void InitTargetModule(void) {
	InitSPIModule();
	init();
	uint8_t i = 0, j = 0;

	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 4; j++)
		{
			g_target.coordinates[i][j] = 0;
		}
	}

	g_target.start = 0;
	g_target.viableFrames = 0;

	last_coor[0] = 0;
	last_coor[1] = 0;
}

void getCurTarget(Block* tar) {
	uint8_t frame = (g_target.start + g_target.viableFrames - 1)%TRACKFRAMES;
	tar->x = g_target.coordinates[frame][0];
	tar->y = g_target.coordinates[frame][1];
	tar->height = g_target.coordinates[frame][2];
	tar->width = g_target.coordinates[frame][3];
	tar->signature = 0;
	tar->angle = 0;
}

char* getTargetString(char * targetString) {
	Block target;
	getCurTarget(&target);
	sprintf(targetString, "%u|%u|%u|%u",
			(unsigned int) target.x, (unsigned int) target.y,
			(unsigned int) target.height, (unsigned int) target.width);
	return targetString;
}

uint8_t TargetMainLoopTask(void)
{
	g_numBlocks = getBlocks(MAX_BLOCKS);
	if (g_numBlocks > 0)
	{
		updateTarget();
		c_triesSinceLastDet = 0;
	}
	else
	{
		c_triesSinceLastDet++;
		if (c_triesSinceLastDet > 5)
		{
			g_target.viableFrames = 0;
		}
	}

	printf("%d\n", g_numBlocks);
	return g_numBlocks;
}

float* getTargetSphericalCoords(float* target_coords)
{
	target_coords[0] = RANGE;
	Block t_target;
	getCurTarget(&t_target);

	unsigned int currentCycles = MAP_TimerValueGet(TIMERA2_BASE, TIMER_B);
	unsigned int cyclesRemaining = 0x00186A00 - currentCycles;
	float timeRemaining = (cyclesRemaining/80000000)*1000;

	t_target.x += g_target.vel[0] * timeRemaining;
	t_target.y += g_target.vel[1] * timeRemaining;

	last_coor[0] = t_target.x;
	last_coor[1] = t_target.y;

	float p_x = t_target.x    - (PIXY_PIXEL_WIDTH/2);
	float p_y = (-t_target.y) + (PIXY_PIXEL_HEIGHT/2);

	//printf("Pixy Pre-00   X: %5u,   Y: %5u\n" , g_target.x, g_target.y);
	//printf("Pixy Pixels   X: %5f,   Y: %5f\n", p_x, p_y);

	target_coords[0] = RANGE;
	target_coords[1] = 90 - (p_y*PIXY_PIXEL_EL);
	target_coords[2] = (-p_x)*PIXY_PIXEL_AZ;


	//printf("Pixy Angles   Yaw: %5f, Pitch: %5f\n", target_coords[2], 90-target_coords[1]);
	return target_coords;
}

float* getServoAngles(float *servo_s_coords)
{
	float target_s_coords[3];
	if (foundTarget())
	{

		getTargetSphericalCoords(target_s_coords);
		float target_c_coords[3];
		float servo_c_coords[3];
		float rad_theta = target_s_coords[1] * (M_PI/180);
		float rad_phi   = target_s_coords[2] * (M_PI/180);
		target_c_coords[0] = target_s_coords[0] * sin(rad_theta) * cos(rad_phi);
		target_c_coords[1] = target_s_coords[0] * sin(rad_theta) * sin(rad_phi);
		target_c_coords[2] = target_s_coords[0] * cos(rad_theta);

		servo_c_coords[0] = S2P_X + target_c_coords[0];
		servo_c_coords[1] = S2P_Y + target_c_coords[1];
		servo_c_coords[2] = S2P_Z + target_c_coords[2];

		printf("Pixy:  %3.5f, %3.5f, %3.5f\n"
			   "S2P:   %3.5f, %3.5f, %3.5f\n"
			   "Servo: %3.5f, %3.5f, %3.5f\n",
			   target_c_coords[0], target_c_coords[1], target_c_coords[2],
			   S2P_X,              S2P_Y,              S2P_Z,
			   servo_c_coords[0],  servo_c_coords[1],  servo_c_coords[2]);

		servo_s_coords[0] = sqrt((servo_c_coords[0] * servo_c_coords[0]) +
								 (servo_c_coords[1] * servo_c_coords[1]) +
							     (servo_c_coords[2] * servo_c_coords[2]));
		servo_s_coords[1] = 90 - (acos(servo_c_coords[2]/servo_s_coords[0]) * (180.0 / M_PI));
		servo_s_coords[2] = atan(servo_c_coords[1]/servo_c_coords[0]) * (180.0 / M_PI);
	}
	else
	{
		servo_s_coords[0] = 0;
		servo_s_coords[1] = 0;
		servo_s_coords[2] = 0;
	}

	return servo_s_coords;
}

void updateTarget() {
	Block targetUpdate = g_blocks[0];
	uint8_t frame = (g_target.start + g_target.viableFrames)%TRACKFRAMES;
	uint8_t lframe = (g_target.start + g_target.viableFrames - 1)%TRACKFRAMES;
	g_target.coordinates[frame][0] = targetUpdate.x;
	g_target.coordinates[frame][1] = targetUpdate.y;
	g_target.coordinates[frame][2] = targetUpdate.height;
	g_target.coordinates[frame][3] = targetUpdate.width;

	if (g_target.viableFrames == TRACKFRAMES)
	{
		g_target.start = (g_target.start+1)%TRACKFRAMES;
	}
	else
	{
		g_target.viableFrames++;
	}


	if (g_target.viableFrames >= VELFRAMES)
	{
		g_target.vel[0] = (g_target.coordinates[frame][0] - g_target.coordinates[lframe][0])/(MS_PER_FRAME * (c_triesSinceLastDet + 1));
		g_target.vel[1] = (g_target.coordinates[frame][1] - g_target.coordinates[lframe][1])/(MS_PER_FRAME * (c_triesSinceLastDet + 1));
	}
}

