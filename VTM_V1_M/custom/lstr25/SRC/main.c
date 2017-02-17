/*****************************************************************************
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained
 *  herein is confidential. The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of Quectel Co., Ltd. 2013
 *
 *****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   main.c
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   This app demonstrates how to send AT command with RIL API, and transparently
 *   transfer the response through MAIN UART. And how to use UART port.
 *   Developer can program the application based on this example.
 *
 * main task:
 * It consist of Radio interface initialization,all tasks initialization and one second timer.
 ****************************************************************************/
#ifdef __CUSTOMER_CODE__
//#ifdef __EXAMPLE_FOTA_FTP__
/*
 *
 * ************** Directories ***************
 *
 */

#include "system.h"
#include "gpio.h"
#include "custom_feature_def.h"
#include "ril.h"
#include "ril_util.h"
#include "ril_network.h"
#include "ril_telephony.h"
#include "ql_stdlib.h"
#include "ql_error.h"
#include "ql_trace.h"
#include "ql_timer.h"
#include "ql_uart.h"
#include "ql_system.h"
//#include "ql_gpio.h"
#include "ql_adc.h"
#include "sms.h"
#include "uart1.h"
#include "task2.h"
#include "task3.h"
#include "rtc.h"
/*
 *
 * ************** Macros ***************
 *
 */

/*
 *
 * ************** function prototypes ***************
 *
 */
static void Task_Timer_handler(u32 timerId, void* param);
/*
 *
 * ************** Object declaration ****************
 *
 */

s_system_info st_sys;

s_interval st_interval;

extern s_gps_info st_gps_info;
extern s_mc_info st_mc_info;
extern s_txbuff_info st_txbuff_info;

extern s_gprs_data st_gprs_data;
extern s_gprs_dataptr st_gprsdata;
extern s_ram_array st_ram_array;
extern s_glob_var st_glob_var;
extern s_globflags st_globflags;

extern s_rtc_cclk st_rtc_cclk;
TASK_EVENTS m_task_evnt;

//extern u8 u8txvector[TX_BUFFER_LEN];

//extern u8 u8rxvector[RECV_BUFFER_LEN];

//extern u16 u16txlength;

//extern u16 u16rxlength;

/*
 *
 * ************** Main task starts ***************
 *
 */

void proc_main_task(s32 taskId) {

	/*
	 *
	 *
	 */

	ST_MSG msg;
	int i2 = 0;
	u8 u8_ret = 0;
	u16 u16length = 0;

	//int iRet = 0, iRet2 = 0;

	// m_tcp_info.m_remain_len = NULL ;

	// u8txvector = NULL ;

	// u8txvector = NULL ;

	// u16txlength = NULL;
	st_ram_array.u16WritePtr = 0;
	st_ram_array.u16ReadPtr = 0;

	Ql_Timer_Register(TASK2_TIMER, Task_Timer_handler, NULL);

	GPIO_Init();
	//Ql_Debug_Trace("\r\n task_start:%d", taskId);  //disable by amruta for testing
	Initaize_AllGlobalVar();

	st_interval.us_polling = 15;
	st_interval.us_transmit = 65;
	st_ram_array.bRewind = FALSE;                //initialization of flag

	while (TRUE) {

		//Ql_Debug_Trace("main task  run \r\n");  //disable by amruta for testing

		//APP_DEBUG("\r\n main task  run -->");

		Ql_OS_GetMessage(&msg);

		switch (msg.message) {

		case MSG_ID_RIL_READY:

			APP_DEBUG("\r\n RIL READY -->")
			;

			Ql_RIL_Initialize();

			Ql_OS_SetEvent(m_task_evnt.TaskEvent1, EVENT_FLAG0); //set event to next task

			break;
		case MSG_ID_URC_INDICATION:

			APP_DEBUG("<-- Received URC: type: %d, -->\r\n", msg.param1)
			;

			switch (msg.param1) {

			case URC_SYS_INIT_STATE_IND:

				APP_DEBUG("<-- Sys Init Status %d -->\r\n", msg.param2)
				;

				break;
			case URC_SIM_CARD_STATE_IND:

				APP_DEBUG("<--Start_task2.-->\r\n")
				;
				Ql_Timer_Start(TASK2_TIMER, TASK_CALL_TIMER, TRUE); // start 1 sec timer
				switch (msg.param2)

				{

				case SIM_STAT_NOT_INSERTED:

					APP_DEBUG("<-- SIM_Card_NOT_Inserted -->\r\n")
					;

					break;

				case SIM_STAT_READY:

					APP_DEBUG("<-- SIM_Card_STATUS_OK -->\r\n")
					;

					break;
				}

				break;
			case URC_GSM_NW_STATE_IND:
				APP_DEBUG("<-- GSM Network Status:%d -->\r\n", msg.param2)
				;
				switch (msg.param2) {

				case NW_STAT_REGISTERED:
					APP_DEBUG("<-- NW_REGISTERED -->\r\n")
					;
					break;
				case NW_STAT_REGISTERED_ROAMING:
					APP_DEBUG("<-- NW_ROAMING -->\r\n")
					;
					break;
				}

				break;
			case URC_GPRS_NW_STATE_IND:

				APP_DEBUG("<-- GPRS Network Status:%d -->\r\n", msg.param2)
				;

				switch (msg.param2) {

				case NW_STAT_REGISTERED:

					APP_DEBUG("<-- GPRS_OK -->\r\n")
					;
					//	if (u8_Cnt == 0) {
					//		APP_DEBUG("<-- GPRS_Fail -->\r\n");
					//	}
					//	/*
					/*
					 Ql_strcpy((char*) st_gprs_data.u8txvector,
					 "GET http://www.tracerivms.com/rfid/getData.php?action=getDcDet&dc_no=d-4&showpending=1 HTTP/1.0\r\n\r\n");
					 st_gprs_data.u16txlength = Ql_strlen(
					 (const char*) st_gprs_data.u8txvector);
					 */
					//rgt_rtc_SetClock(&st_rtc_cclk);
					//	APP_DEBUG("<--Start_task2.-->\r\n")
					//	;
					//	Ql_Timer_Start(TASK2_TIMER, TASK_CALL_TIMER, TRUE);
					//	 */
					break;
				case NW_STAT_REGISTERED_ROAMING:
					APP_DEBUG("<-- ROAM_GPRS_OK -->\r\n")
					;
					break;
				}
				break;
			case URC_CFUN_STATE_IND:
				APP_DEBUG("<-- CFUN Status:%d -->\r\n", msg.param2)
				;
				break;

			case URC_NEW_SMS_IND:
				APP_DEBUG("<-- New SMS Arrives: index=%d\r\n", msg.param2)
				;
				vts_sms_InitSMSService();

				Ql_OS_SendMessage(7, MSG_ID_USER_DATA, 1, 1);

				break;
			case URC_MODULE_VOLTAGE_IND:
				APP_DEBUG("<-- VBatt Voltage Ind: type=%d\r\n", msg.param2)
				;
				break;

			default:
				//    APP_DEBUG("<-- Other URC: type=%d\r\n", msg.param1);          // Disable By K.Mani
				break;
			}
			break;

		case MSG_ID_USER_DATA: {    ///process user data

			u32 u32TaskId = 0;

			u32TaskId = msg.srcTaskId;
			APP_DEBUG(" main_task_user_data:%d\r\n", u32TaskId);
			switch (u32TaskId) {

			case MAIN_TASK: {

				APP_DEBUG("\r\nTASK1 MSG");

				switch (msg.param1) {

				case MSG_POLLEXCEED: { // if polling interval exceed collect data

					APP_DEBUG("MSG_POLLEXCEED\r\n");
					//Data_Load(&st_gps_info, &st_mc_info);
					Data_PushRam();
					// to get data from all structures

					//	Ql_strcpy((char*) st_gprs_data.u8txvector,
					//(const char*) "GET http://www.tracerivms.com/rfid/getData.php?action=getDcDet&dc_no=d-4&showpending=1 HTTP/1.0\r\n\r\n");
					//	st_gprs_data.u16txlength = Ql_strlen(
					//			(const char*) st_gprs_data.u8txvector);

				}
					break;

				case MSG_TRANSMITXCEED: {

					APP_DEBUG("MSG_TRANSMITXCEED");
					//check if gprs transmission is completed then only allow next transmission otherwise increase transmit time
					if (st_globflags.bGprsDataSendProcess == FALSE) {

						if (Data_Retrieve() == TRUE) {

							u16length = *st_gprsdata.u16inLength;

							for (i2 = 0; i2 < u16length; i2++) {
								APP_DEBUG(":%X", st_gprsdata.u8ptrindata[i2]);
							}

							APP_DEBUG("TX_Length:%d\r\n", u16length);
							Ql_OS_SendMessage(TCP_TASK, MSG_ID_USER_DATA, 1, 0);

						} else {
							APP_DEBUG("DATA_RETRIVE_FAIL\r\n");
						}
						st_globflags.bGprsDataSendProcess = TRUE;
					} else {
						st_interval.us_transmit = 10;
					}
				}

					break;
				default:
					break;

				}
			}
				break;

			case UART_TASK: {

			}
				break;

			case TCP_TASK: {

			}
				break;

			case FILE_TASK: {

			}

				break;
			case FOTA_TASK: {

			}
				break;
			case SMS_TASK: {

			}
				break;

			default:

				break;

			}
		}
			break;
// task initialization  failure check
		case MSG_ID_TASK_INIT_DATA: {

			switch (msg.param1) {

			case TASK1:
				APP_DEBUG(" Task1_fail\r\n")
				;
				break;
			case TASK2:
				APP_DEBUG(" Task2_fail\r\n")
				;
				break;
			case TASK3:
				APP_DEBUG(" Task3_fail\r\n")
				;
				break;
			case TASK4:
				APP_DEBUG(" Task4_fail\r\n")
				;
				break;
			default:
				break;
			}
		}
			break;
		default:
			break;

		}

	}


}
////////////////////////////////////////////////////////////////////////////////////////////////
void Task_Timer_handler(u32 timerId, void* param) {

	//APP_DEBUG("TASK 2_Timer_enter");

	if (timerId == TASK2_TIMER) {

		// For data polling

		if (st_interval.us_polling > 0)
			st_interval.us_polling--;
		else {
			st_interval.us_polling = 15; // Load configure interval
			Ql_OS_SendMessage(MAIN_TASK, MSG_ID_USER_DATA, MSG_POLLEXCEED, 0);
		}

		if (st_interval.us_transmit > 0)
			st_interval.us_transmit--;
		else {
			st_interval.us_transmit = 65; // Load configuration value
			Ql_OS_SendMessage(MAIN_TASK, MSG_ID_USER_DATA, MSG_TRANSMITXCEED,
					0);
		}

		//APP_DEBUG("TIMER_CNT: %d\r\n", u8_Cnt);

		//Ql_Timer_Stop(TASK2_TIMER);

		//m_tcp_info.bstart_task2 = TRUE;

		//Ql_OS_SendMessage(main_task2_id, MSG_ID_USER_DATA, 1, 1);
		//Ql_OS_SendMessage(5, MSG_ID_USER_DATA, 1, 1);

		//collect data every one sec
		Data_Load(&st_gps_info, &st_mc_info);
	}
}
/////////////////////////////////////////////

#endif // __CUSTOMER_CODE__