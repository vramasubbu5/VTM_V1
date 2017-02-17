/*
 * rtc.h
 *
 *  Created on: Aug 25, 2016
 *      Author: Amruta
 */

#ifndef RTC_H_
#define RTC_H_

#include "ql_timer.h"
#include "ql_type.h"
#include "ql_stdlib.h"

#define VTS_BS_DATE 			10
#define VTS_BS_TIME				10

typedef struct {
	char cdate[VTS_BS_DATE];
	char ctime[VTS_BS_TIME];
} s_dev_cclk;

typedef struct {
	volatile bool bLoadInternalRtc;
	char cdate[VTS_BS_DATE];
	char ctime[VTS_BS_TIME];
} s_rtc_cclk;

void rtc_SetClock(s_rtc_cclk *rtc_cclk);
void rtc_ReadClock(void);

#endif /* RTC_H_ */
