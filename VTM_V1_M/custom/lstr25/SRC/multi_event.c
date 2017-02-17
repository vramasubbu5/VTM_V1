/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Quectel Co., Ltd. 2013
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   example_multitask_port.c
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   This example demonstrates how to use multitask_port function with APIs in OpenCPU.
 *   Input the specified command through any uart port and the result will be
 *   output through the debug port.
 *
 * Usage:
 * ------
 *   Compile & Run:
 *
 *     Set "C_PREDEF=-D __EXAMPLE_MULTITASK_PORT__" in gcc_makefile file. And compile the
 *     app using "make clean/new".
 *     Download image bin to module to run.
 *
 *   Operation:
 *
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 *----------------------------------------------------------------------------
 *
 ****************************************************************************/
#ifdef __EXAMPLE_MULTITASK_PORT1__
#include "ql_error.h"
#include "ql_trace.h"
#include "ql_uart.h"
#include "ql_system.h"
#include "ql_stdlib.h"
#include "ql_gpio.h"



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



#define SERIAL_RX_BUFFER_LEN  2048
static u8 m_RxBuf_VirtualPort1[SERIAL_RX_BUFFER_LEN];
static u8 m_RxBuf_UartPort1[SERIAL_RX_BUFFER_LEN];
static u8 m_RxBuf_UartPort2[SERIAL_RX_BUFFER_LEN];
static u8 m_RxBuf_UartPort3[SERIAL_RX_BUFFER_LEN];

static void Callback_UART_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void* customizedPara);

static void GPIO_Program(void);
/**************************************************************
* Main Task
***************************************************************/
//APP_DEBUG("<-- Get the GPIO level value: %d -->\r\n\r\n", Ql_GPIO_GetLevel(gpioPin));
void proc_main_task()
{
    s32 ret, ret1;
    ST_MSG msg;
    Enum_SerialPort mySerialPort = VIRTUAL_PORT1;

    // Register & open virtual port
    ret = Ql_UART_Register(mySerialPort, Callback_UART_Hdlr, NULL);
    ////////////////////////////////////////////////////

//    ret1 = Ql_OS_CreateEvent

    ///////////////////////////////////////////////////////////////
    if (ret < QL_RET_OK)
    {
        Ql_Debug_Trace("<--Fail to register serial port[%d], ret=%d-->\r\n", mySerialPort, ret);
    }
    ret = Ql_UART_Open(mySerialPort, 0, 0);
    if (ret < QL_RET_OK)
    {
        Ql_Debug_Trace("<--Fail to open serial port[%d], ret=%d-->\r\n", mySerialPort, ret);
    }
    Ql_Debug_Trace("<--OpenCPU: example_multitask_port-->\r\n");
    Ql_UART_ClrRxBuffer(mySerialPort);
    Ql_Debug_Trace("\n \r MMain \n\r");

    while(TRUE)
    {
        Ql_OS_GetMessage(&msg);
        switch(msg.message)
        {
        case MSG_ID_USER_START:
        	Ql_Debug_Trace("\n \r US \n\r");
            break;
        default:
        	Ql_Debug_Trace("\n \r DF \n\r");
            break;
        }
        Ql_Debug_Trace("\n \r MM \n\r");

    }
}

/**************************************************************
* Subtask1
***************************************************************/
void proc_subtask1(s32 TaskId)
{
    s32 ret;
    Ql_Debug_Trace("\n \r 1 \n\r");
    ST_MSG msg;
    ST_UARTDCB dcb;
    Enum_SerialPort mySerialPort = UART_PORT1;

    Ql_Debug_Trace("<--OpenCPU: proc_subtask1-->\r\n");

    dcb.baudrate = 115200;
    dcb.dataBits = DB_8BIT;
    dcb.stopBits = SB_ONE;
    dcb.parity   = PB_NONE;
    dcb.flowCtrl = FC_NONE;
    ret = Ql_UART_Register(mySerialPort, Callback_UART_Hdlr, NULL);
    if (ret < QL_RET_OK)
    {
        Ql_Debug_Trace("<--Ql_UART_Register(mySerialPort=%d)=%d-->\r\n", mySerialPort, ret);
    }
    ret = Ql_UART_OpenEx(mySerialPort, &dcb);
    if (ret < QL_RET_OK)
    {
        Ql_Debug_Trace("<--Ql_UART_OpenEx(mySerialPort=%d)=%d-->\r\n", mySerialPort, ret);
    }
    Ql_UART_ClrRxBuffer(mySerialPort);
    Ql_Debug_Trace("\n \r T1 \n\r");
    while(TRUE)
    {
    	Ql_Debug_Trace("\n \r T1_W \n\r");
        Ql_OS_GetMessage(&msg);
        switch(msg.message)
        {
        case MSG_ID_USER_START:
        	Ql_Debug_Trace("\n \r T1_US \n\r");
            break;
        default:
        	Ql_Debug_Trace("\n \r D1_US \n\r");
            break;
        }
    }
    Ql_Debug_Trace("\n \r T11 \n\r");
}

/**************************************************************
* Subtask2
***************************************************************/
void proc_subtask2(s32 TaskId)
{
    s32 ret1;
    Ql_Debug_Trace("\n \r GPIO \n\r");
    ST_MSG msg;

    // Register & open UART port
    //ST_UARTDCB dcb;
        Enum_SerialPort mySerialPort = UART_PORT2;

        Ql_Debug_Trace("<--OpenCPU: proc_subtask2-->\r\n");

        ret1 = Ql_UART_Register(mySerialPort, Callback_UART_Hdlr, NULL);
        if (ret1 < QL_RET_OK)
        {
            Ql_Debug_Trace("<--Ql_UART_Register(mySerialPort=%d)=%d-->\r\n", mySerialPort, ret1);
        }
        /*
        ret = Ql_UART_GetDCBConfig(mySerialPort, &dcb);
        if (ret < QL_RET_OK)
        {
            APP_DEBUG("<--Ql_UART_GetDCBConfig(mySerialPort=%d)=%d-->\r\n", mySerialPort, ret);
            APP_DEBUG("<--DCB: br=%d, db=%d, sb=%d, pb=%d, fc=%d-->\r\n",
                dcb.baud, dcb.dataBits, dcb.stopBits, dcb.parity, dcb.flowControl);
        }
        */
        ret1 = Ql_UART_Open(mySerialPort, 115200, FC_NONE);
        if (ret1 < QL_RET_OK)
        {
            Ql_Debug_Trace("<--Ql_UART_OpenEx(mySerialPort=%d)=%d-->\r\n", mySerialPort, ret1);
        }
        Ql_UART_ClrRxBuffer(mySerialPort);
        Ql_Debug_Trace("\n \r T2 \n\r");


    // Start to program GPIO pin
    GPIO_Program();

    // Start message loop of this task
    while(TRUE)
    {
    	Ql_Debug_Trace("\n \r T2_W \n\r");
        Ql_OS_GetMessage(&msg);
        switch(msg.message)
        {
        	case MSG_ID_USER_START:
        		Ql_Debug_Trace("\n \r T2_US \n\r");
        		break;
            default:
            	Ql_Debug_Trace("\n \r D2_US \n\r");
                break;
        }
    }
    Ql_Debug_Trace("\n \r T22 \n\r");
}


/**************************************************************
* Subtask3
***************************************************************/
void proc_subtask3(s32 TaskId)
{
    s32 ret;
    Ql_Debug_Trace("\n \r 3 \n\r");
    ST_MSG msg;

    Enum_SerialPort mySerialPort = UART_PORT3;
    ST_UARTDCB dcb = {
        1200,
        DB_8BIT,
        SB_ONE,
        PB_NONE,
        FC_NONE
    };

    ret = Ql_UART_Register(mySerialPort, Callback_UART_Hdlr, NULL);
    if (ret < QL_RET_OK)
    {
        Ql_Debug_Trace("<--Fail to register serial port[%d], ret=%d-->\r\n", mySerialPort, ret);
    }
    ret = Ql_UART_OpenEx(mySerialPort, &dcb);
    if (ret < QL_RET_OK)
    {
        Ql_Debug_Trace("<--Fail to open serial port[%d], ret=%d-->\r\n", mySerialPort, ret);
    }
    Ql_UART_ClrRxBuffer(mySerialPort);
    Ql_Debug_Trace("\n \r T3 \n\r");
    while(TRUE)
    {
    	Ql_Debug_Trace("\n \r T3_W \n\r");
        Ql_OS_GetMessage(&msg);
        switch(msg.message)
        {
        	case MSG_ID_USER_START:
        		Ql_Debug_Trace("\n \r T3_US \n\r");
                break;
            default:
                Ql_Debug_Trace("\n \r D3_US \n\r");
                break;
        }
    }
}

static s32 ReadSerialPort(Enum_SerialPort port, /*[out]*/u8* pBuffer, /*[in]*/u32 bufLen)
{
    s32 rdLen = 0;
    s32 rdTotalLen = 0;
    if (NULL == pBuffer || 0 == bufLen)
    {
        return -1;
    }
    Ql_memset(pBuffer, 0x0, bufLen);
    while (1)
    {
        rdLen = Ql_UART_Read(port, pBuffer + rdTotalLen, bufLen - rdTotalLen);
        if (rdLen <= 0)  // All data is read out, or Serial Port Error!
        {
            break;
        }
        rdTotalLen += rdLen;
        // Continue to read...
    }
    if (rdLen < 0) // Serial Port Error!
    {
        APP_DEBUG("<--Fail to read from port[%d]-->\r\n", port);
        return -99;
    }
    return rdTotalLen;
}


static void Callback_UART_Hdlr(Enum_SerialPort port, Enum_UARTEventType evt, bool level, void* customizedPara)
{
    s32 ret;
    //APP_DEBUG("<--CallBack_UART_Hdlr: port=%d, event=%d, level=%d, p=%x-->\r\n", port, evt, level, customizedPara);
    switch (evt)
    {
    case EVENT_UART_READY_TO_READ:
        {

        	Ql_Debug_Trace("\n \r CB \n\r");

            if (VIRTUAL_PORT1 == port || VIRTUAL_PORT2 == port)
            {// Print URC through uart1 port
                s32 totalBytes = ReadSerialPort(port, m_RxBuf_VirtualPort1, sizeof(m_RxBuf_VirtualPort1));
                s32 ret = Ql_UART_Write(UART_PORT1, m_RxBuf_VirtualPort1, totalBytes);
                if (ret < totalBytes)
                {
                    APP_DEBUG("<--Only part of bytes are written, %d/%d-->\r\n", ret, totalBytes);
                    // TODO: Need to handle event 'QL_EVENT_UART_READY_TO_WRITE'
                }
            }
            else if (UART_PORT1 == port || UART_PORT2 == port || UART_PORT3 == port)
            {// Reflect the data
                u8* arrRxBuf[] = {m_RxBuf_UartPort1, m_RxBuf_UartPort2, m_RxBuf_UartPort3};
                s32 totalBytes = ReadSerialPort(port, arrRxBuf[port - UART_PORT1], SERIAL_RX_BUFFER_LEN);
                s32 ret = Ql_UART_Write(port, arrRxBuf[port - UART_PORT1], totalBytes);
                if (ret < totalBytes)
                {
                    APP_DEBUG("<--Only part of bytes are written, %d/%d-->\r\n", ret, totalBytes);
                    // TODO: Need to handle event 'QL_EVENT_UART_READY_TO_WRITE'
                }
            }
            break;
        }
    case EVENT_UART_READY_TO_WRITE:
        break;
    default:
        break;
    }
}
static void GPIO_Program(void)
{
    // Specify a GPIO pin
    Enum_PinName  gpioPin = PINNAME_NETLIGHT;

    // Define the initial level for GPIO pin
    Enum_PinLevel gpioLvl = PINLEVEL_HIGH;

    // Initialize the GPIO pin (output high level, pull up)
    Ql_GPIO_Init(gpioPin, PINDIRECTION_OUT, gpioLvl, PINPULLSEL_PULLUP);
    APP_DEBUG("<-- Initialize GPIO pin (PINNAME_STATUS): output, high level, pull up -->\r\n");

    // Get the direction of GPIO
    APP_DEBUG("<-- Get the GPIO direction: %d -->\r\n", Ql_GPIO_GetDirection(gpioPin));

    // Get the level value of GPIO
    APP_DEBUG("<-- Get the GPIO level value: %d -->\r\n\r\n", Ql_GPIO_GetLevel(gpioPin));

    // Set the GPIO level to low after 500ms.
    APP_DEBUG("<-- Set the GPIO level to low after 500ms -->\r\n");
    Ql_Sleep(500);
    Ql_GPIO_SetLevel(gpioPin, PINLEVEL_LOW);
    APP_DEBUG("<-- Get the GPIO level value: %d -->\r\n\r\n", Ql_GPIO_GetLevel(gpioPin));

    // Set the GPIO level to high after 500ms.
    APP_DEBUG("<-- Set the GPIO level to high after 500ms -->\r\n");
    Ql_Sleep(500);
    Ql_GPIO_SetLevel(gpioPin, PINLEVEL_HIGH);
    APP_DEBUG("<-- Get the GPIO level value: %d -->\r\n", Ql_GPIO_GetLevel(gpioPin));
}

#endif // __EXAMPLE_MULTITASK_PORT__
