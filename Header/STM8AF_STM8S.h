/*-------------------------------------------------------------------------
  STM8AF_STM8S.h - Register Declarations for STMAF and STM8S families

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
#ifndef STM8AF_STM8S_H
#define STM8AF_STM8S_H

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
  #error in 'STM8AF_STM8S.h': compiler not supported
#endif


/*-----------------------------------------------------------------------------
    MEMORY WIDTH
-----------------------------------------------------------------------------*/

// if memory sizes [B] are not given, assume smallest available in family
#if !defined(STM8_PFLASH_SIZE)
  #warning undefined STM8_PFLASH_SIZE, assume minimum
  #define STM8_PFLASH_SIZE  2*1024
#endif
#if !defined(STM8_RAM_SIZE)
  #warning undefined STM8_RAM_SIZE, assume minimum
  #define STM8_RAM_SIZE     1*1024
#endif
#if !defined(STM8_EEPROM_SIZE)
  #warning undefined STM8_EEPROM_SIZE, assume minimum
  #define STM8_EEPROM_SIZE  128
#endif

// memory start / end addresses
#define STM8_PFLASH_START 0x8000
#define STM8_PFLASH_END   (STM8_PFLASH_START + STM8_PFLASH_SIZE - 1)
#define STM8_RAM_START    0x0000
#define STM8_RAM_END      (STM8_RAM_START + STM8_RAM_SIZE - 1)
#define STM8_EEPROM_START 0x4000
#define STM8_EEPROM_END   (STM8_EEPROM_START + STM8_EEPROM_SIZE - 1)

// address space width (>32kB flash exceeds 16bit, as flash starts at 0x8000)
#if (STM8_PFLASH_END <= 0xFFFF)
  #define STM8_ADDR_WIDTH      16
  #define STM8_MEM_POINTER_T   uint16_t
#else
  #define STM8_ADDR_WIDTH      32
  #define STM8_MEM_POINTER_T   uint32_t
#endif


/*-----------------------------------------------------------------------------
    COMPILER SPECIFIC SETTINGS
-----------------------------------------------------------------------------*/

// Cosmic compiler
#if defined(_COSMIC_)

  // macros to unify ISR declaration and implementation
  #define ISR_HANDLER(func,irq)  @far @interrupt void func(void)
  #define ISR_HANDLER_TRAP(func) void @far @interrupt func(void)

  // common assembler instructions
  #define NOP()                  _asm("nop")                          ///< perform a nop() operation (=minimum delay)
  #define DISABLE_INTERRUPTS()   _asm("sim")                          ///< disable interrupt handling
  #define ENABLE_INTERRUPTS()    _asm("rim")                          ///< enable interrupt handling
  #define TRIGGER_TRAP           _asm("trap")                         ///< trigger a trap (=soft interrupt) e.g. for EMC robustness (see AN1015)
  #define WAIT_FOR_INTERRUPT()   _asm("wfi")                          ///< stop code execution and wait for interrupt
  #define ENTER_HALT()           _asm("halt")                         ///< put controller to HALT mode
  #define SW_RESET()             (_WWDG_CR=0xBF)                      ///< reset controller via WWGD module

  // data type in bit fields
  #define _BITS                  unsigned int                         ///< follow C90 standard

// Raisonance Compiler
#elif defined(_RAISONANCE_)

  // include intrinsic functions
  #include <intrins.h>

  // macros to unify ISR declaration and implementation
  #define ISR_HANDLER(func,irq)  void func(void) interrupt irq
  #define ISR_HANDLER_TRAP(func) void func(void) trap

  // common assembler instructions
  #define NOP()                _nop_()                              ///< perform a nop() operation (=minimum delay)
  #define DISABLE_INTERRUPTS() _sim_()                              ///< disable interrupt handling
  #define ENABLE_INTERRUPTS()  _rim_()                              ///< enable interrupt handling
  #define TRIGGER_TRAP         _trap_()                             ///< trigger a trap (=soft interrupt) e.g. for EMC robustness (see AN1015)
  #define WAIT_FOR_INTERRUPT() _wfi_()                              ///< stop code execution and wait for interrupt
  #define ENTER_HALT()         _halt_()                             ///< put controller to HALT mode
  #define SW_RESET()           (_WWDG_CR=0xBF)                      ///< reset controller via WWGD module

  // data type in bit fields
  #define _BITS                 unsigned int                        ///< follow C90 standard


// IAR Compiler
#elif defined(_IAR_)

  // include intrinsic functions
  #include <intrinsics.h>

  // macros to unify ISR declaration and implementation
  #define ISR_HANDLER(func,irq)  __interrupt void func(void)
  //#define ISR_HANDLER_TRAP(func) void func(void) trap

  // common assembler instructions
  #define NOP()                __no_operation()                     ///< perform a nop() operation (=minimum delay)
  #define DISABLE_INTERRUPTS() __disable_interrupt()                ///< disable interrupt handling
  #define ENABLE_INTERRUPTS()  __enable_interrupt()                 ///< enable interrupt handling
  #define TRIGGER_TRAP         __trap()                             ///< trigger a trap (=soft interrupt) e.g. for EMC robustness (see AN1015)
  #define WAIT_FOR_INTERRUPT() __wait_for_interrupt()               ///< stop code execution and wait for interrupt
  #define ENTER_HALT()         __halt()                             ///< put controller to HALT mode
  #define SW_RESET()           (_WWDG_CR=0xBF)                      ///< reset controller via WWGD module

  // data type in bit fields
  #define _BITS                 unsigned char                       ///< unsigned int fails for unknown reason


// SDCC compiler
#elif defined(_SDCC_)

  // macros to unify ISR declaration and implementation
  #define ISR_HANDLER(func,irq)   void func(void) __interrupt(irq)
  #if SDCC_VERSION >= 30403  // traps require >=v3.4.3
    #define ISR_HANDLER_TRAP(func)  void func() __trap
  #else
    #error traps require SDCC >=3.4.3. Please update!
  #endif

  // common assembler instructions
  #define NOP()                __asm__("nop")                       ///< perform a nop() operation (=minimum delay)
  #define DISABLE_INTERRUPTS() __asm__("sim")                       ///< disable interrupt handling
  #define ENABLE_INTERRUPTS()  __asm__("rim")                       ///< enable interrupt handling
  #define TRIGGER_TRAP         __asm__("trap")                      ///< trigger a trap (=soft interrupt) e.g. for EMC robustness (see AN1015)
  #define WAIT_FOR_INTERRUPT() __asm__("wfi")                       ///< stop code execution and wait for interrupt
  #define ENTER_HALT()         __asm__("halt")                      ///< put controller to HALT mode
  #define SW_RESET()           (_WWDG_CR=0xBF)                      ///< reset controller via WWGD module

  // data type in bit fields
  #define _BITS                 unsigned int                        ///< follow C90 standard

#endif


/*-----------------------------------------------------------------------------
    DEFINITION OF GLOBAL MACROS/#DEFINES
-----------------------------------------------------------------------------*/

// general macros
#define _SFR(type, addr)       (*((volatile type*) (addr)))         ///< peripheral register


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
    #define   INT8_MAX   0x7f
    #define   INT8_MIN   (-INT8_MAX - 1)

    #define   UINT8_MAX   0xFF
    #define   UINT8_MIN   0

    #define   INT16_MAX   0x7fff
    #define   INT16_MIN   (-INT16_MAX - 1)

    #define   UINT16_MAX  0xFFFF
    #define   UINT16_MIN  0
    /** Internal clock register (CLK_ICKR) */
    struct {


    #define   INT32_MAX   0x7fffffffL
    #define   INT32_MIN   (-INT32_MAX - 1L)

    #define   UINT32_MAX  0xFFFFFFFF
    #define   UINT32_MIN  0

  #endif // INT8_MAX

#elif defined(_SDCC_)

  // use compiler header
  #include <stdint.h>

#endif


/*-----------------------------------------------------------------------------
    ISR Vector Table (SDCC, Raisonance, IAR)
    Note: IAR has an IRQ offset of +2 compared to STM8 datasheet (see below)
-----------------------------------------------------------------------------*/

/// irq0 - External Top Level interrupt (TLI) for pin PD7
#define __TLI_VECTOR__            0

///< irq1 - Auto Wake Up from Halt interrupt (AWU)
#define __AWU_VECTOR__            1

/// irq2 - Clock Controller interrupt
#define __CLK_VECTOR__            2

/// irq3 - External interrupt 0 (GPIOA)
#define __PORTA_VECTOR__          3

/// irq4 - External interrupt 1 (GPIOB)
#define __PORTB_VECTOR__          4

/// irq5 - External interrupt 2 (GPIOC)
#define __PORTC_VECTOR__          5

/// irq6 - External interrupt 3 (GPIOD)
#define __PORTD_VECTOR__          6

/// irq7 - External interrupt 4 (GPIOE)
#define __PORTE_VECTOR__          7

// irq8 - External interrupt 5 (GPIOF) or CAN receive interrupt (device dependent)
#if defined(CAN_BaseAddress)
  #define __CAN_RX_VECTOR__       8     ///< irq8 - CAN receive interrupt
#else
  #define __PORTF_VECTOR__        8     ///< irq8 - External interrupt 5 (GPIOF)
#endif

// irq9 - CAN transmit interrupt (device dependent)
#if defined(CAN_BaseAddress)
  #define __CAN_TX_VECTOR__       9     ///< irq9 - CAN transmit interrupt
#endif

/// irq10 - SPI End of transfer interrupt
#define __SPI_VECTOR__            10

/// irq11 - TIM1 Update/Overflow/Trigger/Break interrupt
#define __TIM1_UPD_OVF_VECTOR__   11

/// irq12 - TIM1 Capture/Compare interrupt
#define __TIM1_CAPCOM_VECTOR__    12

// irq13 - TIM2 or TIM5 Update/overflow interrupt (device dependent)
#if defined(TIM2_BaseAddress)
  #define __TIM2_UPD_OVF_VECTOR__ 13    ///< irq13 - TIM2 Update/overflow interrupt
#elif defined(TIM5_BaseAddress)
  #define __TIM5_UPD_OVF_VECTOR__ 13    ///< irq13 - TIM5 Update/overflow interrupt
#endif

// irq14 - TIM2 or TIM5 Capture/Compare interrupt (device dependent)
#if defined(TIM2_BaseAddress)
  #define __TIM2_CAPCOM_VECTOR__  14    ///< irq14 - TIM2 Capture/Compare interrupt
#elif defined(TIM5_BaseAddress)
  #define __TIM5_CAPCOM_VECTOR__  14    ///< irq14 - TIM5 Capture/Compare interrupt
#endif

// irq15 - TIM3 Update/overflow interrupt (device dependent)
#if defined(TIM3_BaseAddress)
  #define __TIM3_UPD_OVF_VECTOR__ 15    ///< irq15 - TIM3 Update/overflow interrupt
#endif

// irq16 - TIM3 Capture/Compare interrupt (device dependent)
#if defined(TIM3_BaseAddress)
  #define __TIM3_CAPCOM_VECTOR__  16    ///< irq16 - TIM3 Capture/Compare interrupt
#endif

// irq17 - USART/UART1 send (TX empty) interrupt (device dependent)
#if defined(UART1_BaseAddress)
  #define __UART1_TXE_VECTOR__    17    ///< irq17 - USART/UART1 send (TX empty) interrupt
#endif

// irq18 - USART/UART1 receive (RX full) interrupt (device dependent)
#if defined(UART1_BaseAddress)
  #define __UART1_RXF_VECTOR__    18    ///< irq18 - USART/UART1 receive (RX full) interrupt
#endif

///< irq19 - I2C interrupt
#define __I2C_VECTOR__            19

// irq20 - UART2, UART3, UART4 send (TX empty) interrupt (device dependent)
#if defined(UART2_BaseAddress)
  #define __UART2_TXE_VECTOR__    20    ///< irq20 - UART2 send (TX empty) interrupt
#elif defined(UART3_BaseAddress)
  #define __UART3_TXE_VECTOR__    20    ///< irq20 - UART3 send (TX empty) interrupt
#elif defined(UART4_BaseAddress)
  #define __UART4_TXE_VECTOR__    20    ///< irq20 - UART4 send (TX empty) interrupt
#endif

// irq21 - UART2, UART3, UART4 receive (RX full) interrupt (device dependent)
#if defined(UART2_BaseAddress)
  #define __UART2_RXF_VECTOR__    21    ///< irq21 - UART2 receive (RX full) interrupt
#elif defined(UART3_BaseAddress)
  #define __UART3_RXF_VECTOR__    21    ///< irq21 - UART3 receive (RX full) interrupt
#elif defined(UART4_BaseAddress)
  #define __UART4_RXF_VECTOR__    21    ///< irq21 - UART4 receive (RX full) interrupt
#endif

// irq22 - ADC1 end of conversion or ADC2 end of conversion & analog watchdog interrupts (device dependent)
#if defined(ADC1_BaseAddress)
  #define __ADC1_VECTOR__         22    ///< irq22 - ADC1 end of conversion
#elif defined(ADC2_BaseAddress)
  #define __ADC2_VECTOR__         22    ///< irq22 - ADC2 end of conversion & analog watchdog interrupts
#endif

///< irq23 - TIM4, TIM6 Update/Overflow interrupt
#if defined(TIM4_BaseAddress)
  #define __TIM4_UPD_OVF_VECTOR__ 23    ///< irq23 - TIM4 Update/Overflow interrupt
#elif defined(TIM6_BaseAddress)
  #define __TIM6_UPD_OVF_VECTOR__ 23    ///< irq23 - TIM6 Update/Overflow interrupt
#endif

/// irq24 - flash interrupt
#define __FLASH_VECTOR__          24


/*-----------------------------------------------------------------------------
    DEFINITION OF STM8 PERIPHERAL REGISTERS
-----------------------------------------------------------------------------*/

//------------------------
// General purpose input/output pins (PORT)
//------------------------
#if defined(PORTA_BaseAddress) || defined(PORTB_BaseAddress) || defined(PORTC_BaseAddress) || defined(PORTD_BaseAddress) || \
    defined(PORTE_BaseAddress) || defined(PORTF_BaseAddress) || defined(PORTG_BaseAddress) || defined(PORTH_BaseAddress) || \
    defined(PORTI_BaseAddress)

  /** structure for controlling/monitoring pins in PORT mode (PORT_t) */
  typedef struct {

    /** Port x output data register (Px_ODR) */
    struct {
      _BITS   PIN0    : 1;    ///< pin 0 output control
      _BITS   PIN1    : 1;    ///< pin 1 output control
      _BITS   PIN2    : 1;    ///< pin 2 output control
      _BITS   PIN3    : 1;    ///< pPORTA_BaseAddressin 3 output control
      _BITS   PIN4    : 1;    ///< pin 4 output control
      _BITS   PIN5    : 1;    ///< pin 5 output control
      _BITS   PIN6    : 1;    ///< pin 6 output control
      _BITS   PIN7    : 1;    ///< pin 7 output control
    } ODR;

    /** Port x input data register (Px_IDR) */
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

    /** Port x data direction data register (Px_DDR) */
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

    /** Port x control register 1 (Px_CR1) */
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

    /** Port x control register 1 (Px_CR2) */
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
  #if defined(PORTA_BaseAddress)
    #define _GPIOA      _SFR(PORT_t,  PORTA_BaseAddress)    ///< bitfield access to port A
    #define _GPIOA_ODR  _SFR(uint8_t, PORTA_BaseAddress+0)  ///< port A output register
    #define _GPIOA_IDR  _SFR(uint8_t, PORTA_BaseAddress+1)  ///< port A input register
    #define _GPIOA_DDR  _SFR(uint8_t, PORTA_BaseAddress+2)  ///< port A direction register
    #define _GPIOA_CR1  _SFR(uint8_t, PORTA_BaseAddress+3)  ///< port A control register 1
    #define _GPIOA_CR2  _SFR(uint8_t, PORTA_BaseAddress+4)  ///< port A control register 2
  #endif

  /* Pointer to port B registers */
  #if defined(PORTB_BaseAddress)
    #define _GPIOB      _SFR(PORT_t,  PORTB_BaseAddress)    ///< bitfield access to port B
    #define _GPIOB_ODR  _SFR(uint8_t, PORTB_BaseAddress+0)  ///< port B output register
    #define _GPIOB_IDR  _SFR(uint8_t, PORTB_BaseAddress+1)  ///< port B input register
    #define _GPIOB_DDR  _SFR(uint8_t, PORTB_BaseAddress+2)  ///< port B direction register
    #define _GPIOB_CR1  _SFR(uint8_t, PORTB_BaseAddress+3)  ///< port B control register 1
    #define _GPIOB_CR2  _SFR(uint8_t, PORTB_BaseAddress+4)  ///< port B control register 2
  #endif

  /* Pointer to port C registers */
  #if defined(PORTC_BaseAddress)
    #define _GPIOC      _SFR(PORT_t,  PORTC_BaseAddress)    ///< bitfield access to port C
    #define _GPIOC_ODR  _SFR(uint8_t, PORTC_BaseAddress+0)  ///< port C output register
    #define _GPIOC_IDR  _SFR(uint8_t, PORTC_BaseAddress+1)  ///< port C input register
    #define _GPIOC_DDR  _SFR(uint8_t, PORTC_BaseAddress+2)  ///< port C direction register
    #define _GPIOC_CR1  _SFR(uint8_t, PORTC_BaseAddress+3)  ///< port C control register 1
    #define _GPIOC_CR2  _SFR(uint8_t, PORTC_BaseAddress+4)  ///< port C control register 2
  #endif

  /* Pointer to port D registers */
  #if defined(PORTD_BaseAddress)
    #define _GPIOD      _SFR(PORT_t,  PORTD_BaseAddress)    ///< bitfield access to port D
    #define _GPIOD_ODR  _SFR(uint8_t, PORTD_BaseAddress+0)  ///< port D output register
    #define _GPIOD_IDR  _SFR(uint8_t, PORTD_BaseAddress+1)  ///< port D input register
    #define _GPIOD_DDR  _SFR(uint8_t, PORTD_BaseAddress+2)  ///< port D direction register
    #define _GPIOD_CR1  _SFR(uint8_t, PORTD_BaseAddress+3)  ///< port D control register 1
    #define _GPIOD_CR2  _SFR(uint8_t, PORTD_BaseAddress+4)  ///< port D control register 2
  #endif

  /* Pointer to port E registers */
  #if defined(PORTE_BaseAddress)
    #define _GPIOE      _SFR(PORT_t,  PORTE_BaseAddress)    ///< bitfield access to port E
    #define _GPIOE_ODR  _SFR(uint8_t, PORTE_BaseAddress+0)  ///< port E output register
    #define _GPIOE_IDR  _SFR(uint8_t, PORTE_BaseAddress+1)  ///< port E input register
    #define _GPIOE_DDR  _SFR(uint8_t, PORTE_BaseAddress+2)  ///< port E direction register
    #define _GPIOE_CR1  _SFR(uint8_t, PORTE_BaseAddress+3)  ///< port E control register 1
    #define _GPIOE_CR2  _SFR(uint8_t, PORTE_BaseAddress+4)  ///< port E control register 2
  #endif

  /* Pointer to port F registers */
  #if defined(PORTF_BaseAddress)
    #define _GPIOF      _SFR(PORT_t,  PORTF_BaseAddress)    ///< bitfield access to port F
    #define _GPIOF_ODR  _SFR(uint8_t, PORTF_BaseAddress+0)  ///< port F output register
    #define _GPIOF_IDR  _SFR(uint8_t, PORTF_BaseAddress+1)  ///< port F input register
    #define _GPIOF_DDR  _SFR(uint8_t, PORTF_BaseAddress+2)  ///< port F direction register
    #define _GPIOF_CR1  _SFR(uint8_t, PORTF_BaseAddress+3)  ///< port F control register 1
    #define _GPIOF_CR2  _SFR(uint8_t, PORTF_BaseAddress+4)  ///< port F control register 2
  #endif

  /* Pointer to port G registers */
  #if defined(PORTG_BaseAddress)
    #define _GPIOG      _SFR(PORT_t,  PORTG_BaseAddress)    ///< bitfield access to port G
    #define _GPIOG_ODR  _SFR(uint8_t, PORTG_BaseAddress+0)  ///< port G output register
    #define _GPIOG_IDR  _SFR(uint8_t, PORTG_BaseAddress+1)  ///< port G input register
    #define _GPIOG_DDR  _SFR(uint8_t, PORTG_BaseAddress+2)  ///< port G direction register
    #define _GPIOG_CR1  _SFR(uint8_t, PORTG_BaseAddress+3)  ///< port G control register 1
    #define _GPIOG_CR2  _SFR(uint8_t, PORTG_BaseAddress+4)  ///< port G control register 2
  #endif

  /* Pointer to port H registers */
  #if defined(PORTH_BaseAddress)
    #define _GPIOH      _SFR(PORT_t,  PORTH_BaseAddress)    ///< bitfield access to port H
    #define _GPIOH_ODR  _SFR(uint8_t, PORTH_BaseAddress+0)  ///< port H output register
    #define _GPIOH_IDR  _SFR(uint8_t, PORTH_BaseAddress+1)  ///< port H input register
    #define _GPIOH_DDR  _SFR(uint8_t, PORTH_BaseAddress+2)  ///< port H direction register
    #define _GPIOH_CR1  _SFR(uint8_t, PORTH_BaseAddress+3)  ///< port H control register 1
    #define _GPIOH_CR2  _SFR(uint8_t, PORTH_BaseAddress+4)  ///< port H control register 2
  #endif

  /* Pointer to port I registers */
  #if defined(PORTI_BaseAddress)
    #define _GPIOI      _SFR(PORT_t,  PORTI_BaseAddress)    ///< bitfield access to port I
    #define _GPIOI_ODR  _SFR(uint8_t, PORTI_BaseAddress+0)  ///< port I output register
    #define _GPIOI_IDR  _SFR(uint8_t, PORTI_BaseAddress+1)  ///< port I input register
    #define _GPIOI_DDR  _SFR(uint8_t, PORTI_BaseAddress+2)  ///< port I direction register
    #define _GPIOI_CR1  _SFR(uint8_t, PORTI_BaseAddress+3)  ///< port I control register 1
    #define _GPIOI_CR2  _SFR(uint8_t, PORTI_BaseAddress+4)  ///< port I control register 2
  #endif

  /* PORT Module Reset Values (all ports) */
  #define _GPIO_ODR_RESET_VALUE ((uint8_t)0x00)
  #define _GPIO_DDR_RESET_VALUE ((uint8_t)0x00)
  #define _GPIO_CR1_RESET_VALUE ((uint8_t)0x00)
  #define _GPIO_CR2_RESET_VALUE ((uint8_t)0x00)

  /* macro for accessing pin registers (all ports) */
  #define _GPIO_PIN0   ((uint8_t) (0x01 << 0))
  #define _GPIO_PIN1   ((uint8_t) (0x01 << 1))
  #define _GPIO_PIN2   ((uint8_t) (0x01 << 2))
  #define _GPIO_PIN3   ((uint8_t) (0x01 << 3))
  #define _GPIO_PIN4   ((uint8_t) (0x01 << 4))
  #define _GPIO_PIN5   ((uint8_t) (0x01 << 5))
  #define _GPIO_PIN6   ((uint8_t) (0x01 << 6))
  #define _GPIO_PIN7   ((uint8_t) (0x01 << 7))

#endif // PORTx_BaseAddress



//------------------------
// Non-volative memory (FLASH)
//------------------------
#if defined(FLASH_BaseAddress)

  /** struct containing FLASH memory registers */
  typedef struct {

    /** Flash control register 1 (FLASH_CR1) */
    struct {
      _BITS   FIX   : 1;    ///< Fixed Byte programming time
      _BITS   IE    : 1;    ///< Flash Interrupt enable
      _BITS   AHALT : 1;    ///< Power-down in Active-halt mode
      _BITS   HALT  : 1;    ///< Power-down in Halt mode
      _BITS   res   : 4;    ///< Reserved
    } CR1;


    /** Flash control register 2 (FLASH_CR2) */
    struct {
      _BITS   PRG   : 1;    ///< Standard block programming
      _BITS   res   : 3;    ///< Reserved
      _BITS   FPRG  : 1;    ///< Fast block programming
      _BITS   ERASE : 1;    ///< Block erasing
      _BITS   WPRG  : 1;    ///< Word programming
      _BITS   OPT   : 1;    ///< Write option bytes
    } CR2;


    /** complementary Flash control register 2 (FLASH_NCR2) */
    struct {
      _BITS PRG     : 1;    ///< Standard block programming
      _BITS res     : 3;    ///< Reserved
      _BITS FPRG    : 1;    ///< Fast block programming
      _BITS ERASE   : 1;    ///< Block erasing
      _BITS WPRG    : 1;    ///< Word programming
      _BITS OPT     : 1;    ///< Write option bytes
    } NCR2;


    /** Flash protection register (FLASH_FPR) */
    struct {
      _BITS WPB     : 6;    ///< User boot code area protection bits
      _BITS res     : 2;    ///< Reserved
    } FPR;


    /** complementary Flash protection register (FLASH_NFPR) */
    struct {
      _BITS WPB     : 6;    ///< User boot code area protection bits
      _BITS res     : 2;    ///< Reserved
    } NFPR;


    /** Flash status register (FLASH_IAPSR) */
    struct {
      _BITS WR_PG_DIS : 1;  ///< Write attempted to protected page flag
      _BITS PUL       : 1;  ///< Flash Program memory unlocked flag
      _BITS EOP       : 1;  ///< End of programming (write or erase operation) flag
      _BITS DUL       : 1;  ///< Data EEPROM area unlocked flag
      _BITS res       : 2;  ///< Reserved, forced by hardware to 0
      _BITS HVOFF     : 1;  ///< End of high voltage flag
      _BITS res2      : 1;  ///< Reserved
    } IAPSR;


    /** Reserved registers (2B) */
    uint8_t   res[2];


    /** Flash program memory unprotecting key register (FLASH_PUKR). No bit access */
    uint8_t   PUKR;


    /** Reserved register (1B) */
    uint8_t   res2[1];


    /** Data EEPROM unprotection key register (FLASH_DUKR). No bit access */
    uint8_t   DUKR;

  } FLASH_t;

  /* Pointer to flash registers */
  #define _FLASH        _SFR(FLASH_t,  FLASH_BaseAddress)     ///< bitfield access to FLASH
  #define _FLASH_CR1    _SFR(uint8_t,  FLASH_BaseAddress+0)   ///< Flash control register 1 (FLASH_CR1)
  #define _FLASH_CR2    _SFR(uint8_t,  FLASH_BaseAddress+1)   ///< Flash control register 2 (FLASH_CR2)
  #define _FLASH_NCR2   _SFR(uint8_t,  FLASH_BaseAddress+2)   ///< complementary Flash control register 2 (FLASH_NCR2)
  #define _FLASH_FPR    _SFR(uint8_t,  FLASH_BaseAddress+3)   ///< Flash protection register (FLASH_FPR)
  #define _FLASH_NFPR   _SFR(uint8_t,  FLASH_BaseAddress+4)   ///< complementary Flash protection register (FLASH_NFPR)
  #define _FLASH_IAPSR  _SFR(uint8_t,  FLASH_BaseAddress+5)   ///< Flash status register (FLASH_IAPSR)
  // reserved (2B)
  #define _FLASH_PUKR   _SFR(uint8_t,  FLASH_BaseAddress+8)   ///< Flash program memory unprotecting key register (FLASH_PUKR)
  // reserved (1B)
  #define _FLASH_DUKR   _SFR(uint8_t,  FLASH_BaseAddress+10)  ///< Data EEPROM unprotection key register (FLASH_DUKR)

  /* FLASH Module Reset Values */
  #define _FLASH_CR1_RESET_VALUE    ((uint8_t)0x00)
  #define _FLASH_CR2_RESET_VALUE    ((uint8_t)0x00)
  #define _FLASH_NCR2_RESET_VALUE   ((uint8_t)0xFF)
  #define _FLASH_IAPSR_RESET_VALUE  ((uint8_t)0x40)
  #define _FLASH_PUKR_RESET_VALUE   ((uint8_t)0x00)
  #define _FLASH_DUKR_RESET_VALUE   ((uint8_t)0x00)

  /* Flash control register 1 (FLASH_CR1) */
  #define _FLASH_FIX              ((uint8_t) (0x01 << 0))        ///< Fixed Byte programming time [0]
  #define _FLASH_IE               ((uint8_t) (0x01 << 1))        ///< Flash Interrupt enable [0]
  #define _FLASH_AHALT            ((uint8_t) (0x01 << 2))        ///< Power-down in Active-halt mode [0]
  #define _FLASH_HALT             ((uint8_t) (0x01 << 3))        ///< Power-down in Halt mode [0]
  // reserved [7:4]

  /* Flash control register 2 and complement (FLASH_CR2 and FLASH_NCR2) */
  #define _FLASH_PRG              ((uint8_t) (0x01 << 0))        ///< Standard block programming [0]
  // reserved [3:1]
  #define _FLASH_FPRG             ((uint8_t) (0x01 << 4))        ///< Fast block programming [0]
  #define _FLASH_ERASE            ((uint8_t) (0x01 << 5))        ///< Block erasing [0]
  #define _FLASH_WPRG             ((uint8_t) (0x01 << 6))        ///< Word programming [0]
  #define _FLASH_OPT              ((uint8_t) (0x01 << 7))        ///< Write option bytes [0]

  /* Flash protection register and complement (FLASH_FPR and FLASH_NFPR) */
  #define _FLASH_WPB              ((uint8_t) (0x01 << 0))        ///< User boot code area protection bits [0]
  // reserved [7:1]

  /* Flash status register (FLASH_IAPSR) */
  #define _FLASH_WR_PG_DIS        ((uint8_t) (0x01 << 0))        ///< Write attempted to protected page flag [0]
  #define _FLASH_PUL              ((uint8_t) (0x01 << 1))        ///< Flash Program memory unlocked flag [0]
  #define _FLASH_EOP              ((uint8_t) (0x01 << 2))        ///< End of programming (write or erase operation) flag [0]
  #define _FLASH_DUL              ((uint8_t) (0x01 << 3))        ///< Data EEPROM area unlocked flag [0]
  // reserved [4]
  #define _FLASH_HVOFF            ((uint8_t) (0x01 << 5))        ///< End of high voltage flag [0]
  // reserved [7:6]

#endif // FLASH_BaseAddress



//------------------------
// External interrupt control (EXTI)
//------------------------
#if defined(EXTI_BaseAddress)

  /** struct containing EXTI registers */
  typedef struct {

    /** External interrupt control register 1 (EXTI_CR1) */
    struct {
      uint8_t PAIS    : 2;    ///< Port A external interrupt sensitivity bits
      uint8_t PBIS    : 2;    ///< Port B external interrupt sensitivity bits
      uint8_t PCIS    : 2;    ///< Port C external interrupt sensitivity bits
      uint8_t PDIS    : 2;    ///< Port D external interrupt sensitivity bits
    } CR1;


    /** External interrupt control register 2 (EXTI_CR2) */
    struct {
      uint8_t PEIS    : 2;    ///< Port E external interrupt sensitivity bits
      uint8_t TLIS    : 1;    ///< Top level interrupt sensitivity
      uint8_t res     : 5;    ///< Reserved
    } CR2;

  } EXTI_t;

  /* Pointer to EXTI registers */
  #define _EXTI         _SFR(EXTI_t,   EXTI_BaseAddress)      ///< bitfield access to EXTI
  #define _EXTI_CR1     _SFR(uint8_t,  EXTI_BaseAddress+0)    ///< External interrupt control register 1 (EXTI_CR1)
  #define _EXTI_CR2     _SFR(uint8_t,  EXTI_BaseAddress+1)    ///< External interrupt control register 2 (EXTI_CR2)

  /* EXTI Module Reset Values */
  #define _EXTI_CR1_RESET_VALUE ((uint8_t)0x00)
  #define _EXTI_CR2_RESET_VALUE ((uint8_t)0x00)

  /* External interrupt control register 1 (EXTI_CR1) */
  #define _EXTI_PAIS              ((uint8_t) 0x03)               ///< Port A external interrupt sensitivity bits [1:0]
  #define _EXTI_PAIS0             ((uint8_t) (0x01 << 0))        ///< Port A external interrupt sensitivity bits [0]
  #define _EXTI_PAIS1             ((uint8_t) (0x01 << 1))        ///< Port A external interrupt sensitivity bits [1]
  #define _EXTI_PBIS              ((uint8_t) 0x0C)               ///< Port B external interrupt sensitivity bits [1:0]
  #define _EXTI_PBIS0             ((uint8_t) (0x01 << 2))        ///< Port B external interrupt sensitivity bits [0]
  #define _EXTI_PBIS1             ((uint8_t) (0x01 << 3))        ///< Port B external interrupt sensitivity bits [1]
  #define _EXTI_PCIS              ((uint8_t) 0x30)               ///< Port C external interrupt sensitivity bits [1:0]
  #define _EXTI_PCIS0             ((uint8_t) (0x01 << 4))        ///< Port C external interrupt sensitivity bits [0]
  #define _EXTI_PCIS1             ((uint8_t) (0x01 << 5))        ///< Port C external interrupt sensitivity bits [1]
  #define _EXTI_PDIS              ((uint8_t) 0xC0)               ///< Port D external interrupt sensitivity bits [1:0]
  #define _EXTI_PDIS0             ((uint8_t) (0x01 << 6))        ///< Port D external interrupt sensitivity bits [0]
  #define _EXTI_PDIS1             ((uint8_t) (0x01 << 7))        ///< Port D external interrupt sensitivity bits [1]

  /* External interrupt control register 2 (EXTI_CR2) */
  #define _EXTI_PEIS              ((uint8_t) 0x03)               ///< Port E external interrupt sensitivity bits [1:0]
  #define _EXTI_PEIS0             ((uint8_t) (0x01 << 0))        ///< Port E external interrupt sensitivity bits [0]
  #define _EXTI_PEIS1             ((uint8_t) (0x01 << 1))        ///< Port E external interrupt sensitivity bits [1]
  #define _EXTI_TLIS              ((uint8_t) (0x01 << 2))        ///< Top level interrupt sensitivity [0]
  // reserved [7:3]

#endif // EXTI_BaseAddress



//------------------------
// Reset status (RST)
//------------------------
#if defined(RST_BaseAddress)

  /** struct containing RST registers */
  typedef struct {

    /** Reset status register (RST_SR) */
    struct {
      uint8_t WWDGF   : 1;    ///< Window Watchdog reset flag
      uint8_t IWDGF   : 1;    ///< Independent Watchdog reset flag
      uint8_t ILLOPF  : 1;    ///< Illegal opcode reset flag
      uint8_t SWIMF   : 1;    ///< SWIM reset flag
      uint8_t EMCF    : 1;    ///< EMC reset flag
      uint8_t res     : 3;    ///< Reserved
    } SR;

  } RST_t;

  /* Pointer to reset status register */
  #define _RST          _SFR(RST_t,    RST_BaseAddress)       ///< bitfield access to RST
  #define _RST_SR       _SFR(uint8_t,  RST_BaseAddress+0)     ///< Reset status register (RST_SR)

  /* Flash control register 1 (FLASH_CR1) */
  #define _RST_WWDGF              ((uint8_t) (0x01 << 0))        ///< Window Watchdog reset flag [0]
  #define _RST_IWDGF              ((uint8_t) (0x01 << 1))        ///< Independent Watchdog reset flag [0]
  #define _RST_ILLOPF             ((uint8_t) (0x01 << 2))        ///< Illegal opcode reset flag [0]
  #define _RST_SWIMF              ((uint8_t) (0x01 << 3))        ///< SWIM reset flag [0]
  #define _RST_EMCF               ((uint8_t) (0x01 << 4))        ///< EMC reset flag [0]
  // reserved [7:5]

#endif // RST_BaseAddress



//------------------------
// Clock control (CLK)
//------------------------
#if defined(CLK_BaseAddress)

  /** struct containing clock module registers (CLK) */
  typedef struct {

    /** Internal clock register (CLK_ICKR) */
    struct {
      _BITS   HSIEN   : 1;    ///< High speed internal RC oscillator enable
      _BITS   HSIRDY  : 1;    ///< High speed internal oscillator ready flag
      _BITS   FHW     : 1;    ///< Fast wakeup from Halt/Active-halt modes enable
      _BITS   LSIEN   : 1;    ///< Low speed internal RC oscillator enable
      _BITS   LSIRDY  : 1;    ///< Low speed internal oscillator ready flag
      _BITS   REGAH   : 1;    ///< Regulator power off in Active-halt mode enable
      _BITS   res     : 2;    ///< Reserved, must be kept cleared
    } ICKR;


    /** External clock register (CLK_ECKR) */
    struct {
      _BITS   HSEEN   : 1;    ///< High speed external crystal oscillator enable
      _BITS   HSERDY  : 1;    ///< High speed external crystal oscillator ready
      _BITS   res     : 6;    ///< Reserved, must be kept cleared
    } ECKR;


    /** Reserved register (1B) */
    uint8_t   res[1];


    /** Clock master status register (CLK_CMSR) */
    struct {
      _BITS   CKM     : 8;    ///< Clock master status bits
    } CMSR;


    /** Clock master switch register (CLK_SWR) */
    struct {
      _BITS   SWI     : 8;    ///< Clock master selection bits
    } SWR;


    /** Switch control register (CLK_SWCR) */
    struct {
      _BITS   SWBSY   : 1;    ///< Switch busy flag
      _BITS   SWEN    : 1;    ///< Switch start/stop enable
      _BITS   SWIEN   : 1;    ///< Clock switch interrupt enable
      _BITS   SWIF    : 1;    ///< Clock switch interrupt flag
      _BITS   res     : 4;    ///< Reserved
    } SWCR;


    /** Clock divider register (CLK_CKDIVR) */
    struct {
      _BITS   CPUDIV  : 3;    ///< CPU clock prescaler
      _BITS   HSIDIV  : 2;    ///< High speed internal clock prescaler
      _BITS   res     : 3;    ///< Reserved, must be kept cleared.
    } CKDIVR;


    /** Peripheral clock gating register 1 (CLK_PCKENR1) */
    struct {
      _BITS   PCKEN_I2C       : 1;    ///< clock enable I2C
      _BITS   PCKEN_SPI       : 1;    ///< clock enable SPI
      _BITS   PCKEN_UART1     : 1;    ///< clock enable UART1
      _BITS   PCKEN_UART2     : 1;    ///< clock enable UART2
      _BITS   PCKEN_TIM4_TIM6 : 1;    ///< clock enable TIM4/TIM6
      _BITS   PCKEN_TIM2_TIM5 : 1;    ///< clock enable TIM4/TIM6
      _BITS   PCKEN_TIM3      : 1;    ///< clock enable TIM3
      _BITS   PCKEN_TIM1      : 1;    ///< clock enable TIM1
    } PCKENR1;


    /** Clock security system register (CLK_CSSR) */
    struct {
      _BITS   CSSEN   : 1;    ///< Clock security system enable
      _BITS   AUX     : 1;    ///< Auxiliary oscillator connected to master clock
      _BITS   CSSDIE  : 1;    ///< Clock security system detection interrupt enable
      _BITS   CSSD    : 1;    ///< Clock security system detection
      _BITS   res     : 4;    ///< Reserved, must be kept cleared.
    } CSSR;


    /** Configurable clock output register (CLK_CCOR) */
    struct {
      _BITS   CCOEN   : 1;    ///< Configurable clock output enable
      _BITS   CCOSEL  : 4;    ///< Configurable clock output selection.
      _BITS   CCORDY  : 1;    ///< Configurable clock output ready
      _BITS   CCOBSY  : 1;    ///< Configurable clock output busy
      _BITS   res     : 1;    ///< Reserved, must be kept cleared.
    } CCOR;


    /** Peripheral clock gating register 2 (CLK_PCKENR2) */
    struct {
      _BITS   res       : 2;    ///< Reserved
      _BITS   PCKEN_AWU : 1;    ///< clock enable AWU
      _BITS   PCKEN_ADC : 1;    ///< clock enable ADC
      _BITS   res2      : 3;    ///< Reserved
      _BITS   PCKEN_CAN : 1;    ///< clock enable CAN
    } PCKENR2;


    /** Reserved register (1B). Was CAN clock control (obsolete as of STM8 UM rev 7) */
    uint8_t   res2[1];


    /** HSI clock calibration trimming register (CLK_HSITRIMR) */
    struct {
      _BITS   HSITRIM : 4;    ///< HSI trimming value (some devices only support 3 bits, see DS!)
      _BITS   res     : 4;    ///< Reserved, must be kept cleared.
    } HSITRIMR;


    /** SWIM clock control register (CLK_SWIMCCR) */
    struct {
      _BITS   SWIMCLK : 1;    ///< SWIM clock divider
      _BITS   res     : 7;    ///< Reserved.
    } SWIMCCR;

  } CLK_t;

  /* Pointer to CLK registers */
  #define _CLK          _SFR(CLK_t, CLK_BaseAddress)          ///< bitfield access to clock control
  #define _CLK_ICKR     _SFR(uint8_t, CLK_BaseAddress+0)      ///< Internal clock register
  #define _CLK_ECKR     _SFR(uint8_t, CLK_BaseAddress+1)      ///< External clock register
  // reserved (1B)
  #define _CLK_CMSR     _SFR(uint8_t, CLK_BaseAddress+3)      ///< Clock master status register
  #define _CLK_SWR      _SFR(uint8_t, CLK_BaseAddress+4)      ///< Clock master switch register
  #define _CLK_SWCR     _SFR(uint8_t, CLK_BaseAddress+5)      ///< Switch control register
  #define _CLK_CKDIVR   _SFR(uint8_t, CLK_BaseAddress+6)      ///< Clock divider register
  #define _CLK_PCKENR1  _SFR(uint8_t, CLK_BaseAddress+7)      ///< Peripheral clock gating register 1
  #define _CLK_CSSR     _SFR(uint8_t, CLK_BaseAddress+8)      ///< Clock security system register
  #define _CLK_CCOR     _SFR(uint8_t, CLK_BaseAddress+9)      ///< Configurable clock output register
  #define _CLK_PCKENR2  _SFR(uint8_t, CLK_BaseAddress+10)     ///< Peripheral clock gating register 2
  // reserved (1B)
  #define _CLK_HSITRIMR _SFR(uint8_t, CLK_BaseAddress+12)     ///< HSI clock calibration trimming register
  #define _CLK_SWIMCCR  _SFR(uint8_t, CLK_BaseAddress+13)     ///< SWIM clock control register

  /* CLK Module Reset Values */
  #define _CLK_ICKR_RESET_VALUE     ((uint8_t)0x01)
  #define _CLK_ECKR_RESET_VALUE     ((uint8_t)0x00)
  #define _CLK_CMSR_RESET_VALUE     ((uint8_t)0xE1)
  #define _CLK_SWR_RESET_VALUE      ((uint8_t)0xE1)
  #define _CLK_SWCR_RESET_VALUE     ((uint8_t)0x00)
  #define _CLK_CKDIVR_RESET_VALUE   ((uint8_t)0x18)
  #define _CLK_PCKENR1_RESET_VALUE  ((uint8_t)0xFF)
  #define _CLK_PCKENR2_RESET_VALUE  ((uint8_t)0xFF)
  #define _CLK_CSSR_RESET_VALUE     ((uint8_t)0x00)
  #define _CLK_CCOR_RESET_VALUE     ((uint8_t)0x00)
  #define _CLK_HSITRIMR_RESET_VALUE ((uint8_t)0x00)
  #define _CLK_SWIMCCR_RESET_VALUE  ((uint8_t)0x00)

  /* Internal clock register (CLK_ICKR) bits */
  #define _CLK_HSIEN              ((uint8_t) (0x01 << 0))        ///< High speed internal RC oscillator enable [0]
  #define _CLK_HSIRDY             ((uint8_t) (0x01 << 1))        ///< High speed internal oscillator ready [0]
  #define _CLK_FHWU               ((uint8_t) (0x01 << 2))        ///< Fast wakeup from Halt/Active-halt modes [0]
  #define _CLK_LSIEN              ((uint8_t) (0x01 << 3))        ///< Low speed internal RC oscillator enable [0]
  #define _CLK_LSIRDY             ((uint8_t) (0x01 << 4))        ///< Low speed internal oscillator ready [0]
  #define _CLK_REGAH              ((uint8_t) (0x01 << 5))        ///< Regulator power off in Active-halt mode [0]
  // reserved [7:6]

  /* External clock register (CLK_ECKR) */
  #define _CLK_HSEEN              ((uint8_t) (0x01 << 0))        ///< High speed external crystal oscillator enable [0]
  #define _CLK_ECKR_HSERDY        ((uint8_t) (0x01 << 1))        ///< High speed external crystal oscillator ready [0]
  // reserved [7:2]

  /* Switch control register (CLK_SWCR) */
  #define _CLK_SWBSY              ((uint8_t) (0x01 << 0))        ///< Switch busy flag [0]
  #define _CLK_SWEN               ((uint8_t) (0x01 << 1))        ///< Switch start/stop enable [0]
  #define _CLK_SWIEN              ((uint8_t) (0x01 << 2))        ///< Clock switch interrupt enable [0]
  #define _CLK_SWIF               ((uint8_t) (0x01 << 3))        ///< Clock switch interrupt flag [0]
  // reserved [7:4]

  /* Clock divider register (CLK_CKDIVR) */
  #define _CLK_CPUDIV             ((uint8_t) 0x07)               ///< CPU clock prescaler [2:0]
  #define _CLK_CPUDIV0            ((uint8_t) (0x01 << 0))        ///< CPU clock prescaler [0]
  #define _CLK_CPUDIV1            ((uint8_t) (0x01 << 1))        ///< CPU clock prescaler [1]
  #define _CLK_CPUDIV2            ((uint8_t) (0x01 << 2))        ///< CPU clock prescaler [2]
  #define _CLK_HSIDIV             ((uint8_t) 0x18)               ///< High speed internal clock prescaler [1:0]
  #define _CLK_HSIDIV0            ((uint8_t) (0x01 << 3))        ///< High speed internal clock prescaler [0]
  #define _CLK_HSIDIV1            ((uint8_t) (0x01 << 4))        ///< High speed internal clock prescaler [1]
  // reserved [7:5]

  /* Peripheral clock gating register 1 (CLK_PCKENR1) */
  #define _CLK_I2C                ((uint8_t) (0x01 << 0))        ///< clock enable I2C [0]
  #define _CLK_SPI                ((uint8_t) (0x01 << 1))        ///< clock enable SPI [0]
  #define _CLK_UART1              ((uint8_t) (0x01 << 2))        ///< clock enable UART1 [0]
  #define _CLK_UART2              ((uint8_t) (0x01 << 3))        ///< clock enable UART2 [0]
  #define _CLK_TIM4_TIM6          ((uint8_t) (0x01 << 4))        ///< clock enable TIM4/TIM6 [0]
  #define _CLK_TIM2_TIM5          ((uint8_t) (0x01 << 5))        ///< clock enable TIM2/TIM5 [0]
  #define _CLK_TIM3               ((uint8_t) (0x01 << 6))        ///< clock enable TIM3 [0]
  #define _CLK_TIM1               ((uint8_t) (0x01 << 7))        ///< clock enable TIM1 [0]

  /* Clock security system register (CLK_CSSR) */
  #define _CLK_CSSEN              ((uint8_t) (0x01 << 0))        ///< Clock security system enable [0]
  #define _CLK_AUX                ((uint8_t) (0x01 << 1))        ///< Auxiliary oscillator connected to master clock [0]
  #define _CLK_CSSDIE             ((uint8_t) (0x01 << 2))        ///< Clock security system detection interrupt enable [0]
  #define _CLK_CSSD               ((uint8_t) (0x01 << 3))        ///< Clock security system detection [0]
  // reserved [7:4]

  /* Configurable clock output register (CLK_CCOR) */
  #define _CLK_CCOEN              ((uint8_t) (0x01 << 0))        ///< Configurable clock output enable [0]
  #define _CLK_CCOSEL             ((uint8_t) 0x1E)               ///< Configurable clock output selection [3:0]
  #define _CLK_CCOSEL0            ((uint8_t) (0x01 << 1))        ///< Configurable clock output selection [0]
  #define _CLK_CCOSEL1            ((uint8_t) (0x01 << 2))        ///< Configurable clock output selection [1]
  #define _CLK_CCOSEL2            ((uint8_t) (0x01 << 3))        ///< Configurable clock output selection [2]
  #define _CLK_CCOSEL3            ((uint8_t) (0x01 << 4))        ///< Configurable clock output selection [3]
  #define _CLK_CCORDY             ((uint8_t) (0x01 << 5))        ///< Configurable clock output ready [0]
  #define _CLK_CCOBSY             ((uint8_t) (0x01 << 6))        ///< Configurable clock output busy [0]
  // reserved [7]

  /* Peripheral clock gating register 2 (CLK_PCKENR2) */
  // reserved [1:0]
  #define _CLK_AWU                ((uint8_t) (0x01 << 2))        ///< clock enable AWU [0]
  #define _CLK_ADC                ((uint8_t) (0x01 << 3))        ///< clock enable ADC [0]
  // reserved [6:4]
  #define _CLK_CAN                ((uint8_t) (0x01 << 7))        ///< clock enable CAN [0]

  /* HSI clock calibration trimming register (CLK_HSITRIMR) */
  #define _CLK_HSITRIM            ((uint8_t) 0x0F)               ///< HSI trimming value (some devices only support 3 bits, see DS!) [3:0]
  #define _CLK_HSITRIM0           ((uint8_t) (0x01 << 0))        ///< HSI trimming value [0]
  #define _CLK_HSITRIM1           ((uint8_t) (0x01 << 1))        ///< HSI trimming value [1]
  #define _CLK_HSITRIM2           ((uint8_t) (0x01 << 2))        ///< HSI trimming value [2]
  #define _CLK_HSITRIM3           ((uint8_t) (0x01 << 3))        ///< HSI trimming value [3]
  // reserved [7:4]

  /* SWIM clock control register (CLK_SWIMCCR) */
  #define _CLK_SWIMCLK            ((uint8_t) (0x01 << 0))        ///< SWIM clock divider [0]
  // reserved [7:2]

#endif // CLK_BaseAddress



//------------------------
// Window Watchdog (WWDG)
//------------------------
#if defined(WWDG_BaseAddress)

  /** struct containing Window Watchdog registers (WWDG) */
  typedef struct {

    /** WWDG Control register (WWDG_CR) */
    struct {
      uint8_t T       : 7;    ///< 7-bit WWDG counter
      uint8_t WDGA    : 1;    ///< WWDG activation bit (n/a if WWDG enabled by option byte)
    } CR;


    /** WWDR Window register (WWDG_WR) */
    struct {
      uint8_t W       : 7;    ///< 7-bit window value
      uint8_t res     : 1;    ///< Reserved
    } WR;

  } WWDG_t;

  /* Pointer to Window Watchdog registers */
  #define _WWDG         _SFR(WWDG_t,   WWDG_BaseAddress)      ///< bitfield access to WWDG
  #define _WWDG_CR      _SFR(uint8_t,  WWDG_BaseAddress+0)    ///< WWDG Control register (WWDG_CR)
  #define _WWDG_WR      _SFR(uint8_t,  WWDG_BaseAddress+1)    ///< WWDR Window register (WWDG_WR)

  /* WWDG Module Reset Values */
  #define _WWDG_CR_RESET_VALUE      ((uint8_t)0x7F)
  #define _WWDG_WR_RESET_VALUE      ((uint8_t)0x7F)

  /* WWDG Control register (WWDG_CR) */
  #define _WWDG_T                 ((uint8_t) (0x01 << 0))        ///< 7-bit WWDG counter [6:0]
  #define _WWDG_T0                ((uint8_t) (0x01 << 0))        ///< 7-bit WWDG counter [0]
  #define _WWDG_T1                ((uint8_t) (0x01 << 1))        ///< 7-bit WWDG counter [1]
  #define _WWDG_T2                ((uint8_t) (0x01 << 2))        ///< 7-bit WWDG counter [2]
  #define _WWDG_T3                ((uint8_t) (0x01 << 3))        ///< 7-bit WWDG counter [3]
  #define _WWDG_T4                ((uint8_t) (0x01 << 4))        ///< 7-bit WWDG counter [4]
  #define _WWDG_T5                ((uint8_t) (0x01 << 5))        ///< 7-bit WWDG counter [5]
  #define _WWDG_T6                ((uint8_t) (0x01 << 6))        ///< 7-bit WWDG counter [6]
  #define _WWDG_WDGA              ((uint8_t) (0x01 << 7))        ///< WWDG activation bit (n/a if WWDG enabled by option byte) [0]

  /* WWDR Window register (WWDG_WR) */
  #define _WWDG_W                 ((uint8_t) (0x01 << 0))        ///< 7-bit window value [6:0]
  #define _WWDG_W0                ((uint8_t) (0x01 << 0))        ///< 7-bit window value [0]
  #define _WWDG_W1                ((uint8_t) (0x01 << 1))        ///< 7-bit window value [1]
  #define _WWDG_W2                ((uint8_t) (0x01 << 2))        ///< 7-bit window value [2]
  #define _WWDG_W3                ((uint8_t) (0x01 << 3))        ///< 7-bit window value [3]
  #define _WWDG_W4                ((uint8_t) (0x01 << 4))        ///< 7-bit window value [4]
  #define _WWDG_W5                ((uint8_t) (0x01 << 5))        ///< 7-bit window value [5]
  #define _WWDG_W6                ((uint8_t) (0x01 << 6))        ///< 7-bit window value [6]
  // reserved [7]

#endif // WWDG_BaseAddress



//------------------------
// Independent Timeout Watchdog (IWDG)
//------------------------
#if defined(IWDG_BaseAddress)

  /** struct containing IWDG independent watchdog registers */
  typedef struct {

    /** IWDG Key register (IWDG_KR). No bit access */
    uint8_t   KR;


    /** IWDG Prescaler register (IWDG_PR) */
    struct {
      uint8_t PR      : 3;    ///< Prescaler divider
      uint8_t res     : 5;    ///< Reserved
    } PR;


    /** IWDG Reload register (IWDG_RLR). No bit access */
    uint8_t   RLR;

  } IWDG_t;

  /* Pointer to Independent Timeout Watchdog registers */
  #define _IWDG         _SFR(IWDG_t,   IWDG_BaseAddress)      ///< bitfield access to IWDG
  #define _IWDG_KR      _SFR(uint8_t,  IWDG_BaseAddress+0)    ///< IWDG Key register (IWDG_KR)
  #define _IWDG_PR      _SFR(uint8_t,  IWDG_BaseAddress+1)    ///< IWDG Prescaler register (IWDG_PR)
  #define _IWDG_RLR     _SFR(uint8_t,  IWDG_BaseAddress+1)    ///< IWDG Reload register (IWDG_RLR)

  /* IWDG Module Reset Values */
  #define IWDG_PR_RESET_VALUE        ((uint8_t)0x00)
  #define IWDG_RLR_RESET_VALUE       ((uint8_t)0xFF)

  /* IWDG Prescaler register (IWDG_PR) bits */
  #define _IWDG_PR                ((uint8_t) 0x07)               ///< Prescaler divider [2:0]
  #define _IWDG_PR0               ((uint8_t) (0x01 << 0))        ///< Prescaler divider [0]
  #define _IWDG_PR1               ((uint8_t) (0x01 << 1))        ///< Prescaler divider [1]
  #define _IWDG_PR2               ((uint8_t) (0x01 << 2))        ///< Prescaler divider [2]
  // reserved [7:3]

#endif // IWDG_BaseAddress



//------------------------
// Universal Asynchronous Receiver Transmitter 2 (UART2)
//------------------------
#if defined(UART2_BaseAddress)

  /** struct containing UART2 registers (selected devices) */
  typedef struct   {

    /** UART2 Status register (UART2_SR) */
    struct {
      _BITS   PE      : 1;    ///< Parity error
      _BITS   FE      : 1;    ///< Framing error
      _BITS   NF      : 1;    ///< Noise flag
      _BITS   OR_LHE  : 1;    ///< LIN Header Error (LIN slave mode) / Overrun error
      _BITS   IDLE    : 1;    ///< IDLE line detected
      _BITS   RXNE    : 1;    ///< Read data register not empty
      _BITS   TC      : 1;    ///< Transmission complete
      _BITS   TXE     : 1;    ///< Transmit data register empty
    } SR;


    /** UART2 data register (UART2_DR). No bit access */
    uint8_t   DR;


    /** UART2 Baud rate register 1 (UART2_BRR1) */
    struct {
      _BITS   DIV_4_11 : 8;   ///< UART2_BRR bits [11:4]
    } BRR1;


    /** UART2 Baud rate register 2 (UART2_BRR2) */
    struct {
      _BITS   DIV_0_3   : 4;    ///< UART2_BRR bits [3:0]
      _BITS   DIV_12_15 : 4;    ///< UART2_BRR bits [15:12]
    } BRR2;


    /** UART2 Control register 1 (UART2_CR1) */
    struct {
      _BITS   PIEN    : 1;   ///< Parity interrupt enable
      _BITS   PS      : 1;   ///< Parity selection
      _BITS   PCEN    : 1;   ///< Parity control enable
      _BITS   WAKE    : 1;   ///< Wakeup method
      _BITS   M       : 1;   ///< word length
      _BITS   UARTD   : 1;   ///< UART Disable (for low power consumption)
      _BITS   T8      : 1;   ///< Transmit Data bit 8 (in 9-bit mode)
      _BITS   R8      : 1;   ///< Receive Data bit 8 (in 9-bit mode)
    } CR1;


    /** UART2 Control register 2 (UART2_CR2) */
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


    /** UART2 Control register 3 (UART2_CR3) */
    struct {
      _BITS   LBCL    : 1;    ///< Last bit clock pulse
      _BITS   CPHA    : 1;    ///< Clock phase
      _BITS   CPOL    : 1;    ///< Clock polarity
      _BITS   CKEN    : 1;    ///< Clock enable
      _BITS   STOP    : 2;    ///< STOP bits
      _BITS   LINEN   : 1;    ///< LIN mode enable
      _BITS   res     : 1;    ///< Reserved, must be kept cleared
    } CR3;


    /** UART2 Control register 4 (UART2_CR4) */
    struct {
      _BITS   ADD     : 4;    ///< Address of the UART node
      _BITS   LBDF    : 1;    ///< LIN Break Detection Flag
      _BITS   LBDL    : 1;    ///< LIN Break Detection Length
      _BITS   LBDIEN  : 1;    ///< LIN Break Detection Interrupt Enable
      _BITS   res     : 1;    ///< Reserved, must be kept cleared
    } CR4;


    /** UART2 Control register 5 (UART2_CR5) */
    struct {
      _BITS   res     : 1;    ///< Reserved, must be kept cleared
      _BITS   IREN    : 1;    ///< IrDA mode Enable
      _BITS   IRLP    : 1;    ///< IrDA Low Power
      _BITS   res2    : 1;    ///< Reserved, must be kept cleared
      _BITS   NACK    : 1;    ///< Smartcard NACK enable
      _BITS   SCEN    : 1;    ///< Smartcard mode enable
      _BITS   res3    : 2;    ///< Reserved, must be kept cleared
    } CR5;


    /** UART2 Control register 6 (UART2_CR6) */
    struct {
      _BITS   LSF     : 1;    ///< LIN Sync Field
      _BITS   LHDF    : 1;    ///< LIN Header Detection Flag
      _BITS   LHDIEN  : 1;    ///< LIN Header Detection Interrupt Enable
      _BITS   res     : 1;    ///< Reserved
      _BITS   LASE    : 1;    ///< LIN automatic resynchronisation enable
      _BITS   LSLV    : 1;    ///< LIN Slave Enable
      _BITS   res2    : 1;    ///< Reserved
      _BITS   LDUM    : 1;    ///< LIN Divider Update Method
    } CR6;


    /** UART2 guard time register register (UART2_GTR). No bit access */
    uint8_t   GTR;


    /** UART2 prescaler register register register (UART2_PSCR). No bit access */
    uint8_t   PSCR;

  } UART2_t;

  /* Pointer to UART2 registers */
  #define _UART2      _SFR(UART2_t,  UART2_BaseAddress)       ///< bitfield access to UART2
  #define _UART2_SR   _SFR(uint8_t,  UART2_BaseAddress+0)     ///< UART2 Status register
  #define _UART2_DR   _SFR(uint8_t,  UART2_BaseAddress+1)     ///< UART2 data register
  #define _UART2_BRR1 _SFR(uint8_t,  UART2_BaseAddress+2)     ///< UART2 Baud rate register 1
  #define _UART2_BRR2 _SFR(uint8_t,  UART2_BaseAddress+3)     ///< UART2 Baud rate register 2
  #define _UART2_CR1  _SFR(uint8_t,  UART2_BaseAddress+4)     ///< UART2 Control register 1
  #define _UART2_CR2  _SFR(uint8_t,  UART2_BaseAddress+5)     ///< UART2 Control register 2
  #define _UART2_CR3  _SFR(uint8_t,  UART2_BaseAddress+6)     ///< UART2 Control register 3
  #define _UART2_CR4  _SFR(uint8_t,  UART2_BaseAddress+7)     ///< UART2 Control register 4
  #define _UART2_CR5  _SFR(uint8_t,  UART2_BaseAddress+8)     ///< UART2 Control register 5
  #define _UART2_CR6  _SFR(uint8_t,  UART2_BaseAddress+9)     ///< UART2 Control register 6
  #define _UART2_GTR  _SFR(uint8_t,  UART2_BaseAddress+10)    ///< UART2 guard time register register
  #define _UART2_PSCR _SFR(uint8_t,  UART2_BaseAddress+11)    ///< UART2 prescaler register register register

  /* UART2 Module Reset Values */
  #define _UART2_SR_RESET_VALUE      ((uint8_t)0xC0)
  #define _UART2_BRR1_RESET_VALUE    ((uint8_t)0x00)
  #define _UART2_BRR2_RESET_VALUE    ((uint8_t)0x00)
  #define _UART2_CR1_RESET_VALUE     ((uint8_t)0x00)
  #define _UART2_CR2_RESET_VALUE     ((uint8_t)0x00)
  #define _UART2_CR3_RESET_VALUE     ((uint8_t)0x00)
  #define _UART2_CR4_RESET_VALUE     ((uint8_t)0x00)
  #define _UART2_CR5_RESET_VALUE     ((uint8_t)0x00)
  #define _UART2_CR6_RESET_VALUE     ((uint8_t)0x00)
  #define _UART2_GTR_RESET_VALUE     ((uint8_t)0x00)
  #define _UART2_PSCR_RESET_VALUE    ((uint8_t)0x00)

  /* UART2 Status register (UART2_SR) bits */
  #define _UART2_PE               ((uint8_t) (0x01 << 0))        ///< Parity error [0]
  #define _UART2_FE               ((uint8_t) (0x01 << 1))        ///< Framing error [0]
  #define _UART2_NF               ((uint8_t) (0x01 << 2))        ///< Noise flag [0]
  #define _UART2_OR_LHE           ((uint8_t) (0x01 << 3))        ///< LIN Header Error (LIN slave mode) / Overrun error [0]
  #define _UART2_IDLE             ((uint8_t) (0x01 << 4))        ///< IDLE line detected [0]
  #define _UART2_RXNE             ((uint8_t) (0x01 << 5))        ///< Read data register not empty [0]
  #define _UART2_TC               ((uint8_t) (0x01 << 6))        ///< Transmission complete [0]
  #define _UART2_TXE              ((uint8_t) (0x01 << 7))        ///< Transmit data register empty [0]

  /* UART2 Control register 1 (UART2_CR1) bits */
  #define _UART2_PIEN             ((uint8_t) (0x01 << 0))        ///< Parity interrupt enable [0]
  #define _UART2_PS               ((uint8_t) (0x01 << 1))        ///< Parity selection [0]
  #define _UART2_PCEN             ((uint8_t) (0x01 << 2))        ///< Parity control enable [0]
  #define _UART2_WAKE             ((uint8_t) (0x01 << 3))        ///< Wakeup method [0]
  #define _UART2_M                ((uint8_t) (0x01 << 4))        ///< word length [0]
  #define _UART2_UARTD            ((uint8_t) (0x01 << 5))        ///< UART Disable (for low power consumption) [0]
  #define _UART2_T8               ((uint8_t) (0x01 << 6))        ///< Transmit Data bit 8 (in 9-bit mode) [0]
  #define _UART2_R8               ((uint8_t) (0x01 << 7))        ///< Receive Data bit 8 (in 9-bit mode) [0]

  /* UART2 Control register 2 (UART2_CR2) bits */
  #define _UART2_SBK              ((uint8_t) (0x01 << 0))        ///< Send break [0]
  #define _UART2_RWU              ((uint8_t) (0x01 << 1))        ///< Receiver wakeup [0]
  #define _UART2_REN              ((uint8_t) (0x01 << 2))        ///< Receiver enable [0]
  #define _UART2_TEN              ((uint8_t) (0x01 << 3))        ///< Transmitter enable [0]
  #define _UART2_ILIEN            ((uint8_t) (0x01 << 4))        ///< IDLE Line interrupt enable [0]
  #define _UART2_RIEN             ((uint8_t) (0x01 << 5))        ///< Receiver interrupt enable [0]
  #define _UART2_TCIEN            ((uint8_t) (0x01 << 6))        ///< Transmission complete interrupt enable [0]
  #define _UART2_TIEN             ((uint8_t) (0x01 << 7))        ///< Transmitter interrupt enable [0]

  /* UART2 Control register 3 (UART2_CR3) bits */
  #define _UART2_LBCL             ((uint8_t) (0x01 << 0))        ///< Last bit clock pulse [0]
  #define _UART2_CPHA             ((uint8_t) (0x01 << 1))        ///< Clock phase [0]
  #define _UART2_CPOL             ((uint8_t) (0x01 << 2))        ///< Clock polarity [0]
  #define _UART2_CKEN             ((uint8_t) (0x01 << 3))        ///< Clock enable [0]
  #define _UART2_STOP             ((uint8_t) 0x30)               ///< STOP bits [1:0]
  #define _UART2_STOP0            ((uint8_t) (0x01 << 4))        ///< STOP bits [0]
  #define _UART2_STOP1            ((uint8_t) (0x01 << 5))        ///< STOP bits [1]
  #define _UART2_LINEN            ((uint8_t) (0x01 << 6))        ///< LIN mode enable [0]
  // reserved [7]

  /* UART2 Control register 4 (UART2_CR4) bits */
  #define _UART2_ADD              ((uint8_t) 0x0F)               ///< Address of the UART node [3:0]
  #define _UART2_ADD0             ((uint8_t) (0x01 << 0))        ///< Address of the UART node [0]
  #define _UART2_ADD1             ((uint8_t) (0x01 << 1))        ///< Address of the UART node [1]
  #define _UART2_ADD2             ((uint8_t) (0x01 << 2))        ///< Address of the UART node [2]
  #define _UART2_ADD3             ((uint8_t) (0x01 << 3))        ///< Address of the UART node [3]
  #define _UART2_LBDF             ((uint8_t) (0x01 << 4))        ///< LIN Break Detection Flag [0]
  #define _UART2_LBDL             ((uint8_t) (0x01 << 5))        ///< LIN Break Detection Length [0]
  #define _UART2_LBDIEN           ((uint8_t) (0x01 << 6))        ///< LIN Break Detection Interrupt Enable [0]
  // reserved [7]

  /* UART2 Control register 5 (UART2_CR5) bits */
  // reserved [0]
  #define _UART2_IREN             ((uint8_t) (0x01 << 1))        ///< IrDA mode Enable [0]
  #define _UART2_IRLP             ((uint8_t) (0x01 << 2))        ///< IrDA Low Power [0]
  // reserved [3]
  #define _UART2_NACK             ((uint8_t) (0x01 << 4))        ///< Smartcard NACK enable [0]
  #define _UART2_SCEN             ((uint8_t) (0x01 << 5))        ///< Smartcard mode enable [0]
  // reserved [7:6]

  /* UART2 Control register 6 (UART2_CR6) bits */
  #define _UART2_LSF              ((uint8_t) (0x01 << 0))        ///< LIN Sync Field [0]
  #define _UART2_LHDF             ((uint8_t) (0x01 << 1))        ///< LIN Header Detection Flag [0]
  #define _UART2_LHDIEN           ((uint8_t) (0x01 << 2))        ///< LIN Header Detection Interrupt Enable [0]
  // reserved [3]
  #define _UART2_LASE             ((uint8_t) (0x01 << 4))        ///< LIN automatic resynchronisation enable [0]
  #define _UART2_LSLV             ((uint8_t) (0x01 << 5))        ///< LIN Slave Enable [0]
  // reserved [6]
  #define _UART2_LDUM             ((uint8_t) (0x01 << 7))        ///< LIN Divider Update Method [0]

#endif // UART2_BaseAddress



//------------------------
// 8-Bit Timer 4 (TIM4)
//------------------------
#if defined(TIM4_BaseAddress)

  /** struct containing TIM4 registers (selected devices) */
  typedef struct {

    /** TIM4 Control register (TIM4_CR) */
    struct {
      _BITS   CEN     : 1;    ///< Counter enable
      _BITS   UDIS    : 1;    ///< Update disable
      _BITS   URS     : 1;    ///< Update request source
      _BITS   OPM     : 1;    ///< One-pulse mode
      _BITS   res     : 3;    ///< Reserved
      _BITS   ARPE    : 1;    ///< Auto-reload preload enable
    } CR;


    /** Reserved registers on selected devices (2B) */
    #if defined(STM8S103) || defined(STM8S003) || defined(STM8S001)
      uint8_t   res[2];
    #endif


    /** TIM4 Interrupt enable (TIM4_IER) */
    struct {
      _BITS   UIE     : 1;    ///< Update interrupt enable
      _BITS   res     : 7;    ///< Reserved
    } IER;


    /** TIM4 Status register (TIM4_SR) */
    struct {
      _BITS   UIF     : 1;    ///< Update interrupt flag
      _BITS   res     : 7;    ///< Reserved
    } SR;


    /** TIM4 Event Generation (TIM4_EGR) */
    struct {
      _BITS   UG      : 1;    ///< Update generation
      _BITS   res     : 7;    ///< Reserved
    } EGR;


    /** TIM4 8-bit counter register (TIM4_CNTR). No bit access */
    uint8_t   CNTR;


    /** TIM4 clock prescaler (TIM4_PSCR) */
    struct {
      _BITS   PSC     : 3;    ///< clock prescaler
      _BITS   res     : 5;    ///< Reserved
    } PSCR;


    /** TIM4 8-bit auto-reload register (TIM4_ARR). No bit access */
    uint8_t   ARR;

  } TIM4_t;

  /* Pointer to TIM4 registers */
  #define _TIM4      _SFR(TIM4_t,  TIM4_BaseAddress)         ///< bitfield access to TIM4
  #define _TIM4_CR   _SFR(uint8_t, TIM4_BaseAddress+0)       ///< TIM4 control register
  #if defined(STM8S103) || defined(STM8S003)                 // 2B offset for selected devices
    // 2B reserved
    #define _TIM4_IER   _SFR(uint8_t, TIM4_BaseAddress+3)    ///< TIM4 interrupt enable register
    #define _TIM4_SR    _SFR(uint8_t, TIM4_BaseAddress+4)    ///< TIM4 status register
    #define _TIM4_EGR   _SFR(uint8_t, TIM4_BaseAddress+5)    ///< TIM4 event generation register
    #define _TIM4_CNTR  _SFR(uint8_t, TIM4_BaseAddress+6)    ///< TIM4 counter register
    #define _TIM4_PSCR  _SFR(uint8_t, TIM4_BaseAddress+7)    ///< TIM4 clock prescaler register
    #define _TIM4_ARR   _SFR(uint8_t, TIM4_BaseAddress+8)    ///< TIM4 auto-reload register
  #else
    #define _TIM4_IER   _SFR(uint8_t, TIM4_BaseAddress+1)    ///< TIM4 interrupt enable register
    #define _TIM4_SR    _SFR(uint8_t, TIM4_BaseAddress+2)    ///< TIM4 status register
    #define _TIM4_EGR   _SFR(uint8_t, TIM4_BaseAddress+3)    ///< TIM4 event generation register
    #define _TIM4_CNTR  _SFR(uint8_t, TIM4_BaseAddress+4)    ///< TIM4 counter register
    #define _TIM4_PSCR  _SFR(uint8_t, TIM4_BaseAddress+5)    ///< TIM4 clock prescaler register
    #define _TIM4_ARR   _SFR(uint8_t, TIM4_BaseAddress+6)    ///< TIM4 auto-reload register
  #endif

  /* TIM4 Module Reset Values */
  #define _TIM4_CR_RESET_VALUE      ((uint8_t)0x00)
  #define _TIM4_IER_RESET_VALUE     ((uint8_t)0x00)
  #define _TIM4_SR_RESET_VALUE      ((uint8_t)0x00)
  #define _TIM4_EGR_RESET_VALUE     ((uint8_t)0x00)
  #define _TIM4_CNTR_RESET_VALUE    ((uint8_t)0x00)
  #define _TIM4_PSCR_RESET_VALUE    ((uint8_t)0x00)
  #define _TIM4_ARR_RESET_VALUE     ((uint8_t)0xFF)

  /* TIM4 Control register (TIM4_CR) */
  #define _TIM4_CEN               ((uint8_t) (0x01 << 0))        ///< Counter enable [0]
  #define _TIM4_UDIS              ((uint8_t) (0x01 << 1))        ///< Counter enable [0]
  #define _TIM4_URS               ((uint8_t) (0x01 << 2))        ///< Counter enable [0]
  #define _TIM4_OPM               ((uint8_t) (0x01 << 3))        ///< Counter enable [0]
  // reserved [6:4]
  #define _TIM4_ARPE              ((uint8_t) (0x01 << 7))        ///< Counter enable [0]

  /*  TIM4 Interrupt enable (TIM4_IER) */
  #define _TIM4_UIE               ((uint8_t) (0x01 << 0))        ///< Update interrupt enable [0]
  // reserved [7:1]

  /*  TIM4 Status register (TIMx_SR) */
  #define _TIM4_UIF               ((uint8_t) (0x01 << 0))        ///< Trigger interrupt flag [0]
  // reserved [7:1]

  /*  TIM4 Event generation register (TIMx_EGR) */
  #define _TIM4_UG                ((uint8_t) (0x01 << 0))        ///< Update generation [0]
  // reserved [7:1]

  /* TIM4 Prescaler register (TIM4_PSCR) */
  #define _TIM4_PSC               ((uint8_t) 0x07)               ///< Prescaler value [2:0]
  #define _TIM4_PSC0              ((uint8_t) (0x01 << 0))        ///< Prescaler value [0]
  #define _TIM4_PSC1              ((uint8_t) (0x01 << 1))        ///< Prescaler value [1]
  #define _TIM4_PSC2              ((uint8_t) (0x01 << 2))        ///< Prescaler value [2]
  // reserved [7:3]

#endif // TIM4_BaseAddress


// undefine local macros
#undef _BITS

/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _STM8AF_STM8S_H_
