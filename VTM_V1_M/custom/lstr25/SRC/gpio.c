/*
 * gpio.c
 *
 *  Created on: May 30, 2016
 *      Author: Amruta
 */

#ifdef __CUSTOMER_CODE__

#include "gpio.h"
#include "ql_gpio.h"

void GPIO_Init(void) {
	Enum_PinName gpioPin1 = PINNAME_NETLIGHT;
	Enum_PinName gpioPin2 = PINNAME_DTR;
	Enum_PinName gpioPin3 = PINNAME_RI;
	Enum_PinName gpioPin4 = PINNAME_DCD;
	Enum_PinName gpioPin5 = PINNAME_CTS;
	Enum_PinName gpioPin6 = PINNAME_RTS;
	Enum_PinName gpioPin7 = PINNAME_PCM_CLK;
	Enum_PinName gpioPin8 = PINNAME_PCM_SYNC;
	Enum_PinName gpioPin9 = PINNAME_PCM_IN;
	Enum_PinName gpioPin10 = PINNAME_PCM_OUT;
#ifdef NOTUSED
	Enum_PinName gpioPin11 = PINNAME_RXD_AUX;
	Enum_PinName gpioPin11 = PINNAME_TXD_AUX;
#endif

	Enum_PinLevel high = PINLEVEL_HIGH;
	Enum_PinLevel low = PINLEVEL_LOW;
	Ql_GPIO_Init(gpioPin1, PINDIRECTION_OUT, high, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin2, PINDIRECTION_OUT, high, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin3, PINDIRECTION_IN, low, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin4, PINDIRECTION_IN, low, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin5, PINDIRECTION_IN, high, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin6, PINDIRECTION_OUT, low, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin7, PINDIRECTION_IN, high, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin8, PINDIRECTION_OUT, high, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin9, PINDIRECTION_IN, high, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin10, PINDIRECTION_OUT, low, PINPULLSEL_DISABLE);
#ifdef NOTUSED
	Ql_GPIO_Init(gpioPin11, PINDIRECTION_OUT, high, PINPULLSEL_DISABLE);
	Ql_GPIO_Init(gpioPin12, PINDIRECTION_OUT, high, PINPULLSEL_DISABLE);
#endif
}

#endif
