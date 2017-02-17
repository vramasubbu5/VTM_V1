/*
 * task1.c
 *
 *  Created on: Jun 24, 2016
 *      Author: Amruta
 */
/***************************************************************************/
/*This task consist of all three UART initialization in which main UART    */
/* is for debug and programming(UART1),Debug  UART is for micro controller */
/*communication(UART2),Auxillary UART for GPS communication(UART3).        */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
#include "gpio.h"
#include "system.h"
#include "string.h"
#include "rtc.h"


/*
 *
 * ************** Main task starts ***************
 *
 */
/***************************************************************************/
/*  Function   : Uart_Initialize                                           */
/*-------------------------------------------------------------------------*/
/*  Object     : To Initialise the uart  				                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/

void proc_subtask_gpio(s32 taskId)
{
	APP_DEBUG("\r\n xxxxxxxxxxxxxtask_start:%d", taskId);
	ST_MSG msg6;

//	char sc_return = 0;
//	bool bTaskStart = FALSE;
	while (TRUE) {

			Ql_OS_GetMessage(&msg6);


	APP_DEBUG("\r\n xxxxtask_start:%d", taskId);
	APP_DEBUG("\r\n xxtask_start:%d", taskId);
	APP_DEBUG("\r\n xxxxxxtask_start:%d", taskId);
	}
	//m_task_evnt.TaskEvent1 = Ql_OS_CreateEvent("\r\n subtask1_gpio");
	//m_task_evnt.TaskEvent2 = Ql_OS_CreateEvent("\r\n task1");

	//while (Ql_OS_WaitEvent(m_task_evnt.TaskEvent1, EVENT_FLAG0) != 0) {

	//}

	//APP_DEBUG("<-- default-->\r\n");



}



///////////////////////////////////////////

