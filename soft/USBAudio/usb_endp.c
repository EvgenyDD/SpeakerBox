#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "stm32l1xx_dma.h"

volatile uint32_t stream_buffer[50]; //48 for 48000Hz // USBD_AUDIO_FREQ*2*2/1000

#pragma GCC push_options
#pragma GCC optimize ("O3")
/*******************************************************************************
    Function Name  : EP1_OUT_Callback (from host to device)
    Description    : Endpoint 1 out callback routine.
*******************************************************************************/
void EP1_OUT_Callback(void)
{
	uint16_t dlen;  // received data length

	uint32_t *pdwVal;

	if(_GetENDPOINT(ENDP1) & EP_DTOG_TX)
	{
		// read from ENDP1_BUF0Addr buffer
		dlen = (*_pEPTxCount(ENDP1)) & 0x3ff;
		pdwVal = (uint32_t *) (ENDP1_BUF0Addr * 2 + PMAAddr);
	}
	else
	{
		// read from ENDP1_BUF1Addr buffer
		dlen = (*_pEPRxCount(ENDP1)) & 0x3ff;
		pdwVal = (uint32_t *) (ENDP1_BUF1Addr * 2 + PMAAddr);
	}

	uint32_t n = (dlen + 1) >> 2;

	uint32_t k = 0;
	uint16_t num1, num2;

	for(uint32_t i = n; i != 0; i--)
	{
		num1 = (*pdwVal++) ^ 0x8000;
		num2 = (*pdwVal++) ^ 0x8000;
		stream_buffer[k++] = num2 | (num1 << 16);
	}

	DMA1_Channel2->CCR &= (uint16_t)(~DMA_CCR1_EN);  //DMA_Cmd(DMA1_Channel5, DISABLE); 			// disable DMA1 channel 5
	DMA1->IFCR = DMA1_FLAG_TC2;

	DMA1_Channel2->CNDTR = n;
	DMA1_Channel2->CCR |= DMA_CCR1_EN;				// enable DMA1 channel 5

	FreeUserBuffer(ENDP1, EP_DBUF_OUT);
}
#pragma GCC reset_options


/*******************************************************************************
* Function Name  : USB_Istr
* Description    : Start of frame callback function.
*******************************************************************************/
void SOF_Callback(void)
{
}
