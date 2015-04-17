/*
 * HTTP Server
 */
#include "network.h"

//*****************************************************************************
//                 MEMBER VARIABLES -- Start
//*****************************************************************************
unsigned long  g_ulStatus = 0;//SimpleLink Status
unsigned long  g_ulPingPacketsRecv = 0; //Number of Ping Packets received
unsigned long  g_ulGatewayIP = 0; //Network Gateway IP address
unsigned char  g_ucConnectionSSID[SSID_LEN_MAX+1]; //Connection SSID
unsigned char POST_token[] = "__SL_P_ULD";
unsigned char GET_token[]  = "__SL_G_ULD";
signed int g_uiIpAddress = 0;
unsigned char g_ucSSID[SSID_LEN_MAX] = "AIRSTRIKE";
unsigned char g_ucDeviceURN[10] = "airstrike";
unsigned char g_ucPassword[PASSWORD_LEN_MAX] = "airstrike";
unsigned char g_ucDomain[DOMAIN_LEN_MAX] = "airstrike.net";
char g_cTest[SSID_LEN_MAX] = "AIRSTRIKE";
int mCount = 0;

char *g_Buffer;
UINT8 g_success = 0;
int g_close = 0;
UINT16 g_uConnection;

//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]      pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    //
    // This application doesn't work w/ socket - Events are not expected
    //

}

//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
	//TODO do we have events here in AP mode?
    if(!pWlanEvent)
    {

        return;
    }
}
//*****************************************************************************
//
//! \brief This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    if(!pNetAppEvent)
    {
        return;
    }
}

//*****************************************************************************
//
//! This function gets triggered when HTTP Server receives Application
//! defined GET and POST HTTP Tokens.
//!
//! \param pHttpServerEvent Pointer indicating http server event
//! \param pHttpServerResponse Pointer indicating http server response
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pSlHttpServerEvent,
                               SlHttpServerResponse_t *pSlHttpServerResponse)
{

}

void WebSocketCloseSessionHandler(void)
{
	g_close = 1;
	printf("websocket close\n");
}

/*!
 * 	\brief 					This websocket Event is called when WebSocket Server receives data
 * 							from client.
 *
 *
 * 	\param[in]  uConnection	Websocket Client Id
 * 	\param[in] *ReadBuffer		Pointer to the buffer that holds the payload.
 *
 * 	\return					none.
 *
 */
void WebSocketRecvEventHandler(UINT16 uConnection, char *ReadBuffer)
{

	/*
	 * UINT8 Opcode;
	 * struct HttpBlob Write;
	*/

	g_uConnection = uConnection;

	g_Buffer = ReadBuffer;
	g_close = 0;

	//Free memory as we are not using anywhere later
	free(g_Buffer);
	g_Buffer = NULL;
	/* Enter websocket application code here */

	printf("WebSockRecvEventHandler\n");

	//Send something

	UINT8 Opcode = 0x02;

	sl_MyWebSocketSend(uConnection, g_cTest, 10,  Opcode);

	return;
}


/*
 * 	This websocket Event indicates successful handshake with client
 * 	Once this is called the server can start sending data packets over websocket using
 * 	the sl_WebSocketSend API.
 *
 * 	\param[in] uConnection			Websocket Client Id
 *
 * 	\return						none
 */
void WebSocketHandshakeEventHandler(UINT16 uConnection)
{
	g_success = 1;
	g_uConnection = uConnection;

	printf("WebSockhandshake\n");
}


//*****************************************************************************
//! \brief This function puts the device in AP mode
//!           - Set the mode to AP
//!           - Configures connection policy to Auto and AutoSmartConfig
//!           - Deletes all the stored profiles
//!           - Enables DHCP
//!           - Disables Scan policy
//!           - Sets Tx power to maximum
//!           - Sets power policy to normal
//!           - Unregister mDNS services
//!           - Remove all filters
//!
//! \param   none
//! \return  On success, zero is returned. On error, negative is returned
//*****************************************************************************
static long ConfigureSimplelinkToAP()
{
    unsigned char ucVal = 1;
    unsigned char ucPower = 0;

    long lRetVal = -1;
    long lMode = -1;

    printf("BEFORE\n");
    lMode = sl_Start(0, 0, 0);
    printf("AFTER\n");
    ASSERT_ON_ERROR(lMode);

    if (ROLE_AP != lMode)
    {
    	// Switch to AP role and restart
    	lRetVal = sl_WlanSetMode(ROLE_AP);
    	ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(0xFF);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(lRetVal);

        // Check if the device is in AP again
        if (ROLE_STA != lRetVal)
        {
            // We don't want to proceed if the device is not coming up in AP-mode
            return DEVICE_NOT_IN_AP_MODE;
        }

    }

    // Enable DHCP client
    lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&ucVal);
    ASSERT_ON_ERROR(lRetVal);

    //Set max Tx power
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 0;
    lRetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID,
            WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&ucPower);
    ASSERT_ON_ERROR(lRetVal);

    //Set SSID name
    lRetVal = sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SSID, SSID_LEN_MAX, g_ucSSID);
    ASSERT_ON_ERROR(lRetVal);

    //Configure the Security parameter in the AP mode
    _u8 secType = SECURITY_TYPE;
    lRetVal = sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SECURITY_TYPE, 1, (_u8 *)&secType);
    ASSERT_ON_ERROR(lRetVal);

    //Set password
    lRetVal = sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_PASSWORD, PASSWORD_LEN_MAX, g_ucPassword);
    ASSERT_ON_ERROR(lRetVal);

    //Restart again
    lRetVal = sl_Stop(0xFF);
    ASSERT_ON_ERROR(lRetVal);

    lRetVal = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lRetVal);

    return lRetVal;
}

void HTTPServerInit()
{

	long lRetVal = -1;

	ConfigureSimplelinkToAP();

    //Stop Internal HTTP Server
	lRetVal = sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
	if(lRetVal < 0)
	{
		ERR_PRINT(lRetVal);
		LOOP_FOREVER();
	}


    /*configure net app name*/
    lRetVal = sl_NetAppSet(SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DOMAIN_NAME, DOMAIN_LEN_MAX, (unsigned char*)g_ucDomain);
	if(lRetVal < 0)
	{
		ERR_PRINT(lRetVal);
		LOOP_FOREVER();
	}

	/*configure device urn*/
    lRetVal = sl_NetAppSet(SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN, 10, (unsigned char*)g_ucDeviceURN);
	if(lRetVal < 0)
	{
		ERR_PRINT(lRetVal);
		LOOP_FOREVER();
	}

//	//Start Internal HTTP Server
//	lRetVal = sl_NetAppStart(SL_NET_APP_HTTP_SERVER_ID);
//	if(lRetVal < 0)
//	{
//		ERR_PRINT(lRetVal);
//		LOOP_FOREVER();
//	}


}

void HTTPServerStart()
{
	HttpServerInitAndRun(NULL);
}
