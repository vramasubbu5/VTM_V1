/*
 * utils.c
 *
 *  Created on: May 31, 2016
 *      Author: Amruta
 */

#include "gpio.h"
#include "system.h"
#include "utils.h"


#ifdef NOTUSED
/***************************************************************************/
/*  Global Macro Defines,Structure and Variables                           */
/***************************************************************************/
enum COUNTRY {
	INDIA = 1, DUBAI, OMAN
};
#define MODEL_IDENTIFIER_SMS				0
#define MODEL_IDENTIFIER_GPRS				1

extern s_system_info st_sys;

s_rec_hdr st_rec_hdr;


/***************************************************************************/
/*  Function Prototypes                                                    */
/***************************************************************************/

/***************************************************************************/
/*  All Functions                                                          */
/***************************************************************************/

/***************************************************************************/
/*  Function   : vts_utils_IntToBuff                                       */
/*-------------------------------------------------------------------------*/
/*  Object     : To Integers into Buffer String 	                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  IntVal            |   |   |   | IntVal                                 */
/*--------------------+---+---+---+----------------------------------------*/
/*  *cBuff            |   |   |   | *cBuff                                 */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
unsigned char vts_utils_IntToBuff(unsigned int IntVal, char *cBuff) {
	char cSize;
	*((char*) cBuff + 0) = 0x30;
	*((char*) cBuff + 1) = '\0';
	cSize = 1;
	if (IntVal >= 0) {
		*((char*) cBuff + 1) = '\0';
		*((char*) cBuff + 0) = (IntVal % 10) + 0x30;
		cSize = 1;
		if (IntVal > 9) {
			*((char*) cBuff + 2) = '\0';
			*((char*) cBuff + 1) = *((char*) cBuff + 0);
			*((char*) cBuff + 0) = ((IntVal / 10) % 10) + 0x30;
			cSize = 2;
			if (IntVal > 99) {
				*((char*) cBuff + 3) = '\0';
				*((char*) cBuff + 2) = *((char*) cBuff + 1);
				*((char*) cBuff + 1) = *((char*) cBuff + 0);
				*((char*) cBuff + 0) = ((IntVal / 100) % 10) + 0x30;
				cSize = 3;
				if (IntVal > 999) {
					*((char*) cBuff + 4) = '\0';
					*((char*) cBuff + 3) = *((char*) cBuff + 2);
					*((char*) cBuff + 2) = *((char*) cBuff + 1);
					*((char*) cBuff + 1) = *((char*) cBuff + 0);
					*((char*) cBuff + 0) = ((IntVal / 1000) % 10) + 0x30;
					cSize = 4;
					if (IntVal > 9999) {
						*((char*) cBuff + 5) = '\0';
						*((char*) cBuff + 4) = *((char*) cBuff + 3);
						*((char*) cBuff + 3) = *((char*) cBuff + 2);
						*((char*) cBuff + 2) = *((char*) cBuff + 1);
						*((char*) cBuff + 1) = *((char*) cBuff + 0);
						*((char*) cBuff + 0) = ((IntVal / 10000) % 10) + 0x30;
						cSize = 5;
					}
				}
			}
		}
	}
	return cSize;
}
/***************************************************************************/
/*  Function   : vts_utils_ReadSystemSerialNo                              */
/*-------------------------------------------------------------------------*/
/*  Object     : To Read and Process System Serial No                      */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
void vts_utils_ReadSystemSerialNo(void)
/*
 The system serial No is of the form CC M YY XXXX MD
 CC - Company or Client idenrifier Now it is LS
 M  - Indicates the Month. From A to L
 YY - Year Identifier
 XXXX - Serial No
 MD - Model Identifier (SM for SMS or GP for GPRS)

 The ID location in the uC is used to store the serial No.
 The Id location ranges between 0x200000 to 0x200007, 8 nibble is present
 Details that are stored in ID location

 0 - Model Identifier 0 for SMS , 1 for GPRS
 1 - Month Identifier 0 to 12 in Hexadesimal
 2 - MSB of year
 3 - LSB of year
 4,5,6,7 - Serial No

 For programming the Serial No LSB050001GP the ID to be used is
 LS is Hard coded in program

 B is for Feburary therefore put 2 in location 1
 05 put it in location 2,3
 0001 put it in location 4,5,6,7
 GP put 1 in location 0
 Therefore the ID to be programmed will be 12050001
 */
{

	unsigned char c1;
	unsigned char c2[2];
	unsigned long l1;
	char cReadId[15];

//	wm_strcpy(st_sys.u8_tuno,"15100001")  ;
//	LSb2?000GP

	Ql_memset(st_sys.u8_tuno, '\0', sizeof(st_sys.u8_tuno));
	Ql_memset(cReadId, '\0', sizeof(cReadId));
	//vts_flash_ReadTUNo(cReadId);          //disable for testing by amruta
	Ql_strcpy((char *) cReadId, (char *) "LSA143511");

	Ql_strcpy((char *) st_sys.u8_tuno, (char *) "LS");
	// cReadId[0] ignore  fro LS
	// Put Month
	c1 = cReadId[1] - 0x30;
	c2[0] = 64 + c1;
	c2[1] = '\0';
	l1 = c1;
	Ql_strcat((char *) st_sys.u8_tuno, (char *) c2);
	st_rec_hdr.lSerialNo = 0x10000000;
	st_rec_hdr.lSerialNo = st_rec_hdr.lSerialNo + (l1 << (6 * 4));

	// Year
	c1 = cReadId[2] - 0x30;
	c2[0] = cReadId[2];    // c1 ; // + 0x30 ;
	c2[1] = '\0';
	l1 = c1;
	Ql_strcat((char *) st_sys.u8_tuno, (char *) c2);
	st_rec_hdr.lSerialNo = st_rec_hdr.lSerialNo + (l1 << (5 * 4));

	c1 = cReadId[3] - 0x30;
	c2[0] = cReadId[3]; //c1 + 0x30 ;
	c2[1] = '\0';
	l1 = c1;
	Ql_strcat((char *) st_sys.u8_tuno, (char *) c2);
	st_rec_hdr.lSerialNo = st_rec_hdr.lSerialNo + (l1 << (4 * 4));

	// Serial No
	c1 = cReadId[4] - 0x30;
	c2[0] = cReadId[4]; //c1 + 0x30 ;
	c2[1] = '\0';
	l1 = c1;
	Ql_strcat((char *) st_sys.u8_tuno, (char *) c2);
	st_rec_hdr.lSerialNo = st_rec_hdr.lSerialNo + (l1 << (3 * 4));

	c1 = cReadId[5] - 0x30;
	c2[0] = cReadId[5]; //c1 + 0x30 ;
	c2[1] = '\0';
	l1 = c1;
	Ql_strcat((char *) st_sys.u8_tuno, (char *) c2);
	st_rec_hdr.lSerialNo = st_rec_hdr.lSerialNo + (l1 << (2 * 4));

	c1 = cReadId[6] - 0x30;
	c2[0] = cReadId[6]; //c1  + 0x30 ;
	c2[1] = '\0';
	l1 = c1;
	Ql_strcat((char *) st_sys.u8_tuno, (char *) c2);
	st_rec_hdr.lSerialNo = st_rec_hdr.lSerialNo + (l1 << (1 * 4));
	c1 = cReadId[7] - 0x30;
	c2[0] = cReadId[7]; //c1 + 0x30 ;
	c2[1] = '\0';
	l1 = c1;
	Ql_strcat((char *) st_sys.u8_tuno, (char *) c2);
	st_rec_hdr.lSerialNo = st_rec_hdr.lSerialNo + l1;
	// Model Identifier
	c1 = 1;
	if (c1 == MODEL_IDENTIFIER_SMS)
		Ql_strcat((char *) st_sys.u8_tuno, (char *) "SM");
	else if (c1 == MODEL_IDENTIFIER_GPRS)
		Ql_strcat((char *) st_sys.u8_tuno, (char *) "GP");
	st_rec_hdr.u16_FmVarsion = FM_VERSION;
	st_rec_hdr.u8_STX[0] = 2;
	st_rec_hdr.u8_STX[1] = 2;

}
/***************************************************************************/
/*  Function   : vts_utils_PrintVersion                                    */
/*-------------------------------------------------------------------------*/
/*  Object     : To Print the Version Detail of Application                */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
void vts_utils_PrintVersion(void) {

	APP_DEBUG("\r\n");
	APP_DEBUG("Unit No:");
	vts_utils_ReadSystemSerialNo();
	APP_DEBUG((char * ) st_sys.u8_tuno);
	APP_DEBUG("\r\n");
	Ql_strcpy((char *) st_sys.u8_InitApplicationName,
			(char *) "LSTR25_VTS_BASE");
	APP_DEBUG((char * ) st_sys.u8_InitApplicationName);
	APP_DEBUG("\r\n");
	Ql_strcpy((char *) st_sys.u8_InitCompanyName, (char *) "LEMS");
	APP_DEBUG((char * ) st_sys.u8_InitCompanyName);
	APP_DEBUG("\r\n");
	Ql_strcpy((char *) st_sys.u8_InitApplicationVersion, (char *) FM_VERSION);
	APP_DEBUG((char * ) st_sys.u8_InitApplicationVersion);
	APP_DEBUG("\r\n");
	Ql_strcpy((char *) st_sys.u8_FmDate, (char *) "19/07/16");
	APP_DEBUG((char * ) st_sys.u8_FmDate);
	APP_DEBUG("\r\n");

}
/***************************************************************************/
/*  Function   : vts_utils_BuffCpy_RecHeader                               */
/*-------------------------------------------------------------------------*/
/*  Object     : To copy the record header into the Buffer.                */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  *rec_header       |   |   |   | *rec_header                            */
/*--------------------+---+---+---+----------------------------------------*/
/*  *cData            |   |   |   | *cData                                 */
/*--------------------+---+---+---+----------------------------------------*/
/*  *iTotByte         |   |   |   | *iTotByte                              */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//vts_utils_SerialPrintHeader
void vts_utils_BuffCpy_RecHeader(const s_rec_hdr *restrict st_ptrrec,
		unsigned char *cData, int *iTotByte) {
	int i1 = 0;
	intchar itemp;
	LONGCHAR ltemp;
	cData[i1++] = st_ptrrec->u8_STX[0];
	cData[i1++] = st_ptrrec->u8_STX[1];

	cData[i1++] = st_ptrrec->u8_MessageType;

	itemp.ival = st_ptrrec->u16_TransactionID;
	cData[i1++] = itemp.cval[0];
	cData[i1++] = itemp.cval[1];

	ltemp.lval = st_ptrrec->lSerialNo;
	cData[i1++] = ltemp.cval[0];
	cData[i1++] = ltemp.cval[1];
	cData[i1++] = ltemp.cval[2];
	cData[i1++] = ltemp.cval[3];

	itemp.ival = st_ptrrec->u16_FmVarsion;
	cData[i1++] = itemp.cval[0];
	cData[i1++] = itemp.cval[1];

	cData[i1++] = st_ptrrec->u8_NoOfRecords;

	cData[i1++] = st_ptrrec->u8_DataType;

	cData[i1++] = st_ptrrec->u8_ExternalVolt;

	cData[i1++] = st_ptrrec->u8_IntVolt;

	itemp.ival = st_ptrrec->u16_FlhWritePtr;
	cData[i1++] = itemp.cval[0];
	cData[i1++] = itemp.cval[1];

	itemp.ival = st_ptrrec->u16_FlhReadPtr;
	cData[i1++] = itemp.cval[0];
	cData[i1++] = itemp.cval[1];

	*iTotByte = i1;
	//vts_utils_SerialStrPrint((u8 *) "<Data Header>:\r\n");
	//vts_utils_SerialDebug1(cData, i1);

}

/***************************************************************************/
/*  Function   : vts_utils_CopyDataStruct                                  */
/*-------------------------------------------------------------------------*/
/*  Object     : To copy the Data Record structure into                    */
/*               the another Data Record structure.                        */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  *source           |   |   |   | *source                                */
/*--------------------+---+---+---+----------------------------------------*/
/*  *Dest             |   |   |   | *Dest                                  */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/

void vts_utils_CopyDataStruct(s_txbuff_info *source, s_txbuff_info *Dest) {

	Dest->u8_MsgID = source->u8_MsgID;

	Dest->u8_RTC_Date[0] = source->u8_RTC_Date[0];

	Dest->u8_RTC_Date[1] = source->u8_RTC_Date[1];

	Dest->u8_RTC_Time[0] = source->u8_RTC_Time[0];

	Dest->u8_RTC_Time[1] = source->u8_RTC_Time[1];

	Dest->u8_RTC_Time[2] = source->u8_RTC_Time[2];

	Dest->u8_lat[0] = source->u8_lat[0];

	Dest->u8_lat[1] = source->u8_lat[1];

	Dest->u8_lat[2] = source->u8_lat[2];

	Dest->u8_lon[0] = source->u8_lon[0];

	Dest->u8_lon[1] = source->u8_lon[1];

	Dest->u8_lon[2] = source->u8_lon[2];

	Dest->u8_Speed = source->u8_Speed;

	Dest->u8_Direction = source->u8_Direction;

	Dest->lOdometer = source->lOdometer;

	Dest->u8_SignalStrength = source->u8_SignalStrength;

	Dest->Status1.Byte = source->Status1.Byte;

	Dest->Status2.Byte = source->Status2.Byte;

	Dest->u16_ANIP1_Val = source->u16_ANIP1_Val;

	Dest->u16_ANIP2_Val = source->u16_ANIP2_Val;

	Dest->u16_TemperatureVal = source->u16_TemperatureVal;

	Dest->u8_VibrationVal = source->u8_VibrationVal;

	Dest->u16_EngineSpeedPulse = source->u16_EngineSpeedPulse;

	//For Event Records

	Dest->u16_IdealDuration = source->u16_IdealDuration;

	Dest->u16_MaxSpeed = source->u16_MaxSpeed;

	Dest->u16_AverageSpeed = source->u16_AverageSpeed;

	Dest->u16_OverSpeedDuration = source->u16_OverSpeedDuration;

	Dest->u8_IButtonKey[0] = source->u8_IButtonKey[0];
	Dest->u8_IButtonKey[1] = source->u8_IButtonKey[1];
	Dest->u8_IButtonKey[2] = source->u8_IButtonKey[2];
	Dest->u8_IButtonKey[3] = source->u8_IButtonKey[3];
	Dest->u8_IButtonKey[4] = source->u8_IButtonKey[4];
	Dest->u8_IButtonKey[5] = source->u8_IButtonKey[5];
	Dest->u8_IButtonKey[6] = source->u8_IButtonKey[6];
	Dest->u8_IButtonKey[7] = source->u8_IButtonKey[7];

	Dest->u8_IButton_Status = source->u8_IButton_Status;

	//11 Bytes Container ID
	Dest->u8_ContainerID[0] = source->u8_ContainerID[0];
	Dest->u8_ContainerID[1] = source->u8_ContainerID[1];
	Dest->u8_ContainerID[2] = source->u8_ContainerID[2];
	Dest->u8_ContainerID[3] = source->u8_ContainerID[3];
	Dest->u8_ContainerID[4] = source->u8_ContainerID[4];
	Dest->u8_ContainerID[5] = source->u8_ContainerID[5];
	Dest->u8_ContainerID[6] = source->u8_ContainerID[6];
	Dest->u8_ContainerID[7] = source->u8_ContainerID[7];
	Dest->u8_ContainerID[8] = source->u8_ContainerID[8];
	Dest->u8_ContainerID[9] = source->u8_ContainerID[9];
	Dest->u8_ContainerID[10] = source->u8_ContainerID[10];

	Dest->u8_Container_Status = source->u8_Container_Status;  // 80 Bytes

	Dest->lDistance = source->lDistance;
}
#endif
