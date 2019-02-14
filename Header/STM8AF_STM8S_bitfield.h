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
  #define SW_RESET()             _WWDG.CR=0xBF                        ///< reset controller via WWGD module

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
  #define SW_RESET()           _WWDG.CR=0xBF                        ///< reset controller via WWGD module

  // data type in bit fields
  #define _BITS                  unsigned int                         ///< follow C90 standard


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
  #define SW_RESET()           _WWDG.CR=0xBF                        ///< reset controller via WWGD module

  // data type in bit fields
  #define _BITS                  unsigned char                      ///< unsigned int fails for unknown reason


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
  #define SW_RESET()           _WWDG.CR=0xBF                        ///< reset controller via WWGD module

  // data type in bit fields
  #define _BITS                  unsigned int                         ///< follow C90 standard

#endif


/*-----------------------------------------------------------------------------
    DEFINITION OF GLOBAL MACROS/#DEFINES
-----------------------------------------------------------------------------*/

// general macros
#define _SFR(type, addr)       (*((volatile type*) (addr)))              ///< peripheral register


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
// PORT ports (implemented on all devices)
//------------------------
#if defined(PORTA_BaseAddress) || defined(PORTB_BaseAddress) || defined(PORTC_BaseAddress) || defined(PORTD_BaseAddress) || \
    defined(PORTE_BaseAddress) || defined(PORTF_BaseAddress) || defined(PORTG_BaseAddress) || defined(PORTH_BaseAddress) || \
    defined(PORTI_BaseAddress)

  /** structure for controlling/monitoring pins in PORT mode (PORT_t) */
  typedef struct {

    /** Port x output data register (Px_ODR) */
    struct {
      _BITS    PIN0    : 1;    ///< pin 0 output control
      _BITS    PIN1    : 1;    ///< pin 1 output control
      _BITS    PIN2    : 1;    ///< pin 2 output control
      _BITS    PIN3    : 1;    ///< pin 3 output control
      _BITS    PIN4    : 1;    ///< pin 4 output control
      _BITS    PIN5    : 1;    ///< pin 5 output control
      _BITS    PIN6    : 1;    ///< pin 6 output control
      _BITS    PIN7    : 1;    ///< pin 7 output control
    } ODR;

    /** Port x input data register (Px_IDR) */
    struct {
      _BITS    PIN0    : 1;    ///< pin 0 input control
      _BITS    PIN1    : 1;    ///< pin 1 input control
      _BITS    PIN2    : 1;    ///< pin 2 input control
      _BITS    PIN3    : 1;    ///< pin 3 input control
      _BITS    PIN4    : 1;    ///< pin 4 input control
      _BITS    PIN5    : 1;    ///< pin 5 input control
      _BITS    PIN6    : 1;    ///< pin 6 input control
      _BITS    PIN7    : 1;    ///< pin 7 input control
    } IDR;

    /** Port x data direction data register (Px_DDR) */
    struct {
      _BITS    PIN0    : 1;    ///< pin 0 direction control
      _BITS    PIN1    : 1;    ///< pin 1 direction control
      _BITS    PIN2    : 1;    ///< pin 2 direction control
      _BITS    PIN3    : 1;    ///< pin 3 direction control
      _BITS    PIN4    : 1;    ///< pin 4 direction control
      _BITS    PIN5    : 1;    ///< pin 5 direction control
      _BITS    PIN6    : 1;    ///< pin 6 direction control
      _BITS    PIN7    : 1;    ///< pin 7 direction control
    } DDR;

    /** Port x control register 1 (Px_CR1) */
    struct {
      _BITS    PIN0    : 1;    ///< pin 0 control register 1
      _BITS    PIN1    : 1;    ///< pin 1 control register 1
      _BITS    PIN2    : 1;    ///< pin 2 control register 1
      _BITS    PIN3    : 1;    ///< pin 3 control register 1
      _BITS    PIN4    : 1;    ///< pin 4 control register 1
      _BITS    PIN5    : 1;    ///< pin 5 control register 1
      _BITS    PIN6    : 1;    ///< pin 6 control register 1
      _BITS    PIN7    : 1;    ///< pin 7 control register 1
    } CR1;

    /** Port x control register 1 (Px_CR2) */
    struct {
      _BITS    PIN0    : 1;    ///< pin 0 control register 2
      _BITS    PIN1    : 1;    ///< pin 1 control register 2
      _BITS    PIN2    : 1;    ///< pin 2 control register 2
      _BITS    PIN3    : 1;    ///< pin 3 control register 2
      _BITS    PIN4    : 1;    ///< pin 4 control register 2
      _BITS    PIN5    : 1;    ///< pin 5 control register 2
      _BITS    PIN6    : 1;    ///< pin 6 control register 2
      _BITS    PIN7    : 1;    ///< pin 7 control register 2
    } CR2;

  } PORT_t;

  /* pointer to port A registers */
  #if defined(PORTA_BaseAddress)
    #define _GPIOA      _SFR(PORT_t,  PORTA_BaseAddress)    ///< direct port A access
    #define _GPIOA_ODR  _SFR(uint8_t, PORTA_BaseAddress+0)  ///< port A output register
    #define _GPIOA_IDR  _SFR(uint8_t, PORTA_BaseAddress+1)  ///< port A input register
    #define _GPIOA_DDR  _SFR(uint8_t, PORTA_BaseAddress+2)  ///< port A direction register
    #define _GPIOA_CR1  _SFR(uint8_t, PORTA_BaseAddress+3)  ///< port A control register 1
    #define _GPIOA_CR2  _SFR(uint8_t, PORTA_BaseAddress+4)  ///< port A control register 2
  #endif
  
  /* pointer to port B registers */
  #if defined(PORTB_BaseAddress)
    #define _GPIOB      _SFR(PORT_t,  PORTB_BaseAddress)    ///< direct port B access
    #define _GPIOB_ODR  _SFR(uint8_t, PORTB_BaseAddress+0)  ///< port B output register
    #define _GPIOB_IDR  _SFR(uint8_t, PORTB_BaseAddress+1)  ///< port B input register
    #define _GPIOB_DDR  _SFR(uint8_t, PORTB_BaseAddress+2)  ///< port B direction register
    #define _GPIOB_CR1  _SFR(uint8_t, PORTB_BaseAddress+3)  ///< port B control register 1
    #define _GPIOB_CR2  _SFR(uint8_t, PORTB_BaseAddress+4)  ///< port B control register 2
  #endif
  
  /* pointer to port C registers */
  #if defined(PORTC_BaseAddress)
    #define _GPIOC      _SFR(PORT_t,  PORTC_BaseAddress)    ///< direct port C access
    #define _GPIOC_ODR  _SFR(uint8_t, PORTC_BaseAddress+0)  ///< port C output register
    #define _GPIOC_IDR  _SFR(uint8_t, PORTC_BaseAddress+1)  ///< port C input register
    #define _GPIOC_DDR  _SFR(uint8_t, PORTC_BaseAddress+2)  ///< port C direction register
    #define _GPIOC_CR1  _SFR(uint8_t, PORTC_BaseAddress+3)  ///< port C control register 1
    #define _GPIOC_CR2  _SFR(uint8_t, PORTC_BaseAddress+4)  ///< port C control register 2
  #endif
  
  /* pointer to port D registers */
  #if defined(PORTD_BaseAddress)
    #define _GPIOD      _SFR(PORT_t,  PORTD_BaseAddress)    ///< direct port D access
    #define _GPIOD_ODR  _SFR(uint8_t, PORTD_BaseAddress+0)  ///< port D output register
    #define _GPIOD_IDR  _SFR(uint8_t, PORTD_BaseAddress+1)  ///< port D input register
    #define _GPIOD_DDR  _SFR(uint8_t, PORTD_BaseAddress+2)  ///< port D direction register
    #define _GPIOD_CR1  _SFR(uint8_t, PORTD_BaseAddress+3)  ///< port D control register 1
    #define _GPIOD_CR2  _SFR(uint8_t, PORTD_BaseAddress+4)  ///< port D control register 2
  #endif
  
  /* pointer to port E registers */
  #if defined(PORTE_BaseAddress)
    #define _GPIOE      _SFR(PORT_t,  PORTE_BaseAddress)    ///< direct port E access
    #define _GPIOE_ODR  _SFR(uint8_t, PORTE_BaseAddress+0)  ///< port E output register
    #define _GPIOE_IDR  _SFR(uint8_t, PORTE_BaseAddress+1)  ///< port E input register
    #define _GPIOE_DDR  _SFR(uint8_t, PORTE_BaseAddress+2)  ///< port E direction register
    #define _GPIOE_CR1  _SFR(uint8_t, PORTE_BaseAddress+3)  ///< port E control register 1
    #define _GPIOE_CR2  _SFR(uint8_t, PORTE_BaseAddress+4)  ///< port E control register 2
  #endif
  
  /* pointer to port F registers */
  #if defined(PORTF_BaseAddress)
    #define _GPIOF      _SFR(PORT_t,  PORTF_BaseAddress)    ///< direct port F access
    #define _GPIOF_ODR  _SFR(uint8_t, PORTF_BaseAddress+0)  ///< port F output register
    #define _GPIOF_IDR  _SFR(uint8_t, PORTF_BaseAddress+1)  ///< port F input register
    #define _GPIOF_DDR  _SFR(uint8_t, PORTF_BaseAddress+2)  ///< port F direction register
    #define _GPIOF_CR1  _SFR(uint8_t, PORTF_BaseAddress+3)  ///< port F control register 1
    #define _GPIOF_CR2  _SFR(uint8_t, PORTF_BaseAddress+4)  ///< port F control register 2
  #endif
  
  /* pointer to port G registers */
  #if defined(PORTG_BaseAddress)
    #define _GPIOG      _SFR(PORT_t,  PORTG_BaseAddress)    ///< direct port G access
    #define _GPIOG_ODR  _SFR(uint8_t, PORTG_BaseAddress+0)  ///< port G output register
    #define _GPIOG_IDR  _SFR(uint8_t, PORTG_BaseAddress+1)  ///< port G input register
    #define _GPIOG_DDR  _SFR(uint8_t, PORTG_BaseAddress+2)  ///< port G direction register
    #define _GPIOG_CR1  _SFR(uint8_t, PORTG_BaseAddress+3)  ///< port G control register 1
    #define _GPIOG_CR2  _SFR(uint8_t, PORTG_BaseAddress+4)  ///< port G control register 2
  #endif
  
  /* pointer to port H registers */
  #if defined(PORTH_BaseAddress)
    #define _GPIOH      _SFR(PORT_t,  PORTH_BaseAddress)    ///< direct port H access
    #define _GPIOH_ODR  _SFR(uint8_t, PORTH_BaseAddress+0)  ///< port H output register
    #define _GPIOH_IDR  _SFR(uint8_t, PORTH_BaseAddress+1)  ///< port H input register
    #define _GPIOH_DDR  _SFR(uint8_t, PORTH_BaseAddress+2)  ///< port H direction register
    #define _GPIOH_CR1  _SFR(uint8_t, PORTH_BaseAddress+3)  ///< port H control register 1
    #define _GPIOH_CR2  _SFR(uint8_t, PORTH_BaseAddress+4)  ///< port H control register 2
  #endif
  
  /* pointer to port I registers */
  #if defined(PORTI_BaseAddress)
    #define _GPIOI      _SFR(PORT_t,  PORTI_BaseAddress)    ///< direct port I access
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

#endif // (1)



//------------------------
// Clock module CLK (implemented on all devices)
//------------------------
#if defined(CLK_BaseAddress)

  /** struct containing clock module registers (CLK) */
  typedef struct {

    /** Internal clock register (CLK_ICKR) */
    struct {
      _BITS    HSIEN   : 1;    ///< High speed internal RC oscillator enable
      _BITS    HSIRDY  : 1;    ///< High speed internal oscillator ready flag
      _BITS    FHW     : 1;    ///< Fast wakeup from Halt/Active-halt modes enable
      _BITS    LSIEN   : 1;    ///< Low speed internal RC oscillator enable
      _BITS    LSIRDY  : 1;    ///< Low speed internal oscillator ready flag
      _BITS    REGAH   : 1;    ///< Regulator power off in Active-halt mode enable
      _BITS    res     : 2;    ///< Reserved, must be kept cleared
    } ICKR;


    /** External clock register (CLK_ECKR) */
    struct {
      _BITS    HSEEN   : 1;    ///< High speed external crystal oscillator enable
      _BITS    HSERDY  : 1;    ///< High speed external crystal oscillator ready
      _BITS    res     : 6;    ///< Reserved, must be kept cleared
    } ECKR;


    /** Reserved register (1B) */
    uint8_t         res[1]; 


    /** Clock master status register (CLK_CMSR) */
    struct {
      _BITS    CKM     : 8;    ///< Clock master status bits
    } CMSR;


    /** Clock master switch register (CLK_SWR) */
    struct {
      _BITS    SWI     : 8;    ///< Clock master selection bits
    } SWR;


    /** Switch control register (CLK_SWCR) */
    struct {
      _BITS    SWBSY   : 1;    ///< Switch busy flag
      _BITS    SWEN    : 1;    ///< Switch start/stop enable
      _BITS    SWIEN   : 1;    ///< Clock switch interrupt enable
      _BITS    SWIF    : 1;    ///< Clock switch interrupt flag
      _BITS    res     : 4;    ///< Reserved
    } SWCR;


    /** Clock divider register (CLK_CKDIVR) */
    struct {
      _BITS    CPUDIV  : 3;    ///< CPU clock prescaler
      _BITS    HSIDIV  : 2;    ///< High speed internal clock prescaler
      _BITS    res     : 3;    ///< Reserved, must be kept cleared.
    } CKDIVR;


    /** Peripheral clock gating register 1 (CLK_PCKENR1) */
    struct {
      _BITS    PCKEN_I2C       : 1;    ///< clock enable I2C
      _BITS    PCKEN_SPI       : 1;    ///< clock enable SPI
      _BITS    PCKEN_UART1     : 1;    ///< clock enable UART1
      _BITS    PCKEN_UART2     : 1;    ///< clock enable UART2
      _BITS    PCKEN_TIM4_TIM6 : 1;    ///< clock enable TIM4/TIM6
      _BITS    PCKEN_TIM2_TIM5 : 1;    ///< clock enable TIM4/TIM6
      _BITS    PCKEN_TIM3      : 1;    ///< clock enable TIM3
      _BITS    PCKEN_TIM1      : 1;    ///< clock enable TIM1
    } PCKENR1;


    /** Clock security system register (CLK_CSSR) */
    struct {
      _BITS    CSSEN   : 1;    ///< Clock security system enable
      _BITS    AUX     : 1;    ///< Auxiliary oscillator connected to master clock
      _BITS    CSSDIE  : 1;    ///< Clock security system detection interrupt enable
      _BITS    CSSD    : 1;    ///< Clock security system detection
      _BITS    res     : 4;    ///< Reserved, must be kept cleared.
    } CSSR;


    /** Configurable clock output register (CLK_CCOR) */
    struct {
      _BITS    CCOEN   : 1;    ///< Configurable clock output enable
      _BITS    CCOSEL  : 4;    ///< Configurable clock output selection.
      _BITS    CCORDY  : 1;    ///< Configurable clock output ready
      _BITS    CCOBSY  : 1;    ///< Configurable clock output busy
      _BITS    res     : 1;    ///< Reserved, must be kept cleared.
    } CCOR;


    /** Peripheral clock gating register 2 (CLK_PCKENR2) */
    struct {
      _BITS    res       : 2;   ///< Reserved
      _BITS    PCKEN_AWU : 1;   ///< clock enable AWU
      _BITS    PCKEN_ADC : 1;   ///< clock enable ADC
      _BITS    res2      : 3;   ///< Reserved
      _BITS    PCKEN_CAN : 1;   ///< clock enable CAN
    } PCKENR2;


    /** Reserved register (1B). Was CAN clock control (obsolete as of STM8 UM rev 7) */
    uint8_t         res2[1]; 
     

    /** HSI clock calibration trimming register (CLK_HSITRIMR) */
    struct {
      _BITS    HSITRIM : 4;    ///< HSI trimming value (some devices only support 3 bits, see DS!)
      _BITS    res     : 4;    ///< Reserved, must be kept cleared.
    } HSITRIMR;


    /** SWIM clock control register (CLK_SWIMCCR) */
    struct {
      _BITS    SWIMCLK : 1;    ///< SWIM clock divider
      _BITS    res     : 7;    ///< Reserved.
    } SWIMCCR;

  } CLK_t;

  /// pointer to CLK registers
  #define _CLK          _SFR(CLK_t, CLK_BaseAddress)
  #define _CLK_ICKR     _SFR(uint8_t, CLK_BaseAddress+0)      ///< Internal clock register
  #define _CLK_ECKR     _SFR(uint8_t, CLK_BaseAddress+1)      ///< External clock register
                                                              ///< Reserved (1B)
  #define _CLK_CMSR     _SFR(uint8_t, CLK_BaseAddress+3)      ///< Clock master status register
  #define _CLK_SWR      _SFR(uint8_t, CLK_BaseAddress+4)      ///< Clock master switch register
  #define _CLK_SWCR     _SFR(uint8_t, CLK_BaseAddress+5)      ///< Switch control register
  #define _CLK_CKDIVR   _SFR(uint8_t, CLK_BaseAddress+6)      ///< Clock divider register
  #define _CLK_PCKENR1  _SFR(uint8_t, CLK_BaseAddress+7)      ///< Peripheral clock gating register 1
  #define _CLK_CSSR     _SFR(uint8_t, CLK_BaseAddress+8)      ///< Clock security system register
  #define _CLK_CCOR     _SFR(uint8_t, CLK_BaseAddress+9)      ///< Configurable clock output register
  #define _CLK_PCKENR2  _SFR(uint8_t, CLK_BaseAddress+10)     ///< Peripheral clock gating register 2
                                                              ///< Reserved (1B)
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


// undefine local macros
#undef _BITS


/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _STM8AF_STM8S_H_
