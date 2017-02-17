/*
 * data.c
 *
 *  Created on: Aug 4, 2016
 *      Author: Amruta Balasaheb Hajare
 */

#include "gpio.h"
#include "task3.h"
#include "task2.h"
#include "system.h"
#include "utils.h"
#include "rtc.h"
/*
 *
 *
 */

static s_txbuff_info st_txbuff_info;

static s_txbuff_info st_txbuff_inforam[100];

s_rec_hdr st_rec_hdr;

extern s_ufs_data_buff st_ufs_data_buff;

extern s_gprs_dataptr st_gprsdata;
extern s_gprs_data st_gprs_data;

extern s_task1_cnt st_task1_cnt;

extern s_ufs_data_ptrbuff st_ufsdata;

extern s_gps_info st_gps_info;
extern s_rtc_cclk st_rtc_cclk;
static s_dev_cclk st_dev_cclk;

s_ram_array st_ram_array;

s_ram_array st_ram_arraytemp;

s_ram_array1 st_ram_array1;

extern s_globflags st_globflags;
extern s_glob_var st_glob_var;

extern s_gps_buff st_gps_buff;

//static unsigned char Data_PushRam(void);

u8 LatitudeFormatinLong(char *cLat, unsigned long *llat);
u8 LongitudeFormatinLong(char *cLon, unsigned long *llon);
u8 vts_data_FormatDateTimeinInt(s_dev_cclk * restrict dev, u16* u16DateInt,
		unsigned char *cTime);
u8 vts_data_FormatRTCDateTimeinInt(s_rtc_cclk * restrict dev, u16* u16DateInt,
		unsigned char *cTime);

unsigned char Data_Collect_From_ram(unsigned char * ucptrdata,
		u16 * u16ptrlength);

u16 Load_Ram_Data(unsigned char * ucptrdata, s_txbuff_info * restrict st_ptrdes,
		u16 ilength);
u8 Collect_Data_Header(s_rec_hdr * restrict st_ptrdes);
u8 Load_Data_Header(unsigned char * ucptrdata, s_rec_hdr * restrict st_ptrdes);

///////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////
u8 Collect_Data_Header(s_rec_hdr * restrict st_ptrdes) {

	if (st_ptrdes != NULL) {
		Ql_memset(st_ptrdes->u8_STX, 0x00, sizeof(st_ptrdes->u8_STX));

		st_ptrdes->u8_STX[0] = 0x02;

		st_ptrdes->u8_STX[1] = 0x02;

		st_ptrdes->u8_RecordType = 0x00;   // check this display

		st_ptrdes->u8_RecordQty = 0x20;

		st_ptrdes->lDeviceID = 0x03001015;

		st_ptrdes->u16_TransactionID = 0x10;

		st_ptrdes->u16_FmVarsion = 0x9001;

		st_ptrdes->u16_FlhWritePtr = ufs_data_ptr.u16WritePtr;

		st_ptrdes->u16_FlhReadPtr = ufs_data_ptr.u16ReadPtr;

		st_ptrdes->u8_ExternalVolt = 0x20;

		st_ptrdes->u8_IntVolt = 0x30;

		return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////////////
u8 Load_Data_Header(unsigned char * ucptrdata, s_rec_hdr * restrict st_ptrdes) {
	u8 i1 = 0, u8_lenth = 0;
	longchar temp;

	//Ql_memset(ucptrdata, 0x00, sizeof(ucptrdata));

	if ((st_ptrdes != NULL) && (ucptrdata != NULL)) {

		Collect_Data_Header(&st_rec_hdr);

		ucptrdata[i1++] = st_ptrdes->u8_STX[0];
		ucptrdata[i1++] = st_ptrdes->u8_STX[1];

		ucptrdata[i1++] = st_ptrdes->u8_RecordType;

		ucptrdata[i1++] = st_ptrdes->u8_RecordQty;

		temp.lval = st_ptrdes->lDeviceID;
		ucptrdata[i1++] = temp.cval[0];
		ucptrdata[i1++] = temp.cval[1];
		ucptrdata[i1++] = temp.cval[2];
		ucptrdata[i1++] = temp.cval[3];

		ucptrdata[i1++] = (st_ptrdes->u16_TransactionID & 0x00ff);
		ucptrdata[i1++] = (st_ptrdes->u16_TransactionID >> 8) & 0x00ff;

		ucptrdata[i1++] = (st_ptrdes->u16_FmVarsion & 0x00ff);
		ucptrdata[i1++] = (st_ptrdes->u16_FmVarsion >> 8) & 0x00ff;

		ucptrdata[i1++] = (st_ptrdes->u16_FlhWritePtr & 0x00ff);
		ucptrdata[i1++] = (st_ptrdes->u16_FlhWritePtr >> 8) & 0x00ff;

		ucptrdata[i1++] = (st_ptrdes->u16_FlhReadPtr & 0x00ff);
		ucptrdata[i1++] = (st_ptrdes->u16_FlhReadPtr >> 8) & 0x00ff;

		ucptrdata[i1++] = st_ptrdes->u8_ExternalVolt;

		ucptrdata[i1++] = st_ptrdes->u8_IntVolt;
		u8_lenth = i1;

	}

	return u8_lenth;
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////
unsigned char Data_Load(const s_gps_info * restrict stptrgps,
		const s_mc_info * restrict stptrmc) {

	u16 u16_temp = 0;
	char ctempBuff[13];
	long ltemp, ltemp1;
	APP_DEBUG("DATA_LOAD\r\n");
	Ql_memset(ctempBuff, 0x00, sizeof(ctempBuff));
	///*

	if (st_gps_info.u8_gps_sts == GPS_STATUS_FAIL) {
		//APP_DEBUG("GPS_STATUS_FAIL_Enter\r\n");
		//Read the time from the internal clock or from GSM when GPS is not present
		Ql_RIL_SendATCmd("AT+CCLK?\0", 11, ATResponse_QRTC_Handler,
		NULL, 0);
	} else {
		APP_DEBUG("GPS_STATUS_SUCESS_Enter\r\n");
		//Here we start the timer for check the after gps fixing duration to update internal RTC time with that.
		if (st_globflags.bGps_afterfixtmrloaded == FALSE) {
			st_task1_cnt.u16GPS_afterFix_Tmr = 0;
			st_globflags.bGps_afterfixtmrloaded = TRUE;
		}
	}
	// */ 074132.000,A,1902.6571,N,07300.8355,E,0.00,74.79,030217,,,D*52A


	APP_DEBUG((char* )st_gps_buff.u8Rmc);
	APP_DEBUG("\r\n");
	//add signal strength display and detection  and display gprmc string
	u32 ber = 0;
	RIL_NW_GetSignalQuality(&st_glob_var.u32_signal_strength, &ber);

	APP_DEBUG("<-- Signal strength:%d\r\n", st_glob_var.u32_signal_strength);

	//////////////////////////////////////////////////////////
	if (stptrgps != NULL && stptrmc != NULL) {
//#ifdef NOTUSED
		//	/*
		st_txbuff_info.u8_MsgID = 1;		// added for testing

		if (st_gps_info.u8_gps_sts == GPS_STATUS_SUCCESS) {
			Ql_strcpy((char *) st_dev_cclk.cdate, (char *) stptrgps->u8_Date);
			Ql_strcpy((char *) st_dev_cclk.ctime, (char *) stptrgps->u8_Time);
			vts_data_FormatDateTimeinInt(&st_dev_cclk, (u16*) &u16_temp,
					(unsigned char *) ctempBuff);

		} else {
			vts_data_FormatRTCDateTimeinInt(&st_rtc_cclk, (u16*) &u16_temp,
					(unsigned char *) ctempBuff);
		}

		// insert RTC date

		st_txbuff_info.u8_RTC_Date[1] = ((0xFF00 & u16_temp) >> 8) & 0xff;
		st_txbuff_info.u8_RTC_Date[0] = (0x00FF & u16_temp) & 0xff;
		//	 glob_vars.iRTCdate_acc = itemp3;

		if (st_globflags.bGPSfixafterboot == TRUE) {
			//Here we checking the Seconds of previous and current poll is same or not, if it is same we avoid poll
			if ((st_txbuff_info.u8_RTC_Time[2] == ctempBuff[2])) {
				//GLOBALFlags2.bDisableDataPoll = TRUE;
			} else {
				//GLOBALFlags2.bDisableDataPoll = FALSE;
			}
		}
		// insert RTC time
		st_txbuff_info.u8_RTC_Time[0] = ctempBuff[0];
		st_txbuff_info.u8_RTC_Time[1] = ctempBuff[1];
		st_txbuff_info.u8_RTC_Time[2] = ctempBuff[2];

		st_txbuff_info.u8_Speed = stptrgps->u8_Speed;

		u16_temp = Ql_atoi((char *) stptrgps->u8_Alti);

		if (u16_temp > 360)
			u16_temp = 0;
		st_txbuff_info.u8_Direction = ((u16_temp >> 1) & 0x00ff);

		LatitudeFormatinLong((char*) stptrgps->u8_Lat,
				(unsigned long *) &ltemp);
		ltemp1 = (((0x00FF0000 & ltemp) >> 16) & 0xff);
		st_txbuff_info.u8_lat[2] = (unsigned char) ltemp1;
		ltemp1 = (((0x0000FF00 & ltemp) >> 8) & 0xff);
		st_txbuff_info.u8_lat[1] = (unsigned char) ltemp1;
		ltemp1 = (0x000000FF & ltemp) & 0xff;
		st_txbuff_info.u8_lat[0] = (unsigned char) ltemp1;

		st_txbuff_info.u8_SignalStrength = (u8) st_glob_var.u32_signal_strength;

		LongitudeFormatinLong((char*) stptrgps->u8_lon,
				(unsigned long *) &ltemp);
		ltemp1 = (((0x00FF0000 & ltemp) >> 16) & 0xff);
		st_txbuff_info.u8_lon[2] = (unsigned char) ltemp1;
		ltemp1 = (((0x0000FF00 & ltemp) >> 8) & 0xff);
		st_txbuff_info.u8_lon[1] = (unsigned char) ltemp1;
		ltemp1 = (0x000000FF & ltemp) & 0xff;
		st_txbuff_info.u8_lon[0] = (unsigned char) ltemp1;

		st_txbuff_info.Status1.Byte = 0xFF;

		st_txbuff_info.lOdometer = 0xFFFF;

		st_txbuff_info.Status2.Byte = 0xFF;

		st_txbuff_info.Status3.Byte = 0xFF;
		/*
		 st_txbuff_info.u16_ANIP1_Val = stptrmc->u16_TempSens1;
		 st_txbuff_info.u16_ANIP2_Val = stptrmc->u16_TempSens2;
		 st_txbuff_info.u16_ANIP3_Val = stptrmc->u16_TempSens3;
		 st_txbuff_info.u16_ANIP4_Val = stptrmc->u16_PresSens1;
		 st_txbuff_info.u16_ANIP5_Val = stptrmc->u16_PresSens2;
		 */
		st_txbuff_info.u16_ANIP1_Val = 0xaaaa;
		st_txbuff_info.u16_ANIP2_Val = 0xbbbb;
		st_txbuff_info.u16_ANIP3_Val = 0xcccc;
		st_txbuff_info.u16_ANIP4_Val = 0xdddd;
		st_txbuff_info.u16_ANIP5_Val = 0xeeee;

		st_txbuff_info.u16_Rod1_Lit = 0xFF;
		st_txbuff_info.u16_Rod2_Lit = 0xFF;
		st_txbuff_info.u16_Rod3_Lit = 0xFF;
		st_txbuff_info.u16_Rod1_ADC = 0xFF;
		st_txbuff_info.u16_Rod2_ADC = 0xFF;
		st_txbuff_info.u16_Rod3_ADC = 0xFF;

		st_txbuff_info.u8_IButtonKey[0] = 0xfa;
		st_txbuff_info.u8_IButtonKey[1] = 0xea;
		st_txbuff_info.u8_IButtonKey[2] = 0xda;
		st_txbuff_info.u8_IButtonKey[3] = 0xca;
		st_txbuff_info.u8_IButtonKey[4] = 0xba;
		st_txbuff_info.u8_IButtonKey[5] = 0xab;
		st_txbuff_info.u8_IButtonKey[6] = 0xac;
		st_txbuff_info.u8_IButtonKey[7] = 0xad;
		/*
		 st_txbuff_info.u8_IButtonKey[0] = stptrmc->u8_Ibutt[0];
		 st_txbuff_info.u8_IButtonKey[1] = stptrmc->u8_Ibutt[1];
		 st_txbuff_info.u8_IButtonKey[2] = stptrmc->u8_Ibutt[2];
		 st_txbuff_info.u8_IButtonKey[3] = stptrmc->u8_Ibutt[0];
		 st_txbuff_info.u8_IButtonKey[4] = stptrmc->u8_Ibutt[0];
		 st_txbuff_info.u8_IButtonKey[5] = stptrmc->u8_Ibutt[0];
		 st_txbuff_info.u8_IButtonKey[6] = stptrmc->u8_Ibutt[0];
		 st_txbuff_info.u8_IButtonKey[7] = stptrmc->u8_Ibutt[0];
		 */
		//	 */
//#endif
		///////////////////////////////////////////////////////
		/*
		 st_txbuff_info.u8_MsgID = 1;

		 st_txbuff_info.u8_RTC_Time[0] = 0x11;
		 st_txbuff_info.u8_RTC_Time[1] = 0x30;
		 st_txbuff_info.u8_RTC_Time[2] = 0x29;

		 st_txbuff_info.u8_RTC_Date[1] = 0x12;
		 st_txbuff_info.u8_RTC_Date[0] = 0x13;

		 st_txbuff_info.u8_Speed = 0xcd;

		 st_txbuff_info.u8_Direction = 0xef;

		 st_txbuff_info.u8_lat[2] = 0xda;
		 st_txbuff_info.u8_lat[1] = 0xea;
		 st_txbuff_info.u8_lat[0] = 0xfa;

		 st_txbuff_info.u8_SignalStrength = 0xba;

		 st_txbuff_info.u8_lon[2] = 0xba;
		 st_txbuff_info.u8_lon[1] = 0xcb;
		 st_txbuff_info.u8_lon[0] = 0xdb;

		 st_txbuff_info.Status1.Byte = 0xFF;

		 st_txbuff_info.lOdometer = 0xFFFF;

		 st_txbuff_info.Status2.Byte = 0xFF;

		 st_txbuff_info.Status3.Byte = 0xFF;

		 st_txbuff_info.u16_ANIP1_Val = 0xaaaa;
		 st_txbuff_info.u16_ANIP2_Val = 0xbbbb;
		 st_txbuff_info.u16_ANIP3_Val = 0xcccc;
		 st_txbuff_info.u16_ANIP4_Val = 0xdddd;
		 st_txbuff_info.u16_ANIP5_Val = 0xeeee;

		 st_txbuff_info.u16_Rod1_Lit = 0xFF;
		 st_txbuff_info.u16_Rod2_Lit = 0xFF;
		 st_txbuff_info.u16_Rod3_Lit = 0xFF;
		 st_txbuff_info.u16_Rod1_ADC = 0xFF;
		 st_txbuff_info.u16_Rod2_ADC = 0xFF;
		 st_txbuff_info.u16_Rod3_ADC = 0xFF;

		 st_txbuff_info.u8_IButtonKey[0] = 0xfa;
		 st_txbuff_info.u8_IButtonKey[1] = 0xea;
		 st_txbuff_info.u8_IButtonKey[2] = 0xda;
		 st_txbuff_info.u8_IButtonKey[3] = 0xca;
		 st_txbuff_info.u8_IButtonKey[4] = 0xba;
		 st_txbuff_info.u8_IButtonKey[5] = 0xab;
		 st_txbuff_info.u8_IButtonKey[6] = 0xac;
		 st_txbuff_info.u8_IButtonKey[7] = 0xad;
		 */
///////////////////////////////////////////////////////////////////
		//Data_PushRam();
	}
	return FALSE;
}

unsigned char Data_PushRam(void) { // for polling
	//u16 u16_datalength = 0;
	u8 u8_ret = 0;

	Ql_memset(st_ram_array.u8_dataBuff, 0x00, sizeof(st_ram_array.u8_dataBuff));

	st_txbuff_inforam[st_ram_array.u16WritePtr] = st_txbuff_info;
	st_ram_array.u16WritePtr++;

	if (st_ram_array.u16WritePtr >= MAX_SIZE_OF_RAM) {
		st_ram_array.u16WritePtr = 0;
		st_ram_array.bRewind = TRUE;
	}

	u16 u16ptrdiff = 0;

	if (st_ram_array.bRewind == TRUE) {

		APP_DEBUG("\r\n Ram_Rewind_Set -->");

		u16ptrdiff = MAX_SIZE_OF_RAM - st_ram_array.u16ReadPtr;

		u16ptrdiff += st_ram_array.u16WritePtr;

	} else {

		u16ptrdiff = st_ram_array.u16WritePtr - st_ram_array.u16ReadPtr;

	}

//	if (u16ptrdiff >= MAX_WRITE_LIMIT_OF_RAM)   // for reference by KMani

	if (u16ptrdiff ) {
		//write in file
		APP_DEBUG("\r\n Ram_Push_Ram1 -->\r\n");

		if (Data_Collect_From_ram(st_ufs_data_buff.u8writevector,
				&st_ufs_data_buff.u16writelength) == TRUE) {

			// to write in file
			u8_ret = Data_Write_in_File();

			if (u8_ret == SUCCESS) {
				APP_DEBUG("\r\n Ram_Push_in_file -->\r\n");
			} else {
				APP_DEBUG("\r\n Ram_Push_in_file_fail -->\r\n");
			}
		}

	}
	return TRUE;

}

unsigned char Data_Collect_From_ram(unsigned char * ucptrdata,
		u16 * u16ptrlength) {

	u8 u8i1 = 0, u8length = 0, i2 = 0;

	u16 ilength = 0;
//Ql_memset(ucptrdata, 0x00, sizeof(ucptrdata));
//load header here
	if (ucptrdata != NULL && u16ptrlength != NULL) {
		u8length = Load_Data_Header(ucptrdata, &st_rec_hdr);

		APP_DEBUG("Ram_Header\r\n");
		for (u8i1 = 0; u8i1 < u8length; u8i1++) {
			APP_DEBUG(":%x", ucptrdata[u8i1]);
		}

		ilength += u8length;
		st_glob_var.u16OldRamReadPtr = st_ram_array.u16ReadPtr;

		APP_DEBUG("Wrt_Ptr:%d/Rd_Ptr:%d/Old_RdPtr:%d\r\n",
				st_ram_array.u16WritePtr, st_ram_array.u16ReadPtr,
				st_glob_var.u16OldRamReadPtr);
	//	for (u8i1 = 0; u8i1 < MAX_WRITE_LIMIT_OF_RAM; u8i1++)   // for reference by KMani
		for (u8i1 = 0; u8i1 < 05 ; u8i1++) {

			APP_DEBUG("Collect_dta_for_loop\r\n");

			if ((st_ram_array.u16ReadPtr < st_ram_array.u16WritePtr)
					|| (st_ram_array.bRewind == TRUE)) {

				APP_DEBUG("Collect_dta_for_loop111\r\n");

				ilength = Load_Ram_Data(ucptrdata,
						&st_txbuff_inforam[st_ram_array.u16ReadPtr], ilength);

				APP_DEBUG("Ram_data_length:%d\r\n", ilength);

				//ilength += ilength;

				APP_DEBUG("Ram_data_length:%d\r\n", ilength);
				for (i2 = 0; i2 < ilength; i2++) {
					APP_DEBUG(":%x", ucptrdata[i2]);
				}
				st_ram_array.u16ReadPtr++;

				if (st_ram_array.u16ReadPtr >= MAX_SIZE_OF_RAM) {
					st_ram_array.u16ReadPtr = 0;
					st_ram_array.bRewind = FALSE;

				}

			} else
				break;

		}

		*u16ptrlength = 0;

		if (u8i1 > 0) {
			*u16ptrlength = ilength;

			APP_DEBUG("DataCollectLength:%d\r\n", *u16ptrlength);
			return TRUE;

		}
	}
	return FALSE;
}

unsigned char Data_Retrieve(void) {       //for transmit

	char ret = 0;

//APP_DEBUG("Data_Retrieve_enter\r\n");

	ret = Data_Read_From_File();

	APP_DEBUG("Data_Retrieve_enter:%d\r\n", ret);

	if (ret == FAILURE) {

		if (Data_Collect_From_ram(st_ram_array1.u8_txarray,

		&st_ram_array1.u16_txlength) == TRUE) {

			st_gprsdata.u8ptrindata = st_ram_array1.u8_txarray;

			st_gprsdata.u16inLength = &st_ram_array1.u16_txlength;

			APP_DEBUG("Data_Retrieve_from_ram\r\n");
			st_glob_var.u8DataType = 1;
			return TRUE;
		}
	} else if (ret == 1) {

		if ((st_gprsdata.u8ptrindata != NULL)
				&& (st_gprsdata.u16inLength != NULL)) {
			st_gprsdata.u8ptrindata = st_ufsdata.u8ptroutdata;

			st_gprsdata.u16inLength = st_ufsdata.u16ptroutlength;
			APP_DEBUG("Data_Retrieve_from_flash\r\n");
			APP_DEBUG("UFS_Wrt_ptr:%d/Rd_Ptr:%d/OldRd_Ptr:%d!\r\n",
					ufs_data_ptr.u16WritePtr, ufs_data_ptr.u16ReadPtr,
					st_glob_var.u16OldUfsReadPtr);
			st_glob_var.u8DataType = 2;
			return TRUE;
		}
	} else {
		APP_DEBUG("read_file_fail");
		return FALSE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////////////////////
u16 Load_Ram_Data(unsigned char * ucptrdata, s_txbuff_info* restrict st_ptrdes,
		u16 ilength) {
	u16 i1 = ilength, u16_lenth = 0;
	APP_DEBUG("Load_ram_data \r\n");
	if ((st_ptrdes != NULL) && (ucptrdata != NULL)) {

		ucptrdata[i1++] = st_ptrdes->u8_MsgID;

		ucptrdata[i1++] = st_ptrdes->u8_RTC_Time[0];
		ucptrdata[i1++] = st_ptrdes->u8_RTC_Time[1];
		ucptrdata[i1++] = st_ptrdes->u8_RTC_Time[2];

		ucptrdata[i1++] = st_ptrdes->u8_RTC_Date[0];
		ucptrdata[i1++] = st_ptrdes->u8_RTC_Date[1];

		ucptrdata[i1++] = st_ptrdes->u8_Speed;

		ucptrdata[i1++] = st_ptrdes->u8_Direction;

		ucptrdata[i1++] = st_ptrdes->u8_lat[0];
		ucptrdata[i1++] = st_ptrdes->u8_lat[1];
		ucptrdata[i1++] = st_ptrdes->u8_lat[2];

		ucptrdata[i1++] = st_ptrdes->u8_SignalStrength;

		ucptrdata[i1++] = st_ptrdes->u8_lon[0];
		ucptrdata[i1++] = st_ptrdes->u8_lon[1];
		ucptrdata[i1++] = st_ptrdes->u8_lon[2];

		ucptrdata[i1++] = st_ptrdes->Status1.Byte;

		ucptrdata[i1++] = (st_ptrdes->lOdometer & 0x000000ff);
		ucptrdata[i1++] = (st_ptrdes->lOdometer >> 8) & 0x000000ff;
		ucptrdata[i1++] = (st_ptrdes->lOdometer >> 16) & 0x000000ff;
		ucptrdata[i1++] = (st_ptrdes->lOdometer >> 24) & 0x000000ff;

		ucptrdata[i1++] = st_ptrdes->Status2.Byte;
		ucptrdata[i1++] = st_ptrdes->Status3.Byte;

		ucptrdata[i1++] = (st_ptrdes->u16_ANIP1_Val & 0x00ff);
		ucptrdata[i1++] = (st_ptrdes->u16_ANIP1_Val >> 8) & 0x00ff;

		ucptrdata[i1++] = (st_ptrdes->u16_ANIP2_Val & 0x00ff);
		ucptrdata[i1++] = (st_ptrdes->u16_ANIP2_Val >> 8) & 0x00ff;

		ucptrdata[i1++] = (st_ptrdes->u16_ANIP3_Val & 0x00ff);
		ucptrdata[i1++] = (st_ptrdes->u16_ANIP3_Val >> 8) & 0x00ff;

		ucptrdata[i1++] = (st_ptrdes->u16_ANIP4_Val & 0x00ff);
		ucptrdata[i1++] = (st_ptrdes->u16_ANIP4_Val >> 8) & 0x00ff;

		ucptrdata[i1++] = (st_ptrdes->u16_ANIP5_Val & 0x00ff);
		ucptrdata[i1++] = (st_ptrdes->u16_ANIP5_Val >> 8) & 0x00ff;

		ucptrdata[i1++] = (st_ptrdes->u16_Rod1_Lit & 0x00ff);
		ucptrdata[i1++] = (st_ptrdes->u16_Rod1_Lit >> 8) & 0x00ff;

		ucptrdata[i1++] = (st_ptrdes->u16_Rod2_Lit & 0x00ff);
		ucptrdata[i1++] = (st_ptrdes->u16_Rod2_Lit >> 8) & 0x00ff;

		ucptrdata[i1++] = (st_ptrdes->u16_Rod3_Lit & 0x00ff);
		ucptrdata[i1++] = (st_ptrdes->u16_Rod3_Lit >> 8) & 0x00ff;

		ucptrdata[i1++] = (st_ptrdes->u16_Rod1_ADC & 0x00ff);
		ucptrdata[i1++] = (st_ptrdes->u16_Rod1_ADC >> 8) & 0x00ff;

		ucptrdata[i1++] = (st_ptrdes->u16_Rod2_ADC & 0x00ff);
		ucptrdata[i1++] = (st_ptrdes->u16_Rod2_ADC >> 8) & 0x00ff;

		ucptrdata[i1++] = (st_ptrdes->u16_Rod3_ADC & 0x00ff);
		ucptrdata[i1++] = (st_ptrdes->u16_Rod3_ADC >> 8) & 0x00ff;

		ucptrdata[i1++] = st_ptrdes->u8_IButtonKey[0];
		ucptrdata[i1++] = st_ptrdes->u8_IButtonKey[1];
		ucptrdata[i1++] = st_ptrdes->u8_IButtonKey[2];
		ucptrdata[i1++] = st_ptrdes->u8_IButtonKey[3];
		ucptrdata[i1++] = st_ptrdes->u8_IButtonKey[4];
		ucptrdata[i1++] = st_ptrdes->u8_IButtonKey[5];
		ucptrdata[i1++] = st_ptrdes->u8_IButtonKey[6];
		ucptrdata[i1++] = st_ptrdes->u8_IButtonKey[7];
		u16_lenth = i1;
		//APP_DEBUG("Load_ram_data111111111 \r\n");

		//for (i1 = 0; i1 < u16_lenth; i1++) {
		//	APP_DEBUG(":%x", ucptrdata[i1]);
		//}
	}

	return u16_lenth;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////////////////////
u8 LatitudeFormatinLong(char *cLat, unsigned long *llat) {
	char cDegBuf[4], cMinBufd[8];
	float f1, f2;
	unsigned long l1;
	Ql_memset(cDegBuf, 0x00, sizeof(cDegBuf));
	Ql_memset(cMinBufd, 0x00, sizeof(cMinBufd));
//Ql_memset(gps_latlong_bak.lat, 0x00, sizeof(gps_latlong_bak.lat));  //disable by amruta,should enable for complete application
//Ql_strcpy(gps_latlong_bak.lat, (char *) gps_var.cLat);
	Ql_strncpy(cDegBuf, (char *) st_gps_info.u8_Lat, 2);
	cDegBuf[2] = '\0';
	f1 = Ql_atoi(cDegBuf);
// copy lat minutes
	Ql_strncpy(cMinBufd, (char *) st_gps_info.u8_Lat + 2, 7);
// Nul terminate
	cMinBufd[7] = '\0';
	f2 = Ql_atof((char *) cMinBufd);
	f1 = f1 + f2 / 60;
	l1 = f1 * 25000;
	*(unsigned long *) llat = (unsigned long) l1;
	return SUCCESS;
}
////////////////////////////////////////////////////////
u8 LongitudeFormatinLong(char *cLon, unsigned long *llon) {
	char cDegBuf[4], cMinBufd[8];
	float f1, f2;
	unsigned long l1;
	Ql_memset(cDegBuf, 0x00, sizeof(cDegBuf));
	Ql_memset(cMinBufd, 0x00, sizeof(cMinBufd));
//Ql_memset(gps_latlong_bak.lat, 0x00, sizeof(gps_latlong_bak.lon));    //disable by amruta,should enable for complete application
//Ql_strcpy(gps_latlong_bak.lat, (char *) gps_var.cLon);
	Ql_strncpy(cDegBuf, (char *) st_gps_info.u8_lon, 3);
	cDegBuf[2] = '\0';
	f1 = Ql_atoi(cDegBuf);
// copy lat minutes
	Ql_strncpy(cMinBufd, (char *) st_gps_info.u8_lon + 3, 7);
// Nul terminate
	cMinBufd[7] = '\0';
	f2 = Ql_atof((char *) cMinBufd);
	f1 = f1 + f2 / 60;
	l1 = f1 * 25000;
	*(unsigned long *) llon = (unsigned long) l1;
	return SUCCESS;
}
/***************************************************************************/
/*  Function   : vts_data_FormatDateTimeinInt                              */
/*-------------------------------------------------------------------------*/
/*  Object     : To form GPS date and time for sending                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  *dev              |   |   |   | *dev                                   */
/*--------------------+---+---+---+----------------------------------------*/
/*  *iDateInt         |   |   |   | *iDateInt                              */
/*--------------------+---+---+---+----------------------------------------*/
/*  *cTime            |   |   |   | *cTime                                 */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
u8 vts_data_FormatDateTimeinInt(s_dev_cclk * restrict dev, u16* u16DateInt,
		unsigned char *cTime) {
	char cBuff[4];
	unsigned long l1, l2, l3, l4;

	l1 = l2 = l3 = l4 = 0;
///////// date
	cBuff[0] = *((char *) dev->cdate + 0);
	cBuff[1] = *((char *) dev->cdate + 1);
	cBuff[2] = 0;
	l1 = Ql_atoi((char *) cBuff);

	cBuff[0] = *((char *) dev->cdate + 3);
	cBuff[1] = *((char *) dev->cdate + 4);
	cBuff[2] = 0;
	l2 = Ql_atoi((char *) cBuff);

	cBuff[0] = *((char *) dev->cdate + 6);
	cBuff[1] = *((char *) dev->cdate + 7);
	cBuff[2] = 0;
	l3 = Ql_atoi((char *) cBuff);

	l4 = l1 + (l2 * 32) + (l3 * 13 * 32);
	*u16DateInt = (int) l4;

	l1 = l2 = l3 = l4 = 0;
///////////// time
	cBuff[0] = *((char *) dev->ctime + 0);
	cBuff[1] = *((char *) dev->ctime + 1);
	cBuff[2] = 0;
	l1 = Ql_atoi((char *) cBuff);

	cBuff[0] = *((char *) dev->ctime + 3);
	cBuff[1] = *((char *) dev->ctime + 4);
	cBuff[2] = 0;
	l2 = Ql_atoi((char *) cBuff);

	cBuff[0] = *((char *) dev->ctime + 6);
	cBuff[1] = *((char *) dev->ctime + 7);
	cBuff[2] = 0;
	l3 = Ql_atoi((char *) cBuff);

	cTime[0] = (l1 & 0x000000FF);
	cTime[1] = (l2 & 0x000000FF);
	cTime[2] = (l3 & 0x000000FF);
	cTime[3] = '\0';

	return SUCCESS;
}
/***************************************************************************/
/*  Function   : vts_data_FormatRTCDateTimeinInt                           */
/*-------------------------------------------------------------------------*/
/*  Object     : To form RTC date and time for sending                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  *dev              |   |   |   | *dev                                   */
/*--------------------+---+---+---+----------------------------------------*/
/*  *iDateInt         |   |   |   | *iDateInt                              */
/*--------------------+---+---+---+----------------------------------------*/
/*  *cTime            |   |   |   | *cTime                                 */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
u8 vts_data_FormatRTCDateTimeinInt(s_rtc_cclk * restrict dev, u16* u16DateInt,
		unsigned char *cTime) {
	char cBuff[4];
	unsigned long l1, l2, l3, l4;

	l1 = l2 = l3 = l4 = 0;
///////// date
	cBuff[0] = *((char *) dev->cdate + 0);
	cBuff[1] = *((char *) dev->cdate + 1);
	cBuff[2] = 0;
	l1 = Ql_atoi((char *) cBuff);
	//APP_DEBUG(":%d\r\n", l1);

	cBuff[0] = *((char *) dev->cdate + 3);
	cBuff[1] = *((char *) dev->cdate + 4);
	cBuff[2] = 0;
	l2 = Ql_atoi((char *) cBuff);
	//APP_DEBUG(":%d\r\n", l2);

	cBuff[0] = *((char *) dev->cdate + 6);
	cBuff[1] = *((char *) dev->cdate + 7);
	cBuff[2] = 0;
	l3 = Ql_atoi((char *) cBuff);
	//APP_DEBUG(":%d\r\n", l3);

	l4 = l1 + (l2 * 32) + (l3 * 13 * 32);

	*u16DateInt = (int) l4;
	//APP_DEBUG("ConvertDate:%d", (int )*u16DateInt);
	l1 = l2 = l3 = l4 = 0;
///////////// time
	cBuff[0] = *((char *) dev->ctime + 0);
	cBuff[1] = *((char *) dev->ctime + 1);
	cBuff[2] = 0;
	l1 = Ql_atoi((char *) cBuff);

	cBuff[0] = *((char *) dev->ctime + 3);
	cBuff[1] = *((char *) dev->ctime + 4);
	cBuff[2] = 0;
	l2 = Ql_atoi((char *) cBuff);

	cBuff[0] = *((char *) dev->ctime + 6);
	cBuff[1] = *((char *) dev->ctime + 7);
	cBuff[2] = 0;
	l3 = Ql_atoi((char *) cBuff);

	cTime[0] = (l1 & 0x000000FF);
	cTime[1] = (l2 & 0x000000FF);
	cTime[2] = (l3 & 0x000000FF);
	cTime[3] = '\0';
	return SUCCESS;
}
