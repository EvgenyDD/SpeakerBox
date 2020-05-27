/**
  ******************************************************************************
  * @file    usb_conf.h
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Virtual COM Port Demo configuration  header
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


// Define to prevent recursive inclusion
#ifndef __USB_CONF_H
#define __USB_CONF_H


// defines how many endpoints are used by the device
#define EP_NUM              (2)

// --------------   Buffer Description Table  -----------------
// buffer table base address
#define BTABLE_ADDRESS      (0x0000)

// EP0 
// rx/tx buffer base address
#define ENDP0_RXADDR        (0x0010)
#define ENDP0_TXADDR        (0x0050)

// EP1
// buffer base address
#define ENDP1_BUF0Addr      (0x0090)
#define ENDP1_BUF1Addr      (0x00C0)


// -------------------   ISTR events  -------------------------
// IMR_MSK - mask defining which events has to be handled
// by the device application software
//### $$$ THIS IS THE FUCKING REASON WHY ALL Stucks
//https://my.st.com/public/STe2ecommunities/mcu/Lists/cortex_mx_stm32/DispForm.aspx?ID=30622&RootFolder=%2fpublic%2fSTe2ecommunities%2fmcu%2fLists%2fcortex%5fmx%5fstm32%2fUSB%20CDC%20on%20STM32L%20device%20%20unable%20to%20connect%20to%20host
#define IMR_MSK             (CNTR_CTRM | CNTR_WKUPM | /*CNTR_SUSPM |*/ CNTR_ERRM | CNTR_SOFM | /*CNTR_ESOFM |*/ CNTR_RESETM )

//#define CTR_CALLBACK
//#define DOVR_CALLBACK
//#define ERR_CALLBACK
//#define WKUP_CALLBACK
//#define SUSP_CALLBACK
//#define RESET_CALLBACK
#define SOF_CALLBACK
//#define ESOF_CALLBACK

// CTR service routines associated to defined endpoints
#define  EP1_IN_Callback    NOP_Process
#define  EP2_IN_Callback    NOP_Process
#define  EP3_IN_Callback    NOP_Process
#define  EP4_IN_Callback    NOP_Process
#define  EP5_IN_Callback    NOP_Process
#define  EP6_IN_Callback    NOP_Process
#define  EP7_IN_Callback    NOP_Process

//#define  EP1_OUT_Callback   NOP_Process
#define  EP2_OUT_Callback   NOP_Process
#define  EP3_OUT_Callback   NOP_Process
#define  EP4_OUT_Callback   NOP_Process
#define  EP5_OUT_Callback   NOP_Process
#define  EP6_OUT_Callback   NOP_Process
#define  EP7_OUT_Callback   NOP_Process

#endif // __USB_CONF_H

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/