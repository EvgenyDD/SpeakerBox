/* Includes ------------------------------------------------------------------*/
#include "touch.h"
#include "misc.h"
#include "stm32l1xx_tim.h"
#include "hardware.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define UTILS_LP_FAST(value, sample, filter_constant) (value -= (filter_constant) * (value - (sample)))


/* Private variables ---------------------------------------------------------*/
static GPIO_TypeDef* TCH_PORT[11] = {GPIOC, GPIOB, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOC, GPIOC, GPIOB, GPIOB};
static uint16_t TCH_PIN[11] = {7, 15, 2, 0, 1, 9, 8, 9, 8, 14, 13};

static uint16_t tchMax[11];
static uint16_t tchMin[11];
static float tchThr[11];
static float tchFlt[11];


/* Extern variables ----------------------------------------------------------*/
extern uint16_t touchKeyActivated[6];


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : TouchInit
* Description    : Initialize touch module
*******************************************************************************/
void TouchInit()
{
	return;
	for(uint8_t i = 0; i < 11; i++)
	{
		/* Speed mode configuration */
		TCH_PORT[i]->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (TCH_PIN[i] * 2));
		TCH_PORT[i]->OSPEEDR |= ((uint32_t)(GPIO_Speed_10MHz) << (TCH_PIN[i] * 2));

		/* Output mode configuration */
		TCH_PORT[i]->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)TCH_PIN[i]));
		TCH_PORT[i]->OTYPER |= (uint16_t)(((uint16_t) GPIO_OType_PP) << ((uint16_t)TCH_PIN[i]));  //have a sense?

		tchMax[i] = 0x0000;
		tchMin[i] = 0xFFFF;
		tchThr[i] = 15;
	}

	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_InitStructure.TIM_Prescaler = 1000-1;
	TIM_InitStructure.TIM_ClockDivision = 0;
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStructure.TIM_Period = 100;//3200  freq==32000Hz / val
	TIM_TimeBaseInit(TIM2, &TIM_InitStructure);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

uint8_t fuckin = 1;
/*******************************************************************************
* Function Name  : ProcessTouchButtons
*******************************************************************************/
static void ProcessTouchButtons(float *ptrData)
{
	return;
	if(fuckin)
	{
		return;
	}

	static uint8_t startup = 1;
	if(startup)
	{
		startup = 0;
		for(uint8_t i=0; i<11; i++)
		{
			tchFlt[i] = ptrData[i];
		}
	}

	for(uint8_t i=0; i<11; i++)
	{
		UTILS_LP_FAST(tchFlt[i], ptrData[i], 0.005);

		if(tchMax[i] < tchFlt[i])
			tchMax[i] = tchFlt[i];

		if(tchMin[i] > tchFlt[i])
			tchMin[i] = tchFlt[i];

//		if(i < 5)
		{
			//if(tchMax[i] - tchMin[i] > tchThr[i])
			{
				if(ptrData[i] > (tchFlt[i]+1.0)/*(tchMax[i] + tchMin[i]) / 2*/)
					touchKeyActivated[i] = 1;
				else
					touchKeyActivated[i] = 0;
			}
		}
//		else
//		{
//			if(tchMax[i] - tchMin[i] > tchThr[i])
//			{
//				if(tchFlt[i] > (tchMax[i] + tchMin[i]) / 2)
//					touchKeyActivated[i] = 0;
//
////				break;
//			}
//
////			if(i == 10)
////			{
////				touchKeyActivated[5] = -1;
////			}
//		}
	}
}


/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : Handler
*******************************************************************************/
void TIM2_IRQHandler()
{
	static uint16_t touchCounters[11];
	static uint8_t phaseScan = 0;

	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  // clear the interrupt flag

		touchCounters[phaseScan] = 0.0;

		// set to output
		TCH_PORT[phaseScan]->MODER &= ~(GPIO_MODER_MODER0 << (TCH_PIN[phaseScan] * 2));
		TCH_PORT[phaseScan]->MODER |= (((uint32_t) GPIO_Mode_OUT) << (TCH_PIN[phaseScan] * 2));
		// set output high
		TCH_PORT[phaseScan]->BSRRL = 1 << TCH_PIN[phaseScan];
		// set to input nopull
		TCH_PORT[phaseScan]->MODER &= ~(GPIO_MODER_MODER0 << (TCH_PIN[phaseScan] * 2));

		while(((TCH_PORT[phaseScan]->IDR & (1 << TCH_PIN[phaseScan])) != 0) && (touchCounters[phaseScan] < 1200))
			touchCounters[phaseScan]++;

		if(++phaseScan > 10)
		{
			phaseScan = 0;
			ProcessTouchButtons(touchCounters);
		}
	}
}

