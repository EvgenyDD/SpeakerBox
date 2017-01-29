/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _DISPLAY_H
#define _DISPLAY_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "stm32l1xx_rcc.h"
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_exti.h"
#include "stm32l1xx_pwr.h"
#include "stm32l1xx_tim.h"
#include "stm32l1xx_dma.h"
#include "misc.h"


/* Exported types ------------------------------------------------------------*/
typedef struct{
	uint8_t red;
	uint8_t green;

	uint8_t blue;
}RGBColorType;

extern RGBColorType BLACK, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE;


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
enum{DISPLAY_OVERWRITE=0, DISPLAY_OVERLAY};

extern RGBColorType 	RED,GREEN,BLUE,YELLOW,CYAN,MAGENTA,WHITE;

/* Exported functions ------------------------------------------------------- */
void DisplayInit();
void DisplayDeInit();
void DisplayClear();
void _RGBSet(uint16_t offset, RGBColorType* X, uint8_t mode);
RGBColorType RGBColor(uint8_t r, uint8_t g, uint8_t b);
RGBColorType RGBColorWheel(uint16_t hue);
void DisplayPixel(uint8_t x, uint8_t y, RGBColorType X);
void DisplayUpdate();
void Display_HLine(uint8_t row, const RGBColorType color);
void Display_VLine(uint8_t column, const RGBColorType color);
void Display_AngleDownLine(uint8_t x, uint8_t y, RGBColorType color);
void Display_AngleUpLine(uint8_t x, uint8_t y, RGBColorType color);
void Display_Char(uint8_t x, char s, RGBColorType colorH, RGBColorType colorL);
void Display_String(int x, char *s, uint32_t iterator, RGBColorType colorH, RGBColorType colorL);
void Display_RunningString(char *s, uint8_t speed, RGBColorType colorH, RGBColorType colorL);

void SetLinearHLine(uint8_t x, uint32_t value, uint32_t maxValue, uint8_t numLeds, RGBColorType color);
void SetLinearHLine2Colour(uint8_t x, uint32_t value, uint32_t maxValue, uint8_t numLeds, RGBColorType color, RGBColorType color2);

#endif //_DISPLAY_H
