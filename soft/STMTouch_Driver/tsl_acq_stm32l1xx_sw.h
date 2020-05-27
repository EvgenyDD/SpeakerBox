/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TSL_ACQ_STM32L1XX_SW_H
#define __TSL_ACQ_STM32L1XX_SW_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "tsl_conf_stm32l1xx.h"

/* Defines -------------------------------------------------------------------*/

#ifndef const
#define const const
#endif

// SysTick enable/disable interrupt macros
#define enableInterrupts()  {SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;}
#define disableInterrupts() {SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;}

enum
{
	GR1, GR2, GR3, GR4, GR5, GR6, GR7, GR8, GR9, GR10, GR11
};

enum
{
	TSL_BANK_GPIOA = 0, TSL_BANK_GPIOB = 1, TSL_BANK_GPIOC = 2, TSL_BANK_GPIOE = 4, TSL_BANK_GPIOF = 5, TSL_BANK_GPIOG = 6
};

/** GPIOs list
 High significant nibble for the IO port (GPIOA:0,...,GPIOG:6)
 Low significant nibble for the IO number (pin0:0,...,pin15:F)
 */
enum
{
	PA0 = 0x00, /**< TSL_GROUP1_IO1 */
	PA1 = 0x01, PA2 = 0x02, PA3 = 0x03, PA6 = 0x06, /**< TSL_GROUP2_IO1 */
	PA7 = 0x07, PA8 = 0x08, PA9 = 0x09, PA10 = 0x0A, PA13 = 0x0D, /**< TSL_GROUP5_IO1 */
	PA14 = 0x0E, PA15 = 0x0F, PB0 = 0x10, /**< TSL_GROUP3_IO1 */
	PB1 = 0x11, PB2 = 0x12, PB4 = 0x14, /**< TSL_GROUP6_IO1 */
	PB5 = 0x15, PB6 = 0x16, PB7 = 0x17, PB12 = 0x1C, /**< TSL_GROUP7_IO1 */
	PB13 = 0x1D, PB14 = 0x1E, PB15 = 0x1F, PC0 = 0x20, /**< TSL_GROUP8_IO1 */
	PC1 = 0x21, PC2 = 0x22, PC3 = 0x23, PC4 = 0x24, PC5 = 0x25, PC6 = 0x26, PC7 = 0x27, PC8 = 0x28, PC9 = 0x29, PF6 = 0x56, /**< TSL_GROUP11_IO1 */
	PF7 = 0x57, PF8 = 0x58, PF9 = 0x59, PF10 = 0x5A, PF11 = 0x5B, PF12 = 0x5C, PF13 = 0x5D, PF14 = 0x5E, PF15 = 0x5F, PG0 = 0x60, /**< TSL_GROUP2_IO4 */
	PG1 = 0x61, PG2 = 0x62, PG3 = 0x63, PG4 = 0x64
};

/** GPIOs list:
 - High significant nibble for the IO port (GPIOA:0,...,GPIOG:6)
 - Low significant nibble for the IO number (pin0:0,...,pin15:F)
 */
enum
{
	TSL_GROUP1_IO1 = 0x00, /**< PA0 */
	TSL_GROUP1_IO2 = 0x01,
	TSL_GROUP1_IO3 = 0x02,
	TSL_GROUP1_IO4 = 0x03,
	TSL_GROUP2_IO1 = 0x06, /**< PA6 */
	TSL_GROUP2_IO2 = 0x07,
	TSL_GROUP4_IO1 = 0x08,
	TSL_GROUP4_IO2 = 0x09,
	TSL_GROUP4_IO3 = 0x0A,
	TSL_GROUP5_IO1 = 0x0D, /**< PA13 */
	TSL_GROUP5_IO2 = 0x0E,
	TSL_GROUP5_IO3 = 0x0F,
	TSL_GROUP3_IO1 = 0x10, /**< PB0 */
	TSL_GROUP3_IO2 = 0x11,
	TSL_GROUP3_IO3 = 0x12,
	TSL_GROUP6_IO1 = 0x14, /**< PB4 */
	TSL_GROUP6_IO2 = 0x15,
	TSL_GROUP6_IO3 = 0x16,
	TSL_GROUP6_IO4 = 0x17,
	TSL_GROUP7_IO1 = 0x1C, /**< PB12 */
	TSL_GROUP7_IO2 = 0x1D,
	TSL_GROUP7_IO3 = 0x1E,
	TSL_GROUP7_IO4 = 0x1F,
	TSL_GROUP8_IO1 = 0x20, /**< PC0 */
	TSL_GROUP8_IO2 = 0x21,
	TSL_GROUP8_IO3 = 0x22,
	TSL_GROUP8_IO4 = 0x23,
	TSL_GROUP9_IO1 = 0x24,
	TSL_GROUP9_IO2 = 0x25,
	TSL_GROUP10_IO1 = 0x26,
	TSL_GROUP10_IO2 = 0x27,
	TSL_GROUP10_IO3 = 0x28,
	TSL_GROUP10_IO4 = 0x29,
	TSL_GROUP11_IO1 = 0x56, /**< PF6 */
	TSL_GROUP11_IO2 = 0x57,
	TSL_GROUP11_IO3 = 0x58,
	TSL_GROUP11_IO4 = 0x59,
	TSL_GROUP11_IO5 = 0x5A,
	TSL_GROUP3_IO4 = 0x5B,
	TSL_GROUP3_IO5 = 0x5C,
	TSL_GROUP9_IO3 = 0x5D,
	TSL_GROUP9_IO4 = 0x5E,
	TSL_GROUP2_IO3 = 0x5F,
	TSL_GROUP2_IO4 = 0x60, /**< PG0 */
	TSL_GROUP2_IO5 = 0x61,
	TSL_GROUP7_IO5 = 0x62,
	TSL_GROUP7_IO6 = 0x63,
	TSL_GROUP7_IO7 = 0x64
};

/* Exported types ------------------------------------------------------------*/
//------------------------------------------------------------------------------
// Channel
//------------------------------------------------------------------------------
typedef struct
{
	uint8_t ind; /**< Index of source value */
	// For stm32l1x acquisition only
	uint8_t t_sample; /**< Indicates which GPIO.n is used for the sample */
	uint8_t t_channel; /**< Indicates which GPIO.n is used for the channel */
} TSL_ChannelSrc_T;


//------------------------------------------------------------------------------
// Bank
typedef struct
{
	TSL_ChannelSrc_T *ptrChnl; /**< Pointer to the Channel Source and Configuration */
	uint16_t NbChannels; /**< Number of channels in the bank */
} TSL_Bank_T;


typedef enum
{
  TSL_FALSE = 0, /**< A False value */
  TSL_TRUE  = 1  /**< A True value */
}
TSL_Bool_enum_T;

/** Generic status returned by functions */
typedef enum
{
  TSL_STATUS_OK      = 0, /**< The function has been executed correctly */
  TSL_STATUS_BUSY    = 1, /**< The function is in a Busy state */
  TSL_STATUS_ERROR   = 2  /**< The function has been executed not correctly */
} TSL_Status_enum_T;

/** Store all global variables shared between the STMTouch Driver and the Application.  */
typedef struct
{
  TSL_Bank_T *Bank_Array; /**< Pointer to the array containing all Banks */
  uint8_t This_Bank;   /**< Pointer to the current Bank */

}TSL_Globals_T;

extern TSL_Globals_T TSL_Globals;


/* Exported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
TSL_Status_enum_T TSL_acq_Init();
TSL_Status_enum_T TSL_acq_BankConfig(uint8_t idx_bk);
void TSL_acq_BankStartAcq();
uint16_t TSL_acq_GetMeas(uint8_t index);
TSL_Status_enum_T TSL_acq_BankGetResult(uint8_t idx_bk);


#endif /* __TSL_ACQ_STM32L1XX_SW_H */
