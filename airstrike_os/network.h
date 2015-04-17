/*
 * network.h
 *
 *  Created on: Feb 24, 2015
 *      Author: Kevin
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <stdio.h>

// Simplelink includes
#include "common.h"
#include "simplelink.h"

#include "datatypes.h"
#include "WebSockHandler.h"

// HTTP lib includes
#include "HttpCore.h"
#include "HttpRequest.h"
#include "HttpString.h"

#define DEVICE_NAME                   "httpserver"
#define TI_NAME                       "cc3200_"
#define mDNS_SERV                     "._device-info._tcp.local"
#define TTL_MDNS_SERV                 4500

#define MAX_DEV_NAME_SIZE                       23

typedef struct
{
  unsigned char DevNameLen;
  unsigned char DevName[MAX_DEV_NAME_SIZE];
}t_DevName;

typedef struct
{
	char    mDNSServNameUnReg[64];
	unsigned char   mDNSServNameUnRegLen;

}t_mDNSService;

// Application specific status/error codes
typedef enum{
    // Choosing -0x7D0 to avoid overlap w/ host-driver's error codes
    LAN_CONNECTION_FAILED = -0x7D0,
    INTERNET_CONNECTION_FAILED = LAN_CONNECTION_FAILED - 1,
    DEVICE_NOT_IN_AP_MODE = INTERNET_CONNECTION_FAILED - 1,

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

static long ConfigureSimplelinkToAP();
void HTTPServerInit();
void HTTPServerStart();

#endif /* NETWORK_H_ */
