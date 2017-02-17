#ifdef __EXAMPLE_MULTITASK28_12__


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
#include "global.h"






//#include "ql_gpio.h"


#include "ril.h"
#include "ril_util.h"
#include "ril_system.h"
#include "ril_network.h"
#include "ril_telephony.h"

#include "gpio.h"
#include "uart1.h"



#define MSG_ID_SF_STATE		(MSG_ID_USER_START + 101)
#define MSG_ID_SF_GPIO_STATE    (MSG_ID_USER_START + 202)

TASK_EVENTS m_task_evnt;

#define  TASK_CALL_TIMER     1000

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

typedef enum
{
	SF_GPIO_STARTED ,
	SF_GPIO_READ ,
	SF_GPIO_WRITE ,
	SF_GPIO_FAIL ,

} Enum_Sf_GPIO_State;



char data [512];
char response [515];

s32 dataLength;
s32 sent , received;

static void setState(Enum_Sf_State state);
static void prepareData(void);
static void writeData(void);
static void sendData(void);
static void handleResponse(void);
static void upgrade(const char* url);
static void failure(void);
static void finish(void);

void send_callback(s32 error);
void receive_callback(s32 error);

static void setState(Enum_Sf_State state)
{
	Ql_Debug_Trace("<-- setSSState %d -->\r\n", state);
	Ql_OS_SendMessage (0, MSG_ID_SF_STATE , state , 0);
}


static void setGPIO_state(Enum_Sf_GPIO_State gpio_state)
{
	Ql_Debug_Trace("<-- set GPIO State %d -->\r\n", gpio_state);
//	Ql_OS_SendMessage (2, MSG_ID_SF_GPIO_STATE ,0 ,0);	// gpio_state , 2);
//	Ql_OS_SendMessage (1, MSG_ID_SF_GPIO_STATE , gpio_state , 0);
}


static void upgrade(const char* url)
{
	Ql_Debug_Trace("<-- UPGRADING -->\r\n");

}


static void prepareData(void)
{
/*	Ql_Debug_Trace("<-- Preparing data -->\r\n");
//	//setGPIO_state(SF_GPIO_READ);
	Ql_OS_SendMessage (3, MSG_ID_SF_GPIO_STATE , 1 , 0);
//	Ql_Debug_Trace("<-- Preparing data -->\r\n");
//	Ql_OS_SendMessage (3, MSG_ID_SF_GPIO_STATE , 2 , 0);*/
	Ql_OS_SendMessage (3, MSG_ID_SF_GPIO_STATE , 1 , 0);	//read
	Ql_Debug_Trace("\r\n **READ OK** \r\n");
}

static void writeData(void)
{
/*	Ql_Debug_Trace("<-- Writing data -->\r\n");
//	//setGPIO_state(SF_GPIO_READ);
	Ql_OS_SendMessage (3, MSG_ID_SF_GPIO_STATE , 2 , 0);	*/
	Ql_OS_SendMessage (3, MSG_ID_SF_GPIO_STATE , 2 , 0);	//write
	Ql_Debug_Trace("\r\n **WRITE OK** \r\n");
}

static void sendData(void)
{
	Ql_Debug_Trace("<-- Sending data -->\r\n");

}

static void handleResponse(void)
{

	Ql_Debug_Trace("<-- Handle response -->\r\n");
	Ql_Debug_Trace("%s\r\n", response);

}

static void finish ()
{
	Ql_Debug_Trace("<-- FINISH -->\r\n");

}

static void failure ()
{
	Ql_Debug_Trace("<-- FAILURE -->\r\n");
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
//					prepareData ();
//					writeData();
					//setGPIO_state(SF_GPIO_READ);
					break;

				case SF_STATE_GPRS_READY:
					Ql_Debug_Trace("G\r\n");
//					sendData ();
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
					Ql_Debug_Trace("1\r\n");
					break;
			}
			Ql_OS_SendMessage (3, MSG_ID_SF_GPIO_STATE , 0 , 0);	//write
			Ql_Debug_Trace("\r\n M \r\n");

			////editing at 10
			//Ql_Debug_Trace("<-- MSG_ID_SF_STATE = %d\r\n", msg.param1);
			//
		}

		else if(msg.message == MSG_ID_RIL_READY)
		{
			Ql_Debug_Trace("<-- RIL is ready -->\r\n");
			Ql_RIL_Initialize ();
			setState(SF_STATE_RIL_READY);
			//Ql_OS_SetEvent(m_task_evnt.TaskEvent1, EVENT_FLAG0);
//			Ql_OS_SendMessage (3, MSG_ID_SF_GPIO_STATE , 0 , 0);
//			Ql_Timer_Start(TASK2_TIMER, TASK_CALL_TIMER, TRUE); // start 1 sec timer	//main
//			Ql_Debug_Trace("\r\n **Set_Event_OK** \r\n");

		}
		else if(msg.message == MSG_ID_URC_INDICATION)
		{
			switch (msg.param1)
			{
				case URC_CFUN_STATE_IND:
//					Ql_Debug_Trace("<-- URC_CFUN_STATE_IND -->\r\n");
					break;

				case URC_SYS_INIT_STATE_IND:
//					Ql_Debug_Trace("<-- URC_SYS_INIT_STATE_IND -->\r\n");
					break;

				case URC_SIM_CARD_STATE_IND:
//					Ql_Debug_Trace("<-- URC_SIM_CARD_STATE_IND -->\r\n");

					break;

				case URC_GSM_NW_STATE_IND:
//					Ql_Debug_Trace("<-- URC_GSM_NW_STATE_IND -->\r\n");
					break;

				case URC_GPRS_NW_STATE_IND:
//					Ql_Debug_Trace("<-- URC_GPRS_NW_STATE_IND -->\r\n");
					break;

				default:
					Ql_Debug_Trace("<-- Other URC: type=%d\r\n", msg.param1);
					break;
			}
		}

	}
//	Ql_OS_SendMessage (3, MSG_ID_SF_GPIO_STATE , 0 , 0);	//write
//	Ql_Debug_Trace("\r\n **WRITE OK** \r\n");
}

//ret = Ql_Timer_Register(TASK1_TIMER, TASK1_Timer_handler, NULL);

void send_callback(s32 error)
{
	if(error)
	{
		Ql_Debug_Trace("Failed to send data! [%d]\r\n", error);

		return;
	}

//	setState(SF_STATE_DATA_SENT);

}

void receive_callback(s32 error)
{
	if(error)
	{
		Ql_Debug_Trace("Failed to receive data! [%d]\r\n", error);
//		setState(SF_STATE_FAILURE);
		return;
	}
//	setState(SF_STATE_DATA_RECEIVED);
}


void proc_subtask1(s32 TaskId)
{
	Ql_Debug_Trace("Lateral emSoft the world of navigation \r\n");
	Ql_Debug_Trace("GPIO Application wake up\r\n");



	GPIO_Init();
	Ql_Debug_Trace("GPIO_Init \r\n");

	ST_MSG subtask1_msg;

	Enum_Sf_GPIO_State gpio_state = SF_GPIO_STARTED;
//	m_task_evnt.TaskEvent1 = Ql_OS_CreateEvent("\r\n task1");
	Ql_Debug_Trace("GPIO_ST_MSG subtask1_msg \r\n");


//
//	while (Ql_OS_WaitEvent(m_task_evnt.TaskEvent1, EVENT_FLAG0) != 0) {
//}
	while(TRUE)
	{
//		Ql_Debug_Trace("GPIO_while (TRUE)");

//		Ql_Debug_Trace("S\n\r");
		APP_DEBUG("S\n\r");
		gpio_state = subtask1_msg.param1;

		Ql_OS_GetMessage(&subtask1_msg);

//		Ql_Debug_Trace("ok g ok");
		APP_DEBUG(" ok g ok\n\r");



		switch(gpio_state)
		{
//			case SF_GPIO_STARTED:
//				Ql_Debug_Trace("GPIO STARTED");

//				break;
			case SF_GPIO_READ:
				Ql_Debug_Trace("GPIO READ");
				Ql_Debug_Trace("\r\n<-- The GPIO level value: %d -->\r\n", Ql_GPIO_GetLevel(PINNAME_PCM_IN));
				break;
			case SF_GPIO_WRITE:
				Ql_Debug_Trace("GPIO WRITE");
				Ql_GPIO_SetLevel(PINNAME_NETLIGHT, PINLEVEL_LOW);
				Ql_Debug_Trace("\r\n GPIO NETLIGHT is LOW \r\n \r\n");
				break;
			case SF_GPIO_FAIL:
				Ql_Debug_Trace("GPIO FAIL");
				break;
			default:
			//	Ql_Debug_Trace("S \n\r");
				APP_DEBUG("S \n\r");
				break;
		}
		Ql_OS_SendMessage (0, MSG_ID_SF_STATE , 3 , 0);	//write

//			setState(SF_STATE_DATA_READY);
//			setState(SF_STATE_DATA_READY);
//		break;

	}



}




/*
	ST_MSG subtask1_msg;
	while (TRUE)
	{
//		Ql_OS_GetMessage (&msg);
		Ql_OS_GetMessage(&subtask1_msg);

		if(subtask1_msg.message == MSG_ID_SF_GPIO_STATE)
		{
			gpio_state = msg.param1;
			switch (gpio_state)
				{
					case SF_STATE_FULL_FUNC:

	}
*/
void GPIO_Init(void) {
	Enum_PinName gpioPin1 = PINNAME_NETLIGHT;
	Enum_PinName gpioPin2 = PINNAME_DTR;
	Enum_PinName gpioPin3 = PINNAME_RI;
	Enum_PinName gpioPin4 = PINNAME_DCD;
	Enum_PinName gpioPin5 = PINNAME_CTS;
	Enum_PinName gpioPin6 = PINNAME_RTS;
	Enum_PinName gpioPin7 = PINNAME_PCM_CLK;
	Enum_PinName gpioPin8 = PINNAME_PCM_SYNC;
	Enum_PinName gpioPin9 = PINNAME_PCM_IN;
	Enum_PinName gpioPin10 = PINNAME_PCM_OUT;
#ifdef NOTUSED
	Enum_PinName gpioPin11 = PINNAME_RXD_AUX;
	Enum_PinName gpioPin11 = PINNAME_TXD_AUX;
#endif

	Enum_PinLevel high = PINLEVEL_HIGH;
	Enum_PinLevel low = PINLEVEL_LOW;
	Ql_GPIO_Init(gpioPin1, PINDIRECTION_OUT, high, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin2, PINDIRECTION_OUT, high, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin3, PINDIRECTION_IN, low, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin4, PINDIRECTION_IN, low, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin5, PINDIRECTION_IN, high, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin6, PINDIRECTION_OUT, low, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin7, PINDIRECTION_IN, high, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin8, PINDIRECTION_OUT, high, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin9, PINDIRECTION_IN, high, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin10, PINDIRECTION_OUT, low, PINPULLSEL_DISABLE);
#ifdef NOTUSED
	Ql_GPIO_Init(gpioPin11, PINDIRECTION_OUT, high, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin12, PINDIRECTION_OUT, high, PINPULLSEL_DISABLE);
#endif
}


#endif //__EXAMPLE_MULTITASK1__


/*
Ql_Timer_Start(TASK3_TIMER, 1000, TRUE);  // start the timer	//task1
Ql_Timer_Start(TASK2_TIMER, TASK_CALL_TIMER, TRUE); // start 1 sec timer	//main
Ql_Timer_Start(TCP_TIMER_ID, TCP_TIMER_PERIOD, TRUE); // start the TCP timer	//task2
Ql_Timer_Start(TIMEOUT_90S_TIMER_ID, TIMEOUT_90S_PERIOD, FALSE);			//task2
			timeout_90S_monitor = TRUE;
Ql_Timer_Start(TIMEOUT_90S_TIMER_ID, TIMEOUT_90S_PERIOD,FALSE);				//task2
			timeout_90S_monitor = TRUE;


*/


