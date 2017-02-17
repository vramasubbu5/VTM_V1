/*
 * uart1.h
 *
 *  Created on: May 30, 2016
 *      Author: Amruta
 */

#ifndef UART1_H_
#define UART1_H_

#include "ql_uart.h"

#define DEBUG_ENABLE 1
#if DEBUG_ENABLE > 0
#define DEBUG_PORT  UART_PORT1
#define DBG_BUF_LEN   512
static char DBG_BUFFER[DBG_BUF_LEN];

#define APP_DEBUG(FORMAT,...) {\
    Ql_memset(DBG_BUFFER, 0, DBG_BUF_LEN);\
    Ql_sprintf(DBG_BUFFER,FORMAT,##__VA_ARGS__); \
    if (UART_PORT2 == (DEBUG_PORT)) \
    {\
        Ql_Debug_Trace(DBG_BUFFER);\
    } else {\
        Ql_UART_Write((Enum_SerialPort)(DEBUG_PORT), (u8*)(DBG_BUFFER), Ql_strlen((const char *)(DBG_BUFFER)));\
    }\
}
#else
#define APP_DEBUG(FORMAT,...)
#endif

char Uart_Initialize(void);

bool Uart1Receive;
/***************************************************************************/
/*  Macros                      				                           */
/***************************************************************************/
#define GPS_TIME_BUFF      10
#define GPS_DATE_BUFF      10
#define GPS_LAT_BUFF      10
#define GPS_LON_BUFF      11
#define GPS_LATDIR_BUFF      2
#define GPS_LONDIR_BUFF      2
#define GPS_ALTI_BUFF      8

/***************************************************************************/
/*  Structure                   				                           */
/***************************************************************************/

typedef void (*Fptr_UART_CallBack)(Enum_SerialPort, Enum_UARTEventType, bool,
		void*);

typedef void (*Fptr_UART_Write)(Enum_SerialPort, Enum_UARTEventType, bool,
		void*);

typedef void (*Fptr_UART_Read)(Enum_SerialPort, Enum_UARTEventType, bool, void*);

typedef struct {
	Enum_SerialPort eport_no;

	u8 u8_receivebuff[1000];
	u8 u8_transmitbuff[255];
	bool u8_recived;
	bool u8_send;
	Fptr_UART_CallBack EvntCallBack;
	Fptr_UART_Write UartWrite;
	Fptr_UART_Read UartRead;

} UART;

UART uart[3];

typedef struct {
	u8 u8_Time[GPS_TIME_BUFF];      // 10 bytes
	u8 u8_gps_sts;                  // 1 byte
	u8 u8_string_sts;               //1 byte
	u8 u8_Date[GPS_DATE_BUFF];      // 10 bytes
	u8 u8_LatDir[GPS_LATDIR_BUFF];  // 2 bytes
	u8 u8_Lat[GPS_LAT_BUFF];        // 10 bytes
	u8 u8_LonDir[GPS_LONDIR_BUFF];  // 2 bytes
	u8 u8_lon[GPS_LON_BUFF];        // 11 bytes
	u8 u8_Speed;                    // 1 byte
	u8 u8_Alti[GPS_ALTI_BUFF];      // 8 bytes
} s_gps_info;

typedef struct {
	u16 u16_intBattPow;
	u16 u16_TempSens1;
	u16 u16_TempSens2;
	u16 u16_TempSens3;
	u16 u16_PresSens1;
	u16 u16_PresSens2;
	u8 u8_Ibutt[8];
} s_mc_info;

typedef struct {
	u8 u8Gps_fix_delay;
	u16 u16GPS_afterFix_Tmr;
} s_task1_cnt;

typedef struct {
	u8 u8Rmc[70];
	u8 u8Txt[30];
} s_gps_buff;
/***************************************************************************/
/*  Function Prototypes                                                    */
/***************************************************************************/

void CallBack_UART1_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg,
		bool level, void* customizedPara);

void CallBack_UART2_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg,
		bool level, void* customizedPara);

void CallBack_UART3_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg,
		bool level, void* customizedPara);

s32 ReadSerialPort(Enum_SerialPort port, /*[out]*/u8* pBuffer, /*[in]*/
u32 bufLen);
s32 ATResponse_Handler(Enum_SerialPort port, char* line, u32 len,
		void* userData);

#endif /* UART1_H_ */
