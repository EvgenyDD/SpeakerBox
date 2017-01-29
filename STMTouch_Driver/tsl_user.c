#include "tsl_user.h"
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_rcc.h"

#include "hardware.h"

uint16_t TouchThreshold[11] = { 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50 };
uint16_t TouchMax[11] = { 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000 };


#define CHNL_NUM		(11)

int16_t TouchFilteredData[CHNL_NUM];

static uint16_t chnlOffValues[CHNL_NUM];
static uint16_t chnlOnValues[CHNL_NUM];
uint16_t chnlThreshold[CHNL_NUM] = { 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50 };

// Channel Source and Configuration: Always placed in ROM
TSL_ChannelSrc_T MyChannels_Src[TSLPRM_TOTAL_CHANNELS] =
{
	{ CHANNEL_0_SRC, CHANNEL_0_SAMPLE, CHANNEL_0_CHANNEL },
	{ CHANNEL_1_SRC, CHANNEL_1_SAMPLE, CHANNEL_1_CHANNEL },
	{ CHANNEL_2_SRC, CHANNEL_2_SAMPLE, CHANNEL_2_CHANNEL },
	{ CHANNEL_5_SRC, CHANNEL_5_SAMPLE, CHANNEL_5_CHANNEL },

	{ CHANNEL_3_SRC, CHANNEL_3_SAMPLE, CHANNEL_3_CHANNEL },
	{ CHANNEL_6_SRC, CHANNEL_6_SAMPLE, CHANNEL_6_CHANNEL },
	{ CHANNEL_7_SRC, CHANNEL_7_SAMPLE, CHANNEL_7_CHANNEL },
	{ CHANNEL_9_SRC, CHANNEL_9_SAMPLE, CHANNEL_9_CHANNEL },

	{ CHANNEL_4_SRC, CHANNEL_4_SAMPLE, CHANNEL_4_CHANNEL },
	{ CHANNEL_8_SRC, CHANNEL_8_SAMPLE, CHANNEL_8_CHANNEL },
    { CHANNEL_10_SRC, CHANNEL_10_SAMPLE, CHANNEL_10_CHANNEL },
};


// Banks
// Always placed in ROM
TSL_Bank_T MyBanks[3] =
{
	{
		&MyChannels_Src[0],
		BANK_0_NBCHANNELS
	},
	{
		&MyChannels_Src[4],
		BANK_1_NBCHANNELS
	},
	{
		&MyChannels_Src[8],
		BANK_2_NBCHANNELS
	}
};




/* Private functions prototype -----------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Initialize the STMTouch Driver*/
TSL_Globals_T TSL_Globals;
void TSL_user_Init()
{
	for(uint8_t i=0; i<CHNL_NUM; i++)
	{
		chnlOffValues[i] = 0;
		chnlOnValues[i] = 30000;
	}

	TSL_Globals.Bank_Array = MyBanks;

	// Initialization of the acquisition module
	TSL_acq_Init();

}

/* Execute STMTouch Driver main State machine
 * return: status Return TSL_STATUS_OK if the acquisition is done*/
TSL_Status_enum_T TSL_user_Action()
{
	static uint8_t bankIndex = 1;

	TSL_acq_BankConfig(bankIndex);  // Configure Bank
	TSL_acq_BankStartAcq();		  // Start Bank acquisition
	TSL_acq_BankGetResult(bankIndex);


	if(++bankIndex >= TSLPRM_TOTAL_BANKS)
	{
		bankIndex = 0;

		// Process Objects
	//	TSL_obj_GroupProcess(&MyObjGroup);

		return TSL_STATUS_OK; // All banks have been acquired and sensors processed
	}
	else
		return TSL_STATUS_BUSY;
}

int16_t offset[11] = {0,0,0,0,0,0,0,0,0,0,0};
int16_t TouchData[11];


void ProcessTouchGroup()
{
//	static uint8_t flag_flag = 2;
//
//	static uint32_t timeout = 0;
//	if(timeout < 10)
//	{
//		timeout++;
//		if(timeout >= 10)
//		{
//			flag_flag = 1;
//		}
//	}
//
//	static uint8_t cd = 0;
//
//	if(flag_flag == 0)
//		if(++cd >= 6)
//		{
//			cd = 0;
//			for(uint8_t r = 0; r < 11; r++)
//			{
//				int16_t diff = offset[r] - TouchData[r];
//				DebugSendNumWSpace(diff);
//				delay_ms(100);
//			}
//
//			DebugSendString("\n\n");
//		}
//
//	if(flag_flag == 1)
//	{
//		LEDSet(LED_RIGHT, 1, LED_LIGHT);
//		for(uint8_t i=0; i<11; i++)
//			offset[i] = TouchData[i];
//		flag_flag = 0;
//	}

#if 1
	for(uint8_t i=0; i<CHNL_NUM; i++)
	{
		//UTILS_LP_FAST(TouchFilteredData[i], TouchData[i], 0.2);
		TouchFilteredData[i] = TouchData[i];

		if((TouchFilteredData[i] > chnlOffValues[i])/* && (TouchFilteredData[i] < 1000)*/)
			chnlOffValues[i] = TouchFilteredData[i];

		if(TouchFilteredData[i] < chnlOnValues[i])
			chnlOnValues[i] = TouchFilteredData[i];

		if((chnlOffValues[i] - chnlThreshold[i]) > TouchFilteredData[i])
		{
			switch(i)
			{
			case TCH_RIGHT:
				LEDSet(LED_RIGHT, 1, 3);
				break;
			case TCH_LEFT:
				LEDSet(LED_LEFT, 1, 3);
				break;
			case TCH_UP:
				LEDSet(LED_UP, 1, 3);
				break;
			case TCH_DOWN:
				LEDSet(LED_DOWN, 1, 3);
				break;
			case TCH_CENTER:
				LEDSet(LED_MUTE, 1, 3);
				break;

			}
		}
		else
		{
			switch(i)
			{
			case TCH_RIGHT:
				LEDSet(LED_RIGHT, 0, 3);
				break;
			case TCH_LEFT:
				LEDSet(LED_LEFT, 0, 3);
				break;
			case TCH_UP:
				LEDSet(LED_UP, 0, 3);
				break;
			case TCH_DOWN:
				LEDSet(LED_DOWN, 0, 3);
				break;
			case TCH_CENTER:
				LEDSet(LED_MUTE, 0, 3);
				break;

			}
		}
	}
#endif

}
