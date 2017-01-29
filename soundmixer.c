/* Includes ------------------------------------------------------------------*/
#include "soundmixer.h"
#include "hardware.h"
#include "i2c_soft.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PT2314_ADDR		(0x88)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t _SNDMixChnl = CH_DAC;
uint8_t _SNDMixGain = GAIN_0;

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
 * Function Name  : MixerOn
 * Description    : Init soundmixer and turn it on
 *******************************************************************************/
void MixerOn()
{
	I2CS_Init();

	PowerEnableMux();

	delay_ms(200);

//	Si4703_Init();
//		Si4703_SetChannel(984);

	SoundMixerSetVolume(60);

	SoundMixerSwitch(CH_BT);
	SoundMixerSetGain(GAIN_3);


	SoundMixerSetBass(0);
	SoundMixerSetTreble(0);

	SoundMixerSetVolumeL(30);
	SoundMixerSetVolumeR(30);

	delay_ms(100);

	SoundMixerSetVolume(60);
}

/*******************************************************************************
 * Function Name  : MixerSendConfig
 * Description    : Write register of sound mixer
 *******************************************************************************/
uint8_t MixerSendConfig(uint8_t data)
{
	I2CS_Init();

	uint8_t wrStatus = 0, failCounter = 0;

TxPktAgn:
	I2CS_Start();

	if(I2CS_WriteByte(PT2314_ADDR) == 0)
	{
		//delay_ms(1);
		I2CS_Stop();
		goto TxPktAgn;
	}

	wrStatus = I2CS_WriteByte(data);
	I2CS_Stop();

	if(wrStatus == 0)
	{
		if(++failCounter >= 20)
			return STATUS_FAIL;

		goto TxPktAgn;
	}

	return STATUS_OK;
}

/*******************************************************************************
 * Function Name  : SoundMixerSetVolume
 * Description    : Range: 0-63 or VOL_MUTE
 *******************************************************************************/
void SoundMixerSetVolume(uint8_t volume)
{
	if(volume > 63)
		volume = 63;

	MixerSendConfig(SND_VOL | (63 - volume));
}

/*******************************************************************************
 * Function Name  : SoundMixerSetVolumeL
 * Description    : Set left channel volume
 *******************************************************************************/
void SoundMixerSetVolumeL(uint8_t volume)
{
	if(volume >= VOL_MUTE)
		volume = VOL_MUTE;

	MixerSendConfig(SND_SPK_L | (30 - volume));
}

/*******************************************************************************
 * Function Name  : SoundMixerSetVolumeR
 * Description    : Set right channel volume
 *******************************************************************************/
void SoundMixerSetVolumeR(uint8_t volume)
{
	if(volume >= VOL_MUTE)
		volume = VOL_MUTE;

	MixerSendConfig(SND_SPK_R | (30 - volume));
}

/*******************************************************************************
 * Function Name  : SoundMixerSetGain
 * Description    : Set gain of amplifier
 *******************************************************************************/
void SoundMixerSetGain(uint8_t gain)
{
	if(gain != GAIN_0 || gain != GAIN_1 || gain != GAIN_2 || gain != GAIN_3)
		gain = GAIN_0;

	_SNDMixGain = gain;

	MixerSendConfig(SND_SWITCH | _SNDMixChnl | _SNDMixGain);
}

/*******************************************************************************
 * Function Name  : SoundMixerSwitch
 * Description    : Select sound source
 *******************************************************************************/
void SoundMixerSwitch(uint8_t channel)
{
	if(channel > 3)
		channel = CH_DAC;

	_SNDMixChnl = channel;

	MixerSendConfig(SND_SWITCH | _SNDMixChnl | _SNDMixGain);
}

/*******************************************************************************
 * Function Name  : SoundMixerSetBass
 * Description    : Range: -7 to +7
 *******************************************************************************/
void SoundMixerSetBass(int8_t bass)
{
	if(bass < -7)
		bass = -7;
	if(bass > 7)
		bass = 7;

	if(bass >= 0)
		bass += 8;
	else
		bass += 7;

	MixerSendConfig(SND_BASS | bass);
}

/*******************************************************************************
 * Function Name  : SoundMixerSetTreble
 * Description    : Range: -7 to +7
 *******************************************************************************/
void SoundMixerSetTreble(int8_t treble)
{
	if(treble < -7)
		treble = -7;
	if(treble > 7)
		treble = 7;

	if(treble >= 0)
		treble += 8;
	else
		treble += 7;

	MixerSendConfig(SND_TREBLE | treble);
}
