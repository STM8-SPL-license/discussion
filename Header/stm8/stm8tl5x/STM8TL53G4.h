/** \addtogroup STM8TL5X
*  @{
*/

/*-------------------------------------------------------------------------
  STM8TL53G4.h - Register Declarations for STM STM8TL53G4

  8-bit, ultra-low-power, touch-sensing MCUs with 16-Kbyte Flash and proximity detection

  Copyright (C) 2019, Georg Icking-Konert

  This library is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.

  As a special exception, if you link this library with other files
  to produce an executable, this library does not by itself cause the
  resulting executable to be covered by the GNU General Public License.
  This exception does not however invalidate any other reasons why the
  executable file might be covered by the GNU General Public License.
-------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
    MODULE DEFINITION FOR MULTIPLE INCLUSION
-----------------------------------------------------------------------------*/
#ifndef STM8TL53G4_H
#define STM8TL53G4_H

/* define device & family */
#if !defined(STM8TL53G4)
  #define STM8TL53G4
#endif
#if !defined(STM8TL5x)
  #define STM8TL5x
#endif

/* device memory sizes [B] */
#define STM8_PFLASH_SIZE 16384
#define STM8_RAM_SIZE    4096
#define STM8_EEPROM_SIZE 2048

/* define device base addresses */
#define     OPT_AddressBase    0x4800
#define   PORTA_AddressBase    0x5000
#define   PORTB_AddressBase    0x5005
#define   PORTD_AddressBase    0x500F
#define   FLASH_AddressBase    0x5050
#define  SYSCFG_AddressBase    0x509E
#define    EXTI_AddressBase    0x50A0
#define     WFE_AddressBase    0x50A6
#define     RST_AddressBase    0x50B0
#define     CLK_AddressBase    0x50C0
#define    WWDG_AddressBase    0x50D3
#define    IWDG_AddressBase    0x50E0
#define     AWU_AddressBase    0x50F0
#define    BEEP_AddressBase    0x50F3
#define     SPI_AddressBase    0x5200
#define     I2C_AddressBase    0x5210
#define   USART_AddressBase    0x5230
#define    TIM2_AddressBase    0x5250
#define    TIM3_AddressBase    0x5280
#define    TIM4_AddressBase    0x52E0
#define     PXS_AddressBase    0x5300
#define     CFG_AddressBase    0x7F60
#define     ITC_AddressBase    0x7F70
#define      DM_AddressBase    0x7F90
#define     UID_AddressBase    0x4925

/* include generic header for series */
#include "STM8TL5x.h"

/*-----------------------------------------------------------------------------
END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _STM8TL53G4_H

/** @}*/

