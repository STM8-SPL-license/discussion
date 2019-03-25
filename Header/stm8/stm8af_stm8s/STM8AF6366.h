/** \addtogroup STM8AF_STM8S
*  @{
*/

/*-------------------------------------------------------------------------
  STM8AF6366.h - Register Declarations for STM STM8AF6366

  Automotive 8-bit MCU with 32 Kbytes Flash, LIN, 16 MHz CPU, integrated EEPROM

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
#ifndef STM8AF6366_H
#define STM8AF6366_H

/* define device & family */
#if !defined(STM8AF6366)
  #define STM8AF6366
#endif
#if !defined(STM8AF636x)
  #define STM8AF636x
#endif

/* device memory sizes [B] */
#define STM8_PFLASH_SIZE 32768
#define STM8_RAM_SIZE    2048
#define STM8_EEPROM_SIZE 1024

/* define device base addresses */
#define     OPT_AddressBase    0x4800
#define   PORTA_AddressBase    0x5000
#define   PORTB_AddressBase    0x5005
#define   PORTC_AddressBase    0x500A
#define   PORTD_AddressBase    0x500F
#define   PORTE_AddressBase    0x5014
#define   PORTF_AddressBase    0x5019
#define   FLASH_AddressBase    0x505A
#define    EXTI_AddressBase    0x50A0
#define     RST_AddressBase    0x50B3
#define     CLK_AddressBase    0x50C0
#define    WWDG_AddressBase    0x50D1
#define    IWDG_AddressBase    0x50E0
#define     AWU_AddressBase    0x50F0
#define    BEEP_AddressBase    0x50F3
#define     SPI_AddressBase    0x5200
#define     I2C_AddressBase    0x5210
#define   UART2_AddressBase    0x5240
#define    TIM1_AddressBase    0x5250
#define    TIM2_AddressBase    0x5300
#define    TIM3_AddressBase    0x5320
#define    TIM4_AddressBase    0x5340
#define    ADC1_AddressBase    0x53E0
#define     CFG_AddressBase    0x7F60
#define     ITC_AddressBase    0x7F70
#define      DM_AddressBase    0x7F90

/* include generic header for series */
#include "STM8AF_STM8S.h"

/*-----------------------------------------------------------------------------
END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _STM8AF6366_H

/** @}*/

