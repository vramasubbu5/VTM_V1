/*
 * sms.h
 *
 *  Created on: Jun 1, 2016
 *      Author: Amruta
 */

#ifndef SMS_H_
#define SMS_H_



/***************************************************************************/
/*  Global Macro Defines for SMS				                           */
/***************************************************************************/
#define VTS_SMS_TEL_BUF_SIZE			15
#define VTS_SMS_LENGTH_BUF_SIZE			15
#define VTS_SMS_TEXT_BUF_SIZE			250

#define VTS_SMS_VALID_CMD		11
#define VTS_SMS_VALID_CNTRL 	12
#define VTS_SMS_INVALID			13

#define VTS_SMS_DATA_READY		14



/****************************************************************************************************
 SMS command state machine commands
 ****************************************************************************************************/
#define CHECK_USERNAME				75
#define CHECK_PASSWORD				76
#define CHECK_COMMAND				77
#define CHECK_DATA					78

/****************************************************************************************************
 SMS command
 ****************************************************************************************************/
#define SMSCMB_IGOF					79
#define SMSCMB_IGON					80
#define SMSCMB_GLOC					81
#define SMSCMB_SLOC					82
#define SMSCMB_GLLL					83
#define SMSCMB_CPWD					84
#define SMSCMB_NOCMB				85
#define SMSCMB_DSBL					86
#define SMSCMB_ENBL					87
#define SMSCMB_RST					88
#define SMSCMB_GFLV					89
#define SMSCMB_PRST					90
#define SMSCMB_MRST					91
#define SMSCMB_DIAG					92
#define SMSCMB_GREC					93
#define SMSCMB_EXAT					94
#define SMSCMB_GRMV					95
#define SMSCMB_ERST					96
#define SMSCMB_FRST					97
#define SMSCMB_RFS1					98
#define SMSCMB_RFS2					99
#define SMSCMB_RFS3					100
#define SMSCMB_RFS4					101
#define SMSCMB_DOTA                 102
#define SMSCMB_STWD                 103




#define VTS_SMS_INDEX                1


#define VTS_DATA_READY						11
#define VTS_GPRS_NO_DATA					12
#define VTS_SMS_REPLY_IN_RECORD				13

#define SUCCESS 		1
#define FAILURE 		0
#define SMS_TEXT_DATA_MAX_LEN               (4 * (160))
/***************************************************************************/
/*  Global Structure for SMS				                               */
/***************************************************************************/

typedef struct SMS_VAR {
	volatile bool bSMS_Received; // SMS Received identification flag.
	char cSMS_Tel[VTS_SMS_TEL_BUF_SIZE];
	char cSMS_Length[VTS_SMS_LENGTH_BUF_SIZE];
	char cSMS_Text[SMS_TEXT_DATA_MAX_LEN];
	char cSMS_SndBuff[VTS_SMS_TEXT_BUF_SIZE];
	u8 cSMS_Status;
	u8 cSMS_BakMobileNumber[VTS_SMS_TEL_BUF_SIZE + 2];

	/*bool bBootSMSRead;
	bool bBootSMSEnabled;
	bool bBootSMSProgress;
	unsigned char cNoOfMessages;
	unsigned char cTotalNoOfMessages;
	unsigned char cReadMessageCount;
*/
} SMS_VAR;
 SMS_VAR sms_var;

/***************************************************************************/
/*  Functions Prototypes for SMS				                           */
/***************************************************************************/
void vts_sms_DotaStatusSMS(char cCondition) ;
void vts_sms_ProcessReceivedSMS(void) ;
void vts_sms_InitSMSService(void) ;
void vts_sms_Send_MSG(char* cPhNum , u8 *cSmsData);
void vts_sms_DeleteSMSmessage(void);

#endif /* SMS_H_ */
