/*
 * task2.c
 *
 *  Created on: Jun 24, 2016
 *      Author: Amruta
 */
/***************************************************************************/
/*This task includes all GPRS transmission process.                        */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
#include "gpio.h"
#include "task2.h"
#include "system.h"
#include "task3.h"
#include "global.h"
/*
 *
 * ************** Macros ***************
 *
 */

/*
 *
 * ************** function prototypes ***************
 *
 */
//#ifdef NOTUSED
static void TCP_Timer_handler(u32 timerId, void* param);
static char Tcp_Initialize(void);
static void GPRS_Fail_Ack(u8 u8State);
/*
 *
 * ************** Object declaration ****************
 *
 */

s_gprs_data st_gprs_data;

s_gprs_config st_gprsconfig;

s_gprs_dataptr st_gprsdata = {

st_gprs_data.u8txvector, st_gprs_data.u8rxvector, &st_gprs_data.u16txlength,
		&st_gprs_data.u16rxlength };

static s_gprs_configptr st_config = {

st_gprsconfig.u8apnvector, st_gprsconfig.u8url, &st_gprsconfig.u16portno };

static ST_PDPContxt_Callback callback_gprs_func = { Callback_GPRS_Actived,
		CallBack_GPRS_Deactived };

static ST_SOC_Callback callback_soc_func = { callback_socket_connect,
		callback_socket_close, callback_socket_accept, callback_socket_read,
		callback_socket_write };

static s32 ret;

static s32 m_socketid = -1;

static s32 timeout_90S_monitor;

extern s_system_info st_sys;

extern TASK_EVENTS m_task_evnt;

extern s_ufs_data_buff st_ufs_data_buff;
extern s_ufs_data_ptrbuff st_ufsdata;
extern s_globflags st_globflags;
extern s_glob_var st_glob_var;
/*
 *
 *
 *
 */

char Tcp_Initialize(void)

{

	s32 sReturn = 0;
	/*
	 if (st_gprsdata.u16inLength == NULL || st_gprsdata.u16outLength == NULL
	 || st_gprsdata.u8ptrindata == NULL
	 || st_gprsdata.u8ptroutdata == NULL)

	 return -1;
	 */
	if (st_config.u16portNo == NULL || st_config.u8ptrapn == NULL
			|| st_config.u8ptrurl == NULL)

		return -2;

	sReturn = Ql_Timer_Register(TCP_TIMER_ID, TCP_Timer_handler, NULL);

	if (sReturn != QL_RET_OK)

		return -3;

	sReturn = Ql_Timer_Register(TIMEOUT_90S_TIMER_ID, TCP_Timer_handler,
	NULL);

	if (sReturn != QL_RET_OK)

		return -4;

	return QL_RET_OK;

}

/*
 *
 * ************** Main task starts ***************
 *
 */

void proc_main_task2(s32 taskId) {

	ST_MSG msg3;

	bool bTaskStart = FALSE;

	int iRet = 0;

	char sc_return = 0;

	m_task_evnt.TaskEvent2 = Ql_OS_CreateEvent("\r\n task1");

	while (Ql_OS_WaitEvent(m_task_evnt.TaskEvent2, EVENT_FLAG0) != 0) {

	}

	sc_return = Tcp_Initialize();

	if (sc_return == QL_RET_OK) {

		APP_DEBUG("\r\n Tcp_Initialization_Success ");

		s32 s32Return = 0;

		s32Return = Ql_GPRS_Register(0, &callback_gprs_func, NULL);

		if (s32Return != GPRS_PDP_SUCCESS) {

			// Send message to main task task2 Initialization fail

		}

		Ql_OS_SetEvent(m_task_evnt.TaskEvent3, EVENT_FLAG0);

		bTaskStart = TRUE;

	} else {

		// Send message to main task task2 Initialization fail
		Ql_OS_SendMessage(0, MSG_ID_TASK_INIT_DATA, TASK2, 1);

		APP_DEBUG("\r\n Tcp_Initialization_Failure ");

	}

	timeout_90S_monitor = FALSE;

	Ql_strcpy((char*) st_gprsconfig.u8apnvector, "internet"); //added for testing
	Ql_strcpy((char*) st_gprsconfig.u8url, "67.222.136.154"); //added for testing
	st_gprsconfig.u16portno = 80;                            //added for testing
	m_tcp_info.u8tcpStart_RstCnt = 0;

	while (bTaskStart) {

		APP_DEBUG("OpenCPU: Task2\r\n");

		Ql_OS_GetMessage(&msg3);

		switch (msg3.message) {

		case MSG_ID_USER_DATA:

			//iRet = Ql_OS_SendMessage(0, MSG_ID_RIL_READY, msg3.param1,
			// msg3.param2);
			APP_DEBUG("OpenCPU: Task2_MSG_ID_USER_DATA\r\n")
			;
			switch (msg3.param1) {

			case 1:

				Ql_Timer_Start(TCP_TIMER_ID, TCP_TIMER_PERIOD, TRUE); // start the TCP timer

				m_tcp_state = STATE_NW_GET_SIMSTATE;
				if (iRet < 0)

				{

					APP_DEBUG(
							"\r\n<--failed!!, Ql_OS_SendMessage4(1, %d, %d) fail,  ret=%d-->\r\n",
							msg3.param1, msg3.param2, iRet);

				}

				APP_DEBUG(
						"\r\n<--Ql_OS_SendMes_Task4(%d, %d, %d) ret=%d-->\r\n",
						MSG_ID_RIL_READY, msg3.param1, msg3.param2, iRet)
				;
				break;
			default:
				break;
			}

			break;

		default:
			APP_DEBUG("<-- default task2 -->\r\n")
			;
			break;

		}

	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void TCP_Timer_handler(u32 timerId, void* param) {

	if (TIMEOUT_90S_TIMER_ID == timerId) {

		APP_DEBUG("<--90s time out!!!-->\r\n");
		APP_DEBUG("<-- Close socket.-->\r\n");

		Ql_SOC_Close(m_socketid);

		m_socketid = -1;
		timeout_90S_monitor = FALSE;
		/*
		 if (timeout_90S_monitor == FALSE) {
		 APP_DEBUG("<--GPRS_ACK_TIMEOUT-->\r\n");
		 st_glob_var.u8GprsAckState = 11;
		 GPRS_Fail_Ack(st_glob_var.u8GprsAckState);
		 }
		 */ //it is disable for testing.It is disable to avoid acknowledgment from GPRS
		m_tcp_state = STATE_GPRS_DEACTIVATE;

	} else if (TCP_TIMER_ID == timerId) {
		//APP_DEBUG("<--...........m_tcp_state=%d..................-->\r\n",m_tcp_state);

		//APP_DEBUG("<--sentlength:%llu", m_tcp_info.m_nSentLen);

		switch (m_tcp_state) {
//here check the sim signal strength
		case STATE_NW_GET_SIMSTATE: {

			APP_DEBUG("<--STATE_NW_GET_SIMSTATE-->\r\n");

			u32 ber = 0;

			RIL_NW_GetSignalQuality(&st_glob_var.u32_signal_strength, &ber);

			APP_DEBUG("<-- Signal strength:%d, BER:%d -->\r\n",
					st_glob_var.u32_signal_strength, ber);

			if (st_glob_var.u32_signal_strength >= 15) {
				//m_tcp_state = STATE_NW_QUERY_STATE;
				//m_tcp_state = STATE_GPRS_REGISTER;

				m_tcp_state = STATE_GPRS_CONFIG;
				APP_DEBUG("<--SIM card status is normal!-->\r\n");
			} else {
				//    Ql_Timer_Stop(TCP_TIMER_ID);
				APP_DEBUG("<--SIM card status is unnormal!-->\r\n");
				st_glob_var.u8GprsAckState = 1;
				GPRS_Fail_Ack(st_glob_var.u8GprsAckState);
			}
			break;
		}

		case STATE_GPRS_CONFIG: {
			//
			APP_DEBUG("<--STATE_GPRS_CONFIG-->\r\n");

			if (st_config.u8ptrapn != NULL) {

				Ql_strcpy((char*) m_gprsCfg.apnName,
						(const char*) st_config.u8ptrapn);
			}

			APP_DEBUG("\r\n APN_NAME:");
			APP_DEBUG((const char* )m_gprsCfg.apnName);

			Ql_memset(m_gprsCfg.apnUserId, 0x0, sizeof(m_gprsCfg.apnUserId));

			Ql_memset(m_gprsCfg.apnPasswd, 0x0, sizeof(m_gprsCfg.apnPasswd));

			m_gprsCfg.authtype = 0;

			ret = Ql_GPRS_Config(0, &m_gprsCfg);

			if (GPRS_PDP_SUCCESS == ret) {
				APP_DEBUG("<--configure GPRS param successfully.-->\r\n");
			} else {
				APP_DEBUG("<--configure GPRS param failure,ret=%d.-->\r\n", ret);
				st_glob_var.u8GprsAckState = 2;
				GPRS_Fail_Ack(st_glob_var.u8GprsAckState);
			}

			m_tcp_state = STATE_GPRS_ACTIVATE;
			//m_tcp_state = STATE_GPRS_GET_DNSADDRESS;
			break;
		}
		case STATE_GPRS_ACTIVATE: {
			m_tcp_state = STATE_GPRS_ACTIVATING;
			ret = Ql_GPRS_Activate(0);
			if (ret == GPRS_PDP_SUCCESS) {
				APP_DEBUG("<--Activate GPRS successfully.-->\r\n");
				m_tcp_state = STATE_GPRS_GET_DNSADDRESS;
			} else if (ret == GPRS_PDP_WOULDBLOCK) {
				APP_DEBUG(
						"<--Waiting for the result of GPRS activated.,ret=%d.-->\r\n",
						ret);
				//waiting Callback_GPRS_Actived
			} else if (ret == GPRS_PDP_ALREADY) {
				APP_DEBUG("<--GPRS has already been activated,ret=%d.-->\r\n",
						ret);
				m_tcp_state = STATE_GPRS_GET_DNSADDRESS;
			} else				//error
			{
				APP_DEBUG("<--Activate GPRS failure,ret=%d.-->\r\n", ret);
				st_glob_var.u8GprsAckState = 3;
				GPRS_Fail_Ack(st_glob_var.u8GprsAckState);
				m_tcp_state = STATE_GPRS_ACTIVATE;
				//here add timeout latter
			}
			break;
		}
		case STATE_GPRS_GET_DNSADDRESS: {

			APP_DEBUG("<--STATE_GPRS_GET_DNSADDRESS-->\r\n");

			u8 primaryAddr[16] = { 0 };
			u8 bkAddr[16] = { 0 };
			ret = Ql_GPRS_GetDNSAddress(0, (u32*) primaryAddr, (u32*) bkAddr);
			if (ret == GPRS_PDP_SUCCESS) {
				APP_DEBUG(
						"<--Get DNS address successfully,primaryAddr=%d.%d.%d.%d,bkAddr=%d.%d.%d.%d-->\r\n",
						primaryAddr[0], primaryAddr[1], primaryAddr[2],
						primaryAddr[3], bkAddr[0], bkAddr[1], bkAddr[2],
						bkAddr[3]);
				m_tcp_state = STATE_GPRS_GET_LOCALIP;
			} else {
				APP_DEBUG("<--Get DNS address failure,ret=%d.-->\r\n", ret);
				//st_globflags.bGprsDataSendError = TRUE;
				st_glob_var.u8GprsAckState = 4;
				GPRS_Fail_Ack(st_glob_var.u8GprsAckState);
				m_tcp_state = STATE_GPRS_DEACTIVATE;
			}
			break;
		}
		case STATE_GPRS_GET_LOCALIP: {

			APP_DEBUG("<--STATE_GPRS_GET_LOCALIP-->\r\n");

			u8 ip_addr[5];
			Ql_memset(ip_addr, 0, 5);
			ret = Ql_GPRS_GetLocalIPAddress(0, (u32 *) ip_addr);
			if (ret == GPRS_PDP_SUCCESS) {
				APP_DEBUG(
						"<--Get Local Ip successfully,Local Ip=%d.%d.%d.%d-->\r\n",
						ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
				m_tcp_state = STATE_CHACK_SRVADDR;
			} else {
				APP_DEBUG("<--Get Local Ip failure,ret=%d.-->\r\n", ret);
				st_glob_var.u8GprsAckState = 5;
				GPRS_Fail_Ack(st_glob_var.u8GprsAckState);
			}
			break;
		}
		case STATE_CHACK_SRVADDR: {

			APP_DEBUG("<--STATE_CHACK_SRVADDR-->\r\n");

			Ql_memset(m_tcp_info.m_ipaddress, 0, 5);

			if (st_config.u8ptrurl != NULL) {
				ret = Ql_IpHelper_ConvertIpAddr((u8*) st_config.u8ptrurl,
						(u32 *) m_tcp_info.m_ipaddress);
			}

			if (ret == SOC_SUCCESS) // ip address, xxx.xxx.xxx.xxx
					{
				APP_DEBUG(
						"<--Convert Ip Address successfully,m_ipaddress=%d,%d,%d,%d-->\r\n",
						m_tcp_info.m_ipaddress[0], m_tcp_info.m_ipaddress[1],
						m_tcp_info.m_ipaddress[2], m_tcp_info.m_ipaddress[3]);
				m_tcp_state = STATE_SOC_REGISTER;

			} else  //domain name
			{
				if (st_config.u8ptrurl != NULL) {
					ret = Ql_IpHelper_GetIPByHostName(0, 0,
							(u8*) st_config.u8ptrurl, Callback_GetIpByName);
				}
				if (ret == SOC_SUCCESS) {
					APP_DEBUG("<--Get ip by hostname successfully.-->\r\n");
				} else if (ret == SOC_WOULDBLOCK) {
					APP_DEBUG(
							"<--Waiting for the result of Getting ip by hostname,ret=%d.-->\r\n",
							ret);
					//waiting CallBack_getipbyname

				} else {
					APP_DEBUG("<--Get ip by hostname failure:ret=%d-->\r\n",
							ret);
					if (ret == SOC_BEARER_FAIL) {
						//st_globflags.bGprsDataSendError = TRUE;
						st_glob_var.u8GprsAckState = 6;
						GPRS_Fail_Ack(st_glob_var.u8GprsAckState);
						m_tcp_state = STATE_GPRS_DEACTIVATE;
					}

				}
			}
			break;
		}
		case STATE_SOC_REGISTER: {

			APP_DEBUG("<--STATE_SOC_REGISTER-->\r\n");

			ret = Ql_SOC_Register(callback_soc_func, NULL);
			if (SOC_SUCCESS == ret) {
				APP_DEBUG(
						"<--Register socket callback function successfully.-->\r\n");
				m_tcp_state = STATE_SOC_CREATE;
			} else if (SOC_ALREADY == ret) {
				APP_DEBUG(
						"<--Socket callback function has already been registered,ret=%d.-->\r\n",
						ret);
				m_tcp_state = STATE_SOC_CREATE;
			} else {
				APP_DEBUG(
						"<--Register Socket callback function failure,ret=%d.-->\r\n",
						ret);
				st_glob_var.u8GprsAckState = 7;
				GPRS_Fail_Ack(st_glob_var.u8GprsAckState);
			}
			break;
		}
		case STATE_SOC_CREATE: {

			APP_DEBUG("<--STATE_SOC_CREATE-->\r\n");

			m_socketid = Ql_SOC_Create(0, SOC_TYPE_TCP);
			if (m_socketid >= 0) {
				APP_DEBUG(
						"<--Create socket id successfully,socketid=%d.-->\r\n",
						m_socketid);
				m_tcp_state = STATE_SOC_CONNECT;
			} else {
				APP_DEBUG("<--Create socket id failure,error=%d.-->\r\n",
						m_socketid);
				st_glob_var.u8GprsAckState = 8;
				GPRS_Fail_Ack(st_glob_var.u8GprsAckState);
			}
			break;
		}
		case STATE_SOC_CONNECT: {

			APP_DEBUG("<--STATE_SOC_CONNECT-->\r\n");

			m_tcp_state = STATE_SOC_CONNECTING;
			if (st_config.u16portNo != NULL) {
				ret = Ql_SOC_Connect(m_socketid, (u32) m_tcp_info.m_ipaddress,
						*(st_config).u16portNo);
			}
			if (ret == SOC_SUCCESS) {
				APP_DEBUG("<--The socket is already connected.-->\r\n");
				m_tcp_state = STATE_SOC_SEND;

			} else if (ret == SOC_WOULDBLOCK) {
				if (!timeout_90S_monitor)  //start timeout monitor
				{
					Ql_Timer_Start(TIMEOUT_90S_TIMER_ID, TIMEOUT_90S_PERIOD,
					FALSE);
					timeout_90S_monitor = TRUE;
				}
				APP_DEBUG(
						"<--Waiting for the result of socket connection,ret=%d.-->\r\n",
						ret);
				//waiting CallBack_getipbyname

			} else //error
			{
				APP_DEBUG("<--Socket Connect failure,ret=%d.-->\r\n", ret);
				APP_DEBUG("<-- Close socket.-->\r\n");
				Ql_SOC_Close(m_socketid);
				m_socketid = -1;

				if (ret == SOC_BEARER_FAIL) {
					m_tcp_state = STATE_GPRS_DEACTIVATE;
				}
				//st_globflags.bGprsDataSendError = TRUE;
				st_glob_var.u8GprsAckState = 9;
				GPRS_Fail_Ack(st_glob_var.u8GprsAckState);
			}
			break;
		}
		case STATE_SOC_SEND: {

			APP_DEBUG("<--STATE_SOC_SEND-->\r\n");

			if (st_gprsdata.u16inLength != NULL
					&& st_gprsdata.u8ptrindata != NULL) {

				if (!Ql_strlen((const char*) st_gprsdata.u8ptrindata)) //no data need to send
					break;
				//	}
				m_tcp_state = STATE_SOC_SENDING;

				s32 s32_datalength = 0;

				do {

					//ret = Ql_SOC_Send(m_socketid, m_tcp_info.m_pCurrentPos,
					//		u16txlength);

					s32_datalength = *(st_gprsdata).u16inLength;

					APP_DEBUG((const char* )st_gprsdata.u8ptrindata);

					ret = Ql_SOC_Send(m_socketid, st_gprsdata.u8ptrindata,
							s32_datalength);

					APP_DEBUG(
							"<--Send data,socketid=%d,number of bytes sent=%d-->\r\n",
							m_socketid, ret);

					if (ret == s32_datalength)					//send compelete
							{
						APP_DEBUG("<--Send_success-->\r\n");
						s32_datalength = 0;
						//	m_tcp_info.m_pCurrentPos = NULL;
						//  st_gprsdata.u8ptrindata = NULL;
						m_tcp_info.m_nSentLen += ret;
////////////////////////////////////////
						//m_tcp_state = STATE_SOC_ACK; // disable for testing for continue transmitting
						if (timeout_90S_monitor) //stop timeout monitor

						{

							Ql_Timer_Stop(TIMEOUT_90S_TIMER_ID);

							timeout_90S_monitor = FALSE;

						}
						st_glob_var.u8GprsAckState = 255;
						GPRS_Fail_Ack(st_glob_var.u8GprsAckState);
						Ql_SOC_Close(m_socketid);

						m_socketid = -1;
						m_tcp_state = STATE_GPRS_DEACTIVATE;
///////////////////////////////////////////////////// added above for testing for continue transmitting
						*(st_gprsdata).u16inLength = s32_datalength;

						break;
					} else if ((ret <= 0) && (ret == SOC_WOULDBLOCK)) {
						//waiting CallBack_socket_write, then send data;
						break;
					} else if ((ret <= 0) || (ret == SOC_BEARER_FAIL)) {
						APP_DEBUG("<--Send data failure,ret=%d.-->\r\n", ret);
						APP_DEBUG("<-- Close socket.-->\r\n");
						Ql_SOC_Close(m_socketid); //error , Ql_SOC_Close
						m_socketid = -1;

						s32_datalength = 0;
						//m_tcp_info.m_pCurrentPos = NULL;
						//st_gprsdata.u8ptrindata = NULL;
						//write data in file
						st_glob_var.u8GprsAckState = 10;
						GPRS_Fail_Ack(st_glob_var.u8GprsAckState);
						//	st_globflags.bGprsDataSendError = TRUE;

						*(st_gprsdata).u16inLength = s32_datalength;

						m_tcp_state = STATE_GPRS_DEACTIVATE;

						break;
					} else if (ret < st_gprs_data.u16txlength) //continue send, do not send all data
							{
						s32_datalength -= ret;
						//	m_tcp_info.m_pCurrentPos += ret;
						st_gprsdata.u8ptrindata += ret;
						m_tcp_info.m_nSentLen += ret;
						*(st_gprsdata).u16inLength = s32_datalength;
					}
				} while (1);

			}

			break;
		}
		case STATE_SOC_ACK: {

			APP_DEBUG("<--STATE_SOC_ACK-->\r\n");

			u64 ackedNumCurr;

			ret = Ql_SOC_GetAckNumber(m_socketid, &ackedNumCurr);

			if (ret < 0) {
				checkErr_AckNumber(ret);
			}

			if (m_tcp_info.m_nSentLen == ackedNumCurr)

			{

				if (timeout_90S_monitor) //stop timeout monitor

				{

					Ql_Timer_Stop(TIMEOUT_90S_TIMER_ID);

					timeout_90S_monitor = FALSE;

				}
				st_glob_var.u8GprsAckState = 255;
				GPRS_Fail_Ack(st_glob_var.u8GprsAckState);
				APP_DEBUG(
						"<-- ACK Number:%llu/%llu. Server has received all data. -->\r\n\r\n",
						m_tcp_info.m_nSentLen, ackedNumCurr);
				//	if (st_gprsdata.u8ptroutdata != NULL) {
				//		APP_DEBUG((const char* )st_gprsdata.u8ptroutdata);
				//	}
				//////////////////////////////////////////////
				if (st_gprsdata.u8ptrindata != NULL) {

					Ql_memset(st_gprsdata.u8ptrindata, 0,
					/*(u32) sizeof(st_gprsdata.u8ptrindata)*/TX_BUFFER_LEN);
				}
/////////////////////////////////////////disable by amruta for testing
				m_tcp_state = STATE_SOC_SEND;

				//Ql_SOC_Close(m_socketid);

				//m_socketid = -1;

				//m_tcp_state = STATE_GPRS_DEACTIVATE;
			} else {
				if (!timeout_90S_monitor) //start timeout monitor
				{
					Ql_Timer_Start(TIMEOUT_90S_TIMER_ID, TIMEOUT_90S_PERIOD,
					FALSE);
					timeout_90S_monitor = TRUE;

				}
				//st_glob_var.u8GprsAckState = 11;
				//GPRS_Fail_Ack(st_glob_var.u8GprsAckState);
				APP_DEBUG("<-- ACK Number:%llu/%llu from socket[%d] -->\r\n",
						ackedNumCurr, m_tcp_info.m_nSentLen, m_socketid);

			}
			break;
		}
		case STATE_GPRS_DEACTIVATE: {

			APP_DEBUG("<--STATE_GPRS_DEACTIVATE-->\r\n");
			//Ql_GPRS_Deactivate(0);
			//m_tcp_state = STATE_NW_GET_SIMSTATE;
			//if(st_gprsdata.u8ptroutdata != NULL){
			//Ql_OS_SendMessage(0, MSG_ID_USER_DATA, 1,
			//		(u32)u8rxvector);
			//}

			s32 ret;
			ret = Ql_GPRS_Deactivate(0);
			if (GPRS_PDP_SUCCESS == ret) {
				APP_DEBUG("<--GPRS_DEACTIVATE_SUCCESSEFULLY-->\r\n");
			} else if (GPRS_PDP_WOULDBLOCK == ret) {
				APP_DEBUG(
						"<--Waiting for the result of deactivating gprs connection-->\r\n");
			} else {
				APP_DEBUG("<--GPRS_DEACTIVATE_ERROR-->\r\n");
			}

			//Ql_OS_SendMessage(0, MSG_ID_USER_DATA, TCP_TASK, 1);
			Ql_Timer_Stop(TCP_TIMER_ID);  // added for testing by amruta

			break;
		}
		default:
			break;
		}
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void checkErr_AckNumber(s32 err_code) {
	if (SOC_INVALID_SOCKET == err_code) {
		APP_DEBUG("<-- Invalid socket ID -->\r\n");
	} else if (SOC_INVAL == err_code) {
		APP_DEBUG("<-- Invalid parameters for ACK number -->\r\n");
	} else if (SOC_ERROR == err_code) {
		APP_DEBUG("<-- Unspecified error for ACK number -->\r\n");
	} else {
		// get the socket option successfully
	}
}

void Callback_GPRS_Actived(u8 contexId, s32 errCode, void* customParam) {
	if (errCode == SOC_SUCCESS) {
		APP_DEBUG("<--CallBack: active GPRS successfully.-->\r\n");
		//	m_tcp_state = STATE_GPRS_GET_DNSADDRESS;
		m_tcp_state = STATE_NW_GET_SIMSTATE;  // changed by amruta

	} else {
		APP_DEBUG("<--CallBack: active GPRS successfully,errCode=%d-->\r\n",
				errCode);
		m_tcp_state = STATE_GPRS_ACTIVATE;
		//Ql_GPRS_Activate(0);
	}
}

void Callback_GetIpByName(u8 contexId, u8 requestId, s32 errCode, u32 ipAddrCnt,
		u32* ipAddr) {
	u8 i = 0;
	u8* ipSegment = (u8*) ipAddr;

	APP_DEBUG("<-- %s:contexid=%d, requestId=%d,error=%d,num_entry=%d -->\r\n",
			__func__, contexId, requestId, errCode, ipAddrCnt);
	if (errCode == SOC_SUCCESS) {
		APP_DEBUG("<--CallBack: get ip by name successfully.-->\r\n");
		for (i = 0; i < ipAddrCnt; i++) {
			ipSegment = (u8*) (ipAddr + i);
			APP_DEBUG("<--Entry=%d, ip=%d.%d.%d.%d-->\r\n", i, ipSegment[0],
					ipSegment[1], ipSegment[2], ipSegment[3]);
		}

		// Fetch the first ip address as the valid IP
		Ql_memcpy(m_tcp_info.m_ipaddress, ipAddr, 4);
		m_tcp_state = STATE_SOC_REGISTER;
	}
}

void callback_socket_connect(s32 socketId, s32 errCode, void* customParam) {
	if (errCode == SOC_SUCCESS) {
		if (timeout_90S_monitor) //stop timeout monitor
		{
			Ql_Timer_Stop(TIMEOUT_90S_TIMER_ID);
			timeout_90S_monitor = FALSE;
		}

		APP_DEBUG("<--Callback: socket connect successfully.-->\r\n");
		m_tcp_state = STATE_SOC_SEND;

	} else {
		APP_DEBUG(
				"<--Callback: socket connect failure,(socketId=%d),errCode=%d-->\r\n",
				socketId, errCode);
		Ql_SOC_Close(socketId);
		m_tcp_state = STATE_SOC_CREATE;
	}
}

void callback_socket_close(s32 socketId, s32 errCode, void* customParam) {

	m_tcp_info.m_nSentLen = 0;

	if (errCode == SOC_SUCCESS) {
		APP_DEBUG("<--CallBack: close socket successfully.-->\r\n");
	} else if (errCode == SOC_BEARER_FAIL) {
		m_tcp_state = STATE_GPRS_DEACTIVATE;
		APP_DEBUG(
				"<--CallBack: close socket failure,(socketId=%d,error_cause=%d)-->\r\n",
				socketId, errCode);
	} else {
		//	m_tcp_state = STATE_GPRS_GET_DNSADDRESS;   //disable by amruta
		APP_DEBUG(
				"<--CallBack: close socket failure,(socketId=%d,error_cause=%d)-->\r\n",
				socketId, errCode);
		m_tcp_state = STATE_GPRS_DEACTIVATE; // added for testing by amruta
	}
}

void callback_socket_accept(s32 listenSocketId, s32 errCode, void* customParam) {
}

void callback_socket_read(s32 socketId, s32 errCode, void* customParam) {
	s32 ret = 0;
	s32 s32Rxdatatlength = 0;

	if (errCode) {
		APP_DEBUG("<--CallBack: socket read failure,(sock=%d,error=%d)-->\r\n",
				socketId, errCode);
		APP_DEBUG("<-- Close socket.-->\r\n");
		Ql_SOC_Close(socketId);
		m_socketid = -1;
		if (errCode == SOC_BEARER_FAIL) {
			m_tcp_state = STATE_GPRS_DEACTIVATE;
		}
		//else {
		//	m_tcp_state = STATE_GPRS_GET_DNSADDRESS;  //disable by amruta
		//}
		return;
	}
	s32Rxdatatlength = *(st_gprsdata).u16outLength;
	if (st_gprsdata.u8ptroutdata != NULL) {
		Ql_memset(st_gprsdata.u8ptroutdata, 0,
		/*sizeof(st_gprsdata.u8ptroutdata)*/RECV_BUFFER_LEN);

	}
	//Ql_memset(u8rxvector, 0, RECV_BUFFER_LEN);

	do {
		if (st_gprsdata.u8ptroutdata != NULL) {

			//ret = Ql_SOC_Recv(socketId, u8rxvector, RECV_BUFFER_LEN);
			ret = Ql_SOC_Recv(socketId, st_gprsdata.u8ptroutdata,
			/*sizeof(st_gprsdata.u8ptroutdata)*/RECV_BUFFER_LEN);
		}

		if ((ret < 0) && (ret != -2)) {
			APP_DEBUG("<-- Receive data failure,ret=%d.-->\r\n", ret);
			APP_DEBUG("<-- Close socket.-->\r\n");
			Ql_SOC_Close(socketId); //you can close this socket
			m_socketid = -1;
			m_tcp_state = STATE_SOC_CREATE;
			break;
		} else if (ret == -2) {
			//wait next CallBack_socket_read

			break;
		} else if (ret < RECV_BUFFER_LEN) {

			APP_DEBUG("<--Receive data from sock(%d),len(%d):%s\r\n", socketId,
					ret);

			st_gprsdata.u8ptroutdata[ret] = '\0';

			APP_DEBUG((const char* )st_gprsdata.u8ptroutdata);

			break;
		} else if (ret == RECV_BUFFER_LEN) {
			APP_DEBUG("<--Receive data from sock(%d),len(%d):%s\r\n", socketId,
					ret, st_gprsdata.u8ptroutdata);
			st_gprsdata.u8ptroutdata[ret] = '\0';   ////////////

			APP_DEBUG((const char* )st_gprsdata.u8ptroutdata);   ///////////////

		}
	} while (1);

}

void callback_socket_write(s32 socketId, s32 errCode, void* customParam) {
	s32 ret = 0;
	s32 s32_datalength = 0;
	if (errCode) {
		APP_DEBUG("<--CallBack: socket write failure,(sock=%d,error=%d)-->\r\n",
				socketId, errCode);
		APP_DEBUG("<-- Close socket.-->\r\n");
		Ql_SOC_Close(socketId);
		m_socketid = -1;

		if (ret == SOC_BEARER_FAIL) {
			m_tcp_state = STATE_GPRS_DEACTIVATE;
		}
		//else {
		//	m_tcp_state = STATE_GPRS_GET_DNSADDRESS;   //disable by amruta
		//	}
		return;
	}

	m_tcp_state = STATE_SOC_SENDING;

	do {
		if (st_gprsdata.u16inLength != NULL && st_gprsdata.u8ptrindata != NULL) {
			s32_datalength = *(st_gprsdata).u16inLength;
			ret = Ql_SOC_Send(m_socketid, st_gprsdata.u8ptrindata,
					s32_datalength);
		}
		APP_DEBUG(
				"<--CallBack: Send data,socketid=%d,number of bytes sent=%d-->\r\n",
				m_socketid, ret);

		if (ret == s32_datalength) //send compelete
				{
			s32_datalength = 0;
			//st_gprsdata.u8ptrindata = NULL;
			m_tcp_info.m_nSentLen += ret;
			m_tcp_state = STATE_SOC_ACK;
			*(st_gprsdata).u16inLength = s32_datalength;
			break;
		} else if ((ret < 0) && (ret == SOC_WOULDBLOCK)) {
			//you must wait CallBack_socket_write, then send data;
			break;
		} else if (ret < 0) {
			APP_DEBUG("<--Send data failure,ret=%d.-->\r\n", ret);
			APP_DEBUG("<-- Close socket.-->\r\n");
			Ql_SOC_Close(socketId); //error , Ql_SOC_Close
			m_socketid = -1;

			s32_datalength = 0;
			//st_gprsdata.u8ptrindata = NULL;
			*(st_gprsdata).u16inLength = s32_datalength;
			if (ret == SOC_BEARER_FAIL) {
				m_tcp_state = STATE_GPRS_DEACTIVATE;
			}
			//else {
			//	m_tcp_state = STATE_GPRS_GET_DNSADDRESS;  //disable by amruta
			//}
			break;
		} else if (ret < st_gprs_data.u16txlength) //continue send, do not send all data
				{
			s32_datalength -= ret;
			st_gprsdata.u8ptrindata += ret;
			m_tcp_info.m_nSentLen += ret;
			*(st_gprsdata).u16inLength = s32_datalength;
		}
	} while (1);
}

void CallBack_GPRS_Deactived(u8 contextId, s32 errCode, void* customParam) {
	if (errCode == SOC_SUCCESS) {
		APP_DEBUG("<--CallBack: deactived GPRS successfully.-->\r\n");
		m_tcp_state = STATE_NW_GET_SIMSTATE;  //disable by amruta
	} else {
		APP_DEBUG(
				"<--CallBack: deactived GPRS failure,(contexid=%d,error_cause=%d)-->\r\n",
				contextId, errCode);
	}
}

//////////////////////////////////////////////////////////////////
static void GPRS_Fail_Ack(u8 u8State) {
	if (u8State == 255) {
		APP_DEBUG("<--Check_GPRS_Reply\r\n");
		st_globflags.bGprsDataSendProcess = FALSE;
	} else {
		st_globflags.bGprsDataSendProcess = FALSE;
		APP_DEBUG("<--GPRS_Tx_fail\r\n");
		if (st_glob_var.u8DataType == 2) {
			APP_DEBUG("<--GPRS_Tx_fail_1\r\n");
			ufs_data_ptr.u16ReadPtr = st_glob_var.u16OldUfsReadPtr;
			st_glob_var.u8DataType = 0;
			APP_DEBUG(
					"after_gprs_failUFS_Wrt_ptr:%d/Rd_Ptr:%d/OldRd_Ptr:%d!\r\n",
					ufs_data_ptr.u16WritePtr, ufs_data_ptr.u16ReadPtr,
					st_glob_var.u16OldUfsReadPtr);

		}

	}
}
//#endif
