/*
 * rtc.c
 *
 *  Created on: Aug 25, 2016
 *      Author: Amruta
 */

#include "rtc.h"
#include "gpio.h"
#include "utils.h"

extern s_rtc_cclk st_rtc_cclk;
extern s32 Default_atRsp_callback(char* line, u32 len, void* userdata);

s32 AT_Response_Handler(char* line, u32 len);

u8 SendATCommand(char* AtCmd);
//s32 ATResponse_QRTC_Handler(char* line, u32 len, void* userData);
//////////////////////////////////////////////////////////////////////////
s32 AT_Response_Handler(char* line, u32 len) {
	s32 ret = 0;
	ret = Default_atRsp_callback(line, len, 0);
	if (ret == RIL_ATRSP_SUCCESS) {
		APP_DEBUG("AT_OK  -->\r\n");
		return TRUE;
	} else if (ret == RIL_ATRSP_FAILED) {
		APP_DEBUG("+CME ERROR:\r\n");
		return FALSE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
u8 SendATCommand(char* AtCmd) {
	s32 ret = 0;
	if (AtCmd != NULL) {
		ret = Ql_RIL_SendATCmd(AtCmd, Ql_strlen((char*) AtCmd),
		NULL, NULL, 0);
		if (ret == RIL_AT_SUCCESS) {
			APP_DEBUG("AT_cmd_sucess:\r\n");
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
void rtc_SetClock(s_rtc_cclk *rtc_cclk) {
// AT+CCLK="00/06/09,17:33:00" this is the example format.
	char cTemp_Buff[31];
	u8 u8ret = 0;
	Ql_memset((char*) cTemp_Buff, 0, sizeof((char*) cTemp_Buff));
	Ql_strcpy(cTemp_Buff, "AT+CCLK=");
	///*
	///////// date
	cTemp_Buff[8] = '"';
	//Year
	cTemp_Buff[9] = *((char *) rtc_cclk->cdate + 6);
	cTemp_Buff[10] = *((char *) rtc_cclk->cdate + 7);
	cTemp_Buff[11] = '/';
	//Month
	cTemp_Buff[12] = *((char *) rtc_cclk->cdate + 3);
	cTemp_Buff[13] = *((char *) rtc_cclk->cdate + 4);
	cTemp_Buff[14] = '/';
	//Day
	cTemp_Buff[15] = *((char *) rtc_cclk->cdate + 0);
	cTemp_Buff[16] = *((char *) rtc_cclk->cdate + 1);
	cTemp_Buff[17] = ',';
	///////////// time
	//hour
	cTemp_Buff[18] = *((char *) rtc_cclk->ctime + 0);
	cTemp_Buff[19] = *((char *) rtc_cclk->ctime + 1);
	cTemp_Buff[20] = ':';
	//Minutes
	cTemp_Buff[21] = *((char *) rtc_cclk->ctime + 3);
	cTemp_Buff[22] = *((char *) rtc_cclk->ctime + 4);
	cTemp_Buff[23] = ':';
	//Seconds
	cTemp_Buff[24] = *((char *) rtc_cclk->ctime + 6);
	cTemp_Buff[25] = *((char *) rtc_cclk->ctime + 7);
	cTemp_Buff[26] = '+';
	cTemp_Buff[27] = '0';
	cTemp_Buff[28] = '0';
	cTemp_Buff[29] = '"';
	cTemp_Buff[30] = '\0';
	// */
	/*
	 ///////// date
	 cTemp_Buff[8] = '"';
	 //Year
	 cTemp_Buff[9] = '1';
	 cTemp_Buff[10] = '6';
	 cTemp_Buff[11] = '/';
	 //Month
	 cTemp_Buff[12] = '0';
	 cTemp_Buff[13] = '8';
	 cTemp_Buff[14] = '/';
	 //Day
	 cTemp_Buff[15] = '2';
	 cTemp_Buff[16] = '6';
	 cTemp_Buff[17] = ',';
	 ///////////// time
	 //hour
	 cTemp_Buff[18] = '1';
	 cTemp_Buff[19] = '2';
	 cTemp_Buff[20] = ':';
	 //Minutes
	 cTemp_Buff[21] = '1';
	 cTemp_Buff[22] = '5';
	 cTemp_Buff[23] = ':';
	 //Seconds
	 cTemp_Buff[24] = '4';
	 cTemp_Buff[25] = '5';
	 cTemp_Buff[26] = '+';
	 cTemp_Buff[27] = '0';
	 cTemp_Buff[28] = '0';
	 cTemp_Buff[29] = '"';
	 cTemp_Buff[30] = '\0';
	 */
	u8ret = SendATCommand(cTemp_Buff);
	if (u8ret == TRUE) {
		st_rtc_cclk.bLoadInternalRtc = FALSE;
		APP_DEBUG("GSM Internal Clock Set Success\r\n");
	} else {
		APP_DEBUG("Internal Clock Set Error--->\r\n");
	}
}

///////////////////////////////////////////////////////////////////////////
s32 ATResponse_QRTC_Handler(char* line, u32 len, void* userData) {

//	s_rtc_cclk* pDT = (s_rtc_cclk*) userData;

//s_rtc_cclk* pDT = (s_rtc_cclk*) userData;

	s_rtc_cclk* pDT = &st_rtc_cclk;

	char* head = Ql_RIL_FindString(line, len, "+CCLK:"); //continue wait

	char Cbuff[35];
	Ql_memset(Cbuff, '\0', sizeof(Cbuff));

	//Ql_Debug_Trace("Line%d:%s, ", __LINE__, line);
	APP_DEBUG("RTC_Date_Time:\r\n");
	//APP_DEBUG(":%s\r\n", head);
	//APP_DEBUG(":%s\r\n", line);

	if (head != NULL) {

		line += 10;

		Ql_strcpy((char*) Cbuff, line);
		/*
		 APP_DEBUG("RTC_Date_Time_enter\r\n");
		 APP_DEBUG("%s\r\n", line);
		 APP_DEBUG("RTC_Date_Time_Buff\r\n");
		 APP_DEBUG((char* ) Cbuff);
		 */

		pDT->cdate[6] = Cbuff[0];
		pDT->cdate[7] = Cbuff[1];
		pDT->cdate[5] = Cbuff[2];
		pDT->cdate[3] = Cbuff[3];
		pDT->cdate[4] = Cbuff[4];
		pDT->cdate[2] = Cbuff[5];
		pDT->cdate[0] = Cbuff[6];
		pDT->cdate[1] = Cbuff[7];
		pDT->cdate[8] = '\0';

		pDT->ctime[0] = Cbuff[9];
		pDT->ctime[1] = Cbuff[10];
		pDT->ctime[2] = Cbuff[11];
		pDT->ctime[3] = Cbuff[12];
		pDT->ctime[4] = Cbuff[13];
		pDT->ctime[5] = Cbuff[14];
		pDT->ctime[6] = Cbuff[15];
		pDT->ctime[7] = Cbuff[16];
		pDT->ctime[8] = '\0';

		APP_DEBUG((char* ) pDT->cdate);
		APP_DEBUG("\r\n");
		APP_DEBUG((char* ) pDT->ctime)
		APP_DEBUG("\r\n");
		return RIL_ATRSP_SUCCESS;
	}

	head = Ql_RIL_FindLine(line, len, "OK"); // find <CR><LF>OK<CR><LF>, <CR>OK<CR>£¬<LF>OK<LF>
	if (head) {
		return RIL_ATRSP_SUCCESS;
	}

	head = Ql_RIL_FindLine(line, len, "ERROR"); // find <CR><LF>ERROR<CR><LF>, <CR>ERROR<CR>£¬<LF>ERROR<LF>
	if (head) {
		return RIL_ATRSP_FAILED;
	}

	return RIL_ATRSP_CONTINUE; //continue wait
}
