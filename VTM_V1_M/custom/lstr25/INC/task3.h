/*
 * task3.h
 *
 *  Created on: Jul 2, 2016
 *      Author: Amruta
 */

#ifndef TASK3_H_
#define TASK3_H_

#define SUCCESS                        1
#define FAILURE                        0
#define UFS_WRITE_REC_START            0
#define UFS_READ_REC_START             0
#define UFS_MAX_REC_LEN               30//250#define UFS_DELETE_MUL_FACTOR         (UFS_MAX_REC_LEN/10)
#define UFS_WRITE_LIMIT_ID            (UFS_DELETE_MUL_FACTOR + 5)

/******************************************************************************
 * Define APP Structure
 ******************************************************************************/
typedef struct UFS_DATA_PTR {
	u16 u16WritePtr;
	u16 u16ReadPtr;

} s_ufs_data_ptr;

s_ufs_data_ptr ufs_data_ptr;

typedef struct _UFS_DATA_PTR_BUFF {
	u8 * u8ptrindata;
	u8 * u8ptroutdata;
	u16 * u16ptrinlength;
	u16 * u16ptroutlength;
} s_ufs_data_ptrbuff;

typedef struct _UFS_DATA_BUFF {
	u8 u8writevector[1000];
	u8 u8readvector[1000];
	u16 u16writelength;
	u16 u16readlength;
} s_ufs_data_buff;

/////////////////////////////////////////////////////////////////////////////
bool Data_Write_in_File(void);
char Data_Read_From_File(void);
#endif /* TASK3_H_ */
