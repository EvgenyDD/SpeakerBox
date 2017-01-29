/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _HARDWARE_H
#define _HARDWARE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "stm32l1xx_rcc.h"
#include "stm32l1xx_adc.h"
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_exti.h"
#include "stm32l1xx_i2c.h"
#include "stm32l1xx_pwr.h"
#include "stm32l1xx_tim.h"
#include "misc.h"

/* Exported macro ------------------------------------------------------------*/
#define BitSet(p,m) ((p) |= (1<<(m)))
#define BitReset(p,m) ((p) &= ~(1<<(m)))
#define BitFlip(p,m) ((p) ^= (m))
#define BitWrite(c,p,m) ((c) ? BitSet(p,m) : BitReset(p,m))
#define BitIsSet(reg, bit) (((reg) & (1<<(bit))) != 0)
#define BitIsReset(reg, bit) (((reg) & (1<<(bit))) == 0)


/* Exported constants --------------------------------------------------------*/
enum{	LED_OFF=0, 	LED_ON};
enum{	LED_FLASH_05HZ=0, LED_FLASH_1HZ, 	LED_FLASH_2HZ, LED_LIGHT};
enum{ 	LED_POWER=0, LED_BANK, LED_MUTE, LED_LEFT, LED_RIGHT, LED_UP, LED_DOWN, LED_0, LED_1, LED_2, LED_3};
enum{	BTN_POWER=1, 	BTN_BANK=2, 	BTN_MUTE=4,
		BTN_LEFT=8, 	BTN_RIGHT=16, 	BTN_UP=32, 		BTN_DOWN=64,
		BTN_UL=128, 	BTN_UR=256, 	BTN_DL=512, 	BTN_DR=1024,
		BTN_OK=2048};

enum{BT_DISCONNECTED=0, BT_CONNECTED};

enum{MODE_OFF = 0, MODE_EXT, MODE_USB, MODE_BT};

enum{DISP_NONE=0, DISP_SPECTRE, DISP_SPECTRE_MOVE, DISP_SPECTRE_BLUE, DISP_SPECTRE_GREEN, DISP_RAINBOW};


/* Exported types ------------------------------------------------------------*/
typedef struct{
	uint8_t  onOff 	:1;
	uint8_t  type  	:3;
	uint16_t counter:16;
} LEDTypeDef;


/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void HardwareInit();
void delay_ms(uint32_t nTime);
void SysTickConfig(uint32_t sysClock);

uint16_t ADC_ReadChannel(uint8_t channel);

void PowerEnterStandby();
uint32_t KeyboardHWScan();
void LEDSProcess();
void LEDSet(uint8_t number, uint8_t state, uint8_t mode);

void PowerEnableBank();
void PowerDisableBank();

void PowerEnableMux();
void PowerDisableMux();

void PowerEnableDisplaySpectre();
void PowerDisableDisplaySpectre();

void PowerEnableAMP();
void PowerDisableAMP();

void AMPMuteOn();
void AMPMuteOff();

void PowerEnableRadio();
void PowerDisableRadio();

void PowerDisableCharge();
void PowerEnableCharge();

void PowerEnableBT();
void PowerDisableBT();

uint16_t GetVBat();

void ScanSpectre(uint16_t* data);

uint8_t checkConnectedEarphones();
uint8_t checkVExt();
uint8_t checkCharge();
uint8_t checkVUSB();

void ProcessSensors();
void ManageScreen();

#endif //_HARDWARE_H
