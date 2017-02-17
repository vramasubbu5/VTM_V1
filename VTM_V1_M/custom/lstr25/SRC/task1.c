/*
 * task1.c
 *
 *  Created on: Jun 24, 2016
 *      Author: Amruta
 */
/***************************************************************************/
/*This task consist of all three UART initialization in which main UART    */
/* is for debug and programming(UART1),Debug  UART is for micro controller */
/*communication(UART2),Auxillary UART for GPS communication(UART3).        */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
#include "gpio.h"
#include "system.h"
#include "string.h"
#include "rtc.h"

/*
 *
 * ************** Macros ***************
 *
 */
#define UART1_ENABLE
#define UART2_ENABLE
#define UART3_ENABLE
#define COLD_ROOM_ENABLE
#define MAX_STR_QTY    7
#define MAX_STR_LENGTH  100
#define GPS_HEADER_LENGTH  7
#define STR_STX      '$'

#define DATA_IN       "1"
#define DATA_OUT     "2"
#define MCTXCNT       "2"
#define DATA_TYPE      "1"
//#define STR_ETX      '\r'
#define UART2_DATABUF_SIZE 100

#define FAIL                     0
#define SUCCESS                  1
/*
 *
 * ************** function prototypes ***************
 *
 */
void Collect_GPS_Data(s32 datalength, u8 *data);
u8 gps_ParseRMCString(const u8 * data, s_gps_info * st_gps_data);
void gps_ParseTXTString(u8* data);

u8 Cal_Str_checksum(const char * restrict u8_ptrdata);
u8 Comp_Str_checksum(const char * restrict u8_ptrdata);
static unsigned char Nmea_checksum(const char * restrict uc_ptrdata,
		unsigned short us_length);
u8 Internal_ProtocolParse(const char * u8_ptrdata);
u8 CR_ProtocolParse(const char * restrict u8_ptrdata);
u8 TR_ProtocolParse(const char * restrict u8_ptrdata);
void Form_McAck(void);
static void TASK1_Timer_handler(u32 timerId, void* param);
/*
 *
 * ************** Object declaration ****************
 *
 */

static u8 u8_GPSData[UART2_DATABUF_SIZE];
extern s_task1_cnt st_task1_cnt;
extern s_system_info st_sys;
extern s_rtc_cclk st_rtc_cclk;
extern TASK_EVENTS m_task_evnt;

extern s_globflags st_globflags;
s_gps_info st_gps_info;
s_mc_info st_mc_info;

s_gps_buff st_gps_buff;

/*
 *
 * ************** Main task starts ***************
 *
 */
/***************************************************************************/
/*  Function   : Uart_Initialize                                           */
/*-------------------------------------------------------------------------*/
/*  Object     : To Initialise the uart  				                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/

void proc_main_task1(s32 taskId) {

	ST_MSG msg1;

	//int iRet = 0, iRet2 = 0;
//SerialInit();

	char sc_return = 0;
	bool bTaskStart = FALSE;

	//Ql_Debug_Trace("\r\n task_start:%d", taskId);   //disable by amruta for testing

	m_task_evnt.TaskEvent1 = Ql_OS_CreateEvent("\r\n task1");

	while (Ql_OS_WaitEvent(m_task_evnt.TaskEvent1, EVENT_FLAG0) != 0) {



	}




	//Serial_Init();
	sc_return = Uart_Initialize();

	if (sc_return == QL_RET_OK) {
		//	vts_utils_PrintVersion();
		APP_DEBUG("\r\n Uart_Initialization_Success ");
		bTaskStart = TRUE;
		Ql_Timer_Start(TASK3_TIMER, 1000, TRUE);  // start the timer


		Ql_OS_SetEvent(m_task_evnt.TaskEvent2, EVENT_FLAG0);


	} else {
		// Send message to main task task2 Initialization fail
		Ql_OS_SendMessage(0, MSG_ID_TASK_INIT_DATA, TASK1, 1);
		APP_DEBUG("\r\n Uart_Initialization_Failure ");
	}
	//Ql_GPIO_SetLevel(gpioPin10, PINLEVEL_LOW);
	while (bTaskStart) {

		APP_DEBUG("\r\n task1_run ");

		Ql_OS_GetMessage(&msg1);

		switch (msg1.message) {
		/*
		 case MSG_ID_USER_DATA:
		 //taskId = proc_main_task1();

		 iRet = Ql_OS_SendMessage(2, MSG_ID_USER_DATA, msg.param1,
		 msg.param2);
		 if (iRet < 0) {
		 APP_DEBUG(
		 "\r\n<--failed!!, Ql_OS_SendMessage12(1, %d, %d) fail,  ret=%d-->\r\n",
		 msg.param1, msg.param2, iRet);
		 }
		 APP_DEBUG("\r\n<--Ql_OS_SendMes_Task2(%d, %d, %d) ret=%d-->\r\n",
		 MSG_ID_USER_DATA, msg.param1, msg.param2, iRet)
		 ;
		 break;
		 */
		default:
			APP_DEBUG("<-- default-->\r\n")
			;
			break;

		}

	}

}

/***************************************************************************/
/*  All Functions                                                          */
/***************************************************************************/

/***************************************************************************/
/*  Function   : ReadSerialPort                                            */
/*-------------------------------------------------------------------------*/
/*  Object     : Read data from serial port                                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
s32 ReadSerialPort(Enum_SerialPort port, /*[out]*/u8* pBuffer, /*[in]*/
u32 bufLen) {
	s32 rdLen = 0;
	s32 rdTotalLen = 0;
	if (NULL == pBuffer || 0 == bufLen) {
		return -1;
	}
	Ql_memset(pBuffer, 0x0, bufLen);

	while (1) {
		rdLen = Ql_UART_Read(port, pBuffer + rdTotalLen, bufLen - rdTotalLen);
		if (rdLen <= 0)  // All data is read out, or Serial Port Error!
				{
			break;
		}
		rdTotalLen += rdLen;
		// Continue to read...
	}
	if (rdLen < 0) // Serial Port Error!
			{
		//APP_DEBUG("Fail to read from port[%d]\r\n", port);
		return -99;
	}
	return rdTotalLen;

}

/***************************************************************************/
/*  Function   :   CallBack_UART1_Hdlr                                     */
/*-------------------------------------------------------------------------*/
/*  Object     : Uart1 handeler                                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/

void CallBack_UART1_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg,
		bool level, void* customizedPara) {
	uart[0].eport_no = UART_PORT1;
	//Ql_memset(uart[0].u8_receivebuff, 0x0,sizeof(uart[0].u8_receivebuff));
	switch (msg) {
	case EVENT_UART_READY_TO_READ: {
		if (uart[0].eport_no == port) {
			s32 totalBytes = ReadSerialPort(port, uart[0].u8_receivebuff,
					sizeof(uart[0].u8_receivebuff));
			if (totalBytes <= 0) {
				APP_DEBUG("<-- No data in UART buffer! -->\r\n");
				return;
			}

			uart[0].u8_recived = TRUE;
			{ // Read data from UART

				char* pCh = NULL;

				pCh = Ql_strstr((char*) uart[0].u8_receivebuff, "\r\n");
				if (pCh) {
					*(pCh + 0) = '\0';
					*(pCh + 1) = '\0';
				}

				// No permission for single <cr><lf>
				if (Ql_strlen((char*) uart[0].u8_receivebuff) == 0) {
					return;
				}
				//	ret = Ql_RIL_SendATCmd((char*) uart[0].u8_receivebuff, totalBytes,
				//			ATResponse_Handler, NULL, 0);  //here check rx buffer
			}
		}
		break;
	}  //case READ
	case EVENT_UART_READY_TO_WRITE:
		break;
	default:
		break;
	}  //switch bracket
}
/***************************************************************************/
/*  Function   :   CallBack_UART2_Hdlr                                     */
/*-------------------------------------------------------------------------*/
/*  Object     : Uart2 handler                                             */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/

void CallBack_UART2_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg,
		bool level, void* customizedPara) {
	uart[1].eport_no = UART_PORT2;
	Ql_memset(uart[1].u8_receivebuff, 0x0, sizeof(uart[1].u8_receivebuff));
	//APP_DEBUG("<-- CallBack_UART2_Hdlr -->\r\n");
	switch (msg) {
	case EVENT_UART_READY_TO_READ: {
		if (uart[1].eport_no == port) {
			s32 totalBytes = ReadSerialPort(port, uart[1].u8_receivebuff,
					sizeof(uart[1].u8_receivebuff));
			//APP_DEBUG("<-- Total_byte!:%d -->\r\n", totalBytes);
			if (totalBytes <= 0) {
				//APP_DEBUG("<-- No data in UART buffer! -->\r\n");
				return;
			}

			{ // Read data from UART

				char* pCh = NULL;

				// Echo
				//	Ql_UART_Write(uart[2].eport_no, uart[2].u8_receivebuff,
				//			totalBytes);

				pCh = Ql_strstr((char*) uart[1].u8_receivebuff, "\r\n");
				if (pCh) {
					*(pCh + 0) = '\0';
					*(pCh + 1) = '\0';
				}
				//APP_DEBUG("<-- EVENT_UART_READY_TO_READ -->\r\n");
				Collect_GPS_Data(Ql_strlen((char*) uart[1].u8_receivebuff),
						uart[1].u8_receivebuff);
				Ql_memset(uart[1].u8_receivebuff, 0x0,
						sizeof(uart[1].u8_receivebuff));

				// No permission for single <cr><lf>
				if (Ql_strlen((char*) uart[1].u8_receivebuff) == 0) {
					return;
				}

					//ret = Ql_RIL_SendATCmd((char*) uart[2].u8_receivebuff, totalBytes,
					//		ATResponse_Handler, NULL, 0);  //here check rx buffer
			}			//
		}
		break;
	}  //case READ
	case EVENT_UART_READY_TO_WRITE:
		break;
	default:
		break;
	}  //switch bracket
}
/***************************************************************************/
/*  Function   :   CallBack_UART3_Hdlr                                     */
/*-------------------------------------------------------------------------*/
/*  Object     : Uart3 handler                                             */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/

void CallBack_UART3_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg,
		bool level, void* customizedPara) {
	u8 u8_ret = 0;
	uart[2].eport_no = UART_PORT3;
	Ql_memset(uart[2].u8_receivebuff, 0x0, sizeof(uart[2].u8_receivebuff));
	Ql_memset(uart[2].u8_transmitbuff, 0x0, sizeof(uart[2].u8_transmitbuff));
	switch (msg) {
	case EVENT_UART_READY_TO_READ: {
		if (uart[2].eport_no == port) {
			s32 totalBytes = ReadSerialPort(port, uart[2].u8_receivebuff,
					sizeof(uart[2].u8_receivebuff));
			if (totalBytes <= 0) {
				APP_DEBUG("<-- No data in UART buffer! -->\r\n");
				return;
			}
			uart[2].u8_recived = TRUE;
			{ // Read data from UART

				char* pCh = NULL;

				// Echo

				pCh = Ql_strstr((char*) uart[2].u8_receivebuff, "\r\n");
				if (pCh) {
					*(pCh + 0) = '\0';
					*(pCh + 1) = '\0';
				}

				u8_ret = Internal_ProtocolParse((char*) uart[2].u8_receivebuff);
				if (u8_ret == TRUE) {
					Ql_UART_Write(uart[0].eport_no, uart[2].u8_transmitbuff,
							Ql_strlen((char*) uart[2].u8_transmitbuff));
				}

				// No permission for single <cr><lf>
				if (Ql_strlen((char*) uart[2].u8_receivebuff) == 0) {
					return;
				}
				//	ret = Ql_RIL_SendATCmd((char*) uart[1].u8_receivebuff, totalBytes,
				//			ATResponse_Handler, NULL, 0);  //here check rx buffer
			}
		}
		break;
	}  //case READ
	case EVENT_UART_READY_TO_WRITE:
		//Ql_UART_Write(uart[1].eport_no, uart[1].u8_transmitbuff, sizeof(uart[1].u8_transmitbuff));
		break;
	default:
		break;
	}  //switch bracket
}

/***************************************************************************/
/*  Function   : SerialInit                                                */
/*-------------------------------------------------------------------------*/
/*  Object     : Initialize all serial port                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/

char Uart_Initialize(void) {
	s32 ret = 0;
	uart[0].eport_no = UART_PORT1;
	uart[1].eport_no = UART_PORT2;
	uart[2].eport_no = UART_PORT3;
	uart[0].EvntCallBack = CallBack_UART1_Hdlr;
	uart[1].EvntCallBack = CallBack_UART2_Hdlr;
	uart[2].EvntCallBack = CallBack_UART3_Hdlr;

	// Register & open UART port1
#ifdef UART1_ENABLE
	ret = Ql_UART_Register(uart[0].eport_no, uart[0].EvntCallBack,
	NULL);

	if (ret < QL_RET_OK) {

		return -1;
	}

	ret = Ql_UART_Open(uart[0].eport_no, 115200, FC_NONE);
	if (ret < QL_RET_OK) {

		return -2;
	}
#endif

	// Register & open UART port2
#ifdef UART2_ENABLE
	ret = Ql_UART_Register(uart[1].eport_no, uart[1].EvntCallBack,
	NULL);
	if (ret < QL_RET_OK) {

		return -3;
	}
	ret = Ql_UART_Open(uart[1].eport_no, 9600, FC_NONE);
	if (ret < QL_RET_OK) {

		return -4;
	}
#endif

	// Register & open UART port3
#ifdef UART3_ENABLE
	ret = Ql_UART_Register(uart[2].eport_no, uart[2].EvntCallBack,
	NULL);
	if (ret < QL_RET_OK) {

		return -5;
	}
	ret = Ql_UART_Open(uart[2].eport_no, 115200, FC_NONE);
	if (ret < QL_RET_OK) {

		return -6;
	}
#endif
	ret = Ql_Timer_Register(TASK1_TIMER, TASK1_Timer_handler, NULL);
	if (ret != QL_RET_OK)
		return -7;

	return QL_RET_OK;
}
/***************************************************************************/
/*  Function   : Collect_GPS_Data                                          */
/*-------------------------------------------------------------------------*/
/*  Object     : Collect GPS strings from GPS                              */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
void Collect_GPS_Data(s32 datalength, u8 *data) {

	static char sc_nmeamatrix[MAX_STR_QTY][MAX_STR_LENGTH];
	u16 u16_GPSBuff[500];
	unsigned char uc_strcnt = 0, uc_tempvar1 = 0, u8_ret = 0;

	char * token = NULL;
	char *sctemp = NULL;

	// seperate the strings
	//APP_DEBUG("<-- Collect gps data! -->\r\n");
	Ql_memset(u16_GPSBuff, '\0', sizeof(u16_GPSBuff));
	/*

	 if (bInit_Command == FALSE) {

	 Ql_memset(ctempBuff, '\0', sizeof(ctempBuff));
	 Ql_strcpy(ctempBuff,
	 "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0*29\r\n");  // give this command to gps to allow only GPRMC string
	 //count = (u8) Ql_strlen(ctempBuff);
	 //uart2_send((const u8*) ctempBuff, count);
	 bInit_Command = TRUE;

	 } else {
	 */
	//APP_DEBUG("<-- Collect gps data11111 -->\r\n");
	if (data != NULL) {

		Ql_strncpy((char*) u16_GPSBuff, (const char*) data, datalength);
		//APP_DEBUG("<-- Collect gps data2222 -->\r\n:");
		//APP_DEBUG((char* )data);
		for (token = strtok((char*) u16_GPSBuff, "\n"); token != NULL; token =
				strtok(NULL, "\n")) {

			if (*token == STR_STX) {

				if (uc_strcnt < MAX_STR_QTY) {

					Ql_strncpy(sc_nmeamatrix[uc_strcnt], token,
							Ql_strlen(token));

					//	Ql_strncat(sc_nmeamatrix[uc_strcnt], "\0", 1);

					//APP_DEBUG((char* ) "\r\n** GPS_STR **\r\n\0");

					//APP_DEBUG((char * ) sc_nmeamatrix[uc_strcnt]);

					uc_strcnt++;
				}
				// to add later if its possible
				// else
				//	break;
			}

		}
		//	uart2_var.iGPS_WrPtr = 0;

		// for talker based function call....

		for (uc_tempvar1 = 0; uc_tempvar1 < uc_strcnt; uc_tempvar1++) {

			if (!(Ql_strncmp(sc_nmeamatrix[uc_tempvar1], "$GPRMC,",
			GPS_HEADER_LENGTH))) {
				//APP_DEBUG((char* ) "\r\n** GPRMC **\r\n\0");

				u8_ret = Nmea_checksum(sc_nmeamatrix[uc_tempvar1],
						Ql_strlen(sc_nmeamatrix[uc_tempvar1])); /*clculate the checksum of received string  */

				if (u8_ret == 1) {

					//APP_DEBUG((char* ) "\r\n** GPRMC_checksum_ok **\r\n\0");
					sctemp = sc_nmeamatrix[uc_tempvar1];

					sctemp += GPS_HEADER_LENGTH;

					if (sctemp != NULL) {

						Ql_strcpy((char*) u8_GPSData, sctemp);

						s_gps_info st_gps_temp;
						// collect the GPRMC string into sc_nmeamatrix  and call gprmc normal parsing
						if (gps_ParseRMCString(u8_GPSData, &st_gps_temp) == TRUE) {

							st_gps_info = st_gps_temp;

						} else {

							Ql_memset(&st_gps_info, 0, sizeof(s_gps_info));
						}

					}
				} else {
					APP_DEBUG((char* ) "\r\n** GPRMC_checksum_fail **\r\n\0");
				}
				break;
			} else if (!(Ql_strncmp(sc_nmeamatrix[uc_tempvar1], "$GPTXT,",
			GPS_HEADER_LENGTH))) {
				APP_DEBUG((char* ) "\r\n** GPTXT **\r\n\0");
				// for antenna status

				u8_ret = Nmea_checksum(sc_nmeamatrix[uc_tempvar1],
						Ql_strlen(sc_nmeamatrix[uc_tempvar1])); /*clculate the checksum of received string  */
				if (u8_ret == 1) {
					APP_DEBUG((char* ) "\r\n** GPTXT_checksum_ok **\r\n\0");
					sctemp = sc_nmeamatrix[uc_tempvar1];

					sctemp += GPS_HEADER_LENGTH;

					if (sctemp != NULL) {

						Ql_strcpy((char*) u8_GPSData, sctemp);
						// collect the GPTXT string into sc_nmeamatrix  and call gprmc normal parsing
						gps_ParseTXTString(u8_GPSData);
					}
				} else {
					APP_DEBUG((char* ) "\r\n** GPTXT_checksum_fail **\r\n\0");
				}
				break;
				// antenna status
			}

		}
	}
	//}

}
/***************************************************************************/
/*  Function   : gps_ParseRMCString                                        */
/*-------------------------------------------------------------------------*/
/*  Object     : To Parse the GPRMC String of the GPS.                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
u8 gps_ParseRMCString(const u8 * data, s_gps_info * st_gps_data) {

	char * token = NULL;

	u8 u8_strcnt = 1;

	u8 u8_gps_par[15];

	Ql_memset(st_gps_buff.u8Rmc, '\0', sizeof(st_gps_buff.u8Rmc));
	Ql_strncpy((char*) st_gps_buff.u8Rmc, (char*) data,
			Ql_strlen((char*) data));

	for (token = strtok((char*) data, ","); token != NULL;
			token = strtok(NULL, ","), u8_strcnt++) {

		Ql_memset(u8_gps_par, '\0', 15);

		Ql_strncpy((char*) u8_gps_par, token, Ql_strlen((char*) token));

		//Ql_strncat((char*) u8_gps_par, "\0", 1);

		//	APP_DEBUG((char * ) u8_gps_par);

		switch (u8_strcnt) {

		case 1: {      //time
			st_gps_data->u8_Time[0] = u8_gps_par[0];  // sec
			st_gps_data->u8_Time[1] = u8_gps_par[1];
			st_gps_data->u8_Time[2] = ':';

			st_gps_data->u8_Time[3] = u8_gps_par[2];  // minute
			st_gps_data->u8_Time[4] = u8_gps_par[3];
			st_gps_data->u8_Time[5] = ':';

			st_gps_data->u8_Time[6] = u8_gps_par[4];  // hour
			st_gps_data->u8_Time[7] = u8_gps_par[5];
			st_gps_data->u8_Time[8] = '\0';
			APP_DEBUG("\r\n");
			APP_DEBUG((char * )st_gps_data->u8_Time);

		}
			break;
		case 2: {
			//APP_DEBUG("GPS_PARSE_CASE2\r\n");
			if (Ql_strcmp((char*) u8_gps_par, "V") == 0) {
				//APP_DEBUG("GPS_STATUS_FAIL\r\n");
				st_gps_data->u8_Lat[0] = '\0';
				st_gps_data->u8_LatDir[0] = '\0';
				st_gps_data->u8_lon[0] = '\0';
				st_gps_data->u8_LonDir[0] = '\0';
				st_gps_data->u8_Alti[0] = '\0';
				st_gps_data->u8_Speed = 0;
				st_gps_data->u8_gps_sts = GPS_STATUS_FAIL;  //GPS is not fixed
				st_task1_cnt.u8Gps_fix_delay = 30;
				st_globflags.bGps_afterfixtmrloaded = FALSE;
				return FAIL;
			} else {
				//APP_DEBUG("GPS_FIX\r\n");
				///*
				//Here Make 30 polling delay fixing time to avoid future date
				if (st_task1_cnt.u8Gps_fix_delay > 0) {
					st_task1_cnt.u8Gps_fix_delay--;
					return FAIL;
				}
				if (st_gps_info.u8_gps_sts == GPS_STATUS_FAIL) {
					//APP_DEBUG("GPS_PARSE_CASE24444444\r\n");
					APP_DEBUG("GPS_STATUS_SUCCESS\r\n");
					st_gps_info.u8_gps_sts = GPS_STATUS_SUCCESS;
					st_globflags.bGPSfixafterboot = TRUE;
				}
				// */
			//	APP_DEBUG("GPS_OK\r\n");
			}
			///*
			//load date and time in rtc date and time
			if (st_gps_info.u8_gps_sts == GPS_STATUS_SUCCESS) {
				if (st_task1_cnt.u16GPS_afterFix_Tmr > 600) { //check gps fixing for 10 min to avoid error in date and time
					if (st_rtc_cclk.bLoadInternalRtc) {
						//DD
						st_rtc_cclk.cdate[0] = st_gps_data->u8_Date[0];
						st_rtc_cclk.cdate[1] = st_gps_data->u8_Date[1];
						st_rtc_cclk.cdate[2] = st_gps_data->u8_Date[2];

						//mm
						st_rtc_cclk.cdate[3] = st_gps_data->u8_Date[3];
						st_rtc_cclk.cdate[4] = st_gps_data->u8_Date[4];
						st_rtc_cclk.cdate[5] = st_gps_data->u8_Date[5];

						//yy
						st_rtc_cclk.cdate[6] = st_gps_data->u8_Date[6];
						st_rtc_cclk.cdate[7] = st_gps_data->u8_Date[7];

						//time
						//HH

						st_rtc_cclk.ctime[0] = st_gps_data->u8_Time[0];
						st_rtc_cclk.ctime[1] = st_gps_data->u8_Time[1];
						st_rtc_cclk.ctime[2] = st_gps_data->u8_Time[2];
						//MM
						st_rtc_cclk.ctime[3] = st_gps_data->u8_Time[3];
						st_rtc_cclk.ctime[4] = st_gps_data->u8_Time[4];
						st_rtc_cclk.ctime[5] = st_gps_data->u8_Time[5];
						//SS
						st_rtc_cclk.ctime[6] = st_gps_data->u8_Time[6];
						st_rtc_cclk.ctime[7] = st_gps_data->u8_Time[7];

						rtc_SetClock(&st_rtc_cclk);
					}

				}

			}

			// */
		}
			break;
		case 3: {   //latitude
			Ql_memset(st_gps_data->u8_Lat, 0x00, sizeof(st_gps_data->u8_Lat));
			Ql_strcpy((char*) st_gps_data->u8_Lat, (char*) u8_gps_par);
			//APP_DEBUG("\r\n");
			//APP_DEBUG((char * )st_gps_data->u8_Lat);
		}
			break;
		case 4: {  //latitude direction
			Ql_strcpy((char*) st_gps_data->u8_LatDir, (char*) u8_gps_par);
			//APP_DEBUG("\r\n");
			//APP_DEBUG((char * )st_gps_data->u8_LatDir);
		}
			break;
		case 5: {  // longitude
			Ql_memset(st_gps_data->u8_lon, 0x00, sizeof(st_gps_data->u8_lon));
			Ql_strcpy((char*) st_gps_data->u8_lon, (char*) u8_gps_par);
			//APP_DEBUG("\r\n");
			//APP_DEBUG((char * )st_gps_data->u8_lon);
		}
			break;
		case 6: {  //longitude direction
			Ql_strcpy((char*) st_gps_data->u8_LonDir, (char*) u8_gps_par);
			//APP_DEBUG("\r\n");
			//APP_DEBUG((char * )st_gps_data->u8_LonDir);
		}
			break;
		case 7: {  //speed

			st_gps_data->u8_Speed = (unsigned char) (Ql_atoi((char*) u8_gps_par)
					* 1.852);
			//APP_DEBUG("Speed_from_GPS:%d\r\n", (char * )st_gps_data->u8_Speed);
		}
			break;
		case 8: {  // course being added
			Ql_strcpy((char*) st_gps_data->u8_Alti, (char*) u8_gps_par);
			//APP_DEBUG("\r\n");
			//APP_DEBUG((char * )st_gps_data->u8_Alti);
		}
			break;
		case 9: { //date
			st_gps_data->u8_Date[0] = u8_gps_par[0];  // date
			st_gps_data->u8_Date[1] = u8_gps_par[1];
			st_gps_data->u8_Date[2] = '/';

			st_gps_data->u8_Date[3] = u8_gps_par[2];  // month
			st_gps_data->u8_Date[4] = u8_gps_par[3];
			st_gps_data->u8_Date[5] = '/';

			st_gps_data->u8_Date[6] = u8_gps_par[4];  // year
			st_gps_data->u8_Date[7] = u8_gps_par[5];
			//APP_DEBUG("\r\n");
			//APP_DEBUG((char * )st_gps_data->u8_Date);

			return TRUE;

		}
			break;
		default:
			break;
		}

	}

	return FALSE;
}
/***************************************************************************/
/*  Function   : gps_ParseTXTString                                        */
/*-------------------------------------------------------------------------*/
/*  Object     : To Parse the GPTXT String of the GPS.                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
void gps_ParseTXTString(u8* data) {
	Ql_memset(st_gps_buff.u8Txt, '\0', sizeof(st_gps_buff.u8Txt));
	Ql_strncpy((char*) st_gps_buff.u8Txt, (char*) data,
			Ql_strlen((char*) data));

	if (strstr((char*) data, (char*) "ANTSTATUS=OK") != NULL) {
		APP_DEBUG("\r\n Gps_Ext_Active_Antenna\r\n");
	} else if ((strstr((char*) data, (char*) "ANTSTATUS=OPEN") != NULL)
			|| (strstr((char*) data, (char*) "ANTSTATUS=SHORT") != NULL)) {
		APP_DEBUG("\r\n Gps_Int_Active_Antenna\r\n");
	}

}

/***************************************************************************/
/*  Function   :   Nmea_checksum                                           */
/*-------------------------------------------------------------------------*/
/*  Object     : Calculate GPS Nmea checksum                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static unsigned char Nmea_checksum(const char * restrict uc_ptrdata,
		unsigned short us_length) {
	/*
	 *
	 *
	 *
	 */

//  to check below routine
	unsigned short us_var1 = 0;

	unsigned char uc_flag1 = 0, uc_flag2 = 0, uc_ret = 0;

	unsigned char uc_chksum1 = 0, uc_chksum2 = 0;

	unsigned char uc_var1 = 0, uc_var2 = 0;

	if (uc_ptrdata != NULL) {

		uc_chksum1 = *(uc_ptrdata + 1); // For checksum calculation except '$'

		for (us_var1 = 2; us_var1 < us_length; us_var1++) {

			if (*(uc_ptrdata + us_var1) == '*') {
// For checksum calculation except '*'
				uc_flag1 = 1;
				continue;
			}
			if (uc_flag1 == 0)
				uc_chksum1 ^= *(uc_ptrdata + us_var1);
			else {

				uc_var1 = *(uc_ptrdata + us_var1);

				if (!(uc_var1 >= '0' && uc_var1 <= 'F'))
					break;

				uc_var2 =
						(uc_var1 >= '0' && uc_var1 <= '9') ?
								(uc_var1 - 0x30) : (uc_var1 - 55);

				if (uc_flag2 == 0) {
					uc_chksum2 = (uc_var2 << 4) & 0xf0;
					uc_flag2 = 1;
				} else {
					uc_chksum2 += uc_var2 & 0x0f;
					if (uc_chksum2 == uc_chksum1)
						uc_ret = 1;
					break;
				}

			}

		}
	}

	return uc_ret;
}

//#endif
/***************************************************************************/
/*  Function   :  Form_McAck                                               */
/*-------------------------------------------------------------------------*/
/*  Object     : Form acknowledgment command for micro controller          */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
void Form_McAck(void) {

	char cBuff[50], cBuff1[5];
	u8 u8_temp = 0;
	Ql_memset(uart[1].u8_transmitbuff, 0x0, sizeof(uart[1].u8_transmitbuff));
	Ql_memset(cBuff, 0x0, sizeof(cBuff));
	Ql_memset(cBuff1, 0x0, sizeof(cBuff1));

	//Ql_strcpy(cBuff, (char*) STR_STX);     //start of sring
	Ql_strcpy(cBuff, (char*) "$");
	Ql_strcat(cBuff, (char*) ",");

	Ql_strcat(cBuff, (char*) DATA_OUT);
	//Ql_strcat(cBuff, (char*) "2");
	Ql_strcat(cBuff, (char*) ",");

#ifdef COLD_ROOM_ENABLE
	Ql_strcat(cBuff, (char*) "01");  //CR
#else
			Ql_strcat(cBuff, (char*) "02");  //TR
#endif
	Ql_strcat(cBuff, (char*) ",");

	Ql_strcat(cBuff, (char*) DATA_TYPE);
	Ql_strcat(cBuff, (char*) ",");

	Ql_strcat(cBuff, (char*) MCTXCNT);
	//Ql_strcat(cBuff, (char*) "2");
	Ql_strcat(cBuff, (char*) ",");

	Ql_strcat(cBuff, (char*) "3700");  // internal battery cut off voltage
	Ql_strcat(cBuff, (char*) ",");

	Ql_strcat(cBuff, (char*) "300");   //watchdog reset time
	Ql_strcat(cBuff, (char*) ",");

	u8_temp = Cal_Str_checksum(cBuff);
	Ql_strcat((char*) cBuff, "*");

	Ql_sprintf((char*) cBuff1, "%d", u8_temp);
	Ql_strcat((char*) cBuff, cBuff1);

	Ql_strcat(cBuff, (char*) "\r\n");

	Ql_strcpy((char*) uart[1].u8_transmitbuff, (char*) cBuff);
	APP_DEBUG("\r\n");
	APP_DEBUG((char * ) uart[1].u8_transmitbuff);
}
/***************************************************************************/
/*  Function   :  Cal_Str_checksum                                         */
/*-------------------------------------------------------------------------*/
/*  Object     : Calculate checksum of string                              */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
u8 Cal_Str_checksum(const char * restrict u8_ptrdata) {

	u8 u8_ChkSum = 0;

	if (u8_ptrdata != NULL) {

		u8_ptrdata += 1;

		u8_ChkSum = *u8_ptrdata;

		u8_ptrdata += 1;

		u32 u32length = 0;

		u32 u32temp = 0;

		u32length = Ql_strlen((char*) u8_ptrdata);

		//	APP_DEBUG((char* )u8_ret1);

		while (u32temp < u32length) {
			u8_ChkSum ^= *(u8_ptrdata + u32temp); //XOR all characters in the string
			u32temp++;
		}

		return u8_ChkSum;
	}

	return FALSE;
}
/***************************************************************************/
/*  Function   :  Comp_Str_checksum                                        */
/*-------------------------------------------------------------------------*/
/*  Object     : Compare checksum of received from string                  */
/*               and calculated checksum of received string                */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
u8 Comp_Str_checksum(const char * restrict u8_ptrdata) {

	char cBuff[100], cBuff1[100], cBuff2[5];

	u8 u8_temp = 0;

	char *Ret = NULL;

	if (u8_ptrdata != NULL) {

		Ql_memset(cBuff, 0x0, sizeof(cBuff));
		Ql_memset(cBuff1, 0x0, sizeof(cBuff1));
		Ql_memset(cBuff2, 0x0, sizeof(cBuff2));

		Ql_strncpy((char*) cBuff1, (char*) u8_ptrdata,
				(Ql_strlen(u8_ptrdata) - 7));

		u8_temp = Cal_Str_checksum(cBuff1); //calculate the checksum of received string
		Ql_sprintf((char*) cBuff2, "%d", u8_temp);

		Ql_strncpy((char*) cBuff, (char*) u8_ptrdata,
				(Ql_strlen(u8_ptrdata) - 4));
		Ret = Ql_strchr(cBuff, (s32) '*');
		Ret += 1;
		APP_DEBUG("/CalChksum:%s/StrChksum:%s", cBuff2, Ret);
		if (Ret != NULL) {
			if (u8_temp && *Ret) { //here compare the calculated checksum and received checksum
				APP_DEBUG("Checksum_Ok");
				return TRUE;
			} else {
				APP_DEBUG("Checksum_error\r\n");
				return FALSE;
			}
		}
	}
	return FALSE;
}
/***************************************************************************/
/*  Function   :  Internal_ProtocolParse                                   */
/*-------------------------------------------------------------------------*/
/*  Object     :  Parsing of micro controller string                       */
/*                Here diffenntiate whether protocol is normal tracer      */
/*                or cold room protocol                                    */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
u8 Internal_ProtocolParse(const char * u8_ptrdata) {

	u8 u8_ret = 0;

	//char CBuff[100];

	//Ql_strncpy((char*) CBuff, u8_ptrdata, Ql_strlen((char*) u8_ptrdata));

	unsigned char uc_temp[10];

	unsigned char uc_deviceID = 0;

	if (u8_ptrdata != NULL) {

		u8_ret = Comp_Str_checksum((char*) u8_ptrdata);

		if (u8_ret == TRUE) {

			Ql_memset(uc_temp, '\0', 10);

			if (*u8_ptrdata == '$') {
				u8_ptrdata++;

				if (*u8_ptrdata == ',') {
					u8_ptrdata++;

					if (*u8_ptrdata == '1') {
						u8_ptrdata++;

						if (*u8_ptrdata == ',') {

							u8_ptrdata++;

							Ql_strncpy((char*) uc_temp, (char*) u8_ptrdata, 3);

							uc_deviceID = Ql_atoi((char*) uc_temp);
							u8_ptrdata += 4;
							switch (uc_deviceID) {

							case 1: {

								u8_ret = CR_ProtocolParse((char*) u8_ptrdata); // cold room parsing

								if (u8_ret == TRUE) {

									Form_McAck();
									APP_DEBUG("CR_Parsing_OK\r\n");
									return TRUE;
								} else {
									APP_DEBUG("CR_Parsing_error\r\n");
								}

							}
								break;

							case 2: {
								u8_ret = TR_ProtocolParse((char*) u8_ptrdata); // normal tracer to micro controller protocol parsing
								if (u8_ret == TRUE) {
									Form_McAck();
									APP_DEBUG("TR_Parsing_OK\r\n");
									return TRUE;
								} else {
									APP_DEBUG("TR_Parsing_error\r\n");
								}
							}
								break;

							case 255:
								//after parsing of received data string from MC form acknowledgment for micro controller
								Form_McAck();
								return TRUE;
								break;

							default:
								return FALSE;
								break;
							}

						}

					}
				}

			}
		}
	}
	return FALSE;
}
/***************************************************************************/
/*  Function   :  CR_ProtocolParse                                         */
/*-------------------------------------------------------------------------*/
/*  Object     :  Parsing of cold room protocol                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
u8 CR_ProtocolParse(const char * restrict u8_ptrdata) {

	s_mc_info st_mc_data;

	u8 u8_mc_par[15], u8_strcnt = 0;

	char *token = NULL;

	APP_DEBUG((char* ) u8_ptrdata);

	if (u8_ptrdata != NULL) {

		for (token = strtok((char*) u8_ptrdata, ","); token != NULL; token =
				strtok(NULL, ","), u8_strcnt++) {

			Ql_memset(u8_mc_par, '\0', 15);

			Ql_strncpy((char*) u8_mc_par, token, Ql_strlen((char*) token));

			switch (u8_strcnt) {

			case 0:
				APP_DEBUG("Device_type\r\n:%s", u8_mc_par)
				;

				break;
			case 1:
				APP_DEBUG("NoOfbyteCnt\r\n:%s", u8_mc_par)
				;
				break;
			case 2:

				APP_DEBUG("Temp1\r\n:%s", u8_mc_par)
				;
				st_mc_data.u16_TempSens1 = Ql_atoi((char*) u8_mc_par);
				APP_DEBUG("Temp1111\r\n:%d", st_mc_data.u16_TempSens1)
				;
				break;
			case 3:

				APP_DEBUG("Temp2\r\n:%s", u8_mc_par)
				;
				st_mc_data.u16_TempSens2 = Ql_atoi((char*) u8_mc_par);
				APP_DEBUG("Temp2222\r\n:%d", st_mc_data.u16_TempSens2)
				;
				break;
			case 4:

				APP_DEBUG("Temp3\r\n:%s", u8_mc_par)
				;
				st_mc_data.u16_TempSens3 = Ql_atoi((char*) u8_mc_par);
				APP_DEBUG("Temp3333\r\n:%d", st_mc_data.u16_TempSens3)
				;
				break;
			case 5:

				APP_DEBUG("Press1\r\n:%s", u8_mc_par)
				;
				st_mc_data.u16_PresSens1 = Ql_atoi((char*) u8_mc_par);
				APP_DEBUG("Press1111\r\n:%d", st_mc_data.u16_PresSens1)
				;
				break;
			case 6:

				APP_DEBUG("Press2\r\n:%s", u8_mc_par)
				;
				st_mc_data.u16_PresSens2 = Ql_atoi((char*) u8_mc_par);
				APP_DEBUG("Press2222\r\n:%d", st_mc_data.u16_PresSens2)
				;
				break;
			case 7:

				APP_DEBUG("Int_Batt\r\n:%s", u8_mc_par)
				;
				st_mc_data.u16_intBattPow = Ql_atoi((char*) u8_mc_par);
				APP_DEBUG("Int_Batt\r\n:%d\r\n", st_mc_data.u16_intBattPow)
				;
				return TRUE;
				st_mc_info = st_mc_data;
				break;

			default:
				return FALSE;
				break;

			} ///switch

		} ///////for loop

	}

	return FALSE;
}

/***************************************************************************/
/*  Function   :  TR_ProtocolParse                                         */
/*-------------------------------------------------------------------------*/
/*  Object     :  Parsing of Normal Tracer protocol                        */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
u8 TR_ProtocolParse(const char * restrict u8_ptrdata) {
	s_mc_info st_mc_data;
	u8 u8_mc_par[15], u8_strcnt = 0;
	char *token = NULL;
	APP_DEBUG((char* ) u8_ptrdata);

	for (token = strtok((char*) u8_ptrdata, ","); token != NULL;
			token = strtok(NULL, ","), u8_strcnt++) {

		Ql_memset(u8_mc_par, '\0', 15);

		Ql_strncpy((char*) u8_mc_par, token, Ql_strlen((char*) token));

		switch (u8_strcnt) {

		case 0:
			APP_DEBUG("Device_type\r\n:%s", u8_mc_par)
			;

			break;
		case 1:
			APP_DEBUG("NoOfbyteCnt\r\n:%s", u8_mc_par)
			;
			break;
		case 2:

			APP_DEBUG("Temp1\r\n:%s", u8_mc_par)
			;
			st_mc_data.u16_TempSens1 = Ql_atoi((char*) u8_mc_par);
			APP_DEBUG("Temp1111\r\n:%d", st_mc_data.u16_TempSens1)
			;
			break;

		case 3:

			APP_DEBUG("Int_Batt\r\n:%s\r\n", u8_mc_par)
			;
			st_mc_data.u16_intBattPow = Ql_atoi((char*) u8_mc_par);
			APP_DEBUG("Int_Batt\r\n:%d", st_mc_data.u16_intBattPow)
			;
			break;
		case 4:
			Ql_memset(st_mc_data.u8_Ibutt, '\0', sizeof(st_mc_data.u8_Ibutt));
			APP_DEBUG("IBUTT\r\n:%s", u8_mc_par)
			;
			st_mc_data.u8_Ibutt[0] = u8_mc_par[0];
			st_mc_data.u8_Ibutt[1] = u8_mc_par[1];
			st_mc_data.u8_Ibutt[2] = u8_mc_par[2];
			st_mc_data.u8_Ibutt[3] = u8_mc_par[3];
			st_mc_data.u8_Ibutt[4] = u8_mc_par[4];
			st_mc_data.u8_Ibutt[5] = u8_mc_par[5];
			st_mc_data.u8_Ibutt[6] = u8_mc_par[6];
			st_mc_data.u8_Ibutt[7] = u8_mc_par[7];
			//st_mc_data.u8_Ibutt[8] = '\0';
			APP_DEBUG("IBUTT\r\n:%s", st_mc_info.u8_Ibutt)
			;
			return TRUE;
			st_mc_info = st_mc_data;
			break;

			break;
		default:
			return FALSE;
			break;

		} ///switch

	} ///////for loop

	return FALSE;
}
/***************************************************************************/
/*  Function   :  TASK1_Timer_handler                                         */
/*-------------------------------------------------------------------------*/
/*  Object     :  Parsing of Normal Tracer protocol                        */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static void TASK1_Timer_handler(u32 timerId, void* param) {
	if (timerId == TASK3_TIMER) {
		//	if (st_task1_cnt.u8Gps_fix_delay > 0) {
		//		st_task1_cnt.u8Gps_fix_delay--;
		//	}

		///*
		if (st_globflags.bGps_afterfixtmrloaded == TRUE) {
			st_task1_cnt.u16GPS_afterFix_Tmr++;
		}
		// */

	}
}
///////////////////////////////////////////

