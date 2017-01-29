/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef I2C_SOFT_H
#define I2C_SOFT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_rcc.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define I2CS_PORT 		GPIOB
#define I2CS_SDA_PORT  	I2CS_PORT
#define I2CS_SCL_PORT  	I2CS_PORT
#define I2CS_SCL_PIN 	10
#define I2CS_SDA_PIN 	11

#define CLOCK_DELAY 	(21)	//formula X=2100/kHz

/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void I2CS_Init();
void I2CS_Start();
uint8_t I2CS_WriteByte(uint8_t data);
uint8_t I2CS_Read_Byte(uint8_t ACK);
void I2CS_Stop();

void _delay_IIC_soft(uint32_t delay);

#endif //I2C_SOFT_H
