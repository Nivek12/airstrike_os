/*
 * main.c
 */

#define css		6
#define TRACK_TIMER_INT_MSEC	300

#include <stdio.h>
#include <string.h>

// Driverlib Includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "systick.h"
#include "pin.h"
#include "prcm.h"
#include "utils.h"
#include "rom_map.h"
#include "interrupt.h"
#include "timer.h"

#include "pinmux.h"
#include "osi.h"

#include "udma_if.h"
#include "network.h"

//*****************************************************************************
//                      GLOBAL VARIABLES for VECTOR TABLE
//*****************************************************************************
#if(css)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

//*****************************************************************************
//                          LOCAL DEFINES
//*****************************************************************************
#define APP_NAME		        "WebSocket"
#define SPAWN_TASK_PRIORITY     2
#define HTTP_SERVER_APP_TASK_PRIORITY  2
#define TARGET_TASK_PRIORITY  1
#define OSI_STACK_SIZE          2048

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(css)
	IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    IntVTableBaseSet((unsigned long)&__vector_table);
#endif

#endif //USE_TIRTOS

    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);
    PRCMCC3200MCUInit();
}

#ifdef USE_FREERTOS
//*****************************************************************************
//
//! \brief Application defined idle task hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void
vApplicationIdleHook( void)
{
    //Handle Idle Hook for Profiling, Power Management etc
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    //Handle Memory Allocation Errors
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook( OsiTaskHandle *pxTask,
                                   signed char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}
#endif //USE_FREERTOS

void TargetTask(void * param)
{
	while(1)
	{
		printf("target\n");
		osi_Sleep(10);
	}
}

//Thread that runs the web server
void HttpServerAppTask(void * param)
{
	HTTPServerInit();
	HTTPServerStart();
}


int main(void) {
	
	//
	// Board Initialization
	//
	BoardInit();

	//
	// Enable and configure DMA
	//
	UDMAInit();

	//Config pins
	PinMuxConfig();

	VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);

    osi_TaskCreate(HttpServerAppTask,
                    "WebSocketApp",
                        OSI_STACK_SIZE,
                        NULL,
                        HTTP_SERVER_APP_TASK_PRIORITY,
                        NULL );

    osi_TaskCreate(TargetTask,
                    "Target",
                        OSI_STACK_SIZE,
                        NULL,
                        TARGET_TASK_PRIORITY,
                        NULL );

    osi_start();

	return 0;
}
