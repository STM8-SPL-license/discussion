/*-------------------------------------------------------------------------
  STM8S105S4.h - Register Declarations for STM STM8S105S4

  Mainstream Access line 8-bit MCU with 16 Kbytes Flash, 16 MHz CPU, integrated EEPROM

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
#ifndef STM8S105S4_H
#define STM8S105S4_H

/* define device & family */
#if !defined(STM8S105S4)
  #define STM8S105S4
#endif
#if !defined(STM8S105)
  #define STM8S105
#endif

/* device memory sizes [B] */
#define STM8_PFLASH_SIZE 16*1024
#define STM8_RAM_SIZE    2*1024
#define STM8_EEPROM_SIZE 1024

/* define device base addresses */
#define     OPT    0x4800
#define   PORTA    0x5000
#define   PORTB    0x5005
#define   PORTC    0x500A
#define   PORTD    0x500F
#define   PORTE    0x5014
#define   PORTF    0x5019
#define   PORTG    0x501E
#define   FLASH    0x505A
#define    EXTI    0x50A0
#define     RST    0x50B3
#define     CLK    0x50C0
#define    WWDG    0x50D1
#define    IWDG    0x50E0
#define     AWU    0x50F0
#define    BEEP    0x50F3
#define     SPI    0x5200
#define     I2C    0x5210
#define   UART2    0x5240
#define    TIM1    0x5250
#define    TIM2    0x5300
#define    TIM3    0x5320
#define    TIM4    0x5340
#define    ADC1    0x53E0
#define     CFG    0x7F60
#define     ITC    0x7F70
#define      DM    0x7F90
#define     UID    0x48CD

/* include generic header for series */
#include "STM8AF_STM8S.h"

/*-----------------------------------------------------------------------------
END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _STM8S105S4_H
