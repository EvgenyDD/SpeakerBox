/* Includes ------------------------------------------------------------------*/
#include "display.h"
#include "macros.h"
#include "stdbool.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define TIM_COMPARE_HIGH	7
#define TIM_COMPARE_LOW		3
#define TIM_PERIOD			(TIM_COMPARE_HIGH+TIM_COMPARE_LOW)
#define TIM_FREQ			6000

#define d2r (3.14159265/180)

RGBColorType 	BLACK 	= {000,	000, 000},
				RED 	= {255,	000, 000},
				GREEN 	= {000,	255, 000},
				BLUE 	= {000,	000, 255},
				YELLOW 	= {255,	255, 000},
				CYAN 	= {000,	255, 255},
				MAGENTA	= {255,	000, 255},
				WHITE 	= {255,	255, 255};

//gamma correction table
const uint8_t gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

static uint8_t decimate = 1;

static uint8_t _transparent = 1;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint16_t DisplayBuffer[10 + 50 * 24 + 10];

static uint8_t displayStringBuffer[100];
static bool displayRunningString = false;
static uint8_t displaySpeed = 0;
uint8_t displaySpeedCounter = 0;

static struct _current_font
{
	uint8_t* font;
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
	uint8_t inverse;
} cfont;

/* Extern variables ----------------------------------------------------------*/
extern const uint8_t font3x5[];

/* Private function prototypes -----------------------------------------------*/
static void _display_setFont(uint8_t* font);


/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
void DisplayInit()
{
	uint16_t PrescalerValue;

	_display_setFont((uint8_t*)font3x5);

	GPIO_InitTypeDef GPIOInitStructure;
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIOInitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM4);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	/* Compute the prescaler value */
	PrescalerValue = (uint16_t)((32000000 / 1000/*Hz->kHz*/) / TIM_FREQ/*khz*/) - 1;
	/* Time base configuration */

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = TIM_PERIOD; // 800kHz
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);

	/***
	 * Must enable reload for PWM (STMicroelectronicd RM0090 section 18.3.9
	 * "PWM mode":
	 *
	 *   You must enable the corresponding preload register by setting the
	 *   OCxPE bit in the TIMx_CCMRx register.
	 *
	 * This is part of the fix for the pulse corruption (the runt pulse at
	 * the start and the extra pulse at the end).
	 */
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

	/* configure DMA */
	/* DMA clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* DMA1 Channel6 Config */
	DMA_DeInit(DMA1_Channel5);

	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_BufferSize = sizeof(DisplayBuffer) / sizeof(DisplayBuffer[0]);
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &DisplayBuffer;		// this is the buffer memory
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					// automatically increase buffer index
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;						// stop DMA feed after buffer size is reached
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &TIM4->CCR3;	// physical address of Timer 4 CCR3
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;

	DMA_Init(DMA1_Channel5, &DMA_InitStructure);

	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	//Enable DMA1 channel IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* PWM_TIMER CC1 DMA Request enable */
	TIM_DMACmd(TIM4, TIM_DMA_CC3, ENABLE);
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
void DisplayDeInit()
{
}


void DMA1_Channel5_IRQHandler()
{
	if(DMA_GetITStatus(DMA1_IT_TC5))
	{
		TIM4->CCR3 = 0;
		TIM4->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));	//TIM_Cmd(TIM4, DISABLE); 					// disable Timer 4
		DMA1_Channel5->CCR &= (uint16_t)(~DMA_CCR1_EN);  //DMA_Cmd(DMA1_Channel5, DISABLE); 			// disable DMA1 channel 5
		DMA1->IFCR = DMA1_FLAG_TC5;	//DMA_ClearFlag(DMA1_FLAG_TC5); 				// clear DMA1 Channel 5 transfer complete fla

		//Clear DMA1 Channel1 Half Transfer, Transfer Complete and Global interrupt pending bits
		DMA_ClearITPendingBit(DMA1_IT_TC5 );
	}
}

/*******************************************************************************
* Function Name  :
* Description    :
*******************************************************************************/
void DisplayClear()
{
	for(uint32_t i = 0; i < sizeof(DisplayBuffer) / sizeof(DisplayBuffer[0]); i++)
		DisplayBuffer[i] = TIM_COMPARE_LOW;

	for(int i=0; i<10; i++)
		DisplayBuffer[i] = 0;

	for(uint32_t i = (10 + 50 * 24 + 10), j=0; j<10;j++,i--)
		DisplayBuffer[i] = 0;
}

/*******************************************************************************
* Function Name  :
* Description    :
*******************************************************************************/
void _RGBSet(uint16_t offset, RGBColorType* X, uint8_t mode)
{
//	if(X->blue) X->blue = 1;
//	if(X->red) X->red = 10;
//	if(X->green) X->green = 1;
	X->blue = gamma8[X->blue];
	X->red = gamma8[X->red];
	X->green = gamma8[X->green];

	if(mode == DISPLAY_OVERWRITE)
	{
		for(uint8_t i = 0; i < 8; i++)
		{
			DisplayBuffer[offset + i + 0] = BitIsSet(X->green, i) ? TIM_COMPARE_HIGH : TIM_COMPARE_LOW;
			DisplayBuffer[offset + i + 8] = BitIsSet(X->red, i) ? TIM_COMPARE_HIGH : TIM_COMPARE_LOW;
			DisplayBuffer[offset + i + 16] = BitIsSet(X->blue, i) ? TIM_COMPARE_HIGH : TIM_COMPARE_LOW;
		}
	}
	else //DISPLAY_OVERLAY
	{
		for(uint8_t i = 0; i < 8; i++)
		{
			if(BitIsSet(X->green, 7-i))
				DisplayBuffer[offset + i + 0] |= TIM_COMPARE_HIGH;
			if(BitIsSet(X->red, 7-i))
				DisplayBuffer[offset + i + 8] |= TIM_COMPARE_HIGH;
			if(BitIsSet(X->blue, 7-i))
				DisplayBuffer[offset + i + 16] |= TIM_COMPARE_HIGH;
		}
	}
}

RGBColorType RGBColor(uint8_t r, uint8_t g, uint8_t b)
{
	RGBColorType color = {r,g,b};
	return color;
}


RGBColorType colorDecimate(RGBColorType color, uint8_t decimate)
{
	if(decimate > 100)
		decimate = 100;
	decimate = 101-decimate;
	color.red  = color.red * decimate / 100;
	color.green  = color.green * decimate / 100;
	color.blue  = color.blue * decimate / 100;
	return color;
}


/*******************************************************************************
* Function Name  : HSVtoRGB
* Description    : Convert HUE -> RGB colors
*******************************************************************************/
RGBColorType RGBColorWheel(uint16_t hue)
{
	RGBColorType color;

	uint16_t ii;
	double fr, hue2;
	uint8_t c2, c3;

	/*while (hue >= 360)
		hue -= 360;*/
	hue = hue%360;

	hue2 = hue;
	ii = (uint16_t)(hue2 /= 60.0);
	fr = hue2 - ii;
	c2 = (uint8_t)(255 - 255 * fr);
	c3 = (uint8_t)(255 * fr);

	switch (ii) {
	        case 0: color.red = 255; color.green = c3;  color.blue = 0;   break;
	        case 1: color.red = c2;  color.green = 255; color.blue = 0;   break;
	        case 2: color.red = 0;   color.green = 255; color.blue = c3;  break;
	        case 3: color.red = 0;   color.green = c2;  color.blue = 255; break;
	        case 4: color.red = c3;  color.green = 0;   color.blue = 255; break;
	        case 5: color.red = 255; color.green = 0;   color.blue = c2;  break;
	}

	   return color;
}

/*******************************************************************************
* Function Name  :
* Description    :
*******************************************************************************/
void DisplayPixel(uint8_t x, uint8_t y, RGBColorType X)
{
	if(x > 9 || y > 4)
		return;

	const uint8_t LEDLookup[5][10] =
	{
		{24,23,22,21,20,19,18,17,16,15},
		{5, 6, 7, 8, 9, 10,11,12,13,14},
		{4, 3, 2, 1, 0, 25,26,27,28,29},
		{39,38,37,36,35,34,33,32,31,30},
		{40,41,42,43,44,45,46,47,48,49}
	};

	_RGBSet(LEDLookup[y][x] * 24 + 10, &X, DISPLAY_OVERLAY);
}

/*******************************************************************************
* Function Name  :
* Description    :
*******************************************************************************/
void DisplayUpdate()
{
	DMA1_Channel5->CNDTR = sizeof(DisplayBuffer) / sizeof(DisplayBuffer[0]);	//DMA_SetCurrDataCounter(DMA1_Channel5, 48); 	// load number of bytes to be transferred

	// PAP: Clear the timer's counter and set the compare value to 0. This
	// sets the output low on start and gives us a full cycle to set up DMA.
	TIM4->CNT = 0;
	TIM4->CCR3 = 0;
	TIM4->CR1 |= TIM_CR1_CEN;	// enable Timer 4

	// PAP: Start DMA transfer after starting the timer. This prevents the
	// DMA/PWM from dropping the first bit.
	DMA1_Channel5->CCR |= DMA_CCR1_EN;				// enable DMA1 channel 5
}

/*******************************************************************************
* Function Name  :
* Description    :
*******************************************************************************/
void XXX()
{
	DisplayPixel(0,0, RGBColor(255,0,0));
	DisplayPixel(5,2, RGBColor(0,100,0));
	DisplayPixel(7,3, RGBColor(0,0,100));

	DisplayUpdate();
}


void YYY()
{
	static uint8_t tmpPtr = 0;
	static uint16_t hue = 0;

	if(++tmpPtr > 13)
	{
		tmpPtr = 0;
	}

	if(hue >= 360)
		hue = 0;

	RGBColorType color = RGBColorWheel(hue);

#define WIDTH  2

	DisplayClear();
	for(int i=0; i<WIDTH; i++)
	{
		int ptr = tmpPtr - i;
		Display_AngleDownLine((ptr>9?9:ptr), (ptr>9?ptr-9:0), color);
	}
	hue += 2;
}



void rainbow()
{
	static int16_t suchka = 359;

	suchka-=5;
	if(suchka <= 0)
		suchka = 359;

	DisplayClear();
	for(int i=0; i<14; i++)
	{
		for(int j=0; j<5; j++)
			Display_AngleDownLine((i>9?9:i), (i>9?i-9:0), RGBColorWheel(i*360/14+suchka));
	}
}

void Display_HLine(uint8_t row, const RGBColorType color)
{
	for(uint8_t i=0; i<10; i++)
		DisplayPixel(i, row, color);
}

void Display_VLine(uint8_t column, const RGBColorType color)
{
	for(uint8_t i=0; i<10; i++)
		DisplayPixel(column, i, color);
}

//45 degrees line
void Display_AngleDownLine(uint8_t x, uint8_t y, RGBColorType color)
{
	if(x>9 || y>4) return;
	for(int8_t i=x, j=y; (i>-1 || j>-1) && (i < 10 || j<5) ; i++, j--)
		DisplayPixel(i, j, color);
	for(int8_t i=x, j=y; (i>-1 || j>-1) && (i < 10 || j<5) ; i--, j++)
			DisplayPixel(i, j, color);
}

//-45 degrees line
void Display_AngleUpLine(uint8_t x, uint8_t y, RGBColorType color)
{
	if(x>9 || y>4) return;
	for(int8_t i=x, j=y; (i>-1 || j>-1) && (i < 10 || j<5) ; i--, j--)
		DisplayPixel(i, j, color);
	for(int8_t i=x, j=y; (i>-1 || j>-1) && (i < 10 || j<5) ; i++, j++)
			DisplayPixel(i, j, color);
}


//////////////////// TEXT //////////////////////////
static void _display_setFont(uint8_t* font)
{
	cfont.font = font;
	cfont.x_size = cfont.font[0];
	cfont.y_size = cfont.font[1];
	cfont.offset = cfont.font[2];
	cfont.numchars = cfont.font[3];
	cfont.inverse = cfont.font[4];
}

void Display_Char(uint8_t x, char s, RGBColorType colorH, RGBColorType colorL)
{

#define FONT_WIDTH cfont.x_size
#define FONT_HEIGHT cfont.y_size

	if(x >= 10)
		return;

uint32_t temp = ((s - cfont.offset) * FONT_WIDTH * (FONT_HEIGHT < 8 ? 8 : FONT_HEIGHT) / 8) + 5;


	for(uint16_t j = 0; j < FONT_WIDTH * (FONT_HEIGHT / 8 + FONT_HEIGHT % 8 ? 1 : 0); j++)
	{
		char frame = cfont.font[temp];

		for(uint8_t i = 0; i < FONT_HEIGHT; i++)
		{
			if(BitIsSet(frame, cfont.inverse & 0x01 ? (7 - i) : (i)))
				DisplayPixel(x + j, i, colorH);
			else
			{
				if(!_transparent)
					DisplayPixel(x + j, i, colorL);
			}
		}
		if(j % 1 == 0)
			temp++;
	}
}

/*******************************************************************************
* Function Name  :
* Description    :
*******************************************************************************/
void Display_String(int x, char *s, uint32_t iterator, RGBColorType colorH, RGBColorType colorL)
{
	uint32_t len = strlen(s);
	uint8_t i=0;

	x += iterator;
	if(x > 10)
		return;

	while(s[i] != '\0' && x < 10)
	{
		Display_Char(x, s[i], colorH, colorL);
		i++;
		x += cfont.x_size + 1;
	}
}


void Display_RunningString(char *s, uint8_t speed, RGBColorType colorH, RGBColorType colorL)
{
	displayRunningString = true;
	displaySpeed = speed;
	strcpy(displayStringBuffer, s);
}


void SetLinearHLine(uint8_t x, uint32_t value, uint32_t maxValue, uint8_t numLeds, RGBColorType color)
{
	uint8_t numLedsOn = value * numLeds / maxValue;

	for(uint8_t i=0; i<numLedsOn; i++)
		DisplayPixel(x, i, color);

	DisplayPixel(x, numLedsOn, colorDecimate(color, (maxValue - value) * numLeds * 100 / maxValue));
}

void SetLinearHLine2Colour(uint8_t x, uint32_t value, uint32_t maxValue, uint8_t numLeds, RGBColorType color, RGBColorType color2)
{
	uint8_t numLedsOn = value * numLeds / maxValue;

	for(uint8_t i=0; i<numLedsOn; i++)
		DisplayPixel(x, i, color);

	DisplayPixel(x, numLedsOn, colorDecimate(color2, (maxValue - value) * numLeds * 100 / maxValue));
}
