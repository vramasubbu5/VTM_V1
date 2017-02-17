/*


* task3.c
 *
 *  Created on: Jul 1, 2016
 *      Author: Amruta
 */
/***************************************************************************/
/*This task includes UFS(Flash) storage and file system.                   */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
#include "gpio.h"
#include "system.h"
#include "ql_fs.h"
#include "task3.h"
#include "global.h"
/*
 *
 * ************** Macros ***************
 *
 */
//#ifdef NOTUSED
#define  PATH_ROOT    		((u8 *)"myroot")

#define  LENGTH           100

/*
 *
 * ************** function prototypes ***************
 *
 */

char File_Initialize(void);

s32 Subscribe_File_Sys(u16 iptr);
s32 UnSubscribe_File_Sys(u16 iptr);

void Task3_Timer_handler(u32 timerId, void* param);
void Subscribe_Other_File(void);
void Ptr_Write_in_File(u16 WrtPtr, u16 RdPtr);
void Ptr_Read_in_File(u16 * restrict WrtPtr, u16 * restrict RdPtr);
/*
 *
 * ************** Object declaration ****************
 *
 */

s_ufs_data_buff st_ufs_data_buff;

s_ufs_data_ptrbuff st_ufsdata = {

st_ufs_data_buff.u8writevector, st_ufs_data_buff.u8readvector,
		&st_ufs_data_buff.u16writelength, &st_ufs_data_buff.u16readlength };

extern s_system_info st_sys;

extern TASK_EVENTS m_task_evnt;
extern s_globflags st_globflags;
static u8 filePath[LENGTH] = { 0 };
Enum_FSStorage storage = Ql_FS_UFS;
s32 PtrHandle = -1;
s32 CfgHandle = -1;
static u8 PtrreadBuf[LENGTH] = { 0 };
static u8 PtrwriteBuf[LENGTH] = { 0 };

extern s_glob_var st_glob_var;
typedef union {
	u8 cval[2];
	u16 ival;
} ichar;

static bool bUfsFull = FALSE;
//////////////////////////////////////////////////////////////////////
/***************************************************************************/
/*  Function   : File_Initialize                                           */
/*-------------------------------------------------------------------------*/
/*  Object     : To initialize the file task                               */
/* 				                     					                   */
/* 			     													       */
/*-------------------------------------------------------------------------*/

char File_Initialize(void) {
	s32 ret = 0;

	if ((st_ufs_data_buff.u8writevector == NULL)
			|| (st_ufs_data_buff.u8readvector == NULL)
			|| (&st_ufs_data_buff.u16writelength == NULL))
		return -1;

	ret = Ql_FS_CheckDir((char*) PATH_ROOT);

	if (ret != QL_RET_OK) {

		APP_DEBUG("\r\n ROOT PATH CREATE ");

		ret = Ql_FS_CreateDir((char*) PATH_ROOT);

		if (ret != QL_RET_OK)

			return -2;

	}

	ret = Ql_Timer_Register(TASK3_TIMER, Task3_Timer_handler, NULL);
	if (ret != QL_RET_OK)
		return -3;

	return QL_RET_OK;
}

/***************************************************************************/
/*  Function   : proc_main_task3                                           */
/*-------------------------------------------------------------------------*/
/*  Object     : To start the third task (file task)                       */
/* 				                     					                   */
/* 			     													       */
/*-------------------------------------------------------------------------*/

void proc_main_task3(s32 taskId) {

	ST_MSG msg3;

	bool bTaskStart = FALSE;

	char sc_return = 0;

	//Ql_Debug_Trace("\r\n task_start:%d", taskId);  //disable by amruta for testing
	m_task_evnt.TaskEvent3 = Ql_OS_CreateEvent("\r\n task1");

	while (Ql_OS_WaitEvent(m_task_evnt.TaskEvent3, EVENT_FLAG0) != 0) {

	}

	sc_return = File_Initialize();

	if (sc_return == QL_RET_OK) {

		APP_DEBUG("\r\n File_Initialization_Success ");

		Subscribe_Other_File();

		//ufs_data_ptr.u16ReadPtr = 0;
		//ufs_data_ptr.u16WritePtr = 0;

		//Ptr_Read_in_File(&ufs_data_ptr.u16WritePtr, &ufs_data_ptr.u16ReadPtr);
		//UnSubscribe_File_Sys(UFS_MAX_REC_LEN);
		APP_DEBUG("OpenCPU: Task3\r\n");
		Ql_OS_SetEvent(m_task_evnt.TaskEvent4, EVENT_FLAG0);
		bTaskStart = TRUE;
	} else {
		APP_DEBUG("\r\n File_Initialization_Failure%s ", sc_return);
		Ql_OS_SendMessage(0, MSG_ID_TASK_INIT_DATA, TASK3, 1);
	}

	//Ql_Timer_Start(TASK3_TIMER, 2000, TRUE);
	/*
	 Ql_strcpy((char*) st_ufs_data_buff.u8writevector, "This is test data!");

	 st_ufs_data_buff.u16writelength = Ql_strlen(
	 (const char*) st_ufs_data_buff.u8writevector);
	 */
	while (bTaskStart) {

		Ql_OS_GetMessage(&msg3);
		switch (msg3.message) {
		case MSG_ID_USER_DATA:
			APP_DEBUG(" Task3User_Data\r\n")
			;

			/*
			 if (m_RxBuf_UartPort1[0] == 'W') {
			 Data_Write_in_File();
			 } else if (m_RxBuf_UartPort1[0] == 'R') {
			 Data_Read_From_File();
			 } else {

			 //Subscribe_Fle_Sys(1);
			 APP_DEBUG("Default: Task3_MSG\r\n");
			 }
			 Ql_memset((char*) m_RxBuf_UartPort1, 0x0,
			 Ql_strlen((char*) m_RxBuf_UartPort1));
			 */

			//Ql_Timer_Start(TASK3_TIMER, 5000, TRUE);
			break;
		default:
			APP_DEBUG("Default: Task3\r\n")
			;
			break;
		}

	}

}

/***************************************************************************/
/*  Function   : Subscribe_File_Sys                                        */
/*-------------------------------------------------------------------------*/
/*  Object     : To open particular file                                   */
/* 				                     					                   */
/* 			     													       */
/*-------------------------------------------------------------------------*/
s32 Subscribe_File_Sys(u16 iptr) {

	//u16 i1 = 0;

	//s64 space = 0;

	s32 ret = 0;

	s32 sHandle = -1;

	Ql_sprintf((char*) filePath, "%s\\%s%d\0", PATH_ROOT, "test", iptr);

	//Ql_sprintf((char*) filePath, "%s%d\0", PATH_ROOT, iptr);

	APP_DEBUG("\r\n<--filepath=%s -->\r\n", filePath);

	ret = Ql_FS_Check((char*) filePath);
	if (ret != QL_RET_OK) {

		APP_DEBUG("\r\n<--filepath=%s  does not exist creat file! -->",
				filePath);
	}

	sHandle = Ql_FS_Open((char*) filePath,
	QL_FS_READ_WRITE | QL_FS_CREATE);

	APP_DEBUG("\r\n<--Handle=%d   -->", sHandle);

	return sHandle;

}
/***************************************************************************/
/*  Function   : Data_Write_in_File                                        */
/*-------------------------------------------------------------------------*/
/*  Object     : To write in data  file                                    */
/* 				                     					                   */
/* 			     													       */
/*-------------------------------------------------------------------------*/
bool Data_Write_in_File(void) {

	s32 ret = 0, handle = 0;
	u32 writenLen = 0;
	bool boRetVal = FALSE;
	s32 s32_datalength = 0;

	APP_DEBUG("<--Data_Write_in_File_enter-->\r\n");
	APP_DEBUG("UFS_Wrt_ptr:%d/Rd_Ptr:%d/OldRd_Ptr:%d!\r\n",
			ufs_data_ptr.u16WritePtr, ufs_data_ptr.u16ReadPtr,
			st_glob_var.u16OldUfsReadPtr);
	u16 u16NextPtr = ufs_data_ptr.u16WritePtr + 1;

	//if (u16NextPtr >= UFS_MAX_REC_LEN) {
	//	u16NextPtr = UFS_WRITE_REC_START;
	//}
//	/*
	if (u16NextPtr >= UFS_MAX_REC_LEN) {
		if (ufs_data_ptr.u16ReadPtr >= (UFS_MAX_REC_LEN - 20)) {
			u16NextPtr = UFS_WRITE_REC_START;
			bUfsFull = FALSE;
		} else {
			bUfsFull = TRUE;
			APP_DEBUG("WR Record Flash FULLL1!\r\n");
		}

	}
	// */
	if ((u16NextPtr == ufs_data_ptr.u16ReadPtr) || (bUfsFull == TRUE))
	//if (u16NextPtr == ufs_data_ptr.u16ReadPtr)
			{
		APP_DEBUG("WR Record Flash FULLL1!\r\n");
		APP_DEBUG("Read_pointer_1%d\r\n", ufs_data_ptr.u16ReadPtr);
		return FALSE;

	}

	s32_datalength = *(st_ufsdata).u16ptrinlength;
	if (st_ufsdata.u8ptrindata != NULL) {
		handle = Subscribe_File_Sys(ufs_data_ptr.u16WritePtr); //here check handle
		ret = Ql_FS_Write(handle, (u8 *) st_ufsdata.u8ptrindata, s32_datalength,
				&writenLen);

		APP_DEBUG("\r\n<--!! Ql_FS_Write  ret =%d  writenLen =%d-->\r\n", ret,
				writenLen);
		Ql_FS_Close(handle);
	}

	switch (ret) {
	case QL_RET_OK: {

		ufs_data_ptr.u16WritePtr = u16NextPtr;
		Ptr_Write_in_File(ufs_data_ptr.u16WritePtr, ufs_data_ptr.u16ReadPtr);
		APP_DEBUG("UFS_FILE_WRITE_SS!\r\n");
// added for testing
		boRetVal = SUCCESS;
	}
		break;

	case QL_RET_ERR_FILEDISKFULL: {
		APP_DEBUG("UFS_FILE_DISK_FULL!\r\n");
		boRetVal = FAILURE;
	}
		break;

	case QL_RET_ERR_FILEWRITEFAILED: {
		APP_DEBUG("UFS_FILE_WRITE_FAIL!\r\n");
		boRetVal = FAILURE;
	}
		break;

	case QL_RET_ERR_FS_FATAL_ERR1: {
		APP_DEBUG("UFS_FILE_WRITE_SYSTEM_CORRUPTED!\r\n");
//Ql_FS_Format(QL_FS_FAT); // format the whole file system, which takes tens of seconds.
//NEED TO TEST                    // again subscribe whole file system
		boRetVal = FAILURE;
	}
		break;

	default:
		boRetVal = FAILURE;
		break;
	}

	return boRetVal;
}
/***************************************************************************/
/*  Function   : Data_Read_From_File                                       */
/*-------------------------------------------------------------------------*/
/*  Object     : To read from data  file                                   */
/* 				                     					                   */
/* 			     													       */
/*-------------------------------------------------------------------------*/
char Data_Read_From_File(void) {
	s32 ret = 0, handle = -1;
	u16 u16i1 = 0;
	u32 readenLen = 0;
	char u8RetVal = 0;
	s32 position = QL_RET_ERR_FILEFAILED;

	st_glob_var.u16OldUfsReadPtr = ufs_data_ptr.u16ReadPtr;

	APP_DEBUG("UFS_Wrt_ptr:%d/Rd_Ptr:%d/OldRd_Ptr:%d!\r\n",
			ufs_data_ptr.u16WritePtr, ufs_data_ptr.u16ReadPtr,
			st_glob_var.u16OldUfsReadPtr);

	u16 u16NextPtr = ufs_data_ptr.u16ReadPtr + 1;

	if (ufs_data_ptr.u16WritePtr == ufs_data_ptr.u16ReadPtr) {
		APP_DEBUG("FLASH_empty");
		//u8RetVal = -1;
		return FAILURE;
	}

	//if (position != QL_RET_ERR_FILEFAILED) {
	//handle = UnSubscribe_File_Sys(ufs_data_ptr.u16ReadPtr);

	Ql_sprintf((char*) filePath, "%s\\%s%d\0", PATH_ROOT, "test",
			ufs_data_ptr.u16ReadPtr);
//Ql_sprintf((char*) filePath, "%s%d\0", PATH_ROOT, ufs_data_ptr.u16ReadPtr);

	APP_DEBUG("\r\n<--Read_filepath=%s -->\r\n", filePath);

	ret = Ql_FS_Check((char*) filePath);
	if (ret != QL_RET_OK) {

		APP_DEBUG("\r\n<--filepath=%s   does not exist creat file! -->",
				filePath);
	}

	handle = Ql_FS_Open((char*) filePath,
	QL_FS_READ_WRITE | QL_FS_CREATE);

	APP_DEBUG("\r\n<--Read_Handle=%d   -->", handle);

	ret = Ql_FS_Seek(handle, 0, QL_FS_FILE_END);

	position = Ql_FS_GetFilePosition(handle);

	ret = Ql_FS_Seek(handle, 0, QL_FS_FILE_BEGIN);

	APP_DEBUG("\r\n --> pos : %d %d", position, handle);

	u32 u32_readenLen = 0;

	if (ret == QL_RET_OK) {

		if (st_ufsdata.u8ptroutdata != NULL
				&& st_ufsdata.u16ptroutlength != NULL) {

			ret = Ql_FS_Read(handle, st_ufsdata.u8ptroutdata, position,
					&u32_readenLen);

			if (ret == QL_RET_OK) {

				if (u32_readenLen == position) {

					//u32_readenLen = *(st_ufsdata).u16ptroutlength;

					//	st_ufsdata.u8ptroutdata[u32_readenLen] = '\0';
					APP_DEBUG("\r\n<-- Ql_FS_Read() ret=%d: readedlen=%d\r\n",
							ret, u32_readenLen);

					for (u16i1 = 0; u16i1 < u32_readenLen; u16i1++) {
						APP_DEBUG(":%X", st_ufsdata.u8ptroutdata[u16i1]);
					}
					*(st_ufsdata).u16ptroutlength = u32_readenLen;

					Ql_FS_Close(handle);

/////////added for testing
					/*
					 ret = Ql_FS_Delete((char*) filePath);
					 if (ret == QL_RET_OK) {
					 APP_DEBUG("\r\n<-- Ql_FS_Delete");
					 } else {
					 APP_DEBUG("\r\n<-- Ql_FS_Delete()_fail ret=%d: -->\r\n",
					 ret);
					 }
					 */
//////////testing end
					if (u16NextPtr >= UFS_MAX_REC_LEN) {
						APP_DEBUG("UFS_FILE_READ_REWIND!\r\n");
						u16NextPtr = UFS_READ_REC_START;

					}
					ufs_data_ptr.u16ReadPtr = u16NextPtr;
					u8RetVal = SUCCESS;
				} else {

					APP_DEBUG("\r\nFileLengthError");

				}
				//}
			} else {
				APP_DEBUG("read_file_fail");
				u8RetVal = -2;
			}

		}

	} else {
		APP_DEBUG("read_file_seek_fail");
		u8RetVal = -3;
	}

	return u8RetVal;
}
////////////////////////////////////////////////////////////////////////////////////////
s32 UnSubscribe_File_Sys(u16 iptr) {
	s32 ret = 0;
	//u8 u8i1 = 0;
	s32 sHandle = -1;
	APP_DEBUG("\r\n<--Read_file_enter -->\r\n");
	//for (u8i1 = 0; u8i1 < iptr; u8i1++) {
	Ql_sprintf((char*) filePath, "%s\\%s%d\0", PATH_ROOT, "test", iptr);
	//Ql_sprintf((char*) filePath, "%s%d\0", PATH_ROOT, iptr);

	APP_DEBUG("\r\n<--Read_filepath=%s -->\r\n", filePath);

	ret = Ql_FS_Check((char*) filePath);
	if (ret != QL_RET_OK) {

		APP_DEBUG("\r\n<--filepath=%s   does not exist creat file! -->",
				filePath);
	}

	sHandle = Ql_FS_Open((char*) filePath,
	QL_FS_READ_ONLY | QL_FS_CREATE);

	APP_DEBUG("\r\n<--Read_Handle=%d   -->", sHandle);
	Ql_FS_Close(sHandle);

	ret = Ql_FS_Delete((char*) filePath);
	if (ret == QL_RET_OK) {
		APP_DEBUG("\r\n<-- Ql_FS_Delete");
	} else {
		APP_DEBUG("\r\n<-- Ql_FS_Delete() ret=%d: -->\r\n", ret);
	}
	//}
	return sHandle;
}

/***************************************************************************/
/*  Function   : Subscribe_Other_File                                      */
/*-------------------------------------------------------------------------*/
/*  Object     : To open pointer storage and configuration file            */
/* 				                     					                   */
/* 			     													       */
/*-------------------------------------------------------------------------*/
void Subscribe_Other_File(void) {
	s32 ret = 0;
//////////////////open pointer file/////////////////////
	Ql_sprintf((char*) filePath, "%s\\%s\0", PATH_ROOT, "PTR");

	APP_DEBUG("\r\n<--Ptrfilepath=%s -->\r\n", filePath);

	ret = Ql_FS_Check((char*) filePath);
	if (ret != QL_RET_OK) {

		APP_DEBUG("\r\n<--Ptrfilepath=%s  does not exist creat file! -->",
				filePath);
	}

	PtrHandle = Ql_FS_Open((char*) filePath,
	QL_FS_READ_WRITE | QL_FS_CREATE);

	APP_DEBUG("\r\n<--PtrHandle=%d   -->", PtrHandle);
	Ptr_Read_in_File(&ufs_data_ptr.u16WritePtr, &ufs_data_ptr.u16ReadPtr);

	//////////////////open config file/////////////////////
	Ql_sprintf((char*) filePath, "%s\\%s\0", PATH_ROOT, "CFG");

	APP_DEBUG("\r\n<--Cfgfilepath=%s -->\r\n", filePath);

	ret = Ql_FS_Check((char*) filePath);
	if (ret != QL_RET_OK) {

		APP_DEBUG("\r\n<--Cfgfilepath=%s  does not exist creat file! -->",
				filePath);
	}

	CfgHandle = Ql_FS_Open((char*) filePath,
	QL_FS_READ_WRITE | QL_FS_CREATE);

	APP_DEBUG("\r\n<--PtrHandle=%d   -->", CfgHandle);
}
/***************************************************************************/
/*  Function   : Ptr_Write_in_File                                         */
/*-------------------------------------------------------------------------*/
/*  Object     : To write in pointer storage file                          */
/* 				                     					                   */
/* 			     													       */
/*-------------------------------------------------------------------------*/
void Ptr_Write_in_File(u16 WrtPtr, u16 RdPtr) {
	s32 ret = 0;
	u32 writenLen = 0;
	ichar ptr;
	ptr.ival = WrtPtr;
	PtrwriteBuf[0] = ptr.cval[0];
	PtrwriteBuf[1] = ptr.cval[1];
	ptr.ival = RdPtr;
	PtrwriteBuf[2] = ptr.cval[0];
	PtrwriteBuf[3] = ptr.cval[1];
	APP_DEBUG("\r\n<--stored_read_ptr_write_ptr:%d/%d/%d/%d/%d/%d -->", WrtPtr,
			RdPtr, PtrwriteBuf[1], PtrwriteBuf[0], PtrwriteBuf[3],
			PtrwriteBuf[2]);

	ret = Ql_FS_Seek(PtrHandle, 0, QL_FS_FILE_BEGIN);
	ret = Ql_FS_Write(PtrHandle, (u8 *) PtrwriteBuf, 4, &writenLen);

	APP_DEBUG("\r\n<--!! Ql_FS_Pointer_Write  ret =%d  writenLen =%d-->\r\n",
			ret, writenLen);
	//Ptr_Read_in_File(&WrtPtr, &RdPtr);
}
/***************************************************************************/
/*  Function   : Ptr_Read_in_File                                          */
/*-------------------------------------------------------------------------*/
/*  Object     : To read from pointer storage file                         */
/* 				                     					                   */
/* 			     													       */
/*-------------------------------------------------------------------------*/
void Ptr_Read_in_File(u16 * restrict WrtPtr, u16 * restrict RdPtr) {

	s32 ret = 0;
	u32 readenLen = 0;
	s32 position = QL_RET_ERR_FILEFAILED;
	ichar ptr;

	ret = Ql_FS_Seek(PtrHandle, 0, QL_FS_FILE_END);
	//position = Ql_FS_GetFilePosition(PtrHandle);
	//APP_DEBUG("\r\n --> pos : %d", position);
	//ret = Ql_FS_Truncate(PtrHandle);
	position = Ql_FS_GetFilePosition(PtrHandle);

	ret = Ql_FS_Seek(PtrHandle, 0, QL_FS_FILE_BEGIN);

	APP_DEBUG("\r\n --> pos : %d %d", position, PtrHandle);
	if (ret == QL_RET_OK) {

		ret = Ql_FS_Read(PtrHandle, PtrreadBuf, 4, &readenLen);

		if (ret == QL_RET_OK) {
			APP_DEBUG(
					"\r\n<-- Ql_FS_Read() ret=%d: readedlen=%d, readBuffer=%s-->\r\n",
					ret, readenLen, PtrreadBuf);
			ptr.cval[0] = PtrreadBuf[0];
			ptr.cval[1] = PtrreadBuf[1];
			*WrtPtr = ptr.ival;

			ptr.cval[0] = PtrreadBuf[2];
			ptr.cval[1] = PtrreadBuf[3];
			*RdPtr = ptr.ival;
			APP_DEBUG("\r\n --> ponter_stored_in_file :%d:%d ", *WrtPtr, *RdPtr);
		} else {
			APP_DEBUG("\r\n --> Read_ptr_file_fail_1 ");
		}

	} else {
		APP_DEBUG("\r\n --> Read_ptr_file_fail_2 ");
	}
}
/***************************************************************************/
/*  Function   : Task3_Timer_handler                                       */
/*-------------------------------------------------------------------------*/
/*  Object     : To handle task 3 timer                                    */
/* 				                     					                   */
/* 			     													       */
/*-------------------------------------------------------------------------*/
void Task3_Timer_handler(u32 timerId, void* param) {

	if (timerId == TASK3_TIMER) {

		APP_DEBUG("\r\n task3 timer");

		APP_DEBUG("\r\n task3 timer_for_loop");

		APP_DEBUG("\r\n file_pointer%d/%d", ufs_data_ptr.u16WritePtr,
				ufs_data_ptr.u16ReadPtr);
		//Data_Write_in_File();
		//Ptr_Read_in_File(ufs_data_ptr.u16WritePtr, ufs_data_ptr.u16ReadPtr);
		//	ret = Data_Read_From_File();
		//	APP_DEBUG("read_ret%d", ret);

		//Ql_Timer_Stop(TASK3_TIMER);
	}

}
//#endif
