/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TSL_USER_H
#define __TSL_USER_H

#include "tsl_acq_stm32l1xx_sw.h"
//==============================================================================
// IOs definition
//==============================================================================

// Channel IOs definition

//B1
#define CHANNEL_0_SRC    	((uint32_t)(GR7))
#define CHANNEL_0_SAMPLE    (TSL_GROUP7_IO1)
#define CHANNEL_0_CHANNEL   (TSL_GROUP7_IO4)

//B2
#define CHANNEL_1_SRC    	((uint32_t)(GR10))
#define CHANNEL_1_SAMPLE    (TSL_GROUP10_IO1)
#define CHANNEL_1_CHANNEL   (TSL_GROUP10_IO2)

//B3
#define CHANNEL_2_SRC    	((uint32_t)(GR1))
#define CHANNEL_2_SAMPLE    (TSL_GROUP1_IO4)
#define CHANNEL_2_CHANNEL   (TSL_GROUP1_IO3)

//B4
#define CHANNEL_3_SRC    	((uint32_t)(GR1))
#define CHANNEL_3_SAMPLE    (TSL_GROUP1_IO4)
#define CHANNEL_3_CHANNEL   (TSL_GROUP1_IO2)

//B5
#define CHANNEL_4_SRC    	((uint32_t)(GR1))
#define CHANNEL_4_SAMPLE   	(TSL_GROUP1_IO4)
#define CHANNEL_4_CHANNEL  	(TSL_GROUP1_IO1)


//LIN1
#define CHANNEL_10_SRC    	((uint32_t)(GR7))
#define CHANNEL_10_SAMPLE    (TSL_GROUP7_IO1)
#define CHANNEL_10_CHANNEL   (TSL_GROUP7_IO2)

//LIN2
#define CHANNEL_9_SRC    	((uint32_t)(GR7))
#define CHANNEL_9_SAMPLE    (TSL_GROUP7_IO1)
#define CHANNEL_9_CHANNEL   (TSL_GROUP7_IO3)

//LIN3
#define CHANNEL_8_SRC    	((uint32_t)(GR10))
#define CHANNEL_8_SAMPLE    (TSL_GROUP10_IO1)
#define CHANNEL_8_CHANNEL   (TSL_GROUP10_IO3)

//LIN4
#define CHANNEL_7_SRC    	((uint32_t)(GR10))
#define CHANNEL_7_SAMPLE    (TSL_GROUP10_IO1)
#define CHANNEL_7_CHANNEL   (TSL_GROUP10_IO4)

//LIN5
#define CHANNEL_6_SRC    	((uint32_t)(GR4))
#define CHANNEL_6_SAMPLE    (TSL_GROUP4_IO3)
#define CHANNEL_6_CHANNEL   (TSL_GROUP4_IO1)

//LIN6
#define CHANNEL_5_SRC    	((uint32_t)(GR4))
#define CHANNEL_5_SAMPLE    (TSL_GROUP4_IO3)
#define CHANNEL_5_CHANNEL   (TSL_GROUP4_IO2)



// Banks definition
#define BANK_0_NBCHANNELS    		(4)
#define BANK_0_SHIELD_SAMPLE        (0)
#define BANK_0_SHIELD_CHANNEL       (0)

#define BANK_1_NBCHANNELS    		(4)
#define BANK_1_SHIELD_SAMPLE        (0)
#define BANK_1_SHIELD_CHANNEL       (0)

#define BANK_2_NBCHANNELS    		(3)
#define BANK_2_SHIELD_SAMPLE        (0)
#define BANK_2_SHIELD_CHANNEL       (0)

enum{
	TCH_UP = 0,
	TCH_LEFT,
	TCH_CENTER,
	TCH_SL_1,
	TCH_RIGHT,
	TCH_SL_2,
	TCH_SL_3,
	TCH_SL_5,
	TCH_DOWN,
	TCH_SL_4,
	TCH_SL_6
};


// User Parameters
extern TSL_Bank_T MyBanks[];


void TSL_user_Init();
TSL_Status_enum_T TSL_user_Action();

void ProcessTouchGroup();

#endif /* __TSL_USER_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
