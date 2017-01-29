/* Includes ------------------------------------------------------------------*/
#include "hardware.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
uint8_t BTStatus = BT_DISCONNECTED;

/* Private variables ---------------------------------------------------------*/
static GPIO_InitTypeDef GPIOInitStructure;
static volatile LEDTypeDef LEDS[11];

	int16_t touchKeyActivated[6] = {0};

uint8_t chargeStatus = 1;

extern uint8_t failOccured;


/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void ADC_Configuration();


/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : HardwareInit
* Description    : Init hardware part of a system
*******************************************************************************/
void HardwareInit()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD,
		ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	PWR_WakeUpPinCmd(PWR_WakeUpPin_2, DISABLE); //C13

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
								 GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13; 
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIOInitStructure);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIOInitStructure);

	GPIO_SetBits(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12); //LEDS
	GPIO_SetBits(GPIOA, GPIO_Pin_15); //LED

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIOInitStructure);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_2 | 
								 GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIOInitStructure);


	/* BT LED Capture */
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIOInitStructure);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_0; //SPECTRE_OUT - ADC
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIOInitStructure);

	ADC_Configuration();
}


static void ADC_Configuration()
{
	/* PCLK2 is the APB2 clock */
	/* ADCCLK = PCLK2/6 = 72/6 = 12MHz*/
	RCC_HSICmd(ENABLE);
	while (!RCC_GetFlagStatus(RCC_FLAG_HSIRDY));
	/* Enable ADC1 clock so that we can talk to it */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	/* Put everything back to power-on defaults */
	ADC_DeInit(ADC1);

	/* ADC1 Configuration ------------------------------------------------------*/
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = 0;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

//	/* Enable ADC1 reset calibaration register */
//	ADC_ResetCalibration(ADC1);
//	/* Check the end of ADC1 reset calibration register */
//	while(ADC_GetResetCalibrationStatus(ADC1));
//
//	/* Start ADC1 calibaration */
//	ADC_StartCalibration(ADC1);
//	/* Check the end of ADC1 calibration */
//	while(ADC_GetCalibrationStatus(ADC1));
}


uint16_t ADC_ReadChannel(uint8_t channel)
{
	ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_9Cycles);
	// Start the conversion
	ADC_SoftwareStartConv(ADC1);
	// Wait until conversion completion
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);

	// Get the conversion value
	return ADC_GetConversionValue(ADC1);
}


/*******************************************************************************
* Function Name  : SysTickConfig
* Description    : Init system tick timer
* Input			 : sysClock in kHz
*******************************************************************************/
void SysTickConfig(uint32_t sysClock)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_Config(sysClock);
}


/*******************************************************************************
* Function Name  : PowerEnterStandby
*******************************************************************************/
void PowerEnterStandby()
{
	PWR_WakeUpPinCmd(PWR_WakeUpPin_2, ENABLE); //C13
	PWR_EnterSTANDBYMode();
}


/*******************************************************************************
* Function Name  :
*******************************************************************************/
void PowerEnableBank(){
	GPIO_SetBits(GPIOA, GPIO_Pin_7);}
void PowerDisableBank(){
	GPIO_ResetBits(GPIOA, GPIO_Pin_7);}


/*******************************************************************************
* Function Name  :
*******************************************************************************/
void PowerEnableMux(){
	GPIO_SetBits(GPIOC, GPIO_Pin_3);}
void PowerDisableMux(){
	GPIO_ResetBits(GPIOC, GPIO_Pin_3);}

/*******************************************************************************
* Function Name  :
*******************************************************************************/
void PowerEnableDisplaySpectre(){
	GPIO_SetBits(GPIOC, GPIO_Pin_2);}
void PowerDisableDisplaySpectre(){
	GPIO_ResetBits(GPIOC, GPIO_Pin_2);}

/*******************************************************************************
* Function Name  :
*******************************************************************************/
void PowerEnableAMP(){
	GPIO_SetBits(GPIOA, GPIO_Pin_6);}
void PowerDisableAMP(){
	GPIO_SetBits(GPIOA, GPIO_Pin_6);}


/*******************************************************************************
* Function Name  :
*******************************************************************************/
void AMPMuteOn(){
	GPIO_SetBits(GPIOC, GPIO_Pin_4);}
void AMPMuteOff(){
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);}

/*******************************************************************************
* Function Name  :
*******************************************************************************/
void PowerEnableRadio(){
	GPIO_SetBits(GPIOC, GPIO_Pin_5);}
void PowerDisableRadio(){
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);}

/*******************************************************************************
* Function Name  :
*******************************************************************************/
void PowerDisableCharge(){
	GPIO_SetBits(GPIOB, GPIO_Pin_9);chargeStatus = 0;}
void PowerEnableCharge(){
	GPIO_ResetBits(GPIOB, GPIO_Pin_9);chargeStatus = 1;}


/*******************************************************************************
* Function Name  :
*******************************************************************************/
uint8_t checkConnectedEarphones()
{
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIOInitStructure);
	_delay_IIC_soft(100);

	uint8_t status = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIOInitStructure);

	return status;
}

/*******************************************************************************
* Function Name  :
*******************************************************************************/
uint8_t checkVExt()
{
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIOInitStructure);
	_delay_IIC_soft(100);

	uint8_t status = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIOInitStructure);

	return status?0:1;
}

/*******************************************************************************
* Function Name  :
*******************************************************************************/
uint8_t checkCharge()
{
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIOInitStructure);
	_delay_IIC_soft(100);

	uint8_t status = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIOInitStructure);

	return status?0:1;
}

/*******************************************************************************
* Function Name  :
*******************************************************************************/
uint8_t checkVUSB()
{
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIOInitStructure);
	_delay_IIC_soft(100);

	uint8_t status = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIOInitStructure);

	return status?0:1;
}

/*******************************************************************************
* Function Name  :
*******************************************************************************/
void PowerEnableBT()
{
	GPIO_SetBits(GPIOC, GPIO_Pin_1);

	//TIM3 ch4
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //FM capture

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);

	TIM_TimeBaseInitTypeDef TimeBaseInitStructure;
	TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TimeBaseInitStructure.TIM_Prescaler = 64000 - 1;
	TimeBaseInitStructure.TIM_Period = 8000;
	TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3, &TimeBaseInitStructure);

	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; //TIM_ICPSC_DIV1
	TIM_ICInitStructure.TIM_ICFilter = 0; //3
	TIM_ICInit(TIM3, &TIM_ICInitStructure);

	TIM_ITConfig(TIM3, TIM_IT_CC4, ENABLE);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 10;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_EnableIRQ(TIM3_IRQn);

	//TIM_SelectInputTrigger(TIM3, TIM_TS_TI2FP2);
	//TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);

	TIM_Cmd(TIM3, ENABLE);
}

/*******************************************************************************
* Function Name  :
*******************************************************************************/
void PowerDisableBT()
{
	TIM_Cmd(TIM3, DISABLE);
	GPIO_ResetBits(GPIOC, GPIO_Pin_1);
}


uint16_t GetVBat()
{
	//val*3.3/1024*47/10
	return ADC_ReadChannel(ADC_Channel_10)*23/5-44 /** 4.593682028 - 44.35678502*/;
}

void ScanSpectre(uint16_t* data)
{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_10 | GPIO_Pin_11;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_11); //scl

	GPIO_SetBits(GPIOB, GPIO_Pin_2); //CE
	_delay_IIC_soft(10);
	GPIO_ResetBits(GPIOB, GPIO_Pin_2); //CE

	for(uint8_t i=0; i<5; i++)
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_10);//scl
		_delay_IIC_soft(800);

		data[i] = ADC_ReadChannel(ADC_Channel_8); //SPECTRE OUT

		GPIO_SetBits(GPIOB, GPIO_Pin_10); //scl
		_delay_IIC_soft(400);
	}
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : BT Led input handler
*******************************************************************************/
void TIM3_IRQHandler()
{
	/* New FM period capture */
	if(TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);

		static uint16_t _RX_prevCapture = 0;

		uint16_t _RX_currCapture = TIM_GetCapture4(TIM3), realCapture;

		if(_RX_prevCapture < _RX_currCapture)
			realCapture = _RX_currCapture - _RX_prevCapture;
		else
			realCapture = _RX_currCapture + 8000 - _RX_prevCapture;
		_RX_prevCapture = _RX_currCapture;

		if(realCapture > 550 && realCapture < 650)
			BTStatus = BT_CONNECTED;
		else //if(realCapture > 150 && realCapture < 250)
			BTStatus = BT_DISCONNECTED;
	}
}

/*******************************************************************************
* Function Name  : KeyboardHWScan
* Description    : Scan keyboard and return it's state
*******************************************************************************/
uint32_t KeyboardHWScan()
{
	uint32_t status = 0;

	NVIC_DisableIRQ(SysTick_IRQn);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIOInitStructure);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIOInitStructure);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOD, &GPIOInitStructure);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB, &GPIOInitStructure);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &GPIOInitStructure);

	for(uint32_t i=0; i<10000; i++) //?? time?
		asm("nop");

	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10)==0)
		status |= BTN_UL;
	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11)==0)
		status |= BTN_DL;
	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12)==0)
		status |= BTN_DR;
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0)
		status |= BTN_UR;

	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13))
		status |= BTN_POWER;
	if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2))
		status |= BTN_BANK;
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3))
		status |= BTN_MUTE;

//	if(touchKeyActivated[0])
//		status |= BTN_DL;
//	if(touchKeyActivated[1])
//		status |= BTN_DR;
//	if(touchKeyActivated[2])
//		status |= BTN_UR;
//	if(touchKeyActivated[3])
//		status |= BTN_UL;
//	if(touchKeyActivated[4])
//		status |= BTN_OK;

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIOInitStructure);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIOInitStructure);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIOInitStructure);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB, &GPIOInitStructure);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &GPIOInitStructure);

	NVIC_EnableIRQ(SysTick_IRQn);

	return status;
}


#define LED_RESET	do {	if(i < 3 || i > 6) GPIO_ResetBits((GPIO_TypeDef*)LED_GPIO_PORTS[i], LED_GPIO_PINS[i]);	\
							else	  GPIO_SetBits((GPIO_TypeDef*)LED_GPIO_PORTS[i], LED_GPIO_PINS[i]);		\
					}while(0);

#define LED_SET		do {	if(i < 3 || i > 6) GPIO_SetBits((GPIO_TypeDef*)LED_GPIO_PORTS[i], LED_GPIO_PINS[i]);		\
							else	  GPIO_ResetBits((GPIO_TypeDef*)LED_GPIO_PORTS[i], LED_GPIO_PINS[i]);	\
					}while(0);
/*******************************************************************************
* Function Name  : LEDSProcess
* Description    : Process LEDs glow
*******************************************************************************/
void LEDSProcess()
{
	const GPIO_TypeDef* LED_GPIO_PORTS[11] = {GPIOC, GPIOD, GPIOB, GPIOC, GPIOA, GPIOC, GPIOC,GPIOB,GPIOB,GPIOB,GPIOB};
	const uint16_t LED_GPIO_PINS[11] = {GPIO_Pin_13, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_11, GPIO_Pin_15, GPIO_Pin_10, GPIO_Pin_12,GPIO_Pin_7,GPIO_Pin_6,GPIO_Pin_5,GPIO_Pin_4};
	const uint16_t LED_FLASH_OFF_MASS[4] = {2000, 1000, 500, 0};	//delay in ms - led will be ON
	const uint16_t LED_FLASH_ON_MASS[4]  = {100,  400,  250, 0};  //delay in ms - led flash period

	for(uint8_t i=0; i<11; i++) //11 is total number of leds
	{
		if(LEDS[i].onOff == LED_OFF)
		{
			LED_RESET;
			continue;
		}

		if(LEDS[i].type == LED_LIGHT) //constant glow
		{
			LED_SET;
			continue;
		}

		if(LEDS[i].counter == 0) //light LED at new cycle
			LED_SET;

		if(LEDS[i].counter >= LED_FLASH_ON_MASS[LEDS[i].type])
			LED_RESET;

		if(LEDS[i].counter >= LED_FLASH_OFF_MASS[LEDS[i].type])
			LEDS[i].counter = 0;
		else
			LEDS[i].counter++;
	}
}


/*******************************************************************************
* Function Name  : LEDSet
* Description    : Set new state of LEDs
*******************************************************************************/
void LEDSet(uint8_t number, uint8_t state, uint8_t mode)
{
	LEDS[number].onOff = state ? LED_ON : LED_OFF;

	if(state == LED_OFF)
		mode = LED_LIGHT;

	if(LEDS[number].type == mode) //multiple call the same paramaters
		return;

	LEDS[number].type = mode;
	LEDS[number].counter = 0;
}

