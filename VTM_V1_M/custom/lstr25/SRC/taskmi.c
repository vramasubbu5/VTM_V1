/*

typedef enum
{
	SF_GPIO_STARTED ,
	SF_GPIO_READ ,
	SF_GPIO_WRITE ,
	SF_GPIO_FAIL ,

} Enum_Sf_GPIO_State;

static void setGPIO_state(Enum_Sf_GPIO_State gpio_state)
{
	Ql_Debug_Trace("<-- set GPIO State %d -->\r\n", gpio_state);
	Ql_OS_SendMessage (2, MSG_ID_SF_GPIO_STATE ,0 ,0);	// gpio_state , 2);
//	Ql_OS_SendMessage (1, MSG_ID_SF_GPIO_STATE , gpio_state , 0);
}
void proc_subtask1(s32 TaskId)
{
	Ql_Debug_Trace("Lateral emSoft the world of navigation \r\n");
	Ql_Debug_Trace("GPIO Application wake up\r\n");

	Enum_Sf_GPIO_State gpio_state = SF_GPIO_STARTED;

	GPIO_Init();
	Ql_Debug_Trace("GPIO_Init \r\n");

	ST_MSG subtask1_msg;

	Ql_Debug_Trace("GPIO_ST_MSG subtask1_msg \r\n");
//

	while (TRUE)
	{
		Ql_Debug_Trace("GPIO_while (TRUE)");

		switch(gpio_state)
		{
			case SF_GPIO_READ:
				Ql_Debug_Trace("\r \n GPIO READ STARTED before 1 \r \n");
				break;
			default:
				break;
		}

		Ql_OS_GetMessage(&subtask1_msg);

		Ql_Debug_Trace("GPIO_while (TRUE)1");
		Ql_Debug_Trace("GPIO_while (TRUE)2");
	//
		Ql_Debug_Trace("GPIO_Ql_OS_GetMessage(&msg);");
		if(subtask1_msg.message == MSG_ID_GPIO_STATE)
		{
		Ql_Debug_Trace("ok g ok");
		//	gpio_state = msg.param1;
			switch(gpio_state)
			{
				case SF_GPIO_STARTED:
					Ql_Debug_Trace("GPIO STARTED");
					break;
				case SF_GPIO_READ:
					Ql_Debug_Trace("GPIO READ");
					break;
				case SF_GPIO_WRITE:
					Ql_Debug_Trace("GPIO WRITE");
					break;
				case SF_GPIO_FAIL:
					Ql_Debug_Trace("GPIO FAIL");
					break;
				default:
					Ql_Debug_Trace("GPIO default");
					break;
			}

			setState(SF_STATE_DATA_READY);
			setState(SF_STATE_DATA_READY);
			break;

		}
	}


}*/

/*
	ST_MSG subtask1_msg;
	while (TRUE)
	{
//		Ql_OS_GetMessage (&msg);
		Ql_OS_GetMessage(&subtask1_msg);

		if(subtask1_msg.message == MSG_ID_SF_GPIO_STATE)
		{
			gpio_state = msg.param1;
			switch (gpio_state)
				{
					case SF_STATE_FULL_FUNC:

	}
*/
/*
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

*/
