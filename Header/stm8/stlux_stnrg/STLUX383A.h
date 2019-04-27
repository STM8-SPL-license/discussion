/** \addtogroup STLUX_STNRG
*  @{
*/

/*-------------------------------------------------------------------------
  STLUX383A.h - Register Declarations for STM STLUX383A

  Digital controllers for lighting and power conversion applications with up to 6 programmable PWM generators, 96 MHz PLL, DALI

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
#ifndef STLUX383A_H
#define STLUX383A_H

/* define device & family */
#if !defined(STLUX383A)
  #define STLUX383A
#endif
#if !defined(STLUX)
  #define STLUX
#endif

/* device memory sizes [B] */
#define STM8_PFLASH_SIZE 32768
#define STM8_RAM_SIZE    2048
#define STM8_EEPROM_SIZE 1024

/* define device base addresses */
#define     OPT_AddressBase    0x4800
#define   PORT0_AddressBase    0x5000
#define   PORT1_AddressBase    0x5005
#define     MSC_AddressBase    0x5010
#define   FLASH_AddressBase    0x505A
#define     RST_AddressBase    0x50B3
#define     CLK_AddressBase    0x50B4
#define    WWDG_AddressBase    0x50D1
#define    IWDG_AddressBase    0x50E0
#define     AWU_AddressBase    0x50F0
#define     I2C_AddressBase    0x5210
#define    UART_AddressBase    0x5230
#define  SYSTIM_AddressBase    0x5340
#define    DALI_AddressBase    0x53C0
#define     ADC_AddressBase    0x5400
#define   SMED0_AddressBase    0x5500
#define   SMED1_AddressBase    0x5540
#define   SMED2_AddressBase    0x5580
#define   SMED3_AddressBase    0x55C0
#define   SMED4_AddressBase    0x5600
#define   SMED5_AddressBase    0x5640
#define     GCR_AddressBase    0x7F60
#define     ITC_AddressBase    0x7F70
#define      DM_AddressBase    0x7F80
#define     UID_AddressBase    0x48E0
#define   DEVID_AddressBase    0x4896

/* include generic header for series */
#include "STLUX_STNRG.h"

/*-----------------------------------------------------------------------------
END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _STLUX383A_H

/** @}*/

