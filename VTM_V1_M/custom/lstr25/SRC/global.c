/*
 * global.c
 *
 *  Created on: Aug 18, 2016
 *      Author: Amruta
 */

#include "global.h"
#include "uart1.h"
#include "rtc.h"
#include "uart1.h"

s_globflags st_globflags;
s_task1_cnt st_task1_cnt;
s_glob_var st_glob_var;
s_rtc_cclk st_rtc_cclk;
extern s_gps_info st_gps_info;
void Initaize_AllGlobalVar(void) {
	st_globflags.bGps_afterfixtmrloaded = FALSE;
	st_globflags.bGPSfixafterboot = FALSE;                    //ok
	st_globflags.bGprsDataSendProcess = FALSE;

	st_task1_cnt.u8Gps_fix_delay = 0;
	st_task1_cnt.u16GPS_afterFix_Tmr = 0;

	st_glob_var.u8DataType = 0;
	st_glob_var.u8GprsAckState = 0;
	st_glob_var.u16OldRamReadPtr = 0;
	st_glob_var.u16OldUfsReadPtr = 0;
	st_glob_var.u32_signal_strength = 0;
	st_rtc_cclk.bLoadInternalRtc = TRUE;
	st_gps_info.u8_gps_sts = GPS_STATUS_FAIL;
}

