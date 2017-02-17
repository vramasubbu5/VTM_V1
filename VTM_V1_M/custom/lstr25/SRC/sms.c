/*
 * sms.c
 *
 *  Created on: May 31, 2016
 *      Author: Amruta
 */
#ifdef __CUSTOMER_CODE__
#include "gpio.h"
#include "ril_sms.h"
#include "ql_memory.h"
#include "ql_timer.h"
#include "sms.h"
//#ifdef NOTUSED
/***********************************************************************
 * MACRO CONSTANT DEFINITIONS
 ************************************************************************/

#define CON_SMS_BUF_MAX_CNT   (1)
#define CON_SMS_SEG_MAX_CHAR  (160)
#define CON_SMS_SEG_MAX_BYTE  (4 * CON_SMS_SEG_MAX_CHAR)
#define CON_SMS_MAX_SEG       (7)

/***********************************************************************
 * STRUCT TYPE DEFINITIONS
 ************************************************************************/
typedef struct {
	u8 aData[CON_SMS_SEG_MAX_BYTE];
	u16 uLen;
} ConSMSSegStruct;

typedef struct {
	u16 uMsgRef;
	u8 uMsgTot;

	ConSMSSegStruct asSeg[CON_SMS_MAX_SEG];
	bool abSegValid[CON_SMS_MAX_SEG];
} ConSMSStruct;

///////////////////////////////////////////////////////////////////////////////////////
ConSMSStruct g_asConSMSBuf[CON_SMS_BUF_MAX_CNT];
///////////////////////////////////////////////////////////////////////////////////////
bool vts_sms_Check_SMS_validity(u8 *cData);
bool vts_sms_Check_SMS_Command(u8 cSms_Status, u8 *cData);
unsigned char vts_sms_Cntrl_getCommand(char * cCCommandString);
unsigned char vts_sms_Cntrl_processRequest(char * cUserRequest,
		unsigned char *cGotData);
//////////////////////////////////////////////////////////////////////////////////////////
void proc_main_task5(s32 taskId) {
	ST_MSG msg5;
	while (TRUE) {

		Ql_OS_GetMessage(&msg5);
		switch (msg5.message) {
		case MSG_ID_USER_DATA:
			APP_DEBUG(" Task5User_Data_sms\r\n")
			;
			if (sms_var.bSMS_Received == TRUE) {
				vts_sms_ProcessReceivedSMS();
			}
			break;
		default:
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
/***************************************************************************/
/*  Function   : vts_sms_Check_SMS_validity                                */
/*-------------------------------------------------------------------------*/
/*  Object     : To Check whether the SMS is Valid or Not                  */
/* 				                     					                   */
/* 			     													       */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  *cData            |   |   |   | *cData                                 */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/

bool vts_sms_Check_SMS_validity(u8 *cData) {
	if (Ql_strncmp((char *) (cData), (char *) "LEMS:", 5) == 0) {
		APP_DEBUG("\r\n Got Command Message \r\n");
		sms_var.cSMS_Status = VTS_SMS_VALID_CMD;
		return TRUE;
	} else {
		if (Ql_strncmp((char *) (cData), (char *) "***", 3) == 0) {
			APP_DEBUG("\r\n Got Control Message \r\n");
			sms_var.cSMS_Status = VTS_SMS_VALID_CNTRL;
			return TRUE;
		}
	}
	sms_var.cSMS_Status = VTS_SMS_INVALID;
	return FALSE;

}
/***************************************************************************/
/*  Function   : vts_sms_ProcessReceivedSMS                                */
/*-------------------------------------------------------------------------*/
/*  Object     : To process the received SMS    				           */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
void vts_sms_ProcessReceivedSMS(void) {
	///*
	// vts_logic_Stop_MainTask(); //For Stop polling timer, while config SMS processing.
	// Ql_strcpy((char *) sms_var.cSMS_BakMobileNumber, sms_var.cSMS_Tel);

	APP_DEBUG("Got SMS From: = %s\r\n", sms_var.cSMS_BakMobileNumber);
	APP_DEBUG("\r\nSMS -->= %s\r\n", sms_var.cSMS_Text);

	vts_sms_Check_SMS_validity((u8 *) sms_var.cSMS_Text);
	vts_sms_Check_SMS_Command(sms_var.cSMS_Status, (u8 *) sms_var.cSMS_Text);
	sms_var.bSMS_Received = FALSE;
	// vts_logic_Start_MainTask();    //need to enable
	// */
}
/***************************************************************************/
/*  All Functions                                                          */
/***************************************************************************/

/***************************************************************************/
/*  Function   : vts_sms_InitSMSService                                    */
/*-------------------------------------------------------------------------*/
/*  Object     : To Initialise the SMS Services 				           */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
void vts_sms_InitSMSService(void) {
	s32 iResult = 0;

	ST_RIL_SMS_TextInfo *pTextInfo = NULL;
	ST_RIL_SMS_DeliverParam *pDeliverTextInfo = NULL;

	pTextInfo = Ql_MEM_Alloc(sizeof(ST_RIL_SMS_TextInfo));
	if (NULL == pTextInfo) {
		APP_DEBUG("%s/%d:Ql_MEM_Alloc FAIL! size:%u\r\n",
				sizeof(ST_RIL_SMS_TextInfo), __func__, __LINE__);
		return;
	}
	Ql_memset(pTextInfo, 0x00, sizeof(ST_RIL_SMS_TextInfo));
	iResult = RIL_SMS_ReadSMS_Text(VTS_SMS_INDEX, LIB_SMS_CHARSET_GSM,
			pTextInfo);
	if (iResult != RIL_AT_SUCCESS) {
		Ql_MEM_Free(pTextInfo);
		APP_DEBUG("Fail to read text SMS[%d], cause:%d\r\n", VTS_SMS_INDEX,
				iResult);
		return;
	}

	if ((LIB_SMS_PDU_TYPE_DELIVER != (pTextInfo->type))
			|| (RIL_SMS_STATUS_TYPE_INVALID == (pTextInfo->status))) {
		Ql_MEM_Free(pTextInfo);
		APP_DEBUG("WARNING: NOT a new received SMS.\r\n");
		return;
	}

	pDeliverTextInfo = &((pTextInfo->param).deliverParam);
	/////////////////////////////////
	APP_DEBUG(
			"<-- RIL_SMS_ReadSMS_Text OK. eCharSet:LIB_SMS_CHARSET_GSM,nIndex:%u -->\r\n",
			VTS_SMS_INDEX);
	APP_DEBUG(
			"status:%u,type:%u,alpha:%u,sca:%s,oa:%s,scts:%s,data length:%u\r\n",
			pTextInfo->status, pTextInfo->type, pDeliverTextInfo->alpha,
			pTextInfo->sca, pDeliverTextInfo->oa, pDeliverTextInfo->scts,
			pDeliverTextInfo->length);
	APP_DEBUG("SMS_DATA = %s\r\n", (pDeliverTextInfo->data));
	Ql_strcpy(sms_var.cSMS_Text, (const char*) pDeliverTextInfo->data);
	APP_DEBUG("TEXT_SMS_DATA = %s\r\n", sms_var.cSMS_Text);
	Ql_strcpy((char*) sms_var.cSMS_BakMobileNumber, pDeliverTextInfo->oa);
	APP_DEBUG("BAKMOBILE_NO = %s\r\n", sms_var.cSMS_BakMobileNumber);
	Ql_MEM_Free(pTextInfo);

///////////////////////////////////////////////////////////////////
	iResult = RIL_SMS_DeleteSMS(0, RIL_SMS_DEL_READ_SENT_UNSENT_MSG);
	if (iResult != RIL_AT_SUCCESS) {
		APP_DEBUG("Fail to delete all messages, cause:%d\r\n", iResult);

	}
	APP_DEBUG("Delete all existed messages\r\n");
	//vts_sms_DeleteSMSmessage();
	sms_var.bSMS_Received = TRUE;
}
/***************************************************************************/
/*  Function   : vts_sms_Check_SMS_Command                                 */
/*-------------------------------------------------------------------------*/
/*  Object     : To Check and Process the SMS Command                      */
/* 				                     					                   */
/* 			     													       */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  cSms_Status       |   |   |   | cSms_Status                            */
/*--------------------+---+---+---+----------------------------------------*/
/*  *cData            |   |   |   | *cData                                 */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
bool vts_sms_Check_SMS_Command(u8 cSms_Status, u8 *cData) {
	char cStatus = VTS_DATA_READY;   //added for testing
	char cBSSMSNo[] = "+918976346403\0";

	switch (cSms_Status) {
	case VTS_SMS_VALID_CMD:

		//cStatus = vts_data_CfgprocessReply(VTS_DATA_THROUGH_SMS, (char *) cData,
		//		(char *) sms_var.cSMS_SndBuff);

		//Ql_strcpy(cfg_par.cBSSMSNo,"+918080560256") ;
		if (cStatus == VTS_DATA_READY) {
			Ql_strcpy((char *) sms_var.cSMS_SndBuff, (char *) "LEMS:OK"); // added for testing
			APP_DEBUG("\r\nSMS RDY--->%s", sms_var.cSMS_SndBuff);

			// If sending fails then handle this case to resend it again
			//vts_sms_Send_MSG(cfg_par.cBSSMSNo,sms_var.cSMS_SndBuff);
			vts_sms_Send_MSG((char*) cBSSMSNo, (u8*) sms_var.cSMS_SndBuff);
		}
		vts_sms_DeleteSMSmessage();
		break;
	case VTS_SMS_VALID_CNTRL:
		APP_DEBUG("Actual SMS=%s\r\n", cData)
		;
		cStatus = vts_sms_Cntrl_processRequest((char *) cData, //need to enable
				(u8 *) sms_var.cSMS_SndBuff);
		// vts_utils_SerialStrPrint((u8 *)sms_var.cSMS_SndBuff);
		// Acknowledge is sent immediately only to command SMS therefore command SMS No is used
		if (cStatus == VTS_DATA_READY) {
			//	vts_utils_SerialStrPrint((u8 *)" .... Reply_Snd...\r\n");
			// If sending fails then handle this case to resend it again
			APP_DEBUG("\r\nSMS RDY--->%s", sms_var.cSMS_SndBuff);
			vts_sms_Send_MSG((char*) sms_var.cSMS_BakMobileNumber,
					(u8*) sms_var.cSMS_SndBuff);
		}
		break;
	case VTS_SMS_INVALID:
		APP_DEBUG("Invalid Command Message... \r\n")
		;
		// Invalid SMS indicates a message which is not in the specific format
		vts_sms_DeleteSMSmessage();
		break;
	}
	return TRUE;
}
/***************************************************************************/
/*  Function   : vts_sms_Send_MSG                                 */
/*-------------------------------------------------------------------------*/
/*  Object     : To send the SMS Command                      */
/* 				                     					                   */
/* 			     													       */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  cSms_Status       |   |   |   | cSms_Status                            */
/*--------------------+---+---+---+----------------------------------------*/
/*  *cData            |   |   |   | *cData                                 */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
void vts_sms_Send_MSG(char* cPhNum, u8 *cSmsData) {
	s32 iResult = 0;
	u32 uMsgRef = 0;

	APP_DEBUG("<-- Replying NORMAL SMS... -->\r\n");
	APP_DEBUG("MOBILE_NO = %s\r\n", cPhNum);
	APP_DEBUG("REPLY_DATA = %s\r\n", cSmsData);
	iResult = RIL_SMS_SendSMS_Text(cPhNum, Ql_strlen(cPhNum),
			LIB_SMS_CHARSET_GSM, (u8*) cSmsData,

			Ql_strlen((const char*) cSmsData), &uMsgRef);

	if (iResult != RIL_AT_SUCCESS) {
		APP_DEBUG("RIL_SMS_SendSMS_NORMAL Text FAIL! iResult:%u\r\n", iResult);
		return;
	}
	APP_DEBUG("<-- RIL_SMS_SendNORMALTextSMS OK. uMsgRef:%d -->\r\n", uMsgRef);
}
/***************************************************************************/
/*  Function   : vts_sms_DeleteSMSmessage                                 */
/*-------------------------------------------------------------------------*/
/*  Object     : To delete the SMS Command                      */
/* 				                     					                   */
/* 			     													       */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  cSms_Status       |   |   |   | cSms_Status                            */
/*--------------------+---+---+---+----------------------------------------*/
/*  *cData            |   |   |   | *cData                                 */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
void vts_sms_DeleteSMSmessage(void) {
	s32 iResult = 0;

	iResult = RIL_SMS_DeleteSMS(1, RIL_SMS_DEL_READ_SENT_UNSENT_MSG);
	if (iResult != RIL_AT_SUCCESS) {
		APP_DEBUG("Fail to delete all messages, cause:%d\r\n", iResult);

	}
	APP_DEBUG("Delete all existed messages\r\n");
}
/***************************************************************************/
/*  Function   : vts_sms_Cntrl_processRequest                              */
/*-------------------------------------------------------------------------*/
/*  Object     : To Process the control SMS Command                        */
/* 				                     					                   */
/* 			     													       */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  cUserRequest      |   |   |   | cUserRequest                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  *cGotData         |   |   |   | *cGotData                              */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
unsigned char vts_sms_Cntrl_processRequest(char * cUserRequest,
		unsigned char *cGotData) {
	// The command format as folloes
	//<userid>,<passwd>,<command>,<data>,
	// userid - u or a
	// password - can be any 4 digit alpha numeric char in ascii
	// command - 4 digit command
	// data    - optional
	unsigned char cMessageLen, c1, cCount;
	unsigned char cSMSCommand;
	char ctempBuff[7];
	char cPassWord[5];
	char cCheckPar;
	char ctempBuff1[20];
	cCount = 0;
	cCheckPar = CHECK_USERNAME;
	cMessageLen = Ql_strlen(cUserRequest);
	Ql_memset(ctempBuff, '\0', sizeof(ctempBuff));
	for (c1 = 0; c1 < cMessageLen; c1++) {
		ctempBuff[cCount++] = cUserRequest[c1];
		if ((ctempBuff[cCount - 1]) == ',') {
			ctempBuff[cCount - 1] = '\0';
			switch (cCheckPar) {
			case CHECK_USERNAME:
				Ql_strcpy(ctempBuff1, "***U");
				if (Ql_strncmp(ctempBuff1, ctempBuff, 4) == 0) {
					Ql_strcpy((char *) cPassWord, (char *) "1234");
					//	APP_DEBUG("Check User\r\n");
					cCheckPar = CHECK_PASSWORD;
					cCount = 0;
					break;
				}
				Ql_strcpy(ctempBuff1, "***A");
				if (Ql_strncmp(ctempBuff1, ctempBuff, 4) == 0) {
					Ql_strcpy((char *) cPassWord, (char *) "ABCD");
					cCheckPar = CHECK_PASSWORD;
					cCount = 0;
					break;
				} else {
					APP_DEBUG("Invalid User\r\n");
					// Set the error code
					cCount = 0;
					return FAILURE;
				}
				break;
			case CHECK_PASSWORD:
				if (Ql_strncmp((char *) cPassWord, (char *) ctempBuff, 4)
						== 0) {
					//APP_DEBUG("Check Password\r\n");
					cCheckPar = CHECK_COMMAND;
					cCount = 0;
					break;
				} else {
					APP_DEBUG("Invalid Password\r\n");
					// Set the error code
					cCount = 0;
					return FAILURE;
				}
				break;
///////////////////// COMMAND CASE ////////////////////////////////////////////////////////////////////////////////////
			case CHECK_COMMAND:
				cSMSCommand = vts_sms_Cntrl_getCommand(ctempBuff);
				switch (cSMSCommand) {
				case SMSCMB_IGON:       // Ignition ON command
					break;
				case SMSCMB_IGOF:       // Ignition OFF command
					break;
				case SMSCMB_GLOC:
					break;
				case SMSCMB_SLOC:
					break;
				case SMSCMB_GLLL:
					break;
				case SMSCMB_CPWD:
					break;
				case SMSCMB_DSBL:       // Disable Data Transfer
					break;
				case SMSCMB_ENBL:       // Enable Data Transfer
					break;
				case SMSCMB_RST:
					//APP_DEBUG("Check REPLY\r\n");
					Ql_strcpy((char *) sms_var.cSMS_SndBuff,
							(char *) "LEMS:OK");       // added for testing
					//APP_DEBUG("Reset GPS------>%s\r\n",sms_var.cSMS_SndBuff);
					/*
					 vts_gpio_Wireless_Rst_Enable();
					 vts_data_CheckRAM1_WR();
					 vts_logic_Stop_MainTask();
					 vts_logic_SaveRecordFlashPointer(0);
					 vts_flash_CyclicDistanceWR(&distval,
					 VTS_FLH_SIZE_DISTVAL_CYCLIC, VTS_IMEDIATE_WR);
					 APP_DEBUG( "System Reset command\r\n");
					 vts_flash_CyclicRecPointerWR(SIZE_OF_CYCLIC_POINTER);
					 vts_logic_GSMReset();
					 */
					//return SUCCESS;    //disable for testing
					return VTS_DATA_READY;
					break;
				case SMSCMB_EXAT:
					// will be implemented later
					break;
				case SMSCMB_PRST:
					APP_DEBUG("Reset GPS\r\n")
					;
					Ql_strcpy((char *) sms_var.cSMS_SndBuff,
							(char *) "LEMS:OK");
					// Power OFF and Power On GPS
					//	vts_gps_Reset();
					// will be implemented later
					return SUCCESS;
					break;

				case SMSCMB_GREC:
					APP_DEBUG("Get a Record via SMS\r\n")
					;
					Ql_strcpy((char *) sms_var.cSMS_SndBuff,
							(char *) "LEMS:OK");
					/*
					 vts_data_FormARecordEx(cGotData);
					 vts_data_FormRecordFooter(VTS_DATA_THROUGH_SMS, cGotData);
					 // will be implemented later

					 */
					return VTS_DATA_READY;
					break;
				case SMSCMB_ERST:
					APP_DEBUG("Reset EEPROM Pointers\r\n")
					;
					/*
					 vts_flash_ClearEEPROMPointers();
					 //Erase the incident record pointers
					 vts_flash_ClearIncidentPtr();
					 vts_logic_SaveRecordFlashPointer(0);
					 */
#ifdef AD_FLASH_ENABLED
					vts_AnD_Format();
#endif
					return SUCCESS;
					break;
				case SMSCMB_FRST:
					APP_DEBUG("Reset Fuel Counters\r\n")
					;
					return SUCCESS;
					break;
				case SMSCMB_DOTA:
					Ql_OS_SendMessage(6, MSG_ID_USER_DATA, 1, 1);

					/*
					 dota_info.bDownloadProgress = TRUE;
					 dota_info.bDotaAndProcess = TRUE;
					 vts_data_CheckRAM1_WR();
					 vts_flash_CyclicDistanceWR(&distval,
					 VTS_FLH_SIZE_DISTVAL_CYCLIC, VTS_IMEDIATE_WR);
					 vts_logic_SaveRecordFlashPointer(0); //Here Write the Record pointer
					 #ifndef NEW_DOTA_LOGIC
					 DOTA_AnDInit();
					 #endif
					 */
					return SUCCESS;
					break;
				case SMSCMB_STWD:
					//vts_wdg_StopWatchdogPulse();
					break;
				case SMSCMB_NOCMB:
				default:
					APP_DEBUG("Invalid Command\r\n")
					;
					return SUCCESS;
					break;
				} //switch of command
				break; //CHECK_COMMAND
			case CHECK_DATA:
				break;
			} // switch of check par
		}
	}
	return 0;
}
/***************************************************************************/
/*  Function   : vts_sms_Cntrl_getCommand                                  */
/*-------------------------------------------------------------------------*/
/*  Object     : To check and get the control command string               */
/* 				                     					                   */
/* 			     													       */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  cCondition        |   |   |   | cCondition                             */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/

unsigned char vts_sms_Cntrl_getCommand(char * cCCommandString) {
	char ctempBuff[6];
	Ql_strcpy((char *) ctempBuff, (char *) "IGON");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_IGON;

	Ql_strcpy((char *) ctempBuff, (char *) "IGOF");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_IGOF;

	Ql_strcpy((char *) ctempBuff, (char *) "GFLV");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_GFLV;

	Ql_strcpy((char *) ctempBuff, (char *) "GLLL");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_GLLL;

	Ql_strcpy((char *) ctempBuff, (char *) "GLOC");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_GLOC;

	Ql_strcpy((char *) ctempBuff, (char *) "CPWD");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_CPWD;

	Ql_strcpy((char *) ctempBuff, (char *) "DSBL");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_DSBL;

	Ql_strcpy((char *) ctempBuff, (char *) "ENBL");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_ENBL;

	Ql_strcpy((char *) ctempBuff, (char *) "SRST");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0) {
		APP_DEBUG("Get Cmd\r\n");
		return SMSCMB_RST;
	}
	Ql_strcpy((char *) ctempBuff, (char *) "EXAT");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_EXAT;

	Ql_strcpy((char *) ctempBuff, (char *) "PRST");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_PRST;

	Ql_strcpy((char *) ctempBuff, (char *) "MRST");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_MRST;

	Ql_strcpy((char *) ctempBuff, (char *) "DIAG");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_DIAG;

	Ql_strcpy((char *) ctempBuff, (char *) "GREC");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_GREC;

	Ql_strcpy((char *) ctempBuff, (char *) "GRMV");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_GRMV;

	Ql_strcpy((char *) ctempBuff, (char *) "ERST");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_ERST;

	Ql_strcpy((char *) ctempBuff, (char *) "FRST");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_FRST;

	Ql_strcpy((char *) ctempBuff, (char *) "DOTA");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_DOTA;
	Ql_strcpy((char *) ctempBuff, (char *) "STWD");
	if (Ql_strncmp(cCCommandString, ctempBuff, 4) == 0)
		return SMSCMB_STWD;

	else
		return SMSCMB_NOCMB;
}
#endif
//#endif
