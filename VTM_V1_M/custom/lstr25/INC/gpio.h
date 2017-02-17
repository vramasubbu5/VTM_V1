/*
 * gpio.h
 *
 *  Created on: May 28, 2016
 *      Author: Amruta
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "custom_feature_def.h"
#include "ril.h"
#include "ril_util.h"
#include "ril_telephony.h"
#include "ril_network.h"
#include "ql_stdlib.h"
#include "string.h"
#include "ql_error.h"
#include "ql_trace.h"
#include "ql_timer.h"
#include "ql_uart.h"
#include "ql_system.h"
#include "ql_gpio.h"
#include "ql_adc.h"
#include "uart1.h"
#include "ql_timer.h"
#include "ql_type.h"
#include "global.h"
#include "main.h"

s32 stestvalue;

#define MSG_ID_USER_DATA              (0x1000 + 3)
#define MSG_ID_TASK_INIT_DATA         (0x1000 + 4)

void ADC_Program(void);
void GPIO_Init(void);
void sms_TextMode_Send(void);
void sms_TextMode_Read(u32 nIndex);

void Hdlr_RecvNewSMS(u32 nIndex, bool bAutoReply);

void Timer_handler(u32 timerId, void* param);

#endif /* GPIO_H_ */
