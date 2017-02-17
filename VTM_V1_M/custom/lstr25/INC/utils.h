/*
 * utils.h
 *
 *  Created on: Jul 19, 2016
 *      Author: Amruta
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "system.h"
/***************************************************************************/
/*  Global Macro Defines for UTILS				                           */
/***************************************************************************/

/***************************************************************************/
/*  Global Structure for UTILS				                               */
/***************************************************************************/
typedef union {
	unsigned int ival;
	unsigned char cval[2];
} intchar;

typedef union {
	unsigned long lval;
	unsigned int ival[2];
	unsigned char cval[4];
} longchar;
/***************************************************************************/
/*  Functions Prototypes for UTILS				                           */
/***************************************************************************/

unsigned char vts_utils_IntToBuff(unsigned int IntVal, char *cBuff);
void vts_utils_ReadSystemSerialNo(void);
void vts_utils_BuffCpy_RecHeader(const s_rec_hdr *restrict st_ptrrec,
		unsigned char *cData, int *iTotByte);
void vts_utils_CopyDataStruct(s_txbuff_info *source, s_txbuff_info *Dest);

#endif /* UTILS_H_ */
