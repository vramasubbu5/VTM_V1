/*
 * system.h
 *
 *  Created on: Jun 24, 2016
 *      Author: Elangovan
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "ql_stdlib.h"
#include "uart1.h"

#define FM_VERSION 9001

/***************************************************************************/
/*  Global Macro Defines for RTC				                           */
/***************************************************************************/

#define GPS_STATUS_FAIL          0
#define GPS_STATUS_SUCCESS       1

#define MAX_WRITE_LIMIT_OF_RAM    2     //18
#define MAX_SIZE_OF_RAM           10    // 100

#define MAX_TX_ARRAY_LENGTH      1000
#define MAX_RX_ARRAY_LENGTH      200

/*
 ***************** Enum and Macro Declarations ***************
 */

typedef enum _TASK_IDS_ {

	MAIN_TASK = 0, UART_TASK = 3, TCP_TASK, FILE_TASK, FOTA_TASK, SMS_TASK

} e_taskids;

/*
 ***************** Enum and Macro Declarations ***************
 */

typedef struct {
	u8 u8_tuno[12];
	u8 u8_InitApplicationName[50];
	u8 u8_InitCompanyName[28];
	u8 u8_InitApplicationVersion[8];
	u8 u8_IMEINo[20];
	u8 u8_CoreVer[20];
	u8 u8_SDKVer[20];
	u8 u8_FmDate[10];
} s_system_info;

typedef struct {

	u32 TaskEvent1;
	u32 TaskEvent2;
	u32 TaskEvent3;
	u32 TaskEvent4;
	u32 TaskEvent5;

} TASK_EVENTS;

typedef struct {

} s_task_mutex;

typedef struct {

} s_task_semaphore;

typedef struct REC_HDR {

	u8 u8_STX[2];                                 //2 bytes
	u8 u8_RecordType;     //u8 u8_MessageType;    // 1 byte
	u8 u8_RecordQty;     //u8 u8_NoOfRecords;     // 1 byte
	long lDeviceID;     //long lSerialNo;         // 4 bytes
	u16 u16_TransactionID;                        // 2 bytes
	u16 u16_FmVarsion;                            // 2 bytes
	u16 u16_FlhWritePtr;                          // 2 bytes
	u16 u16_FlhReadPtr;                           // 2 bytes
	u8 u8_ExternalVolt;                           // 1 byte
	u8 u8_IntVolt;                                // 1 byte
	u8 u8_DataType;                               // 1 byte
} s_rec_hdr;

typedef struct {
	u8 u8_MsgID;          //1 byte
	u8 u8_RTC_Time[3];    //3 bytes
	u8 u8_RTC_Date[2];    //2 bytes
	u8 u8_Speed;          //1 byte
	u8 u8_Direction;      //1 byte
	u8 u8_lat[3];         //3 bytes
	u8 u8_SignalStrength; //1 byte
	u8 u8_lon[3];         //3 bytes
	//IO Status
	union {
		struct {
			unsigned bIgnitionStatus :1;
			unsigned bInput1Status :1;
			unsigned bInput2Status :1;
			unsigned bInput3Status :1;
			unsigned bInput4Status :1;
			unsigned bInput5Status :1;
			unsigned bOutput1Status :1;
			unsigned bOutput2Status :1;
		} Bit;
		unsigned char Byte;
	} Status1;               // 1 byte

	unsigned long lOdometer;

	//19 Bytes

	//Vehicle Status
	union {
		struct {
			unsigned bEngineStatus :1;
			unsigned bVehicleStatus :1;
			unsigned bGpsStatus :1;
			unsigned bOperatingPowerStatus :1;
			unsigned bGpsAntStatus :1;
			unsigned bSeatBeltStatus :1;
			unsigned bNetworkAvail :1;
			unsigned bRoamingStatus :1;
		} Bit;
		unsigned char Byte;
	} Status2;                         // 1 byte

	//Communication Status
	union {
		struct {
			unsigned bRod1CommStatus :1;
			unsigned bRod2CommStatus :1;
			unsigned bRod3CommStatus :1;
			unsigned bMcCommStatus :1;
			unsigned bExtCommStatus :1;
			unsigned bCommStatus1 :1;
			unsigned bCommStatus2 :1;
			unsigned bCommStatus3 :1;
		} Bit;
		unsigned char Byte;
	} Status3;              // 1 byte

	u16 u16_ANIP1_Val;      //2 bytes
	u16 u16_ANIP2_Val;      //2 bytes
	u16 u16_ANIP3_Val;      //2 bytes
	u16 u16_ANIP4_Val;      //2 bytes
	u16 u16_ANIP5_Val;      //2 bytes
	u16 u16_Rod1_Lit;       //2 bytes
	u16 u16_Rod2_Lit;       //2 bytes
	u16 u16_Rod3_Lit;       //2 bytes
	u16 u16_Rod1_ADC;       //2 bytes
	u16 u16_Rod2_ADC;       //2 bytes
	u16 u16_Rod3_ADC;       //2 bytes
	u8 u8_IButtonKey[8];    //8 bytes

} s_txbuff_info;
////////////////////////////////////////////////////////////////////////////

typedef struct {
	u8 u8_dataBuff[1000];
	u16 u16WritePtr;
	u16 u16ReadPtr;
	bool bRewind;
} s_ram_array;

typedef struct {
	u8 u8_txarray[MAX_TX_ARRAY_LENGTH];
	u8 u8_rxarray[MAX_RX_ARRAY_LENGTH];
	u16 u16_txlength;
	u16 u16_rxlength;
} s_ram_array1;

void vts_utils_PrintVersion(void);
unsigned char Data_Load(const s_gps_info * restrict stptrgps,
		const s_mc_info * restrict stptrmc);
unsigned char Data_Retrieve(void);
unsigned char Data_PushRam(void);

s32 ATResponse_QRTC_Handler(char* line, u32 len, void* userData);
#endif /* SYSTEM_H_ */
