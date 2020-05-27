/* Includes ------------------------------------------------------------------*/
#include "i2c_soft.h"
#include "hardware.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t fmTxEnbled = 0;


/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
 * Function Name  : _delay_IIC_soft //formula delay=2100/kHz
 *******************************************************************************/
void _delay_IIC_soft(uint32_t delay)
{
	for(uint32_t i = 0; i < delay; i++)
		asm("nop");
}

/*******************************************************************************
 * Function Name  : I2CS_Init
 *******************************************************************************/
void I2CS_Init()
{
	/* I2CS */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2CS_SCL_PORT->BSRRL = (1 << I2CS_SCL_PIN);
	I2CS_SDA_PORT->BSRRL = (1 << I2CS_SDA_PIN);

	delay_ms(1);
}

/*******************************************************************************
 * Function Name  : I2CS_Start
 *******************************************************************************/
void I2CS_Start()
{
	if(BitIsReset(GPIOC->ODR, 4))
	{
		fmTxEnbled = 1;
		//PowerEnableFMTX();
	}
	delay_ms(10);

	I2CS_SDA_PORT->BSRRL = (1 << I2CS_SDA_PIN);
	_delay_IIC_soft(CLOCK_DELAY);
	I2CS_SCL_PORT->BSRRL = (1 << I2CS_SCL_PIN);
	_delay_IIC_soft(CLOCK_DELAY);

	while(!(I2CS_SDA_PORT->IDR & (1 << I2CS_SDA_PIN)))
	{
		I2CS_SCL_PORT->BSRRH = (1 << I2CS_SCL_PIN);
		_delay_IIC_soft(CLOCK_DELAY);
		I2CS_SCL_PORT->BSRRL = (1 << I2CS_SCL_PIN);
		_delay_IIC_soft(CLOCK_DELAY);
	}

	I2CS_SDA_PORT->BSRRH = (1 << I2CS_SDA_PIN);
	_delay_IIC_soft(CLOCK_DELAY);
	I2CS_SCL_PORT->BSRRH = (1 << I2CS_SCL_PIN);
	_delay_IIC_soft(CLOCK_DELAY);
}

/*******************************************************************************
 * Function Name  : I2CS_WriteByte
 * Description    : Write Byte and return ack/nack
 *******************************************************************************/
uint8_t I2CS_WriteByte(uint8_t data)
{
	for(uint8_t i = 0; i < 8; i++)
	{
		if(data & 0x80)
			I2CS_SDA_PORT->BSRRL = (1 << I2CS_SDA_PIN);
		else
			I2CS_SDA_PORT->BSRRH = (1 << I2CS_SDA_PIN);

		_delay_IIC_soft(CLOCK_DELAY);
		I2CS_SCL_PORT->BSRRL = (1 << I2CS_SCL_PIN);
		_delay_IIC_soft(CLOCK_DELAY);
		I2CS_SCL_PORT->BSRRH = (1 << I2CS_SCL_PIN);
		data = data << 1;
	}
	_delay_IIC_soft(CLOCK_DELAY);
	I2CS_SCL_PORT->BSRRL = (1 << I2CS_SCL_PIN);
	I2CS_SDA_PORT->BSRRL = (1 << I2CS_SDA_PIN);
//	_delay_IIC_soft(2*CLOCK_DELAY);
	_delay_IIC_soft(CLOCK_DELAY);
	uint8_t ACK = !(I2CS_SDA_PORT->IDR & (1 << I2CS_SDA_PIN));
	I2CS_SCL_PORT->BSRRH = (1 << I2CS_SCL_PIN);
	I2CS_SDA_PORT->BSRRH = (1 << I2CS_SDA_PIN);

	return ACK;
}

/*******************************************************************************
 * Function Name  : I2CS_WriteByte
 * Description    : Read Byte and ack/nack packet
 *******************************************************************************/
uint8_t I2CS_Read_Byte(uint8_t ACK)
{
	uint8_t data = 0;

	I2CS_SDA_PORT->BSRRL = (1 << I2CS_SDA_PIN);

	for(uint8_t i = 0; i < 8; i++)
	{
		_delay_IIC_soft(CLOCK_DELAY);
		I2CS_SCL_PORT->BSRRL = (1 << I2CS_SCL_PIN);
		_delay_IIC_soft(CLOCK_DELAY);
		data <<= 1;
		if(I2CS_SDA_PORT->IDR & (1 << I2CS_SDA_PIN))
			data++;
		I2CS_SCL_PORT->BSRRH = (1 << I2CS_SCL_PIN);
	}

	if(ACK)
		I2CS_SDA_PORT->BSRRH = (1 << I2CS_SDA_PIN);

	_delay_IIC_soft(CLOCK_DELAY);
	I2CS_SCL_PORT->BSRRL = (1 << I2CS_SCL_PIN);

	_delay_IIC_soft(CLOCK_DELAY);
	I2CS_SCL_PORT->BSRRH = (1 << I2CS_SCL_PIN);
	I2CS_SDA_PORT->BSRRL = (1 << I2CS_SDA_PIN);

	return data;
}

/*******************************************************************************
 * Function Name  : I2CS_Stop
 *******************************************************************************/
void I2CS_Stop()
{
	I2CS_SDA_PORT->BSRRH = (1 << I2CS_SDA_PIN);
	_delay_IIC_soft(CLOCK_DELAY);
	I2CS_SCL_PORT->BSRRL = (1 << I2CS_SCL_PIN);
	_delay_IIC_soft(CLOCK_DELAY);
	I2CS_SDA_PORT->BSRRL = (1 << I2CS_SDA_PIN);
	_delay_IIC_soft(CLOCK_DELAY);

	if(fmTxEnbled)
	{
		fmTxEnbled = 0;
		//PowerDisableFMTX();
	}
}
