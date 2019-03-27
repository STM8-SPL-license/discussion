/** \addtogroup STM8L10X
 *  @{
 */

/*-------------------------------------------------------------------------
  STM8L10x.h - Register Declarations for STM8L10x families

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
#ifndef STM8L10X_H
#define STM8L10X_H

/* Check the used compiler */
#if defined(__CSMC__)
  #define _COSMIC_
#elif defined(__RCST7__)
  #define _RAISONANCE_
#elif defined(__ICCSTM8__)
  #define _IAR_
#elif defined(__SDCC)
  #define _SDCC_
  #define SDCC_VERSION (__SDCC_VERSION_MAJOR * 10000 \
                      + __SDCC_VERSION_MINOR * 100 \
                      + __SDCC_VERSION_PATCH)
#else
  #error in 'STM8L10x.h': compiler not supported
#endif


/*-----------------------------------------------------------------------------
    MEMORY WIDTH
-----------------------------------------------------------------------------*/

// if memory sizes [B] are not given, assume smallest available in family
#if !defined(STM8_PFLASH_SIZE)
  #warning undefined STM8_PFLASH_SIZE, assume minimum
  #define STM8_PFLASH_SIZE      2048          ///< size of program flash [B]
#endif
#if !defined(STM8_RAM_SIZE)
  #warning undefined STM8_RAM_SIZE, assume minimum
  #define STM8_RAM_SIZE         1536          ///< size of RAM [B]
#endif
#if !defined(STM8_EEPROM_SIZE)
  #warning undefined STM8_EEPROM_SIZE, assume minimum
  #define STM8_EEPROM_SIZE      0             ///< size of data EEPROM [B]
#endif

// memory start / end addresses
#define STM8_PFLASH_START       0x8000        ///< first address in program flash
#define STM8_PFLASH_END         (STM8_PFLASH_START + STM8_PFLASH_SIZE - 1)  ///< last address in program flash
#define STM8_RAM_START          0x0000        ///< first address in RAM
#define STM8_RAM_END            (STM8_RAM_START + STM8_RAM_SIZE - 1)        ///< last address in RAM
#define STM8_EEPROM_START       9800          ///< first address in EEPROM
#define STM8_EEPROM_END         (STM8_EEPROM_START + STM8_EEPROM_SIZE - 1)  ///< last address in EEPROM

// address space width (>32kB flash exceeds 16bit, as flash starts at 0x8000)
#if (STM8_PFLASH_END <= 0xFFFF)
  #define STM8_ADDR_WIDTH       16            ///< width of address space
  #define STM8_MEM_POINTER_T    uint16_t      ///< address variable type
#else
  #define STM8_ADDR_WIDTH       32            ///< width of address space
  #define STM8_MEM_POINTER_T    uint32_t      ///< address variable type
#endif


/*-----------------------------------------------------------------------------
    COMPILER SPECIFIC SETTINGS
-----------------------------------------------------------------------------*/

// Cosmic compiler
#if defined(_COSMIC_)

  // macros to unify ISR declaration and implementation
  #define ISR_HANDLER(func,irq)  @far @interrupt void func(void)      ///< handler for interrupt service routine
  #define ISR_HANDLER_TRAP(func) void @far @interrupt func(void)      ///< handler for trap service routine

  // common assembler instructions
  #define NOP()                  _asm("nop")                          ///< perform a nop() operation (=minimum delay)
  #define DISABLE_INTERRUPTS()   _asm("sim")                          ///< disable interrupt handling
  #define ENABLE_INTERRUPTS()    _asm("rim")                          ///< enable interrupt handling
  #define TRIGGER_TRAP           _asm("trap")                         ///< trigger a trap (=soft interrupt) e.g. for EMC robustness (see AN1015)
  #define WAIT_FOR_INTERRUPT()   _asm("wfi")                          ///< stop code execution and wait for interrupt
  #define ENTER_HALT()           _asm("halt")                         ///< put controller to HALT mode
  #define SW_RESET()             (_WWDG_CR=0xBF)                      ///< reset controller via WWGD module

  // data type in bit fields
  #define _BITS                  unsigned int                         ///< data type in bit structs (follow C90 standard)

// Raisonance Compiler
#elif defined(_RAISONANCE_)

  // include intrinsic functions
  #include <intrins.h>

  // macros to unify ISR declaration and implementation
  #define ISR_HANDLER(func,irq)  void func(void) interrupt irq        ///< handler for interrupt service routine
  #define ISR_HANDLER_TRAP(func) void func(void) trap                 ///< handler for trap service routine

  // common assembler instructions
  #define NOP()                  _nop_()                              ///< perform a nop() operation (=minimum delay)
  #define DISABLE_INTERRUPTS()   _sim_()                              ///< disable interrupt handling
  #define ENABLE_INTERRUPTS()    _rim_()                              ///< enable interrupt handling
  #define TRIGGER_TRAP           _trap_()                             ///< trigger a trap (=soft interrupt) e.g. for EMC robustness (see AN1015)
  #define WAIT_FOR_INTERRUPT()   _wfi_()                              ///< stop code execution and wait for interrupt
  #define ENTER_HALT()           _halt_()                             ///< put controller to HALT mode
  #define SW_RESET()             (_WWDG_CR=0xBF)                      ///< reset controller via WWGD module

  // data type in bit fields
  #define _BITS                  unsigned int                         ///< data type in bit structs (follow C90 standard)

// IAR Compiler
#elif defined(_IAR_)

  // include intrinsic functions
  #include <intrinsics.h>

  // macros to unify ISR declaration and implementation
  #define ISR_HANDLER(func,irq)  __interrupt void func(void)          ///< handler for interrupt service routine
  //#define ISR_HANDLER_TRAP(func) void func(void) trap                 ///< handler for trap service routine

  // common assembler instructions
  #define NOP()                  __no_operation()                     ///< perform a nop() operation (=minimum delay)
  #define DISABLE_INTERRUPTS()   __disable_interrupt()                ///< disable interrupt handling
  #define ENABLE_INTERRUPTS()    __enable_interrupt()                 ///< enable interrupt handling
  #define TRIGGER_TRAP           __trap()                             ///< trigger a trap (=soft interrupt) e.g. for EMC robustness (see AN1015)
  #define WAIT_FOR_INTERRUPT()   __wait_for_interrupt()               ///< stop code execution and wait for interrupt
  #define ENTER_HALT()           __halt()                             ///< put controller to HALT mode
  #define SW_RESET()             (_WWDG_CR=0xBF)                      ///< reset controller via WWGD module

  // data type in bit fields
  #define _BITS                  unsigned char                        ///< data type in bit structs (deviating from C90 standard)

// SDCC compiler
#elif defined(_SDCC_)

  // macros to unify ISR declaration and implementation
  #define ISR_HANDLER(func,irq)   void func(void) __interrupt(irq)    ///< handler for interrupt service routine
  #if SDCC_VERSION >= 30403  // traps require >=v3.4.3
    #define ISR_HANDLER_TRAP(func)  void func() __trap                ///< handler for trap service routine
  #else
    #error traps require SDCC >=3.4.3. Please update!
  #endif

  // common assembler instructions
  #define NOP()                  __asm__("nop")                       ///< perform a nop() operation (=minimum delay)
  #define DISABLE_INTERRUPTS()   __asm__("sim")                       ///< disable interrupt handling
  #define ENABLE_INTERRUPTS()    __asm__("rim")                       ///< enable interrupt handling
  #define TRIGGER_TRAP           __asm__("trap")                      ///< trigger a trap (=soft interrupt) e.g. for EMC robustness (see AN1015)
  #define WAIT_FOR_INTERRUPT()   __asm__("wfi")                       ///< stop code execution and wait for interrupt
  #define ENTER_HALT()           __asm__("halt")                      ///< put controller to HALT mode
  #define SW_RESET()             (_WWDG_CR=0xBF)                      ///< reset controller via WWGD module

  // data type in bit fields
  #define _BITS                  unsigned int                         ///< data type in bit structs (follow C90 standard)

#endif


/*-----------------------------------------------------------------------------
    DEFINITION OF GLOBAL MACROS/#DEFINES
-----------------------------------------------------------------------------*/

// general macros
#define _SFR(type, addr)       (*((volatile type*) (addr)))           ///< peripheral register


/*-----------------------------------------------------------------------------
    STANDARD DATA TYPES
-----------------------------------------------------------------------------*/
#if defined(_COSMIC_) || defined(_RAISONANCE_) || defined(_IAR_)

  // skip if already defined
  #ifndef INT8_MAX

    // compiler specific --> If possible, use <stdint.h> from compiler
    typedef signed long     int32_t;
    typedef signed short    int16_t;
    typedef signed char     int8_t;

    typedef unsigned long   uint32_t;
    typedef unsigned short  uint16_t;
    typedef unsigned char   uint8_t;


    // define min/max values
    #define   INT8_MAX      0x7f
    #define   INT8_MIN      (-INT8_MAX - 1)

    #define   UINT8_MAX     0xFF
    #define   UINT8_MIN     0

    #define   INT16_MAX     0x7fff
    #define   INT16_MIN     (-INT16_MAX - 1)

    #define   UINT16_MAX    0xFFFF
    #define   UINT16_MIN    0

    #define   INT32_MAX     0x7fffffffL
    #define   INT32_MIN     (-INT32_MAX - 1L)

    #define   UINT32_MAX    0xFFFFFFFF
    #define   UINT32_MIN    0

  #endif // INT8_MAX

#elif defined(_SDCC_)

  // use compiler header
  #include <stdint.h>

#endif


/*-----------------------------------------------------------------------------
    ISR Vector Table (SDCC, Raisonance, IAR)
    Note: IAR has an IRQ offset of +2 compared to STM8 datasheet (see below)
-----------------------------------------------------------------------------*/

// reserved                       0
#define __FLASH_VECTOR__          1       ///< irq1 - flash interrupt
// reserved                       2
// reserved                       3
#define __AWU_VECTOR__            4       ///< irq4 - Auto Wake Up from Halt interrupt (AWU)
// reserved                       5
#define __PORTB_VECTOR__          6       ///< irq6 - External interrupt port B
#define __PORTD_VECTOR__          7       ///< irq7 - External interrupt port D
#define __EXTI0_VECTOR__          8       ///< irq8 - External interrupt 0
#define __EXTI1_VECTOR__          9       ///< irq9 - External interrupt 1
#define __EXTI2_VECTOR__          10      ///< irq10 - External interrupt 2
#define __EXTI3_VECTOR__          11      ///< irq11 - External interrupt 3
#define __EXTI4_VECTOR__          12      ///< irq12 - External interrupt 4
#define __EXTI5_VECTOR__          13      ///< irq13 - External interrupt 5
#define __EXTI6_VECTOR__          14      ///< irq14 - External interrupt 6
#define __EXTI7_VECTOR__          15      ///< irq15 - External interrupt 7
// reserved                       16
// reserved                       17
#define __COMP_VECTOR__           18      ///< irq18 - comparator interrupt
#define __TIM2_UPD_OVF_VECTOR__   19      ///< irq19 - TIM2 Update/overflow/trigger/break interrupt
#define __TIM2_CAPCOM_VECTOR__    20      ///< irq20 - TIM2 Capture/Compare interrupt
#define __TIM3_UPD_OVF_VECTOR__   21      ///< irq21 - TIM3 Update/overflow/break interrupt
#define __TIM3_CAPCOM_VECTOR__    22      ///< irq22 - TIM3 Capture/Compare interrupt
// reserved                       23
// reserved                       24
#define __TIM4_UPD_VECTOR__       25      ///< irq25 - TIM4 Update/trigger interrupt
#define __SPI_VECTOR__            26      ///< irq26 - SPI End of transfer interrupt
#define __USART_TXE_VECTOR__      27      ///< irq27 - USART send (TX empty) interrupt
#define __USART_RXF_VECTOR__      28      ///< irq28 - USART receive (RX full) interrupt
#define __I2C_VECTOR__            19      ///< irq29 - I2C interrupt



/*-----------------------------------------------------------------------------
    DEFINITION OF STM8 PERIPHERAL REGISTERS
-----------------------------------------------------------------------------*/

//------------------------
// General purpose input/output pins (PORT)
//------------------------
#if defined(PORTA_AddressBase) || defined(PORTB_AddressBase) || defined(PORTC_AddressBase) || defined(PORTD_AddressBase)

  /** @brief structure for controlling pins in PORT mode (PORTx, x=A..I) */
  typedef struct {

    /** @brief Port x output data register (Px_ODR) */
    struct {
      _BITS   PIN0    : 1;    ///< pin 0 output control
      _BITS   PIN1    : 1;    ///< pin 1 output control
      _BITS   PIN2    : 1;    ///< pin 2 output control
      _BITS   PIN3    : 1;    ///< pin 3 output control
      _BITS   PIN4    : 1;    ///< pin 4 output control
      _BITS   PIN5    : 1;    ///< pin 5 output control
      _BITS   PIN6    : 1;    ///< pin 6 output control
      _BITS   PIN7    : 1;    ///< pin 7 output control
    } ODR;

    /** @brief Port x input data register (Px_IDR) */
    struct {
      _BITS   PIN0    : 1;    ///< pin 0 input control
      _BITS   PIN1    : 1;    ///< pin 1 input control
      _BITS   PIN2    : 1;    ///< pin 2 input control
      _BITS   PIN3    : 1;    ///< pin 3 input control
      _BITS   PIN4    : 1;    ///< pin 4 input control
      _BITS   PIN5    : 1;    ///< pin 5 input control
      _BITS   PIN6    : 1;    ///< pin 6 input control
      _BITS   PIN7    : 1;    ///< pin 7 input control
    } IDR;

    /** @brief Port x data direction data register (Px_DDR) */
    struct {
      _BITS   PIN0    : 1;    ///< pin 0 direction control
      _BITS   PIN1    : 1;    ///< pin 1 direction control
      _BITS   PIN2    : 1;    ///< pin 2 direction control
      _BITS   PIN3    : 1;    ///< pin 3 direction control
      _BITS   PIN4    : 1;    ///< pin 4 direction control
      _BITS   PIN5    : 1;    ///< pin 5 direction control
      _BITS   PIN6    : 1;    ///< pin 6 direction control
      _BITS   PIN7    : 1;    ///< pin 7 direction control
    } DDR;

    /** @brief Port x control register 1 (Px_CR1) */
    struct {
      _BITS   PIN0    : 1;    ///< pin 0 control register 1
      _BITS   PIN1    : 1;    ///< pin 1 control register 1
      _BITS   PIN2    : 1;    ///< pin 2 control register 1
      _BITS   PIN3    : 1;    ///< pin 3 control register 1
      _BITS   PIN4    : 1;    ///< pin 4 control register 1
      _BITS   PIN5    : 1;    ///< pin 5 control register 1
      _BITS   PIN6    : 1;    ///< pin 6 control register 1
      _BITS   PIN7    : 1;    ///< pin 7 control register 1
    } CR1;

    /** @brief Port x control register 1 (Px_CR2) */
    struct {
      _BITS   PIN0    : 1;    ///< pin 0 control register 2
      _BITS   PIN1    : 1;    ///< pin 1 control register 2
      _BITS   PIN2    : 1;    ///< pin 2 control register 2
      _BITS   PIN3    : 1;    ///< pin 3 control register 2
      _BITS   PIN4    : 1;    ///< pin 4 control register 2
      _BITS   PIN5    : 1;    ///< pin 5 control register 2
      _BITS   PIN6    : 1;    ///< pin 6 control register 2
      _BITS   PIN7    : 1;    ///< pin 7 control register 2
    } CR2;

  } PORT_t;

  /* Pointer to port A registers */
  #if defined(PORTA_AddressBase)
    #define _GPIOA      _SFR(PORT_t,  PORTA_AddressBase)       ///< port A struct/bit access
    #define _GPIOA_ODR  _SFR(uint8_t, PORTA_AddressBase+0x00)  ///< port A output register
    #define _GPIOA_IDR  _SFR(uint8_t, PORTA_AddressBase+0x01)  ///< port A input register
    #define _GPIOA_DDR  _SFR(uint8_t, PORTA_AddressBase+0x02)  ///< port A direction register
    #define _GPIOA_CR1  _SFR(uint8_t, PORTA_AddressBase+0x03)  ///< port A control register 1
    #define _GPIOA_CR2  _SFR(uint8_t, PORTA_AddressBase+0x04)  ///< port A control register 2
  #endif

  /* Pointer to port B registers */
  #if defined(PORTB_AddressBase)
    #define _GPIOB      _SFR(PORT_t,  PORTB_AddressBase)       ///< port B struct/bit access
    #define _GPIOB_ODR  _SFR(uint8_t, PORTB_AddressBase+0x00)  ///< port B output register
    #define _GPIOB_IDR  _SFR(uint8_t, PORTB_AddressBase+0x01)  ///< port B input register
    #define _GPIOB_DDR  _SFR(uint8_t, PORTB_AddressBase+0x02)  ///< port B direction register
    #define _GPIOB_CR1  _SFR(uint8_t, PORTB_AddressBase+0x03)  ///< port B control register 1
    #define _GPIOB_CR2  _SFR(uint8_t, PORTB_AddressBase+0x04)  ///< port B control register 2
  #endif

  /* Pointer to port C registers */
  #if defined(PORTC_AddressBase)
    #define _GPIOC      _SFR(PORT_t,  PORTC_AddressBase)       ///< port C struct/bit access
    #define _GPIOC_ODR  _SFR(uint8_t, PORTC_AddressBase+0x00)  ///< port C output register
    #define _GPIOC_IDR  _SFR(uint8_t, PORTC_AddressBase+0x01)  ///< port C input register
    #define _GPIOC_DDR  _SFR(uint8_t, PORTC_AddressBase+0x02)  ///< port C direction register
    #define _GPIOC_CR1  _SFR(uint8_t, PORTC_AddressBase+0x03)  ///< port C control register 1
    #define _GPIOC_CR2  _SFR(uint8_t, PORTC_AddressBase+0x04)  ///< port C control register 2
  #endif

  /* Pointer to port D registers */
  #if defined(PORTD_AddressBase)
    #define _GPIOD      _SFR(PORT_t,  PORTD_AddressBase)       ///< port D struct/bit access
    #define _GPIOD_ODR  _SFR(uint8_t, PORTD_AddressBase+0x00)  ///< port D output register
    #define _GPIOD_IDR  _SFR(uint8_t, PORTD_AddressBase+0x01)  ///< port D input register
    #define _GPIOD_DDR  _SFR(uint8_t, PORTD_AddressBase+0x02)  ///< port D direction register
    #define _GPIOD_CR1  _SFR(uint8_t, PORTD_AddressBase+0x03)  ///< port D control register 1
    #define _GPIOD_CR2  _SFR(uint8_t, PORTD_AddressBase+0x04)  ///< port D control register 2
  #endif

  /* PORT Module Reset Values (all ports) */
  #define _GPIO_ODR_RESET_VALUE     ((uint8_t) 0x00)           ///< port output register reset value
  #define _GPIO_DDR_RESET_VALUE     ((uint8_t) 0x00)           ///< port direction register reset value
  #define _GPIO_CR1_RESET_VALUE     ((uint8_t) 0x00)           ///< port control register 1 reset value
  #define _GPIO_CR2_RESET_VALUE     ((uint8_t) 0x00)           ///< port control register 2 reset value

  /* macro for accessing pin registers (all ports) */
  #define _GPIO_PIN0   ((uint8_t) (0x01 << 0))                 ///< port bit mask for pin 0 (in _GPIOI_ODR, _GPIOI_IDR, _GPIOI_DDR, _GPIOI_CR1, _GPIOI_CR2)
  #define _GPIO_PIN1   ((uint8_t) (0x01 << 1))                 ///< port bit mask for pin 1 (in _GPIOI_ODR, _GPIOI_IDR, _GPIOI_DDR, _GPIOI_CR1, _GPIOI_CR2)
  #define _GPIO_PIN2   ((uint8_t) (0x01 << 2))                 ///< port bit mask for pin 2 (in _GPIOI_ODR, _GPIOI_IDR, _GPIOI_DDR, _GPIOI_CR1, _GPIOI_CR2)
  #define _GPIO_PIN3   ((uint8_t) (0x01 << 3))                 ///< port bit mask for pin 3 (in _GPIOI_ODR, _GPIOI_IDR, _GPIOI_DDR, _GPIOI_CR1, _GPIOI_CR2)
  #define _GPIO_PIN4   ((uint8_t) (0x01 << 4))                 ///< port bit mask for pin 4 (in _GPIOI_ODR, _GPIOI_IDR, _GPIOI_DDR, _GPIOI_CR1, _GPIOI_CR2)
  #define _GPIO_PIN5   ((uint8_t) (0x01 << 5))                 ///< port bit mask for pin 5 (in _GPIOI_ODR, _GPIOI_IDR, _GPIOI_DDR, _GPIOI_CR1, _GPIOI_CR2)
  #define _GPIO_PIN6   ((uint8_t) (0x01 << 6))                 ///< port bit mask for pin 6 (in _GPIOI_ODR, _GPIOI_IDR, _GPIOI_DDR, _GPIOI_CR1, _GPIOI_CR2)
  #define _GPIO_PIN7   ((uint8_t) (0x01 << 7))                 ///< port bit mask for pin 7 (in _GPIOI_ODR, _GPIOI_IDR, _GPIOI_DDR, _GPIOI_CR1, _GPIOI_CR2)

#endif // PORTx_AddressBase



//------------------------
// Non-volative memory (FLASH)
//------------------------
#if defined(FLASH_AddressBase)

  /** @brief struct to control write/erase of flash memory (FLASH) */
  typedef struct {

    /** @brief Flash control register 1 (FLASH_CR1) */
    struct {
      _BITS   FIX   : 1;    ///< Fixed Byte programming time
      _BITS   IE    : 1;    ///< Flash Interrupt enable
      _BITS         : 6;    //   Reserved
    } CR1;


    /** @brief Flash control register 2 (FLASH_CR2) */
    struct {
      _BITS   PRG   : 1;    ///< Standard block programming
      _BITS         : 3;    //   Reserved
      _BITS   FPRG  : 1;    ///< Fast block programming
      _BITS   ERASE : 1;    ///< Block erasing
      _BITS   WPRG  : 1;    ///< Word programming
      _BITS   OPT   : 1;    ///< Write option bytes
    } CR2;


    /** @brief Flash program memory unprotecting key register (FLASH_PUKR) */
    struct {
      _BITS   PUK    : 8;   ///< Program memory write unlock key
    } PUKR;


    /** @brief Data EEPROM unprotection key register (FLASH_DUKR) */
    struct {
      _BITS   DUK    : 8;   ///< Data EEPROM write unlock key
    } DUKR;


    /** @brief Flash status register (FLASH_IAPSR) */
    struct {
      _BITS   WR_PG_DIS : 1;  ///< Write attempted to protected page flag
      _BITS   PUL       : 1;  ///< Flash Program memory unlocked flag
      _BITS   EOP       : 1;  ///< End of programming (write or erase operation) flag
      _BITS   DUL       : 1;  ///< Data EEPROM area unlocked flag
      _BITS             : 4;  //   Reserved, forced by hardware to 0
    } IAPSR;

  } FLASH_t;

  /* Pointer to flash registers */
  #define _FLASH        _SFR(FLASH_t,  FLASH_AddressBase)        ///< Flash struct/bit access
  #define _FLASH_CR1    _SFR(uint8_t,  FLASH_AddressBase+0x00)   ///< Flash control register 1 (FLASH_CR1)
  #define _FLASH_CR2    _SFR(uint8_t,  FLASH_AddressBase+0x01)   ///< Flash control register 2 (FLASH_CR2)
  #define _FLASH_PUKR   _SFR(uint8_t,  FLASH_AddressBase+0x02)   ///< Flash program memory unprotecting key register (FLASH_PUKR)
  #define _FLASH_DUKR   _SFR(uint8_t,  FLASH_AddressBase+0x03)   ///< Data EEPROM unprotection key register (FLASH_DUKR)
  #define _FLASH_IAPSR  _SFR(uint8_t,  FLASH_AddressBase+0x04)   ///< Flash status register (FLASH_IAPSR)

  /* FLASH Module Reset Values */
  #define _FLASH_CR1_RESET_VALUE       ((uint8_t) 0x00)          ///< Flash control register 1 reset value
  #define _FLASH_CR2_RESET_VALUE       ((uint8_t) 0x00)          ///< Flash control register 2 reset value
  #define _FLASH_PUKR_RESET_VALUE      ((uint8_t) 0x00)          ///< Flash program memory unprotecting key reset value
  #define _FLASH_DUKR_RESET_VALUE      ((uint8_t) 0x00)          ///< Data EEPROM unprotection key reset value
  #define _FLASH_IAPSR_RESET_VALUE     ((uint8_t) 0x40)          ///< Flash status register reset value

  /* Flash control register 1 (_FLASH_CR1) */
  #define _FLASH_FIX              ((uint8_t) (0x01 << 0))        ///< Fixed Byte programming time [0] (in _FLASH_CR1)
  #define _FLASH_IE               ((uint8_t) (0x01 << 1))        ///< Flash Interrupt enable [0] (in _FLASH_CR1)
  // reserved [7:2]

  /* Flash control register 2 and complement (FLASH_CR2 and FLASH_NCR2) */
  #define _FLASH_PRG              ((uint8_t) (0x01 << 0))        ///< Standard block programming [0] (in _FLASH_CR2 and _FLASH_NCR2)
  // reserved [3:1]
  #define _FLASH_FPRG             ((uint8_t) (0x01 << 4))        ///< Fast block programming [0] (in _FLASH_CR2 and _FLASH_NCR2)
  #define _FLASH_ERASE            ((uint8_t) (0x01 << 5))        ///< Block erasing [0] (in _FLASH_CR2 and _FLASH_NCR2)
  #define _FLASH_WPRG             ((uint8_t) (0x01 << 6))        ///< Word programming [0] (in _FLASH_CR2 and _FLASH_NCR2)
  #define _FLASH_OPT              ((uint8_t) (0x01 << 7))        ///< Write option bytes [0] (in _FLASH_CR2 and _FLASH_NCR2)

  /* Flash status register (FLASH_IAPSR) */
  #define _FLASH_WR_PG_DIS        ((uint8_t) (0x01 << 0))        ///< Write attempted to protected page flag [0] (in _FLASH_IAPSR)
  #define _FLASH_PUL              ((uint8_t) (0x01 << 1))        ///< Flash Program memory unlocked flag [0] (in _FLASH_IAPSR)
  #define _FLASH_EOP              ((uint8_t) (0x01 << 2))        ///< End of programming (write or erase operation) flag [0] (in _FLASH_IAPSR)
  #define _FLASH_DUL              ((uint8_t) (0x01 << 3))        ///< Data EEPROM area unlocked flag [0] (in _FLASH_IAPSR)
  // reserved [7:4]

#endif // FLASH_AddressBase



//------------------------
// External interrupt control (EXTI)
//------------------------
#if defined(EXTI_AddressBase)

  /** @brief struct for configuring external port interrupts (EXTI) */
  typedef struct {

    /** @brief External interrupt control register 1 (EXTI_CR1) */
    struct {
      _BITS   P0IS    : 2;    ///< Portx bit 0 external interrupt sensitivity bits
      _BITS   P1IS    : 2;    ///< Portx bit 1 external interrupt sensitivity bits
      _BITS   P2IS    : 2;    ///< Portx bit 2 external interrupt sensitivity bits
      _BITS   P3IS    : 2;    ///< Portx bit 3 external interrupt sensitivity bits
    } CR1;


    /** @brief External interrupt control register 2 (EXTI_CR2) */
    struct {
      _BITS   P4IS    : 2;    ///< Portx bit 4 external interrupt sensitivity bits
      _BITS   P5IS    : 2;    ///< Portx bit 5 external interrupt sensitivity bits
      _BITS   P6IS    : 2;    ///< Portx bit 6 external interrupt sensitivity bits
      _BITS   P7IS    : 2;    ///< Portx bit 7 external interrupt sensitivity bits
    } CR2;


    /** @brief External interrupt control register 3 (EXTI_CR3) */
    struct {
      _BITS   PBIS    : 2;    ///< Port B external interrupt sensitivity bits
      _BITS   PDIS    : 2;    ///< Port D external interrupt sensitivity bits
      _BITS           : 4;    //   Reserved
    } CR3;


    /** @brief External interrupt status register 1 (EXTI_SR1) */
    struct {
      _BITS   P0F     : 1;    ///< Portx bit 0 external interrupt flag
      _BITS   P1F     : 1;    ///< Portx bit 1 external interrupt flag
      _BITS   P2F     : 1;    ///< Portx bit 2 external interrupt flag
      _BITS   P3F     : 1;    ///< Portx bit 3 external interrupt flag
      _BITS   P4F     : 1;    ///< Portx bit 4 external interrupt flag
      _BITS   P5F     : 1;    ///< Portx bit 5 external interrupt flag
      _BITS   P6F     : 1;    ///< Portx bit 6 external interrupt flag
      _BITS   P7F     : 1;    ///< Portx bit 7 external interrupt flag
    } SR1;


    /** @brief External interrupt status register 2 (EXTI_SR2) */
    struct {
      _BITS   PBF     : 1;    ///< Port B external interrupt flag
      _BITS   PDF     : 1;    ///< Port D external interrupt flag
      _BITS           : 6;    //   Reserved
    } SR2;


    /** @brief External interrupt port selector (EXTI_CONF) */
    struct {
      _BITS   PBLIS   : 1;    ///< Port B, pins 0..3 external interrupt select
      _BITS   PBHIS   : 1;    ///< Port B, pins 4..7 external interrupt select
      _BITS   PDLIS   : 1;    ///< Port D, pins 0..3 external interrupt select
      _BITS   PDHIS   : 1;    ///< Port D, pins 4..7 external interrupt select
      _BITS           : 4;    //   Reserved
    } CONF;

  } EXTI_t;

  /* Pointer to EXTI registers */
  #define _EXTI         _SFR(EXTI_t,   EXTI_AddressBase)         ///< External interrupt struct/bit access
  #define _EXTI_CR1     _SFR(uint8_t,  EXTI_AddressBase+0x00)    ///< External interrupt control register 1 (EXTI_CR1)
  #define _EXTI_CR2     _SFR(uint8_t,  EXTI_AddressBase+0x01)    ///< External interrupt control register 2 (EXTI_CR2)
  #define _EXTI_CR3     _SFR(uint8_t,  EXTI_AddressBase+0x02)    ///< External interrupt control register 3 (EXTI_CR2)
  #define _EXTI_SR1     _SFR(uint8_t,  EXTI_AddressBase+0x03)    ///< External interrupt status register 1 (EXTI_SR1)
  #define _EXTI_SR2     _SFR(uint8_t,  EXTI_AddressBase+0x04)    ///< External interrupt status register 2 (EXTI_SR2)
  #define _EXTI_CONF    _SFR(uint8_t,  EXTI_AddressBase+0x05)    ///< External interrupt port selector (EXTI_CONF)

  /* EXTI Module Reset Values */
  #define _EXTI_CR1_RESET_VALUE        ((uint8_t) 0x00)          ///< External interrupt control register 1 reset value
  #define _EXTI_CR2_RESET_VALUE        ((uint8_t) 0x00)          ///< External interrupt control register 2 reset value
  #define _EXTI_CR3_RESET_VALUE        ((uint8_t) 0x00)          ///< External interrupt control register 3 reset value
  #define _EXTI_SR1_RESET_VALUE        ((uint8_t) 0x00)          ///< External interrupt status register 1 reset value
  #define _EXTI_SR2_RESET_VALUE        ((uint8_t) 0x00)          ///< External interrupt status register 2 reset value
  #define _EXTI_CONF_RESET_VALUE       ((uint8_t) 0x00)          ///< External interrupt port selector reset value

  /* External interrupt control register 1 (EXTI_CR1) */
  #define _EXTI_P0IS              ((uint8_t) (0x03 << 0))        ///< External interrupt sensitivity for Portx bit 0 [1:0] (in _EXTI_CR1)
  #define _EXTI_P0IS0             ((uint8_t) (0x01 << 0))        ///< External interrupt sensitivity for Portx bit 0 [0] (in _EXTI_CR1)
  #define _EXTI_P0IS1             ((uint8_t) (0x01 << 1))        ///< External interrupt sensitivity for Portx bit 0 [1] (in _EXTI_CR1)
  #define _EXTI_P1IS              ((uint8_t) (0x03 << 0))        ///< External interrupt sensitivity for Portx bit 1 [1:0] (in _EXTI_CR1)
  #define _EXTI_P1IS0             ((uint8_t) (0x01 << 0))        ///< External interrupt sensitivity for Portx bit 1 [0] (in _EXTI_CR1)
  #define _EXTI_P1IS1             ((uint8_t) (0x01 << 1))        ///< External interrupt sensitivity for Portx bit 1 [1] (in _EXTI_CR1)
  #define _EXTI_P2IS              ((uint8_t) (0x03 << 0))        ///< External interrupt sensitivity for Portx bit 2 [1:0] (in _EXTI_CR1)
  #define _EXTI_P2IS0             ((uint8_t) (0x01 << 0))        ///< External interrupt sensitivity for Portx bit 2 [0] (in _EXTI_CR1)
  #define _EXTI_P2IS1             ((uint8_t) (0x01 << 1))        ///< External interrupt sensitivity for Portx bit 2 [1] (in _EXTI_CR1)
  #define _EXTI_P3IS              ((uint8_t) (0x03 << 0))        ///< External interrupt sensitivity for Portx bit 3 [1:0] (in _EXTI_CR1)
  #define _EXTI_P3IS0             ((uint8_t) (0x01 << 0))        ///< External interrupt sensitivity for Portx bit 3 [0] (in _EXTI_CR1)
  #define _EXTI_P3IS1             ((uint8_t) (0x01 << 1))        ///< External interrupt sensitivity for Portx bit 3 [1] (in _EXTI_CR1)

  /* External interrupt control register 2 (EXTI_CR2) */
  #define _EXTI_P4IS              ((uint8_t) (0x03 << 0))        ///< External interrupt sensitivity for Portx bit 4 [1:0] (in _EXTI_CR2)
  #define _EXTI_P4IS0             ((uint8_t) (0x01 << 0))        ///< External interrupt sensitivity for Portx bit 4 [0] (in _EXTI_CR2)
  #define _EXTI_P4IS1             ((uint8_t) (0x01 << 1))        ///< External interrupt sensitivity for Portx bit 4 [1] (in _EXTI_CR2)
  #define _EXTI_P5IS              ((uint8_t) (0x03 << 0))        ///< External interrupt sensitivity for Portx bit 5 [1:0] (in _EXTI_CR2)
  #define _EXTI_P5IS0             ((uint8_t) (0x01 << 0))        ///< External interrupt sensitivity for Portx bit 5 [0] (in _EXTI_CR2)
  #define _EXTI_P5IS1             ((uint8_t) (0x01 << 1))        ///< External interrupt sensitivity for Portx bit 5 [1] (in _EXTI_CR2)
  #define _EXTI_P6IS              ((uint8_t) (0x03 << 0))        ///< External interrupt sensitivity for Portx bit 6 [1:0] (in _EXTI_CR2)
  #define _EXTI_P6IS0             ((uint8_t) (0x01 << 0))        ///< External interrupt sensitivity for Portx bit 6 [0] (in _EXTI_CR2)
  #define _EXTI_P6IS1             ((uint8_t) (0x01 << 1))        ///< External interrupt sensitivity for Portx bit 6 [1] (in _EXTI_CR2)
  #define _EXTI_P7IS              ((uint8_t) (0x03 << 0))        ///< External interrupt sensitivity for Portx bit 7 [1:0] (in _EXTI_CR2)
  #define _EXTI_P7IS0             ((uint8_t) (0x01 << 0))        ///< External interrupt sensitivity for Portx bit 7 [0] (in _EXTI_CR2)
  #define _EXTI_P7IS1             ((uint8_t) (0x01 << 1))        ///< External interrupt sensitivity for Portx bit 7 [1] (in _EXTI_CR2)

  /* External interrupt control register 3 (EXTI_CR3) */
  #define _EXTI_PBIS              ((uint8_t) (0x03 << 0))        ///< Port B external interrupt sensitivity bits [1:0] (in _EXTI_CR3)
  #define _EXTI_PBIS0             ((uint8_t) (0x01 << 0))        ///< Port B external interrupt sensitivity bits [0] (in _EXTI_CR3)
  #define _EXTI_PBIS1             ((uint8_t) (0x01 << 1))        ///< Port B external interrupt sensitivity bits [1] (in _EXTI_CR3)
  #define _EXTI_PDIS              ((uint8_t) (0x03 << 0))        ///< Port D external interrupt sensitivity bits [1:0] (in _EXTI_CR3)
  #define _EXTI_PDIS0             ((uint8_t) (0x01 << 0))        ///< Port D external interrupt sensitivity bits [0] (in _EXTI_CR3)
  #define _EXTI_PDIS1             ((uint8_t) (0x01 << 1))        ///< Port D external interrupt sensitivity bits [1] (in _EXTI_CR3)
  // reserved [7:4]

  /* External interrupt status register 1 (EXTI_SR1) */
  #define _EXTI_P0F               ((uint8_t) (0x01 << 0))        ///< Portx bit 0 external interrupt flag (in _EXTI_SR1)
  #define _EXTI_P1F               ((uint8_t) (0x02 << 0))        ///< Portx bit 1 external interrupt flag (in _EXTI_SR1)
  #define _EXTI_P2F               ((uint8_t) (0x04 << 0))        ///< Portx bit 2 external interrupt flag (in _EXTI_SR1)
  #define _EXTI_P3F               ((uint8_t) (0x08 << 0))        ///< Portx bit 3 external interrupt flag (in _EXTI_SR1)
  #define _EXTI_P4F               ((uint8_t) (0x10 << 0))        ///< Portx bit 4 external interrupt flag (in _EXTI_SR1)
  #define _EXTI_P5F               ((uint8_t) (0x20 << 0))        ///< Portx bit 5 external interrupt flag (in _EXTI_SR1)
  #define _EXTI_P6F               ((uint8_t) (0x40 << 0))        ///< Portx bit 6 external interrupt flag (in _EXTI_SR1)
  #define _EXTI_P7F               ((uint8_t) (0x80 << 0))        ///< Portx bit 7 external interrupt flag (in _EXTI_SR1)

  /* External interrupt status register 2 (EXTI_SR2) */
  #define _EXTI_PBF               ((uint8_t) (0x01 << 0))        ///< Port B external interrupt flag (in _EXTI_SR2)
  #define _EXTI_PDF               ((uint8_t) (0x02 << 0))        ///< Port D external interrupt flag (in _EXTI_SR2)
  // reserved [7:2]

  /* External interrupt port selector (EXTI_CONF) */
  #define _EXTI_PBLIS             ((uint8_t) (0x01 << 0))        ///< Port B, pins 0..3 external interrupt select (in _EXTI_CONF)
  #define _EXTI_PBHIS             ((uint8_t) (0x02 << 0))        ///< Port B, pins 4..7 external interrupt select (in _EXTI_CONF)
  #define _EXTI_PDLIS             ((uint8_t) (0x01 << 0))        ///< Port D, pins 0..3 external interrupt select (in _EXTI_CONF)
  #define _EXTI_PDHIS             ((uint8_t) (0x02 << 0))        ///< Port D, pins 4..7 external interrupt select (in _EXTI_CONF)
  // reserved [7:4]

#endif // EXTI_AddressBase



//------------------------
// Reset status (RST)
//------------------------
#if defined(RST_AddressBase)

  /** @brief struct for determining reset source (RST) */
  typedef struct {

    /** @brief Reset pin configuration register (RST_CR) */
    struct {
      _BITS   PIN_KEY : 8;    ///< Reset pin configuration key
    } CR;

    /** @brief Reset status register (RST_SR) */
    struct {
      _BITS   PORF    : 1;    ///< Power-on reset (POR) flag
      _BITS   IWDGF   : 1;    ///< Independent Watchdog reset flag
      _BITS   ILLOPF  : 1;    ///< Illegal opcode reset flag
      _BITS   SWIMF   : 1;    ///< SWIM reset flag
      _BITS           : 4;    //   Reserved
    } SR;

  } RST_t;

  /* Pointer to reset status register */
  #define _RST     _SFR(RST_t,         RST_AddressBase)          ///< Reset module struct/bit access
  #define _RST_CR  _SFR(uint8_t,       RST_AddressBase+0x00)     ///< Reset pin configuration register (RST_CR)
  #define _RST_SR  _SFR(uint8_t,       RST_AddressBase+0x01)     ///< Reset module status register (RST_SR)

  /*  Reset pin configuration register (RST_CR) */
  #define _RST_PIN_KEY            ((uint8_t) 0xD0)               ///< Configure PA1 as GPIO, else NRST (in _RST_CR)

  /*  Reset module status register (RST_SR) */
  #define _RST_PORF               ((uint8_t) (0x01 << 0))        ///< Power-on reset (POR) flag [0] (in _RST_SR)
  #define _RST_IWDGF              ((uint8_t) (0x01 << 1))        ///< Independent Watchdog reset flag [0] (in _RST_SR)
  #define _RST_ILLOPF             ((uint8_t) (0x01 << 2))        ///< Illegal opcode reset flag [0] (in _RST_SR)
  #define _RST_SWIMF              ((uint8_t) (0x01 << 3))        ///< SWIM reset flag [0] (in _RST_SR)
  // reserved [7:4]

#endif // RST_AddressBase



//------------------------
// Clock control (CLK)
//------------------------
#if defined(CLK_AddressBase)

  /** @brief struct for configuring/monitoring clock module (CLK) */
  typedef struct {

    /** @brief Internal clock register (CLK_ICKR) */
    struct {
      _BITS   HSIDIV  : 2;    ///< High speed internal clock prescaler
      _BITS           : 6;    //   Reserved
    } CKDIVR;

    /** @brief Reserved register (2B) */
    uint8_t res     [2];

    /** @brief Peripheral clock gating register (CLK_PCKENR) */
    struct {
      _BITS   PCKEN_TIM2      : 1;    ///< clock enable TIM2
      _BITS   PCKEN_TIM3      : 1;    ///< clock enable TIM3
      _BITS   PCKEN_TIM4      : 1;    ///< clock enable TIM4
      _BITS   PCKEN_I2C       : 1;    ///< clock enable I2C
      _BITS   PCKEN_SPI       : 1;    ///< clock enable SPI
      _BITS   PCKEN_USART     : 1;    ///< clock enable USART
      _BITS   PCKEN_AWU_BEEP  : 1;    ///< clock enable AWU/BEEP
      _BITS                   : 1;    //   Reserved
    } PCKENR1;

    /** @brief Reserved register (1B) */
    uint8_t res2    [1];

    /** @brief Configurable clock output register (CLK_CCOR) */
    struct {
      _BITS   CCOEN   : 1;    ///< Configurable clock output enable
      _BITS   CCOSEL  : 2;    ///< Configurable clock output selection
      _BITS           : 5;    //   Reserved
    } CCOR;

  } CLK_t;

  /* Pointer to CLK registers */
  #define _CLK          _SFR(CLK_t,    CLK_AddressBase)          ///< Clock module struct/bit access
  #define _CLK_CKDIVR   _SFR(uint8_t,  CLK_AddressBase+0x00)     ///< Clock Divider Register
  // reserved (2B)
  #define _CLK_PCKENR   _SFR(uint8_t,  CLK_AddressBase+0x03)     ///< Peripheral clock gating register
  // reserved (1B)
  #define _CLK_CCOR     _SFR(uint8_t,  CLK_AddressBase+0x05)     ///< Configurable clock output register

  /* CLK Module Reset Values */
  #define _CLK_CKDIVR_RESET_VALUE      ((uint8_t) 0x03)          ///< Clock divider register reset value
  #define _CLK_PCKENR_RESET_VALUE      ((uint8_t) 0x00)          ///< Peripheral clock gating register reset value
  #define _CLK_CCOR_RESET_VALUE        ((uint8_t) 0x00)          ///< Configurable clock output register reset value

  /* Clock divider register (CLK_CKDIVR) */
  #define _CLK_HSIDIV             ((uint8_t) (0x03 << 0))        ///< High speed internal clock prescaler [1:0] (in _CLK_CKDIVR)
  #define _CLK_HSIDIV0            ((uint8_t) (0x01 << 0))        ///< High speed internal clock prescaler [0] (in _CLK_CKDIVR)
  #define _CLK_HSIDIV1            ((uint8_t) (0x01 << 1))        ///< High speed internal clock prescaler [1] (in _CLK_CKDIVR)

  /* pre-defined constants for _CLK_CKDIVR */
  #define _CLK_HSIDIV_DIV1        ((uint8_t) (0x00 << 0))        ///< set HSI prescaler to 1 (in _CLK_CKDIVR)
  #define _CLK_HSIDIV_DIV2        ((uint8_t) (0x01 << 0))        ///< set HSI prescaler to 1/2 (in _CLK_CKDIVR)
  #define _CLK_HSIDIV_DIV4        ((uint8_t) (0x02 << 0))        ///< set HSI prescaler to 1/4 (in _CLK_CKDIVR)
  #define _CLK_HSIDIV_DIV8        ((uint8_t) (0x03 << 0))        ///< set HSI prescaler to 1/8 (in _CLK_CKDIVR)

  /* Peripheral clock gating register 1 (CLK_PCKENR1) */
  #define _CLK_TIM2               ((uint8_t) (0x01 << 0))        ///< clock enable TIM2 [0] (in _CLK_PCKENR1)
  #define _CLK_TIM3               ((uint8_t) (0x01 << 1))        ///< clock enable TIM3 [0] (in _CLK_PCKENR1)
  #define _CLK_TIM4               ((uint8_t) (0x01 << 2))        ///< clock enable TIM4 [0] (in _CLK_PCKENR1)
  #define _CLK_I2C                ((uint8_t) (0x01 << 3))        ///< clock enable I2C [0] (in _CLK_PCKENR1)
  #define _CLK_SPI                ((uint8_t) (0x01 << 4))        ///< clock enable SPI [0] (in _CLK_PCKENR1)
  #define _CLK_USART              ((uint8_t) (0x01 << 5))        ///< clock enable USART [0] (in _CLK_PCKENR1)
  #define _CLK_AWU_BEEP           ((uint8_t) (0x01 << 6))        ///< clock enable AWU/BEEP [0] (in _CLK_PCKENR1)
  // reserved [7]

  /* Configurable clock output register (CLK_CCOR) */
  #define _CLK_CCOEN              ((uint8_t) (0x01 << 0))        ///< Configurable clock output enable [0] (in _CLK_CCOR)
  #define _CLK_CCOSEL             ((uint8_t) (0x03 << 1))        ///< Configurable clock output selection [1:0] (in _CLK_CCOR)
  #define _CLK_CCOSEL0            ((uint8_t) (0x01 << 1))        ///< Configurable clock output selection [0] (in _CLK_CCOR)
  #define _CLK_CCOSEL1            ((uint8_t) (0x01 << 2))        ///< Configurable clock output selection [1] (in _CLK_CCOR)

  /* pre-defined constants for _CLK_CCOR */
  #define _CLK_CCOSEL_DIV1        ((uint8_t) (0x00 << 1))        ///< set clock output selection to 1 (in _CLK_CCOR)
  #define _CLK_CCOSEL_DIV2        ((uint8_t) (0x01 << 1))        ///< set clock output selection to 1/2 (in _CLK_CCOR)
  #define _CLK_CCOSEL_DIV4        ((uint8_t) (0x02 << 1))        ///< set clock output selection to 1/4 (in _CLK_CCOR)
  #define _CLK_CCOSEL_DIV16       ((uint8_t) (0x03 << 1))        ///< set clock output selection to 1/16 (in _CLK_CCOR)
  // reserved [7:3]

#endif // CLK_AddressBase



//------------------------
// Independent Timeout Watchdog (IWDG)
//------------------------
#if defined(IWDG_AddressBase)

  /** @brief struct for access to Independent Timeout Watchdog registers (IWDG) */
  typedef struct {

    /** @brief IWDG Key register (IWDG_KR) */
    struct {
      _BITS KEY       : 8;  ///< IWDG Key
    } KR;


    /** @brief IWDG Prescaler register (IWDG_PR) */
    struct {
      _BITS   PRE     : 3;  ///< Prescaler divider
      _BITS           : 5;  //   Reserved
    } PR;


    /** @brief IWDG Reload register (IWDG_RLR) */
    struct {
      _BITS RL        : 8;  ///< IWDG Reload value
    } RLR;

  } IWDG_t;

  /* Pointer to Independent Timeout Watchdog registers */
  #define _IWDG         _SFR(IWDG_t,   IWDG_AddressBase)         ///< Independent Timeout Watchdog struct/bit access
  #define _IWDG_KR      _SFR(uint8_t,  IWDG_AddressBase+0x00)    ///< Independent Timeout Watchdog Key register (IWDG_KR)
  #define _IWDG_PR      _SFR(uint8_t,  IWDG_AddressBase+0x01)    ///< Independent Timeout Watchdog Prescaler register (IWDG_PR)
  #define _IWDG_RLR     _SFR(uint8_t,  IWDG_AddressBase+0x02)    ///< Independent Timeout Watchdog Reload register (IWDG_RLR)

  /* IWDG Module Reset Values */
  #define _IWDG_PR_RESET_VALUE         ((uint8_t) 0x00)          ///< Independent Timeout Watchdog Prescaler register reset value
  #define _IWDG_RLR_RESET_VALUE        ((uint8_t) 0xFF)          ///< Independent Timeout Watchdog Reload register reset value

  /* pre-defined Key constants for _IWDG_KR keys */
  #define _IWDG_KEY_ENABLE        ((uint8_t) 0xCC)               ///< Independent Timeout Watchdog enable (in _IWDG_KR)
  #define _IWDG_KEY_REFRESH       ((uint8_t) 0xAA)               ///< Independent Timeout Watchdog refresh (in _IWDG_KR)
  #define _IWDG_KEY_ACCESS        ((uint8_t) 0x55)               ///< Independent Timeout Watchdog unlock write to IWDG_PR and IWDG_RLR (in _IWDG_KR)

  /* IWDG Prescaler register (IWDG_PR) bits */
  #define _IWDG_PRE               ((uint8_t) (0x07 << 0))        ///< Independent Timeout Watchdog Prescaler divider [2:0] (in _IWDG_PR)
  #define _IWDG_PRE0              ((uint8_t) (0x01 << 0))        ///< Independent Timeout Watchdog Prescaler divider [0] (in _IWDG_PR)
  #define _IWDG_PRE1              ((uint8_t) (0x01 << 1))        ///< Independent Timeout Watchdog Prescaler divider [1] (in _IWDG_PR)
  #define _IWDG_PRE2              ((uint8_t) (0x01 << 2))        ///< Independent Timeout Watchdog Prescaler divider [2] (in _IWDG_PR)
  // reserved [7:3]

#endif // IWDG_AddressBase



//------------------------
// Auto Wake-Up Module (AWU)
//------------------------
#if defined(AWU_AddressBase)

  /** @brief struct for cofiguring the Auto Wake-Up Module (AWU) */
  typedef struct {

    /** @brief AWU Control/status register (AWU_CSR) */
    struct {
      _BITS   MSR     : 1;    ///< LSI measurement enable
      _BITS           : 3;    //   Reserved
      _BITS   AWUEN   : 1;    ///< Auto-wakeup enable
      _BITS   AWUF    : 1;    ///< Auto-wakeup flag
      _BITS           : 2;    //   Reserved
    } CSR;


    /** @brief AWU Asynchronous prescaler register (AWU_APR) */
    struct {
      _BITS   APRE    : 6;    ///< Asynchronous prescaler divider
      _BITS           : 2;    //   Reserved
    } APR;


    /** @brief AWU Timebase selection register (AWU_TBR) */
    struct {
      _BITS   AWUTB   : 4;    ///< Auto-wakeup timebase selection
      _BITS           : 4;    //   Reserved
    } TBR;

  } AWU_t;

  /* Pointer to AWU registers */
  #define _AWU          _SFR(AWU_t,    AWU_AddressBase)          ///< Auto Wake-Up struct/bit access
  #define _AWU_CSR      _SFR(uint8_t,  AWU_AddressBase+0x00)     ///< Auto Wake-Up Control/status register (AWU_CSR)
  #define _AWU_APR      _SFR(uint8_t,  AWU_AddressBase+0x01)     ///< Auto Wake-Up Asynchronous prescaler register (AWU_APR)
  #define _AWU_TBR      _SFR(uint8_t,  AWU_AddressBase+0x02)     ///< Auto Wake-Up Timebase selection register (AWU_TBR)

  /* AWU Module Reset Values */
  #define _AWU_CSR_RESET_VALUE         ((uint8_t) 0x00)          ///< Auto Wake-Up Control/status register reset value
  #define _AWU_APR_RESET_VALUE         ((uint8_t) 0x3F)          ///< Auto Wake-Up Asynchronous prescaler register reset value
  #define _AWU_TBR_RESET_VALUE         ((uint8_t) 0x00)          ///< Auto Wake-Up Timebase selection register reset value

  /* AWU Control/status register (AWU_CSR) */
  #define _AWU_MSR                ((uint8_t) (0x01 << 0))        ///< Auto Wake-Up LSI measurement enable [0] (in _AWU_CSR)
  // reserved [3:1]
  #define _AWU_AWUEN              ((uint8_t) (0x01 << 4))        ///< Auto-wakeup enable [0] (in _AWU_CSR)
  #define _AWU_AWUF               ((uint8_t) (0x01 << 5))        ///< Auto-wakeup status flag [0] (in _AWU_CSR)
  // reserved [7:6]

  /* AWU Asynchronous prescaler register (AWU_APR) */
  #define _AWU_APRE               ((uint8_t) (0x3F << 0))        ///< Auto-wakeup asynchronous prescaler divider [5:0] (in _AWU_APR)
  #define _AWU_APRE0              ((uint8_t) (0x01 << 0))        ///< Auto-wakeup asynchronous prescaler divider [0] (in _AWU_APR)
  #define _AWU_APRE1              ((uint8_t) (0x01 << 1))        ///< Auto-wakeup asynchronous prescaler divider [1] (in _AWU_APR)
  #define _AWU_APRE2              ((uint8_t) (0x01 << 2))        ///< Auto-wakeup asynchronous prescaler divider [2] (in _AWU_APR)
  #define _AWU_APRE3              ((uint8_t) (0x01 << 3))        ///< Auto-wakeup asynchronous prescaler divider [3] (in _AWU_APR)
  #define _AWU_APRE4              ((uint8_t) (0x01 << 4))        ///< Auto-wakeup asynchronous prescaler divider [4] (in _AWU_APR)
  #define _AWU_APRE5              ((uint8_t) (0x01 << 5))        ///< Auto-wakeup asynchronous prescaler divider [5] (in _AWU_APR)
  // reserved [7:6]

  /* AWU Asynchronous prescaler register (AWU_APR) */
  #define _AWU_AWUTB              ((uint8_t) (0x0F << 0))        ///< Auto-wakeup timebase selection [3:0] (in _AWU_APR)
  #define _AWU_AWUTB0             ((uint8_t) (0x01 << 0))        ///< Auto-wakeup timebase selection [0] (in _AWU_APR)
  #define _AWU_AWUTB1             ((uint8_t) (0x01 << 1))        ///< Auto-wakeup timebase selection [1] (in _AWU_APR)
  #define _AWU_AWUTB2             ((uint8_t) (0x01 << 2))        ///< Auto-wakeup timebase selection [2] (in _AWU_APR)
  #define _AWU_AWUTB3             ((uint8_t) (0x01 << 3))        ///< Auto-wakeup timebase selection [3] (in _AWU_APR)
  // reserved [7:4]

#endif // AWU_AddressBase



//------------------------
// Beeper module BEEP (all devices, may require option byte change)
//------------------------
#if defined(BEEP_AddressBase)

  /** @brief struct for beeper control (BEEP) */
  typedef struct {

    /** @brief Beeper control/status register (BEEP_CSR) */
    struct {
      _BITS   BEEPDIV : 5;    ///< Beep clock prescaler divider
      _BITS   BEEPEN  : 1;    ///< Beep enable
      _BITS   BEEPSEL : 2;    ///< Beeper frequency selection
    } CSR;

  } BEEP_t;

  /* Pointer to BEEP registers */
  #define _BEEP         _SFR(BEEP_t,   BEEP_AddressBase)         ///< Beeper struct/bit access
  #define _BEEP_CSR     _SFR(uint8_t,  BEEP_AddressBase+0x00)    ///< Beeper control/status register (BEEP_CSR)

  /* BEEP Module Reset Values */
  #define _BEEP_CSR_RESET_VALUE        ((uint8_t) 0x1F)          ///< Beeper control/status register reset value

  /* Beeper control/status register (BEEP_CSR) */
  #define _BEEP_BEEPDIV           ((uint8_t) (0x1F << 0))        ///< Beeper clock prescaler divider [4:0] (in _BEEP_CSR)
  #define _BEEP_BEEPDIV0          ((uint8_t) (0x01 << 0))        ///< Beeper clock prescaler divider [0] (in _BEEP_CSR)
  #define _BEEP_BEEPDIV1          ((uint8_t) (0x01 << 1))        ///< Beeper clock prescaler divider [1] (in _BEEP_CSR)
  #define _BEEP_BEEPDIV2          ((uint8_t) (0x01 << 2))        ///< Beeper clock prescaler divider [2] (in _BEEP_CSR)
  #define _BEEP_BEEPDIV3          ((uint8_t) (0x01 << 3))        ///< Beeper clock prescaler divider [3] (in _BEEP_CSR)
  #define _BEEP_BEEPDIV4          ((uint8_t) (0x01 << 4))        ///< Beeper clock prescaler divider [4] (in _BEEP_CSR)
  #define _BEEP_BEEPEN            ((uint8_t) (0x01 << 5))        ///< Beeper enable [0] (in _BEEP_CSR)
  #define _BEEP_BEEPSEL           ((uint8_t) (0x03 << 6))        ///< Beeper frequency selection [1:0] (in _BEEP_CSR)
  #define _BEEP_BEEPSEL0          ((uint8_t) (0x01 << 6))        ///< Beeper frequency selection [0] (in _BEEP_CSR)
  #define _BEEP_BEEPSEL1          ((uint8_t) (0x01 << 7))        ///< Beeper frequency selection [1] (in _BEEP_CSR)

#endif // BEEP_AddressBase



//------------------------
// Serial Peripheral Interface SPI
//------------------------
#if defined(SPI_AddressBase)

  /** @brief struct for controlling SPI module (SPI) */
  typedef struct {

    /** @brief SPI control register 1 (SPI_CR1) */
    struct {
      _BITS   CPHA     : 1;    ///< Clock phase
      _BITS   CPOL     : 1;    ///< Clock polarity
      _BITS   MSTR     : 1;    ///< Master/slave selection
      _BITS   BR       : 3;    ///< Baudrate control
      _BITS   SPE      : 1;    ///< SPI enable
      _BITS   LSBFIRST : 1;    ///< Frame format
    } CR1;


    /** @brief SPI control register 2 (SPI_CR2) */
    struct {
      _BITS   SSI     : 1;    ///< Internal slave select
      _BITS   SSM     : 1;    ///< Software slave management
      _BITS   RXONLY  : 1;    ///< Receive only
      _BITS           : 3;    //   Reserved
      _BITS   BDOE    : 1;    ///< Input/Output enable in bidirectional mode
      _BITS   BDM     : 1;    ///< Bidirectional data mode enable
    } CR2;


    /** @brief SPI interrupt control register (SPI_ICR) */
    struct {
      _BITS           : 4;    //   Reserved
      _BITS   WKIE    : 1;    ///< Wakeup interrupt enable
      _BITS   ERRIE   : 1;    ///< Error interrupt enable
      _BITS   RXIE    : 1;    ///< Rx buffer not empty interrupt enable
      _BITS   TXIE    : 1;    ///< Tx buffer empty interrupt enable
    } ICR;


    /** @brief SPI status register (SPI_SR) */
    struct {
      _BITS   RXNE    : 1;    ///< Receive buffer not empty
      _BITS   TXE     : 1;    ///< Transmit buffer empty
      _BITS           : 1;    //   Reserved
      _BITS   WKUP    : 1;    ///< Wakeup flag
      _BITS           : 1;    //   Reserved
      _BITS   MODF    : 1;    ///< Mode fault
      _BITS   OVR     : 1;    ///< Overrun flag
      _BITS   BSY     : 1;    ///< Busy flag
    } SR;


    /** @brief SPI data register (SPI_DR) */
    struct {
      _BITS   DATA    : 8;    ///< SPI data
    } DR;

  } SPI_t;

  /// register for SPI control
  #define _SPI        _SFR(SPI_t,      SPI_AddressBase)          ///< SPI struct/bit access
  #define _SPI_CR1    _SFR(uint8_t,    SPI_AddressBase+0x00)     ///< SPI control register 1
  #define _SPI_CR2    _SFR(uint8_t,    SPI_AddressBase+0x01)     ///< SPI control register 2
  #define _SPI_ICR    _SFR(uint8_t,    SPI_AddressBase+0x02)     ///< SPI interrupt control register
  #define _SPI_SR     _SFR(uint8_t,    SPI_AddressBase+0x03)     ///< SPI status register
  #define _SPI_DR     _SFR(uint8_t,    SPI_AddressBase+0x04)     ///< SPI data register

  /* SPI Module Reset Values */
  #define _SPI_CR1_RESET_VALUE         ((uint8_t) 0x00)          ///< SPI Control Register 1 reset value
  #define _SPI_CR2_RESET_VALUE         ((uint8_t) 0x00)          ///< SPI Control Register 2 reset value
  #define _SPI_ICR_RESET_VALUE         ((uint8_t) 0x00)          ///< SPI Interrupt Control Register reset value
  #define _SPI_SR_RESET_VALUE          ((uint8_t) 0x02)          ///< SPI Status Register reset value
  #define _SPI_DR_RESET_VALUE          ((uint8_t) 0x00)          ///< SPI Data Register reset value

  /* SPI control register 1 (SPI_CR1) */
  #define _SPI_CPHA             ((uint8_t) (0x01 << 0))          ///< SPI Clock phase [0] (in _SPI_CR1)
  #define _SPI_CPOL             ((uint8_t) (0x01 << 1))          ///< SPI Clock polarity [0] (in _SPI_CR1)
  #define _SPI_MSTR             ((uint8_t) (0x01 << 2))          ///< SPI Master/slave selection [0] (in _SPI_CR1)
  #define _SPI_BR               ((uint8_t) (0x07 << 3))          ///< SPI Baudrate control [2:0] (in _SPI_CR1)
  #define _SPI_BR0              ((uint8_t) (0x01 << 3))          ///< SPI Baudrate control [0] (in _SPI_CR1)
  #define _SPI_BR1              ((uint8_t) (0x01 << 4))          ///< SPI Baudrate control [1] (in _SPI_CR1)
  #define _SPI_BR2              ((uint8_t) (0x01 << 5))          ///< SPI Baudrate control [2] (in _SPI_CR1)
  #define _SPI_SPE              ((uint8_t) (0x01 << 6))          ///< SPI enable [0] (in _SPI_CR1)
  #define _SPI_LSBFIRST         ((uint8_t) (0x01 << 7))          ///< SPI Frame format [0] (in _SPI_CR1)

  /* SPI control register 2 (SPI_CR2) */
  #define _SPI_SSI              ((uint8_t) (0x01 << 0))          ///< SPI Internal slave select [0] (in _SPI_CR2)
  #define _SPI_SSM              ((uint8_t) (0x01 << 1))          ///< SPI Software slave management [0] (in _SPI_CR2)
  #define _SPI_RXONLY           ((uint8_t) (0x01 << 2))          ///< SPI Receive only [0] (in _SPI_CR2)
  // reserved [5:3]
  #define _SPI_BDOE             ((uint8_t) (0x01 << 6))          ///< SPI Input/Output enable in bidirectional mode [0] (in _SPI_CR2)
  #define _SPI_BDM              ((uint8_t) (0x01 << 7))          ///< SPI Bidirectional data mode enable [0] (in _SPI_CR2)

  /* SPI interrupt control register (SPI_ICR) */
  // reserved [3:0]
  #define _SPI_WKIE             ((uint8_t) (0x01 << 4))          ///< SPI Wakeup interrupt enable [0] (in _SPI_ICR)
  #define _SPI_ERRIE            ((uint8_t) (0x01 << 5))          ///< SPI Error interrupt enable [0] (in _SPI_ICR)
  #define _SPI_RXIE             ((uint8_t) (0x01 << 6))          ///< SPI Rx buffer not empty interrupt enable [0] (in _SPI_ICR)
  #define _SPI_TXIE             ((uint8_t) (0x01 << 7))          ///< SPI Tx buffer empty interrupt enable [0] (in _SPI_ICR)

  /* SPI status register (SPI_SR) */
  #define _SPI_RXNE             ((uint8_t) (0x01 << 0))          ///< SPI Receive buffer not empty [0] (in _SPI_SR)
  #define _SPI_TXE              ((uint8_t) (0x01 << 1))          ///< SPI Transmit buffer empty [0] (in _SPI_SR)
  // reserved [2]
  #define _SPI_WKUP             ((uint8_t) (0x01 << 3))          ///< SPI Wakeup flag [0] (in _SPI_SR)
  // reserved [4]
  #define _SPI_MODF             ((uint8_t) (0x01 << 5))          ///< SPI Mode fault [0] (in _SPI_SR)
  #define _SPI_OVR              ((uint8_t) (0x01 << 6))          ///< SPI Overrun flag [0] (in _SPI_SR)
  #define _SPI_BSY              ((uint8_t) (0x01 << 7))          ///< SPI Busy flag [0] (in _SPI_SR)

#endif // SPI_AddressBase



//------------------------
// I2C Bus Interface
//------------------------
#if defined(I2C_AddressBase)

  /** @brief struct for controlling I2C module (I2C) */
  typedef struct {

    /** @brief I2C Control register 1 (I2C_CR1) */
    struct {
      _BITS   PE        : 1;    ///< Peripheral enable
      _BITS             : 5;    //   Reserved
      _BITS   ENGC      : 1;    ///< General call enable
      _BITS   NOSTRETCH : 1;    ///< Clock stretching disable (Slave mode)
    } CR1;


    /** @brief I2C Control register 2 (I2C_CR2) */
    struct {
      _BITS   START     : 1;    ///< Start generation
      _BITS   STOP      : 1;    ///< Stop generation
      _BITS   ACK       : 1;    ///< Acknowledge enable
      _BITS   POS       : 1;    ///< Acknowledge position (for data reception)
      _BITS             : 3;    //   Reserved
      _BITS   SWRST     : 1;    ///< Software reset
    } CR2;


    /** @brief I2C Frequency register (I2C_FREQR) */
    struct {
      _BITS   FREQ      : 6;    ///< Peripheral clock frequency
      _BITS             : 2;    //   Reserved
    } FREQR;


    /** @brief I2C own address register low byte (I2C_OARL) */
    struct {
      _BITS   ADD0      : 1;    ///< Interface address [0] (in 10-bit address mode)
      _BITS   ADD       : 7;    ///< Interface address [7:1]
    } OARL;


    /** @brief I2C own address register high byte (I2C_OARH) */
    struct {
      _BITS             : 1;    //   Reserved
      _BITS   ADD       : 2;    ///< Interface address [9:8] (in 10-bit address mode)
      _BITS             : 3;    //   Reserved
      _BITS   ADDCONF   : 1;    ///< Address mode configuration (must always be written as ‘1’)
      _BITS   ADDMODE   : 1;    ///< 7-/10-bit addressing mode (Slave mode)
    } OARH;


    /** @brief Reserved register (1B) */
    uint8_t res     [1];


    /** @brief I2C data register (I2C_DR) */
    struct {
      _BITS   DATA    : 8;    ///< I2C data
    } DR;


    /** @brief I2C Status register 1 (I2C_SR1) */
    struct {
      _BITS   SB        : 1;    ///< Start bit (Master mode)
      _BITS   ADDR      : 1;    ///< Address sent (Master mode) / matched (Slave mode)
      _BITS   BTF       : 1;    ///< Byte transfer finished
      _BITS   ADD10     : 1;    ///< 10-bit header sent (Master mode)
      _BITS   STOPF     : 1;    ///< Stop detection (Slave mode)
      _BITS             : 1;    //   Reserved
      _BITS   RXNE      : 1;    ///< Data register not empty (receivers)
      _BITS   TXE       : 1;    ///< Data register empty (transmitters)
    } SR1;


    /** @brief I2C Status register 2 (I2C_SR2) */
    struct {
      _BITS   BERR      : 1;    ///< Bus error
      _BITS   ARLO      : 1;    ///< Arbitration lost (Master mode)
      _BITS   AF        : 1;    ///< Acknowledge failure
      _BITS   OVR       : 1;    ///< Overrun/underrun
      _BITS             : 1;    //   Reserved
      _BITS   WUFH      : 1;    ///< Wakeup from Halt
      _BITS             : 2;    //   Reserved
    } SR2;


    /** @brief I2C Status register 3 (I2C_SR3) */
    struct {
      _BITS   MSL       : 1;    ///< Master/Slave
      _BITS   BUSY      : 1;    ///< Bus busy
      _BITS   TRA       : 1;    ///< Transmitter/Receiver
      _BITS             : 1;    //   Reserved
      _BITS   GENCALL   : 1;    ///< General call header (Slave mode)
      _BITS             : 2;    //   Reserved
      _BITS   DUALF     : 1;    ///< Dual flag (Slave mode)
    } SR3;


    /** @brief I2C Interrupt register (I2C_ITR) */
    struct {
      _BITS   ITERREN   : 1;    ///< Error interrupt enable
      _BITS   ITEVTEN   : 1;    ///< Event interrupt enable
      _BITS   ITBUFEN   : 1;    ///< Buffer interrupt enable
      _BITS             : 5;    //   Reserved
    } ITR;


    /** @brief I2C Clock control register low byte (I2C_CCRL) */
    struct {
      _BITS   CCR       : 8;    ///< Clock control register (Master mode)
    } CCRL;


    /** @brief I2C Clock control register high byte (I2C_CCRH) */
    struct {
      _BITS   CCR       : 4;    ///< Clock control register in Fast/Standard mode (Master mode)
      _BITS             : 2;    //   Reserved
      _BITS   DUTY      : 1;    ///< Fast mode duty cycle
      _BITS   FS        : 1;    ///< I2C Master mode selection
    } CCRH;


    /** @brief I2C rise time register (I2C_TRISER) */
    struct {
      _BITS   TRISE     : 6;    ///< Maximum rise time in Fast/Standard mode (Master mode)
      _BITS             : 2;    //   Reserved
    } TRISER;

  } I2C_t;

  /// register for SPI control
  #define _I2C        _SFR(I2C_t,      I2C_AddressBase)          ///< I2C struct/bit access
  #define _I2C_CR1    _SFR(uint8_t,    I2C_AddressBase+0x00)     ///< I2C Control register 1
  #define _I2C_CR2    _SFR(uint8_t,    I2C_AddressBase+0x01)     ///< I2C Control register 2
  #define _I2C_FREQR  _SFR(uint8_t,    I2C_AddressBase+0x02)     ///< I2C Frequency register
  #define _I2C_OARL   _SFR(uint8_t,    I2C_AddressBase+0x03)     ///< I2C own address register low byte
  #define _I2C_OARH   _SFR(uint8_t,    I2C_AddressBase+0x04)     ///< I2C own address register high byte
  // 1B
  #define _I2C_DR     _SFR(uint8_t,    I2C_AddressBase+0x06)     ///< I2C data register
  #define _I2C_SR1    _SFR(uint8_t,    I2C_AddressBase+0x07)     ///< I2C Status register 1
  #define _I2C_SR2    _SFR(uint8_t,    I2C_AddressBase+0x08)     ///< I2C Status register 2
  #define _I2C_SR3    _SFR(uint8_t,    I2C_AddressBase+0x09)     ///< I2C Status register 3
  #define _I2C_ITR    _SFR(uint8_t,    I2C_AddressBase+0x0A)     ///< I2C Interrupt register
  #define _I2C_CCRL   _SFR(uint8_t,    I2C_AddressBase+0x0B)     ///< I2C Clock control register low byte
  #define _I2C_CCRH   _SFR(uint8_t,    I2C_AddressBase+0x0C)     ///< I2C Clock control register high byte
  #define _I2C_TRISER _SFR(uint8_t,    I2C_AddressBase+0x0D)     ///< I2C rise time register
  // 1B

  /* I2C Module Reset Values */
  #define _I2C_CR1_RESET_VALUE         ((uint8_t) 0x00)          ///< I2C Control register 1 reset value
  #define _I2C_CR2_RESET_VALUE         ((uint8_t) 0x00)          ///< I2C Control register 2 reset value
  #define _I2C_FREQR_RESET_VALUE       ((uint8_t) 0x00)          ///< I2C Frequency register reset value
  #define _I2C_OARL_RESET_VALUE        ((uint8_t) 0x00)          ///< I2C own address register low byte reset value
  #define _I2C_OARH_RESET_VALUE        ((uint8_t) 0x00)          ///< I2C own address register high byte reset value
  #define _I2C_DR_RESET_VALUE          ((uint8_t) 0x00)          ///< I2C data register reset value
  #define _I2C_SR1_RESET_VALUE         ((uint8_t) 0x00)          ///< I2C Status register 1 reset value
  #define _I2C_SR2_RESET_VALUE         ((uint8_t) 0x00)          ///< I2C Status register 2 reset value
  #define _I2C_SR3_RESET_VALUE         ((uint8_t) 0x00)          ///< I2C Status register 3 reset value
  #define _I2C_ITR_RESET_VALUE         ((uint8_t) 0x00)          ///< I2C Interrupt register reset value
  #define _I2C_CCRL_RESET_VALUE        ((uint8_t) 0x00)          ///< I2C Clock control register low byte reset value
  #define _I2C_CCRH_RESET_VALUE        ((uint8_t) 0x00)          ///< I2C Clock control register high byte reset value
  #define _I2C_TRISER_RESET_VALUE      ((uint8_t) 0x02)          ///< I2C rise time register reset value

  /* I2C Control register 1 (I2C_CR1) */
  #define _I2C_PE               ((uint8_t) (0x01 << 0))          ///< I2C Peripheral enable [0] (in _I2C_CR1)
  // reserved [5:1]
  #define _I2C_ENGC             ((uint8_t) (0x01 << 6))          ///< I2C General call enable [0] (in _I2C_CR1)
  #define _I2C_NOSTRETCH        ((uint8_t) (0x01 << 7))          ///< I2C Clock stretching disable (Slave mode) [0] (in _I2C_CR1)

  /* I2C Control register 2 (I2C_CR2) */
  #define _I2C_START            ((uint8_t) (0x01 << 0))          ///< I2C Start generation [0] (in _I2C_CR2)
  #define _I2C_STOP             ((uint8_t) (0x01 << 1))          ///< I2C Stop generation [0] (in _I2C_CR2)
  #define _I2C_ACK              ((uint8_t) (0x01 << 2))          ///< I2C Acknowledge enable [0] (in _I2C_CR2)
  #define _I2C_POS              ((uint8_t) (0x01 << 3))          ///< I2C Acknowledge position (for data reception) [0] (in _I2C_CR2)
  // reserved [6:4]
  #define _I2C_SWRST            ((uint8_t) (0x01 << 7))          ///< I2C Software reset [0] (in _I2C_CR2)

  /* I2C Frequency register (I2C_FREQR) */
  #define _I2C_FREQ             ((uint8_t) (0x3F << 0))          ///< I2C Peripheral clock frequency [5:0] (in _I2C_FREQR)
  #define _I2C_FREQ0            ((uint8_t) (0x01 << 0))          ///< I2C Peripheral clock frequency [0] (in _I2C_FREQR)
  #define _I2C_FREQ1            ((uint8_t) (0x01 << 1))          ///< I2C Peripheral clock frequency [1] (in _I2C_FREQR)
  #define _I2C_FREQ2            ((uint8_t) (0x01 << 2))          ///< I2C Peripheral clock frequency [2] (in _I2C_FREQR)
  #define _I2C_FREQ3            ((uint8_t) (0x01 << 3))          ///< I2C Peripheral clock frequency [3] (in _I2C_FREQR)
  #define _I2C_FREQ4            ((uint8_t) (0x01 << 4))          ///< I2C Peripheral clock frequency [4] (in _I2C_FREQR)
  #define _I2C_FREQ5            ((uint8_t) (0x01 << 5))          ///< I2C Peripheral clock frequency [5] (in _I2C_FREQR)
  // reserved [7:6]

  /* I2C own address register low byte (I2C_OARL) */
  #define _I2C_ADD0            ((uint8_t) (0x01 << 0))           ///< I2C Interface address [0] (in 10-bit address mode) (in _I2C_OARL)
  #define _I2C_ADD1            ((uint8_t) (0x01 << 1))           ///< I2C Interface address [1] (in _I2C_OARL)
  #define _I2C_ADD2            ((uint8_t) (0x01 << 2))           ///< I2C Interface address [2] (in _I2C_OARL)
  #define _I2C_ADD3            ((uint8_t) (0x01 << 3))           ///< I2C Interface address [3] (in _I2C_OARL)
  #define _I2C_ADD4            ((uint8_t) (0x01 << 4))           ///< I2C Interface address [4] (in _I2C_OARL)
  #define _I2C_ADD5            ((uint8_t) (0x01 << 5))           ///< I2C Interface address [5] (in _I2C_OARL)
  #define _I2C_ADD6            ((uint8_t) (0x01 << 6))           ///< I2C Interface address [6] (in _I2C_OARL)
  #define _I2C_ADD7            ((uint8_t) (0x01 << 7))           ///< I2C Interface address [7] (in _I2C_OARL)

  /* I2C own address register high byte (I2C_OARH) */
  // reserved [0]
  #define _I2C_ADD_8_9         ((uint8_t) (0x03 << 1))           ///< I2C Interface address [9:8] (in 10-bit address mode) (in _I2C_OARH)
  #define _I2C_ADD8            ((uint8_t) (0x01 << 1))           ///< I2C Interface address [8] (in _I2C_OARH)
  #define _I2C_ADD9            ((uint8_t) (0x01 << 2))           ///< I2C Interface address [9] (in _I2C_OARH)
  // reserved [5:3]
  #define _I2C_ADDCONF         ((uint8_t) (0x01 << 6))           ///< I2C Address mode configuration [0] (in _I2C_OARH)
  #define _I2C_ADDMODE         ((uint8_t) (0x01 << 7))           ///< I2C 7-/10-bit addressing mode (Slave mode) [0] (in _I2C_OARH)

  /* I2C Status register 1 (I2C_SR1) */
  #define _I2C_SB              ((uint8_t) (0x01 << 0))           ///< I2C Start bit (Master mode) [0] (in _I2C_SR1)
  #define _I2C_ADDR            ((uint8_t) (0x01 << 1))           ///< I2C Address sent (Master mode) / matched (Slave mode) [0] (in _I2C_SR1)
  #define _I2C_BTF             ((uint8_t) (0x01 << 2))           ///< I2C Byte transfer finished [0] (in _I2C_SR1)
  #define _I2C_ADD10           ((uint8_t) (0x01 << 3))           ///< I2C 10-bit header sent (Master mode) [0] (in _I2C_SR1)
  #define _I2C_STOPF           ((uint8_t) (0x01 << 4))           ///< I2C Stop detection (Slave mode) [0] (in _I2C_SR1)
  // reserved [5]
  #define _I2C_RXNE            ((uint8_t) (0x01 << 6))           ///< I2C Data register not empty (receivers) [0] (in _I2C_SR1)
  #define _I2C_TXE             ((uint8_t) (0x01 << 7))           ///< I2C Data register empty (transmitters) [0] (in _I2C_SR1)

  /* I2C Status register 2 (I2C_SR2) */
  #define _I2C_BERR            ((uint8_t) (0x01 << 0))           ///< I2C Bus error [0] (in _I2C_SR2)
  #define _I2C_ARLO            ((uint8_t) (0x01 << 1))           ///< I2C Arbitration lost (Master mode) [0] (in _I2C_SR2)
  #define _I2C_AF              ((uint8_t) (0x01 << 2))           ///< I2C Acknowledge failure [0] (in _I2C_SR2)
  #define _I2C_OVR             ((uint8_t) (0x01 << 3))           ///< I2C Overrun/underrun [0] (in _I2C_SR2)
  // reserved [4]
  #define _I2C_WUFH            ((uint8_t) (0x01 << 5))           ///< I2C Wakeup from Halt [0] (in _I2C_SR2)
  // reserved [7:6]

  /* I2C Status register 3 (I2C_SR3) */
  #define _I2C_MSL             ((uint8_t) (0x01 << 0))           ///< I2C Master/Slave [0] (in _I2C_SR3)
  #define _I2C_BUSY            ((uint8_t) (0x01 << 1))           ///< I2C Bus busy [0] (in _I2C_SR3)
  #define _I2C_TRA             ((uint8_t) (0x01 << 2))           ///< I2C Transmitter/Receiver [0] (in _I2C_SR3)
  // reserved [3]
  #define _I2C_GENCALL         ((uint8_t) (0x01 << 4))           ///< I2C General call header (Slave mode) [0] (in _I2C_SR3)
  // reserved [6:5]
  #define _I2C_DUALF           ((uint8_t) (0x01 << 7))           ///< Dual flag (Slave mode) [0] (in _I2C_SR3)

  /* I2C Interrupt register (I2C_ITR) */
  #define _I2C_ITERREN         ((uint8_t) (0x01 << 0))           ///< I2C Error interrupt enable [0] (in _I2C_ITR)
  #define _I2C_ITEVTEN         ((uint8_t) (0x01 << 1))           ///< I2C Event interrupt enable [0] (in _I2C_ITR)
  #define _I2C_ITBUFEN         ((uint8_t) (0x01 << 2))           ///< I2C Buffer interrupt enable [0] (in _I2C_ITR)
  // reserved [7:5]

  /* I2C Clock control register high byte (I2C_CCRH) */
  #define _I2C_CCR             ((uint8_t) (0x0F << 0))           ///< I2C Clock control register (Master mode) [3:0] (in _I2C_CCRH)
  #define _I2C_CCR0            ((uint8_t) (0x01 << 0))           ///< I2C Clock control register (Master mode) [0] (in _I2C_CCRH)
  #define _I2C_CCR1            ((uint8_t) (0x01 << 1))           ///< I2C Clock control register (Master mode) [1] (in _I2C_CCRH)
  #define _I2C_CCR2            ((uint8_t) (0x01 << 2))           ///< I2C Clock control register (Master mode) [2] (in _I2C_CCRH)
  #define _I2C_CCR3            ((uint8_t) (0x01 << 3))           ///< I2C Clock control register (Master mode) [3] (in _I2C_CCRH)
  // reserved [5:4]
  #define _I2C_DUTY            ((uint8_t) (0x01 << 6))           ///< I2C Fast mode duty cycle [0] (in _I2C_CCRH)
  #define _I2C_FS              ((uint8_t) (0x01 << 7))           ///< I2C Master mode selection [0] (in _I2C_CCRH)

  /* I2C rise time register (I2C_TRISER) */
  #define _I2C_TRISE           ((uint8_t) (0x3F << 0))           ///< I2C Maximum rise time (Master mode) [5:0] (in _I2C_TRISER)
  #define _I2C_TRISE0          ((uint8_t) (0x01 << 0))           ///< I2C Maximum rise time (Master mode) [0] (in _I2C_TRISER)
  #define _I2C_TRISE1          ((uint8_t) (0x01 << 1))           ///< I2C Maximum rise time (Master mode) [1] (in _I2C_TRISER)
  #define _I2C_TRISE2          ((uint8_t) (0x01 << 2))           ///< I2C Maximum rise time (Master mode) [2] (in _I2C_TRISER)
  #define _I2C_TRISE3          ((uint8_t) (0x01 << 3))           ///< I2C Maximum rise time (Master mode) [3] (in _I2C_TRISER)
  #define _I2C_TRISE4          ((uint8_t) (0x01 << 4))           ///< I2C Maximum rise time (Master mode) [4] (in _I2C_TRISER)
  #define _I2C_TRISE5          ((uint8_t) (0x01 << 5))           ///< I2C Maximum rise time (Master mode) [5] (in _I2C_TRISER)
  // reserved [7:6]

#endif // I2C_AddressBase



//------------------------
// Universal Asynchronous Receiver Transmitter (USART)
//------------------------
#if defined(USART_AddressBase)

  /** @brief struct for controlling Universal Asynchronous Receiver Transmitter (USART) */
  typedef struct {

    /** @brief USART Status register (USART_SR) */
    struct {
      _BITS   PE      : 1;    ///< Parity error
      _BITS   FE      : 1;    ///< Framing error
      _BITS   NF      : 1;    ///< Noise flag
      _BITS   OR      : 1;    ///< LIN Header Error (LIN Slave mode) / Overrun error
      _BITS   IDLE    : 1;    ///< IDLE line detected
      _BITS   RXNE    : 1;    ///< Read data register not empty
      _BITS   TC      : 1;    ///< Transmission complete
      _BITS   TXE     : 1;    ///< Transmit data register empty
    } SR;


    /** @brief USART data register (USART_DR) */
    struct {
      _BITS   DATA    : 8;    ///< USART data
    } DR;


    /** @brief USART Baud rate register 1 (USART_BRR1) */
    struct {
      _BITS   DIV_4_11 : 8;    ///< UART_DIV bits [11:4]
    } BRR1;


    /** @brief USART Baud rate register 2 (USART_BRR2) */
    struct {
      _BITS   DIV_0_3   : 4;    ///< UART_DIV bits [3:0]
      _BITS   DIV_12_15 : 4;    ///< UART_DIV bits [15:12]
    } BRR2;


    /** @brief USART Control register 1 (USART_CR1) */
    struct {
      _BITS   PIEN    : 1;    ///< Parity interrupt enable
      _BITS   PS      : 1;    ///< Parity selection
      _BITS   PCEN    : 1;    ///< Parity control enable
      _BITS   WAKE    : 1;    ///< Wakeup method
      _BITS   M       : 1;    ///< word length
      _BITS   UARTD   : 1;    ///< UART Disable (for low power consumption)
      _BITS   T8      : 1;    ///< Transmit Data bit 8 (in 9-bit mode)
      _BITS   R8      : 1;    ///< Receive Data bit 8 (in 9-bit mode)
    } CR1;


    /** @brief USART Control register 2 (USART_CR2) */
    struct {
      _BITS   SBK     : 1;    ///< Send break
      _BITS   RWU     : 1;    ///< Receiver wakeup
      _BITS   REN     : 1;    ///< Receiver enable
      _BITS   TEN     : 1;    ///< Transmitter enable
      _BITS   ILIEN   : 1;    ///< IDLE Line interrupt enable
      _BITS   RIEN    : 1;    ///< Receiver interrupt enable
      _BITS   TCIEN   : 1;    ///< Transmission complete interrupt enable
      _BITS   TIEN    : 1;    ///< Transmitter interrupt enable
    } CR2;


    /** @brief USART Control register 3 (USART_CR3) */
    struct {
      _BITS   LBCL    : 1;    ///< Last bit clock pulse
      _BITS   CPHA    : 1;    ///< Clock phase
      _BITS   CPOL    : 1;    ///< Clock polarity
      _BITS   CKEN    : 1;    ///< Clock enable
      _BITS   STOP    : 2;    ///< STOP bits
      _BITS           : 2;    //   Reserved
    } CR3;


    /** @brief USART Control register 4 (USART_CR4) */
    struct {
      _BITS   ADD     : 4;    ///< Address of the UART node
      _BITS           : 4;    //   Reserved
    } CR4;

  } USART_t;

  /* Pointer to USART registers */
  #define _USART      _SFR(USART_t,    USART_AddressBase)        ///< USART struct/bit access
  #define _USART_SR   _SFR(uint8_t,    USART_AddressBase+0x00)   ///< USART Status register
  #define _USART_DR   _SFR(uint8_t,    USART_AddressBase+0x01)   ///< USART data register
  #define _USART_BRR1 _SFR(uint8_t,    USART_AddressBase+0x02)   ///< USART Baud rate register 1
  #define _USART_BRR2 _SFR(uint8_t,    USART_AddressBase+0x03)   ///< USART Baud rate register 2
  #define _USART_CR1  _SFR(uint8_t,    USART_AddressBase+0x04)   ///< USART Control register 1
  #define _USART_CR2  _SFR(uint8_t,    USART_AddressBase+0x05)   ///< USART Control register 2
  #define _USART_CR3  _SFR(uint8_t,    USART_AddressBase+0x06)   ///< USART Control register 3
  #define _USART_CR4  _SFR(uint8_t,    USART_AddressBase+0x07)   ///< USART Control register 4

  /* USART Module Reset Values */
  #define _USART_SR_RESET_VALUE        ((uint8_t) 0xC0)          ///< USART Status register reset value
  #define _USART_BRR1_RESET_VALUE      ((uint8_t) 0x00)          ///< USART Baud rate register 1 reset value
  #define _USART_BRR2_RESET_VALUE      ((uint8_t) 0x00)          ///< USART Baud rate register 2 reset value
  #define _USART_CR1_RESET_VALUE       ((uint8_t) 0x00)          ///< USART Control register 1 reset value
  #define _USART_CR2_RESET_VALUE       ((uint8_t) 0x00)          ///< USART Control register 2 reset value
  #define _USART_CR3_RESET_VALUE       ((uint8_t) 0x00)          ///< USART Control register 3 reset value
  #define _USART_CR4_RESET_VALUE       ((uint8_t) 0x00)          ///< USART Control register 4 reset value

  /* USART Status register (USART_SR) bits */
  #define _USART_PE               ((uint8_t) (0x01 << 0))        ///< USART Parity error [0] (in _USART_SR)
  #define _USART_FE               ((uint8_t) (0x01 << 1))        ///< USART Framing error [0] (in _USART_SR)
  #define _USART_NF               ((uint8_t) (0x01 << 2))        ///< USART Noise flag [0] (in _USART_SR)
  #define _USART_OR_LHE           ((uint8_t) (0x01 << 3))        ///< USART LIN Header Error (LIN Slave mode) / Overrun error [0] (in _USART_SR)
  #define _USART_IDLE             ((uint8_t) (0x01 << 4))        ///< USART IDLE line detected [0] (in _USART_SR)
  #define _USART_RXNE             ((uint8_t) (0x01 << 5))        ///< USART Read data register not empty [0] (in _USART_SR)
  #define _USART_TC               ((uint8_t) (0x01 << 6))        ///< USART Transmission complete [0] (in _USART_SR)
  #define _USART_TXE              ((uint8_t) (0x01 << 7))        ///< USART Transmit data register empty [0] (in _USART_SR)

  /* USART Control register 1 (USART_CR1) bits */
  #define _USART_PIEN             ((uint8_t) (0x01 << 0))        ///< USART Parity interrupt enable [0] (in _USART_CR1)
  #define _USART_PS               ((uint8_t) (0x01 << 1))        ///< USART Parity selection [0] (in _USART_CR1)
  #define _USART_PCEN             ((uint8_t) (0x01 << 2))        ///< USART Parity control enable [0] (in _USART_CR1)
  #define _USART_WAKE             ((uint8_t) (0x01 << 3))        ///< USART Wakeup method [0] (in _USART_CR1)
  #define _USART_M                ((uint8_t) (0x01 << 4))        ///< USART word length [0] (in _USART_CR1)
  #define _USART_UARTD            ((uint8_t) (0x01 << 5))        ///< USART Disable (for low power consumption) [0] (in _USART_CR1)
  #define _USART_T8               ((uint8_t) (0x01 << 6))        ///< USART Transmit Data bit 8 (in 9-bit mode) [0] (in _USART_CR1)
  #define _USART_R8               ((uint8_t) (0x01 << 7))        ///< USART Receive Data bit 8 (in 9-bit mode) [0] (in _USART_CR1)

  /* USART Control register 2 (USART_CR2) bits */
  #define _USART_SBK              ((uint8_t) (0x01 << 0))        ///< USART Send break [0] (in _USART_CR2)
  #define _USART_RWU              ((uint8_t) (0x01 << 1))        ///< USART Receiver wakeup [0] (in _USART_CR2)
  #define _USART_REN              ((uint8_t) (0x01 << 2))        ///< USART Receiver enable [0] (in _USART_CR2)
  #define _USART_TEN              ((uint8_t) (0x01 << 3))        ///< USART Transmitter enable [0] (in _USART_CR2)
  #define _USART_ILIEN            ((uint8_t) (0x01 << 4))        ///< USART IDLE Line interrupt enable [0] (in _USART_CR2)
  #define _USART_RIEN             ((uint8_t) (0x01 << 5))        ///< USART Receiver interrupt enable [0] (in _USART_CR2)
  #define _USART_TCIEN            ((uint8_t) (0x01 << 6))        ///< USART Transmission complete interrupt enable [0] (in _USART_CR2)
  #define _USART_TIEN             ((uint8_t) (0x01 << 7))        ///< USART Transmitter interrupt enable [0] (in _USART_CR2)

  /* USART Control register 3 (USART_CR3) bits */
  #define _USART_LBCL             ((uint8_t) (0x01 << 0))        ///< USART Last bit clock pulse [0] (in _USART_CR3)
  #define _USART_CPHA             ((uint8_t) (0x01 << 1))        ///< USART Clock phase [0] (in _USART_CR3)
  #define _USART_CPOL             ((uint8_t) (0x01 << 2))        ///< USART Clock polarity [0] (in _USART_CR3)
  #define _USART_CKEN             ((uint8_t) (0x01 << 3))        ///< USART Clock enable [0] (in _USART_CR3)
  #define _USART_STOP             ((uint8_t) (0x03 << 4))        ///< USART STOP bits [1:0] (in _USART_CR3)
  #define _USART_STOP0            ((uint8_t) (0x01 << 4))        ///< USART STOP bits [0] (in _USART_CR3)
  #define _USART_STOP1            ((uint8_t) (0x01 << 5))        ///< USART STOP bits [1] (in _USART_CR3)
  // reserved [7:6]

  /* USART Control register 4 (USART_CR4) bits */
  #define _USART_ADD              ((uint8_t) (0x0F << 0))        ///< USART Address of the UART node [3:0] (in _USART_CR4)
  #define _USART_ADD0             ((uint8_t) (0x01 << 0))        ///< USART Address of the UART node [0] (in _USART_CR4)
  #define _USART_ADD1             ((uint8_t) (0x01 << 1))        ///< USART Address of the UART node [1] (in _USART_CR4)
  #define _USART_ADD2             ((uint8_t) (0x01 << 2))        ///< USART Address of the UART node [2] (in _USART_CR4)
  #define _USART_ADD3             ((uint8_t) (0x01 << 3))        ///< USART Address of the UART node [3] (in _USART_CR4)
  // reserved [7:4]

#endif // USART_AddressBase



//------------------------
// Wait for Event Configuration (WFE)
//------------------------
#if defined(WFE_AddressBase)

  /** @brief struct to configure interrupt sources as external interrupts or wake events (WFE) */
  typedef struct {

    /** @brief WFE control register 1 (WFE_CR1) */
    struct {
      _BITS   TIM2_EV0 : 1;    ///< TIM2 update, trigger or break event
      _BITS   TIM2_EV1 : 1;    ///< TIM2 capture or compare event
      _BITS            : 2;    //   Reserved
      _BITS   EXTI_EV0 : 1;    ///< Interrupt on pin 0 of all ports event
      _BITS   EXTI_EV1 : 1;    ///< Interrupt on pin 1 of all ports event
      _BITS   EXTI_EV2 : 1;    ///< Interrupt on pin 2 of all ports event
      _BITS   EXTI_EV3 : 1;    ///< Interrupt on pin 3 of all ports event
    } CR1;


    /** @brief WFE control register 2 (WFE_CR2) */
    struct {
      _BITS   EXTI_EV4 : 1;    ///< Interrupt on pin 4 of all ports event
      _BITS   EXTI_EV5 : 1;    ///< Interrupt on pin 5 of all ports event
      _BITS   EXTI_EV6 : 1;    ///< Interrupt on pin 6 of all ports event
      _BITS   EXTI_EV7 : 1;    ///< Interrupt on pin 7 of all ports event
      _BITS   EXTI_EVB : 1;    ///< Interrupt on port B event
      _BITS   EXTI_EVD : 1;    ///< Interrupt on port D event
      _BITS            : 2;    //   Reserved
    } CR2;

  } WFE_t;

  /* Pointer to WFE registers */
  #define _WFE        _SFR(WFE_t,      WFE_AddressBase)          ///< WFE struct/bit access
  #define _WFE_CR1    _SFR(uint8_t,    WFE_AddressBase+0x00)     ///< WFE Control register 1
  #define _WFE_CR2    _SFR(uint8_t,    WFE_AddressBase+0x01)     ///< WFE Control register 2

  /* WFE Module Reset Values */
  #define _WFE_CR1_RESET_VALUE         ((uint8_t) 0x03)          ///< WFE Control register 1 reset value
  #define _WFE_CR2_RESET_VALUE         ((uint8_t) 0x00)          ///< WFE Control register 2 reset value

  /* WFE Control register 1 (WFE_CR1) bits */
  #define _WFE_TIM2_EV0           ((uint8_t) (0x01 << 0))        ///< TIM2 update, trigger or break event [0] (in _WFE_CR1)
  #define _WFE_TIM2_EV1           ((uint8_t) (0x01 << 1))        ///< TIM2 capture or compare event [0] (in _WFE_CR1)
  // reserved [3:2]
  #define _WFE_EXTI_EV0           ((uint8_t) (0x01 << 4))        ///< Interrupt on pin 0 of all ports event [0] (in _WFE_CR1)
  #define _WFE_EXTI_EV1           ((uint8_t) (0x01 << 5))        ///< Interrupt on pin 1 of all ports event [0] (in _WFE_CR1)
  #define _WFE_EXTI_EV2           ((uint8_t) (0x01 << 6))        ///< Interrupt on pin 2 of all ports event [0] (in _WFE_CR1)
  #define _WFE_EXTI_EV3           ((uint8_t) (0x01 << 7))        ///< Interrupt on pin 3 of all ports event [0] (in _WFE_CR1)

  /* WFE Control register 2 (WFE_CR2) bits */
  #define _WFE_EXTI_EV4           ((uint8_t) (0x01 << 0))        ///< Interrupt on pin 4 of all ports event [0] (in _WFE_CR1)
  #define _WFE_EXTI_EV5           ((uint8_t) (0x01 << 1))        ///< Interrupt on pin 5 of all ports event [0] (in _WFE_CR1)
  #define _WFE_EXTI_EV6           ((uint8_t) (0x01 << 2))        ///< Interrupt on pin 6 of all ports event [0] (in _WFE_CR1)
  #define _WFE_EXTI_EV7           ((uint8_t) (0x01 << 3))        ///< Interrupt on pin 7 of all ports event [0] (in _WFE_CR1)
  #define _WFE_EXTI_EVB           ((uint8_t) (0x01 << 4))        ///< Interrupt on port B event [0] (in _WFE_CR1)
  #define _WFE_EXTI_EVD           ((uint8_t) (0x01 << 5))        ///< Interrupt on port D event [0] (in _WFE_CR1)
  // reserved [7:6]

#endif // WFE_AddressBase



//------------------------
// 16-Bit Timer 2+3 (TIM2, TIM3)
//------------------------
#if defined(TIM2_AddressBase) || defined(TIM3_AddressBase)

  /** @brief struct for controlling 16-Bit Timer 2+3 (TIM2, TIM3) */
  typedef struct {

    /** @brief TIM2/3 Control register 1 (TIM2/3_CR1) */
    struct {
      _BITS   CEN     : 1;    ///< Counter enable
      _BITS   UDIS    : 1;    ///< Update disable
      _BITS   URS     : 1;    ///< Update request source
      _BITS   OPM     : 1;    ///< One-pulse mode
      _BITS   DIR     : 1;    ///< Direction
      _BITS           : 2;    //   Reserved
      _BITS   ARPE    : 1;    ///< Auto-reload preload enable
    } CR1;


    /** @brief TIM2/3 Control register 2 (TIM2/3_CR2) */
    struct {
      _BITS           : 4;    //   Reserved
      _BITS   MMS     : 3;    ///< Master mode selection
      _BITS           : 1;    //   Reserved
    } CR2;


    /** @brief TIM2/3 Slave mode control register (TIM2/3_SMCR) */
    struct {
      _BITS   SMS     : 3;    ///< Clock/trigger/slave mode selection
      _BITS           : 1;    //   Reserved
      _BITS   TS      : 3;    ///< Trigger selection
      _BITS   MSM     : 1;    ///< Master/slave mode
    } SMCR;


    /** @brief TIM2/3 External trigger register (TIM2/3_ETR) */
    struct {
      _BITS   ETF     : 4;    ///< External trigger filter
      _BITS   ETPS    : 2;    ///< External trigger prescaler
      _BITS   ECE     : 1;    ///< External clock enable
      _BITS   ETP     : 1;    ///< External trigger polarity
    } ETR;


    /** @brief TIM2/3 Interrupt enable register (TIM2/3_IER) */
    struct {
      _BITS   UIE     : 1;    ///< Update interrupt enable
      _BITS   CC1IE   : 1;    ///< Capture/compare 1 interrupt enable
      _BITS   CC2IE   : 1;    ///< Capture/compare 2 interrupt enable
      _BITS           : 3;    //   Reserved
      _BITS   TIE     : 1;    ///< Trigger interrupt enable
      _BITS   BIE     : 1;    ///< Break interrupt enable
    } IER;


    /** @brief TIM2/3 Status register 1 (TIM2/3_SR1) */
    struct {
      _BITS   UIF     : 1;    ///< Update interrupt flag
      _BITS   CC1IF   : 1;    ///< Capture/compare 1 interrupt flag
      _BITS   CC2IF   : 1;    ///< Capture/compare 2 interrupt flag
      _BITS           : 3;    //   Reserved
      _BITS   TIF     : 1;    ///< Trigger interrupt flag
      _BITS   BIF     : 1;    ///< Break interrupt flag
    } SR1;


    /** @brief TIM2/3 Status register 2 (TIM2/3_SR2) */
    struct {
      _BITS           : 1;    //   Reserved, must be kept cleared
      _BITS   CC1OF   : 1;    ///< Capture/compare 1 overcapture flag
      _BITS   CC2OF   : 1;    ///< Capture/compare 2 overcapture flag
      _BITS           : 5;    //   Reserved
    } SR2;


    /** @brief TIM2/3 Event generation register (TIM2/3_EGR) */
    struct {
      _BITS   UG      : 1;    ///< Update generation
      _BITS   CC1G    : 1;    ///< Capture/compare 1 generation
      _BITS   CC2G    : 1;    ///< Capture/compare 2 generation
      _BITS           : 3;    //   Reserved
      _BITS   TG      : 1;    ///< Trigger generation
      _BITS   BG      : 1;    ///< Break generation
    } EGR;


    /** @brief TIM2/3 Capture/compare mode register 1 (TIM2/3_CCMR1) */
    union {

      /** @brief bitwise access to register (output mode) */
      struct {
        _BITS   CC1S    : 2;    ///< Compare 1 selection
        _BITS   OC1FE   : 1;    ///< Output compare 1 fast enable
        _BITS   OC1PE   : 1;    ///< Output compare 1 preload enable
        _BITS   OC1M    : 3;    ///< Output compare 1 mode
        _BITS           : 1;    //   Reserved
      } OUT;

      /** @brief bitwise access to register (input mode) */
      struct {
        _BITS   CC1S    : 2;    ///< Capture 1 selection
        _BITS   IC1PSC  : 2;    ///< Input capture 1 prescaler
        _BITS   IC1F    : 4;    ///< Input capture 1 filter
      } IN;

    } CCMR1;


    /** @brief TIM2/3 Capture/compare mode register 2 (TIM2/3_CCMR2) */
    union {

      /** @brief bitwise access to register (output mode) */
      struct {
        _BITS   CC2S    : 2;    ///< Compare 2 selection
        _BITS   OC2FE   : 1;    ///< Output compare 2 fast enable
        _BITS   OC2PE   : 1;    ///< Output compare 2 preload enable
        _BITS   OC2M    : 3;    ///< Output compare 2 mode
        _BITS           : 1;    //   Reserved
      } OUT;

      /** @brief bitwise access to register (input mode) */
      struct {
        _BITS   CC2S    : 2;    ///< Capture/compare 2 selection
        _BITS   IC2PSC  : 2;    ///< Input capture 2 prescaler
        _BITS   IC2F    : 4;    ///< Input capture 2 filter
      } IN;

    } CCMR2;


    /** @brief TIM2/3 Capture/compare enable register 1 (TIM2/3_CCER1) */
    struct {
      _BITS   CC1E    : 1;    ///< Capture/compare 1 output enable
      _BITS   CC1P    : 1;    ///< Capture/compare 1 output polarity
      _BITS           : 2;    //   Reserved
      _BITS   CC2E    : 1;    ///< Capture/compare 2 output enable
      _BITS   CC2P    : 1;    ///< Capture/compare 2 output polarity
      _BITS           : 2;    //   Reserved
    } CCER1;


    /** @brief TIM2/3 16-bit counter high byte (TIM2/3_CNTRH) */
    struct {
      _BITS   CNT     : 8;    ///< 16-bit counter [15:8]
    } CNTRH;


    /** @brief TIM2/3 16-bit counter low byte (TIM2/3_CNTRL) */
    struct {
      _BITS   CNT     : 8;    ///< 16-bit counter [7:0]
    } CNTRL;


    /** @brief TIM2/3 16-bit prescaler (TIM2/3_PSCR) */
    struct {
      _BITS   PSC     : 3;    ///< prescaler [2:0]
      _BITS           : 5;    //   Reserved
    } PSCR;


    /** @brief TIM2/3 16-bit auto-reload value high byte (TIM2/3_ARRH) */
    struct {
      _BITS   ARR     : 8;    ///< 16-bit auto-reload value [15:8]
    } ARRH;


    /** @brief TIM2/3 16-bit auto-reload value low byte (TIM2/3_ARRL) */
    struct {
      _BITS   ARR     : 8;    ///< 16-bit auto-reload value [7:0]
    } ARRL;


    /** @brief TIM2/3 16-bit capture/compare value 1 high byte (TIM2/3_CCR1H) */
    struct {
      _BITS   CCR1    : 8;    ///< 16-bit capture/compare value 1 [15:8]
    } CCR1H;


    /** @brief TIM2/3 16-bit capture/compare value 1 low byte (TIM2/3_CCR1L) */
    struct {
      _BITS   CCR1    : 8;    ///< 16-bit capture/compare value 1 [7:0]
    } CCR1L;


    /** @brief TIM2/3 16-bit capture/compare value 2 high byte (TIM2/3_CCR2H) */
    struct {
      _BITS   CCR2    : 8;  ///< 16-bit capture/compare value 2 [15:8]
    } CCR2H;


    /** @brief TIM2/3 16-bit capture/compare value 2 low byte (TIM2/3_CCR2L) */
    struct {
      _BITS   CCR2    : 8;    ///< 16-bit capture/compare value 2 [7:0]
    } CCR2L;


    /** @brief TIM2/3 Break register (TIM2/3_BKR) */
    struct {
      _BITS   LOCK    : 2;    ///< Lock configuration
      _BITS   OSSI    : 1;    ///< Off state selection for idle mode
      _BITS           : 1;    ///< Reserved
      _BITS   BKE     : 1;    ///< Break enable
      _BITS   BKP     : 1;    ///< Break polarity
      _BITS   AOE     : 1;    ///< Automatic output enable
      _BITS   MOE     : 1;    ///< Main output enable
    } BKR;


    /** @brief TIM2/3 Output idle state register (TIM2/3_OISR) */
    struct {
      _BITS   OIS1    : 1;    ///< Output idle state 1 (OC1 output)
      _BITS           : 1;    ///< Reserved
      _BITS   OIS2    : 1;    ///< Output idle state 2 (OC2 output)
      _BITS           : 5;    //   Reserved, forced by hardware to 0
    } OISR;

  } TIM2_3_t;

  /* Pointer to TIM2 registers */
  #if defined(TIM2_AddressBase)
    #define _TIM2        _SFR(TIM2_3_t,  TIM2_AddressBase)         ///< TIM2 struct/bit access
    #define _TIM2_CR1    _SFR(uint8_t,   TIM2_AddressBase+0x00)    ///< TIM2 control register 1
    #define _TIM2_CR2    _SFR(uint8_t,   TIM2_AddressBase+0x01)    ///< TIM2 control register 2
    #define _TIM2_SMCR   _SFR(uint8_t,   TIM2_AddressBase+0x02)    ///< TIM2 Slave mode control register
    #define _TIM2_ETR    _SFR(uint8_t,   TIM2_AddressBase+0x03)    ///< TIM2 External trigger register
    #define _TIM2_IER    _SFR(uint8_t,   TIM2_AddressBase+0x04)    ///< TIM2 interrupt enable register
    #define _TIM2_SR1    _SFR(uint8_t,   TIM2_AddressBase+0x05)    ///< TIM2 status register 1
    #define _TIM2_SR2    _SFR(uint8_t,   TIM2_AddressBase+0x06)    ///< TIM2 status register 2
    #define _TIM2_EGR    _SFR(uint8_t,   TIM2_AddressBase+0x07)    ///< TIM2 Event generation register
    #define _TIM2_CCMR1  _SFR(uint8_t,   TIM2_AddressBase+0x08)    ///< TIM2 Capture/compare mode register 1
    #define _TIM2_CCMR2  _SFR(uint8_t,   TIM2_AddressBase+0x09)    ///< TIM2 Capture/compare mode register 2
    #define _TIM2_CCER1  _SFR(uint8_t,   TIM2_AddressBase+0x0A)    ///< TIM2 Capture/compare enable register 1
    #define _TIM2_CNTRH  _SFR(uint8_t,   TIM2_AddressBase+0x0B)    ///< TIM2 counter register high byte
    #define _TIM2_CNTRL  _SFR(uint8_t,   TIM2_AddressBase+0x0C)    ///< TIM2 counter register low byte
    #define _TIM2_PSCR   _SFR(uint8_t,   TIM2_AddressBase+0x0D)    ///< TIM2 clock prescaler register
    #define _TIM2_ARRH   _SFR(uint8_t,   TIM2_AddressBase+0x0E)    ///< TIM2 auto-reload register high byte
    #define _TIM2_ARRL   _SFR(uint8_t,   TIM2_AddressBase+0x0F)    ///< TIM2 auto-reload register low byte
    #define _TIM2_CCR1H  _SFR(uint8_t,   TIM2_AddressBase+0x10)    ///< TIM2 16-bit capture/compare value 1 high byte
    #define _TIM2_CCR1L  _SFR(uint8_t,   TIM2_AddressBase+0x11)    ///< TIM2 16-bit capture/compare value 1 low byte
    #define _TIM2_CCR2H  _SFR(uint8_t,   TIM2_AddressBase+0x12)    ///< TIM2 16-bit capture/compare value 2 high byte
    #define _TIM2_CCR2L  _SFR(uint8_t,   TIM2_AddressBase+0x13)    ///< TIM2 16-bit capture/compare value 2 low byte
    #define _TIM2_BKR    _SFR(uint8_t,   TIM2_AddressBase+0x14)    ///< TIM2 Break register
    #define _TIM2_OISR   _SFR(uint8_t,   TIM2_AddressBase+0x15)    ///< TIM2 Output idle state register

    /* TIM2 Module Reset Values */
    #define _TIM2_CR1_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM2 control register 1 reset value
    #define _TIM2_CR2_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM2 control register 2 reset value
    #define _TIM2_SMCR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM2 Slave mode control register reset value
    #define _TIM2_ETR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM2 External trigger register reset value
    #define _TIM2_IER_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM2 interrupt enable register reset value
    #define _TIM2_SR1_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM2 status register 1 reset value
    #define _TIM2_SR2_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM2 status register 2 reset value
    #define _TIM2_EGR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM2 Event generation register reset value
    #define _TIM2_CCMR1_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 Capture/compare mode register 1 reset value
    #define _TIM2_CCMR2_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 Capture/compare mode register 2 reset value
    #define _TIM2_CCER1_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 Capture/compare enable register 1 reset value
    #define _TIM2_CNTRH_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 counter register high byte reset value
    #define _TIM2_CNTRL_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 counter register low byte reset value
    #define _TIM2_PSCR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM2 clock prescaler register reset value
    #define _TIM2_ARRH_RESET_VALUE       ((uint8_t) 0xFF)          ///< TIM2 auto-reload register high byte reset value
    #define _TIM2_ARRL_RESET_VALUE       ((uint8_t) 0xFF)          ///< TIM2 auto-reload register low byte reset value
    #define _TIM2_RCR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM2 Repetition counter reset value
    #define _TIM2_CCR1H_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 16-bit capture/compare value 1 high byte reset value
    #define _TIM2_CCR1L_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 16-bit capture/compare value 1 low byte reset value
    #define _TIM2_CCR2H_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 16-bit capture/compare value 2 high byte reset value
    #define _TIM2_CCR2L_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 16-bit capture/compare value 2 low byte reset value
    #define _TIM2_BKR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM2 Break register reset value
    #define _TIM2_OISR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM2 Output idle state register reset value

    /* TIM2 Control register (TIM2_CR1) */
    #define _TIM2_CEN               ((uint8_t) (0x01 << 0))        ///< TIM2 Counter enable [0] (in _TIM2_CR1)
    #define _TIM2_UDIS              ((uint8_t) (0x01 << 1))        ///< TIM2 Update disable [0] (in _TIM2_CR1)
    #define _TIM2_URS               ((uint8_t) (0x01 << 2))        ///< TIM2 Update request source [0] (in _TIM2_CR1)
    #define _TIM2_OPM               ((uint8_t) (0x01 << 3))        ///< TIM2 One-pulse mode [0] (in _TIM2_CR1)
    #define _TIM2_DIR               ((uint8_t) (0x01 << 4))        ///< TIM2 Direction [0] (in _TIM2_CR1)
    // reserved [6:5]
    #define _TIM2_ARPE              ((uint8_t) (0x01 << 7))        ///< TIM2 Auto-reload preload enable [0] (in _TIM2_CR1)

    /* TIM2 Control register (TIM2_CR2) */
    // reserved [3:0]
    #define _TIM2_MMS               ((uint8_t) (0x07 << 4))        ///< TIM2 Master mode selection [2:0] (in _TIM2_CR2)
    #define _TIM2_MMS0              ((uint8_t) (0x01 << 4))        ///< TIM2 Master mode selection [0] (in _TIM2_CR2)
    #define _TIM2_MMS1              ((uint8_t) (0x01 << 5))        ///< TIM2 Master mode selection [1] (in _TIM2_CR2)
    #define _TIM2_MMS2              ((uint8_t) (0x01 << 6))        ///< TIM2 Master mode selection [2] (in _TIM2_CR2)
    // reserved [7]

    /* TIM2 Slave mode control register (TIM2_SMCR) */
    #define _TIM2_SMS               ((uint8_t) (0x07 << 0))        ///< TIM2 Clock/trigger/slave mode selection [2:0] (in _TIM2_SMCR)
    #define _TIM2_SMS0              ((uint8_t) (0x01 << 0))        ///< TIM2 Clock/trigger/slave mode selection [0] (in _TIM2_SMCR)
    #define _TIM2_SMS1              ((uint8_t) (0x01 << 1))        ///< TIM2 Clock/trigger/slave mode selection [1] (in _TIM2_SMCR)
    #define _TIM2_SMS2              ((uint8_t) (0x01 << 2))        ///< TIM2 Clock/trigger/slave mode selection [2] (in _TIM2_SMCR)
    // reserved [3]
    #define _TIM2_TS                ((uint8_t) (0x07 << 4))        ///< TIM2 Trigger selection [2:0] (in _TIM2_SMCR)
    #define _TIM2_TS0               ((uint8_t) (0x01 << 4))        ///< TIM2 Trigger selection [0] (in _TIM2_SMCR)
    #define _TIM2_TS1               ((uint8_t) (0x01 << 5))        ///< TIM2 Trigger selection [1] (in _TIM2_SMCR)
    #define _TIM2_TS2               ((uint8_t) (0x01 << 6))        ///< TIM2 Trigger selection [2] (in _TIM2_SMCR)
    #define _TIM2_MSM               ((uint8_t) (0x01 << 7))        ///< TIM2 Master/slave mode [0] (in _TIM2_SMCR)

    /* TIM2 External trigger register (TIM2_ETR) */
    #define _TIM2_ETF               ((uint8_t) (0x0F << 0))        ///< TIM2 External trigger filter [3:0] (in _TIM2_ETR)
    #define _TIM2_ETF0              ((uint8_t) (0x01 << 0))        ///< TIM2 External trigger filter [0] (in _TIM2_ETR)
    #define _TIM2_ETF1              ((uint8_t) (0x01 << 1))        ///< TIM2 External trigger filter [1] (in _TIM2_ETR)
    #define _TIM2_ETF2              ((uint8_t) (0x01 << 2))        ///< TIM2 External trigger filter [2] (in _TIM2_ETR)
    #define _TIM2_ETF3              ((uint8_t) (0x01 << 3))        ///< TIM2 External trigger filter [3] (in _TIM2_ETR)
    #define _TIM2_ETPS              ((uint8_t) (0x03 << 4))        ///< TIM2 External trigger prescaler [1:0] (in _TIM2_ETR)
    #define _TIM2_ETPS0             ((uint8_t) (0x01 << 4))        ///< TIM2 External trigger prescaler [0] (in _TIM2_ETR)
    #define _TIM2_ETPS1             ((uint8_t) (0x01 << 5))        ///< TIM2 External trigger prescaler [1] (in _TIM2_ETR)
    #define _TIM2_ECE               ((uint8_t) (0x01 << 6))        ///< TIM2 External clock enable [0] (in _TIM2_ETR)
    #define _TIM2_ETP               ((uint8_t) (0x01 << 7))        ///< TIM2 External trigger polarity [0] (in _TIM2_ETR)

    /* TIM2 Interrupt enable (TIM2_IER) */
    #define _TIM2_UIE               ((uint8_t) (0x01 << 0))        ///< TIM2 Update interrupt enable [0] (in _TIM2_IER)
    #define _TIM2_CC1IE             ((uint8_t) (0x01 << 1))        ///< TIM2 Capture/compare 1 interrupt enable [0] (in _TIM2_IER)
    #define _TIM2_CC2IE             ((uint8_t) (0x01 << 2))        ///< TIM2 Capture/compare 2 interrupt enable [0] (in _TIM2_IER)
    // reserved [5:3]
    #define _TIM2_TIE               ((uint8_t) (0x01 << 6))        ///< TIM2 Trigger interrupt enable [0] (in _TIM2_IER)
    #define _TIM2_BIE               ((uint8_t) (0x01 << 7))        ///< TIM2 Break interrupt enable [0] (in _TIM2_IER)

    /*  TIM2 Status register 1 (TIM2_SR1) */
    #define _TIM2_UIF               ((uint8_t) (0x01 << 0))        ///< TIM2 Update interrupt flag [0] (in _TIM2_SR1)
    #define _TIM2_CC1IF             ((uint8_t) (0x01 << 1))        ///< TIM2 Capture/compare 1 interrupt flag [0] (in _TIM2_SR1)
    #define _TIM2_CC2IF             ((uint8_t) (0x01 << 2))        ///< TIM2 Capture/compare 2 interrupt flag [0] (in _TIM2_SR1)
    // reserved [5:3]
    #define _TIM2_TIF               ((uint8_t) (0x01 << 6))        ///< TIM2 Trigger interrupt flag [0] (in _TIM2_SR1)
    #define _TIM2_BIF               ((uint8_t) (0x01 << 7))        ///< TIM2 Break interrupt flag [0] (in _TIM2_SR1)

    /*  TIM2 Status register 2 (TIM2_SR2) */
    // reserved [0]
    #define _TIM2_CC1OF             ((uint8_t) (0x01 << 1))        ///< TIM2 Capture/compare 1 overcapture flag [0] (in _TIM2_SR2)
    #define _TIM2_CC2OF             ((uint8_t) (0x01 << 2))        ///< TIM2 Capture/compare 2 overcapture flag [0] (in _TIM2_SR2)
    // reserved [7:3]

    /*  TIM2 Event generation register (TIM2_EGR) */
    #define _TIM2_UG                ((uint8_t) (0x01 << 0))        ///< TIM2 Update generation [0] (in _TIM2_EGR)
    #define _TIM2_CC1G              ((uint8_t) (0x01 << 1))        ///< TIM2 Capture/compare 1 generation [0] (in _TIM2_EGR)
    #define _TIM2_CC2G              ((uint8_t) (0x01 << 2))        ///< TIM2 Capture/compare 2 generation [0] (in _TIM2_EGR)
    // reserved [5:3]
    #define _TIM2_TG                ((uint8_t) (0x01 << 6))        ///< TIM2 Trigger generation [0] (in _TIM2_EGR)
    #define _TIM2_BG                ((uint8_t) (0x01 << 7))        ///< TIM2 Break generation [0] (in _TIM2_EGR)

    /*  TIM2 Capture/compare mode register 1 (TIM2_CCMR1). Output mode */
    #define _TIM2_CC1S              ((uint8_t) (0x03 << 0))        ///< TIM2 Compare 1 selection [1:0] (in _TIM2_CCMR1)
    #define _TIM2_CC1S0             ((uint8_t) (0x01 << 0))        ///< TIM2 Compare 1 selection [0] (in _TIM2_CCMR1)
    #define _TIM2_CC1S1             ((uint8_t) (0x01 << 1))        ///< TIM2 Compare 1 selection [1] (in _TIM2_CCMR1)
    #define _TIM2_OC1FE             ((uint8_t) (0x01 << 2))        ///< TIM2 Output compare 1 fast enable [0] (in _TIM2_CCMR1)
    #define _TIM2_OC1PE             ((uint8_t) (0x01 << 3))        ///< TIM2 Output compare 1 preload enable [0] (in _TIM2_CCMR1)
    #define _TIM2_OC1M              ((uint8_t) (0x07 << 4))        ///< TIM2 Output compare 1 mode [2:0] (in _TIM2_CCMR1)
    #define _TIM2_OC1M0             ((uint8_t) (0x01 << 4))        ///< TIM2 Output compare 1 mode [0] (in _TIM2_CCMR1)
    #define _TIM2_OC1M1             ((uint8_t) (0x01 << 5))        ///< TIM2 Output compare 1 mode [1] (in _TIM2_CCMR1)
    #define _TIM2_OC1M2             ((uint8_t) (0x01 << 6))        ///< TIM2 Output compare 1 mode [2] (in _TIM2_CCMR1)
    // reserved [7]

    /*  TIM2 Capture/compare mode register 1 (TIM2_CCMR1). Input mode */
    // _TIM2_CC1S [1:0] defined above
    #define _TIM2_IC1PSC            ((uint8_t) (0x03 << 2))        ///< TIM2 Input capture 1 prescaler [1:0] (in _TIM2_CCMR1)
    #define _TIM2_IC1PSC0           ((uint8_t) (0x01 << 2))        ///< TIM2 Input capture 1 prescaler [0] (in _TIM2_CCMR1)
    #define _TIM2_IC1PSC1           ((uint8_t) (0x01 << 3))        ///< TIM2 Input capture 1 prescaler [1] (in _TIM2_CCMR1)
    #define _TIM2_IC1F              ((uint8_t) (0x0F << 4))        ///< TIM2 Output compare 1 mode [3:0] (in _TIM2_CCMR1)
    #define _TIM2_IC1F0             ((uint8_t) (0x01 << 4))        ///< TIM2 Input capture 1 filter [0] (in _TIM2_CCMR1)
    #define _TIM2_IC1F1             ((uint8_t) (0x01 << 5))        ///< TIM2 Input capture 1 filter [1] (in _TIM2_CCMR1)
    #define _TIM2_IC1F2             ((uint8_t) (0x01 << 6))        ///< TIM2 Input capture 1 filter [2] (in _TIM2_CCMR1)
    #define _TIM2_IC1F3             ((uint8_t) (0x01 << 7))        ///< TIM2 Input capture 1 filter [3] (in _TIM2_CCMR1)

    /*  TIM2 Capture/compare mode register 2 (TIM2_CCMR2). Output mode */
    #define _TIM2_CC2S              ((uint8_t) (0x03 << 0))        ///< TIM2 Compare 2 selection [1:0] (in _TIM2_CCMR2)
    #define _TIM2_CC2S0             ((uint8_t) (0x01 << 0))        ///< TIM2 Compare 2 selection [0] (in _TIM2_CCMR2)
    #define _TIM2_CC2S1             ((uint8_t) (0x01 << 1))        ///< TIM2 Compare 2 selection [1] (in _TIM2_CCMR2)
    #define _TIM2_OC2FE             ((uint8_t) (0x01 << 2))        ///< TIM2 Output compare 2 fast enable [0] (in _TIM2_CCMR2)
    #define _TIM2_OC2PE             ((uint8_t) (0x01 << 3))        ///< TIM2 Output compare 2 preload enable [0] (in _TIM2_CCMR2)
    #define _TIM2_OC2M              ((uint8_t) (0x07 << 4))        ///< TIM2 Output compare 2 mode [2:0] (in _TIM2_CCMR2)
    #define _TIM2_OC2M0             ((uint8_t) (0x01 << 4))        ///< TIM2 Output compare 2 mode [0] (in _TIM2_CCMR2)
    #define _TIM2_OC2M1             ((uint8_t) (0x01 << 5))        ///< TIM2 Output compare 2 mode [1] (in _TIM2_CCMR2)
    #define _TIM2_OC2M2             ((uint8_t) (0x01 << 6))        ///< TIM2 Output compare 2 mode [2] (in _TIM2_CCMR2)
    // reserved [7]

    /*  TIM2 Capture/compare mode register 2 (TIM2_CCMR2). Input mode */
    // _TIM2_CC2S [1:0] defined above
    #define _TIM2_IC2PSC            ((uint8_t) (0x03 << 2))        ///< TIM2 Input capture 2 prescaler [1:0] (in _TIM2_CCMR2)
    #define _TIM2_IC2PSC0           ((uint8_t) (0x01 << 2))        ///< TIM2 Input capture 2 prescaler [0] (in _TIM2_CCMR2)
    #define _TIM2_IC2PSC1           ((uint8_t) (0x01 << 3))        ///< TIM2 Input capture 2 prescaler [1] (in _TIM2_CCMR2)
    #define _TIM2_IC2F              ((uint8_t) (0x0F << 4))        ///< TIM2 Output compare 2 mode [3:0] (in _TIM2_CCMR2)
    #define _TIM2_IC2F0             ((uint8_t) (0x01 << 4))        ///< TIM2 Input capture 2 filter [0] (in _TIM2_CCMR2)
    #define _TIM2_IC2F1             ((uint8_t) (0x01 << 5))        ///< TIM2 Input capture 2 filter [1] (in _TIM2_CCMR2)
    #define _TIM2_IC2F2             ((uint8_t) (0x01 << 6))        ///< TIM2 Input capture 2 filter [2] (in _TIM2_CCMR2)
    #define _TIM2_IC2F3             ((uint8_t) (0x01 << 7))        ///< TIM2 Input capture 2 filter [3] (in _TIM2_CCMR2)

    /*  TIM2 Capture/compare enable register 1 (TIM2_CCER1) */
    #define _TIM2_CC1E              ((uint8_t) (0x01 << 0))        ///< TIM2 Capture/compare 1 output enable [0] (in _TIM2_CCER1)
    #define _TIM2_CC1P              ((uint8_t) (0x01 << 1))        ///< TIM2 Capture/compare 1 output polarity [0] (in _TIM2_CCER1)
    // reserved [3:4]
    #define _TIM2_CC2E              ((uint8_t) (0x01 << 4))        ///< TIM2 Capture/compare 2 output enable [0] (in _TIM2_CCER1)
    #define _TIM2_CC2P              ((uint8_t) (0x01 << 5))        ///< TIM2 Capture/compare 2 output polarity [0] (in _TIM2_CCER1)
    // reserved [7:6]

    /*  TIM2 prescaler (TIM2_PSCR) */
    #define _TIM2_PSC               ((uint8_t) (0x07 << 0))        ///< TIM2 prescaler [2:0] (in _TIM2_PSCR)
    #define _TIM2_PSC0              ((uint8_t) (0x01 << 0))        ///< TIM2 prescaler [0] (in _TIM2_PSCR)
    #define _TIM2_PSC1              ((uint8_t) (0x01 << 1))        ///< TIM2 prescaler [1] (in _TIM2_PSCR)
    #define _TIM2_PSC2              ((uint8_t) (0x01 << 2))        ///< TIM2 prescaler [2] (in _TIM2_PSCR)
    // reserved [7:3]

    /*  TIM2 Break register (TIM2_BKR) */
    #define _TIM2_LOCK              ((int8_t) (0x03 << 0))         ///< TIM2 Lock configuration [1:0] (in _TIM2_BKR)
    #define _TIM2_LOCK0             ((uint8_t) (0x01 << 0))        ///< TIM2 Lock configuration [0] (in _TIM2_BKR)
    #define _TIM2_LOCK1             ((uint8_t) (0x01 << 1))        ///< TIM2 Lock configuration [1] (in _TIM2_BKR)
    #define _TIM2_OSSI              ((uint8_t) (0x01 << 2))        ///< TIM2 Off state selection for idle mode [0] (in _TIM2_BKR)
    // reserved [3]
    #define _TIM2_BKE               ((uint8_t) (0x01 << 4))        ///< TIM2 Break enable [0] (in _TIM2_BKR)
    #define _TIM2_BKP               ((uint8_t) (0x01 << 5))        ///< TIM2 Break polarity [0] (in _TIM2_BKR)
    #define _TIM2_AOE               ((uint8_t) (0x01 << 6))        ///< TIM2 Automatic output enable [0] (in _TIM2_BKR)
    #define _TIM2_MOE               ((uint8_t) (0x01 << 7))        ///< TIM2 Main output enable [0] (in _TIM2_BKR)

    /*  TIM2 Output idle state register (TIM2_OISR) */
    #define _TIM2_OIS1              ((uint8_t) (0x01 << 0))        ///< TIM2 Output idle state 1 (OC1 output) [0] (in _TIM2_OISR)
    // reserved [1]
    #define _TIM2_OIS2              ((uint8_t) (0x01 << 2))        ///< TIM2 Output idle state 2 (OC2 output) [0] (in _TIM2_OISR)
    // reserved [7:3

  #endif // TIM2_AddressBase


  /* Pointer to TIM3 registers */
  #if defined(TIM3_AddressBase)
    #define _TIM3        _SFR(TIM2_3_t,  TIM3_AddressBase)         ///< TIM3 struct/bit access
    #define _TIM3_CR1    _SFR(uint8_t,   TIM3_AddressBase+0x00)    ///< TIM3 control register 1
    #define _TIM3_CR2    _SFR(uint8_t,   TIM3_AddressBase+0x01)    ///< TIM3 control register 2
    #define _TIM3_SMCR   _SFR(uint8_t,   TIM3_AddressBase+0x02)    ///< TIM3 Slave mode control register
    #define _TIM3_ETR    _SFR(uint8_t,   TIM3_AddressBase+0x03)    ///< TIM3 External trigger register
    #define _TIM3_IER    _SFR(uint8_t,   TIM3_AddressBase+0x04)    ///< TIM3 interrupt enable register
    #define _TIM3_SR1    _SFR(uint8_t,   TIM3_AddressBase+0x05)    ///< TIM3 status register 1
    #define _TIM3_SR2    _SFR(uint8_t,   TIM3_AddressBase+0x06)    ///< TIM3 status register 2
    #define _TIM3_EGR    _SFR(uint8_t,   TIM3_AddressBase+0x07)    ///< TIM3 Event generation register
    #define _TIM3_CCMR1  _SFR(uint8_t,   TIM3_AddressBase+0x08)    ///< TIM3 Capture/compare mode register 1
    #define _TIM3_CCMR2  _SFR(uint8_t,   TIM3_AddressBase+0x09)    ///< TIM3 Capture/compare mode register 2
    #define _TIM3_CCER1  _SFR(uint8_t,   TIM3_AddressBase+0x0A)    ///< TIM3 Capture/compare enable register 1
    #define _TIM3_CNTRH  _SFR(uint8_t,   TIM3_AddressBase+0x0B)    ///< TIM3 counter register high byte
    #define _TIM3_CNTRL  _SFR(uint8_t,   TIM3_AddressBase+0x0C)    ///< TIM3 counter register low byte
    #define _TIM3_PSCR   _SFR(uint8_t,   TIM3_AddressBase+0x0D)    ///< TIM3 clock prescaler register
    #define _TIM3_ARRH   _SFR(uint8_t,   TIM3_AddressBase+0x0E)    ///< TIM3 auto-reload register high byte
    #define _TIM3_ARRL   _SFR(uint8_t,   TIM3_AddressBase+0x0F)    ///< TIM3 auto-reload register low byte
    #define _TIM3_CCR1H  _SFR(uint8_t,   TIM3_AddressBase+0x10)    ///< TIM3 16-bit capture/compare value 1 high byte
    #define _TIM3_CCR1L  _SFR(uint8_t,   TIM3_AddressBase+0x11)    ///< TIM3 16-bit capture/compare value 1 low byte
    #define _TIM3_CCR2H  _SFR(uint8_t,   TIM3_AddressBase+0x12)    ///< TIM3 16-bit capture/compare value 2 high byte
    #define _TIM3_CCR2L  _SFR(uint8_t,   TIM3_AddressBase+0x13)    ///< TIM3 16-bit capture/compare value 2 low byte
    #define _TIM3_BKR    _SFR(uint8_t,   TIM3_AddressBase+0x14)    ///< TIM3 Break register
    #define _TIM3_OISR   _SFR(uint8_t,   TIM3_AddressBase+0x15)    ///< TIM3 Output idle state register

    /* TIM3 Module Reset Values */
    #define _TIM3_CR1_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM3 control register 1 reset value
    #define _TIM3_CR2_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM3 control register 2 reset value
    #define _TIM3_SMCR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM3 Slave mode control register reset value
    #define _TIM3_ETR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM3 External trigger register reset value
    #define _TIM3_IER_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM3 interrupt enable register reset value
    #define _TIM3_SR1_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM3 status register 1 reset value
    #define _TIM3_SR2_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM3 status register 2 reset value
    #define _TIM3_EGR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM3 Event generation register reset value
    #define _TIM3_CCMR1_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM3 Capture/compare mode register 1 reset value
    #define _TIM3_CCMR2_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM3 Capture/compare mode register 2 reset value
    #define _TIM3_CCER1_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM3 Capture/compare enable register 1 reset value
    #define _TIM3_CNTRH_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM3 counter register high byte reset value
    #define _TIM3_CNTRL_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM3 counter register low byte reset value
    #define _TIM3_PSCR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM3 clock prescaler register reset value
    #define _TIM3_ARRH_RESET_VALUE       ((uint8_t) 0xFF)          ///< TIM3 auto-reload register high byte reset value
    #define _TIM3_ARRL_RESET_VALUE       ((uint8_t) 0xFF)          ///< TIM3 auto-reload register low byte reset value
    #define _TIM3_RCR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM3 Repetition counter reset value
    #define _TIM3_CCR1H_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM3 16-bit capture/compare value 1 high byte reset value
    #define _TIM3_CCR1L_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM3 16-bit capture/compare value 1 low byte reset value
    #define _TIM3_CCR2H_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM3 16-bit capture/compare value 2 high byte reset value
    #define _TIM3_CCR2L_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM3 16-bit capture/compare value 2 low byte reset value
    #define _TIM3_BKR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM3 Break register reset value
    #define _TIM3_OISR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM3 Output idle state register reset value

    /* TIM3 Control register (TIM3_CR1) */
    #define _TIM3_CEN               ((uint8_t) (0x01 << 0))        ///< TIM3 Counter enable [0] (in _TIM3_CR1)
    #define _TIM3_UDIS              ((uint8_t) (0x01 << 1))        ///< TIM3 Update disable [0] (in _TIM3_CR1)
    #define _TIM3_URS               ((uint8_t) (0x01 << 2))        ///< TIM3 Update request source [0] (in _TIM3_CR1)
    #define _TIM3_OPM               ((uint8_t) (0x01 << 3))        ///< TIM3 One-pulse mode [0] (in _TIM3_CR1)
    #define _TIM3_DIR               ((uint8_t) (0x01 << 4))        ///< TIM3 Direction [0] (in _TIM3_CR1)
    // reserved [6:5]
    #define _TIM3_ARPE              ((uint8_t) (0x01 << 7))        ///< TIM3 Auto-reload preload enable [0] (in _TIM3_CR1)

    /* TIM3 Control register (TIM3_CR2) */
    // reserved [3:0]
    #define _TIM3_MMS               ((uint8_t) (0x07 << 4))        ///< TIM3 Master mode selection [2:0] (in _TIM3_CR2)
    #define _TIM3_MMS0              ((uint8_t) (0x01 << 4))        ///< TIM3 Master mode selection [0] (in _TIM3_CR2)
    #define _TIM3_MMS1              ((uint8_t) (0x01 << 5))        ///< TIM3 Master mode selection [1] (in _TIM3_CR2)
    #define _TIM3_MMS2              ((uint8_t) (0x01 << 6))        ///< TIM3 Master mode selection [2] (in _TIM3_CR2)
    // reserved [7]

    /* TIM3 Slave mode control register (TIM3_SMCR) */
    #define _TIM3_SMS               ((uint8_t) (0x07 << 0))        ///< TIM3 Clock/trigger/slave mode selection [2:0] (in _TIM3_SMCR)
    #define _TIM3_SMS0              ((uint8_t) (0x01 << 0))        ///< TIM3 Clock/trigger/slave mode selection [0] (in _TIM3_SMCR)
    #define _TIM3_SMS1              ((uint8_t) (0x01 << 1))        ///< TIM3 Clock/trigger/slave mode selection [1] (in _TIM3_SMCR)
    #define _TIM3_SMS2              ((uint8_t) (0x01 << 2))        ///< TIM3 Clock/trigger/slave mode selection [2] (in _TIM3_SMCR)
    // reserved [3]
    #define _TIM3_TS                ((uint8_t) (0x07 << 4))        ///< TIM3 Trigger selection [2:0] (in _TIM3_SMCR)
    #define _TIM3_TS0               ((uint8_t) (0x01 << 4))        ///< TIM3 Trigger selection [0] (in _TIM3_SMCR)
    #define _TIM3_TS1               ((uint8_t) (0x01 << 5))        ///< TIM3 Trigger selection [1] (in _TIM3_SMCR)
    #define _TIM3_TS2               ((uint8_t) (0x01 << 6))        ///< TIM3 Trigger selection [2] (in _TIM3_SMCR)
    #define _TIM3_MSM               ((uint8_t) (0x01 << 7))        ///< TIM3 Master/slave mode [0] (in _TIM3_SMCR)

    /* TIM3 External trigger register (TIM3_ETR) */
    #define _TIM3_ETF               ((uint8_t) (0x0F << 0))        ///< TIM3 External trigger filter [3:0] (in _TIM3_ETR)
    #define _TIM3_ETF0              ((uint8_t) (0x01 << 0))        ///< TIM3 External trigger filter [0] (in _TIM3_ETR)
    #define _TIM3_ETF1              ((uint8_t) (0x01 << 1))        ///< TIM3 External trigger filter [1] (in _TIM3_ETR)
    #define _TIM3_ETF2              ((uint8_t) (0x01 << 2))        ///< TIM3 External trigger filter [2] (in _TIM3_ETR)
    #define _TIM3_ETF3              ((uint8_t) (0x01 << 3))        ///< TIM3 External trigger filter [3] (in _TIM3_ETR)
    #define _TIM3_ETPS              ((uint8_t) (0x03 << 4))        ///< TIM3 External trigger prescaler [1:0] (in _TIM3_ETR)
    #define _TIM3_ETPS0             ((uint8_t) (0x01 << 4))        ///< TIM3 External trigger prescaler [0] (in _TIM3_ETR)
    #define _TIM3_ETPS1             ((uint8_t) (0x01 << 5))        ///< TIM3 External trigger prescaler [1] (in _TIM3_ETR)
    #define _TIM3_ECE               ((uint8_t) (0x01 << 6))        ///< TIM3 External clock enable [0] (in _TIM3_ETR)
    #define _TIM3_ETP               ((uint8_t) (0x01 << 7))        ///< TIM3 External trigger polarity [0] (in _TIM3_ETR)

    /* TIM3 Interrupt enable (TIM3_IER) */
    #define _TIM3_UIE               ((uint8_t) (0x01 << 0))        ///< TIM3 Update interrupt enable [0] (in _TIM3_IER)
    #define _TIM3_CC1IE             ((uint8_t) (0x01 << 1))        ///< TIM3 Capture/compare 1 interrupt enable [0] (in _TIM3_IER)
    #define _TIM3_CC2IE             ((uint8_t) (0x01 << 2))        ///< TIM3 Capture/compare 2 interrupt enable [0] (in _TIM3_IER)
    // reserved [5:3]
    #define _TIM3_TIE               ((uint8_t) (0x01 << 6))        ///< TIM3 Trigger interrupt enable [0] (in _TIM3_IER)
    #define _TIM3_BIE               ((uint8_t) (0x01 << 7))        ///< TIM3 Break interrupt enable [0] (in _TIM3_IER)

    /*  TIM3 Status register 1 (TIM3_SR1) */
    #define _TIM3_UIF               ((uint8_t) (0x01 << 0))        ///< TIM3 Update interrupt flag [0] (in _TIM3_SR1)
    #define _TIM3_CC1IF             ((uint8_t) (0x01 << 1))        ///< TIM3 Capture/compare 1 interrupt flag [0] (in _TIM3_SR1)
    #define _TIM3_CC2IF             ((uint8_t) (0x01 << 2))        ///< TIM3 Capture/compare 2 interrupt flag [0] (in _TIM3_SR1)
    // reserved [5:3]
    #define _TIM3_TIF               ((uint8_t) (0x01 << 6))        ///< TIM3 Trigger interrupt flag [0] (in _TIM3_SR1)
    #define _TIM3_BIF               ((uint8_t) (0x01 << 7))        ///< TIM3 Break interrupt flag [0] (in _TIM3_SR1)

    /*  TIM3 Status register 2 (TIM3_SR2) */
    // reserved [0]
    #define _TIM3_CC1OF             ((uint8_t) (0x01 << 1))        ///< TIM3 Capture/compare 1 overcapture flag [0] (in _TIM3_SR2)
    #define _TIM3_CC2OF             ((uint8_t) (0x01 << 2))        ///< TIM3 Capture/compare 2 overcapture flag [0] (in _TIM3_SR2)
    // reserved [7:3]

    /*  TIM3 Event generation register (TIM3_EGR) */
    #define _TIM3_UG                ((uint8_t) (0x01 << 0))        ///< TIM3 Update generation [0] (in _TIM3_EGR)
    #define _TIM3_CC1G              ((uint8_t) (0x01 << 1))        ///< TIM3 Capture/compare 1 generation [0] (in _TIM3_EGR)
    #define _TIM3_CC2G              ((uint8_t) (0x01 << 2))        ///< TIM3 Capture/compare 2 generation [0] (in _TIM3_EGR)
    // reserved [5:3]
    #define _TIM3_TG                ((uint8_t) (0x01 << 6))        ///< TIM3 Trigger generation [0] (in _TIM3_EGR)
    #define _TIM3_BG                ((uint8_t) (0x01 << 7))        ///< TIM3 Break generation [0] (in _TIM3_EGR)

    /*  TIM3 Capture/compare mode register 1 (TIM3_CCMR1). Output mode */
    #define _TIM3_CC1S              ((uint8_t) (0x03 << 0))        ///< TIM3 Compare 1 selection [1:0] (in _TIM3_CCMR1)
    #define _TIM3_CC1S0             ((uint8_t) (0x01 << 0))        ///< TIM3 Compare 1 selection [0] (in _TIM3_CCMR1)
    #define _TIM3_CC1S1             ((uint8_t) (0x01 << 1))        ///< TIM3 Compare 1 selection [1] (in _TIM3_CCMR1)
    #define _TIM3_OC1FE             ((uint8_t) (0x01 << 2))        ///< TIM3 Output compare 1 fast enable [0] (in _TIM3_CCMR1)
    #define _TIM3_OC1PE             ((uint8_t) (0x01 << 3))        ///< TIM3 Output compare 1 preload enable [0] (in _TIM3_CCMR1)
    #define _TIM3_OC1M              ((uint8_t) (0x07 << 4))        ///< TIM3 Output compare 1 mode [2:0] (in _TIM3_CCMR1)
    #define _TIM3_OC1M0             ((uint8_t) (0x01 << 4))        ///< TIM3 Output compare 1 mode [0] (in _TIM3_CCMR1)
    #define _TIM3_OC1M1             ((uint8_t) (0x01 << 5))        ///< TIM3 Output compare 1 mode [1] (in _TIM3_CCMR1)
    #define _TIM3_OC1M2             ((uint8_t) (0x01 << 6))        ///< TIM3 Output compare 1 mode [2] (in _TIM3_CCMR1)
    // reserved [7]

    /*  TIM3 Capture/compare mode register 1 (TIM3_CCMR1). Input mode */
    // _TIM3_CC1S [1:0] defined above
    #define _TIM3_IC1PSC            ((uint8_t) (0x03 << 2))        ///< TIM3 Input capture 1 prescaler [1:0] (in _TIM3_CCMR1)
    #define _TIM3_IC1PSC0           ((uint8_t) (0x01 << 2))        ///< TIM3 Input capture 1 prescaler [0] (in _TIM3_CCMR1)
    #define _TIM3_IC1PSC1           ((uint8_t) (0x01 << 3))        ///< TIM3 Input capture 1 prescaler [1] (in _TIM3_CCMR1)
    #define _TIM3_IC1F              ((uint8_t) (0x0F << 4))        ///< TIM3 Output compare 1 mode [3:0] (in _TIM3_CCMR1)
    #define _TIM3_IC1F0             ((uint8_t) (0x01 << 4))        ///< TIM3 Input capture 1 filter [0] (in _TIM3_CCMR1)
    #define _TIM3_IC1F1             ((uint8_t) (0x01 << 5))        ///< TIM3 Input capture 1 filter [1] (in _TIM3_CCMR1)
    #define _TIM3_IC1F2             ((uint8_t) (0x01 << 6))        ///< TIM3 Input capture 1 filter [2] (in _TIM3_CCMR1)
    #define _TIM3_IC1F3             ((uint8_t) (0x01 << 7))        ///< TIM3 Input capture 1 filter [3] (in _TIM3_CCMR1)

    /*  TIM3 Capture/compare mode register 2 (TIM3_CCMR2). Output mode */
    #define _TIM3_CC2S              ((uint8_t) (0x03 << 0))        ///< TIM3 Compare 2 selection [1:0] (in _TIM3_CCMR2)
    #define _TIM3_CC2S0             ((uint8_t) (0x01 << 0))        ///< TIM3 Compare 2 selection [0] (in _TIM3_CCMR2)
    #define _TIM3_CC2S1             ((uint8_t) (0x01 << 1))        ///< TIM3 Compare 2 selection [1] (in _TIM3_CCMR2)
    #define _TIM3_OC2FE             ((uint8_t) (0x01 << 2))        ///< TIM3 Output compare 2 fast enable [0] (in _TIM3_CCMR2)
    #define _TIM3_OC2PE             ((uint8_t) (0x01 << 3))        ///< TIM3 Output compare 2 preload enable [0] (in _TIM3_CCMR2)
    #define _TIM3_OC2M              ((uint8_t) (0x07 << 4))        ///< TIM3 Output compare 2 mode [2:0] (in _TIM3_CCMR2)
    #define _TIM3_OC2M0             ((uint8_t) (0x01 << 4))        ///< TIM3 Output compare 2 mode [0] (in _TIM3_CCMR2)
    #define _TIM3_OC2M1             ((uint8_t) (0x01 << 5))        ///< TIM3 Output compare 2 mode [1] (in _TIM3_CCMR2)
    #define _TIM3_OC2M2             ((uint8_t) (0x01 << 6))        ///< TIM3 Output compare 2 mode [2] (in _TIM3_CCMR2)
    // reserved [7]

    /*  TIM3 Capture/compare mode register 2 (TIM3_CCMR2). Input mode */
    // _TIM3_CC2S [1:0] defined above
    #define _TIM3_IC2PSC            ((uint8_t) (0x03 << 2))        ///< TIM3 Input capture 2 prescaler [1:0] (in _TIM3_CCMR2)
    #define _TIM3_IC2PSC0           ((uint8_t) (0x01 << 2))        ///< TIM3 Input capture 2 prescaler [0] (in _TIM3_CCMR2)
    #define _TIM3_IC2PSC1           ((uint8_t) (0x01 << 3))        ///< TIM3 Input capture 2 prescaler [1] (in _TIM3_CCMR2)
    #define _TIM3_IC2F              ((uint8_t) (0x0F << 4))        ///< TIM3 Output compare 2 mode [3:0] (in _TIM3_CCMR2)
    #define _TIM3_IC2F0             ((uint8_t) (0x01 << 4))        ///< TIM3 Input capture 2 filter [0] (in _TIM3_CCMR2)
    #define _TIM3_IC2F1             ((uint8_t) (0x01 << 5))        ///< TIM3 Input capture 2 filter [1] (in _TIM3_CCMR2)
    #define _TIM3_IC2F2             ((uint8_t) (0x01 << 6))        ///< TIM3 Input capture 2 filter [2] (in _TIM3_CCMR2)
    #define _TIM3_IC2F3             ((uint8_t) (0x01 << 7))        ///< TIM3 Input capture 2 filter [3] (in _TIM3_CCMR2)

    /*  TIM3 Capture/compare enable register 1 (TIM3_CCER1) */
    #define _TIM3_CC1E              ((uint8_t) (0x01 << 0))        ///< TIM3 Capture/compare 1 output enable [0] (in _TIM3_CCER1)
    #define _TIM3_CC1P              ((uint8_t) (0x01 << 1))        ///< TIM3 Capture/compare 1 output polarity [0] (in _TIM3_CCER1)
    // reserved [3:4]
    #define _TIM3_CC2E              ((uint8_t) (0x01 << 4))        ///< TIM3 Capture/compare 2 output enable [0] (in _TIM3_CCER1)
    #define _TIM3_CC2P              ((uint8_t) (0x01 << 5))        ///< TIM3 Capture/compare 2 output polarity [0] (in _TIM3_CCER1)
    // reserved [7:6]

    /*  TIM3 prescaler (TIM3_PSCR) */
    #define _TIM3_PSC               ((uint8_t) (0x07 << 0))        ///< TIM3 prescaler [2:0] (in _TIM3_PSCR)
    #define _TIM3_PSC0              ((uint8_t) (0x01 << 0))        ///< TIM3 prescaler [0] (in _TIM3_PSCR)
    #define _TIM3_PSC1              ((uint8_t) (0x01 << 1))        ///< TIM3 prescaler [1] (in _TIM3_PSCR)
    #define _TIM3_PSC2              ((uint8_t) (0x01 << 2))        ///< TIM3 prescaler [2] (in _TIM3_PSCR)
    // reserved [7:3]

    /*  TIM3 Break register (TIM3_BKR) */
    #define _TIM3_LOCK              ((int8_t) (0x03 << 0))         ///< TIM3 Lock configuration [1:0] (in _TIM3_BKR)
    #define _TIM3_LOCK0             ((uint8_t) (0x01 << 0))        ///< TIM3 Lock configuration [0] (in _TIM3_BKR)
    #define _TIM3_LOCK1             ((uint8_t) (0x01 << 1))        ///< TIM3 Lock configuration [1] (in _TIM3_BKR)
    #define _TIM3_OSSI              ((uint8_t) (0x01 << 2))        ///< TIM3 Off state selection for idle mode [0] (in _TIM3_BKR)
    // reserved [3]
    #define _TIM3_BKE               ((uint8_t) (0x01 << 4))        ///< TIM3 Break enable [0] (in _TIM3_BKR)
    #define _TIM3_BKP               ((uint8_t) (0x01 << 5))        ///< TIM3 Break polarity [0] (in _TIM3_BKR)
    #define _TIM3_AOE               ((uint8_t) (0x01 << 6))        ///< TIM3 Automatic output enable [0] (in _TIM3_BKR)
    #define _TIM3_MOE               ((uint8_t) (0x01 << 7))        ///< TIM3 Main output enable [0] (in _TIM3_BKR)

    /*  TIM3 Output idle state register (TIM3_OISR) */
    #define _TIM3_OIS1              ((uint8_t) (0x01 << 0))        ///< TIM3 Output idle state 1 (OC1 output) [0] (in _TIM3_OISR)
    // reserved [1]
    #define _TIM3_OIS2              ((uint8_t) (0x01 << 2))        ///< TIM3 Output idle state 2 (OC2 output) [0] (in _TIM3_OISR)
    // reserved [7:3

  #endif // TIM3_AddressBase

#endif // TIM2_AddressBase || TIM3_AddressBase



//------------------------
// 8-Bit Timer 4 (TIM4)
//------------------------
#if defined(TIM4_AddressBase)

  /** @brief struct for controlling 8-Bit Timer 4 (TIM4) */
  typedef struct {

    /** @brief TIM4 Control register 1 (TIM4_CR1) */
    struct {
      _BITS   CEN     : 1;    ///< Counter enable
      _BITS   UDIS    : 1;    ///< Update disable
      _BITS   URS     : 1;    ///< Update request source
      _BITS   OPM     : 1;    ///< One-pulse mode
      _BITS           : 3;    //   Reserved
      _BITS   ARPE    : 1;    ///< Auto-reload preload enable
    } CR1;


    /** @brief TIM4 Control register 2 (TIM4_CR2) */
    struct {
      _BITS           : 4;    //   Reserved
      _BITS   MMS     : 3;    ///< Master mode selection
      _BITS           : 1;    //   Reserved
    } CR2;


    /** @brief TIM4 Slave mode control register (TIM4_SMCR) */
    struct {
      _BITS   SMS     : 3;    ///< Clock/trigger/slave mode selection
      _BITS           : 1;    //   Reserved
      _BITS   TS      : 3;    ///< Trigger selection
      _BITS   MSM     : 1;    ///< Master/slave mode
    } SMCR;


    /** @brief TIM4 Interrupt enable (TIM4_IER) */
    struct {
      _BITS   UIE     : 1;    ///< Update interrupt enable
      _BITS           : 5;    //   Reserved
      _BITS   TIE     : 1;    ///< Trigger interrupt enable
      _BITS           : 1;    //   Reserved
    } IER;


    /** @brief TIM4 Status register (TIM4_SR1) */
    struct {
      _BITS   UIF     : 1;    ///< Update interrupt flag
      _BITS           : 5;    //   Reserved
      _BITS   TIF     : 1;    ///< Trigger interrupt flag
      _BITS           : 1;    //   Reserved
    } SR1;


    /** @brief TIM4 Event Generation (TIM4_EGR) */
    struct {
      _BITS   UG      : 1;    ///< Update generation
      _BITS           : 5;    //   Reserved
      _BITS   TG      : 1;    ///< Trigger generation
      _BITS           : 1;    //   Reserved
    } EGR;


    /** @brief TIM4 8-bit counter register (TIM4_CNTR) */
    struct {
      _BITS   CNT     : 8;    ///< 8-bit counter
    } CNTR;

    /** @brief TIM4 clock prescaler (TIM4_PSCR) */
    struct {
      _BITS   PSC     : 4;    ///< clock prescaler
      _BITS           : 4;    //   Reserved
    } PSCR;


    /** @brief TIM4 8-bit auto-reload register (TIM4_ARR) */
    struct {
      _BITS   ARR     : 8;    ///< auto-reload value
    } ARR;

  } TIM4_t;

  /* Pointer to TIM4 registers */
  #define _TIM4       _SFR(TIM4_t,   TIM4_AddressBase)         ///< TIM4 struct/bit access
  #define _TIM4_CR1   _SFR(uint8_t,  TIM4_AddressBase+0x00)    ///< TIM4 control register 1
  #define _TIM4_CR2   _SFR(uint8_t,  TIM4_AddressBase+0x01)    ///< TIM4 control register 2
  #define _TIM4_SMCR  _SFR(uint8_t,  TIM4_AddressBase+0x02)    ///< TIM4 Slave mode control register
  #define _TIM4_IER   _SFR(uint8_t,  TIM4_AddressBase+0x03)    ///< TIM4 interrupt enable register
  #define _TIM4_SR1   _SFR(uint8_t,  TIM4_AddressBase+0x04)    ///< TIM4 status register
  #define _TIM4_EGR   _SFR(uint8_t,  TIM4_AddressBase+0x05)    ///< TIM4 event generation register
  #define _TIM4_CNTR  _SFR(uint8_t,  TIM4_AddressBase+0x06)    ///< TIM4 counter register
  #define _TIM4_PSCR  _SFR(uint8_t,  TIM4_AddressBase+0x07)    ///< TIM4 clock prescaler register
  #define _TIM4_ARR   _SFR(uint8_t,  TIM4_AddressBase+0x08)    ///< TIM4 auto-reload register

  /* TIM4 Module Reset Values */
  #define _TIM4_CR1_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM4 control register 1 reset value
  #define _TIM4_CR2_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM4 control register 2 reset value
  #define _TIM4_SMCR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM4 Slave mode control register reset value
  #define _TIM4_IER_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM4 interrupt enable register reset value
  #define _TIM4_SR1_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM4 status register reset value
  #define _TIM4_EGR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM4 event generation register reset value
  #define _TIM4_CNTR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM4 counter register reset value
  #define _TIM4_PSCR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM4 clock prescaler register reset value
  #define _TIM4_ARR_RESET_VALUE        ((uint8_t) 0xFF)          ///< TIM4 auto-reload register reset value

  /* TIM4 Control register 1 (TIM4_CR1) */
  #define _TIM4_CEN               ((uint8_t) (0x01 << 0))        ///< TIM4 Counter enable [0] (in _TIM4_CR1)
  #define _TIM4_UDIS              ((uint8_t) (0x01 << 1))        ///< TIM4 Update disable [0] (in _TIM4_CR1)
  #define _TIM4_URS               ((uint8_t) (0x01 << 2))        ///< TIM4 Update request source [0] (in _TIM4_CR1)
  #define _TIM4_OPM               ((uint8_t) (0x01 << 3))        ///< TIM4 One-pulse mode [0] (in _TIM4_CR1)
  // reserved [6:4]
  #define _TIM4_ARPE              ((uint8_t) (0x01 << 7))        ///< TIM4 Auto-reload preload enable [0] (in _TIM4_CR)

  /* TIM4 Control register (TIM4_CR2) */
  // reserved [3:0]
  #define _TIM4_MMS               ((uint8_t) (0x07 << 4))        ///< TIM4 Master mode selection [2:0] (in _TIM4_CR2)
  #define _TIM4_MMS0              ((uint8_t) (0x01 << 4))        ///< TIM4 Master mode selection [0] (in _TIM4_CR2)
  #define _TIM4_MMS1              ((uint8_t) (0x01 << 5))        ///< TIM4 Master mode selection [1] (in _TIM4_CR2)
  #define _TIM4_MMS2              ((uint8_t) (0x01 << 6))        ///< TIM4 Master mode selection [2] (in _TIM4_CR2)
  // reserved [7]

  /* TIM4 Slave mode control register (TIM4_SMCR) */
  #define _TIM4_SMS               ((uint8_t) (0x07 << 0))        ///< TIM4 Clock/trigger/slave mode selection [2:0] (in _TIM4_SMCR)
  #define _TIM4_SMS0              ((uint8_t) (0x01 << 0))        ///< TIM4 Clock/trigger/slave mode selection [0] (in _TIM4_SMCR)
  #define _TIM4_SMS1              ((uint8_t) (0x01 << 1))        ///< TIM4 Clock/trigger/slave mode selection [1] (in _TIM4_SMCR)
  #define _TIM4_SMS2              ((uint8_t) (0x01 << 2))        ///< TIM4 Clock/trigger/slave mode selection [2] (in _TIM4_SMCR)
  // reserved [3]
  #define _TIM4_TS                ((uint8_t) (0x07 << 4))        ///< TIM4 Trigger selection [2:0] (in _TIM4_SMCR)
  #define _TIM4_TS0               ((uint8_t) (0x01 << 4))        ///< TIM4 Trigger selection [0] (in _TIM4_SMCR)
  #define _TIM4_TS1               ((uint8_t) (0x01 << 5))        ///< TIM4 Trigger selection [1] (in _TIM4_SMCR)
  #define _TIM4_TS2               ((uint8_t) (0x01 << 6))        ///< TIM4 Trigger selection [2] (in _TIM4_SMCR)
  #define _TIM4_MSM               ((uint8_t) (0x01 << 7))        ///< TIM4 Master/slave mode [0] (in _TIM4_SMCR)

  /*  TIM4 Interrupt enable (TIM4_IER) */
  #define _TIM4_UIE               ((uint8_t) (0x01 << 0))        ///< TIM4 Update interrupt enable [0] (in _TIM4_IER)
  // reserved [5:1]
  #define _TIM4_TIE               ((uint8_t) (0x01 << 6))        ///< TIM4 Trigger interrupt enable [0] (in _TIM4_IER)
  // reserved [7]

  /*  TIM4 Status register (TIM4_SR1) */
  #define _TIM4_UIF               ((uint8_t) (0x01 << 0))        ///< TIM4 Update interrupt flag [0] (in _TIM4_SR1)
  // reserved [5:1]
  #define _TIM4_TIF               ((uint8_t) (0x01 << 6))        ///< TIM4 Trigger interrupt flag [0] (in _TIM4_SR1)
  // reserved [7]

  /*  TIM4 Event generation register (TIM4_EGR) */
  #define _TIM4_UG                ((uint8_t) (0x01 << 0))        ///< TIM4 Update generation [0] (in _TIM4_EGR)
  // reserved [5:3]
  #define _TIM4_TG                ((uint8_t) (0x01 << 6))        ///< TIM4 Trigger generation [0] (in _TIM4_EGR)
  // reserved [7]

  /* TIM4 Prescaler register (TIM4_PSCR) */
  #define _TIM4_PSC               ((uint8_t) (0x0F << 0))        ///< TIM4 clock prescaler [3:0] (in _TIM4_PSCR)
  #define _TIM4_PSC0              ((uint8_t) (0x01 << 0))        ///< TIM4 clock prescaler [0] (in _TIM4_PSCR)
  #define _TIM4_PSC1              ((uint8_t) (0x01 << 1))        ///< TIM4 clock prescaler [1] (in _TIM4_PSCR)
  #define _TIM4_PSC2              ((uint8_t) (0x01 << 2))        ///< TIM4 clock prescaler [2] (in _TIM4_PSCR)
  #define _TIM4_PSC3              ((uint8_t) (0x01 << 3))        ///< TIM4 clock prescaler [3] (in _TIM4_PSCR)
  // reserved [7:4]

#endif // TIM4_AddressBase



//------------------------
// Infrared Timer Module (IRTIM)
//------------------------
#if defined(IRTIM_AddressBase)

  /** @brief struct for Infrared Timer Module (IRTIM) */
  typedef struct {

    /** @brief IR-Timer Control register (IR_CR) */
    struct {
      _BITS   IR_EN   : 1;    ///< Infrared output enable
      _BITS   HS_EN   : 1;    ///< High Sink LED driver enable
      _BITS           : 6;    //   Reserved
    } CR;

  } IRTIM_t;

  /* Pointer to TIM4 registers */
  #define _IRTIM      _SFR(IRTIM_t,  IRTIM_AddressBase)        ///< IRTIM struct/bit access
  #define _IRTIM_CR1  _SFR(uint8_t,  IRTIM_AddressBase+0x00)   ///< IRTIM control register

  /* _IRTIM Module Reset Values */
  #define _IRTIM_CR_RESET_VALUE        ((uint8_t) 0x00)          ///< IRTIM control register reset value

  /* IRTIM Control register (IRTIM_CR1) */
  #define _IRTIM_IR_EN            ((uint8_t) (0x01 << 0))        ///< IRTIM Infrared output enable [0] (in _IRTIM_CR)
  #define _IRTIM_HS_EN            ((uint8_t) (0x01 << 1))        ///< IRTIM High Sink LED driver enable [0] (in _IRTIM_CR)
  // reserved [7:2]

#endif // IRTIM_AddressBase



//------------------------
// Comparator Module (COMP)
//------------------------
#if defined(COMP_AddressBase)

  /** @brief struct for Comparator Module (COMP) */
  typedef struct {

    /** @brief Comparator control register (COMP_CR) */
    struct {
      _BITS   BIAS_EN  : 1;    ///< Bias enable
      _BITS   COMP1_EN : 1;    ///< First comparator enable
      _BITS   COMP2_EN : 1;    ///< Second comparator enable
      _BITS   COMPREF  : 1;    ///< Comparator reference
      _BITS   POL      : 1;    ///< Comparator polarity
      _BITS   CNF_TIM  : 2;    ///< Comparator 1/2 output connected to TIM2/3 capture or break
      _BITS   IC1_BK   : 1;    ///< Input capture 1 / break selection
    } CR;

    /** @brief Comparator control status register (COMP_CSR) */
    struct {
      _BITS   COMP1_OUT : 1;    ///< First comparator output
      _BITS   COMP2_OUT : 1;    ///< Second comparator output
      _BITS             : 2;    //   Reserved, forced by hardware to 0
      _BITS   CEF1      : 1;    ///< First comparator event flag
      _BITS   ITEN1     : 1;    ///< First comparator interrupt enable
      _BITS   CEF2      : 1;    ///< Second comparator event flag
      _BITS   ITEN2     : 1;    ///< Second comparator interrupt enable
    } CSR;

    /** @brief Comparator channel selection (COMP_CCS) */
    struct {
      _BITS   COMP1_CH1 : 1;    ///< Comparator 1 switch 1 enable
      _BITS   COMP1_CH2 : 1;    ///< Comparator 1 switch 2 enable
      _BITS   COMP1_CH3 : 1;    ///< Comparator 1 switch 3 enable
      _BITS   COMP1_CH4 : 1;    ///< Comparator 1 switch 4 enable
      _BITS   COMP2_CH1 : 1;    ///< Comparator 2 switch 1 enable
      _BITS   COMP2_CH2 : 1;    ///< Comparator 2 switch 2 enable
      _BITS   COMP2_CH3 : 1;    ///< Comparator 2 switch 3 enable
      _BITS   COMP2_CH4 : 1;    ///< Comparator 2 switch 4 enable
    } CCS;

  } COMP_t;

  /* Pointer to TIM4 registers */
  #define _COMP       _SFR(COMP_t_t, COMP_AddressBase)         ///< COMP struct/bit access
  #define _COMP_CR    _SFR(uint8_t,  COMP_AddressBase+0x00)    ///< Comparator control register
  #define _COMP_CSR   _SFR(uint8_t,  COMP_AddressBase+0x01)    ///< Comparator control status register
  #define _COMP_CCS   _SFR(uint8_t,  COMP_AddressBase+0x02)    ///< Comparator channel selection

  /* _IRTIM Module Reset Values */
  #define _COMP_CR_RESET_VALUE         ((uint8_t) 0x00)          ///< Comparator control register reset value
  #define _COMP_CSR_RESET_VALUE        ((uint8_t) 0x00)          ///< Comparator control status register reset value
  #define _COMP_CCS_RESET_VALUE        ((uint8_t) 0x00)          ///< Comparator channel selection reset value

  /* Comparator control register (COMP_CR) */
  #define _COMP_BIAS_EN           ((uint8_t) (0x01 << 0))        ///< COMP Bias enable [0] (in _COMP_CR)
  #define _COMP_COMP1_EN          ((uint8_t) (0x01 << 1))        ///< COMP First comparator enable [0] (in _COMP_CR)
  #define _COMP_COMP2_EN          ((uint8_t) (0x01 << 2))        ///< COMP Second comparator enable [0] (in _COMP_CR)
  #define _COMP_COMPREF           ((uint8_t) (0x01 << 3))        ///< COMP Comparator reference [0] (in _COMP_CR)
  #define _COMP_POL               ((uint8_t) (0x01 << 4))        ///< COMP Comparator polarity [0] (in _COMP_CR)
  #define _COMP_CNF_TIM           ((uint8_t) (0x03 << 5))        ///< COMP Comparator 1/2 output connected to TIM2/3 capture or break [1:0] (in _COMP_CR)
  #define _COMP_CNF_TIM0          ((uint8_t) (0x03 << 5))        ///< COMP Comparator 1/2 output connected to TIM2/3 capture or break [0] (in _COMP_CR)
  #define _COMP_CNF_TIM1          ((uint8_t) (0x03 << 6))        ///< COMP Comparator 1/2 output connected to TIM2/3 capture or break [1] (in _COMP_CR)
  #define _COMP_IC1_BK            ((uint8_t) (0x01 << 7))        ///< COMP Input capture 1 / break selection [0] (in _COMP_CR)

  /* Comparator control register (COMP_CR) */
  #define _COMP_COMP1_OUT         ((uint8_t) (0x01 << 0))        ///< COMP First comparator output [0] (in _COMP_CSR)
  #define _COMP_COMP2_OUT         ((uint8_t) (0x01 << 1))        ///< COMP Second comparator output [0] (in _COMP_CSR)
  // reserved [3:2]
  #define _COMP_CEF1              ((uint8_t) (0x01 << 4))        ///< COMP First comparator event flag [0] (in _COMP_CSR)
  #define _COMP_ITEN1             ((uint8_t) (0x01 << 5))        ///< COMP First comparator interrupt enable [0] (in _COMP_CSR)
  #define _COMP_CEF2              ((uint8_t) (0x01 << 6))        ///< COMP Second comparator event flag [0] (in _COMP_CSR)
  #define _COMP_ITEN2             ((uint8_t) (0x01 << 7))        ///< COMP Second comparator interrupt enable [0] (in _COMP_CSR)

  /* Comparator channel selection (COMP_CCS) */
  #define _COMP_COMP1_CH1         ((uint8_t) (0x01 << 0))        ///< COMP Comparator 1 switch 1 enable [0] (in _COMP_CCS)
  #define _COMP_COMP1_CH2         ((uint8_t) (0x01 << 1))        ///< COMP Comparator 1 switch 2 enable [0] (in _COMP_CCS)
  #define _COMP_COMP1_CH3         ((uint8_t) (0x01 << 2))        ///< COMP Comparator 1 switch 3 enable [0] (in _COMP_CCS)
  #define _COMP_COMP1_CH4         ((uint8_t) (0x01 << 3))        ///< COMP Comparator 1 switch 4 enable [0] (in _COMP_CCS)
  #define _COMP_COMP2_CH1         ((uint8_t) (0x01 << 4))        ///< COMP Comparator 2 switch 1 enable [0] (in _COMP_CCS)
  #define _COMP_COMP2_CH2         ((uint8_t) (0x01 << 5))        ///< COMP Comparator 2 switch 2 enable [0] (in _COMP_CCS)
  #define _COMP_COMP2_CH3         ((uint8_t) (0x01 << 6))        ///< COMP Comparator 2 switch 3 enable [0] (in _COMP_CCS)
  #define _COMP_COMP2_CH4         ((uint8_t) (0x01 << 7))        ///< COMP Comparator 2 switch 4 enable [0] (in _COMP_CCS)

#endif // COMP_AddressBase



//------------------------
// Global Configuration (CFG)
//------------------------
#if defined(CFG_AddressBase)

  /** @brief struct for Global Configuration registers (CFG) */
  typedef struct {

    /** @brief Global configuration register (CFG_GCR) */
    struct {
      _BITS   SWD     : 1;    ///< SWIM disable
      _BITS   AL      : 1;    ///< Activation level
      _BITS           : 6;    //   Reserved
    } GCR;

  } CFG_t;

  /* Pointer to CFG registers */
  #define _CFG          _SFR(CFG_t,    CFG_AddressBase)          ///< CFG struct/bit access
  #define _CFG_GCR      _SFR(uint8_t,  CFG_AddressBase+0x00)     ///< Global configuration register (CFG_GCR)

  /* CFG Module Reset Values */
  #define _CFG_GCR_RESET_VALUE         ((uint8_t)0x00)

  /* Global configuration register (CFG_GCR) */
  #define _CFG_SWD              ((uint8_t) (0x01 << 0))        ///< SWIM disable [0]
  #define _CFG_AL               ((uint8_t) (0x01 << 1))        ///< Activation level [0]
  // reserved [7:2]

#endif // CFG_AddressBase



//------------------------
// Interrupt Priority Module (ITC)
//------------------------
#if defined(ITC_AddressBase)

  /** @brief struct for setting interrupt Priority (ITC) */
  typedef struct {

    /** @brief interrupt priority register 1 (ITC_SPR1) */
    struct {
      _BITS             : 2;    //   Reserved (TLI always highest prio)
      _BITS   VECT1SPR  : 2;    ///< interrupt priority vector 1
      _BITS             : 4;    //   Reserved
    } SPR1;


    /** @brief interrupt priority register 2 (ITC_SPR2) */
    struct {
      _BITS   VECT4SPR  : 2;    ///< interrupt priority vector 4
      _BITS             : 2;    //   Reserved
      _BITS   VECT6SPR  : 2;    ///< interrupt priority vector 6
      _BITS   VECT7SPR  : 2;    ///< interrupt priority vector 7
    } SPR2;


    /** @brief interrupt priority register 3 (ITC_SPR3) */
    struct {
      _BITS   VECT8SPR  : 2;    ///< interrupt priority vector 8
      _BITS   VECT9SPR  : 2;    ///< interrupt priority vector 9
      _BITS   VECT10SPR : 2;    ///< interrupt priority vector 10
      _BITS   VECT11SPR : 2;    ///< interrupt priority vector 11
    } SPR3;


    /** @brief interrupt priority register 4 (ITC_SPR4) */
    struct {
      _BITS   VECT12SPR : 2;    ///< interrupt priority vector 12
      _BITS   VECT13SPR : 2;    ///< interrupt priority vector 13
      _BITS   VECT14SPR : 2;    ///< interrupt priority vector 14
      _BITS   VECT15SPR : 2;    ///< interrupt priority vector 15
    } SPR4;


    /** @brief interrupt priority register 5 (ITC_SPR5) */
    struct {
      _BITS             : 6;    //   Reserved
      _BITS   VECT19SPR : 2;    ///< interrupt priority vector 19
    } SPR5;


    /** @brief interrupt priority register 6 (ITC_SPR6) */
    struct {
      _BITS   VECT20SPR : 2;    ///< interrupt priority vector 20
      _BITS   VECT21SPR : 2;    ///< interrupt priority vector 21
      _BITS   VECT22SPR : 2;    ///< interrupt priority vector 22
      _BITS             : 2;    //   Reserved
    } SPR6;


    /** @brief interrupt priority register 7 (ITC_SPR7) */
    struct {
      _BITS             : 2;    //   Reserved
      _BITS   VECT25SPR : 2;    ///< interrupt priority vector 25
      _BITS   VECT26SPR : 2;    ///< interrupt priority vector 26
      _BITS   VECT27SPR : 2;    ///< interrupt priority vector 27
    } SPR7;


    /** @brief interrupt priority register 8 (ITC_SPR8) */
    struct {
      _BITS   VECT28SPR : 2;    ///< interrupt priority vector 28
      _BITS   VECT29SPR : 2;    ///< interrupt priority vector 29
      _BITS             : 4;    //   Reserved
    } SPR8;

  } ITC_t;

  /* Pointer to ITC registers */
  #define _ITC          _SFR(ITC_t,    ITC_AddressBase)          ///< ITC struct/bit access
  #define _ITC_SPR1     _SFR(uint8_t,  ITC_AddressBase+0x00)     ///< Interrupt priority register 1/8
  #define _ITC_SPR2     _SFR(uint8_t,  ITC_AddressBase+0x01)     ///< Interrupt priority register 2/8
  #define _ITC_SPR3     _SFR(uint8_t,  ITC_AddressBase+0x02)     ///< Interrupt priority register 3/8
  #define _ITC_SPR4     _SFR(uint8_t,  ITC_AddressBase+0x03)     ///< Interrupt priority register 4/8
  #define _ITC_SPR5     _SFR(uint8_t,  ITC_AddressBase+0x04)     ///< Interrupt priority register 5/8
  #define _ITC_SPR6     _SFR(uint8_t,  ITC_AddressBase+0x05)     ///< Interrupt priority register 6/8
  #define _ITC_SPR7     _SFR(uint8_t,  ITC_AddressBase+0x06)     ///< Interrupt priority register 7/8
  #define _ITC_SPR8     _SFR(uint8_t,  ITC_AddressBase+0x07)     ///< Interrupt priority register 8/8

  /* ITC Module Reset Values */
  #define  _ITC_SPR1_RESET_VALUE       ((uint8_t) 0xFF)          ///< Interrupt priority register 1/8 reset value
  #define  _ITC_SPR2_RESET_VALUE       ((uint8_t) 0xFF)          ///< Interrupt priority register 2/8 reset value
  #define  _ITC_SPR3_RESET_VALUE       ((uint8_t) 0xFF)          ///< Interrupt priority register 3/8 reset value
  #define  _ITC_SPR4_RESET_VALUE       ((uint8_t) 0xFF)          ///< Interrupt priority register 4/8 reset value
  #define  _ITC_SPR5_RESET_VALUE       ((uint8_t) 0xFF)          ///< Interrupt priority register 5/8 reset value
  #define  _ITC_SPR6_RESET_VALUE       ((uint8_t) 0xFF)          ///< Interrupt priority register 6/8 reset value
  #define  _ITC_SPR7_RESET_VALUE       ((uint8_t) 0xFF)          ///< Interrupt priority register 7/8 reset value
  #define  _ITC_SPR8_RESET_VALUE       ((uint8_t) 0x0F)          ///< Interrupt priority register 8/8 reset value

  /* Software priority register 1 (ITC_SPR1) */
  // reserved [1:0]
  #define _ITC_VECT1SPR         ((uint8_t) (0x03 << 2))          ///< ITC interrupt priority vector 1 [1:0] (in _ITC_SPR1)
  #define _ITC_VECT1SPR0        ((uint8_t) (0x01 << 2))          ///< ITC interrupt priority vector 1 [0] (in _ITC_SPR1)
  #define _ITC_VECT1SPR1        ((uint8_t) (0x01 << 3))          ///< ITC interrupt priority vector 1 [1] (in _ITC_SPR1)
  // reserved [7:4]

  /* Software priority register 2 (ITC_SPR2) */
  #define _ITC_VECT4SPR         ((uint8_t) (0x03 << 0))          ///< ITC interrupt priority vector 4 [1:0] (in _ITC_SPR2)
  #define _ITC_VECT4SPR0        ((uint8_t) (0x01 << 0))          ///< ITC interrupt priority vector 4 [0] (in _ITC_SPR2)
  #define _ITC_VECT4SPR1        ((uint8_t) (0x01 << 1))          ///< ITC interrupt priority vector 4 [1] (in _ITC_SPR2)
  // reserved [3:2]
  #define _ITC_VECT6SPR         ((uint8_t) (0x03 << 4))          ///< ITC interrupt priority vector 6 [1:0] (in _ITC_SPR2)
  #define _ITC_VECT6SPR0        ((uint8_t) (0x01 << 4))          ///< ITC interrupt priority vector 6 [0] (in _ITC_SPR2)
  #define _ITC_VECT6SPR1        ((uint8_t) (0x01 << 5))          ///< ITC interrupt priority vector 6 [1] (in _ITC_SPR2)
  #define _ITC_VECT7SPR         ((uint8_t) (0x03 << 6))          ///< ITC interrupt priority vector 7 [1:0] (in _ITC_SPR2)
  #define _ITC_VECT7SPR0        ((uint8_t) (0x01 << 6))          ///< ITC interrupt priority vector 7 [0] (in _ITC_SPR2)
  #define _ITC_VECT7SPR1        ((uint8_t) (0x01 << 7))          ///< ITC interrupt priority vector 7 [1] (in _ITC_SPR2)

  /* Software priority register 3 (ITC_SPR3) */
  #define _ITC_VECT8SPR         ((uint8_t) (0x03 << 0))          ///< ITC interrupt priority vector 8 [1:0] (in _ITC_SPR3)
  #define _ITC_VECT8SPR0        ((uint8_t) (0x01 << 0))          ///< ITC interrupt priority vector 8 [0] (in _ITC_SPR3)
  #define _ITC_VECT8SPR1        ((uint8_t) (0x01 << 1))          ///< ITC interrupt priority vector 8 [1] (in _ITC_SPR3)
  #define _ITC_VECT9SPR         ((uint8_t) (0x03 << 2))          ///< ITC interrupt priority vector 9 [1:0] (in _ITC_SPR3)
  #define _ITC_VECT9SPR0        ((uint8_t) (0x01 << 2))          ///< ITC interrupt priority vector 9 [0] (in _ITC_SPR3)
  #define _ITC_VECT9SPR1        ((uint8_t) (0x01 << 3))          ///< ITC interrupt priority vector 9 [1] (in _ITC_SPR3)
  #define _ITC_VECT10SPR        ((uint8_t) (0x03 << 4))          ///< ITC interrupt priority vector 10 [1:0] (in _ITC_SPR3)
  #define _ITC_VECT10SPR0       ((uint8_t) (0x01 << 4))          ///< ITC interrupt priority vector 10 [0] (in _ITC_SPR3)
  #define _ITC_VECT10SPR1       ((uint8_t) (0x01 << 5))          ///< ITC interrupt priority vector 10 [1] (in _ITC_SPR3)
  #define _ITC_VECT11SPR        ((uint8_t) (0x03 << 6))          ///< ITC interrupt priority vector 11 [1:0] (in _ITC_SPR3)
  #define _ITC_VECT11SPR0       ((uint8_t) (0x01 << 6))          ///< ITC interrupt priority vector 11 [0] (in _ITC_SPR3)
  #define _ITC_VECT11SPR1       ((uint8_t) (0x01 << 7))          ///< ITC interrupt priority vector 11 [1] (in _ITC_SPR3)

  /* Software priority register 4 (ITC_SPR4) */
  #define _ITC_VECT12SPR        ((uint8_t) (0x03 << 0))          ///< ITC interrupt priority vector 12 [1:0] (in _ITC_SPR4)
  #define _ITC_VECT12SPR0       ((uint8_t) (0x01 << 0))          ///< ITC interrupt priority vector 12 [0] (in _ITC_SPR4)
  #define _ITC_VECT12SPR1       ((uint8_t) (0x01 << 1))          ///< ITC interrupt priority vector 12 [1] (in _ITC_SPR4)
  #define _ITC_VECT13SPR        ((uint8_t) (0x03 << 2))          ///< ITC interrupt priority vector 13 [1:0] (in _ITC_SPR4)
  #define _ITC_VECT13SPR0       ((uint8_t) (0x01 << 2))          ///< ITC interrupt priority vector 13 [0] (in _ITC_SPR4)
  #define _ITC_VECT13SPR1       ((uint8_t) (0x01 << 3))          ///< ITC interrupt priority vector 13 [1] (in _ITC_SPR4)
  #define _ITC_VECT14SPR        ((uint8_t) (0x03 << 4))          ///< ITC interrupt priority vector 14 [1:0] (in _ITC_SPR4)
  #define _ITC_VECT14SPR0       ((uint8_t) (0x01 << 4))          ///< ITC interrupt priority vector 14 [0] (in _ITC_SPR4)
  #define _ITC_VECT14SPR1       ((uint8_t) (0x01 << 5))          ///< ITC interrupt priority vector 14 [1] (in _ITC_SPR4)
  #define _ITC_VECT15SPR        ((uint8_t) (0x03 << 6))          ///< ITC interrupt priority vector 15 [1:0] (in _ITC_SPR4)
  #define _ITC_VECT15SPR0       ((uint8_t) (0x01 << 6))          ///< ITC interrupt priority vector 15 [0] (in _ITC_SPR4)
  #define _ITC_VECT15SPR1       ((uint8_t) (0x01 << 7))          ///< ITC interrupt priority vector 15 [1] (in _ITC_SPR4)

  /* Software priority register 5 (ITC_SPR5) */
  // reserved [5:0]
  #define _ITC_VECT19SPR        ((uint8_t) (0x03 << 6))          ///< ITC interrupt priority vector 19 [1:0] (in _ITC_SPR5)
  #define _ITC_VECT19SPR0       ((uint8_t) (0x01 << 6))          ///< ITC interrupt priority vector 19 [0] (in _ITC_SPR5)
  #define _ITC_VECT19SPR1       ((uint8_t) (0x01 << 7))          ///< ITC interrupt priority vector 19 [1] (in _ITC_SPR5)

  /* Software priority register 6 (ITC_SPR6) */
  #define _ITC_VECT20SPR        ((uint8_t) (0x03 << 0))          ///< ITC interrupt priority vector 20 [1:0] (in _ITC_SPR6)
  #define _ITC_VECT20SPR0       ((uint8_t) (0x01 << 0))          ///< ITC interrupt priority vector 20 [0] (in _ITC_SPR6)
  #define _ITC_VECT20SPR1       ((uint8_t) (0x01 << 1))          ///< ITC interrupt priority vector 20 [1] (in _ITC_SPR6)
  #define _ITC_VECT21SPR        ((uint8_t) (0x03 << 2))          ///< ITC interrupt priority vector 21 [1:0] (in _ITC_SPR6)
  #define _ITC_VECT21SPR0       ((uint8_t) (0x01 << 2))          ///< ITC interrupt priority vector 21 [0] (in _ITC_SPR6)
  #define _ITC_VECT21SPR1       ((uint8_t) (0x01 << 3))          ///< ITC interrupt priority vector 21 [1] (in _ITC_SPR6)
  #define _ITC_VECT22SPR        ((uint8_t) (0x03 << 4))          ///< ITC interrupt priority vector 22 [1:0] (in _ITC_SPR6)
  #define _ITC_VECT22SPR0       ((uint8_t) (0x01 << 4))          ///< ITC interrupt priority vector 22 [0] (in _ITC_SPR6)
  #define _ITC_VECT22SPR1       ((uint8_t) (0x01 << 5))          ///< ITC interrupt priority vector 22 [1] (in _ITC_SPR6)
  // reserved [7:6]

  /* Software priority register 7 (ITC_SPR7) */
  // reserved [1:0]
  #define _ITC_VECT25SPR        ((uint8_t) (0x03 << 2))          ///< ITC interrupt priority vector 25 [1:0] (in _ITC_SPR7)
  #define _ITC_VECT25SPR0       ((uint8_t) (0x01 << 2))          ///< ITC interrupt priority vector 25 [0] (in _ITC_SPR7)
  #define _ITC_VECT25SPR1       ((uint8_t) (0x01 << 3))          ///< ITC interrupt priority vector 25 [1] (in _ITC_SPR7)
  #define _ITC_VECT26SPR        ((uint8_t) (0x03 << 4))          ///< ITC interrupt priority vector 26 [1:0] (in _ITC_SPR7)
  #define _ITC_VECT26SPR0       ((uint8_t) (0x01 << 4))          ///< ITC interrupt priority vector 26 [0] (in _ITC_SPR7)
  #define _ITC_VECT26SPR1       ((uint8_t) (0x01 << 5))          ///< ITC interrupt priority vector 26 [1] (in _ITC_SPR7)
  #define _ITC_VECT27SPR        ((uint8_t) (0x03 << 6))          ///< ITC interrupt priority vector 27 [1:0] (in _ITC_SPR7)
  #define _ITC_VECT27SPR0       ((uint8_t) (0x01 << 6))          ///< ITC interrupt priority vector 27 [0] (in _ITC_SPR7)
  #define _ITC_VECT27SPR1       ((uint8_t) (0x01 << 7))          ///< ITC interrupt priority vector 27 [1] (in _ITC_SPR7)

  /* Software priority register 8 (ITC_SPR8) */
  #define _ITC_VECT28SPR        ((uint8_t) (0x03 << 0))          ///< ITC interrupt priority vector 28 [1:0] (in _ITC_SPR8)
  #define _ITC_VECT28SPR0       ((uint8_t) (0x01 << 0))          ///< ITC interrupt priority vector 28 [0] (in _ITC_SPR8)
  #define _ITC_VECT28SPR1       ((uint8_t) (0x01 << 1))          ///< ITC interrupt priority vector 28 [1] (in _ITC_SPR8)
  #define _ITC_VECT29SPR        ((uint8_t) (0x03 << 2))          ///< ITC interrupt priority vector 29 [1:0] (in _ITC_SPR8)
  #define _ITC_VECT29SPR0       ((uint8_t) (0x01 << 2))          ///< ITC interrupt priority vector 29 [0] (in _ITC_SPR8)
  #define _ITC_VECT29SPR1       ((uint8_t) (0x01 << 3))          ///< ITC interrupt priority vector 29 [1] (in _ITC_SPR8)
  // reserved [7:4]

#endif // ITC_AddressBase


// undefine local macros
#undef _BITS

/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // STM8L10X_H

/** @}*/
