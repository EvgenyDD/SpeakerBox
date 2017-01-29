/* Includes ------------------------------------------------------------------*/
#include "tsl_acq_stm32l1xx_sw.h"
#include "stm32l1xx_rcc.h"


/* Private typedefs ----------------------------------------------------------*/

// Register configuration
typedef struct
{
	unsigned int RI_ASCR :3;
	unsigned int RI_ASCR_bit :5;
} TSL_RIConf_t;

/* Private defines -----------------------------------------------------------*/
#define SIZEOFBANKCONF (17) //2 mask RIRs + 5 ports x 3 mask registers(MODER input, output, ODR) => 17 registers

/* Private macros ------------------------------------------------------------*/
#define TSL_CHANNEL_PORT(channel)  (channel >> 4)
#define TSL_CHANNEL_IO(channel)    (channel & 0x0F)

#define TSL_RI_HYSCR_MASK(channel) (1 << TSL_CHANNEL_IO(channel))

#define TSL_RCC_AHBENR_Config(channel) (RCC->AHBENR |= TSL_GPIO_Clock_LookUpTable[TSL_CHANNEL_PORT(channel)])

#define TSL_RI_HYSCR_Config(channel)       (*TSL_RI_HYSCR_LookUpTable[TSL_CHANNEL_PORT(channel)] |= TSL_RI_HYSCR_MASK(channel))

#define TSL_GPIO_MODER_IN_Config(channel)      (TSL_GPIO_LookUpTable[TSL_CHANNEL_PORT(channel)]->MODER &= (uint32_t)(~(3 << (2 * TSL_CHANNEL_IO(channel)))))
#define TSL_GPIO_MODER_OUT_Config(channel)     (TSL_GPIO_LookUpTable[TSL_CHANNEL_PORT(channel)]->MODER = (TSL_GPIO_LookUpTable[TSL_CHANNEL_PORT(channel)]->MODER & (uint32_t)(~(3 << (2 * TSL_CHANNEL_IO(channel))))) | (1 << (2 * TSL_CHANNEL_IO(channel))))
#define TSL_GPIO_PUPDR_NO_PUPD_Config(channel) (TSL_GPIO_LookUpTable[TSL_CHANNEL_PORT(channel)]->PUPDR &= (uint32_t)(~(3 << (2 * TSL_CHANNEL_IO(channel)))))
#define TSL_GPIO_OTYPER_PP_Config(channel)     (TSL_GPIO_LookUpTable[TSL_CHANNEL_PORT(channel)]->OTYPER &= (uint32_t)(~(1 << TSL_CHANNEL_IO(channel))))
#define TSL_GPIO_OSPEEDR_VL_Config(channel)    (TSL_GPIO_LookUpTable[TSL_CHANNEL_PORT(channel)]->OSPEEDR &= (uint32_t)~(3 << (2 * TSL_CHANNEL_IO(channel))))
#define TSL_GPIO_BS_Config(channel)            (TSL_GPIO_LookUpTable[TSL_CHANNEL_PORT(channel)]->BSRRL = (uint16_t)(1 << (TSL_CHANNEL_IO(channel))))
#define TSL_GPIO_BR_Config(channel)            (TSL_GPIO_LookUpTable[TSL_CHANNEL_PORT(channel)]->BSRRH = (uint16_t)(1 << (TSL_CHANNEL_IO(channel))))

/* Private variables ---------------------------------------------------------*/
uint32_t TSL_BankSampleConf[SIZEOFBANKCONF];
uint32_t TSL_BankChannelConf[SIZEOFBANKCONF];
uint32_t tab_MeasurementCounter[11];
TSL_Bank_T *bank;
uint16_t NumberOfChannels = 0;
TSL_Status_enum_T TSL_Acq_Status = TSL_STATUS_BUSY;
uint16_t GroupToCheck = 0;

uint32_t TSL_GPIO_Clock_LookUpTable[] =
{
	RCC_AHBPeriph_GPIOA, RCC_AHBPeriph_GPIOB, RCC_AHBPeriph_GPIOC, RCC_AHBPeriph_GPIOD, RCC_AHBPeriph_GPIOE, RCC_AHBPeriph_GPIOF,
	RCC_AHBPeriph_GPIOG, RCC_AHBPeriph_GPIOH };
GPIO_TypeDef *TSL_GPIO_LookUpTable[] =
{ GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH };

uint16_t *TSL_RI_HYSCR_LookUpTable[] =
{
	(uint16_t *) &RI->HYSCR1, (uint16_t *) &RI->HYSCR1 + 1, (uint16_t *) &RI->HYSCR2, (uint16_t *) &RI->HYSCR2 + 1,
	(uint16_t *) &RI->HYSCR3, (uint16_t *) &RI->HYSCR3 + 1, (uint16_t *) &RI->HYSCR4, (uint16_t *) &RI->HYSCR4 + 1 };
const TSL_RIConf_t TSL_RI_Conf_LookUpTable[101] =
{
{ 0, 0 },
{ 0, 1 },
{ 0, 2 },
{ 0, 3 },
{ 0, 0 }, //padding
{ 0, 0 }, //padding
	{ 0, 6 },
	{ 0, 7 },
	{ 1, 9 },
	{ 1, 10 },
	{ 1, 11 },
	{ 1, 15 },
	{ 0, 0 }, //padding
	{ 1, 6 },
	{ 1, 7 },
	{ 1, 8 },

	{ 0, 8 },
	{ 0, 9 },
	{ 1, 16 },
	{ 0, 0 }, //padding
	{ 1, 4 },
	{ 1, 5 },
	{ 1, 27 },
	{ 1, 28 },
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 18 },
	{ 0, 19 },
	{ 0, 20 },
	{ 0, 21 },

	{ 0, 10 },
	{ 0, 11 },
	{ 0, 12 },
	{ 0, 13 },
	{ 0, 14 },
	{ 0, 15 },
	{ 1, 0 },
	{ 1, 1 },
	{ 1, 2 },
	{ 1, 3 },
	{ 1, 29 },
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding

	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding

	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding

	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 0 }, //padding
	{ 0, 27 },
	{ 0, 28 },
	{ 0, 29 },
	{ 0, 30 },
	{ 0, 16 },
	{ 1, 17 },
	{ 1, 18 },
	{ 1, 19 },
	{ 1, 20 },
	{ 1, 21 },

	{ 1, 22 },
	{ 1, 23 },
	{ 1, 24 },
	{ 1, 25 },
	{ 1, 26 } };

uint32_t GPIOA_IDR_Mask = 0;
uint32_t GPIOB_IDR_Mask = 0;
uint32_t GPIOC_IDR_Mask = 0;


#if (TSLPRM_USE_SPREAD_SPECTRUM > 0)
uint8_t SpreadCounter = TSLPRM_SPREAD_MIN;
#endif


#pragma GCC push_options
#pragma GCC optimize ("O0")

/**
 * @brief  Software delay (private routine)
 * @param  val Wait delay
 * With fHCLK = 32MHz: 1 = ~1탎, 50 = ~14탎, 100 = ~25탎, 200 = ~50탎
 * @retval None
 */
void SoftDelay(uint16_t val)
{
	__IO uint16_t idx;
	for(idx = val; idx > 0; idx--)
	{
	}
}


#pragma GCC push_options
#pragma GCC optimize ("O0")

/**
 * @brief  Spread Spectrum using a variable software delay.
 * @param  None
 * @retval None
 */
//void SwSpreadSpectrum()
//{
//	uint8_t idx;
//
//	SpreadCounter++;
//
//	if(SpreadCounter == TSLPRM_SPREAD_MAX)
//	{
//		SpreadCounter = TSLPRM_SPREAD_MIN;
//	}
//
//	idx = SpreadCounter;
//
//	while(--idx)
//	{
//	}
//}


/* Private functions prototype -----------------------------------------------*/
void SoftDelay(uint16_t val);

#if (TSLPRM_USE_SPREAD_SPECTRUM > 0)
__INLINE void SwSpreadSpectrum();
#endif

void TSL_BankConf(uint32_t * BankConf, uint8_t Conf);
void TSL_acq_GroupDone(uint16_t EndedGroup);

/**
 * @brief  Configures the acquisition module.
 * @param[in] BankConf Pointer to the bank to configure
 * @param[in] Conf Configuration
 * @retval None
 */
void TSL_BankConf(uint32_t *BankConf, uint8_t Conf)
{
	BankConf[TSL_RI_Conf_LookUpTable[Conf].RI_ASCR] |= (1 << (TSL_RI_Conf_LookUpTable[Conf].RI_ASCR_bit));

	switch(TSL_CHANNEL_PORT(Conf))
	{
	case TSL_BANK_GPIOA:
		BankConf[2] |= (3 << (2 * (TSL_CHANNEL_IO(Conf)))); //MODER input
		BankConf[3] |= (1 << (2 * (TSL_CHANNEL_IO(Conf)))); //MODER output
		BankConf[4] |= (1 << (TSL_CHANNEL_IO(Conf))); //ODR
		break;
	case TSL_BANK_GPIOB:
		BankConf[5] |= (3 << (2 * (TSL_CHANNEL_IO(Conf)))); //MODER input
		BankConf[6] |= (1 << (2 * (TSL_CHANNEL_IO(Conf)))); //MODER output
		BankConf[7] |= (1 << (TSL_CHANNEL_IO(Conf))); //ODR
		break;
	case TSL_BANK_GPIOC:
		BankConf[8] |= (3 << (2 * (TSL_CHANNEL_IO(Conf)))); //MODER input
		BankConf[9] |= (1 << (2 * (TSL_CHANNEL_IO(Conf)))); //MODER output
		BankConf[10] |= (1 << (TSL_CHANNEL_IO(Conf))); //ODR
		break;
	default:
		break;
	}
}

/**
 * @brief  Initializes the acquisition module.
 * @param  None
 * @retval None
 */
TSL_Status_enum_T TSL_acq_Init()
{
	const TSL_Bank_T *LocalBank = &(TSL_Globals.Bank_Array[0]);
	uint16_t NumberOfBanks = TSLPRM_TOTAL_BANKS;
	uint16_t LocalNumberOfChannels = 0;
	uint8_t idx_bk;
	uint8_t idx_ch;
	const TSL_ChannelSrc_T *ptrChnl = LocalBank->ptrChnl; // Pointer to the current channel

	/* Enables the comparator interface clock */
	RCC->APB1ENR |= RCC_APB1Periph_COMP;

	//====================
	// GPIOs configuration
	//====================
	for(idx_bk = 0; idx_bk < NumberOfBanks; idx_bk++)
	{
		LocalBank = &(TSL_Globals.Bank_Array[idx_bk]);
		ptrChnl = LocalBank->ptrChnl;

		LocalNumberOfChannels = LocalBank->NbChannels;

		for(idx_ch = 0; idx_ch < LocalNumberOfChannels; idx_ch++)
		{
			/* Enables GPIOs clock */
			TSL_RCC_AHBENR_Config(ptrChnl->t_sample);
			TSL_RCC_AHBENR_Config(ptrChnl->t_channel);

			// Bank/channel configuration
			/* Disables Hysteresis Register */
			TSL_RI_HYSCR_Config(ptrChnl->t_sample);
			/* Output PP config */
			TSL_GPIO_OTYPER_PP_Config(ptrChnl->t_sample);
			TSL_GPIO_OTYPER_PP_Config(ptrChnl->t_channel);
			/* 400kHz config */
			TSL_GPIO_OSPEEDR_VL_Config(ptrChnl->t_sample);
			TSL_GPIO_OSPEEDR_VL_Config(ptrChnl->t_channel);
			/* No pull up/pull down config */
			TSL_GPIO_PUPDR_NO_PUPD_Config(ptrChnl->t_sample);
			TSL_GPIO_PUPDR_NO_PUPD_Config(ptrChnl->t_channel);
			/* Set ODR */
			TSL_GPIO_BR_Config(ptrChnl->t_sample);
			TSL_GPIO_BR_Config(ptrChnl->t_channel);
			/* Output mode */
			TSL_GPIO_MODER_OUT_Config(ptrChnl->t_sample);
			TSL_GPIO_MODER_OUT_Config(ptrChnl->t_channel);

			ptrChnl++;
		}
	}

	/* Enable RI Switch */
	RI->ASCR1 &= (uint32_t)(~0x80000000); // ADC analog switches open !!!

	return TSL_STATUS_OK;
}

/**
 * @brief Configures a Bank.
 * @param[in] idx_bk  Index of the Bank to configure
 * @retval Status
 */
TSL_Status_enum_T TSL_acq_BankConfig(uint8_t idx_bk)
{
	bank = &(TSL_Globals.Bank_Array[idx_bk]);

	for(uint8_t index = 0; index < SIZEOFBANKCONF; index++)
	{
		TSL_BankSampleConf[index] = 0x00000000;
		TSL_BankChannelConf[index] = 0x00000000;
	}

	NumberOfChannels = TSL_Globals.Bank_Array[idx_bk].NbChannels;
	GroupToCheck = 0; //init group to check

	TSL_ChannelSrc_T *ptrChnl; // Pointer to the current channel
	ptrChnl = TSL_Globals.Bank_Array[idx_bk].ptrChnl;

	for(uint8_t i = 0; i < NumberOfChannels; i++)
	{
		TSL_BankConf(TSL_BankSampleConf, ptrChnl->t_sample);
		TSL_BankConf(TSL_BankChannelConf, ptrChnl->t_channel);
		GroupToCheck |= (1 << (ptrChnl->ind));
		ptrChnl++;
	}

	GPIOA_IDR_Mask = TSL_BankSampleConf[4];
	GPIOB_IDR_Mask = TSL_BankSampleConf[7];
	GPIOC_IDR_Mask = TSL_BankSampleConf[10];
	return TSL_STATUS_OK;
}

/**
 * @brief Check which group is not over
 * @param[in] EndedGroup
 * @retval None
 */
void TSL_acq_GroupDone(uint16_t EndedGroup)
{
	for(uint16_t i = 0; i < 11; i++)
		if((EndedGroup & (1 << i)) != (1 << i))
			tab_MeasurementCounter[i] = 8000 + 1;
}

/**
 * @brief Start acquisition on a previously configured bank
 * @param None
 * @retval None
 */
void TSL_acq_BankStartAcq()
{
	static uint8_t ee = 10;
	TSL_Bank_T *LocalBank = &(TSL_Globals.Bank_Array[0]);
	uint16_t NumberOfBanks = TSLPRM_TOTAL_BANKS;
	uint16_t LocalNumberOfChannels = 0;
	uint8_t BankIndex;

	uint16_t MeasurementCounter = 0;
	TSL_ChannelSrc_T *ptrChnl;
	uint8_t idx_ch;
	uint16_t GroupToCheckMask = 0;
	uint32_t GPIO_IDR_Mask = 0;
	uint8_t Check_Input = 0;


	uint16_t TSL_GPIOA_IDR = 0;
	uint16_t TSL_GPIOB_IDR = 0;
	uint16_t TSL_GPIOC_IDR = 0;
	uint16_t GPIO_IDR = 0;

	/* Open the analog switches */
	RI->ASCR1 &= (uint32_t)(~(TSL_BankSampleConf[0] | TSL_BankChannelConf[0]));
	RI->ASCR2 &= (uint32_t)(~(TSL_BankSampleConf[1] | TSL_BankChannelConf[1]));

	/* All IO to pushpull LOW for discharging all capacitors (Ctouch and Csense) */
	/* Discharging sampling capacitor and CTouch */
	GPIOA->ODR &= (uint32_t)(~(TSL_BankSampleConf[4] | TSL_BankChannelConf[4]));
	GPIOA->MODER = (GPIOA->MODER & (uint32_t)(~(TSL_BankSampleConf[2] | TSL_BankChannelConf[2]))) | (TSL_BankSampleConf[3] | TSL_BankChannelConf[3]);

	GPIOB->ODR &= (uint32_t)(~(TSL_BankSampleConf[7] | TSL_BankChannelConf[7]));
	GPIOB->MODER = (GPIOB->MODER & (uint32_t)(~(TSL_BankSampleConf[5] | TSL_BankChannelConf[5]))) | (TSL_BankSampleConf[6] | TSL_BankChannelConf[6]);

	GPIOC->ODR &= (uint32_t)(~(TSL_BankSampleConf[10] | TSL_BankChannelConf[10]));
	GPIOC->MODER = (GPIOC->MODER & (uint32_t)(~(TSL_BankSampleConf[8] | TSL_BankChannelConf[8]))) | (TSL_BankSampleConf[9] | TSL_BankChannelConf[9]);

	/* Wait a while for a good discharging of all capacitors */
	SoftDelay(50); // ~14탎 with fHCLK = 32MHz
	//this time depends of the size of the sampling capacitor

	/* All IO in input floating */
	GPIOA->MODER &= (uint32_t)(~(TSL_BankSampleConf[2] | TSL_BankChannelConf[2]));
	GPIOB->MODER &= (uint32_t)(~(TSL_BankSampleConf[5] | TSL_BankChannelConf[5]));
	GPIOC->MODER &= (uint32_t)(~(TSL_BankSampleConf[8] | TSL_BankChannelConf[8]));

	/* set the IO to Vdd (io in push-pull HIGH when in output mode) */
	GPIOA->ODR |= (TSL_BankSampleConf[4] | TSL_BankChannelConf[4]); /* HIGH level */
	GPIOB->ODR |= (TSL_BankSampleConf[7] | TSL_BankChannelConf[7]); /* HIGH level */
	GPIOC->ODR |= (TSL_BankSampleConf[10] | TSL_BankChannelConf[10]); /* HIGH level */

	/* Close the sampling capacitor analog switch */
	RI->ASCR1 |= (TSL_BankSampleConf[0]);
	RI->ASCR2 |= (TSL_BankSampleConf[1]);


	/* Loop while all the 1st channel of each group have not reach the VIH level */
	do
	{
		/* Charging Ctouch by connecting the IO to Vdd (io in push-pull HIGH) */
//		GPIOA->MODER |= (TSL_BankChannelConf[3]); /* Output push pull config */
//		GPIOB->MODER |= (TSL_BankChannelConf[6]); /* Output push pull config */
//		GPIOC->MODER |= (TSL_BankChannelConf[9]); /* Output push pull config */

		/* test GPIOx->IDR bit + group configuration for each channel */

		TSL_GPIOA_IDR = GPIOA->IDR;
		if ((TSL_GPIOA_IDR & GPIOA_IDR_Mask) != 0)
		{
			Check_Input = 1;
			GPIOA_IDR_Mask &= (uint32_t)(~TSL_GPIOA_IDR);
		}

		TSL_GPIOB_IDR = GPIOB->IDR;
		if ((TSL_GPIOB_IDR & GPIOB_IDR_Mask) != 0)
		{
			Check_Input = (1 << 1);
			GPIOB_IDR_Mask &= (uint32_t)(~TSL_GPIOB_IDR);
		}

		TSL_GPIOC_IDR = GPIOC->IDR;
		if ((TSL_GPIOC_IDR & GPIOC_IDR_Mask) != 0)
		{
			Check_Input = (1 << 2);
			GPIOC_IDR_Mask &= (uint32_t)(~TSL_GPIOC_IDR);
		}

		if(Check_Input)
		{
			ptrChnl = bank->ptrChnl;
			for(idx_ch = 0; idx_ch < NumberOfChannels; idx_ch++)
			{
				GroupToCheckMask = (1 << (ptrChnl->ind));
				if((GroupToCheck & GroupToCheckMask) == (GroupToCheckMask))
				{
					GPIO_IDR_Mask = (1 << TSL_CHANNEL_IO(ptrChnl->t_sample));

					switch (TSL_CHANNEL_PORT(ptrChnl->t_sample))
					{
					case 0: GPIO_IDR = TSL_GPIOA_IDR; break;
					case 1: GPIO_IDR = TSL_GPIOB_IDR; break;
					case 2: GPIO_IDR = TSL_GPIOC_IDR; break;

					default:
						break;
					}

					if((GPIO_IDR & GPIO_IDR_Mask) == GPIO_IDR_Mask)
					{
						tab_MeasurementCounter[ptrChnl->ind] = MeasurementCounter;
						GroupToCheck &= (uint32_t)(~(1 << (ptrChnl->ind)));
						Check_Input &= (uint32_t)(~(1 << TSL_CHANNEL_PORT(ptrChnl->t_sample)));
					}
				}
				ptrChnl++;
			}
		}

		MeasurementCounter++;

		/* Configure All channels in input floating */
//		GPIOA->MODER &= (uint32_t)(~(TSL_BankChannelConf[2]));
//		GPIOB->MODER &= (uint32_t)(~(TSL_BankChannelConf[5]));
//		GPIOC->MODER &= (uint32_t)(~(TSL_BankChannelConf[8]));

		/* Charging the Csense cap with connecting it to Ctouch by closing the analog switch */
//		RI->ASCR1 |= (TSL_BankChannelConf[0]);
//		RI->ASCR2 |= (TSL_BankChannelConf[1]);
//
//		RI->ASCR1 &= (uint32_t)(~(TSL_BankChannelConf[0]));
//		RI->ASCR2 &= (uint32_t)(~(TSL_BankChannelConf[1]));

		/*it's better to implement this like that because it's much more faster than to put this test in the "while test" below */
		if(MeasurementCounter > 4000)
		{
			TSL_acq_GroupDone(GroupToCheck);
			__NOP();
			break;
		}

ee--;
	} while(0/*GroupToCheck != 0*/);


	//====================
	// All GPIOs in PP Low
	//====================
	for(BankIndex = 0; BankIndex < NumberOfBanks; BankIndex++)
	{
		LocalBank = &(TSL_Globals.Bank_Array[BankIndex]);
		ptrChnl = LocalBank->ptrChnl;

		LocalNumberOfChannels = LocalBank->NbChannels;

		for(idx_ch = 0; idx_ch < LocalNumberOfChannels; idx_ch++)
		{
			TSL_GPIO_BR_Config(ptrChnl->t_sample);
			TSL_GPIO_BR_Config(ptrChnl->t_channel);
			TSL_GPIO_MODER_OUT_Config(ptrChnl->t_sample);
			TSL_GPIO_MODER_OUT_Config(ptrChnl->t_channel);

			ptrChnl++;
		}
	}
}


/**
 * @brief Return the current measure
 * @param[in] index Index of the measure source
 * @retval Measure
 */
uint16_t TSL_acq_GetMeas(uint8_t index)
{
	return (tab_MeasurementCounter[index]);
}



extern int16_t TouchData[30];
/**
 * @brief Read all channels measurement of a Bank, calculate Delta
 * @param[in]  idx_bk  Index of the Bank to access
 * @param[in]  mfilter Pointer to the Measure filter function
 * @param[in]  dfilter Pointer to the Delta filter function
 * @retval Status
 */
TSL_Status_enum_T TSL_acq_BankGetResult(uint8_t idx_bk)
{
	TSL_ChannelSrc_T *pchSrc = TSL_Globals.Bank_Array[idx_bk].ptrChnl;

	// For all channels in the bank copy the measure + calculate delta and store them.
	for(uint8_t idx_ch = 0; idx_ch < TSL_Globals.Bank_Array[idx_bk].NbChannels; idx_ch++)
	{
		// Get the new measure (the access is different between acquisitions)
		TouchData[idx_bk * 4 + idx_ch] = TSL_acq_GetMeas(pchSrc->ind);
		pchSrc++;
	}

	return TSL_STATUS_OK;
}


