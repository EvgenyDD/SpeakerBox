/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DEBUG_H
#define DEBUG_H


/* Includes ------------------------------------------------------------------*/
#include <stm32l1xx.h>
#include <stm32l1xx_usart.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_rcc.h>


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void DebugInit();
void DebugSendString(char*);
void DebugSendChar(char);
void DebugSysTimeEnd();
void DebugSysTimeStart();
void DebugSendNum(int32_t Num);
void DebugSendNumWDesc(char *string, int16_t Num);

#endif //DEBUG_H
