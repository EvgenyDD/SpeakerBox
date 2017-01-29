/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SOUNDMIXER_H
#define _SOUNDMIXER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_i2c.h"
#include "stm32l1xx_rcc.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
enum {STATUS_OK=0, STATUS_FAIL};

/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define SND_VOL		0
#define SND_SPK_L	0xC0
#define SND_SPK_R	0xE0
#define SND_SWITCH	0x40
#define SND_BASS	0x60
#define SND_TREBLE	0x70

#define VOL_MUTE	31

enum{GAIN_0=0x18, 	GAIN_1=0x10, 	GAIN_2=0x08, 	GAIN_3=0};
enum{CH_BT=2, 		CH_DAC=0, 		CH_FM=1, 		CH_EXT=3};


/* Exported functions ------------------------------------------------------- */


void MixerOn();
uint8_t MixerSendConfig(uint8_t data);
void SoundMixerSetVolume(uint8_t volume);
void SoundMixerSetVolumeL(uint8_t volume);
void SoundMixerSetVolumeR(uint8_t volume);
void SoundMixerSetGain(uint8_t gain);
void SoundMixerSwitch(uint8_t channel);
void SoundMixerSetBass(int8_t bass);
void SoundMixerSetTreble(int8_t treble);

#endif //_SOUNDMIXER_H
