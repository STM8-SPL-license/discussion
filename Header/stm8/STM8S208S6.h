/*-------------------------------------------------------------------------
  STM8S208S6.h - Register Declarations for STM STM8S208S6

  Mainstream Performance line 8-bit MCU with 32 Kbytes Flash, 24 MHz CPU, integrated EEPROM

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
#ifndef _STM8S208S6_H
#define _STM8S208S6_H

// device specific memory sizes [B]
#define PFLASH_SIZE 32*1024
#define RAM_SIZE    6*1024
#define EEPROM_SIZE 1536

// define device line
#if !defined(STM8S208)
  #define STM8S208
#endif

// device specific base addresses
#define OPT_BaseAddress         0x4800
#define PORTA_BaseAddress       0x5000
#define PORTB_BaseAddress       0x5005
#define PORTC_BaseAddress       0x500A
#define PORTD_BaseAddress       0x500F
#define PORTE_BaseAddress       0x5014
#define PORTF_BaseAddress       0x5019
#define PORTG_BaseAddress       0x501E
#define PORTH_BaseAddress       0x5023
#define PORTI_BaseAddress       0x5028
#define FLASH_BaseAddress       0x505A
#define EXTI_BaseAddress        0x50A0
#define RST_BaseAddress         0x50B3
#define CLK_BaseAddress         0x50C0
#define WWDG_BaseAddress        0x50D1
#define IWDG_BaseAddress        0x50E0
#define AWU_BaseAddress         0x50F0
#define BEEP_BaseAddress        0x50F3
#define SPI_BaseAddress         0x5200
#define I2C_BaseAddress         0x5210
#define UART1_BaseAddress       0x5230
#define UART3_BaseAddress       0x5240
#define TIM1_BaseAddress        0x5250
#define TIM2_BaseAddress        0x5300
#define TIM3_BaseAddress        0x5320
#define TIM4_BaseAddress        0x5340
#define ADC2_BaseAddress        0x5400
#define CAN_BaseAddress         0x5420
#define CFG_BaseAddress         0x7F60
#define ITC_BaseAddress         0x7F70

// optional unique ID  start address
#define UID_BaseAddress   0x48CD

// include generic header for series
#include "STM8AF_STM8S"

/*-----------------------------------------------------------------------------
END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _STM8S208S6_H
