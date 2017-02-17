/*
 * global.h
 *
 *  Created on: Jun 23, 2016
 *      Author: Amruta
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

//#include "ql_socket.h"
#include "ql_timer.h"
#include "ql_type.h"
#include "ql_stdlib.h"
#include "system.h"

typedef enum {
	TCP_TIMER_ID = TIMER_ID_USER_START,
	TIMEOUT_90S_TIMER_ID,
	TASK2_TIMER,
	TASK3_TIMER,
	TASK1_TIMER,
} Enum_TMRID;

typedef enum {
	TASK1 = 0, TASK2, TASK3, TASK4, TASK5,
} m_task_init_fail;

typedef struct {
	bool bGps_afterfixtmrloaded;
	bool bGPSfixafterboot;
	bool bGprsDataSendProcess;
} s_globflags;

typedef struct {
	u8 u8DataType;
	u8 u8GprsAckState;
	u16 u16OldRamReadPtr;
	u16 u16OldUfsReadPtr;
	u32 u32_signal_strength;
} s_glob_var;
void Initaize_AllGlobalVar(void);

#endif /* GLOBAL_H_ */
