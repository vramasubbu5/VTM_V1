#ifdef __EXAMPLE_MULTITASK1__


#include "system.h"
#include "custom_feature_def.h"
#include "ql_common.h"
#include "ql_system.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_stdlib.h"
#include "ql_uart.h"
#include "ql_memory.h"
#include "ql_error.h"
#include "ql_power.h"
#include "ql_gprs.h"
#include "ql_fs.h"
#include "ql_socket.h"
#include "ql_timer.h"
#include "ql_adc.h"
#include "fota_main.h"






//#include "ql_gpio.h"


#include "ril.h"
#include "ril_util.h"
#include "ril_system.h"
#include "ril_network.h"
#include "ril_telephony.h"

#include "gpio.h"






//#include "ril_urc.c"

#define MSG_ID_SF_STATE (MSG_ID_USER_START + 100)

typedef enum
{
	SF_STATE_STARTED ,
	SF_STATE_RIL_READY ,
	SF_STATE_FULL_FUNC ,
	SF_STATE_GPRS_READY ,
	SF_STATE_DATA_READY ,
	SF_STATE_DATA_SENT ,
	SF_STATE_DATA_RECEIVED ,
	SF_STATE_UPGRADE ,
	SF_STATE_FINISHED ,
	SF_STATE_FAILURE
} Enum_Sf_State;

char data [512];
char response [515];

s32 dataLength;
s32 sent , received;

static void setState(Enum_Sf_State state);
static void prepareData(void);
static void sendData(void);
static void handleResponse(void);
static void upgrade(const char* url);
static void failure(void);
static void finish(void);

void send_callback(s32 error);
void receive_callback(s32 error);

static void setState(Enum_Sf_State state)
{
	Ql_Debug_Trace("<-- setState %d -->\r\n", state);
	Ql_OS_SendMessage (0, MSG_ID_SF_STATE , state , 0);
}

static void upgrade(const char* url)
{
	Ql_Debug_Trace("<-- UPGRADING -->\r\n");
/*
	ST_GprsConfig config = { SF_APN , "", "", 0, NULL , NULL };
	s32 err = Ql_FOTA_StartUpgrade ((u8*)url , &config , NULL);
	if(err)
	{
		Ql_Debug_Trace("FOTA update failed !\r\n");
		setState(SF_STATE_FAILURE);
	}*/
}

static void prepareData(void)
{
	Ql_Debug_Trace("<-- Preparing data -->\r\n");
/*
	ST_SENSOR datastreams [20];
	u8 streamCount = 0;

	// Add battery voltage as a datastream
	// Power: <voltage >
	Ql_Debug_Trace("<-- Reading battery voltage -->\r\n");
	float voltage;
	if(Sf_GetBatteryVoltage (& voltage))
	{
		Ql_Debug_Trace("Battery voltage: %fV\r\n", voltage);

		if(voltage < SF_LOW_VOLTAGE_LIMIT)
		{
			Ql_Debug_Trace("Low voltage , powering down!\r\n");
			setState(SF_STATE_FAILURE);
			return;
		}

			// Set id
			Ql_strcpy(datastreams[streamCount ].id , "Power");

			// Set value
			datastreams[streamCount ]. value = voltage;

			++ streamCount;
	}
	else return setState(SF_STATE_FAILURE);

	// Read IMEI number.
	Ql_Debug_Trace("<-- Fetching IMEI -->\r\n");
	char imei [16];
	if(Sf_GetIMEI(imei))
		Ql_Debug_Trace("IMEI: %s\r\n", imei);
	else
	return setState(SF_STATE_FAILURE);

	// Check IIC devices
	Ql_Debug_Trace("<-- Checking IIC -devices -->\r\n");
	if(Sf_IIC_Init ())
	{
	// Check for SHT21
		Sf_SHT21_Read(datastreams , &streamCount);

		// Uninitialize IIC
		Sf_IIC_Uninit ();
	}

	// GPS
	Sf_GPS_Read(datastreams , &streamCount);
	Sf_GPS_Uninit ();

	// Construct JSON
	Ql_Debug_Trace("<-- Building JSON -->\r\n");
	dataLength = Sf_JSON_Build(data , imei , datastreams ,
	streamCount);

	Ql_Debug_Trace("%s\r\n", data);

	setState(SF_STATE_DATA_READY);
*/
}
static void sendData(void)
{
	Ql_Debug_Trace("<-- Sending data -->\r\n");
/*	sent = 0;
	received = 0;

	// Activate GPRS
	if(! Sf_ActivateGPRS ())
	{
		Ql_Debug_Trace("Failed to activate GPRS!\r\n");
		setState(SF_STATE_FAILURE);
		return;
	}

	if(! Sf_HTTP_SendRequest(SF_HTTP_SERVER , SF_REQUEST_TYPE , dataLength , send_callback))
	{
		Ql_Debug_Trace("Failed to send request !\r\n");
		setState(SF_STATE_FAILURE);
		return;
	}
*/
}

static void handleResponse(void)
{
//	Sf_DeactivateGPRS ();

	Ql_Debug_Trace("<-- Handle response -->\r\n");
	Ql_Debug_Trace("%s\r\n", response);
/*
	char update [256];
	s32 s, log;

	// Parse JSON
	Sf_JSON_Parse(response , &s, &log , update);

	Ql_Debug_Trace("s: %d\r\n", s);
	Ql_Debug_Trace("log: %d\r\n", log);
	Ql_Debug_Trace("update: %s\r\n", update);

	if(Ql_strlen(update) > 0)
	{
		setState(SF_STATE_UPGRADE);
		upgrade(update);
		return;
	}

	// Set wake up timer.
	if(s)
	{
		Sf_SetWakeupDelay(s);
		setState(SF_STATE_FINISHED);
	}
	else
		setState(SF_STATE_FAILURE);
*/
}

static void finish ()
{
	Ql_Debug_Trace("<-- FINISH -->\r\n");
/*
	// Show time spent
	u64 timeSincePwrOn = Ql_GetMsSincePwrOn ();
	Ql_Debug_Trace("<-- Operation took: %dms -->\r\n", (u32)
			timeSincePwrOn);

	// Power down
	Ql_Debug_Trace("<-- Powering down -->\r\n");
	Ql_PowerDown (1);
*/
}

static void failure ()
{
	Ql_Debug_Trace("<-- FAILURE -->\r\n");
/*
	// Retry later
	Sf_SetWakeupDelay(SF_RETRY_DELAY);

	// Power down
	Ql_Debug_Trace("<-- Powering down -->\r\n");
	Ql_PowerDown (1);
*/
}




void proc_main_task(s32 taskId)
{
	Ql_Debug_Trace("Lateral emSoft the world of navigation \r\n");
	Ql_Debug_Trace("Main Application wake up\r\n");

	Enum_Sf_State state = SF_STATE_STARTED;

	ST_MSG msg;

	while (TRUE)
	{
		Ql_OS_GetMessage (&msg);

		if(msg.message == MSG_ID_SF_STATE)
		{
			state = msg.param1;
			switch (state)
			{
				case SF_STATE_FULL_FUNC:
					Ql_Debug_Trace("SF_STATE_FULL_FUNC up\r\n");
					prepareData ();
					break;

				case SF_STATE_GPRS_READY:
					Ql_Debug_Trace("SF_STATE_GPRS_READY up\r\n");
					sendData ();
					break;

				case SF_STATE_DATA_SENT:
					Ql_Debug_Trace("SF_STATE_DATA_SENT up\r\n");

					break;

				case SF_STATE_DATA_RECEIVED:
					Ql_Debug_Trace("SF_STATE_DATA_RECEIVED up\r\n");
					handleResponse ();
					break;

				case SF_STATE_FAILURE:
					Ql_Debug_Trace("SF_STATE_FAILURE up\r\n");
					failure ();
					break;
				case SF_STATE_FINISHED:
					Ql_Debug_Trace("SF_STATE_DATA_RECEIVED up\r\n");
					finish ();
					break;

				default:
					break;
			}
		}
		else if(msg.message == MSG_ID_RIL_READY)
		{
			Ql_Debug_Trace("<-- RIL is ready -->\r\n");
			Ql_RIL_Initialize ();
			Ql_Debug_Trace("<-- Synchronizing time -->\r\n");
//			Sf_SyncTime ();											//// will call time Synchronize function here
			setState(SF_STATE_RIL_READY);
		}
		else if(msg.message == MSG_ID_URC_INDICATION)
		{
			switch (msg.param1)
			{
				case URC_CFUN_STATE_IND:
					Ql_Debug_Trace("<-- CFUN Status :%d -->\r\n", msg.param2);
																// Minimum functionality (when started from ALARM)
					if(msg.param2 == 0)
					{
						Ql_Debug_Trace("<-- Enabling full functionality -->\r\n");

					}
					setState(SF_STATE_FULL_FUNC);
					break;

				case URC_SYS_INIT_STATE_IND:
					Ql_Debug_Trace("<-- Sys Init Status %d -->\r\n", msg.param2);
					break;

				case URC_SIM_CARD_STATE_IND:
					if (SIM_STAT_READY == msg.param2)
					{
						Ql_Debug_Trace("<-- SIM card is ready -->\r\n");
					}
					else
					{
						Ql_Debug_Trace("<-- SIM card is not available , cause:%d -->\r\n", msg.param2);
						/* cause: 0 = SIM card not inserted
						 * 2 = Need to input PIN code
						 * 3 = Need to input PUK code
						 * 9 = SIM card is not recognized
						 */
					}
					break;

				case URC_GSM_NW_STATE_IND:
					break;

				case URC_GPRS_NW_STATE_IND:
					if (NW_STAT_REGISTERED == msg.param2) //|| msg.param2 == NW_STAT_NOT_ACTIVE)
					{
						Ql_Debug_Trace("<-- GPRS ready -->\r\n");
						if(state == SF_STATE_DATA_READY)
							setState(SF_STATE_GPRS_READY);
						break;
					}
					else
					{
						Ql_Debug_Trace("<-- GPRS network status :%d -->\r\n",msg.param2);
						/* status: 0 = Not registered , module not currently search a new operator
						 * 2 = Not registered , but module is currently searching a new operator
						 * 3 = Registration denied
						 */

						u32 rssi;
						u32 ber;
						RIL_NW_GetSignalQuality (&rssi , &ber);
						Ql_Debug_Trace("<-- Signal strength :%d, BER:%d -->\r\n", rssi , ber);
					}
						break;

					default:
						Ql_Debug_Trace("<-- Other URC: type=%d\r\n", msg.param1);
						break;
					}
		}
	}
}



void send_callback(s32 error)
{
	if(error)
	{
		Ql_Debug_Trace("Failed to send data! [%d]\r\n", error);
		//Sf_DeactivateGPRS ();
		return;
	}

	// Send header
	while(sent < dataLength)
	{
	//	s32 ret = Sf_TCP_Send ((u8*)data + sent , dataLength - sent);
		Ql_Debug_Trace("Failed to send data! [%d]\r\n");
/*		// Blocking
//		if(ret == SOC_WOULDBLOCK)
			return;

		// Error
//		else if(ret < 0)
		{
			Ql_Debug_Trace("Failed to send data! [%d]\r\n", ret);
		//	Sf_DeactivateGPRS ();
			setState(SF_STATE_FAILURE);
			return;
		}

		sent += ret;
	*/
	}

	setState(SF_STATE_DATA_SENT);

}

void receive_callback(s32 error)
{
	if(error)
	{
		Ql_Debug_Trace("Failed to receive data! [%d]\r\n", error);
		setState(SF_STATE_FAILURE);
		return;
	}

	// Read header
	s32 ret;
	do
	{
	//	ret = Sf_TCP_Recv ((u8*) response + received , 511 - received);

		// Blocking
		if(ret == SOC_WOULDBLOCK)
			return;

			// Error
		else if(ret < 0)
		{
			Ql_Debug_Trace("Failed to receive data! [%d]\r\n", ret);
			setState(SF_STATE_FAILURE);
			return;
		}

		received += ret;
	}
	while(ret != 0);

	// Terminate response string.
	response[received] = '\0';

	setState(SF_STATE_DATA_RECEIVED);

}


#endif //__EXAMPLE_MULTITASK1__
