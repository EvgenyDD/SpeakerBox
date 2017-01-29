/* Includes ------------------------------------------------------------------*/
#include "radio.h"
#include "soundmixer.h"
#include "hardware.h"
#include "i2c_soft.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint16_t Si4703_REGs[16];

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
 * Function Name  : Si4703_Read
 * Description    : Read all registers from Si4703
 *******************************************************************************/
uint8_t Si4703_Read()
{
	uint8_t i, buffer[32]; // 16 of 16-bit registers

SndPktAgn:
	I2CS_Start();
	if(I2CS_WriteByte(Si4703_ADDR | 1) == 0)
	{
		delay_ms(1);
		I2CS_Stop();
		goto SndPktAgn;
	}

	// Si4703 read start from r0Ah register
	for(i = 0x14;; i++)
	{
		if(i == 0x20)
			i = 0x00;

		buffer[i] = I2CS_Read_Byte(1);
		if(i == 0x12)
			break;
	}

	buffer[i++] = I2CS_Read_Byte(0);	//last byte NACKed

	I2CS_Stop();

	for(uint8_t i = 0; i < 16; i++)
		Si4703_REGs[i] = (buffer[i << 1] << 8) | buffer[(i << 1) + 1];
}


/*******************************************************************************
 * Function Name  : Si4703_Write
 * Description    : Write all registers into Si4703
 *******************************************************************************/
uint8_t Si4703_Write()
{
	uint8_t error = 0;
SndPktAgn:
	I2CS_Start();

	if(I2CS_WriteByte(Si4703_ADDR) == 0)
	{
		delay_ms(1);
		I2CS_Stop();
		goto SndPktAgn;
	}

	for(uint8_t i = 2; i < 8; i++)
	{
		if(I2CS_WriteByte(Si4703_REGs[i] >> 8) == 0)
			error = 1;
		if(I2CS_WriteByte(Si4703_REGs[i] & 0x00ff) == 0)
			error = 1;;
	}

	I2CS_Stop();
	return error;
}


/*******************************************************************************
 * Function Name  : Si4703_Init
 * Description    : Powerup initialization of Si4703
 *******************************************************************************/
void Si4703_Init()
{
	PowerEnableRadio();
	// Si4703 powerup configuration sequence
	Si4703_Read();
	Si4703_REGs[Si4703_TEST1] = (1 << Si4703_T1_XOSCEN) | (1 << Si4703_T1_WTF) | (1<<14); // power up crystal
	Si4703_Write();
	delay_ms(500); // wait for crystal to power up (by AN230 v0.61)

	// Configure tuner beginning conditions
	Si4703_Read();
	Si4703_REGs[Si4703_POWERCFG] = (1 << Si4703_PWR_DMUTE) | (1 << Si4703_PWR_ENABLE);
	Si4703_REGs[Si4703_SYSCONFIG1] |= (1 << Si4703_SC1_RDS); // Enable RDS
	Si4703_REGs[Si4703_SYSCONFIG1] |= (1 << Si4703_SC1_DE); // 50us de-emphasis (must be on for Europe, Australia and Japan)
	Si4703_REGs[Si4703_SYSCONFIG2] &= ~(1 << Si4703_SC2_BAND1) | (1 << Si4703_SC2_BAND0); // 87.5-108MHz (USA,Europe)
	//Si4703_REGs[Si4703_SYSCONFIG2] |= (1<<Si4703_SC2_BAND1)|(1<<Si4703_SC2_BAND0); // 76-108MHz (Japan wide band)
	Si4703_REGs[Si4703_SYSCONFIG2] |= (1 << Si4703_SC2_SPACE0); // 100kHz spacing (Europe)
	Si4703_REGs[Si4703_SYSCONFIG2] &= 0xfff0;
	//Si4703_REGs[Si4703_SYSCONFIG2] |= 0x0001; // minimum volume
	//Si4703_REGs[Si4703_SYSCONFIG2] |= 0x0007; // medium volume
	Si4703_REGs[Si4703_SYSCONFIG2] |= 0x000f; // maximum volume
	//Si4703_REGs[Si4703_SYSCONFIG3] |= (1<<Si4703_SC3_VOLEXT); // Decrease the volume by 28dB
	Si4703_Write();
	delay_ms(150); // wait for device powerup (110ms from datasheet?)

	DebugSendNumWDesc("DevID=", Si4703_REGs[0]);
	DebugSendNumWDesc(" ChipID=", Si4703_REGs[1]);
}

// Tune for channel
// Input:
//   Channel - channel frequency (MHz * 10)
//   87.5MHz  = 875
//   107.9MHz = 1079
/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
void Si4703_SetChannel(int32_t Channel)
{
	Si4703_Read();

	Channel *= 10;
	Channel -=
		((Si4703_REGs[Si4703_SYSCONFIG2] & ((1 << Si4703_SC2_BAND1) | (1 << Si4703_SC2_BAND0))) == 0) ? 8750 : 7600;
	Channel /= 10;

	Si4703_REGs[Si4703_CHANNEL] &= 0xfe00; // Clear channel frequency from register
	Si4703_REGs[Si4703_CHANNEL] |= Channel; // Set new channel frequency
	Si4703_REGs[Si4703_CHANNEL] |= (1 << Si4703_CH_TUNE); // Set TUNE flag
	Si4703_Write();

	delay_ms(50); // Gime some time for the Si4703 to tune up

	Si4703_Read();
	DebugSendNumWDesc("DevID=", Si4703_REGs[0]);
	DebugSendNumWDesc(" ChipID=", Si4703_REGs[1]);
	DebugSendNumWDesc(" reg 02h=", Si4703_REGs[2]);
	DebugSendNumWDesc(" 07h=", Si4703_REGs[7]);
	DebugSendNumWDesc(" rdsd=", Si4703_REGs[0x0f]);
	// Wait for the Si4703 to set STC flag
	DebugSendNumWDesc("\nTuning: ", Channel + 875);

	while(1)
	{
		Si4703_Read();
		if((Si4703_REGs[Si4703_RSSI] & (1 << Si4703_RSSI_STC)) != 0)
			break; // Tuning complete
	}
	DebugSendString(" Ok");

	Si4703_Read();
	Si4703_REGs[Si4703_CHANNEL] &= ~(1 << Si4703_CH_TUNE); // Clear TUNE flag
	Si4703_Write();
	delay_ms(1); // <---------------------- Is this necessary?

	// Wait for the Si4703 to clear STC flag
	while(1)
	{
		Si4703_Read();
		if((Si4703_REGs[Si4703_RSSI] & (1 << Si4703_RSSI_STC)) == 0)
			break; // Tuning complete
		DebugSendString("Waiting STC clear...");
	}
}

// Get current tuned channel
// Return:
//   875  = 87.5MHz
//   1079 = 107.9MHz
/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
uint32_t Si4703_GetChannel()
{
	Si4703_Read();
	uint32_t Channel = Si4703_REGs[Si4703_READCHANNEL] & 0x03ff;
	Channel +=
		((Si4703_REGs[Si4703_SYSCONFIG2] & ((1 << Si4703_SC2_BAND1) | (1 << Si4703_SC2_BAND0))) == 0) ? 875 : 760;

	return Channel;
}

// Seek for next station
// Input:
//   SeekDirection: SEEK_UP/SEEK_DOWN
//   Wrap: WRAP_ON/WRAP_OFF
// Return:
//   0 - Success
//   1 - Fail
/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
uint32_t Si4703_Seek(uint8_t SeekDirection, uint8_t Wrap)
{
	uint32_t freq;
	uint32_t _sfbl;

	Si4703_Read();

	BitWrite(Wrap, Si4703_REGs[Si4703_POWERCFG], Si4703_PWR_SKMODE);
//	if(Wrap)
//		Si4703_REGs[Si4703_POWERCFG] |= (1 << Si4703_PWR_SKMODE); // Band wrap on
//	else
//		Si4703_REGs[Si4703_POWERCFG] &= ~(1 << Si4703_PWR_SKMODE); // Band wrap off

	BitWrite(!SeekDirection, Si4703_REGs[Si4703_POWERCFG], Si4703_PWR_SEEKUP);
//	if(SeekDirection)
//		Si4703_REGs[Si4703_POWERCFG] &= ~(1 << Si4703_PWR_SEEKUP); // Seek up
//	else
//		Si4703_REGs[Si4703_POWERCFG] |= (1 << Si4703_PWR_SEEKUP); // Seek down

	Si4703_REGs[Si4703_POWERCFG] |= (1 << Si4703_PWR_SEEK); // Set seek start bit

	Si4703_Write(); // Start seek

	// Wait for the Si4703 to set STC flag
	DebugSendString("Seek...");
	while(1)
	{
		Si4703_Read();
		if((Si4703_REGs[Si4703_RSSI] & (1 << Si4703_RSSI_STC)) != 0)
			break; // Seek complete
		freq = Si4703_GetChannel();

		DebugSendNumWDesc("  --> ", freq);
		Delay_ms(50); // <-- Fancy delay, in real this unnecessary
	}

	Si4703_Read();

	_sfbl = Si4703_REGs[Si4703_RSSI] & (1 << Si4703_RSSI_SFBL); // Store value of SFBL bit
	Si4703_REGs[Si4703_POWERCFG] &= ~(1 << Si4703_PWR_SEEK); // Reset seek bit (it must be done after seek)
	Si4703_Write();
	Delay_ms(1); // <---------------------- Is this necessary?

	// Wait for the Si4703 to clear STC flag
	while(1)
	{
		Si4703_Read();
		if((Si4703_REGs[Si4703_RSSI] & (1 << Si4703_RSSI_STC)) == 0)
			break; // Tuning complete

		DebugSendString("Waiting STC clear...");
	}

	if(_sfbl)
	{
		DebugSendString("Seek limit hit.");
		return 1;
	}

	DebugSendString("Seek completed.");
	return 0;
}

//int main(void) {
//    Si4703_Read();
//
//	UART_SendStr("DevID="); UART_SendHex16(Si4703_REGs[0]);
//	UART_SendStr(" ChipID="); UART_SendHex16(Si4703_REGs[1]);
//
////    Si4703_SetChannel(893); // Radio Rocks
////    Si4703_SetChannel(904); // Retro FM
////    Si4703_SetChannel(1011); // Europa plus
////    Si4703_SetChannel(1045); // Nashe radio (RDS)
////    Si4703_SetChannel(1079); // Radio Melodia
//
//	Si4703_SetChannel(1065);
//
//	Si4703_Read();
//	uint32_t freq = Si4703_GetChannel();
//    UART_SendStr("Freq: "); UART_SendInt(freq / 10); UART_SendChar('.');
//    UART_SendInt(freq % 10); UART_SendStr("MHz\n");
//
//    Si4703_Seek(Si4703_SEEK_DOWN,Si4703_WRAP_ON);
//
//    Si4703_Read();
//	UART_SendStr((Si4703_REGs[Si4703_RSSI] & (1<<Si4703_RSSI_ST)) == 0 ? "MONO\n" : "STEREO\n");
//	UART_SendStr("Signal: "); UART_SendInt(Si4703_REGs[Si4703_RSSI] & 0x007f); UART_SendStr("dBuV\n");
//	freq = Si4703_GetChannel();
//    UART_SendStr("Freq: "); UART_SendInt(freq / 10); UART_SendChar('.');
//    UART_SendInt(freq % 10); UART_SendStr("MHz\n");
//
//	while(1);
//}



void SetFMTXFreq(uint16_t freq)
{
	PowerEnableFMTX();

	if(freq<700)
		freq = 700;
	if(freq>1200)
		freq = 1200;

	uint16_t data = freq;
	BitSet(data, 11); //stereo if bit set

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	I2CS_SCL_PORT->BSRRH = (1 << I2CS_SCL_PIN);


	GPIO_SetBits(GPIOB, GPIO_Pin_0); //CE = 1

	//Writing Cycle
	for(uint8_t i = 0; i < 16; i++)
	{
		if(data & 0x0001)
			I2CS_SDA_PORT->BSRRL = (1 << I2CS_SDA_PIN);
		else
			I2CS_SDA_PORT->BSRRH = (1 << I2CS_SDA_PIN);

		_delay_IIC_soft(CLOCK_DELAY);
		I2CS_SCL_PORT->BSRRL = (1 << I2CS_SCL_PIN);
		_delay_IIC_soft(CLOCK_DELAY);
		I2CS_SCL_PORT->BSRRH = (1 << I2CS_SCL_PIN);
		data = data >> 1;
	}

	GPIO_ResetBits(GPIOB, GPIO_Pin_0); //CE = 0
	I2CS_Init();
}
