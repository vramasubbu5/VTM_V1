/*
 * task2.h
 *
 *  Created on: Jun 25, 2016
 *      Author: Elangovan
 */

#ifndef TASK2_H_
#define TASK2_H_

#include "ql_gprs.h"
#include "ql_socket.h"

#define  TASK_CALL_TIMER     1000
#define TCP_TIMER_PERIOD     500    // changed for testing previously it was 90 seconds
#define TIMEOUT_90S_PERIOD   20000  // changed for testing previously it was 90 seconds
#define FILE_WRITE_PERIOD     100

#define RECV_BUFFER_LEN       500
#define TX_BUFFER_LEN         1000

/******************************************************************************
 * Define APP Enum
 ******************************************************************************/
typedef enum {
	STATE_NW_GET_SIMSTATE,
	STATE_NW_QUERY_STATE,
	STATE_GPRS_REGISTER,
	STATE_GPRS_CONFIG,
	STATE_GPRS_ACTIVATE,
	STATE_GPRS_ACTIVATING,
	STATE_GPRS_GET_DNSADDRESS,
	STATE_GPRS_GET_LOCALIP,
	STATE_CHACK_SRVADDR,
	STATE_SOC_REGISTER,
	STATE_SOC_CREATE,
	STATE_SOC_CONNECT,
	STATE_SOC_CONNECTING,
	STATE_SOC_SEND,
	STATE_SOC_SENDING,
	STATE_SOC_ACK,
	STATE_SOC_CLOSE,
	STATE_GPRS_DEACTIVATE,
	STATE_TOTAL_NUM
} Enum_TCPSTATE;
u8 m_tcp_state;

ST_GprsConfig m_gprsCfg;

/******************************************************************************
 * Define APP Structure
 ******************************************************************************/
typedef struct {

	//bool bstart_task2;
	u8 m_ipaddress[5];  //only save the number of server ip, remove the comma
	u64 m_nSentLen;
	u8 u8tcpStart_RstCnt;

//u8 m_apn[APN_NAME] ;

} TCP_INFO;

typedef struct _INTERVALS_ {

	unsigned short us_polling;
	unsigned short us_transmit;

} s_interval;

TCP_INFO m_tcp_info;

//s32 timeout_90S_monitor;

typedef struct _GPRS_PTR_DATA_ {
	u8 * u8ptrindata;
	u8 * u8ptroutdata;
	u16 * u16inLength;
	u16 * u16outLength;
} s_gprs_dataptr;

typedef struct _GPRS_PTR_CONFIG_ {
	u8 * u8ptrapn;
	u8 * u8ptrurl;
	u16 * u16portNo;
} s_gprs_configptr;

typedef struct _GPRS_DATA_ {
	u8 u8txvector[TX_BUFFER_LEN];
	u8 u8rxvector[RECV_BUFFER_LEN];
	u16 u16txlength;
	u16 u16rxlength;
} s_gprs_data;

typedef struct _GPRS_CONFIG_ {
	u8 u8apnvector[25];
	u8 u8url[50];
	u16 u16portno;
} s_gprs_config;

/*****************************************************************
 * GPRS and socket callback function
 ******************************************************************/
void callback_socket_connect(s32 socketId, s32 errCode, void* customParam);
void callback_socket_close(s32 socketId, s32 errCode, void* customParam);
void callback_socket_accept(s32 listenSocketId, s32 errCode, void* customParam);
void callback_socket_read(s32 socketId, s32 errCode, void* customParam);
void callback_socket_write(s32 socketId, s32 errCode, void* customParam);

void Callback_GPRS_Actived(u8 contexId, s32 errCode, void* customParam);
void CallBack_GPRS_Deactived(u8 contextId, s32 errCode, void* customParam);
void Callback_GetIpByName(u8 contexId, u8 requestId, s32 errCode, u32 ipAddrCnt,
		u32* ipAddr);
void checkErr_AckNumber(s32 err_code);

#endif /* TASK2_H_ */
