/* Includes ------------------------------------------------------------------*/
#include "debug.h"
#include "string.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t DebugDecrease;

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : DebugInit
* Description    : Initialize debug (via USART1)
*******************************************************************************/
void DebugInit()
{

//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE );
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
//	GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	//NVIC_InitTypeDef NVIC_InitStructure;
//
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1); //PB6 to TX USART1
//
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//	USART_InitStructure.USART_BaudRate = 115200;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = /*USART_Mode_Rx | */USART_Mode_Tx;
//	USART_Init(USART1, &USART_InitStructure);
//
//	//USART RX
//	/*NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//	NVIC_EnableIRQ(USART1_IRQn);*/
//
//	USART_Cmd(USART1, ENABLE);
//
//	DebugSendString("\n\n==============================");
//	DebugSendString("======SYSTEM STARTED==========");
//	DebugSendString("==============================\n");
}


/*******************************************************************************
* Function Name  : DebugSendString
* Description    : Sends debug information (via USART1)
* Input          : pointer to text massive
*******************************************************************************/
void DebugSendString(char *pText)
{
	SH_SendString(pText);
	SH_SendChar('\n');

	return;
	for(; *pText != '\0'; pText++)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART_SendData(USART1, *pText);
	}
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, '\x0D');

}

/*******************************************************************************
* Function Name  : DebugSendString
* Description    : Sends debug information (via USART1)
* Input          : pointer to text massive
*******************************************************************************/
void DebugSendStringWSpace(char *pText)
{
	SH_SendString(pText);
	SH_SendChar(' ');

	return;
	for(; *pText != '\0'; pText++)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART_SendData(USART1, *pText);
	}
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, '\t');

}

/*******************************************************************************
* Function Name  : DebugSendChar
* Description    : Sends debug information (via USART1)
* Input          : char to send
*******************************************************************************/
void DebugSendChar(char charTx)
{
	SH_SendChar(charTx);
	return;
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, charTx);
}

void DebugSysTimeStart()
{
	DebugDecrease = 0;
}
/*******************************************************************************
* Function Name  : DebugSysTimeEnd
* Description    : send time from DebugSysTimeStart()
*******************************************************************************/
void DebugSysTimeEnd()
{
	char f[15];
	//if(Debug)
	ftoa_(DebugDecrease, f);
	DebugSendString(f);
}



void DebugSendNum(int32_t Num)
{
	char str[50];
	itoa_(Num, str);
	DebugSendString(str);
}

void DebugSendNumWDesc(char *string, int16_t Num)
{
	char str[50]={'\0'}, number[20];
	strcat_(str, string);
	itoa_(Num, number);
	strcat_(str, number);
	//DebugSendString(str);

	//				char fff[30];
	//				itoa_(vbat, fff);
	//				strcat_(fff, "\n\n");
					SH_SendString(str);
}
void DebugSendNumWSpace(int16_t Num)
{
	char str[50]={'\0'}, number[20];
	itoa_(Num, number);
	strcat_(str, number);
	DebugSendStringWSpace(str);
}
