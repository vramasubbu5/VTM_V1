/*
 * task4.c
 *
 *  Created on: Jul 12, 2016
 *      Author: Amruta
 */
/***************************************************************************/
/* This task consist of FOTA Process.It is control by sms.                 */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
#include "gpio.h"
#include "system.h"
#include "fota_main.h"
#include "task4.h"
#include "task2.h"
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
//#ifdef NOTUSED
void Fota_Process(void);

/*
 *
 * ************** Object declaration ****************
 *
 */

extern TASK_EVENTS m_task_evnt;
//extern u8 u8apnvector[25];
//extern u8 u8url[50];
extern s_gprs_config st_gprsconfig;

u8 u8servportno[10];
u8 u8servfilepath[20];
u8 u8servusername[25];
u8 u8servpassword[20];
u8 u8servfilename[25];

s_fota_config st_fota_config = { u8servportno, u8servfilepath, u8servusername,
		u8servpassword, u8servfilename };

/*
 *
 * ************** Main task starts ***************
 *
 */

void proc_main_task4(s32 taskId) {

	ST_MSG msg4;

	//Ql_Debug_Trace("\r\n task_start:%d", taskId);   //disable by amruta for testing
	m_task_evnt.TaskEvent4 = Ql_OS_CreateEvent("\r\n task4");

	while (Ql_OS_WaitEvent(m_task_evnt.TaskEvent4, EVENT_FLAG0) != 0) {

	}
	Ql_strcpy((char*) u8servportno, "21");
	Ql_strcpy((char*) u8servfilepath, "./quect_test/");
	Ql_strcpy((char*) u8servusername, "ram@lateralsoft.com");
	Ql_strcpy((char*) u8servpassword, "lems123");
	Ql_strcpy((char*) u8servfilename, "APPGS3MDM32A01.bin");
	Ql_OS_SetEvent(m_task_evnt.TaskEvent1, EVENT_FLAG0);
	while (TRUE) {

		Ql_OS_GetMessage(&msg4);
		switch (msg4.message) {
		case MSG_ID_USER_DATA:
			APP_DEBUG(" Task4User_Data\r\n")
			;

			Fota_Process();

			break;
		case MSG_ID_FTP_RESULT_IND:
			APP_DEBUG("\r\n<##### Restart FTP 3s later #####>\r\n")
			;
			Ql_Sleep(3000);
			Fota_Process();
			break;
		case MSG_ID_RESET_MODULE_REQ:
			APP_DEBUG("\r\n<##### Restart the module... #####>\r\n")
			;

			Ql_Sleep(50);
			Ql_Reset(0);
			break;
		default:
			APP_DEBUG("Default: Task4\r\n")
			;
			break;
		}

	}
}
/////////////////////////////////////////////////////////////////////////////////////////////
void Fota_Process(void) {

	ST_GprsConfig apnCfg;
	s32 strLen;
	static u8 m_URL_Buffer[500];

	Ql_memcpy(apnCfg.apnName, st_gprsconfig.u8apnvector,
			Ql_strlen((char*) st_gprsconfig.u8apnvector));
	Ql_memset(m_gprsCfg.apnUserId, 0x0, sizeof(m_gprsCfg.apnUserId));
	Ql_memset(m_gprsCfg.apnPasswd, 0x0, sizeof(m_gprsCfg.apnPasswd));

	if ((st_fota_config.u8portNo != NULL) && (st_fota_config.u8filepath != NULL)
			&& (st_fota_config.u8username != NULL)
			&& (st_fota_config.u8password != NULL)
			&& (st_fota_config.u8filename != NULL))

			{

		strLen = Ql_sprintf((char*) m_URL_Buffer, "ftp://%s%s%s:%s@%s:%s",
				st_gprsconfig.u8url, st_fota_config.u8filepath,
				st_fota_config.u8filename, st_fota_config.u8portNo,
				st_fota_config.u8username, st_fota_config.u8password);

		Ql_UART_Write(UART_PORT1, m_URL_Buffer, strLen);

		Ql_FOTA_StartUpgrade(m_URL_Buffer, &apnCfg, NULL);

	}

}

//#endif
