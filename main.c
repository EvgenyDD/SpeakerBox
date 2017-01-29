/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "stm32l1xx_rcc.h"
#include "stm32l1xx_syscfg.h"
#include "stm32l1xx_pwr.h"
#include "misc.h"
#include "soundcard.h"

#include "hardware.h"
#include "clock.h"
#include "usb_pwr.h"
#include <stdbool.h>


#include "display.h"
#include "debug.h"
#include "soundmixer.h"




/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define map(x, in_min, in_max, out_min, out_max) ( (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)


/* Private variables ---------------------------------------------------------*/
volatile uint32_t delay = 0;
uint32_t btnState = 0;

uint8_t failHSE= 0;

extern __IO uint32_t bDeviceState;
extern uint8_t chargeStatus;
extern uint8_t BTStatus;



uint8_t failOccured = 0;

static void selectMaxMin(uint16_t *maxMass, uint16_t *minMass, uint32_t len, uint16_t* max, uint16_t *min)
{
	*min = 0xFFFF;
	*max = 0;

	for(uint32_t i=0; i<len; i++)
	{
		if(maxMass[i] > *max)
			*max = maxMass[i];
		if(minMass[i] < *min)
			*min = minMass[i];
	}
}



static void ProcessChargeLevel();

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
 * Function Name  : __delay_ms.
 * Description    : Delay the code on the N cycles of SysTick Timer.
 * Input          : N Delay Cycles.
 *******************************************************************************/
void delay_ms(uint32_t nTime)
{
	delay = nTime;
	while(delay);
}


/*******************************************************************************
 * Function Name  : SysTick_Handler.
 * Description    : Handles SysTick Timer Interrupts every 1ms.
 *******************************************************************************/
void SysTick_Handler(void)
{
	if(delay)
		delay--;

	LEDSProcess();

	static uint16_t countdown = 0;
	if(++countdown >= 50)
	{
		countdown = 0;
		btnState = KeyboardHWScan();
	}

//	static uint16_t touchExecDelay = 0;
//	if(++touchExecDelay >= 100)
//	{
//		touchExecDelay = 0;
//		if(TSL_user_Action() == TSL_STATUS_OK)
//		{
//			ProcessTouchGroup();
//		}
//	}

#include "stm32l1xx_gpio.h"

//	GPIO_InitTypeDef GPIOInitStructure;
//	GPIOInitStructure.GPIO_Pin = GPIO_Pin_15;
//	GPIOInitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//	GPIO_Init(GPIOB, &GPIOInitStructure);


}

uint16_t datar[7][50];
			uint8_t ptrS[7];
			uint8_t ptrE[7];

			uint8_t modeOper, modeBL;

uint8_t fuckfuck = 0;

/*******************************************************************************
 * Function Name  : main
 *******************************************************************************/
int main(void)
{


	HardwareInit();

	RTCInit();
	DebugInit();

	TouchInit();

//	LEDSet(LED_POWER, LED_ON, LED_LIGHT);
	SoundCardInit();

	PowerEnableCharge();

	SysTickConfig(32000);

	LEDSet(LED_POWER, LED_ON, LED_LIGHT);
	delay_ms(1500);

//	if(!(btnState & BTN_POWER))
//	{
//		PowerEnterStandby();
//	}
	LEDSet(LED_POWER, LED_ON, LED_LIGHT);
	LEDSet(LED_BANK, LED_ON, LED_LIGHT);
	LEDSet(LED_MUTE, LED_ON, LED_LIGHT);
	while(btnState & BTN_POWER)
		delay_ms(10);
	LEDSet(LED_BANK, LED_OFF, LED_LIGHT);
	LEDSet(LED_MUTE, LED_OFF, LED_LIGHT);



	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
	GPIO_Init(GPIOC, &GPIO_InitStructure);



	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_RTCAccessCmd(ENABLE);


	modeOper = *(__IO uint32_t *) (RTC->BKP0R + 0);

	if(modeOper > 3)
	{
		modeOper = MODE_USB;
		*(__IO uint32_t *) (RTC->BKP0R + 0) = modeOper;
	}
	
	modeBL = *(__IO uint32_t *) (RTC->BKP8R + 0);

	if(modeBL > 3)
	{
		modeBL = MODE_OFF;
		*(__IO uint32_t *) (RTC->BKP0R + 8) = modeBL;
	}

	PowerEnableAMP();
	MixerOn();

	delay_ms(100);

	SoundMixerSetVolume(52);

	SoundMixerSetVolume(60);
	SoundMixerSetTreble(0);
	SoundMixerSetBass(2);

	SoundMixerSetGain(GAIN_0);

	switch(modeOper)
	{
	case MODE_EXT:
		SoundMixerSwitch(CH_EXT);
		LEDSet(LED_RIGHT, LED_ON, LED_LIGHT);
		break;

	case MODE_BT:
		SoundMixerSwitch(CH_BT);
		LEDSet(LED_LEFT, LED_ON, LED_LIGHT);
		PowerEnableBT();
		break;

	case MODE_USB:
		LEDSet(LED_DOWN, LED_ON, LED_LIGHT);
		SoundMixerSwitch(CH_DAC);
		break;
	}





	//PowerEnableBank();
	AMPMuteOn();
	//PowerEnableBT();
	PowerEnableDisplaySpectre();
	DisplayInit();


//	SetFMTXFreq(1012);

	//uint8_t data[6] = {8, 192, 224, 91, 103, 127};
	//PowerEnableDisplaySpectre();


//	PowerEnableRadio();
	delay_ms(10);
//	Si4703_Init();
	DisplayClear();

	while(1)
	{



		//LEDSet(LED_RIGHT, checkConnectedEarphones(), LED_LIGHT);
		//LEDSet(LED_UP, checkVExt(), LED_LIGHT);

		//		// Execute STMTouch Driver state machine
//		if(TSL_user_Action() == TSL_STATUS_OK)
//		{
//			ProcessTouchGroup();
//			delay_ms(10);
//		}
//		else
		{

			delay_ms(15);
//			extern int16_t touchKeyActivated[6];
//			LEDSet(LED_RIGHT, !checkConnectedEarphones(), LED_LIGHT);//fail
//			LEDSet(LED_UP, !checkVExt(), LED_LIGHT);//ok
//			LEDSet(LED_LEFT, !checkCharge(), LED_LIGHT);//ok
//			LEDSet(LED_DOWN, touchKeyActivated[1], LED_LIGHT); //ok
		//}

#if 1

//		LEDSet(LED_BANK, checkVUSB(), LED_LIGHT);
//		LEDSet(LED_MUTE, checkCharge(), LED_LIGHT);
			if(modeOper == MODE_BT)
			{
				if(BTStatus == BT_CONNECTED)
				{
					LEDSet(LED_LEFT, LED_ON, LED_LIGHT);
					AMPMuteOff();
				}
				else
				{
					LEDSet(LED_LEFT, LED_ON, LED_FLASH_1HZ);
					AMPMuteOn();
				}
			}

			static uint16_t prevBtnState = 0;
			if(btnState != prevBtnState)
			{
				if(prevBtnState == 0 && btnState)
				{
					switch(btnState)
					{
					case BTN_POWER:
						delay_ms(500);
						LEDSet(LED_UP, LED_ON, LED_LIGHT);
						PowerEnterStandby();
						break;

					case BTN_DR:
						AMPMuteOff();
						SoundMixerSwitch(CH_EXT);
						LEDSet(LED_LEFT, LED_OFF, LED_LIGHT);
						LEDSet(LED_DOWN, LED_OFF, LED_LIGHT);
						LEDSet(LED_RIGHT, LED_ON, LED_LIGHT);
						modeOper = MODE_EXT;
						PowerDisableBT();
						*(__IO uint32_t *) (RTC->BKP0R + 0) = modeOper;
						break;

					case BTN_UL:
						SoundMixerSwitch(CH_BT);
						LEDSet(LED_DOWN, LED_OFF, LED_LIGHT);
						LEDSet(LED_RIGHT, LED_OFF, LED_LIGHT);
						LEDSet(LED_LEFT, LED_ON, LED_LIGHT);
						PowerEnableBT();
						modeOper = MODE_BT;
						*(__IO uint32_t *) (RTC->BKP0R + 0) = modeOper;
						break;

					case BTN_DL:
						LEDSet(LED_LEFT, LED_OFF, LED_LIGHT);
						LEDSet(LED_RIGHT, LED_OFF, LED_LIGHT);
						LEDSet(LED_DOWN, LED_ON, LED_LIGHT);
						SoundMixerSwitch(CH_DAC);
						PowerDisableBT();
						modeOper = MODE_USB;
						*(__IO uint32_t *) (RTC->BKP0R + 0) = modeOper;
						break;

					case BTN_BANK:
					{
						static uint8_t muteIsOn = false;
						if(muteIsOn)
						{
							muteIsOn = false;
							LEDSet(LED_MUTE, LED_OFF, LED_LIGHT);
							AMPMuteOff();
						}
						else
						{
							muteIsOn = true;
							LEDSet(LED_MUTE, LED_ON, LED_LIGHT);
							AMPMuteOn();
						}
					}
						break;
						enum
						{
							DISP_NONE = 0, DISP_SPECTRE, DISP_SPECTRE_MOVE, DISP_RAINBOW
						};
					case BTN_MUTE:
					{
						if(++modeBL > 5)
						{
							modeBL = DISP_NONE;
							PowerDisableDisplaySpectre();
						}
						else
						{
							PowerEnableDisplaySpectre();
							DisplayInit();
						}
						*(__IO uint32_t *) (RTC->BKP0R + 8) = modeBL;
					}
						break;
					}
				}
				prevBtnState = btnState;
			}

			ProcessChargeLevel();

			LEDSet(LED_POWER, checkCharge(), LED_LIGHT);

			DisplayClear();

//			char s[25] = "";
//
//			itoa_(touchKeyActivated[5], s);
//			//Display_String(0, s, 0, RED, BLACK);
//			for(uint8_t i=0; i<10; i++)
//				DisplayPixel(i, 0, touchKeyActivated[i]?BLUE:GREEN);
//			DisplayPixel(0, 1, touchKeyActivated[10]?BLUE:GREEN);

			if(modeBL == DISP_RAINBOW)
			{
				rainbow();
			}
			else if(modeBL == DISP_SPECTRE || modeBL == DISP_SPECTRE_MOVE || modeBL == DISP_SPECTRE_BLUE || modeBL == DISP_SPECTRE_GREEN)
			{
				uint16_t band[10];

#define MAX_SND_VAL 	(1275)
#define SAMPLES_SIZE 	(40)

				static uint16_t maxSamples[SAMPLES_SIZE], minSamples[SAMPLES_SIZE];
				static uint8_t ptrMin = 0, ptrMax = 0;
				uint16_t outChannel[10];

				ScanSpectre(band);
				band[0] = band[0] * 3 / 2;

				static uint16_t maxVal, minVal;
				selectMaxMin(band, band, 5, &maxVal, &minVal);

				maxSamples[ptrMax++] = maxVal;
				minSamples[ptrMin++] = minVal;

				if(ptrMax == SAMPLES_SIZE)
					ptrMax = 0;

				if(ptrMin == SAMPLES_SIZE)
					ptrMin = 0;

				uint16_t max, min;
				selectMaxMin(maxSamples, minSamples, SAMPLES_SIZE, &max, &min);

				for(uint8_t i = 0; i < 10; i++)
					band[i] = map(band[i], min, max, 0, MAX_SND_VAL);

				outChannel[0] = band[0];
				outChannel[1] = band[0] * 5 / 9 + band[1] * 4 / 9;
				outChannel[2] = band[0] * 1 / 9 + band[1] * 8 / 9;
				outChannel[3] = band[1] * 6 / 9 + band[2] * 3 / 9;
				outChannel[4] = band[1] * 2 / 9 + band[2] * 7 / 9;
				outChannel[5] = band[2] * 7 / 9 + band[3] * 2 / 9;
				outChannel[6] = band[2] * 3 / 9 + band[3] * 6 / 9;
				outChannel[7] = band[3] * 8 / 9 + band[4] * 1 / 9;
				outChannel[8] = band[3] * 4 / 9 + band[4] * 5 / 9;
				outChannel[9] = band[4];

				if(modeBL == DISP_SPECTRE)
				{
					for(uint8_t i = 0; i < 10; i++)
						SetLinearHLine(i, outChannel[i], MAX_SND_VAL, 5, RGBColorWheel(i * 30));
				}
				else if(modeBL == DISP_SPECTRE_MOVE)
				{
					static uint16_t ddd = 0;
					static uint8_t preddd = 0;
					if(++preddd >= 3)
					{
						preddd = 0;
						ddd++;
					}
					for(uint8_t i = 0; i < 10; i++)
						SetLinearHLine(i, outChannel[i], MAX_SND_VAL, 5, RGBColorWheel(i * 30 + ddd));
				}
				else if(modeBL == DISP_SPECTRE_BLUE)
				{
					for(uint8_t i = 0; i < 10; i++)
						SetLinearHLine2Colour(i, outChannel[i], MAX_SND_VAL, 5, BLUE, RED);
				}
				else if(modeBL == DISP_SPECTRE_GREEN)
				{
					for(uint8_t i = 0; i < 10; i++)
						SetLinearHLine2Colour(i, outChannel[i], MAX_SND_VAL, 5, GREEN, RED);
				}
			}

			DisplayUpdate();
			extern uint8_t fuckin;
			fuckin = 0;

		}
#endif

		//XXX();
//			DisplayAngleUpLine(2,2,RED);
//			DisplayAngleUpLine(9,4,GREEN);
//			DisplayAngleUpLine(6,2,BLUE);

		//Display_String(0, "AB", 0, BLUE, GREEN);

//				char fff[30];
//				itoa_(vbat, fff);
//				strcat_(fff, "\n\n");
//				SH_SendString(fff);

		//delay_ms(10);
////		if(btnState&BTN_BANK)
////			RadioReadReg(0);
//
////		static uint8_t enbl = 0;
////		if(btnState & BTN_MUTE)
////		{
////			AMPMuteOn();
//////			Si4703_Read();
//////			Si4703_Write();
////
//////			if(enbl == 0)
//////			{
//////				enbl = 1;
//////				LEDSet(LED_MUTE, enbl, LED_LIGHT);
//////				//MixerSendConfig(64|3|4);
//////				delay_ms(1000);
//////			}
//////
//////			else
//////			{
//////				enbl = 0;
//////				LEDSet(LED_MUTE, enbl, LED_LIGHT);
//////			//MixerSendConfig(64|3);
//////				delay_ms(1000);
//////			}
////		}
//

////		static uint8_t yy = 0;
//		static uint8_t ddd=0;
//		if(0)//(++ddd>=1)
//		{
//			ddd = 0;
////
////			LEDSet(LED_BANK,( btnState&BTN_BANK ), LED_LIGHT);
////			if(yy==0)
////				yy = 1;
////			else
////				yy = 0;
//

//
//			for(uint8_t i=0; i<7; i++)
//			{
//				datar[i][ptrE[i]++]=data[i];
//				if(ptrE[i]>50)
//					ptrE[i]=0;
//			}
//
//			static uint16_t fdldl = 0;
//			if(++fdldl >= 50)
//			{
//				fdldl = 0;
//				DebugSendString("------SPECTRE------");
//				for(uint8_t i=0; i<7; i++)
//				{
//					uint32_t num =0;
//					for(uint8_t j=0; j<50; j++)
//						num += datar[i][j];
//					num/=50;
//					DebugSendNumWSpace(num);
//				}
//			}
//		}
//		delay_ms(3);

	}

	//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD,	DISABLE);

	PowerEnterStandby();
}


static void ProcessChargeLevel()
{
	static uint32_t vbat = 0;
	vbat += GetVBat();

	static uint8_t sts = 0;
	if(++sts >= 5)
	{
		vbat /= 5;
		if(vbat < 3300)
		{
			LEDSet(LED_0, LED_ON, LED_LIGHT);
			LEDSet(LED_1, LED_ON, LED_LIGHT);
			LEDSet(LED_2, LED_ON, LED_LIGHT);
			LEDSet(LED_3, LED_ON, LED_LIGHT);
			delay_ms(800);
			LEDSet(LED_3, LED_OFF, LED_LIGHT);
			delay_ms(800);
			LEDSet(LED_2, LED_OFF, LED_LIGHT);
			delay_ms(800);
			LEDSet(LED_1, LED_OFF, LED_LIGHT);
			delay_ms(800);
			LEDSet(LED_0, LED_OFF, LED_LIGHT);
			PowerEnterStandby();
		}
		if(vbat < 3400)
		{
			LEDSet(LED_0, LED_OFF, LED_LIGHT);
			LEDSet(LED_1, LED_OFF, LED_LIGHT);
			LEDSet(LED_2, LED_OFF, LED_LIGHT);
			LEDSet(LED_3, LED_OFF, LED_LIGHT);
		}
		if(vbat >= 3400 && vbat < 3468)
		{
			LEDSet(LED_0, LED_ON, LED_FLASH_05HZ);
			LEDSet(LED_1, LED_OFF, LED_LIGHT);
			LEDSet(LED_2, LED_OFF, LED_LIGHT);
			LEDSet(LED_3, LED_OFF, LED_LIGHT);
		}
		if(vbat >= 3468 && vbat < 3536)
		{
			LEDSet(LED_0, LED_ON, LED_FLASH_1HZ);
			LEDSet(LED_1, LED_OFF, LED_LIGHT);
			LEDSet(LED_2, LED_OFF, LED_LIGHT);
			LEDSet(LED_3, LED_OFF, LED_LIGHT);
		}
		if(vbat >= 3536 && vbat < 3605)
		{
			LEDSet(LED_0, LED_ON, LED_LIGHT);
			LEDSet(LED_1, LED_OFF, LED_LIGHT);
			LEDSet(LED_2, LED_OFF, LED_LIGHT);
			LEDSet(LED_3, LED_OFF, LED_LIGHT);
		}
		if(vbat >= 3605 && vbat < 3673)
		{
			LEDSet(LED_0, LED_ON, LED_LIGHT);
			LEDSet(LED_1, LED_ON, LED_FLASH_05HZ);
			LEDSet(LED_2, LED_OFF, LED_LIGHT);
			LEDSet(LED_3, LED_OFF, LED_LIGHT);
		}
		if(vbat >= 3673 && vbat < 3673)
		{
			LEDSet(LED_0, LED_ON, LED_LIGHT);
			LEDSet(LED_1, LED_ON, LED_FLASH_1HZ);
			LEDSet(LED_2, LED_OFF, LED_LIGHT);
			LEDSet(LED_3, LED_OFF, LED_LIGHT);
		}
		if(vbat >= 3673 && vbat < 3741)
		{
			LEDSet(LED_0, LED_ON, LED_LIGHT);
			LEDSet(LED_1, LED_ON, LED_LIGHT);
			LEDSet(LED_2, LED_OFF, LED_LIGHT);
			LEDSet(LED_3, LED_OFF, LED_LIGHT);
		}
		if(vbat >= 3741 && vbat < 3877)
		{
			LEDSet(LED_0, LED_ON, LED_LIGHT);
			LEDSet(LED_1, LED_ON, LED_LIGHT);
			LEDSet(LED_2, LED_ON, LED_FLASH_05HZ);
			LEDSet(LED_3, LED_OFF, LED_LIGHT);
		}
		if(vbat >= 3877 && vbat < 3945)
		{
			LEDSet(LED_0, LED_ON, LED_LIGHT);
			LEDSet(LED_1, LED_ON, LED_LIGHT);
			LEDSet(LED_2, LED_ON, LED_FLASH_1HZ);
			LEDSet(LED_3, LED_OFF, LED_LIGHT);
		}
		if(vbat >= 3945 && vbat < 4014)
		{
			LEDSet(LED_0, LED_ON, LED_LIGHT);
			LEDSet(LED_1, LED_ON, LED_LIGHT);
			LEDSet(LED_2, LED_ON, LED_LIGHT);
			LEDSet(LED_3, LED_OFF, LED_LIGHT);
		}
		if(vbat >= 4014 && vbat < 4082)
		{
			LEDSet(LED_0, LED_ON, LED_LIGHT);
			LEDSet(LED_1, LED_ON, LED_LIGHT);
			LEDSet(LED_2, LED_ON, LED_LIGHT);
			LEDSet(LED_3, LED_ON, LED_FLASH_05HZ);
		}
		if(vbat >= 4082 && vbat < 4150)
		{
			LEDSet(LED_0, LED_ON, LED_LIGHT);
			LEDSet(LED_1, LED_ON, LED_LIGHT);
			LEDSet(LED_2, LED_ON, LED_LIGHT);
			LEDSet(LED_3, LED_ON, LED_FLASH_1HZ);
		}
		if(vbat >= 4150)
		{
			LEDSet(LED_0, LED_ON, LED_LIGHT);
			LEDSet(LED_1, LED_ON, LED_LIGHT);
			LEDSet(LED_2, LED_ON, LED_LIGHT);
			LEDSet(LED_3, LED_ON, LED_LIGHT);
		}

		vbat = 0;

		sts = 0;

		if(GetVBat() > 4050)
			PowerDisableCharge();

	}
}
