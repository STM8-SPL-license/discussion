/*-------------------------------------------------------------------------
  STM8S10x_all-peripherals.h - Register Declarations for STM8S10x Umbrella Device

  Virtual STM8S10x device, containing all peripherals to check header

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
#ifndef STM8S10X_ALL_H
#define STM8S10X_ALL_H

/* device memory sizes [B] */
#define STM8_PFLASH_SIZE 8192
#define STM8_RAM_SIZE    1536
#define STM8_EEPROM_SIZE 0

/* define device base addresses */
#define     OPT_AddressBase    0x4800
#define   PORTA_AddressBase    0x5000
#define   PORTB_AddressBase    0x5005
#define   PORTC_AddressBase    0x500A
#define   PORTD_AddressBase    0x500F
#define   FLASH_AddressBase    0x5050
#define    EXTI_AddressBase    0x50A0
#define     RST_AddressBase    0x50B0
#define     CLK_AddressBase    0x50C0
#define    IWDG_AddressBase    0x50E0
#define     AWU_AddressBase    0x50F0
#define    BEEP_AddressBase    0x50F3
#define     SPI_AddressBase    0x5200
#define     I2C_AddressBase    0x5210
#define   USART_AddressBase    0x5230
#define     WFE_AddressBase    0x50A6
#define    TIM2_AddressBase    0x5250
#define    TIM3_AddressBase    0x5280
#define    TIM4_AddressBase    0x52E0
#define   IRTIM_AddressBase    0x52FF
#define    COMP_AddressBase    0x5300
#define     CFG_AddressBase    0x7F60
#define     ITC_AddressBase    0x7F70
#define      DM_AddressBase    0x7F90
#define     UID_AddressBase    0x4925

/* include generic header for series */
#include "../stm8/stm8l10x/STM8L10x.h"

/*-----------------------------------------------------------------------------
END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // STM8S10X_ALL_H
