/** \addtogroup STM8AF_STM8S
 *  @{
 */

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
  #define STM8_PFLASH_SIZE      2*1024        ///< size of program flash [B]
#endif
#if !defined(STM8_RAM_SIZE)
  #warning undefined STM8_RAM_SIZE, assume minimum
  #define STM8_RAM_SIZE         1*1024        ///< size of RAM [B]
#endif
#if !defined(STM8_EEPROM_SIZE)
  #warning undefined STM8_EEPROM_SIZE, assume minimum
  #define STM8_EEPROM_SIZE      128           ///< size of data EEPROM [B]
#endif

// memory start / end addresses
#define STM8_PFLASH_START       0x8000        ///< first address in program flash
#define STM8_PFLASH_END         (STM8_PFLASH_START + STM8_PFLASH_SIZE - 1)  ///< last address in program flash
#define STM8_RAM_START          0x0000        ///< first address in RAM
#define STM8_RAM_END            (STM8_RAM_START + STM8_RAM_SIZE - 1)        ///< last address in RAM
#define STM8_EEPROM_START       0x4000        ///< first address in EEPROM
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

#define __TLI_VECTOR__            0       ///< irq0 - External Top Level interrupt (TLI) for pin PD7
#define __AWU_VECTOR__            1       ///< irq1 - Auto Wake Up from Halt interrupt (AWU)
#define __CLK_VECTOR__            2       /// irq2 - Clock Controller interrupt
#define __PORTA_VECTOR__          3       /// irq3 - External interrupt 0 (GPIOA)
#define __PORTB_VECTOR__          4       /// irq4 - External interrupt 1 (GPIOB)
#define __PORTC_VECTOR__          5       /// irq5 - External interrupt 2 (GPIOC)
#define __PORTD_VECTOR__          6       /// irq6 - External interrupt 3 (GPIOD)
#define __PORTE_VECTOR__          7       /// irq7 - External interrupt 4 (GPIOE)
#if defined(CAN_AddressBase)
  #define __CAN_RX_VECTOR__       8       ///< irq8 - CAN receive interrupt (shared with \__PORTF_VECTOR__)
#endif
#if defined(PORTF_AddressBase)
  #define __PORTF_VECTOR__        8       ///< irq8 - External interrupt 5 (GPIOF, shared with \__CAN_RX_VECTOR__)
#endif
#if defined(CAN_AddressBase)
  #define __CAN_TX_VECTOR__       9       ///< irq9 - CAN transmit interrupt
#endif
#define __SPI_VECTOR__            10      /// irq10 - SPI End of transfer interrupt
#define __TIM1_UPD_OVF_VECTOR__   11      /// irq11 - TIM1 Update/Overflow/Trigger/Break interrupt
#define __TIM1_CAPCOM_VECTOR__    12      /// irq12 - TIM1 Capture/Compare interrupt
#if defined(TIM2_AddressBase)
  #define __TIM2_UPD_OVF_VECTOR__ 13      ///< irq13 - TIM2 Update/overflow interrupt (shared with \__TIM5_UPD_OVF_VECTOR__)
#endif
#if defined(TIM5_AddressBase)
  #define __TIM5_UPD_OVF_VECTOR__ 13      ///< irq13 - TIM5 Update/overflow interrupt (shared with \__TIM2_UPD_OVF_VECTOR__)
#endif
#if defined(TIM2_AddressBase)
  #define __TIM2_CAPCOM_VECTOR__  14      ///< irq14 - TIM2 Capture/Compare interrupt (shared with \__TIM5_CAPCOM_VECTOR__)
#elif defined(TIM5_AddressBase)
  #define __TIM5_CAPCOM_VECTOR__  14      ///< irq14 - TIM5 Capture/Compare interrupt (shared with \__TIM2_CAPCOM_VECTOR__)
#endif
#if defined(TIM3_AddressBase)
  #define __TIM3_UPD_OVF_VECTOR__ 15      ///< irq15 - TIM3 Update/overflow interrupt
#endif
#if defined(TIM3_AddressBase)
  #define __TIM3_CAPCOM_VECTOR__  16      ///< irq16 - TIM3 Capture/Compare interrupt
#endif
#if defined(UART1_AddressBase)
  #define __UART1_TXE_VECTOR__    17      ///< irq17 - USART/UART1 send (TX empty) interrupt
#endif
#if defined(UART1_AddressBase)
  #define __UART1_RXF_VECTOR__    18      ///< irq18 - USART/UART1 receive (RX full) interrupt
#endif
#define __I2C_VECTOR__            19      ///< irq19 - I2C interrupt
#if defined(UART2_AddressBase)
  #define __UART2_TXE_VECTOR__    20      ///< irq20 - UART2 send (TX empty) interrupt (shared with \__UART3_TXE_VECTOR__ and \__UART4_TXE_VECTOR__)
#elif defined(UART3_AddressBase)
  #define __UART3_TXE_VECTOR__    20      ///< irq20 - UART3 send (TX empty) interrupt (shared with \__UART2_TXE_VECTOR__ and \__UART4_TXE_VECTOR__)
#elif defined(UART4_AddressBase)
  #define __UART4_TXE_VECTOR__    20      ///< irq20 - UART4 send (TX empty) interrupt (shared with \__UART2_TXE_VECTOR__ and \__UART3_TXE_VECTOR__)
#endif
#if defined(UART2_AddressBase)
  #define __UART2_RXF_VECTOR__    21      ///< irq21 - UART2 receive (RX full) interrupt (shared with \__UART3_RXF_VECTOR__ and \__UART4_RXF_VECTOR__)
#elif defined(UART3_AddressBase)
  #define __UART3_RXF_VECTOR__    21      ///< irq21 - UART3 receive (RX full) interrupt (shared with \__UART2_RXF_VECTOR__ and \__UART4_RXF_VECTOR__)
#elif defined(UART4_AddressBase)
  #define __UART4_RXF_VECTOR__    21      ///< irq21 - UART4 receive (RX full) interrupt (shared with \__UART2_RXF_VECTOR__ and \__UART3_RXF_VECTOR__)
#endif
#if defined(ADC1_AddressBase)
  #define __ADC1_VECTOR__         22      ///< irq22 - ADC1 end of conversion (shared with \__ADC2_VECTOR__)
#elif defined(ADC2_AddressBase)
  #define __ADC2_VECTOR__         22      ///< irq22 - ADC2 end of conversion & analog watchdog interrupts (shared with \__ADC1_VECTOR__)
#endif
#if defined(TIM4_AddressBase)
  #define __TIM4_UPD_OVF_VECTOR__ 23      ///< irq23 - TIM4 Update/Overflow interrupt (shared with \__TIM6_UPD_OVF_VECTOR__)
#elif defined(TIM6_AddressBase)
  #define __TIM6_UPD_OVF_VECTOR__ 23      ///< irq23 - TIM6 Update/Overflow interrupt (shared with \__TIM4_UPD_OVF_VECTOR__)
#endif
#define __FLASH_VECTOR__          24      /// irq24 - flash interrupt



/*-----------------------------------------------------------------------------
    DEFINITION OF STM8 PERIPHERAL REGISTERS
-----------------------------------------------------------------------------*/

//------------------------
// General purpose input/output pins (PORT)
//------------------------
#if defined(PORTA_AddressBase) || defined(PORTB_AddressBase) || defined(PORTC_AddressBase) || defined(PORTD_AddressBase) || defined(PORTE_AddressBase) || defined(PORTF_AddressBase) || defined(PORTG_AddressBase) || defined(PORTH_AddressBase) || defined(PORTI_AddressBase)

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

  /* Pointer to port E registers */
  #if defined(PORTE_AddressBase)
    #define _GPIOE      _SFR(PORT_t,  PORTE_AddressBase)       ///< port E struct/bit access
    #define _GPIOE_ODR  _SFR(uint8_t, PORTE_AddressBase+0x00)  ///< port E output register
    #define _GPIOE_IDR  _SFR(uint8_t, PORTE_AddressBase+0x01)  ///< port E input register
    #define _GPIOE_DDR  _SFR(uint8_t, PORTE_AddressBase+0x02)  ///< port E direction register
    #define _GPIOE_CR1  _SFR(uint8_t, PORTE_AddressBase+0x03)  ///< port E control register 1
    #define _GPIOE_CR2  _SFR(uint8_t, PORTE_AddressBase+0x04)  ///< port E control register 2
  #endif

  /* Pointer to port F registers */
  #if defined(PORTF_AddressBase)
    #define _GPIOF      _SFR(PORT_t,  PORTF_AddressBase)       ///< port F struct/bit access
    #define _GPIOF_ODR  _SFR(uint8_t, PORTF_AddressBase+0x00)  ///< port F output register
    #define _GPIOF_IDR  _SFR(uint8_t, PORTF_AddressBase+0x01)  ///< port F input register
    #define _GPIOF_DDR  _SFR(uint8_t, PORTF_AddressBase+0x02)  ///< port F direction register
    #define _GPIOF_CR1  _SFR(uint8_t, PORTF_AddressBase+0x03)  ///< port F control register 1
    #define _GPIOF_CR2  _SFR(uint8_t, PORTF_AddressBase+0x04)  ///< port F control register 2
  #endif

  /* Pointer to port G registers */
  #if defined(PORTG_AddressBase)
    #define _GPIOG      _SFR(PORT_t,  PORTG_AddressBase)       ///< port G struct/bit access
    #define _GPIOG_ODR  _SFR(uint8_t, PORTG_AddressBase+0x00)  ///< port G output register
    #define _GPIOG_IDR  _SFR(uint8_t, PORTG_AddressBase+0x01)  ///< port G input register
    #define _GPIOG_DDR  _SFR(uint8_t, PORTG_AddressBase+0x02)  ///< port G direction register
    #define _GPIOG_CR1  _SFR(uint8_t, PORTG_AddressBase+0x03)  ///< port G control register 1
    #define _GPIOG_CR2  _SFR(uint8_t, PORTG_AddressBase+0x04)  ///< port G control register 2
  #endif

  /* Pointer to port H registers */
  #if defined(PORTH_AddressBase)
    #define _GPIOH      _SFR(PORT_t,  PORTH_AddressBase)       ///< port H struct/bit access
    #define _GPIOH_ODR  _SFR(uint8_t, PORTH_AddressBase+0x00)  ///< port H output register
    #define _GPIOH_IDR  _SFR(uint8_t, PORTH_AddressBase+0x01)  ///< port H input register
    #define _GPIOH_DDR  _SFR(uint8_t, PORTH_AddressBase+0x02)  ///< port H direction register
    #define _GPIOH_CR1  _SFR(uint8_t, PORTH_AddressBase+0x03)  ///< port H control register 1
    #define _GPIOH_CR2  _SFR(uint8_t, PORTH_AddressBase+0x04)  ///< port H control register 2
  #endif

  /* Pointer to port I registers */
  #if defined(PORTI_AddressBase)
    #define _GPIOI      _SFR(PORT_t,  PORTI_AddressBase)       ///< port I struct/bit access
    #define _GPIOI_ODR  _SFR(uint8_t, PORTI_AddressBase+0x00)  ///< port I output register
    #define _GPIOI_IDR  _SFR(uint8_t, PORTI_AddressBase+0x01)  ///< port I input register
    #define _GPIOI_DDR  _SFR(uint8_t, PORTI_AddressBase+0x02)  ///< port I direction register
    #define _GPIOI_CR1  _SFR(uint8_t, PORTI_AddressBase+0x03)  ///< port I control register 1
    #define _GPIOI_CR2  _SFR(uint8_t, PORTI_AddressBase+0x04)  ///< port I control register 2
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
      _BITS   AHALT : 1;    ///< Power-down in Active-halt mode
      _BITS   HALT  : 1;    ///< Power-down in Halt mode
      _BITS   res   : 4;    ///< Reserved
    } CR1;


    /** @brief Flash control register 2 (FLASH_CR2) */
    struct {
      _BITS   PRG   : 1;    ///< Standard block programming
      _BITS   res   : 3;    ///< Reserved
      _BITS   FPRG  : 1;    ///< Fast block programming
      _BITS   ERASE : 1;    ///< Block erasing
      _BITS   WPRG  : 1;    ///< Word programming
      _BITS   OPT   : 1;    ///< Write option bytes
    } CR2;


    /** @brief Complementary flash control register 2 (FLASH_NCR2) */
    struct {
      _BITS   NPRG   : 1;   ///< Standard block programming
      _BITS   res    : 3;   ///< Reserved
      _BITS   NFPRG  : 1;   ///< Fast block programming
      _BITS   NERASE : 1;   ///< Block erasing
      _BITS   NWPRG  : 1;   ///< Word programming
      _BITS   NOPT   : 1;   ///< Write option bytes
    } NCR2;


    /** @brief Flash protection register (FLASH_FPR) */
    struct {
      _BITS   WPB    : 6;   ///< User boot code area protection bits
      _BITS   res    : 2;   ///< Reserved
    } FPR;


    /** @brief Complementary flash protection register (FLASH_NFPR) */
    struct {
      _BITS   NWPB   : 6;   ///< User boot code area protection bits
      _BITS   res    : 2;   ///< Reserved
    } NFPR;


    /** @brief Flash status register (FLASH_IAPSR) */
    struct {
      _BITS   WR_PG_DIS : 1;  ///< Write attempted to protected page flag
      _BITS   PUL       : 1;  ///< Flash Program memory unlocked flag
      _BITS   EOP       : 1;  ///< End of programming (write or erase operation) flag
      _BITS   DUL       : 1;  ///< Data EEPROM area unlocked flag
      _BITS   res       : 2;  ///< Reserved, forced by hardware to 0
      _BITS   HVOFF     : 1;  ///< End of high voltage flag
      _BITS   res2      : 1;  ///< Reserved
    } IAPSR;


    /** @brief Reserved registers (2B) */
    uint8_t   res[2];


    /** @brief Flash program memory unprotecting key register (FLASH_PUKR) */
    struct {
      _BITS   PUK    : 8;   ///< Program memory write unlock key
    } PUKR;


    /** @brief Reserved register (1B) */
    uint8_t   res2[1];


    /** @brief Data EEPROM unprotection key register (FLASH_DUKR) */
    struct {
      _BITS   DUK    : 8;   ///< Data EEPROM write unlock key
    } DUKR;

  } FLASH_t;

  /* Pointer to flash registers */
  #define _FLASH        _SFR(FLASH_t,  FLASH_AddressBase)        ///< Flash struct/bit access
  #define _FLASH_CR1    _SFR(uint8_t,  FLASH_AddressBase+0x00)   ///< Flash control register 1 (FLASH_CR1)
  #define _FLASH_CR2    _SFR(uint8_t,  FLASH_AddressBase+0x01)   ///< Flash control register 2 (FLASH_CR2)
  #define _FLASH_NCR2   _SFR(uint8_t,  FLASH_AddressBase+0x02)   ///< complementary Flash control register 2 (FLASH_NCR2)
  #define _FLASH_FPR    _SFR(uint8_t,  FLASH_AddressBase+0x03)   ///< Flash protection register (FLASH_FPR)
  #define _FLASH_NFPR   _SFR(uint8_t,  FLASH_AddressBase+0x04)   ///< complementary Flash protection register (FLASH_NFPR)
  #define _FLASH_IAPSR  _SFR(uint8_t,  FLASH_AddressBase+0x05)   ///< Flash status register (FLASH_IAPSR)
  // reserved (2B)
  #define _FLASH_PUKR   _SFR(uint8_t,  FLASH_AddressBase+0x08)   ///< Flash program memory unprotecting key register (FLASH_PUKR)
  // reserved (1B)
  #define _FLASH_DUKR   _SFR(uint8_t,  FLASH_AddressBase+0x0A)   ///< Data EEPROM unprotection key register (FLASH_DUKR)

  /* FLASH Module Reset Values */
  #define _FLASH_CR1_RESET_VALUE       ((uint8_t) 0x00)          ///< Flash control register 1 reset value
  #define _FLASH_CR2_RESET_VALUE       ((uint8_t) 0x00)          ///< Flash control register 2 reset value
  #define _FLASH_NCR2_RESET_VALUE      ((uint8_t) 0xFF)          ///< complementary Flash control register 2 reset value
  #define _FLASH_IAPSR_RESET_VALUE     ((uint8_t) 0x40)          ///< Flash status register reset value
  #define _FLASH_PUKR_RESET_VALUE      ((uint8_t) 0x00)          ///< Flash program memory unprotecting key reset value
  #define _FLASH_DUKR_RESET_VALUE      ((uint8_t) 0x00)          ///< Data EEPROM unprotection key reset value

  /* Flash control register 1 (_FLASH_CR1) */
  #define _FLASH_FIX              ((uint8_t) (0x01 << 0))        ///< Fixed Byte programming time [0] (in _FLASH_CR1)
  #define _FLASH_IE               ((uint8_t) (0x01 << 1))        ///< Flash Interrupt enable [0] (in _FLASH_CR1)
  #define _FLASH_AHALT            ((uint8_t) (0x01 << 2))        ///< Power-down in Active-halt mode [0] (in _FLASH_CR1)
  #define _FLASH_HALT             ((uint8_t) (0x01 << 3))        ///< Power-down in Halt mode [0] (in _FLASH_CR1)
  // reserved [7:4]

  /* Flash control register 2 and complement (FLASH_CR2 and FLASH_NCR2) */
  #define _FLASH_PRG              ((uint8_t) (0x01 << 0))        ///< Standard block programming [0] (in _FLASH_CR2 and _FLASH_NCR2)
  // reserved [3:1]
  #define _FLASH_FPRG             ((uint8_t) (0x01 << 4))        ///< Fast block programming [0] (in _FLASH_CR2 and _FLASH_NCR2)
  #define _FLASH_ERASE            ((uint8_t) (0x01 << 5))        ///< Block erasing [0] (in _FLASH_CR2 and _FLASH_NCR2)
  #define _FLASH_WPRG             ((uint8_t) (0x01 << 6))        ///< Word programming [0] (in _FLASH_CR2 and _FLASH_NCR2)
  #define _FLASH_OPT              ((uint8_t) (0x01 << 7))        ///< Write option bytes [0] (in _FLASH_CR2 and _FLASH_NCR2)

  /* Flash protection register and complement (FLASH_FPR and FLASH_NFPR) */
  #define _FLASH_WPB              ((uint8_t) (0x3F << 0))        ///< User boot code area protection bits [5:0] (in _FLASH_FPR and _FLASH_NFPR)
  #define _FLASH_WPB0             ((uint8_t) (0x01 << 0))        ///< User boot code area protection bit [0] (in _FLASH_FPR and _FLASH_NFPR)
  #define _FLASH_WPB1             ((uint8_t) (0x01 << 1))        ///< User boot code area protection bit [1] (in _FLASH_FPR and _FLASH_NFPR)
  #define _FLASH_WPB2             ((uint8_t) (0x01 << 2))        ///< User boot code area protection bit [2] (in _FLASH_FPR and _FLASH_NFPR)
  #define _FLASH_WPB3             ((uint8_t) (0x01 << 3))        ///< User boot code area protection bit [3] (in _FLASH_FPR and _FLASH_NFPR)
  #define _FLASH_WPB4             ((uint8_t) (0x01 << 4))        ///< User boot code area protection bit [4] (in _FLASH_FPR and _FLASH_NFPR)
  #define _FLASH_WPB5             ((uint8_t) (0x01 << 5))        ///< User boot code area protection bit [5] (in _FLASH_FPR and _FLASH_NFPR)
  // reserved [7:6]

  /* Flash status register (FLASH_IAPSR) */
  #define _FLASH_WR_PG_DIS        ((uint8_t) (0x01 << 0))        ///< Write attempted to protected page flag [0] (in _FLASH_IAPSR)
  #define _FLASH_PUL              ((uint8_t) (0x01 << 1))        ///< Flash Program memory unlocked flag [0] (in _FLASH_IAPSR)
  #define _FLASH_EOP              ((uint8_t) (0x01 << 2))        ///< End of programming (write or erase operation) flag [0] (in _FLASH_IAPSR)
  #define _FLASH_DUL              ((uint8_t) (0x01 << 3))        ///< Data EEPROM area unlocked flag [0] (in _FLASH_IAPSR)
  // reserved [4]
  #define _FLASH_HVOFF            ((uint8_t) (0x01 << 5))        ///< End of high voltage flag [0] (in _FLASH_IAPSR)
  // reserved [7:6]

#endif // FLASH_AddressBase



//------------------------
// External interrupt control (EXTI)
//------------------------
#if defined(EXTI_AddressBase)

  /** @brief struct for configuring external port interrupts (EXTI) */
  typedef struct {

    /** @brief External interrupt control register 1 (EXTI_CR1) */
    struct {
      _BITS   PAIS    : 2;    ///< Port A external interrupt sensitivity bits
      _BITS   PBIS    : 2;    ///< Port B external interrupt sensitivity bits
      _BITS   PCIS    : 2;    ///< Port C external interrupt sensitivity bits
      _BITS   PDIS    : 2;    ///< Port D external interrupt sensitivity bits
    } CR1;


    /** @brief External interrupt control register 2 (EXTI_CR2) */
    struct {
      _BITS   PEIS    : 2;    ///< Port E external interrupt sensitivity bits
      _BITS   TLIS    : 1;    ///< Top level interrupt sensitivity
      _BITS   res     : 5;    ///< Reserved
    } CR2;

  } EXTI_t;

  /* Pointer to EXTI registers */
  #define _EXTI         _SFR(EXTI_t,   EXTI_AddressBase)         ///< External interrupt struct/bit access
  #define _EXTI_CR1     _SFR(uint8_t,  EXTI_AddressBase+0x00)    ///< External interrupt control register 1 (EXTI_CR1)
  #define _EXTI_CR2     _SFR(uint8_t,  EXTI_AddressBase+0x01)    ///< External interrupt control register 2 (EXTI_CR2)

  /* EXTI Module Reset Values */
  #define _EXTI_CR1_RESET_VALUE        ((uint8_t) 0x00)          ///< External interrupt control register 1 reset value
  #define _EXTI_CR2_RESET_VALUE        ((uint8_t) 0x00)          ///< External interrupt control register 2 reset value

  /* External interrupt control register 1 (EXTI_CR1) */
  #define _EXTI_PAIS              ((uint8_t) (0x03 << 0))        ///< External interrupt sensitivity for Port A [1:0] (in _EXTI_CR1)
  #define _EXTI_PAIS0             ((uint8_t) (0x01 << 0))        ///< External interrupt sensitivity for Port A [0] (in _EXTI_CR1)
  #define _EXTI_PAIS1             ((uint8_t) (0x01 << 1))        ///< External interrupt sensitivity for Port A [1] (in _EXTI_CR1)
  #define _EXTI_PBIS              ((uint8_t) (0x03 << 2))        ///< External interrupt sensitivity for Port B [1:0] (in _EXTI_CR1)
  #define _EXTI_PBIS0             ((uint8_t) (0x01 << 2))        ///< External interrupt sensitivity for Port B [0] (in _EXTI_CR1)
  #define _EXTI_PBIS1             ((uint8_t) (0x01 << 3))        ///< External interrupt sensitivity for Port B [1] (in _EXTI_CR1)
  #define _EXTI_PCIS              ((uint8_t) (0x03 << 4))        ///< External interrupt sensitivity for Port C [1:0] (in _EXTI_CR1)
  #define _EXTI_PCIS0             ((uint8_t) (0x01 << 4))        ///< External interrupt sensitivity for Port C [0] (in _EXTI_CR1)
  #define _EXTI_PCIS1             ((uint8_t) (0x01 << 5))        ///< External interrupt sensitivity for Port C [1] (in _EXTI_CR1)
  #define _EXTI_PDIS              ((uint8_t) (0x03 << 6))        ///< External interrupt sensitivity for Port D [1:0] (in _EXTI_CR1)
  #define _EXTI_PDIS0             ((uint8_t) (0x01 << 6))        ///< External interrupt sensitivity for Port D [0] (in _EXTI_CR1)
  #define _EXTI_PDIS1             ((uint8_t) (0x01 << 7))        ///< External interrupt sensitivity for Port D [1] (in _EXTI_CR1)

  /* External interrupt control register 2 (EXTI_CR2) */
  #define _EXTI_PEIS              ((uint8_t) (0x03 << 0))        ///< Port E external interrupt sensitivity bits [1:0] (in _EXTI_CR2)
  #define _EXTI_PEIS0             ((uint8_t) (0x01 << 0))        ///< Port E external interrupt sensitivity bits [0] (in _EXTI_CR2)
  #define _EXTI_PEIS1             ((uint8_t) (0x01 << 1))        ///< Port E external interrupt sensitivity bits [1] (in _EXTI_CR2)
  #define _EXTI_TLIS              ((uint8_t) (0x01 << 2))        ///< Top level interrupt sensitivity [0] (in _EXTI_CR2)
  // reserved [7:3]

#endif // EXTI_AddressBase



//------------------------
// Reset status (RST)
//------------------------
#if defined(RST_AddressBase)

  /** @brief struct for determining reset source (RST) */
  typedef struct {

    /** @brief Reset status register (RST_SR) */
    struct {
      _BITS   WWDGF   : 1;    ///< Window Watchdog reset flag
      _BITS   IWDGF   : 1;    ///< Independent Watchdog reset flag
      _BITS   ILLOPF  : 1;    ///< Illegal opcode reset flag
      _BITS   SWIMF   : 1;    ///< SWIM reset flag
      _BITS   EMCF    : 1;    ///< EMC reset flag
      _BITS   res     : 3;    ///< Reserved
    } SR;

  } RST_t;

  /* Pointer to reset status register */
  #define _RST     _SFR(RST_t,         RST_AddressBase)          ///< Reset module struct/bit access
  #define _RST_SR  _SFR(uint8_t,       RST_AddressBase+0x00)     ///< Reset module status register (RST_SR)

  /*  Reset module status register (RST_SR) */
  #define _RST_WWDGF              ((uint8_t) (0x01 << 0))        ///< Window Watchdog reset flag [0] (in _RST_SR)
  #define _RST_IWDGF              ((uint8_t) (0x01 << 1))        ///< Independent Watchdog reset flag [0] (in _RST_SR)
  #define _RST_ILLOPF             ((uint8_t) (0x01 << 2))        ///< Illegal opcode reset flag [0] (in _RST_SR)
  #define _RST_SWIMF              ((uint8_t) (0x01 << 3))        ///< SWIM reset flag [0] (in _RST_SR)
  #define _RST_EMCF               ((uint8_t) (0x01 << 4))        ///< EMC reset flag [0] (in _RST_SR)
  // reserved [7:5]

#endif // RST_AddressBase



//------------------------
// Clock control (CLK)
//------------------------
#if defined(CLK_AddressBase)

  /** @brief struct for configuring/monitoring clock module (CLK) */
  typedef struct {

    /** @brief Internal clock register (CLK_ICKR) */
    struct {
      _BITS   HSIEN   : 1;    ///< High speed internal RC oscillator enable
      _BITS   HSIRDY  : 1;    ///< High speed internal oscillator ready flag
      _BITS   FHWU    : 1;    ///< Fast wakeup from Halt/Active-halt modes enable
      _BITS   LSIEN   : 1;    ///< Low speed internal RC oscillator enable
      _BITS   LSIRDY  : 1;    ///< Low speed internal oscillator ready flag
      _BITS   REGAH   : 1;    ///< Regulator power off in Active-halt mode enable
      _BITS   res     : 2;    ///< Reserved, must be kept cleared
    } ICKR;


    /** @brief External clock register (CLK_ECKR) */
    struct {
      _BITS   HSEEN   : 1;    ///< High speed external crystal oscillator enable
      _BITS   HSERDY  : 1;    ///< High speed external crystal oscillator ready
      _BITS   res     : 6;    ///< Reserved, must be kept cleared
    } ECKR;


    /** @brief Reserved register (1B) */
    uint8_t   res[1];


    /** @brief Clock master status register (CLK_CMSR) */
    struct {
      _BITS   CKM     : 8;    ///< Clock master status bits
    } CMSR;


    /** @brief Clock master switch register (CLK_SWR) */
    struct {
      _BITS   SWI     : 8;    ///< Clock master selection bits
    } SWR;


    /** @brief Switch control register (CLK_SWCR) */
    struct {
      _BITS   SWBSY   : 1;    ///< Switch busy flag
      _BITS   SWEN    : 1;    ///< Switch start/stop enable
      _BITS   SWIEN   : 1;    ///< Clock switch interrupt enable
      _BITS   SWIF    : 1;    ///< Clock switch interrupt flag
      _BITS   res     : 4;    ///< Reserved
    } SWCR;


    /** @brief Clock divider register (CLK_CKDIVR) */
    struct {
      _BITS   CPUDIV  : 3;    ///< CPU clock prescaler
      _BITS   HSIDIV  : 2;    ///< High speed internal clock prescaler
      _BITS   res     : 3;    ///< Reserved, must be kept cleared.
    } CKDIVR;


    /** @brief Peripheral clock gating register 1 (CLK_PCKENR1) */
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


    /** @brief Clock security system register (CLK_CSSR) */
    struct {
      _BITS   CSSEN   : 1;    ///< Clock security system enable
      _BITS   AUX     : 1;    ///< Auxiliary oscillator connected to master clock
      _BITS   CSSDIE  : 1;    ///< Clock security system detection interrupt enable
      _BITS   CSSD    : 1;    ///< Clock security system detection
      _BITS   res     : 4;    ///< Reserved, must be kept cleared.
    } CSSR;


    /** @brief Configurable clock output register (CLK_CCOR) */
    struct {
      _BITS   CCOEN   : 1;    ///< Configurable clock output enable
      _BITS   CCOSEL  : 4;    ///< Configurable clock output selection.
      _BITS   CCORDY  : 1;    ///< Configurable clock output ready
      _BITS   CCOBSY  : 1;    ///< Configurable clock output busy
      _BITS   res     : 1;    ///< Reserved, must be kept cleared.
    } CCOR;


    /** @brief Peripheral clock gating register 2 (CLK_PCKENR2) */
    struct {
      _BITS   res       : 2;    ///< Reserved
      _BITS   PCKEN_AWU : 1;    ///< clock enable AWU
      _BITS   PCKEN_ADC : 1;    ///< clock enable ADC
      _BITS   res2      : 3;    ///< Reserved
      _BITS   PCKEN_CAN : 1;    ///< clock enable CAN
    } PCKENR2;


    /** @brief Reserved register (1B). Was CAN clock control (obsolete as of STM8 UM rev 7) */
    uint8_t   res2[1];


    /** @brief HSI clock calibration trimming register (CLK_HSITRIMR) */
    struct {
      _BITS   HSITRIM : 4;    ///< HSI trimming value (some devices only support 3 bits, see DS!)
      _BITS   res     : 4;    ///< Reserved, must be kept cleared.
    } HSITRIMR;


    /** @brief SWIM clock control register (CLK_SWIMCCR) */
    struct {
      _BITS   SWIMCLK : 1;    ///< SWIM clock divider
      _BITS   res     : 7;    ///< Reserved.
    } SWIMCCR;

  } CLK_t;

  /* Pointer to CLK registers */
  #define _CLK          _SFR(CLK_t,    CLK_AddressBase)          ///< Clock module struct/bit access
  #define _CLK_ICKR     _SFR(uint8_t,  CLK_AddressBase+0x00)     ///< Internal clock register
  #define _CLK_ECKR     _SFR(uint8_t,  CLK_AddressBase+0x01)     ///< External clock register
  // reserved (1B)
  #define _CLK_CMSR     _SFR(uint8_t,  CLK_AddressBase+0x03)     ///< Clock master status register
  #define _CLK_SWR      _SFR(uint8_t,  CLK_AddressBase+0x04)     ///< Clock master switch register
  #define _CLK_SWCR     _SFR(uint8_t,  CLK_AddressBase+0x05)     ///< Clock switch control register
  #define _CLK_CKDIVR   _SFR(uint8_t,  CLK_AddressBase+0x06)     ///< Clock divider register
  #define _CLK_PCKENR1  _SFR(uint8_t,  CLK_AddressBase+0x07)     ///< Peripheral clock gating register 1
  #define _CLK_CSSR     _SFR(uint8_t,  CLK_AddressBase+0x08)     ///< Clock security system register
  #define _CLK_CCOR     _SFR(uint8_t,  CLK_AddressBase+0x09)     ///< Configurable clock output register
  #define _CLK_PCKENR2  _SFR(uint8_t,  CLK_AddressBase+0x0A)     ///< Peripheral clock gating register 2
  // reserved (1B)
  #define _CLK_HSITRIMR _SFR(uint8_t,  CLK_AddressBase+0x0C)     ///< HSI clock calibration trimming register
  #define _CLK_SWIMCCR  _SFR(uint8_t,  CLK_AddressBase+0x0D)     ///< SWIM clock control register

  /* CLK Module Reset Values */
  #define _CLK_ICKR_RESET_VALUE        ((uint8_t) 0x01)          ///< Internal clock register reset value
  #define _CLK_ECKR_RESET_VALUE        ((uint8_t) 0x00)          ///< External clock register reset value
  #define _CLK_CMSR_RESET_VALUE        ((uint8_t) 0xE1)          ///< Clock master status reset value
  #define _CLK_SWR_RESET_VALUE         ((uint8_t) 0xE1)          ///< Clock master switch reset value
  #define _CLK_SWCR_RESET_VALUE        ((uint8_t) 0x00)          ///< Clock switch control reset value
  #define _CLK_CKDIVR_RESET_VALUE      ((uint8_t) 0x18)          ///< Clock divider register reset value
  #define _CLK_PCKENR1_RESET_VALUE     ((uint8_t) 0xFF)          ///< Peripheral clock gating register 1 reset value
  #define _CLK_PCKENR2_RESET_VALUE     ((uint8_t) 0xFF)          ///< Peripheral clock gating register 2 reset value
  #define _CLK_CSSR_RESET_VALUE        ((uint8_t) 0x00)          ///< Clock security system register reset value
  #define _CLK_CCOR_RESET_VALUE        ((uint8_t) 0x00)          ///< Configurable clock output register reset value
  #define _CLK_HSITRIMR_RESET_VALUE    ((uint8_t) 0x00)          ///< HSI clock calibration trimming register reset value
  #define _CLK_SWIMCCR_RESET_VALUE     ((uint8_t) 0x00)          ///< SWIM clock control register reset value

  /* Internal clock register (CLK_ICKR) bits */
  #define _CLK_HSIEN              ((uint8_t) (0x01 << 0))        ///< High speed internal RC oscillator enable [0] (in _CLK_ICKR)
  #define _CLK_HSIRDY             ((uint8_t) (0x01 << 1))        ///< High speed internal oscillator ready [0] (in _CLK_ICKR)
  #define _CLK_FHWU               ((uint8_t) (0x01 << 2))        ///< Fast wakeup from Halt/Active-halt modes [0] (in _CLK_ICKR)
  #define _CLK_LSIEN              ((uint8_t) (0x01 << 3))        ///< Low speed internal RC oscillator enable [0] (in _CLK_ICKR)
  #define _CLK_LSIRDY             ((uint8_t) (0x01 << 4))        ///< Low speed internal oscillator ready [0] (in _CLK_ICKR)
  #define _CLK_REGAH              ((uint8_t) (0x01 << 5))        ///< Regulator power off in Active-halt mode [0] (in _CLK_ICKR)
  // reserved [7:6]

  /* External clock register (CLK_ECKR) */
  #define _CLK_HSEEN              ((uint8_t) (0x01 << 0))        ///< High speed external crystal oscillator enable [0] (in _CLK_ECKR)
  #define _CLK_ECKR_HSERDY        ((uint8_t) (0x01 << 1))        ///< High speed external crystal oscillator ready [0] (in _CLK_ECKR)
  // reserved [7:2]

  /* Clock master switch register (CLK_SWR) */
  #define _CLK_SWI_HSI            ((uint8_t) 0xE1)               ///< write to CLK_SWR for HSI clock (in _CLK_SWR)
  #define _CLK_SWI_LSI            ((uint8_t) 0xD2)               ///< write to CLK_SWR for LSI clock (in _CLK_SWR)
  #define _CLK_SWI_HSE            ((uint8_t) 0xB4)               ///< write to CLK_SWR for HSE clock (in _CLK_SWR)

  /* Switch control register (CLK_SWCR) */
  #define _CLK_SWBSY              ((uint8_t) (0x01 << 0))        ///< Switch busy flag [0] (in _CLK_SWCR)
  #define _CLK_SWEN               ((uint8_t) (0x01 << 1))        ///< Switch start/stop enable [0] (in _CLK_SWCR)
  #define _CLK_SWIEN              ((uint8_t) (0x01 << 2))        ///< Clock switch interrupt enable [0] (in _CLK_SWCR)
  #define _CLK_SWIF               ((uint8_t) (0x01 << 3))        ///< Clock switch interrupt flag [0] (in _CLK_SWCR)
  // reserved [7:4]

  /* Clock divider register (CLK_CKDIVR) */
  #define _CLK_CPUDIV             ((uint8_t) (0x07 << 0))        ///< CPU clock prescaler [2:0] (in _CLK_CKDIVR)
  #define _CLK_CPUDIV0            ((uint8_t) (0x01 << 0))        ///< CPU clock prescaler [0] (in _CLK_CKDIVR)
  #define _CLK_CPUDIV1            ((uint8_t) (0x01 << 1))        ///< CPU clock prescaler [1] (in _CLK_CKDIVR)
  #define _CLK_CPUDIV2            ((uint8_t) (0x01 << 2))        ///< CPU clock prescaler [2] (in _CLK_CKDIVR)
  #define _CLK_HSIDIV             ((uint8_t) (0x03 << 3))        ///< High speed internal clock prescaler [1:0] (in _CLK_CKDIVR)
  #define _CLK_HSIDIV0            ((uint8_t) (0x01 << 3))        ///< High speed internal clock prescaler [0] (in _CLK_CKDIVR)
  #define _CLK_HSIDIV1            ((uint8_t) (0x01 << 4))        ///< High speed internal clock prescaler [1] (in _CLK_CKDIVR)
  // reserved [7:5]

  /* Peripheral clock gating register 1 (CLK_PCKENR1) */
  #define _CLK_I2C                ((uint8_t) (0x01 << 0))        ///< clock enable I2C [0] (in _CLK_PCKENR1)
  #define _CLK_SPI                ((uint8_t) (0x01 << 1))        ///< clock enable SPI [0] (in _CLK_PCKENR1)
  #define _CLK_UART1              ((uint8_t) (0x01 << 2))        ///< clock enable UART1 [0] (in _CLK_PCKENR1)
  #define _CLK_UART2              ((uint8_t) (0x01 << 3))        ///< clock enable UART2 [0] (in _CLK_PCKENR1)
  #define _CLK_TIM4_TIM6          ((uint8_t) (0x01 << 4))        ///< clock enable TIM4/TIM6 [0] (in _CLK_PCKENR1)
  #define _CLK_TIM2_TIM5          ((uint8_t) (0x01 << 5))        ///< clock enable TIM2/TIM5 [0] (in _CLK_PCKENR1)
  #define _CLK_TIM3               ((uint8_t) (0x01 << 6))        ///< clock enable TIM3 [0] (in _CLK_PCKENR1)
  #define _CLK_TIM1               ((uint8_t) (0x01 << 7))        ///< clock enable TIM1 [0] (in _CLK_PCKENR1)

  /* Clock security system register (CLK_CSSR) */
  #define _CLK_CSSEN              ((uint8_t) (0x01 << 0))        ///< Clock security system enable [0] (in _CLK_CSSR)
  #define _CLK_AUX                ((uint8_t) (0x01 << 1))        ///< Auxiliary oscillator connected to master clock [0] (in _CLK_CSSR)
  #define _CLK_CSSDIE             ((uint8_t) (0x01 << 2))        ///< Clock security system detection interrupt enable [0] (in _CLK_CSSR)
  #define _CLK_CSSD               ((uint8_t) (0x01 << 3))        ///< Clock security system detection [0] (in _CLK_CSSR)
  // reserved [7:4]

  /* Configurable clock output register (CLK_CCOR) */
  #define _CLK_CCOEN              ((uint8_t) (0x01 << 0))        ///< Configurable clock output enable [0] (in _CLK_CCOR)
  #define _CLK_CCOSEL             ((uint8_t) (0x0F << 1))        ///< Configurable clock output selection [3:0] (in _CLK_CCOR)
  #define _CLK_CCOSEL0            ((uint8_t) (0x01 << 1))        ///< Configurable clock output selection [0] (in _CLK_CCOR)
  #define _CLK_CCOSEL1            ((uint8_t) (0x01 << 2))        ///< Configurable clock output selection [1] (in _CLK_CCOR)
  #define _CLK_CCOSEL2            ((uint8_t) (0x01 << 3))        ///< Configurable clock output selection [2] (in _CLK_CCOR)
  #define _CLK_CCOSEL3            ((uint8_t) (0x01 << 4))        ///< Configurable clock output selection [3] (in _CLK_CCOR)
  #define _CLK_CCORDY             ((uint8_t) (0x01 << 5))        ///< Configurable clock output ready [0] (in _CLK_CCOR)
  #define _CLK_CCOBSY             ((uint8_t) (0x01 << 6))        ///< Configurable clock output busy [0] (in _CLK_CCOR)
  // reserved [7]

  /* Peripheral clock gating register 2 (CLK_PCKENR2) */
  // reserved [1:0]
  #define _CLK_AWU                ((uint8_t) (0x01 << 2))        ///< clock enable AWU [0] (in _CLK_PCKENR2)
  #define _CLK_ADC                ((uint8_t) (0x01 << 3))        ///< clock enable ADC [0] (in _CLK_PCKENR2)
  // reserved [6:4]
  #define _CLK_CAN                ((uint8_t) (0x01 << 7))        ///< clock enable CAN [0] (in _CLK_PCKENR2)

  /* HSI clock calibration trimming register (CLK_HSITRIMR) */
  #define _CLK_HSITRIM            ((uint8_t) (0x0F << 0))        ///< HSI trimming value (some devices only support 3 bits, see DS!) [3:0] (in _CLK_HSITRIMR)
  #define _CLK_HSITRIM0           ((uint8_t) (0x01 << 0))        ///< HSI trimming value [0] (in _CLK_HSITRIMR)
  #define _CLK_HSITRIM1           ((uint8_t) (0x01 << 1))        ///< HSI trimming value [1] (in _CLK_HSITRIMR)
  #define _CLK_HSITRIM2           ((uint8_t) (0x01 << 2))        ///< HSI trimming value [2] (in _CLK_HSITRIMR)
  #define _CLK_HSITRIM3           ((uint8_t) (0x01 << 3))        ///< HSI trimming value [3] (in _CLK_HSITRIMR)
  // reserved [7:4]

  /* SWIM clock control register (CLK_SWIMCCR) */
  #define _CLK_SWIMCLK            ((uint8_t) (0x01 << 0))        ///< SWIM clock divider [0] (in _CLK_SWIMCCR)
  // reserved [7:2]

#endif // CLK_AddressBase



//------------------------
// Window Watchdog (WWDG)
//------------------------
#if defined(WWDG_AddressBase)

  /** @brief struct for access to Window Watchdog registers (WWDG) */
  typedef struct {

    /** @brief WWDG Control register (WWDG_CR) */
    struct {
      _BITS   T       : 7;    ///< 7-bit WWDG counter
      _BITS   WDGA    : 1;    ///< WWDG activation bit (n/a if WWDG enabled by option byte)
    } CR;


    /** @brief WWDR Window register (WWDG_WR) */
    struct {
      _BITS   W       : 7;    ///< 7-bit window value
      _BITS   res     : 1;    ///< Reserved
    } WR;

  } WWDG_t;

  /* Pointer to Window Watchdog registers */
  #define _WWDG         _SFR(WWDG_t,   WWDG_AddressBase)         ///< Window Watchdog struct/bit access
  #define _WWDG_CR      _SFR(uint8_t,  WWDG_AddressBase+0x00)    ///< Window Watchdog Control register (WWDG_CR)
  #define _WWDG_WR      _SFR(uint8_t,  WWDG_AddressBase+0x01)    ///< Window Watchdog Window register (WWDG_WR)

  /* WWDG Module Reset Values */
  #define _WWDG_CR_RESET_VALUE         ((uint8_t) 0x7F)          ///< Window Watchdog Control register reset value
  #define _WWDG_WR_RESET_VALUE         ((uint8_t) 0x7F)          ///< Window Watchdog Window register reset value

  /* WWDG Control register (WWDG_CR) */
  #define _WWDG_T                 ((uint8_t) (0x7F << 0))        ///< Window Watchdog 7-bit counter [6:0] (in _WWDG_CR)
  #define _WWDG_T0                ((uint8_t) (0x01 << 0))        ///< Window Watchdog 7-bit counter [0] (in _WWDG_CR)
  #define _WWDG_T1                ((uint8_t) (0x01 << 1))        ///< Window Watchdog 7-bit counter [1] (in _WWDG_CR)
  #define _WWDG_T2                ((uint8_t) (0x01 << 2))        ///< Window Watchdog 7-bit counter [2] (in _WWDG_CR)
  #define _WWDG_T3                ((uint8_t) (0x01 << 3))        ///< Window Watchdog 7-bit counter [3] (in _WWDG_CR)
  #define _WWDG_T4                ((uint8_t) (0x01 << 4))        ///< Window Watchdog 7-bit counter [4] (in _WWDG_CR)
  #define _WWDG_T5                ((uint8_t) (0x01 << 5))        ///< Window Watchdog 7-bit counter [5] (in _WWDG_CR)
  #define _WWDG_T6                ((uint8_t) (0x01 << 6))        ///< Window Watchdog 7-bit counter [6] (in _WWDG_CR)
  #define _WWDG_WDGA              ((uint8_t) (0x01 << 7))        ///< Window Watchdog activation bit (n/a if WWDG enabled by option byte) [0] (in _WWDG_CR)

  /* WWDR Window register (WWDG_WR) */
  #define _WWDG_W                 ((uint8_t) (0x7F << 0))        ///< Window Watchdog 7-bit window value [6:0] (in _WWDG_WR)
  #define _WWDG_W0                ((uint8_t) (0x01 << 0))        ///< Window Watchdog 7-bit window value [0] (in _WWDG_WR)
  #define _WWDG_W1                ((uint8_t) (0x01 << 1))        ///< Window Watchdog 7-bit window value [1] (in _WWDG_WR)
  #define _WWDG_W2                ((uint8_t) (0x01 << 2))        ///< Window Watchdog 7-bit window value [2] (in _WWDG_WR)
  #define _WWDG_W3                ((uint8_t) (0x01 << 3))        ///< Window Watchdog 7-bit window value [3] (in _WWDG_WR)
  #define _WWDG_W4                ((uint8_t) (0x01 << 4))        ///< Window Watchdog 7-bit window value [4] (in _WWDG_WR)
  #define _WWDG_W5                ((uint8_t) (0x01 << 5))        ///< Window Watchdog 7-bit window value [5] (in _WWDG_WR)
  #define _WWDG_W6                ((uint8_t) (0x01 << 6))        ///< Window Watchdog 7-bit window value [6] (in _WWDG_WR)
  // reserved [7]

#endif // WWDG_AddressBase



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
      _BITS   res     : 5;  ///< Reserved
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

  /* IWDG Key register (IWDG_KR) keys */
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
      _BITS   res     : 3;    ///< Reserved
      _BITS   AWUEN   : 1;    ///< Auto-wakeup enable
      _BITS   AWUF    : 1;    ///< Auto-wakeup flag
      _BITS   res2    : 2;    ///< Reserved
    } CSR;


    /** @brief AWU Asynchronous prescaler register (AWU_APR) */
    struct {
      _BITS   APRE    : 6;    ///< Asynchronous prescaler divider
      _BITS   res     : 2;    ///< Reserved
    } APR;


    /** @brief AWU Timebase selection register (AWU_TBR) */
    struct {
      _BITS   AWUTB   : 4;    ///< Auto-wakeup timebase selection
      _BITS   res     : 4;    ///< Reserved
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
      _BITS   BEEPSEL : 2;    ///< Beeper frequency selectiouint8_tn
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
      _BITS   res     : 1;    ///< Reserved
      _BITS   CRCNEXT : 1;    ///< Transmit CRC next
      _BITS   CRCEN   : 1;    ///< Hardware CRC calculation enable
      _BITS   BDOE    : 1;    ///< Input/Output enable in bidirectional mode
      _BITS   BDM     : 1;    ///< Bidirectional data mode enable
    } CR2;


    /** @brief SPI interrupt control register (SPI_ICR) */
    struct {
      _BITS   res     : 4;    ///< Reserved
      _BITS   WKIE    : 1;    ///< Wakeup interrupt enable
      _BITS   ERRIE   : 1;    ///< Error interrupt enable
      _BITS   RXIE    : 1;    ///< Rx buffer not empty interrupt enable
      _BITS   TXIE    : 1;    ///< Tx buffer empty interrupt enable
    } ICR;


    /** @brief SPI status register (SPI_SR) */
    struct {
      _BITS   RXNE    : 1;    ///< Receive buffer not empty
      _BITS   TXE     : 1;    ///< Transmit buffer empty
      _BITS   res     : 1;    ///< Reserved
      _BITS   WKUP    : 1;    ///< Wakeup flag
      _BITS   CRCERR  : 1;    ///< CRC error flag
      _BITS   MODF    : 1;    ///< Mode fault
      _BITS   OVR     : 1;    ///< Overrun flag
      _BITS   BSY     : 1;    ///< Busy flag
    } SR;


    /** @brief SPI data register (SPI_DR) */
    struct {
      _BITS   DATA    : 8;    ///< SPI data
    } DR;


    /** @brief SPI CRC polynomial register (SPI_CRCPR) */
    struct {
      _BITS   CRCPOLY : 8;    ///< CRC polynomial register
    } CRCPR;


    /** @brief SPI Rx CRC register (SPI_RXCRCR) */
    struct {
      _BITS   RxCRC   : 8;    ///< Rx CRC Register
    } RXCRCR;


    /** @brief SPI Tx CRC register (SPI_TXCRCR) */
    struct {
      _BITS   TxCRC   : 8;    ///< Tx CRC register
    } TXCRCR;

  } SPI_t;

  /// register for SPI control
  #define _SPI        _SFR(SPI_t,      SPI_AddressBase)          ///< SPI struct/bit access
  #define _SPI_CR1    _SFR(uint8_t,    SPI_AddressBase+0x00)     ///< SPI control register 1
  #define _SPI_CR2    _SFR(uint8_t,    SPI_AddressBase+0x01)     ///< SPI control register 2
  #define _SPI_ICR    _SFR(uint8_t,    SPI_AddressBase+0x02)     ///< SPI interrupt control register
  #define _SPI_SR     _SFR(uint8_t,    SPI_AddressBase+0x03)     ///< SPI status register
  #define _SPI_DR     _SFR(uint8_t,    SPI_AddressBase+0x04)     ///< SPI data register
  #define _SPI_CRCPR  _SFR(uint8_t,    SPI_AddressBase+0x05)     ///< SPI CRC polynomial register
  #define _SPI_RXCRCR _SFR(uint8_t,    SPI_AddressBase+0x06)     ///< SPI Rx CRC register
  #define _SPI_TXCRCR _SFR(uint8_t,    SPI_AddressBase+0x07)     ///< SPI Tx CRC register

  /* SPI Module Reset Values */
  #define _SPI_CR1_RESET_VALUE         ((uint8_t) 0x00)          ///< SPI Control Register 1 reset value
  #define _SPI_CR2_RESET_VALUE         ((uint8_t) 0x00)          ///< SPI Control Register 2 reset value
  #define _SPI_ICR_RESET_VALUE         ((uint8_t) 0x00)          ///< SPI Interrupt Control Register reset value
  #define _SPI_SR_RESET_VALUE          ((uint8_t) 0x02)          ///< SPI Status Register reset value
  #define _SPI_DR_RESET_VALUE          ((uint8_t) 0x00)          ///< SPI Data Register reset value
  #define _SPI_CRCPR_RESET_VALUE       ((uint8_t) 0x07)          ///< SPI Polynomial Register reset value
  #define _SPI_RXCRCR_RESET_VALUE      ((uint8_t) 0x00)          ///< SPI RX CRC Register reset value
  #define _SPI_TXCRCR_RESET_VALUE      ((uint8_t) 0x00)          ///< SPI TX CRC Register reset value

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
  // reserved [3]
  #define _SPI_CRCNEXT          ((uint8_t) (0x01 << 4))          ///< SPI Transmit CRC next [0] (in _SPI_CR2)
  #define _SPI_CRCEN            ((uint8_t) (0x01 << 5))          ///< SPI Hardware CRC calculation enable [0] (in _SPI_CR2)
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
  #define _SPI_CRCERR           ((uint8_t) (0x01 << 4))          ///< SPI CRC error flag [0] (in _SPI_SR)
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
      _BITS   res       : 5;    ///< Reserved
      _BITS   ENGC      : 1;    ///< General call enable
      _BITS   NOSTRETCH : 1;    ///< Clock stretching disable (Slave mode)
    } CR1;


    /** @brief I2C Control register 2 (I2C_CR2) */
    struct {
      _BITS   START     : 1;    ///< Start generation
      _BITS   STOP      : 1;    ///< Stop generation
      _BITS   ACK       : 1;    ///< Acknowledge enable
      _BITS   POS       : 1;    ///< Acknowledge position (for data reception)
      _BITS   res       : 3;    ///< Reserved
      _BITS   SWRST     : 1;    ///< Software reset
    } CR2;


    /** @brief I2C Frequency register (I2C_FREQR) */
    struct {
      _BITS   FREQ      : 6;    ///< Peripheral clock frequency
      _BITS   res       : 2;    ///< Reserved
    } FREQR;


    /** @brief I2C own address register low byte (I2C_OARL) */
    struct {
      _BITS   ADD0      : 1;    ///< Interface address [0] (in 10-bit address mode)
      _BITS   ADD       : 7;    ///< Interface address [7:1]
    } OARL;


    /** @brief I2C own address register high byte (I2C_OARH) */
    struct {
      _BITS   res       : 1;    ///< Reserved
      _BITS   ADD       : 2;    ///< Interface address [9:8] (in 10-bit address mode)
      _BITS   res2      : 3;    ///< Reserved
      _BITS   ADDCONF   : 1;    ///< Address mode configuration (must always be written as ‘1’)
      _BITS   ADDMODE   : 1;    ///< 7-/10-bit addressing mode (Slave mode)
    } OARH;


    /** @brief Reserved register (1B) */
    uint8_t         res[1];


    /** @brief I2C data register (I2C_DR) */
    struct {
      _BITS   DATA    : 8;    ///< I2C data
    } DR;


    /** @brief I2C Status register 1 (I2C_SR1) */
    struct {
      _BITS   SB        : 1;    ///< Start bit (Mastermode)
      _BITS   ADDR      : 1;    ///< Address sent (master mode) / matched (slave mode)
      _BITS   BTF       : 1;    ///< Byte transfer finished
      _BITS   ADD10     : 1;    ///< 10-bit header sent (Master mode)
      _BITS   STOPF     : 1;    ///< Stop detection (Slave mode)
      _BITS   res       : 1;    ///< Reserved
      _BITS   RXNE      : 1;    ///< Data register not empty (receivers)
      _BITS   TXE       : 1;    ///< Data register empty (transmitters)
    } SR1;


    /** @brief I2C Status register 2 (I2C_SR2) */
    struct {
      _BITS   BERR      : 1;    ///< Bus error
      _BITS   ARLO      : 1;    ///< Arbitration lost (master mode)
      _BITS   AF        : 1;    ///< Acknowledge failure
      _BITS   OVR       : 1;    ///< Overrun/underrun
      _BITS   res       : 1;    ///< Reserved
      _BITS   WUFH      : 1;    ///< Wakeup from Halt
      _BITS   res2      : 2;    ///< Reserved
    } SR2;


    /** @brief I2C Status register 3 (I2C_SR3) */
    struct {
      _BITS   MSL       : 1;    ///< Master/Slave
      _BITS   BUSY      : 1;    ///< Bus busy
      _BITS   TRA       : 1;    ///< Transmitter/Receiver
      _BITS   res       : 1;    ///< Reserved
      _BITS   GENCALL   : 1;    ///< General call header (Slavemode)
      _BITS   res2      : 3;    ///< Reserved
    } SR3;


    /** @brief I2C Interrupt register (I2C_ITR) */
    struct {
      _BITS   ITERREN   : 1;    ///< Error interrupt enable
      _BITS   ITEVTEN   : 1;    ///< Event interrupt enable
      _BITS   ITBUFEN   : 1;    ///< Buffer interrupt enable
      _BITS   res       : 5;    ///< Reserved
    } ITR;


    /** @brief I2C Clock control register low byte (I2C_CCRL) */
    struct {
      _BITS   CCR       : 8;    ///< Clock control register (Master mode)
    } CCRL;


    /** @brief I2C Clock control register high byte (I2C_CCRH) */
    struct {
      _BITS   CCR       : 4;    ///< Clock control register in Fast/Standard mode (Master mode)
      _BITS   res       : 2;    ///< Reserved
      _BITS   DUTY      : 1;    ///< Fast mode duty cycle
      _BITS   FS        : 1;    ///< I2C master mode selection
    } CCRH;


    /** @brief I2C rise time register (I2C_TRISER) */
    struct {
      _BITS   TRISE     : 6;    ///< Maximum rise time in Fast/Standard mode (Master mode)
      _BITS   res       : 2;    ///< Reserved
    } TRISER;


    /** @brief Reserved register (1B). Was I2C packet error checking (undocumented in STM8 UM rev 9) */
    uint8_t         res2[1];

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
  #define _I2C_SB              ((uint8_t) (0x01 << 0))           ///< I2C Start bit (Mastermode) [0] (in _I2C_SR1)
  #define _I2C_ADDR            ((uint8_t) (0x01 << 1))           ///< I2C Address sent (master mode) / matched (slave mode) [0] (in _I2C_SR1)
  #define _I2C_BTF             ((uint8_t) (0x01 << 2))           ///< I2C Byte transfer finished [0] (in _I2C_SR1)
  #define _I2C_ADD10           ((uint8_t) (0x01 << 3))           ///< I2C 10-bit header sent (Master mode) [0] (in _I2C_SR1)
  #define _I2C_STOPF           ((uint8_t) (0x01 << 4))           ///< I2C Stop detection (Slave mode) [0] (in _I2C_SR1)
  // reserved [5]
  #define _I2C_RXNE            ((uint8_t) (0x01 << 6))           ///< I2C Data register not empty (receivers) [0] (in _I2C_SR1)
  #define _I2C_TXE             ((uint8_t) (0x01 << 7))           ///< I2C Data register empty (transmitters) [0] (in _I2C_SR1)

  /* I2C Status register 2 (I2C_SR2) */
  #define _I2C_BERR            ((uint8_t) (0x01 << 0))           ///< I2C Bus error [0] (in _I2C_SR2)
  #define _I2C_ARLO            ((uint8_t) (0x01 << 1))           ///< I2C Arbitration lost (master mode) [0] (in _I2C_SR2)
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
  #define _I2C_GENCALL         ((uint8_t) (0x01 << 4))           ///< I2C General call header (Slavemode) [0] (in _I2C_SR3)
  // reserved [7:5]

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
  #define _I2C_FS              ((uint8_t) (0x01 << 7))           ///< I2C master mode selection [0] (in _I2C_CCRH)

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
// Universal Asynchronous Receiver Transmitter 1 (UART1)
//------------------------
#if defined(UART1_AddressBase)

  /** @brief struct for controlling Universal Asynchronous Receiver Transmitter 1 (UART1) */
  typedef struct {

    /** @brief UART1 Status register (UART1_SR) */
    struct {
      _BITS   PE      : 1;    ///< Parity error
      _BITS   FE      : 1;    ///< Framing error
      _BITS   NF      : 1;    ///< Noise flag
      _BITS   OR      : 1;    ///< LIN Header Error (LIN slave mode) / Overrun error
      _BITS   IDLE    : 1;    ///< IDLE line detected
      _BITS   RXNE    : 1;    ///< Read data register not empty
      _BITS   TC      : 1;    ///< Transmission complete
      _BITS   TXE     : 1;    ///< Transmit data register empty
    } SR;


    /** @brief UART1 data register (UART1_DR) */
    struct {
      _BITS   DATA    : 8;    ///< UART1 data
    } DR;


    /** @brief UART1 Baud rate register 1 (UART1_BRR1) */
    struct {
      _BITS   DIV_4_11 : 8;    ///< UART_DIV bits [11:4]
    } BRR1;


    /** @brief UART1 Baud rate register 2 (UART1_BRR2) */
    struct {
      _BITS   DIV_0_3   : 4;    ///< UART_DIV bits [3:0]
      _BITS   DIV_12_15 : 4;    ///< UART_DIV bits [15:12]
    } BRR2;


    /** @brief UART1 Control register 1 (UART1_CR1) */
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


    /** @brief UART1 Control register 2 (UART1_CR2) */
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


    /** @brief UART1 Control register 3 (UART1_CR3) */
    struct {
      _BITS   LBCL    : 1;    ///< Last bit clock pulse
      _BITS   CPHA    : 1;    ///< Clock phase
      _BITS   CPOL    : 1;    ///< Clock polarity
      _BITS   CKEN    : 1;    ///< Clock enable
      _BITS   STOP    : 2;    ///< STOP bits
      _BITS   LINEN   : 1;    ///< LIN mode enable
      _BITS   res     : 1;    ///< Reserved, must be kept cleared
    } CR3;


    /** @brief UART1 Control register 4 (UART1_CR4) */
    struct {
      _BITS   ADD     : 4;    ///< Address of the UART node
      _BITS   LBDF    : 1;    ///< LIN Break Detection Flag
      _BITS   LBDL    : 1;    ///< LIN Break Detection Length
      _BITS   LBDIEN  : 1;    ///< LIN Break Detection Interrupt Enable
      _BITS   res     : 1;    ///< Reserved, must be kept cleared
    } CR4;


    /** @brief UART1 Control register 5 (UART1_CR5) */
    struct {
      _BITS   res     : 1;    ///< Reserved, must be kept cleared
      _BITS   IREN    : 1;    ///< IrDA mode Enable
      _BITS   IRLP    : 1;    ///< IrDA Low Power
      _BITS   HDSEL   : 1;    ///< Half-Duplex Selection
      _BITS   NACK    : 1;    ///< Smartcard NACK enable
      _BITS   SCEN    : 1;    ///< Smartcard mode enable
      _BITS   res2    : 2;    ///< Reserved, must be kept cleared
    } CR5;


    /** @brief UART1 guard time register (UART1_GTR) */
    struct {
      _BITS GT        : 8;  ///< UART1 guard time
    } GTR;


    /** @brief UART1 prescaler register (UART1_PSCR) */
    struct {
      _BITS PSC       : 8;  ///< UART1 prescaler
    } PSCR;

  } UART1_t;

  /* Pointer to UART1 registers */
  #define _UART1      _SFR(UART1_t,    UART1_AddressBase)        ///< UART1 struct/bit access
  #define _UART1_SR   _SFR(uint8_t,    UART1_AddressBase+0x00)   ///< UART1 Status register
  #define _UART1_DR   _SFR(uint8_t,    UART1_AddressBase+0x01)   ///< UART1 data register
  #define _UART1_BRR1 _SFR(uint8_t,    UART1_AddressBase+0x02)   ///< UART1 Baud rate register 1
  #define _UART1_BRR2 _SFR(uint8_t,    UART1_AddressBase+0x03)   ///< UART1 Baud rate register 2
  #define _UART1_CR1  _SFR(uint8_t,    UART1_AddressBase+0x04)   ///< UART1 Control register 1
  #define _UART1_CR2  _SFR(uint8_t,    UART1_AddressBase+0x05)   ///< UART1 Control register 2
  #define _UART1_CR3  _SFR(uint8_t,    UART1_AddressBase+0x06)   ///< UART1 Control register 3
  #define _UART1_CR4  _SFR(uint8_t,    UART1_AddressBase+0x07)   ///< UART1 Control register 4
  #define _UART1_CR5  _SFR(uint8_t,    UART1_AddressBase+0x08)   ///< UART1 Control register 5
  #define _UART1_GTR  _SFR(uint8_t,    UART1_AddressBase+0x09)   ///< UART1 guard time register
  #define _UART1_PSCR _SFR(uint8_t,    UART1_AddressBase+0x0A)   ///< UART1 prescaler register

  /* UART1 Module Reset Values */
  #define _UART1_SR_RESET_VALUE        ((uint8_t) 0xC0)          ///< UART1 Status register reset value
  #define _UART1_BRR1_RESET_VALUE      ((uint8_t) 0x00)          ///< UART1 Baud rate register 1 reset value
  #define _UART1_BRR2_RESET_VALUE      ((uint8_t) 0x00)          ///< UART1 Baud rate register 2 reset value
  #define _UART1_CR1_RESET_VALUE       ((uint8_t) 0x00)          ///< UART1 Control register 1 reset value
  #define _UART1_CR2_RESET_VALUE       ((uint8_t) 0x00)          ///< UART1 Control register 2 reset value
  #define _UART1_CR3_RESET_VALUE       ((uint8_t) 0x00)          ///< UART1 Control register 3 reset value
  #define _UART1_CR4_RESET_VALUE       ((uint8_t) 0x00)          ///< UART1 Control register 4 reset value
  #define _UART1_CR5_RESET_VALUE       ((uint8_t) 0x00)          ///< UART1 Control register 5 reset value
  #define _UART1_GTR_RESET_VALUE       ((uint8_t) 0x00)          ///< UART1 guard time register reset value
  #define _UART1_PSCR_RESET_VALUE      ((uint8_t) 0x00)          ///< UART1 prescaler register reset value

  /* UART1 Status register (UART1_SR) bits */
  #define _UART1_PE               ((uint8_t) (0x01 << 0))        ///< UART1 Parity error [0] (in _UART1_SR)
  #define _UART1_FE               ((uint8_t) (0x01 << 1))        ///< UART1 Framing error [0] (in _UART1_SR)
  #define _UART1_NF               ((uint8_t) (0x01 << 2))        ///< UART1 Noise flag [0] (in _UART1_SR)
  #define _UART1_OR_LHE           ((uint8_t) (0x01 << 3))        ///< UART1 LIN Header Error (LIN slave mode) / Overrun error [0] (in _UART1_SR)
  #define _UART1_IDLE             ((uint8_t) (0x01 << 4))        ///< UART1 IDLE line detected [0] (in _UART1_SR)
  #define _UART1_RXNE             ((uint8_t) (0x01 << 5))        ///< UART1 Read data register not empty [0] (in _UART1_SR)
  #define _UART1_TC               ((uint8_t) (0x01 << 6))        ///< UART1 Transmission complete [0] (in _UART1_SR)
  #define _UART1_TXE              ((uint8_t) (0x01 << 7))        ///< UART1 Transmit data register empty [0] (in _UART1_SR)

  /* UART1 Control register 1 (UART1_CR1) bits */
  #define _UART1_PIEN             ((uint8_t) (0x01 << 0))        ///< UART1 Parity interrupt enable [0] (in _UART1_CR1)
  #define _UART1_PS               ((uint8_t) (0x01 << 1))        ///< UART1 Parity selection [0] (in _UART1_CR1)
  #define _UART1_PCEN             ((uint8_t) (0x01 << 2))        ///< UART1 Parity control enable [0] (in _UART1_CR1)
  #define _UART1_WAKE             ((uint8_t) (0x01 << 3))        ///< UART1 Wakeup method [0] (in _UART1_CR1)
  #define _UART1_M                ((uint8_t) (0x01 << 4))        ///< UART1 word length [0] (in _UART1_CR1)
  #define _UART1_UARTD            ((uint8_t) (0x01 << 5))        ///< UART1 Disable (for low power consumption) [0] (in _UART1_CR1)
  #define _UART1_T8               ((uint8_t) (0x01 << 6))        ///< UART1 Transmit Data bit 8 (in 9-bit mode) [0] (in _UART1_CR1)
  #define _UART1_R8               ((uint8_t) (0x01 << 7))        ///< UART1 Receive Data bit 8 (in 9-bit mode) [0] (in _UART1_CR1)

  /* UART1 Control register 2 (UART1_CR2) bits */
  #define _UART1_SBK              ((uint8_t) (0x01 << 0))        ///< UART1 Send break [0] (in _UART1_CR2)
  #define _UART1_RWU              ((uint8_t) (0x01 << 1))        ///< UART1 Receiver wakeup [0] (in _UART1_CR2)
  #define _UART1_REN              ((uint8_t) (0x01 << 2))        ///< UART1 Receiver enable [0] (in _UART1_CR2)
  #define _UART1_TEN              ((uint8_t) (0x01 << 3))        ///< UART1 Transmitter enable [0] (in _UART1_CR2)
  #define _UART1_ILIEN            ((uint8_t) (0x01 << 4))        ///< UART1 IDLE Line interrupt enable [0] (in _UART1_CR2)
  #define _UART1_RIEN             ((uint8_t) (0x01 << 5))        ///< UART1 Receiver interrupt enable [0] (in _UART1_CR2)
  #define _UART1_TCIEN            ((uint8_t) (0x01 << 6))        ///< UART1 Transmission complete interrupt enable [0] (in _UART1_CR2)
  #define _UART1_TIEN             ((uint8_t) (0x01 << 7))        ///< UART1 Transmitter interrupt enable [0] (in _UART1_CR2)

  /* UART1 Control register 3 (UART1_CR3) bits */
  #define _UART1_LBCL             ((uint8_t) (0x01 << 0))        ///< UART1 Last bit clock pulse [0] (in _UART1_CR3)
  #define _UART1_CPHA             ((uint8_t) (0x01 << 1))        ///< UART1 Clock phase [0] (in _UART1_CR3)
  #define _UART1_CPOL             ((uint8_t) (0x01 << 2))        ///< UART1 Clock polarity [0] (in _UART1_CR3)
  #define _UART1_CKEN             ((uint8_t) (0x01 << 3))        ///< UART1 Clock enable [0] (in _UART1_CR3)
  #define _UART1_STOP             ((uint8_t) (0x03 << 4))        ///< UART1 STOP bits [1:0] (in _UART1_CR3)
  #define _UART1_STOP0            ((uint8_t) (0x01 << 4))        ///< UART1 STOP bits [0] (in _UART1_CR3)
  #define _UART1_STOP1            ((uint8_t) (0x01 << 5))        ///< UART1 STOP bits [1] (in _UART1_CR3)
  #define _UART1_LINEN            ((uint8_t) (0x01 << 6))        ///< UART1 LIN mode enable [0] (in _UART1_CR3)
  // reserved [7]

  /* UART1 Control register 4 (UART1_CR4) bits */
  #define _UART1_ADD              ((uint8_t) (0x0F << 0))        ///< UART1 Address of the UART node [3:0] (in _UART1_CR4)
  #define _UART1_ADD0             ((uint8_t) (0x01 << 0))        ///< UART1 Address of the UART node [0] (in _UART1_CR4)
  #define _UART1_ADD1             ((uint8_t) (0x01 << 1))        ///< UART1 Address of the UART node [1] (in _UART1_CR4)
  #define _UART1_ADD2             ((uint8_t) (0x01 << 2))        ///< UART1 Address of the UART node [2] (in _UART1_CR4)
  #define _UART1_ADD3             ((uint8_t) (0x01 << 3))        ///< UART1 Address of the UART node [3] (in _UART1_CR4)
  #define _UART1_LBDF             ((uint8_t) (0x01 << 4))        ///< UART1 LIN Break Detection Flag [0] (in _UART1_CR4)
  #define _UART1_LBDL             ((uint8_t) (0x01 << 5))        ///< UART1 LIN Break Detection Length [0] (in _UART1_CR4)
  #define _UART1_LBDIEN           ((uint8_t) (0x01 << 6))        ///< UART1 LIN Break Detection Interrupt Enable [0] (in _UART1_CR4)
  // reserved [7]

  /* UART1 Control register 5 (UART1_CR5) bits */
  // reserved [0]
  #define _UART1_IREN             ((uint8_t) (0x01 << 1))        ///< UART1 IrDA mode Enable [0] (in _UART1_CR5)
  #define _UART1_IRLP             ((uint8_t) (0x01 << 2))        ///< UART1 IrDA Low Power [0] (in _UART1_CR5)
  #define _UART1_HDSEL            ((uint8_t) (0x01 << 3))        ///< UART1 Half-Duplex Selection [0] (in _UART1_CR5)
  #define _UART1_NACK             ((uint8_t) (0x01 << 4))        ///< UART1 Smartcard NACK enable [0] (in _UART1_CR5)
  #define _UART1_SCEN             ((uint8_t) (0x01 << 5))        ///< UART1 Smartcard mode enable [0] (in _UART1_CR5)
  // reserved [7:6]

#endif // UART1_AddressBase



//------------------------
// Universal Asynchronous Receiver Transmitter 2 (UART2)
//------------------------
#if defined(UART2_AddressBase)

  /** @brief struct for controlling Universal Asynchronous Receiver Transmitter 2 (UART2) */
  typedef struct {

    /** @brief UART2 Status register (UART2_SR) */
    struct {
      _BITS   PE      : 1;    ///< Parity error
      _BITS   FE      : 1;    ///< Framing error
      _BITS   NF      : 1;    ///< Noise flag
      _BITS   OR      : 1;    ///< LIN Header Error (LIN slave mode) / Overrun error
      _BITS   IDLE    : 1;    ///< IDLE line detected
      _BITS   RXNE    : 1;    ///< Read data register not empty
      _BITS   TC      : 1;    ///< Transmission complete
      _BITS   TXE     : 1;    ///< Transmit data register empty
    } SR;


    /** @brief UART2 data register (UART2_DR) */
    struct {
      _BITS   DATA    : 8;    ///< UART2 data
    } DR;


    /** @brief UART2 Baud rate register 1 (UART2_BRR1) */
    struct {
      _BITS   DIV_4_11 : 8;   ///< UART2_BRR bits [11:4]
    } BRR1;


    /** @brief UART2 Baud rate register 2 (UART2_BRR2) */
    struct {
      _BITS   DIV_0_3   : 4;    ///< UART2_BRR bits [3:0]
      _BITS   DIV_12_15 : 4;    ///< UART2_BRR bits [15:12]
    } BRR2;


    /** @brief UART2 Control register 1 (UART2_CR1) */
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


    /** @brief UART2 Control register 2 (UART2_CR2) */
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


    /** @brief UART2 Control register 3 (UART2_CR3) */
    struct {
      _BITS   LBCL    : 1;    ///< Last bit clock pulse
      _BITS   CPHA    : 1;    ///< Clock phase
      _BITS   CPOL    : 1;    ///< Clock polarity
      _BITS   CKEN    : 1;    ///< Clock enable
      _BITS   STOP    : 2;    ///< STOP bits
      _BITS   LINEN   : 1;    ///< LIN mode enable
      _BITS   res     : 1;    ///< Reserved, must be kept cleared
    } CR3;


    /** @brief UART2 Control register 4 (UART2_CR4) */
    struct {
      _BITS   ADD     : 4;    ///< Address of the UART node
      _BITS   LBDF    : 1;    ///< LIN Break Detection Flag
      _BITS   LBDL    : 1;    ///< LIN Break Detection Length
      _BITS   LBDIEN  : 1;    ///< LIN Break Detection Interrupt Enable
      _BITS   res     : 1;    ///< Reserved, must be kept cleared
    } CR4;


    /** @brief UART2 Control register 5 (UART2_CR5) */
    struct {
      _BITS   res     : 1;    ///< Reserved, must be kept cleared
      _BITS   IREN    : 1;    ///< IrDA mode Enable
      _BITS   IRLP    : 1;    ///< IrDA Low Power
      _BITS   res2    : 1;    ///< Reserved, must be kept cleared
      _BITS   NACK    : 1;    ///< Smartcard NACK enable
      _BITS   SCEN    : 1;    ///< Smartcard mode enable
      _BITS   res3    : 2;    ///< Reserved, must be kept cleared
    } CR5;


    /** @brief UART2 Control register 6 (UART2_CR6) */
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


    /** @brief UART2 guard time register (UART2_GTR) */
    struct {
      _BITS GT        : 8;  ///< UART2 guard time
    } GTR;


    /** @brief UART2 prescaler register (UART2_PSCR) */
    struct {
      _BITS PSC       : 8;  ///< UART1 prescaler
    } PSCR;

  } UART2_t;

  /* Pointer to UART2 registers */
  #define _UART2      _SFR(UART2_t,    UART2_AddressBase)        ///< UART2 struct/bit access
  #define _UART2_SR   _SFR(uint8_t,    UART2_AddressBase+0x00)   ///< UART2 Status register
  #define _UART2_DR   _SFR(uint8_t,    UART2_AddressBase+0x01)   ///< UART2 data register
  #define _UART2_BRR1 _SFR(uint8_t,    UART2_AddressBase+0x02)   ///< UART2 Baud rate register 1
  #define _UART2_BRR2 _SFR(uint8_t,    UART2_AddressBase+0x03)   ///< UART2 Baud rate register 2
  #define _UART2_CR1  _SFR(uint8_t,    UART2_AddressBase+0x04)   ///< UART2 Control register 1
  #define _UART2_CR2  _SFR(uint8_t,    UART2_AddressBase+0x05)   ///< UART2 Control register 2
  #define _UART2_CR3  _SFR(uint8_t,    UART2_AddressBase+0x06)   ///< UART2 Control register 3
  #define _UART2_CR4  _SFR(uint8_t,    UART2_AddressBase+0x07)   ///< UART2 Control register 4
  #define _UART2_CR5  _SFR(uint8_t,    UART2_AddressBase+0x08)   ///< UART2 Control register 5
  #define _UART2_CR6  _SFR(uint8_t,    UART2_AddressBase+0x09)   ///< UART2 Control register 6
  #define _UART2_GTR  _SFR(uint8_t,    UART2_AddressBase+0x0A)   ///< UART2 guard time register
  #define _UART2_PSCR _SFR(uint8_t,    UART2_AddressBase+0x0B)   ///< UART2 prescaler register

  /* UART2 Module Reset Values */
  #define _UART2_SR_RESET_VALUE        ((uint8_t) 0xC0)          ///< UART2 Status register reset value
  #define _UART2_BRR1_RESET_VALUE      ((uint8_t) 0x00)          ///< UART2 Baud rate register 1 reset value
  #define _UART2_BRR2_RESET_VALUE      ((uint8_t) 0x00)          ///< UART2 Baud rate register 2 reset value
  #define _UART2_CR1_RESET_VALUE       ((uint8_t) 0x00)          ///< UART2 Control register 1 reset value
  #define _UART2_CR2_RESET_VALUE       ((uint8_t) 0x00)          ///< UART2 Control register 2 reset value
  #define _UART2_CR3_RESET_VALUE       ((uint8_t) 0x00)          ///< UART2 Control register 3 reset value
  #define _UART2_CR4_RESET_VALUE       ((uint8_t) 0x00)          ///< UART2 Control register 4 reset value
  #define _UART2_CR5_RESET_VALUE       ((uint8_t) 0x00)          ///< UART2 Control register 5 reset value
  #define _UART2_CR6_RESET_VALUE       ((uint8_t) 0x00)          ///< UART2 Control register 6 reset value
  #define _UART2_GTR_RESET_VALUE       ((uint8_t) 0x00)          ///< UART2 guard time register reset value
  #define _UART2_PSCR_RESET_VALUE      ((uint8_t) 0x00)          ///< UART2 prescaler register reset value

  /* UART2 Status register (UART2_SR) bits */
  #define _UART2_PE               ((uint8_t) (0x01 << 0))        ///< UART2 Parity error [0] (in _UART2_SR)
  #define _UART2_FE               ((uint8_t) (0x01 << 1))        ///< UART2 Framing error [0] (in _UART2_SR)
  #define _UART2_NF               ((uint8_t) (0x01 << 2))        ///< UART2 Noise flag [0] (in _UART2_SR)
  #define _UART2_OR_LHE           ((uint8_t) (0x01 << 3))        ///< UART2 LIN Header Error (LIN slave mode) / Overrun error [0] (in _UART2_SR)
  #define _UART2_IDLE             ((uint8_t) (0x01 << 4))        ///< UART2 IDLE line detected [0] (in _UART2_SR)
  #define _UART2_RXNE             ((uint8_t) (0x01 << 5))        ///< UART2 Read data register not empty [0] (in _UART2_SR)
  #define _UART2_TC               ((uint8_t) (0x01 << 6))        ///< UART2 Transmission complete [0] (in _UART2_SR)
  #define _UART2_TXE              ((uint8_t) (0x01 << 7))        ///< UART2 Transmit data register empty [0] (in _UART2_SR)

  /* UART2 Control register 1 (UART2_CR1) bits */
  #define _UART2_PIEN             ((uint8_t) (0x01 << 0))        ///< UART2 Parity interrupt enable [0] (in _UART2_CR1)
  #define _UART2_PS               ((uint8_t) (0x01 << 1))        ///< UART2 Parity selection [0] (in _UART2_CR1)
  #define _UART2_PCEN             ((uint8_t) (0x01 << 2))        ///< UART2 Parity control enable [0] (in _UART2_CR1)
  #define _UART2_WAKE             ((uint8_t) (0x01 << 3))        ///< UART2 Wakeup method [0] (in _UART2_CR1)
  #define _UART2_M                ((uint8_t) (0x01 << 4))        ///< UART2 word length [0] (in _UART2_CR1)
  #define _UART2_UARTD            ((uint8_t) (0x01 << 5))        ///< UART2 Disable (for low power consumption) [0] (in _UART2_CR1)
  #define _UART2_T8               ((uint8_t) (0x01 << 6))        ///< UART2 Transmit Data bit 8 (in 9-bit mode) [0] (in _UART2_CR1)
  #define _UART2_R8               ((uint8_t) (0x01 << 7))        ///< UART2 Receive Data bit 8 (in 9-bit mode) [0] (in _UART2_CR1)

  /* UART2 Control register 2 (UART2_CR2) bits */
  #define _UART2_SBK              ((uint8_t) (0x01 << 0))        ///< UART2 Send break [0] (in _UART2_CR2)
  #define _UART2_RWU              ((uint8_t) (0x01 << 1))        ///< UART2 Receiver wakeup [0] (in _UART2_CR2)
  #define _UART2_REN              ((uint8_t) (0x01 << 2))        ///< UART2 Receiver enable [0] (in _UART2_CR2)
  #define _UART2_TEN              ((uint8_t) (0x01 << 3))        ///< UART2 Transmitter enable [0] (in _UART2_CR2)
  #define _UART2_ILIEN            ((uint8_t) (0x01 << 4))        ///< UART2 IDLE Line interrupt enable [0] (in _UART2_CR2)
  #define _UART2_RIEN             ((uint8_t) (0x01 << 5))        ///< UART2 Receiver interrupt enable [0] (in _UART2_CR2)
  #define _UART2_TCIEN            ((uint8_t) (0x01 << 6))        ///< UART2 Transmission complete interrupt enable [0] (in _UART2_CR2)
  #define _UART2_TIEN             ((uint8_t) (0x01 << 7))        ///< UART2 Transmitter interrupt enable [0] (in _UART2_CR2)

  /* UART2 Control register 3 (UART2_CR3) bits */
  #define _UART2_LBCL             ((uint8_t) (0x01 << 0))        ///< UART2 Last bit clock pulse [0] (in _UART2_CR3)
  #define _UART2_CPHA             ((uint8_t) (0x01 << 1))        ///< UART2 Clock phase [0] (in _UART2_CR3)
  #define _UART2_CPOL             ((uint8_t) (0x01 << 2))        ///< UART2 Clock polarity [0] (in _UART2_CR3)
  #define _UART2_CKEN             ((uint8_t) (0x01 << 3))        ///< UART2 Clock enable [0] (in _UART2_CR3)
  #define _UART2_STOP             ((uint8_t) (0x03 << 4))        ///< UART2 STOP bits [1:0] (in _UART2_CR3)
  #define _UART2_STOP0            ((uint8_t) (0x01 << 4))        ///< UART2 STOP bits [0] (in _UART2_CR3)
  #define _UART2_STOP1            ((uint8_t) (0x01 << 5))        ///< UART2 STOP bits [1] (in _UART2_CR3)
  #define _UART2_LINEN            ((uint8_t) (0x01 << 6))        ///< UART2 LIN mode enable [0] (in _UART2_CR3)
  // reserved [7]

  /* UART2 Control register 4 (UART2_CR4) bits */
  #define _UART2_ADD              ((uint8_t) (0x0F << 0))        ///< UART2 Address of the UART node [3:0] (in _UART2_CR4)
  #define _UART2_ADD0             ((uint8_t) (0x01 << 0))        ///< UART2 Address of the UART node [0] (in _UART2_CR4)
  #define _UART2_ADD1             ((uint8_t) (0x01 << 1))        ///< UART2 Address of the UART node [1] (in _UART2_CR4)
  #define _UART2_ADD2             ((uint8_t) (0x01 << 2))        ///< UART2 Address of the UART node [2] (in _UART2_CR4)
  #define _UART2_ADD3             ((uint8_t) (0x01 << 3))        ///< UART2 Address of the UART node [3] (in _UART2_CR4)
  #define _UART2_LBDF             ((uint8_t) (0x01 << 4))        ///< UART2 LIN Break Detection Flag [0] (in _UART2_CR4)
  #define _UART2_LBDL             ((uint8_t) (0x01 << 5))        ///< UART2 LIN Break Detection Length [0] (in _UART2_CR4)
  #define _UART2_LBDIEN           ((uint8_t) (0x01 << 6))        ///< UART2 LIN Break Detection Interrupt Enable [0] (in _UART2_CR4)
  // reserved [7]

  /* UART2 Control register 5 (UART2_CR5) bits */
  // reserved [0]
  #define _UART2_IREN             ((uint8_t) (0x01 << 1))        ///< UART2 IrDA mode Enable [0] (in _UART2_CR5)
  #define _UART2_IRLP             ((uint8_t) (0x01 << 2))        ///< UART2 IrDA Low Power [0] (in _UART2_CR5)
  // reserved [3]
  #define _UART2_NACK             ((uint8_t) (0x01 << 4))        ///< UART2 Smartcard NACK enable [0] (in _UART2_CR5)
  #define _UART2_SCEN             ((uint8_t) (0x01 << 5))        ///< UART2 Smartcard mode enable [0] (in _UART2_CR5)
  // reserved [7:6]

  /* UART2 Control register 6 (UART2_CR6) bits */
  #define _UART2_LSF              ((uint8_t) (0x01 << 0))        ///< UART2 LIN Sync Field [0] (in _UART2_CR6)
  #define _UART2_LHDF             ((uint8_t) (0x01 << 1))        ///< UART2 LIN Header Detection Flag [0] (in _UART2_CR6)
  #define _UART2_LHDIEN           ((uint8_t) (0x01 << 2))        ///< UART2 LIN Header Detection Interrupt Enable [0] (in _UART2_CR6)
  // reserved [3]
  #define _UART2_LASE             ((uint8_t) (0x01 << 4))        ///< UART2 LIN automatic resynchronisation enable [0] (in _UART2_CR6)
  #define _UART2_LSLV             ((uint8_t) (0x01 << 5))        ///< UART2 LIN Slave Enable [0] (in _UART2_CR6)
  // reserved [6]
  #define _UART2_LDUM             ((uint8_t) (0x01 << 7))        ///< UART2 LIN Divider Update Method [0] (in _UART2_CR6)

#endif // UART2_AddressBase



//------------------------
// Universal Asynchronous Receiver Transmitter 3 (UART3)
//------------------------
#if defined(UART3_AddressBase)

  /** @brief struct for controlling Universal Asynchronous Receiver Transmitter 3 (UART3) */
  typedef struct {

    /** @brief UART3 Status register (UART3_SR) */
    struct {
      _BITS   PE      : 1;    ///< Parity error
      _BITS   FE      : 1;    ///< Framing error
      _BITS   NF      : 1;    ///< Noise flag
      _BITS   OR      : 1;    ///< LIN Header Error (LIN slave mode) / Overrun error
      _BITS   IDLE    : 1;    ///< IDLE line detected
      _BITS   RXNE    : 1;    ///< Read data register not empty
      _BITS   TC      : 1;    ///< Transmission complete
      _BITS   TXE     : 1;    ///< Transmit data register empty
    } SR;


    /** @brief UART3 data register (UART3_DR) */
    struct {
      _BITS   DATA    : 8;    ///< UART3 data
    } DR;


    /** @brief UART3 Baud rate register 1 (UART3_BRR1) */
    struct {
      _BITS   DIV_4_11 : 8;    ///< UART_DIV bits [11:4]
    } BRR1;


    /** @brief UART3 Baud rate register 2 (UART3_BRR2) */
    struct {
      _BITS   DIV_0_3   : 4;    ///< UART_DIV bits [3:0]
      _BITS   DIV_12_15 : 4;    ///< UART_DIV bits [15:12]
    } BRR2;


    /** @brief UART3 Control register 1 (UART3_CR1) */
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


    /** @brief UART3 Control register 2 (UART3_CR2) */
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


    /** @brief UART3 Control register 3 (UART3_CR3) */
    struct {
      _BITS   res     : 4;    ///< Reserved, must be kept cleared
      _BITS   STOP    : 2;    ///< STOP bits
      _BITS   LINEN   : 1;    ///< LIN mode enable
      _BITS   res2    : 1;    ///< Reserved, must be kept cleared
    } CR3;


    /** @brief UART3 Control register 4 (UART3_CR4) */
    struct {
      _BITS   ADD     : 4;    ///< Address of the UART node
      _BITS   LBDF    : 1;    ///< LIN Break Detection Flag
      _BITS   LBDL    : 1;    ///< LIN Break Detection Length
      _BITS   LBDIEN  : 1;    ///< LIN Break Detection Interrupt Enable
      _BITS   res     : 1;    ///< Reserved, must be kept cleared
    } CR4;


    /** @brief Reserved register (1B) */
    uint8_t   res[1];


    /** @brief UART3 Control register 6 (UART3_CR6) */
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

  } UART3_t;

  /* Pointer to UART3 registers */
  #define _UART3      _SFR(UART3_t,    UART3_AddressBase)        ///< UART3 struct/bit access
  #define _UART3_SR   _SFR(uint8_t,    UART3_AddressBase+0x00)   ///< UART3 Status register
  #define _UART3_DR   _SFR(uint8_t,    UART3_AddressBase+0x01)   ///< UART3 data register
  #define _UART3_BRR1 _SFR(uint8_t,    UART3_AddressBase+0x02)   ///< UART3 Baud rate register 1
  #define _UART3_BRR2 _SFR(uint8_t,    UART3_AddressBase+0x03)   ///< UART3 Baud rate register 2
  #define _UART3_CR1  _SFR(uint8_t,    UART3_AddressBase+0x04)   ///< UART3 Control register 1
  #define _UART3_CR2  _SFR(uint8_t,    UART3_AddressBase+0x05)   ///< UART3 Control register 2
  #define _UART3_CR3  _SFR(uint8_t,    UART3_AddressBase+0x06)   ///< UART3 Control register 3
  #define _UART3_CR4  _SFR(uint8_t,    UART3_AddressBase+0x07)   ///< UART3 Control register 4
  // 1B
  #define _UART3_CR6  _SFR(uint8_t,    UART3_AddressBase+0x09)   ///< UART3 Control register 6

  /* UART3 Module Reset Values */
  #define _UART3_SR_RESET_VALUE        ((uint8_t) 0xC0)          ///< UART3 Status register reset value
  #define _UART3_BRR1_RESET_VALUE      ((uint8_t) 0x00)          ///< UART3 Baud rate register 1 reset value
  #define _UART3_BRR2_RESET_VALUE      ((uint8_t) 0x00)          ///< UART3 Baud rate register 2 reset value
  #define _UART3_CR1_RESET_VALUE       ((uint8_t) 0x00)          ///< UART3 Control register 1 reset value
  #define _UART3_CR2_RESET_VALUE       ((uint8_t) 0x00)          ///< UART3 Control register 2 reset value
  #define _UART3_CR3_RESET_VALUE       ((uint8_t) 0x00)          ///< UART3 Control register 3 reset value
  #define _UART3_CR4_RESET_VALUE       ((uint8_t) 0x00)          ///< UART3 Control register 4 reset value
  #define _UART3_CR6_RESET_VALUE       ((uint8_t) 0x00)          ///< UART3 Control register 6 reset value

  /* UART3 Status register (UART3_SR) bits */
  #define _UART3_PE               ((uint8_t) (0x01 << 0))        ///< UART3 Parity error [0] (in _UART3_SR)
  #define _UART3_FE               ((uint8_t) (0x01 << 1))        ///< UART3 Framing error [0] (in _UART3_SR)
  #define _UART3_NF               ((uint8_t) (0x01 << 2))        ///< UART3 Noise flag [0] (in _UART3_SR)
  #define _UART3_OR_LHE           ((uint8_t) (0x01 << 3))        ///< UART3 LIN Header Error (LIN slave mode) / Overrun error [0] (in _UART3_SR)
  #define _UART3_IDLE             ((uint8_t) (0x01 << 4))        ///< UART3 IDLE line detected [0] (in _UART3_SR)
  #define _UART3_RXNE             ((uint8_t) (0x01 << 5))        ///< UART3 Read data register not empty [0] (in _UART3_SR)
  #define _UART3_TC               ((uint8_t) (0x01 << 6))        ///< UART3 Transmission complete [0] (in _UART3_SR)
  #define _UART3_TXE              ((uint8_t) (0x01 << 7))        ///< UART3 Transmit data register empty [0] (in _UART3_SR)

  /* UART3 Control register 1 (UART3_CR1) bits */
  #define _UART3_PIEN             ((uint8_t) (0x01 << 0))        ///< UART3 Parity interrupt enable [0] (in _UART3_CR1)
  #define _UART3_PS               ((uint8_t) (0x01 << 1))        ///< UART3 Parity selection [0] (in _UART3_CR1)
  #define _UART3_PCEN             ((uint8_t) (0x01 << 2))        ///< UART3 Parity control enable [0] (in _UART3_CR1)
  #define _UART3_WAKE             ((uint8_t) (0x01 << 3))        ///< UART3 Wakeup method [0] (in _UART3_CR1)
  #define _UART3_M                ((uint8_t) (0x01 << 4))        ///< UART3 word length [0] (in _UART3_CR1)
  #define _UART3_UARTD            ((uint8_t) (0x01 << 5))        ///< UART3 Disable (for low power consumption) [0] (in _UART3_CR1)
  #define _UART3_T8               ((uint8_t) (0x01 << 6))        ///< UART3 Transmit Data bit 8 (in 9-bit mode) [0] (in _UART3_CR1)
  #define _UART3_R8               ((uint8_t) (0x01 << 7))        ///< UART3 Receive Data bit 8 (in 9-bit mode) [0] (in _UART3_CR1)

  /* UART3 Control register 2 (UART3_CR2) bits */
  #define _UART3_SBK              ((uint8_t) (0x01 << 0))        ///< UART3 Send break [0] (in _UART3_CR2)
  #define _UART3_RWU              ((uint8_t) (0x01 << 1))        ///< UART3 Receiver wakeup [0] (in _UART3_CR2)
  #define _UART3_REN              ((uint8_t) (0x01 << 2))        ///< UART3 Receiver enable [0] (in _UART3_CR2)
  #define _UART3_TEN              ((uint8_t) (0x01 << 3))        ///< UART3 Transmitter enable [0] (in _UART3_CR2)
  #define _UART3_ILIEN            ((uint8_t) (0x01 << 4))        ///< UART3 IDLE Line interrupt enable [0] (in _UART3_CR2)
  #define _UART3_RIEN             ((uint8_t) (0x01 << 5))        ///< UART3 Receiver interrupt enable [0] (in _UART3_CR2)
  #define _UART3_TCIEN            ((uint8_t) (0x01 << 6))        ///< UART3 Transmission complete interrupt enable [0] (in _UART3_CR2)
  #define _UART3_TIEN             ((uint8_t) (0x01 << 7))        ///< UART3 Transmitter interrupt enable [0] (in _UART3_CR2)

  /* UART3 Control register 3 (UART3_CR3) bits */
  // reserved [3:0]
  #define _UART3_STOP             ((uint8_t) (0x03 << 4))        ///< UART3 STOP bits [1:0] (in _UART3_CR3)
  #define _UART3_STOP0            ((uint8_t) (0x01 << 4))        ///< UART3 STOP bits [0] (in _UART3_CR3)
  #define _UART3_STOP1            ((uint8_t) (0x01 << 5))        ///< UART3 STOP bits [1] (in _UART3_CR3)
  #define _UART3_LINEN            ((uint8_t) (0x01 << 6))        ///< UART3 LIN mode enable [0] (in _UART3_CR3)
  // reserved [7]

  /* UART3 Control register 4 (UART3_CR4) bits */
  #define _UART3_ADD              ((uint8_t) (0x0F << 0))        ///< UART3 Address of the UART node [3:0] (in _UART3_CR4)
  #define _UART3_ADD0             ((uint8_t) (0x01 << 0))        ///< UART3 Address of the UART node [0] (in _UART3_CR4)
  #define _UART3_ADD1             ((uint8_t) (0x01 << 1))        ///< UART3 Address of the UART node [1] (in _UART3_CR4)
  #define _UART3_ADD2             ((uint8_t) (0x01 << 2))        ///< UART3 Address of the UART node [2] (in _UART3_CR4)
  #define _UART3_ADD3             ((uint8_t) (0x01 << 3))        ///< UART3 Address of the UART node [3] (in _UART3_CR4)
  #define _UART3_LBDF             ((uint8_t) (0x01 << 4))        ///< UART3 LIN Break Detection Flag [0] (in _UART3_CR4)
  #define _UART3_LBDL             ((uint8_t) (0x01 << 5))        ///< UART3 LIN Break Detection Length [0] (in _UART3_CR4)
  #define _UART3_LBDIEN           ((uint8_t) (0x01 << 6))        ///< UART3 LIN Break Detection Interrupt Enable [0] (in _UART3_CR4)
  // reserved [7]

  /* UART3 Control register 6 (UART3_CR6) bits */
  #define _UART3_LSF              ((uint8_t) (0x01 << 0))        ///< UART3 LIN Sync Field [0] (in _UART3_CR6)
  #define _UART3_LHDF             ((uint8_t) (0x01 << 1))        ///< UART3 LIN Header Detection Flag [0] (in _UART3_CR6)
  #define _UART3_LHDIEN           ((uint8_t) (0x01 << 2))        ///< UART3 LIN Header Detection Interrupt Enable [0] (in _UART3_CR6)
  // reserved [3]
  #define _UART3_LASE             ((uint8_t) (0x01 << 4))        ///< UART3 LIN automatic resynchronisation enable [0] (in _UART3_CR6)
  #define _UART3_LSLV             ((uint8_t) (0x01 << 5))        ///< UART3 LIN Slave Enable [0] (in _UART3_CR6)
  // reserved [6]
  #define _UART3_LDUM             ((uint8_t) (0x01 << 7))        ///< UART3 LIN Divider Update Method [0] (in _UART3_CR6)

#endif // UART3_AddressBase



//------------------------
// Universal Asynchronous Receiver Transmitter 4 (UART4)
//------------------------
#if defined(UART4_AddressBase)

  /** @brief struct for controlling Universal Asynchronous Receiver Transmitter 4 (UART4) */
  typedef struct {

    /** @brief UART4 Status register (UART4_SR) */
    struct {
      _BITS   PE      : 1;    ///< Parity error
      _BITS   FE      : 1;    ///< Framing error
      _BITS   NF      : 1;    ///< Noise flag
      _BITS   OR      : 1;    ///< LIN Header Error (LIN slave mode) / Overrun error
      _BITS   IDLE    : 1;    ///< IDLE line detected
      _BITS   RXNE    : 1;    ///< Read data register not empty
      _BITS   TC      : 1;    ///< Transmission complete
      _BITS   TXE     : 1;    ///< Transmit data register empty
    } SR;


    /** @brief UART4 data register (UART4_DR) */
    struct {
      _BITS   DATA    : 8;    ///< UART4 data
    } DR;


    /** @brief UART4 Baud rate register 1 (UART4_BRR1) */
    struct {
      _BITS   DIV_4_11 : 8;    ///< UART_DIV bits [11:4]
    } BRR1;


    /** @brief UART4 Baud rate register 2 (UART4_BRR2) */
    struct {
      _BITS   DIV_0_3   : 4;    ///< UART_DIV bits [3:0]
      _BITS   DIV_12_15 : 4;    ///< UART_DIV bits [15:12]
    } BRR2;


    /** @brief UART4 Control register 1 (UART4_CR1) */
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


    /** @brief UART4 Control register 2 (UART4_CR2) */
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


    /** @brief UART4 Control register 3 (UART4_CR3) */
    struct {
      _BITS   LBCL    : 1;    ///< Last bit clock pulse
      _BITS   CPHA    : 1;    ///< Clock phase
      _BITS   CPOL    : 1;    ///< Clock polarity
      _BITS   CKEN    : 1;    ///< Clock enable
      _BITS   STOP    : 2;    ///< STOP bits
      _BITS   LINEN   : 1;    ///< LIN mode enable
      _BITS   res     : 1;    ///< Reserved, must be kept cleared
    } CR3;


    /** @brief UART4 Control register 4 (UART4_CR4) */
    struct {
      _BITS   ADD     : 4;    ///< Address of the UART node
      _BITS   LBDF    : 1;    ///< LIN Break Detection Flag
      _BITS   LBDL    : 1;    ///< LIN Break Detection Length
      _BITS   LBDIEN  : 1;    ///< LIN Break Detection Interrupt Enable
      _BITS   res     : 1;    ///< Reserved, must be kept cleared
    } CR4;


    /** @brief UART4 Control register 5 (UART4_CR5) */
    struct {
      _BITS   res     : 1;    ///< Reserved, must be kept cleared
      _BITS   IREN    : 1;    ///< IrDA mode Enable
      _BITS   IRLP    : 1;    ///< IrDA Low Power
      _BITS   HDSEL   : 1;    ///< Half-Duplex Selection
      _BITS   NACK    : 1;    ///< Smartcard NACK enable
      _BITS   SCEN    : 1;    ///< Smartcard mode enable
      _BITS   res2    : 2;    ///< Reserved, must be kept cleared
    } CR5;


    /** @brief UART4 Control register 6 (UART4_CR6) */
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


    /** @brief UART4 guard time register (UART4_GTR) */
    struct {
      _BITS GT        : 8;  ///< UART4 guard time
    } GTR;


    /** @brief UART4 prescaler register (UART4_PSCR) */
    struct {
      _BITS PSC       : 8;  ///< UART4 prescaler
    } PSCR;

  } UART4_t;

  /* Pointer to UART4 registers */
  #define _UART4      _SFR(UART4_t,    UART4_AddressBase)        ///< UART4 struct/bit access
  #define _UART4_SR   _SFR(uint8_t,    UART4_AddressBase+0x00)   ///< UART4 Status register
  #define _UART4_DR   _SFR(uint8_t,    UART4_AddressBase+0x01)   ///< UART4 data register
  #define _UART4_BRR1 _SFR(uint8_t,    UART4_AddressBase+0x02)   ///< UART4 Baud rate register 1
  #define _UART4_BRR2 _SFR(uint8_t,    UART4_AddressBase+0x03)   ///< UART4 Baud rate register 2
  #define _UART4_CR1  _SFR(uint8_t,    UART4_AddressBase+0x04)   ///< UART4 Control register 1
  #define _UART4_CR2  _SFR(uint8_t,    UART4_AddressBase+0x05)   ///< UART4 Control register 2
  #define _UART4_CR3  _SFR(uint8_t,    UART4_AddressBase+0x06)   ///< UART4 Control register 3
  #define _UART4_CR4  _SFR(uint8_t,    UART4_AddressBase+0x07)   ///< UART4 Control register 4
  #define _UART4_CR5  _SFR(uint8_t,    UART4_AddressBase+0x08)   ///< UART4 Control register 5
  #define _UART4_CR6  _SFR(uint8_t,    UART4_AddressBase+0x09)   ///< UART4 Control register 6
  #define _UART4_GTR  _SFR(uint8_t,    UART4_AddressBase+0x0A)   ///< UART4 guard time register
  #define _UART4_PSCR _SFR(uint8_t,    UART4_AddressBase+0x0B)   ///< UART4 prescaler register

  /* UART4 Module Reset Values */
  #define _UART4_SR_RESET_VALUE        ((uint8_t) 0xC0)          ///< UART4 Status register reset value
  #define _UART4_BRR1_RESET_VALUE      ((uint8_t) 0x00)          ///< UART4 Baud rate register 1 reset value
  #define _UART4_BRR2_RESET_VALUE      ((uint8_t) 0x00)          ///< UART4 Baud rate register 2 reset value
  #define _UART4_CR1_RESET_VALUE       ((uint8_t) 0x00)          ///< UART4 Control register 1 reset value
  #define _UART4_CR2_RESET_VALUE       ((uint8_t) 0x00)          ///< UART4 Control register 2 reset value
  #define _UART4_CR3_RESET_VALUE       ((uint8_t) 0x00)          ///< UART4 Control register 3 reset value
  #define _UART4_CR4_RESET_VALUE       ((uint8_t) 0x00)          ///< UART4 Control register 4 reset value
  #define _UART4_CR5_RESET_VALUE       ((uint8_t) 0x00)          ///< UART4 Control register 5 reset value
  #define _UART4_CR6_RESET_VALUE       ((uint8_t) 0x00)          ///< UART4 Control register 6 reset value
  #define _UART4_GTR_RESET_VALUE       ((uint8_t) 0x00)          ///< UART4 guard time register reset value
  #define _UART4_PSCR_RESET_VALUE      ((uint8_t) 0x00)          ///< UART4 prescaler register reset value

  /* UART4 Status register (UART4_SR) bits */
  #define _UART4_PE               ((uint8_t) (0x01 << 0))        ///< UART4 Parity error [0] (in _UART4_SR)
  #define _UART4_FE               ((uint8_t) (0x01 << 1))        ///< UART4 Framing error [0] (in _UART4_SR)
  #define _UART4_NF               ((uint8_t) (0x01 << 2))        ///< UART4 Noise flag [0] (in _UART4_SR)
  #define _UART4_OR_LHE           ((uint8_t) (0x01 << 3))        ///< UART4 LIN Header Error (LIN slave mode) / Overrun error [0] (in _UART4_SR)
  #define _UART4_IDLE             ((uint8_t) (0x01 << 4))        ///< UART4 IDLE line detected [0] (in _UART4_SR)
  #define _UART4_RXNE             ((uint8_t) (0x01 << 5))        ///< UART4 Read data register not empty [0] (in _UART4_SR)
  #define _UART4_TC               ((uint8_t) (0x01 << 6))        ///< UART4 Transmission complete [0] (in _UART4_SR)
  #define _UART4_TXE              ((uint8_t) (0x01 << 7))        ///< UART4 Transmit data register empty [0] (in _UART4_SR)

  /* UART4 Control register 1 (UART4_CR1) bits */
  #define _UART4_PIEN             ((uint8_t) (0x01 << 0))        ///< UART4 Parity interrupt enable [0] (in _UART4_CR1)
  #define _UART4_PS               ((uint8_t) (0x01 << 1))        ///< UART4 Parity selection [0] (in _UART4_CR1)
  #define _UART4_PCEN             ((uint8_t) (0x01 << 2))        ///< UART4 Parity control enable [0] (in _UART4_CR1)
  #define _UART4_WAKE             ((uint8_t) (0x01 << 3))        ///< UART4 Wakeup method [0] (in _UART4_CR1)
  #define _UART4_M                ((uint8_t) (0x01 << 4))        ///< UART4 word length [0] (in _UART4_CR1)
  #define _UART4_UARTD            ((uint8_t) (0x01 << 5))        ///< UART4 Disable (for low power consumption) [0] (in _UART4_CR1)
  #define _UART4_T8               ((uint8_t) (0x01 << 6))        ///< UART4 Transmit Data bit 8 (in 9-bit mode) [0] (in _UART4_CR1)
  #define _UART4_R8               ((uint8_t) (0x01 << 7))        ///< UART4 Receive Data bit 8 (in 9-bit mode) [0] (in _UART4_CR1)

  /* UART4 Control register 2 (UART4_CR2) bits */
  #define _UART4_SBK              ((uint8_t) (0x01 << 0))        ///< UART4 Send break [0] (in _UART4_CR2)
  #define _UART4_RWU              ((uint8_t) (0x01 << 1))        ///< UART4 Receiver wakeup [0] (in _UART4_CR2)
  #define _UART4_REN              ((uint8_t) (0x01 << 2))        ///< UART4 Receiver enable [0] (in _UART4_CR2)
  #define _UART4_TEN              ((uint8_t) (0x01 << 3))        ///< UART4 Transmitter enable [0] (in _UART4_CR2)
  #define _UART4_ILIEN            ((uint8_t) (0x01 << 4))        ///< UART4 IDLE Line interrupt enable [0] (in _UART4_CR2)
  #define _UART4_RIEN             ((uint8_t) (0x01 << 5))        ///< UART4 Receiver interrupt enable [0] (in _UART4_CR2)
  #define _UART4_TCIEN            ((uint8_t) (0x01 << 6))        ///< UART4 Transmission complete interrupt enable [0] (in _UART4_CR2)
  #define _UART4_TIEN             ((uint8_t) (0x01 << 7))        ///< UART4 Transmitter interrupt enable [0] (in _UART4_CR2)

  /* UART4 Control register 3 (UART4_CR3) bits */
  #define _UART4_LBCL             ((uint8_t) (0x01 << 0))        ///< UART4 Last bit clock pulse [0] (in _UART4_CR3)
  #define _UART4_CPHA             ((uint8_t) (0x01 << 1))        ///< UART4 Clock phase [0] (in _UART4_CR3)
  #define _UART4_CPOL             ((uint8_t) (0x01 << 2))        ///< UART4 Clock polarity [0] (in _UART4_CR3)
  #define _UART4_CKEN             ((uint8_t) (0x01 << 3))        ///< UART4 Clock enable [0] (in _UART4_CR3)
  #define _UART4_STOP             ((uint8_t) (0x03 << 4))        ///< UART4 STOP bits [1:0] (in _UART4_CR3)
  #define _UART4_STOP0            ((uint8_t) (0x01 << 4))        ///< UART4 STOP bits [0] (in _UART4_CR3)
  #define _UART4_STOP1            ((uint8_t) (0x01 << 5))        ///< UART4 STOP bits [1] (in _UART4_CR3)
  #define _UART4_LINEN            ((uint8_t) (0x01 << 6))        ///< UART4 LIN mode enable [0] (in _UART4_CR3)
  // reserved [7]

  /* UART4 Control register 4 (UART4_CR4) bits */
  #define _UART4_ADD              ((uint8_t) (0x0F << 0))        ///< UART4 Address of the UART node [3:0] (in _UART4_CR4)
  #define _UART4_ADD0             ((uint8_t) (0x01 << 0))        ///< UART4 Address of the UART node [0] (in _UART4_CR4)
  #define _UART4_ADD1             ((uint8_t) (0x01 << 1))        ///< UART4 Address of the UART node [1] (in _UART4_CR4)
  #define _UART4_ADD2             ((uint8_t) (0x01 << 2))        ///< UART4 Address of the UART node [2] (in _UART4_CR4)
  #define _UART4_ADD3             ((uint8_t) (0x01 << 3))        ///< UART4 Address of the UART node [3] (in _UART4_CR4)
  #define _UART4_LBDF             ((uint8_t) (0x01 << 4))        ///< UART4 LIN Break Detection Flag [0] (in _UART4_CR4)
  #define _UART4_LBDL             ((uint8_t) (0x01 << 5))        ///< UART4 LIN Break Detection Length [0] (in _UART4_CR4)
  #define _UART4_LBDIEN           ((uint8_t) (0x01 << 6))        ///< UART4 LIN Break Detection Interrupt Enable [0] (in _UART4_CR4)
  // reserved [7]

  /* UART4 Control register 5 (UART4_CR5) bits */
  // reserved [0]
  #define _UART4_IREN             ((uint8_t) (0x01 << 1))        ///< UART4 IrDA mode Enable [0] (in _UART4_CR5)
  #define _UART4_IRLP             ((uint8_t) (0x01 << 2))        ///< UART4 IrDA Low Power [0] (in _UART4_CR5)
  #define _UART4_HDSEL            ((uint8_t) (0x01 << 3))        ///< UART4 Half-Duplex Selection [0] (in _UART4_CR5)
  #define _UART4_NACK             ((uint8_t) (0x01 << 4))        ///< UART4 Smartcard NACK enable [0] (in _UART4_CR5)
  #define _UART4_SCEN             ((uint8_t) (0x01 << 5))        ///< UART4 Smartcard mode enable [0] (in _UART4_CR5)
  // reserved [7:6]

  /* UART4 Control register 6 (UART4_CR6) bits */
  #define _UART4_LSF              ((uint8_t) (0x01 << 0))        ///< UART4 LIN Sync Field [0] (in _UART4_CR6)
  #define _UART4_LHDF             ((uint8_t) (0x01 << 1))        ///< UART4 LIN Header Detection Flag [0] (in _UART4_CR6)
  #define _UART4_LHDIEN           ((uint8_t) (0x01 << 2))        ///< UART4 LIN Header Detection Interrupt Enable [0] (in _UART4_CR6)
  // reserved [3]
  #define _UART4_LASE             ((uint8_t) (0x01 << 4))        ///< UART4 LIN automatic resynchronisation enable [0] (in _UART4_CR6)
  #define _UART4_LSLV             ((uint8_t) (0x01 << 5))        ///< UART4 LIN Slave Enable [0] (in _UART4_CR6)
  // reserved [6]
  #define _UART4_LDUM             ((uint8_t) (0x01 << 7))        ///< UART4 LIN Divider Update Method [0] (in _UART4_CR6)

#endif // UART4_AddressBase



//------------------------
// 16-Bit Timer 1 (TIM1)
//------------------------
#if defined(TIM1_AddressBase)

  /** @brief struct for controlling 16-Bit Timer 1 (TIM1) */
  typedef struct {

    /** @brief TIM1 Control register 1 (TIM1_CR1) */
    struct {
      _BITS   CEN     : 1;    ///< Counter enable
      _BITS   UDIS    : 1;    ///< Update disable
      _BITS   URS     : 1;    ///< Update request source
      _BITS   OPM     : 1;    ///< One-pulse mode
      _BITS   DIR     : 1;    ///< Direction
      _BITS   CMS     : 2;    ///< Center-aligned mode selection
      _BITS   ARPE    : 1;    ///< Auto-reload preload enable
    } CR1;


    /** @brief TIM1 Control register 2 (TIM1_CR2) */
    struct {
      _BITS   CCPC    : 1;    ///< Capture/compare preloaded control
      _BITS   res     : 1;    ///< Reserved, forced by hardware to 0
      _BITS   COMS    : 1;    ///< Capture/compare control update selection
      _BITS   res2    : 1;    ///< Reserved, must be kept cleared
      _BITS   MMS     : 3;    ///< Master mode selection
      _BITS   res3    : 1;    ///< Reserved
    } CR2;


    /** @brief Slave mode control register (TIM1_SMCR) */
    struct {
      _BITS   SMS     : 3;    ///< Clock/trigger/slave mode selection
      _BITS   res     : 1;    ///< Reserved
      _BITS   TS      : 3;    ///< Trigger selection
      _BITS   MSM     : 1;    ///< Master/slave mode
    } SMCR;


    /** @brief TIM1 External trigger register (TIM1_ETR) */
    struct {
      _BITS   ETF     : 4;    ///< External trigger filter
      _BITS   ETPS    : 2;    ///< External trigger prescaler
      _BITS   ECE     : 1;    ///< External clock enable
      _BITS   ETP     : 1;    ///< External trigger polarity
    } ETR;


    /** @brief TIM1 Interrupt enable register (TIM1_IER) */
    struct {
      _BITS   UIE     : 1;    ///< Update interrupt enable
      _BITS   CC1IE   : 1;    ///< Capture/compare 1 interrupt enable
      _BITS   CC2IE   : 1;    ///< Capture/compare 2 interrupt enable
      _BITS   CC3IE   : 1;    ///< Capture/compare 3 interrupt enable
      _BITS   CC4IE   : 1;    ///< Capture/compare 4 interrupt enable
      _BITS   COMIE   : 1;    ///< Commutation interrupt enable
      _BITS   TIE     : 1;    ///< Trigger interrupt enable
      _BITS   BIE     : 1;    ///< Break interrupt enable
    } IER;


    /** @brief TIM1 Status register 1 (TIM1_SR1) */
    struct {
      _BITS   UIF     : 1;    ///< Update interrupt flag
      _BITS   CC1IF   : 1;    ///< Capture/compare 1 interrupt flag
      _BITS   CC2IF   : 1;    ///< Capture/compare 2 interrupt flag
      _BITS   CC3IF   : 1;    ///< Capture/compare 3 interrupt flag
      _BITS   CC4IF   : 1;    ///< Capture/compare 4 interrupt flag
      _BITS   COMIF   : 1;    ///< Commutation interrupt flag
      _BITS   TIF     : 1;    ///< Trigger interrupt flag
      _BITS   BIF     : 1;    ///< Break interrupt flag
    } SR1;


    /** @brief TIM1 Status register 2 (TIM1_SR2) */
    struct {
      _BITS   res     : 1;    ///< Reserved, must be kept cleared
      _BITS   CC1OF   : 1;    ///< Capture/compare 1 overcapture flag
      _BITS   CC2OF   : 1;    ///< Capture/compare 2 overcapture flag
      _BITS   CC3OF   : 1;    ///< Capture/compare 3 overcapture flag
      _BITS   CC4OF   : 1;    ///< Capture/compare 4 overcapture flag
      _BITS   res2    : 3;    ///< Reserved, must be kept cleared
    } SR2;


    /** @brief TIM1 Event generation register (TIM1_EGR) */
    struct {
      _BITS   UG      : 1;    ///< Update generation
      _BITS   CC1G    : 1;    ///< Capture/compare 1 generation
      _BITS   CC2G    : 1;    ///< Capture/compare 2 generation
      _BITS   CC3G    : 1;    ///< Capture/compare 3 generation
      _BITS   CC4G    : 1;    ///< Capture/compare 4 generation
      _BITS   COMG    : 1;    ///< Capture/compare control update generation
      _BITS   TG      : 1;    ///< Trigger generation
      _BITS   BG      : 1;    ///< Break generation
    } EGR;


    /** @brief TIM1 Capture/compare mode register 1 (TIM1_CCMR1) */
    union {

      /** @brief bitwise access to register (output mode) */
      struct {
        _BITS   CC1S    : 2;    ///< Compare 1 selection
        _BITS   OC1FE   : 1;    ///< Output compare 1 fast enable
        _BITS   OC1PE   : 1;    ///< Output compare 1 preload enable
        _BITS   OC1M    : 3;    ///< Output compare 1 mode
        _BITS   OC1CE   : 1;    ///< Output compare 1 clear enable
      } OUT;

      /** @brief bitwise access to register (input mode) */
      struct {
        _BITS   CC1S    : 2;    ///< Capture 1 selection
        _BITS   IC1PSC  : 2;    ///< Input capture 1 prescaler
        _BITS   IC1F    : 4;    ///< Input capture 1 filter
      } IN;

    } CCMR1;


    /** @brief TIM1 Capture/compare mode register 2 (TIM1_CCMR2) */
    union {

      /** @brief bitwise access to register (output mode) */
      struct {
       _BITS   CC2S    : 2;    ///< Capture/compare 2 selection
       _BITS   OC2FE   : 1;    ///< Output compare 2 fast enable
       _BITS   OC2PE   : 1;    ///< Output compare 2 preload enable
       _BITS   OC2M    : 3;    ///< Output compare 2 mode
       _BITS   OC2CE   : 1;    ///< Output compare 2 clear enable
      } OUT;

      /** @brief bitwise access to register (input mode) */
      struct {
        _BITS   CC2S    : 2;    ///< Capture/compare 2 selection
        _BITS   IC2PSC  : 2;    ///< Input capture 2 prescaler
        _BITS   IC2F    : 4;    ///< Input capture 2 filter
      } IN;

    } CCMR2;


    /** @brief TIM1 Capture/compare mode register 3 (TIM1_CCMR3) */
    union {

      /** @brief bitwise access to register (output mode) */
      struct {
        _BITS   CC3S    : 2;    ///< Capture/compare 3 selection
        _BITS   OC3FE   : 1;    ///< Output compare 3 fast enable
        _BITS   OC3PE   : 1;    ///< Output compare 3 preload enable
        _BITS   OC3M    : 3;    ///< Output compare 3 mode
        _BITS   OC3CE   : 1;    ///< Output compare 3 clear enable
      } OUT;

      /** @brief bitwise access to register (input mode) */
      struct {
        _BITS   CC3S    : 2;    ///< Capture/compare 3 selection
        _BITS   IC3PSC  : 2;    ///< Input capture 3 prescaler
        _BITS   IC3F    : 4;    ///< Input capture 3 filter
      } IN;

    } CCMR3;


    /** @brief TIM1 Capture/compare mode register 4 (TIM1_CCMR4) */
    union {

      /** @brief bitwise access to register (output mode) */
      struct {
        _BITS   CC4S    : 2;    ///< Capture/compare 4 selection
        _BITS   OC4FE   : 1;    ///< Output compare 4 fast enable
        _BITS   OC4PE   : 1;    ///< Output compare 4 preload enable
        _BITS   OC4M    : 3;    ///< Output compare 4 mode
        _BITS   OC4CE   : 1;    ///< Output compare 4 clear enable
      } OUT;

      /** @brief bitwise access to register (input mode) */
      struct {
        _BITS   CC4S    : 2;    ///< Capture/compare 4 selection
        _BITS   IC4PSC  : 2;    ///< Input capture 4 prescaler
        _BITS   IC4F    : 4;    ///< Input capture 4 filter
      } IN;

    } CCMR4;


    /** @brief TIM1 Capture/compare enable register 1 (TIM1_CCER1) */
    struct {
      _BITS   CC1E    : 1;    ///< Capture/compare 1 output enable
      _BITS   CC1P    : 1;    ///< Capture/compare 1 output polarity
      _BITS   CC1NE   : 1;    ///< Capture/compare 1 complementary output enable
      _BITS   CC1NP   : 1;    ///< Capture/compare 1 complementary output polarity
      _BITS   CC2E    : 1;    ///< Capture/compare 2 output enable
      _BITS   CC2P    : 1;    ///< Capture/compare 2 output polarity
      _BITS   CC2NE   : 1;    ///< Capture/compare 2 complementary output enable
      _BITS   CC2NP   : 1;    ///< Capture/compare 2 complementary output polarity
    } CCER1;


    /** @brief TIM1 Capture/compare enable register 2 (TIM1_CCER2) */
    struct {
      _BITS   CC3E    : 1;    ///< Capture/compare 3 output enable
      _BITS   CC3P    : 1;    ///< Capture/compare 3 output polarity
      _BITS   CC3NE   : 1;    ///< Capture/compare 3 complementary output enable
      _BITS   CC3NP   : 1;    ///< Capture/compare 3 complementary output polarity
      _BITS   CC4E    : 1;    ///< Capture/compare 4 output enable
      _BITS   CC4P    : 1;    ///< Capture/compare 4 output polarity
      _BITS   res     : 2;    ///< Reserved
    } CCER2;


    /** @brief TIM1 16-bit counter high byte (TIM1_CNTRH) */
    struct {
      _BITS   CNT     : 8;  ///< 16-bit counter [15:8]
    } CNTRH;


    /** @brief TIM1 16-bit counter low byte (TIM1_CNTRL) */
    struct {
      _BITS   CNT     : 8;  ///< 16-bit counter [7:0]
    } CNTRL;


    /** @brief TIM1 16-bit prescaler high byte (TIM1_PSCRH) */
    struct {
      _BITS   PSC     : 8;  ///< 16-bit prescaler [15:8]
    } PSCRH;


    /** @brief TIM1 16-bit prescaler low byte (TIM1_PSCRL) */
     struct {
      _BITS   PSC     : 8;  ///< 16-bit prescaler [7:0]
    } PSCRL;


    /** @brief TIM1 16-bit auto-reload value high byte (TIM1_ARRH) */
    struct {
      _BITS   ARR     : 8;  ///< 16-bit auto-reload value [15:8]
    } ARRH;


    /** @brief TIM1 16-bit auto-reload value low byte (TIM1_ARRL) */
    struct {
      _BITS   ARR     : 8;  ///< 16-bit auto-reload value [7:0]
    } ARRL;


    /** @brief TIM1 Repetition counter (TIM1_RCR) */
    struct {
      _BITS   REP     : 8;  ///< Repetition counter value
    } RCR;


    /** @brief TIM1 16-bit capture/compare value 1 high byte (TIM1_CCR1H) */
    struct {
      _BITS   CCR1    : 8;  ///< 16-bit capture/compare value 1 [15:8]
    } CCR1H;


    /** @brief TIM1 16-bit capture/compare value 1 low byte (TIM1_CCR1L) */
    struct {
      _BITS   CCR1    : 8;  ///< 16-bit capture/compare value 1 [7:0]
    } CCR1L;


    /** @brief TIM1 16-bit capture/compare value 2 high byte (TIM1_CCR2H) */
    struct {
      _BITS   CCR2    : 8;  ///< 16-bit capture/compare value 2 [15:8]
    } CCR2H;


    /** @brief TIM1 16-bit capture/compare value 2 low byte (TIM1_CCR2L) */
    struct {
      _BITS   CCR2    : 8;  ///< 16-bit capture/compare value 2 [7:0]
    } CCR2L;


    /** @brief TIM1 16-bit capture/compare value 3 high byte (TIM1_CCR3H) */
    struct {
      _BITS   CCR3    : 8;  ///< 16-bit capture/compare value 3 [15:8]
    } CCR3H;


    /** @brief TIM1 16-bit capture/compare value 3 low byte (TIM1_CCR3L) */
    struct {
      _BITS   CCR3    : 8;  ///< 16-bit capture/compare value 3 [7:0]
    } CCR3L;


    /** @brief TIM1 16-bit capture/compare value 4 high byte (TIM1_CCR4H) */
    struct {
      _BITS   CCR4    : 8;  ///< 16-bit capture/compare value 4 [15:8]
    } CCR4H;


    /** @brief TIM1 16-bit capture/compare value 4 low byte (TIM1_CCR4L) */
    struct {
      _BITS   CCR4    : 8;  ///< 16-bit capture/compare value 4 [7:0]
    } CCR4L;


    /** @brief TIM1 Break register (TIM1_BKR) */
    struct {
      _BITS   LOCK    : 2;    ///< Lock configuration
      _BITS   OSSI    : 1;    ///< Off state selection for idle mode
      _BITS   OSSR    : 1;    ///< Off state selection for Run mode
      _BITS   BKE     : 1;    ///< Break enable
      _BITS   BKP     : 1;    ///< Break polarity
      _BITS   AOE     : 1;    ///< Automatic output enable
      _BITS   MOE     : 1;    ///< Main output enable
    } BKR;


    /** @brief TIM1 Dead-time register (TIM1_DTR) */
    struct {
      _BITS   DTG     : 8;    ///< Deadtime generator set-up
    } DTR;


    /** @brief TIM1 Output idle state register (TIM1_OISR) */
    struct {
      _BITS   OIS1    : 1;    ///< Output idle state 1 (OC1 output)
      _BITS   OIS1N   : 1;    ///< Output idle state 1 (OC1N output)
      _BITS   OIS2    : 1;    ///< Output idle state 2 (OC2 output)
      _BITS   OIS2N   : 1;    ///< Output idle state 2 (OC2N output)
      _BITS   OIS3    : 1;    ///< Output idle state 3 (OC3 output)
      _BITS   OIS3N   : 1;    ///< Output idle state 3 (OC3N output)
      _BITS   OIS4    : 1;    ///< Output idle state 4 (OC4 output)
      _BITS   res     : 1;    ///< Reserved, forced by hardware to 0
    } OISR;

  } TIM1_t;

  /* Pointer to TIM1 registers */
  #define _TIM1        _SFR(TIM1_t,    TIM1_AddressBase)         ///< TIM1 struct/bit access
  #define _TIM1_CR1    _SFR(uint8_t,   TIM1_AddressBase+0x00)    ///< TIM1 control register 1
  #define _TIM1_CR2    _SFR(uint8_t,   TIM1_AddressBase+0x01)    ///< TIM1 control register 2
  #define _TIM1_SMCR   _SFR(uint8_t,   TIM1_AddressBase+0x02)    ///< TIM1 Slave mode control register
  #define _TIM1_ETR    _SFR(uint8_t,   TIM1_AddressBase+0x03)    ///< TIM1 External trigger register
  #define _TIM1_IER    _SFR(uint8_t,   TIM1_AddressBase+0x04)    ///< TIM1 interrupt enable register
  #define _TIM1_SR1    _SFR(uint8_t,   TIM1_AddressBase+0x05)    ///< TIM1 status register 1
  #define _TIM1_SR2    _SFR(uint8_t,   TIM1_AddressBase+0x06)    ///< TIM1 status register 2
  #define _TIM1_EGR    _SFR(uint8_t,   TIM1_AddressBase+0x07)    ///< TIM1 Event generation register
  #define _TIM1_CCMR1  _SFR(uint8_t,   TIM1_AddressBase+0x08)    ///< TIM1 Capture/compare mode register 1
  #define _TIM1_CCMR2  _SFR(uint8_t,   TIM1_AddressBase+0x09)    ///< TIM1 Capture/compare mode register 2
  #define _TIM1_CCMR3  _SFR(uint8_t,   TIM1_AddressBase+0x0A)    ///< TIM1 Capture/compare mode register 3
  #define _TIM1_CCMR4  _SFR(uint8_t,   TIM1_AddressBase+0x0B)    ///< TIM1 Capture/compare mode register 4
  #define _TIM1_CCER1  _SFR(uint8_t,   TIM1_AddressBase+0x0C)    ///< TIM1 Capture/compare enable register 1
  #define _TIM1_CCER2  _SFR(uint8_t,   TIM1_AddressBase+0x0D)    ///< TIM1 Capture/compare enable register 2
  #define _TIM1_CNTRH  _SFR(uint8_t,   TIM1_AddressBase+0x0E)    ///< TIM1 counter register high byte
  #define _TIM1_CNTRL  _SFR(uint8_t,   TIM1_AddressBase+0x0F)    ///< TIM1 counter register low byte
  #define _TIM1_PSCRH  _SFR(uint8_t,   TIM1_AddressBase+0x10)    ///< TIM1 clock prescaler register high byte
  #define _TIM1_PSCRL  _SFR(uint8_t,   TIM1_AddressBase+0x11)    ///< TIM1 clock prescaler register low byte
  #define _TIM1_ARRH   _SFR(uint8_t,   TIM1_AddressBase+0x12)    ///< TIM1 auto-reload register high byte
  #define _TIM1_ARRL   _SFR(uint8_t,   TIM1_AddressBase+0x13)    ///< TIM1 auto-reload register low byte
  #define _TIM1_RCR    _SFR(uint8_t,   TIM1_AddressBase+0x14)    ///< TIM1 Repetition counter
  #define _TIM1_CCR1H  _SFR(uint8_t,   TIM1_AddressBase+0x15)    ///< TIM1 16-bit capture/compare value 1 high byte
  #define _TIM1_CCR1L  _SFR(uint8_t,   TIM1_AddressBase+0x16)    ///< TIM1 16-bit capture/compare value 1 low byte
  #define _TIM1_CCR2H  _SFR(uint8_t,   TIM1_AddressBase+0x17)    ///< TIM1 16-bit capture/compare value 2 high byte
  #define _TIM1_CCR2L  _SFR(uint8_t,   TIM1_AddressBase+0x18)    ///< TIM1 16-bit capture/compare value 2 low byte
  #define _TIM1_CCR3H  _SFR(uint8_t,   TIM1_AddressBase+0x19)    ///< TIM1 16-bit capture/compare value 3 high byte
  #define _TIM1_CCR3L  _SFR(uint8_t,   TIM1_AddressBase+0x1A)    ///< TIM1 16-bit capture/compare value 3 low byte
  #define _TIM1_CCR4H  _SFR(uint8_t,   TIM1_AddressBase+0x1B)    ///< TIM1 16-bit capture/compare value 4 high byte
  #define _TIM1_CCR4L  _SFR(uint8_t,   TIM1_AddressBase+0x1C)    ///< TIM1 16-bit capture/compare value 4 low byte
  #define _TIM1_BKR    _SFR(uint8_t,   TIM1_AddressBase+0x1D)    ///< TIM1 Break register
  #define _TIM1_DTR    _SFR(uint8_t,   TIM1_AddressBase+0x1E)    ///< TIM1 Dead-time register
  #define _TIM1_OISR   _SFR(uint8_t,   TIM1_AddressBase+0x1F)    ///< TIM1 Output idle state register

  /* TIM1 Module Reset Values */
  #define _TIM1_CR1_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM1 control register 1 reset value
  #define _TIM1_CR2_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM1 control register 2 reset value
  #define _TIM1_SMCR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM1 Slave mode control register reset value
  #define _TIM1_ETR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM1 External trigger register reset value
  #define _TIM1_IER_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM1 interrupt enable register reset value
  #define _TIM1_SR1_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM1 status register 1 reset value
  #define _TIM1_SR2_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM1 status register 2 reset value
  #define _TIM1_EGR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM1 Event generation register reset value
  #define _TIM1_CCMR1_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 Capture/compare mode register 1 reset value
  #define _TIM1_CCMR2_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 Capture/compare mode register 2 reset value
  #define _TIM1_CCMR3_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 Capture/compare mode register 3 reset value
  #define _TIM1_CCMR4_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 Capture/compare mode register 4 reset value
  #define _TIM1_CCER1_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 Capture/compare enable register 1 reset value
  #define _TIM1_CCER2_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 Capture/compare enable register 2 reset value
  #define _TIM1_CNTRH_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 counter register high byte reset value
  #define _TIM1_CNTRL_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 counter register low byte reset value
  #define _TIM1_PSCRH_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 clock prescaler register high byte reset value
  #define _TIM1_PSCRL_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 clock prescaler register low byte reset value
  #define _TIM1_ARRH_RESET_VALUE       ((uint8_t) 0xFF)          ///< TIM1 auto-reload register high byte reset value
  #define _TIM1_ARRL_RESET_VALUE       ((uint8_t) 0xFF)          ///< TIM1 auto-reload register low byte reset value
  #define _TIM1_RCR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM1 Repetition counter reset value
  #define _TIM1_CCR1H_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 16-bit capture/compare value 1 high byte reset value
  #define _TIM1_CCR1L_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 16-bit capture/compare value 1 low byte reset value
  #define _TIM1_CCR2H_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 16-bit capture/compare value 2 high byte reset value
  #define _TIM1_CCR2L_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 16-bit capture/compare value 2 low byte reset value
  #define _TIM1_CCR3H_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 16-bit capture/compare value 3 high byte reset value
  #define _TIM1_CCR3L_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 16-bit capture/compare value 3 low byte reset value
  #define _TIM1_CCR4H_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 16-bit capture/compare value 4 high byte reset value
  #define _TIM1_CCR4L_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM1 16-bit capture/compare value 4 low byte reset value
  #define _TIM1_BKR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM1 Break register reset value
  #define _TIM1_DTR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM1 Dead-time register reset value
  #define _TIM1_OISR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM1 Output idle state register reset value

  /* TIM1 Control register (TIM1_CR1) */
  #define _TIM1_CEN               ((uint8_t) (0x01 << 0))        ///< TIM1 Counter enable [0] (in _TIM1_CR1)
  #define _TIM1_UDIS              ((uint8_t) (0x01 << 1))        ///< TIM1 Update disable [0] (in _TIM1_CR1)
  #define _TIM1_URS               ((uint8_t) (0x01 << 2))        ///< TIM1 Update request source [0] (in _TIM1_CR1)
  #define _TIM1_OPM               ((uint8_t) (0x01 << 3))        ///< TIM1 One-pulse mode [0] (in _TIM1_CR1)
  #define _TIM1_DIR               ((uint8_t) (0x01 << 4))        ///< TIM1 Direction [0] (in _TIM1_CR1)
  #define _TIM1_CMS               ((uint8_t) (0x03 << 5))        ///< TIM1 Center-aligned mode selection [1:0] (in _TIM1_CR1)
  #define _TIM1_CMS0              ((uint8_t) (0x01 << 5))        ///< TIM1 Center-aligned mode selection [0] (in _TIM1_CR1)
  #define _TIM1_CMS1              ((uint8_t) (0x01 << 6))        ///< TIM1 Center-aligned mode selection [1] (in _TIM1_CR1)
  #define _TIM1_ARPE              ((uint8_t) (0x01 << 7))        ///< TIM1 Auto-reload preload enable [0] (in _TIM1_CR1)

  /* TIM1 Control register (TIM1_CR2) */
  #define _TIM1_CCPC              ((uint8_t) (0x01 << 0))        ///< TIM1 Capture/compare preloaded control [0] (in _TIM1_CR2)
  // reserved [1]
  #define _TIM1_COMS              ((uint8_t) (0x01 << 2))        ///< TIM1 Capture/compare control update selection [0] (in _TIM1_CR2)
  // reserved [3]
  #define _TIM1_MMS               ((uint8_t) (0x07 << 4))        ///< TIM1 Master mode selection [2:0] (in _TIM1_CR2)
  #define _TIM1_MMS0              ((uint8_t) (0x01 << 4))        ///< TIM1 Master mode selection [0] (in _TIM1_CR2)
  #define _TIM1_MMS1              ((uint8_t) (0x01 << 5))        ///< TIM1 Master mode selection [1] (in _TIM1_CR2)
  #define _TIM1_MMS2              ((uint8_t) (0x01 << 6))        ///< TIM1 Master mode selection [2] (in _TIM1_CR2)
  // reserved [7]

  /* TIM1 Slave mode control register (TIM1_SMCR) */
  #define _TIM1_SMS               ((uint8_t) (0x07 << 0))        ///< TIM1 Clock/trigger/slave mode selection [2:0] (in _TIM1_SMCR)
  #define _TIM1_SMS0              ((uint8_t) (0x01 << 0))        ///< TIM1 Clock/trigger/slave mode selection [0] (in _TIM1_SMCR)
  #define _TIM1_SMS1              ((uint8_t) (0x01 << 1))        ///< TIM1 Clock/trigger/slave mode selection [1] (in _TIM1_SMCR)
  #define _TIM1_SMS2              ((uint8_t) (0x01 << 2))        ///< TIM1 Clock/trigger/slave mode selection [2] (in _TIM1_SMCR)
  // reserved [3]
  #define _TIM1_TS                ((uint8_t) (0x07 << 4))        ///< TIM1 Trigger selection [2:0] (in _TIM1_SMCR)
  #define _TIM1_TS0               ((uint8_t) (0x01 << 4))        ///< TIM1 Trigger selection [0] (in _TIM1_SMCR)
  #define _TIM1_TS1               ((uint8_t) (0x01 << 5))        ///< TIM1 Trigger selection [1] (in _TIM1_SMCR)
  #define _TIM1_TS2               ((uint8_t) (0x01 << 6))        ///< TIM1 Trigger selection [2] (in _TIM1_SMCR)
  #define _TIM1_MSM               ((uint8_t) (0x01 << 7))        ///< TIM1 Master/slave mode [0] (in _TIM1_SMCR)

  /* TIM1 External trigger register (TIM1_ETR) */
  #define _TIM1_ETF               ((uint8_t) (0x0F << 0))        ///< TIM1 External trigger filter [3:0] (in _TIM1_ETR)
  #define _TIM1_ETF0              ((uint8_t) (0x01 << 0))        ///< TIM1 External trigger filter [0] (in _TIM1_ETR)
  #define _TIM1_ETF1              ((uint8_t) (0x01 << 1))        ///< TIM1 External trigger filter [1] (in _TIM1_ETR)
  #define _TIM1_ETF2              ((uint8_t) (0x01 << 2))        ///< TIM1 External trigger filter [2] (in _TIM1_ETR)
  #define _TIM1_ETF3              ((uint8_t) (0x01 << 3))        ///< TIM1 External trigger filter [3] (in _TIM1_ETR)
  #define _TIM1_ETPS              ((uint8_t) (0x03 << 4))        ///< TIM1 External trigger prescaler [1:0] (in _TIM1_ETR)
  #define _TIM1_ETPS0             ((uint8_t) (0x01 << 4))        ///< TIM1 External trigger prescaler [0] (in _TIM1_ETR)
  #define _TIM1_ETPS1             ((uint8_t) (0x01 << 5))        ///< TIM1 External trigger prescaler [1] (in _TIM1_ETR)
  #define _TIM1_ECE               ((uint8_t) (0x01 << 6))        ///< TIM1 External clock enable [0] (in _TIM1_ETR)
  #define _TIM1_ETP               ((uint8_t) (0x01 << 7))        ///< TIM1 External trigger polarity [0] (in _TIM1_ETR)

  /* TIM1 Interrupt enable (TIM1_IER) */
  #define _TIM1_UIE               ((uint8_t) (0x01 << 0))        ///< TIM1 Update interrupt enable [0] (in _TIM1_IER)
  #define _TIM1_CC1IE             ((uint8_t) (0x01 << 1))        ///< TIM1 Capture/compare 1 interrupt enable [0] (in _TIM1_IER)
  #define _TIM1_CC2IE             ((uint8_t) (0x01 << 2))        ///< TIM1 Capture/compare 2 interrupt enable [0] (in _TIM1_IER)
  #define _TIM1_CC3IE             ((uint8_t) (0x01 << 3))        ///< TIM1 Capture/compare 3 interrupt enable [0] (in _TIM1_IER)
  #define _TIM1_CC4IE             ((uint8_t) (0x01 << 4))        ///< TIM1 Capture/compare 4 interrupt enable [0] (in _TIM1_IER)
  #define _TIM1_COMIE             ((uint8_t) (0x01 << 5))        ///< TIM1 Commutation interrupt enable [0] (in _TIM1_IER)
  #define _TIM1_TIE               ((uint8_t) (0x01 << 6))        ///< TIM1 Trigger interrupt enable [0] (in _TIM1_IER)
  #define _TIM1_BIE               ((uint8_t) (0x01 << 7))        ///< TIM1 Break interrupt enable [0] (in _TIM1_IER)

  /*  TIM1 Status register 1 (TIM1_SR1) */
  #define _TIM1_UIF               ((uint8_t) (0x01 << 0))        ///< TIM1 Update interrupt flag [0] (in _TIM1_SR1)
  #define _TIM1_CC1IF             ((uint8_t) (0x01 << 1))        ///< TIM1 Capture/compare 1 interrupt flag [0] (in _TIM1_SR1)
  #define _TIM1_CC2IF             ((uint8_t) (0x01 << 2))        ///< TIM1 Capture/compare 2 interrupt flag [0] (in _TIM1_SR1)
  #define _TIM1_CC3IF             ((uint8_t) (0x01 << 3))        ///< TIM1 Capture/compare 3 interrupt flag [0] (in _TIM1_SR1)
  #define _TIM1_CC4IF             ((uint8_t) (0x01 << 4))        ///< TIM1 Capture/compare 4 interrupt flag [0] (in _TIM1_SR1)
  #define _TIM1_COMIF             ((uint8_t) (0x01 << 5))        ///< TIM1 Commutation interrupt flag [0] (in _TIM1_SR1)
  #define _TIM1_TIF               ((uint8_t) (0x01 << 6))        ///< TIM1 Trigger interrupt flag [0] (in _TIM1_SR1)
  #define _TIM1_BIF               ((uint8_t) (0x01 << 7))        ///< TIM1 Break interrupt flag [0] (in _TIM1_SR1)

  /*  TIM1 Status register 2 (TIM1_SR2) */
  // reserved [0]
  #define _TIM1_CC1OF             ((uint8_t) (0x01 << 1))        ///< TIM1 Capture/compare 1 overcapture flag [0] (in _TIM1_SR2)
  #define _TIM1_CC2OF             ((uint8_t) (0x01 << 2))        ///< TIM1 Capture/compare 2 overcapture flag [0] (in _TIM1_SR2)
  #define _TIM1_CC3OF             ((uint8_t) (0x01 << 3))        ///< TIM1 Capture/compare 3 overcapture flag [0] (in _TIM1_SR2)
  #define _TIM1_CC4OF             ((uint8_t) (0x01 << 4))        ///< TIM1 Capture/compare 4 overcapture flag [0] (in _TIM1_SR2)
  // reserved [7:5]

  /*  TIM1 Event generation register (TIM1_EGR) */
  #define _TIM1_UG                ((uint8_t) (0x01 << 0))        ///< TIM1 Update generation [0] (in _TIM1_EGR)
  #define _TIM1_CC1G              ((uint8_t) (0x01 << 1))        ///< TIM1 Capture/compare 1 generation [0] (in _TIM1_EGR)
  #define _TIM1_CC2G              ((uint8_t) (0x01 << 2))        ///< TIM1 Capture/compare 2 generation [0] (in _TIM1_EGR)
  #define _TIM1_CC3G              ((uint8_t) (0x01 << 3))        ///< TIM1 Capture/compare 3 generation [0] (in _TIM1_EGR)
  #define _TIM1_CC4G              ((uint8_t) (0x01 << 4))        ///< TIM1 Capture/compare 4 generation [0] (in _TIM1_EGR)
  #define _TIM1_COMG              ((uint8_t) (0x01 << 5))        ///< TIM1 Capture/compare control update generation [0] (in _TIM1_EGR)
  #define _TIM1_TG                ((uint8_t) (0x01 << 6))        ///< TIM1 Trigger generation [0] (in _TIM1_EGR)
  #define _TIM1_BG                ((uint8_t) (0x01 << 7))        ///< TIM1 Break generation [0] (in _TIM1_EGR)

  /*  TIM1 Capture/compare mode register 1 (TIM1_CCMR1). Output mode */
  #define _TIM1_CC1S              ((uint8_t) (0x03 << 0))        ///< TIM1 Compare 1 selection [1:0] (in _TIM1_CCMR1)
  #define _TIM1_CC1S0             ((uint8_t) (0x01 << 0))        ///< TIM1 Compare 1 selection [0] (in _TIM1_CCMR1)
  #define _TIM1_CC1S1             ((uint8_t) (0x01 << 1))        ///< TIM1 Compare 1 selection [1] (in _TIM1_CCMR1)
  #define _TIM1_OC1FE             ((uint8_t) (0x01 << 2))        ///< TIM1 Output compare 1 fast enable [0] (in _TIM1_CCMR1)
  #define _TIM1_OC1PE             ((uint8_t) (0x01 << 3))        ///< TIM1 Output compare 1 preload enable [0] (in _TIM1_CCMR1)
  #define _TIM1_OC1M              ((uint8_t) (0x07 << 4))        ///< TIM1 Output compare 1 mode [2:0] (in _TIM1_CCMR1)
  #define _TIM1_OC1M0             ((uint8_t) (0x01 << 4))        ///< TIM1 Output compare 1 mode [0] (in _TIM1_CCMR1)
  #define _TIM1_OC1M1             ((uint8_t) (0x01 << 5))        ///< TIM1 Output compare 1 mode [1] (in _TIM1_CCMR1)
  #define _TIM1_OC1M2             ((uint8_t) (0x01 << 6))        ///< TIM1 Output compare 1 mode [2] (in _TIM1_CCMR1)
  #define _TIM1_OC1CE             ((uint8_t) (0x01 << 7))        ///< TIM1 Output compare 1 clear enable [0] (in _TIM1_CCMR1)

  /*  TIM1 Capture/compare mode register 1 (TIM1_CCMR1). Input mode */
  // _TIM1_CC1S [1:0] defined above
  #define _TIM1_IC1PSC            ((uint8_t) (0x03 << 2))        ///< TIM1 Input capture 1 prescaler [1:0] (in _TIM1_CCMR1)
  #define _TIM1_IC1PSC0           ((uint8_t) (0x01 << 2))        ///< TIM1 Input capture 1 prescaler [0] (in _TIM1_CCMR1)
  #define _TIM1_IC1PSC1           ((uint8_t) (0x01 << 3))        ///< TIM1 Input capture 1 prescaler [1] (in _TIM1_CCMR1)
  #define _TIM1_IC1F              ((uint8_t) (0x0F << 4))        ///< TIM1 Output compare 1 mode [3:0] (in _TIM1_CCMR1)
  #define _TIM1_IC1F0             ((uint8_t) (0x01 << 4))        ///< TIM1 Input capture 1 filter [0] (in _TIM1_CCMR1)
  #define _TIM1_IC1F1             ((uint8_t) (0x01 << 5))        ///< TIM1 Input capture 1 filter [1] (in _TIM1_CCMR1)
  #define _TIM1_IC1F2             ((uint8_t) (0x01 << 6))        ///< TIM1 Input capture 1 filter [2] (in _TIM1_CCMR1)
  #define _TIM1_IC1F3             ((uint8_t) (0x01 << 7))        ///< TIM1 Input capture 1 filter [3] (in _TIM1_CCMR1)

  /*  TIM1 Capture/compare mode register 2 (TIM1_CCMR2). Output mode */
  #define _TIM1_CC2S              ((uint8_t) (0x03 << 0))        ///< TIM1 Compare 2 selection [1:0] (in _TIM1_CCMR2)
  #define _TIM1_CC2S0             ((uint8_t) (0x01 << 0))        ///< TIM1 Compare 2 selection [0] (in _TIM1_CCMR2)
  #define _TIM1_CC2S1             ((uint8_t) (0x01 << 1))        ///< TIM1 Compare 2 selection [1] (in _TIM1_CCMR2)
  #define _TIM1_OC2FE             ((uint8_t) (0x01 << 2))        ///< TIM1 Output compare 2 fast enable [0] (in _TIM1_CCMR2)
  #define _TIM1_OC2PE             ((uint8_t) (0x01 << 3))        ///< TIM1 Output compare 2 preload enable [0] (in _TIM1_CCMR2)
  #define _TIM1_OC2M              ((uint8_t) (0x07 << 4))        ///< TIM1 Output compare 2 mode [2:0] (in _TIM1_CCMR2)
  #define _TIM1_OC2M0             ((uint8_t) (0x01 << 4))        ///< TIM1 Output compare 2 mode [0] (in _TIM1_CCMR2)
  #define _TIM1_OC2M1             ((uint8_t) (0x01 << 5))        ///< TIM1 Output compare 2 mode [1] (in _TIM1_CCMR2)
  #define _TIM1_OC2M2             ((uint8_t) (0x01 << 6))        ///< TIM1 Output compare 2 mode [2] (in _TIM1_CCMR2)
  #define _TIM1_OC2CE             ((uint8_t) (0x01 << 7))        ///< TIM1 Output compare 2 clear enable [0] (in _TIM1_CCMR2)

  /*  TIM1 Capture/compare mode register 2 (TIM1_CCMR2). Input mode */
  // _TIM1_CC2S [1:0] defined above
  #define _TIM1_IC2PSC            ((uint8_t) (0x03 << 2))        ///< TIM1 Input capture 2 prescaler [1:0] (in _TIM1_CCMR2)
  #define _TIM1_IC2PSC0           ((uint8_t) (0x01 << 2))        ///< TIM1 Input capture 2 prescaler [0] (in _TIM1_CCMR2)
  #define _TIM1_IC2PSC1           ((uint8_t) (0x01 << 3))        ///< TIM1 Input capture 2 prescaler [1] (in _TIM1_CCMR2)
  #define _TIM1_IC2F              ((uint8_t) (0x0F << 4))        ///< TIM1 Output compare 2 mode [3:0] (in _TIM1_CCMR2)
  #define _TIM1_IC2F0             ((uint8_t) (0x01 << 4))        ///< TIM1 Input capture 2 filter [0] (in _TIM1_CCMR2)
  #define _TIM1_IC2F1             ((uint8_t) (0x01 << 5))        ///< TIM1 Input capture 2 filter [1] (in _TIM1_CCMR2)
  #define _TIM1_IC2F2             ((uint8_t) (0x01 << 6))        ///< TIM1 Input capture 2 filter [2] (in _TIM1_CCMR2)
  #define _TIM1_IC2F3             ((uint8_t) (0x01 << 7))        ///< TIM1 Input capture 2 filter [3] (in _TIM1_CCMR2)

  /*  TIM1 Capture/compare mode register 3 (TIM1_CCMR3). Output mode */
  #define _TIM1_CC3S              ((uint8_t) (0x03 << 0))        ///< TIM1 Compare 3 selection [1:0] (in _TIM1_CCMR3)
  #define _TIM1_CC3S0             ((uint8_t) (0x01 << 0))        ///< TIM1 Compare 3 selection [0] (in _TIM1_CCMR3)
  #define _TIM1_CC3S1             ((uint8_t) (0x01 << 1))        ///< TIM1 Compare 3 selection [1] (in _TIM1_CCMR3)
  #define _TIM1_OC3FE             ((uint8_t) (0x01 << 2))        ///< TIM1 Output compare 3 fast enable [0] (in _TIM1_CCMR3)
  #define _TIM1_OC3PE             ((uint8_t) (0x01 << 3))        ///< TIM1 Output compare 3 preload enable [0] (in _TIM1_CCMR3)
  #define _TIM1_OC3M              ((uint8_t) (0x07 << 4))        ///< TIM1 Output compare 3 mode [2:0] (in _TIM1_CCMR3)
  #define _TIM1_OC3M0             ((uint8_t) (0x01 << 4))        ///< TIM1 Output compare 3 mode [0] (in _TIM1_CCMR3)
  #define _TIM1_OC3M1             ((uint8_t) (0x01 << 5))        ///< TIM1 Output compare 3 mode [1] (in _TIM1_CCMR3)
  #define _TIM1_OC3M2             ((uint8_t) (0x01 << 6))        ///< TIM1 Output compare 3 mode [2] (in _TIM1_CCMR3)
  #define _TIM1_OC3CE             ((uint8_t) (0x01 << 7))        ///< TIM1 Output compare 3 clear enable [0] (in _TIM1_CCMR3)

  /*  TIM1 Capture/compare mode register 3 (TIM1_CCMR3). Input mode */
  // _TIM1_CC3S [1:0] defined above
  #define _TIM1_IC3PSC            ((uint8_t) (0x03 << 2))        ///< TIM1 Input capture 3 prescaler [1:0] (in _TIM1_CCMR3)
  #define _TIM1_IC3PSC0           ((uint8_t) (0x01 << 2))        ///< TIM1 Input capture 3 prescaler [0] (in _TIM1_CCMR3)
  #define _TIM1_IC3PSC1           ((uint8_t) (0x01 << 3))        ///< TIM1 Input capture 3 prescaler [1] (in _TIM1_CCMR3)
  #define _TIM1_IC3F              ((uint8_t) (0x0F << 4))        ///< TIM1 Output compare 3 mode [3:0] (in _TIM1_CCMR3)
  #define _TIM1_IC3F0             ((uint8_t) (0x01 << 4))        ///< TIM1 Input capture 3 filter [0] (in _TIM1_CCMR3)
  #define _TIM1_IC3F1             ((uint8_t) (0x01 << 5))        ///< TIM1 Input capture 3 filter [1] (in _TIM1_CCMR3)
  #define _TIM1_IC3F2             ((uint8_t) (0x01 << 6))        ///< TIM1 Input capture 3 filter [2] (in _TIM1_CCMR3)
  #define _TIM1_IC3F3             ((uint8_t) (0x01 << 7))        ///< TIM1 Input capture 3 filter [3] (in _TIM1_CCMR3)

  /*  TIM1 Capture/compare mode register 4 (TIM1_CCMR4). Output mode */
  #define _TIM1_CC4S              ((uint8_t) (0x03 << 0))        ///< TIM1 Compare 4 selection [1:0] (in _TIM1_CCMR4)
  #define _TIM1_CC4S0             ((uint8_t) (0x01 << 0))        ///< TIM1 Compare 4 selection [0] (in _TIM1_CCMR4)
  #define _TIM1_CC4S1             ((uint8_t) (0x01 << 1))        ///< TIM1 Compare 4 selection [1] (in _TIM1_CCMR4)
  #define _TIM1_OC4FE             ((uint8_t) (0x01 << 2))        ///< TIM1 Output compare 4 fast enable [0] (in _TIM1_CCMR4)
  #define _TIM1_OC4PE             ((uint8_t) (0x01 << 3))        ///< TIM1 Output compare 4 preload enable [0] (in _TIM1_CCMR4)
  #define _TIM1_OC4M              ((uint8_t) (0x07 << 4))        ///< TIM1 Output compare 4 mode [2:0] (in _TIM1_CCMR4)
  #define _TIM1_OC4M0             ((uint8_t) (0x01 << 4))        ///< TIM1 Output compare 4 mode [0] (in _TIM1_CCMR4)
  #define _TIM1_OC4M1             ((uint8_t) (0x01 << 5))        ///< TIM1 Output compare 4 mode [1] (in _TIM1_CCMR4)
  #define _TIM1_OC4M2             ((uint8_t) (0x01 << 6))        ///< TIM1 Output compare 4 mode [2] (in _TIM1_CCMR4)
  #define _TIM1_OC4CE             ((uint8_t) (0x01 << 7))        ///< TIM1 Output compare 4 clear enable [0] (in _TIM1_CCMR4)

  /*  TIM1 Capture/compare mode register 4 (TIM1_CCMR4). Input mode */
  // _TIM1_CC4S [1:0] defined above
  #define _TIM1_IC4PSC            ((uint8_t) (0x03 << 2))        ///< TIM1 Input capture 4 prescaler [1:0] (in _TIM1_CCMR4)
  #define _TIM1_IC4PSC0           ((uint8_t) (0x01 << 2))        ///< TIM1 Input capture 4 prescaler [0] (in _TIM1_CCMR4)
  #define _TIM1_IC4PSC1           ((uint8_t) (0x01 << 3))        ///< TIM1 Input capture 4 prescaler [1] (in _TIM1_CCMR4)
  #define _TIM1_IC4F              ((uint8_t) (0x0F << 4))        ///< TIM1 Output compare 4 mode [3:0] (in _TIM1_CCMR4)
  #define _TIM1_IC4F0             ((uint8_t) (0x01 << 4))        ///< TIM1 Input capture 4 filter [0] (in _TIM1_CCMR4)
  #define _TIM1_IC4F1             ((uint8_t) (0x01 << 5))        ///< TIM1 Input capture 4 filter [1] (in _TIM1_CCMR4)
  #define _TIM1_IC4F2             ((uint8_t) (0x01 << 6))        ///< TIM1 Input capture 4 filter [2] (in _TIM1_CCMR4)
  #define _TIM1_IC4F3             ((uint8_t) (0x01 << 7))        ///< TIM1 Input capture 4 filter [3] (in _TIM1_CCMR4)

  /*  TIM1 Capture/compare enable register 1 (TIM1_CCER1) */
  #define _TIM1_CC1E              ((uint8_t) (0x01 << 0))        ///< TIM1 Capture/compare 1 output enable [0] (in _TIM1_CCER1)
  #define _TIM1_CC1P              ((uint8_t) (0x01 << 1))        ///< TIM1 Capture/compare 1 output polarity [0] (in _TIM1_CCER1)
  #define _TIM1_CC1NE             ((uint8_t) (0x01 << 2))        ///< TIM1 Capture/compare 1 complementary output enable [0] (in _TIM1_CCER1)
  #define _TIM1_CC1NP             ((uint8_t) (0x01 << 3))        ///< TIM1 Capture/compare 1 complementary output polarity [0] (in _TIM1_CCER1)
  #define _TIM1_CC2E              ((uint8_t) (0x01 << 4))        ///< TIM1 Capture/compare 2 output enable [0] (in _TIM1_CCER1)
  #define _TIM1_CC2P              ((uint8_t) (0x01 << 5))        ///< TIM1 Capture/compare 2 output polarity [0] (in _TIM1_CCER1)
  #define _TIM1_CC2NE             ((uint8_t) (0x01 << 6))        ///< TIM1 Capture/compare 2 complementary output enable [0] (in _TIM1_CCER1)
  #define _TIM1_CC2NP             ((uint8_t) (0x01 << 7))        ///< TIM1 Capture/compare 2 complementary output polarity [0] (in _TIM1_CCER1)

  /*  TIM1 Capture/compare enable register 2 (TIM1_CCER2) */
  #define _TIM1_CC3E              ((uint8_t) (0x01 << 0))        ///< TIM1 Capture/compare 3 output enable [0] (in _TIM1_CCER2)
  #define _TIM1_CC3P              ((uint8_t) (0x01 << 1))        ///< TIM1 Capture/compare 3 output polarity [0] (in _TIM1_CCER2)
  #define _TIM1_CC3NE             ((uint8_t) (0x01 << 2))        ///< TIM1 Capture/compare 3 complementary output enable [0] (in _TIM1_CCER2)
  #define _TIM1_CC3NP             ((uint8_t) (0x01 << 3))        ///< TIM1 Capture/compare 3 complementary output polarity [0] (in _TIM1_CCER2)
  #define _TIM1_CC4E              ((uint8_t) (0x01 << 4))        ///< TIM1 Capture/compare 4 output enable [0] (in _TIM1_CCER2)
  #define _TIM1_CC4P              ((uint8_t) (0x01 << 5))        ///< TIM1 Capture/compare 4 output polarity [0] (in _TIM1_CCER2)
  // reserved [7:6]

  /*  TIM1 Break register (TIM1_BKR) */
  #define _TIM1_LOCK              ((int8_t) (0x03 << 0))         ///< TIM1 Lock configuration [1:0] (in _TIM1_BKR)
  #define _TIM1_LOCK0             ((uint8_t) (0x01 << 0))        ///< TIM1 Lock configuration [0] (in _TIM1_BKR)
  #define _TIM1_LOCK1             ((uint8_t) (0x01 << 1))        ///< TIM1 Lock configuration [1] (in _TIM1_BKR)
  #define _TIM1_OSSI              ((uint8_t) (0x01 << 2))        ///< TIM1 Off state selection for idle mode [0] (in _TIM1_BKR)
  #define _TIM1_OSSR              ((uint8_t) (0x01 << 3))        ///< TIM1 Off state selection for Run mode [0] (in _TIM1_BKR)
  #define _TIM1_BKE               ((uint8_t) (0x01 << 4))        ///< TIM1 Break enable [0] (in _TIM1_BKR)
  #define _TIM1_BKP               ((uint8_t) (0x01 << 5))        ///< TIM1 Break polarity [0] (in _TIM1_BKR)
  #define _TIM1_AOE               ((uint8_t) (0x01 << 6))        ///< TIM1 Automatic output enable [0] (in _TIM1_BKR)
  #define _TIM1_MOE               ((uint8_t) (0x01 << 7))        ///< TIM1 Main output enable [0] (in _TIM1_BKR)

  /*  TIM1 Output idle state register (TIM1_OISR) */
  #define _TIM1_OIS1              ((uint8_t) (0x01 << 0))        ///< TIM1 Output idle state 1 (OC1 output) [0] (in _TIM1_OISR)
  #define _TIM1_OIS1N             ((uint8_t) (0x01 << 1))        ///< TIM1 Output idle state 1 (OC1N output) [0] (in _TIM1_OISR)
  #define _TIM1_OIS2              ((uint8_t) (0x01 << 2))        ///< TIM1 Output idle state 2 (OC2 output) [0] (in _TIM1_OISR)
  #define _TIM1_OIS2N             ((uint8_t) (0x01 << 3))        ///< TIM1 Output idle state 2 (OC2N output) [0] (in _TIM1_OISR)
  #define _TIM1_OIS3              ((uint8_t) (0x01 << 4))        ///< TIM1 Output idle state 3 (OC3 output) [0] (in _TIM1_OISR)
  #define _TIM1_OIS3N             ((uint8_t) (0x01 << 5))        ///< TIM1 Output idle state 3 (OC3N output) [0] (in _TIM1_OISR)
  #define _TIM1_OIS4              ((uint8_t) (0x01 << 6))        ///< TIM1 Output idle state 4 (OC4 output) [0] (in _TIM1_OISR)
  // reserved [7]

#endif // TIM1_AddressBase



//------------------------
// 16-Bit Timer 2 (TIM2)
//------------------------
#if defined(TIM2_AddressBase)

  /** @brief struct for controlling 16-Bit Timer 2 (TIM2) */
  typedef struct {

    /** @brief TIM2 Control register 1 (TIM2_CR1) */
    struct {
      _BITS   CEN     : 1;    ///< Counter enable
      _BITS   UDIS    : 1;    ///< Update disable
      _BITS   URS     : 1;    ///< Update request source
      _BITS   OPM     : 1;    ///< One-pulse mode
      _BITS   res     : 3;    ///< Reserved
      _BITS   ARPE    : 1;    ///< Auto-reload preload enable
    } CR1;


    /** @brief Reserved registers on selected devices (2B) */
    #if defined(STM8S103) || defined(STM8S003) || defined(STM8S001)
      uint8_t   res[2];
    #endif


    /** @brief TIM2 Interrupt enable register (TIM2_IER) */
    struct {
      _BITS   UIE     : 1;    ///< Update interrupt enable
      _BITS   CC1IE   : 1;    ///< Capture/compare 1 interrupt enable
      _BITS   CC2IE   : 1;    ///< Capture/compare 2 interrupt enable
      _BITS   CC3IE   : 1;    ///< Capture/compare 3 interrupt enable
      _BITS   res     : 4;    ///< Reserved
    } IER;


    /** @brief TIM2 Status register 1 (TIM2_SR1) */
    struct {
      _BITS   UIF     : 1;    ///< Update interrupt flag
      _BITS   CC1IF   : 1;    ///< Capture/compare 1 interrupt flag
      _BITS   CC2IF   : 1;    ///< Capture/compare 2 interrupt flag
      _BITS   CC3IF   : 1;    ///< Capture/compare 3 interrupt flag
      _BITS   res     : 4;    ///< Reserved
    } SR1;


    /** @brief TIM2 Status register 2 (TIM2_SR2) */
    struct {
      _BITS   res     : 1;    ///< Reserved, must be kept cleared
      _BITS   CC1OF   : 1;    ///< Capture/compare 1 overcapture flag
      _BITS   CC2OF   : 1;    ///< Capture/compare 2 overcapture flag
      _BITS   CC3OF   : 1;    ///< Capture/compare 3 overcapture flag
      _BITS   res2    : 4;    ///< Reserved, must be kept cleared
    } SR2;


    /** @brief TIM2 Event generation register (TIM2_EGR) */
    struct {
      _BITS   UG      : 1;    ///< Update generation
      _BITS   CC1G    : 1;    ///< Capture/compare 1 generation
      _BITS   CC2G    : 1;    ///< Capture/compare 2 generation
      _BITS   CC3G    : 1;    ///< Capture/compare 3 generation
      _BITS   res     : 4;    ///< Reserved
    } EGR;


    /** @brief TIM2 Capture/compare mode register 1 (TIM2_CCMR1) */
    union {

      /** @brief bitwise access to register (output mode) */
      struct {
        _BITS   CC1S    : 2;    ///< Compare 1 selection
        _BITS   res     : 1;    ///< Reserved
        _BITS   OC1PE   : 1;    ///< Output compare 1 preload enable
        _BITS   OC1M    : 3;    ///< Output compare 1 mode
        _BITS   res2    : 1;    ///< Reserved
      } OUT;

      /** @brief bitwise access to register (input mode) */
      struct {
        _BITS   CC1S    : 2;    ///< Capture 1 selection
        _BITS   IC1PSC  : 2;    ///< Input capture 1 prescaler
        _BITS   IC1F    : 4;    ///< Input capture 1 filter
      } IN;

    } CCMR1;


    /** @brief TIM2 Capture/compare mode register 2 (TIM2_CCMR2) */
    union {

      /** @brief bitwise access to register (output mode) */
      struct {
       _BITS   CC2S    : 2;    ///< Capture/compare 2 selection
       _BITS   res     : 1;    ///< Reserved
       _BITS   OC2PE   : 1;    ///< Output compare 2 preload enable
       _BITS   OC2M    : 3;    ///< Output compare 2 mode
       _BITS   res2    : 1;    ///< Reserved
      } OUT;

      /** @brief bitwise access to register (input mode) */
      struct {
        _BITS   CC2S    : 2;    ///< Capture/compare 2 selection
        _BITS   IC2PSC  : 2;    ///< Input capture 2 prescaler
        _BITS   IC2F    : 4;    ///< Input capture 2 filter
      } IN;

    } CCMR2;


    /** @brief TIM2 Capture/compare mode register 3 (TIM2_CCMR3) */
    union {

      /** @brief bitwise access to register (output mode) */
      struct {
        _BITS   CC3S    : 2;    ///< Capture/compare 3 selection
        _BITS   res     : 1;    ///< Reserved
        _BITS   OC3PE   : 1;    ///< Output compare 3 preload enable
        _BITS   OC3M    : 3;    ///< Output compare 3 mode
        _BITS   OC3CE   : 1;    ///< Output compare 3 clear enable
      } OUT;

      /** @brief bitwise access to register (input mode) */
      struct {
        _BITS   CC3S    : 2;    ///< Capture/compare 3 selection
        _BITS   IC3PSC  : 2;    ///< Input capture 3 prescaler
        _BITS   IC3F    : 4;    ///< Input capture 3 filter
      } IN;

    } CCMR3;


    /** @brief TIM2 Capture/compare enable register 1 (TIM2_CCER1) */
    struct {
      _BITS   CC1E    : 1;    ///< Capture/compare 1 output enable
      _BITS   CC1P    : 1;    ///< Capture/compare 1 output polarity
      _BITS   res     : 2;    ///< Reserved
      _BITS   CC2E    : 1;    ///< Capture/compare 2 output enable
      _BITS   CC2P    : 1;    ///< Capture/compare 2 output polarity
      _BITS   res2    : 2;    ///< Reserved
    } CCER1;


    /** @brief TIM2 Capture/compare enable register 2 (TIM2_CCER2) */
    struct {
      _BITS   CC3E    : 1;    ///< Capture/compare 3 output enable
      _BITS   CC3P    : 1;    ///< Capture/compare 3 output polarity
      _BITS   res     : 6;    ///< Reserved
    } CCER2;


    /** @brief TIM2 16-bit counter high byte (TIM2_CNTRH) */
    struct {
      _BITS   CNT     : 8;    ///< 16-bit counter [15:8]
    } CNTRH;


    /** @brief TIM2 16-bit counter low byte (TIM2_CNTRL) */
    struct {
      _BITS   CNT     : 8;    ///< 16-bit counter [7:0]
    } CNTRL;


    /** @brief TIM2 16-bit prescaler high byte (TIM2_PSCR) */
    struct {
      _BITS   PSC     : 4;    ///< prescaler [3:0]
      _BITS   res     : 4;    ///< Reserved
    } PSCR;


    /** @brief TIM2 16-bit auto-reload value high byte (TIM2_ARRH) */
    struct {
      _BITS   ARR     : 8;    ///< 16-bit auto-reload value [15:8]
    } ARRH;


    /** @brief TIM2 16-bit auto-reload value low byte (TIM2_ARRL) */
    struct {
      _BITS   ARR     : 8;    ///< 16-bit auto-reload value [7:0]
    } ARRL;


    /** @brief TIM2 16-bit capture/compare value 1 high byte (TIM2_CCR1H) */
    struct {
      _BITS   CCR1    : 8;    ///< 16-bit capture/compare value 1 [15:8]
    } CCR1H;


    /** @brief TIM2 16-bit capture/compare value 1 low byte (TIM2_CCR1L) */
    struct {
      _BITS   CCR1    : 8;    ///< 16-bit capture/compare value 1 [7:0]
    } CCR1L;


    /** @brief TIM2 16-bit capture/compare value 2 high byte (TIM2_CCR2H) */
    struct {
      _BITS   CCR2    : 8;  ///< 16-bit capture/compare value 2 [15:8]
    } CCR2H;


    /** @brief TIM2 16-bit capture/compare value 2 low byte (TIM2_CCR2L) */
    struct {
      _BITS   CCR2    : 8;    ///< 16-bit capture/compare value 2 [7:0]
    } CCR2L;


    /** @brief TIM2 16-bit capture/compare value 3 high byte (TIM2_CCR3H) */
    struct {
      _BITS   CCR3    : 8;    ///< 16-bit capture/compare value 3 [15:8]
    } CCR3H;


    /** @brief TIM2 16-bit capture/compare value 3 low byte (TIM2_CCR3L) */
    struct {
      _BITS   CCR3    : 8;    ///< 16-bit capture/compare value 3 [7:0]
    } CCR3L;

  } TIM2_t;

  /* Pointer to TIM2 registers */
  #define _TIM2        _SFR(TIM2_t,    TIM2_AddressBase)         ///< TIM2 struct/bit access
  #define _TIM2_CR1    _SFR(uint8_t,   TIM2_AddressBase+0x00)    ///< TIM2 control register 1
  #if defined(STM8S103) || defined(STM8S003) || defined(STM8S001)  // 2B offset for selected devices
    // 2B reserved
    #define _TIM2_IER    _SFR(uint8_t,   TIM2_AddressBase+0x03)    ///< TIM2 interrupt enable register
    #define _TIM2_SR1    _SFR(uint8_t,   TIM2_AddressBase+0x04)    ///< TIM2 status register 1
    #define _TIM2_SR2    _SFR(uint8_t,   TIM2_AddressBase+0x05)    ///< TIM2 status register 2
    #define _TIM2_EGR    _SFR(uint8_t,   TIM2_AddressBase+0x06)    ///< TIM2 Event generation register
    #define _TIM2_CCMR1  _SFR(uint8_t,   TIM2_AddressBase+0x07)    ///< TIM2 Capture/compare mode register 1
    #define _TIM2_CCMR2  _SFR(uint8_t,   TIM2_AddressBase+0x08)    ///< TIM2 Capture/compare mode register 2
    #define _TIM2_CCMR3  _SFR(uint8_t,   TIM2_AddressBase+0x09)    ///< TIM2 Capture/compare mode register 3
    #define _TIM2_CCER1  _SFR(uint8_t,   TIM2_AddressBase+0x0A)    ///< TIM2 Capture/compare enable register 1
    #define _TIM2_CCER2  _SFR(uint8_t,   TIM2_AddressBase+0x0B)    ///< TIM2 Capture/compare enable register 2
    #define _TIM2_CNTRH  _SFR(uint8_t,   TIM2_AddressBase+0x0C)    ///< TIM2 counter register high byte
    #define _TIM2_CNTRL  _SFR(uint8_t,   TIM2_AddressBase+0x0D)    ///< TIM2 counter register low byte
    #define _TIM2_PSCR   _SFR(uint8_t,   TIM2_AddressBase+0x0E)    ///< TIM2 clock prescaler register
    #define _TIM2_ARRH   _SFR(uint8_t,   TIM2_AddressBase+0x0F)    ///< TIM2 auto-reload register high byte
    #define _TIM2_ARRL   _SFR(uint8_t,   TIM2_AddressBase+0x10)    ///< TIM2 auto-reload register low byte
    #define _TIM2_CCR1H  _SFR(uint8_t,   TIM2_AddressBase+0x11)    ///< TIM2 16-bit capture/compare value 1 high byte
    #define _TIM2_CCR1L  _SFR(uint8_t,   TIM2_AddressBase+0x12)    ///< TIM2 16-bit capture/compare value 1 low byte
    #define _TIM2_CCR2H  _SFR(uint8_t,   TIM2_AddressBase+0x13)    ///< TIM2 16-bit capture/compare value 2 high byte
    #define _TIM2_CCR2L  _SFR(uint8_t,   TIM2_AddressBase+0x14)    ///< TIM2 16-bit capture/compare value 2 low byte
    #define _TIM2_CCR3H  _SFR(uint8_t,   TIM2_AddressBase+0x15)    ///< TIM2 16-bit capture/compare value 3 high byte
    #define _TIM2_CCR3L  _SFR(uint8_t,   TIM2_AddressBase+0x16)    ///< TIM2 16-bit capture/compare value 3 low byte
  #else
    #define _TIM2_IER    _SFR(uint8_t,   TIM2_AddressBase+0x01)    ///< TIM2 interrupt enable register
    #define _TIM2_SR1    _SFR(uint8_t,   TIM2_AddressBase+0x02)    ///< TIM2 status register 1
    #define _TIM2_SR2    _SFR(uint8_t,   TIM2_AddressBase+0x03)    ///< TIM2 status register 2
    #define _TIM2_EGR    _SFR(uint8_t,   TIM2_AddressBase+0x04)    ///< TIM2 Event generation register
    #define _TIM2_CCMR1  _SFR(uint8_t,   TIM2_AddressBase+0x05)    ///< TIM2 Capture/compare mode register 1
    #define _TIM2_CCMR2  _SFR(uint8_t,   TIM2_AddressBase+0x06)    ///< TIM2 Capture/compare mode register 2
    #define _TIM2_CCMR3  _SFR(uint8_t,   TIM2_AddressBase+0x07)    ///< TIM2 Capture/compare mode register 3
    #define _TIM2_CCER1  _SFR(uint8_t,   TIM2_AddressBase+0x08)    ///< TIM2 Capture/compare enable register 1
    #define _TIM2_CCER2  _SFR(uint8_t,   TIM2_AddressBase+0x09)    ///< TIM2 Capture/compare enable register 2
    #define _TIM2_CNTRH  _SFR(uint8_t,   TIM2_AddressBase+0x0A)    ///< TIM2 counter register high byte
    #define _TIM2_CNTRL  _SFR(uint8_t,   TIM2_AddressBase+0x0B)    ///< TIM2 counter register low byte
    #define _TIM2_PSCR   _SFR(uint8_t,   TIM2_AddressBase+0x0C)    ///< TIM2 clock prescaler register
    #define _TIM2_ARRH   _SFR(uint8_t,   TIM2_AddressBase+0x0D)    ///< TIM2 auto-reload register high byte
    #define _TIM2_ARRL   _SFR(uint8_t,   TIM2_AddressBase+0x0E)    ///< TIM2 auto-reload register low byte
    #define _TIM2_CCR1H  _SFR(uint8_t,   TIM2_AddressBase+0x0F)    ///< TIM2 16-bit capture/compare value 1 high byte
    #define _TIM2_CCR1L  _SFR(uint8_t,   TIM2_AddressBase+0x10)    ///< TIM2 16-bit capture/compare value 1 low byte
    #define _TIM2_CCR2H  _SFR(uint8_t,   TIM2_AddressBase+0x11)    ///< TIM2 16-bit capture/compare value 2 high byte
    #define _TIM2_CCR2L  _SFR(uint8_t,   TIM2_AddressBase+0x12)    ///< TIM2 16-bit capture/compare value 2 low byte
    #define _TIM2_CCR3H  _SFR(uint8_t,   TIM2_AddressBase+0x13)    ///< TIM2 16-bit capture/compare value 3 high byte
    #define _TIM2_CCR3L  _SFR(uint8_t,   TIM2_AddressBase+0x14)    ///< TIM2 16-bit capture/compare value 3 low byte
  #endif

  /* TIM2 Module Reset Values */
  #define _TIM2_CR1_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM2 control register 1 reset value
  #define _TIM2_IER_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM2 interrupt enable register reset value
  #define _TIM2_SR1_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM2 status register 1 reset value
  #define _TIM2_SR2_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM2 status register 2 reset value
  #define _TIM2_EGR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM2 Event generation register reset value
  #define _TIM2_CCMR1_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 Capture/compare mode register 1 reset value
  #define _TIM2_CCMR2_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 Capture/compare mode register 2 reset value
  #define _TIM2_CCMR3_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 Capture/compare mode register 3 reset value
  #define _TIM2_CCER1_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 Capture/compare enable register 1 reset value
  #define _TIM2_CCER2_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 Capture/compare enable register 2 reset value
  #define _TIM2_CNTRH_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 counter register high byte reset value
  #define _TIM2_CNTRL_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 counter register low byte reset value
  #define _TIM2_PSCR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM2 clock prescaler register reset value
  #define _TIM2_ARRH_RESET_VALUE       ((uint8_t) 0xFF)          ///< TIM2 auto-reload register high byte reset value
  #define _TIM2_ARRL_RESET_VALUE       ((uint8_t) 0xFF)          ///< TIM2 auto-reload register low byte reset value
  #define _TIM2_CCR1H_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 16-bit capture/compare value 1 high byte reset value
  #define _TIM2_CCR1L_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 16-bit capture/compare value 1 low byte reset value
  #define _TIM2_CCR2H_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 16-bit capture/compare value 2 high byte reset value
  #define _TIM2_CCR2L_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 16-bit capture/compare value 2 low byte reset value
  #define _TIM2_CCR3H_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 16-bit capture/compare value 3 high byte reset value
  #define _TIM2_CCR3L_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM2 16-bit capture/compare value 3 low byte reset value

  /* TIM2 Control register (TIM2_CR1) */
  #define _TIM2_CEN               ((uint8_t) (0x01 << 0))        ///< TIM2 Counter enable [0] (in _TIM2_CR1)
  #define _TIM2_UDIS              ((uint8_t) (0x01 << 1))        ///< TIM2 Update disable [0] (in _TIM2_CR1)
  #define _TIM2_URS               ((uint8_t) (0x01 << 2))        ///< TIM2 Update request source [0] (in _TIM2_CR1)
  #define _TIM2_OPM               ((uint8_t) (0x01 << 3))        ///< TIM2 One-pulse mode [0] (in _TIM2_CR1)
  // reserved [6:4]
  #define _TIM2_ARPE              ((uint8_t) (0x01 << 7))        ///< TIM2 Auto-reload preload enable [0] (in _TIM2_CR1)

  /* TIM2 Interrupt enable (TIM2_IER) */
  #define _TIM2_UIE               ((uint8_t) (0x01 << 0))        ///< TIM2 Update interrupt enable [0] (in _TIM2_IER)
  #define _TIM2_CC1IE             ((uint8_t) (0x01 << 1))        ///< TIM2 Capture/compare 1 interrupt enable [0] (in _TIM2_IER)
  #define _TIM2_CC2IE             ((uint8_t) (0x01 << 2))        ///< TIM2 Capture/compare 2 interrupt enable [0] (in _TIM2_IER)
  #define _TIM2_CC3IE             ((uint8_t) (0x01 << 3))        ///< TIM2 Capture/compare 3 interrupt enable [0] (in _TIM2_IER)
  // reserved [7:4]

  /*  TIM2 Status register 1 (TIM2_SR1) */
  #define _TIM2_UIF               ((uint8_t) (0x01 << 0))        ///< TIM2 Update interrupt flag [0] (in _TIM2_SR1)
  #define _TIM2_CC1IF             ((uint8_t) (0x01 << 1))        ///< TIM2 Capture/compare 1 interrupt flag [0] (in _TIM2_SR1)
  #define _TIM2_CC2IF             ((uint8_t) (0x01 << 2))        ///< TIM2 Capture/compare 2 interrupt flag [0] (in _TIM2_SR1)
  #define _TIM2_CC3IF             ((uint8_t) (0x01 << 3))        ///< TIM2 Capture/compare 3 interrupt flag [0] (in _TIM2_SR1)
  // reserved [7:4]

  /*  TIM2 Status register 2 (TIM2_SR2) */
  // reserved [0]
  #define _TIM2_CC1OF             ((uint8_t) (0x01 << 1))        ///< TIM2 Capture/compare 1 overcapture flag [0] (in _TIM2_SR2)
  #define _TIM2_CC2OF             ((uint8_t) (0x01 << 2))        ///< TIM2 Capture/compare 2 overcapture flag [0] (in _TIM2_SR2)
  #define _TIM2_CC3OF             ((uint8_t) (0x01 << 3))        ///< TIM2 Capture/compare 3 overcapture flag [0] (in _TIM2_SR2)
  // reserved [7:4]

  /*  TIM2 Event generation register (TIM2_EGR) */
  #define _TIM2_UG                ((uint8_t) (0x01 << 0))        ///< TIM2 Update generation [0] (in _TIM2_EGR)
  #define _TIM2_CC1G              ((uint8_t) (0x01 << 1))        ///< TIM2 Capture/compare 1 generation [0] (in _TIM2_EGR)
  #define _TIM2_CC2G              ((uint8_t) (0x01 << 2))        ///< TIM2 Capture/compare 2 generation [0] (in _TIM2_EGR)
  #define _TIM2_CC3G              ((uint8_t) (0x01 << 3))        ///< TIM2 Capture/compare 3 generation [0] (in _TIM2_EGR)
  // reserved [7:4]

  /*  TIM2 Capture/compare mode register 1 (TIM2_CCMR1). Output mode */
  #define _TIM2_CC1S              ((uint8_t) (0x03 << 0))        ///< TIM2 Compare 1 selection [1:0] (in _TIM2_CCMR1)
  #define _TIM2_CC1S0             ((uint8_t) (0x01 << 0))        ///< TIM2 Compare 1 selection [0] (in _TIM2_CCMR1)
  #define _TIM2_CC1S1             ((uint8_t) (0x01 << 1))        ///< TIM2 Compare 1 selection [1] (in _TIM2_CCMR1)
  // reserved [2]
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
  #define _TIM2_IC1F0             ((uint8_t) (0x01 << 4))        ///< TIM2 Output compare 1 mode [0] (in _TIM2_CCMR1)
  #define _TIM2_IC1F1             ((uint8_t) (0x01 << 5))        ///< TIM2 Output compare 1 mode [1] (in _TIM2_CCMR1)
  #define _TIM2_IC1F2             ((uint8_t) (0x01 << 6))        ///< TIM2 Output compare 1 mode [2] (in _TIM2_CCMR1)
  #define _TIM2_IC1F3             ((uint8_t) (0x01 << 7))        ///< TIM2 Output compare 1 mode [3] (in _TIM2_CCMR1)

  /*  TIM2 Capture/compare mode register 2 (TIM2_CCMR2). Output mode */
  #define _TIM2_CC2S              ((uint8_t) (0x03 << 0))        ///< TIM2 Compare 2 selection [1:0] (in _TIM2_CCMR2)
  #define _TIM2_CC2S0             ((uint8_t) (0x01 << 0))        ///< TIM2 Compare 2 selection [0] (in _TIM2_CCMR2)
  #define _TIM2_CC2S1             ((uint8_t) (0x01 << 1))        ///< TIM2 Compare 2 selection [1] (in _TIM2_CCMR2)
  // reserved [2]
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
  #define _TIM2_IC2F0             ((uint8_t) (0x01 << 4))        ///< TIM2 Output compare 2 mode [0] (in _TIM2_CCMR2)
  #define _TIM2_IC2F1             ((uint8_t) (0x01 << 5))        ///< TIM2 Output compare 2 mode [1] (in _TIM2_CCMR2)
  #define _TIM2_IC2F2             ((uint8_t) (0x01 << 6))        ///< TIM2 Output compare 2 mode [2] (in _TIM2_CCMR2)
  #define _TIM2_IC2F3             ((uint8_t) (0x01 << 7))        ///< TIM2 Output compare 2 mode [3] (in _TIM2_CCMR2)

  /*  TIM2 Capture/compare mode register 3 (TIM2_CCMR3). Output mode */
  #define _TIM2_CC3S              ((uint8_t) (0x03 << 0))        ///< TIM2 Compare 3 selection [1:0] (in _TIM2_CCMR3)
  #define _TIM2_CC3S0             ((uint8_t) (0x01 << 0))        ///< TIM2 Compare 3 selection [0] (in _TIM2_CCMR3)
  #define _TIM2_CC3S1             ((uint8_t) (0x01 << 1))        ///< TIM2 Compare 3 selection [1] (in _TIM2_CCMR3)
  // reserved [2]
  #define _TIM2_OC3PE             ((uint8_t) (0x01 << 3))        ///< TIM2 Output compare 3 preload enable [0] (in _TIM2_CCMR3)
  #define _TIM2_OC3M              ((uint8_t) (0x07 << 4))        ///< TIM2 Output compare 3 mode [2:0] (in _TIM2_CCMR3)
  #define _TIM2_OC3M0             ((uint8_t) (0x01 << 4))        ///< TIM2 Output compare 3 mode [0] (in _TIM2_CCMR3)
  #define _TIM2_OC3M1             ((uint8_t) (0x01 << 5))        ///< TIM2 Output compare 3 mode [1] (in _TIM2_CCMR3)
  #define _TIM2_OC3M2             ((uint8_t) (0x01 << 6))        ///< TIM2 Output compare 3 mode [2] (in _TIM2_CCMR3)
  // reserved [7]

  /*  TIM2 Capture/compare mode register 3 (TIM2_CCMR3). Input mode */
  // _TIM2_CC3S [1:0] defined above
  #define _TIM2_IC3PSC            ((uint8_t) (0x03 << 2))        ///< TIM2 Input capture 3 prescaler [1:0] (in _TIM2_CCMR3)
  #define _TIM2_IC3PSC0           ((uint8_t) (0x01 << 2))        ///< TIM2 Input capture 3 prescaler [0] (in _TIM2_CCMR3)
  #define _TIM2_IC3PSC1           ((uint8_t) (0x01 << 3))        ///< TIM2 Input capture 3 prescaler [1] (in _TIM2_CCMR3)
  #define _TIM2_IC3F              ((uint8_t) (0x0F << 4))        ///< TIM2 Output compare 3 mode [3:0] (in _TIM2_CCMR3)
  #define _TIM2_IC3F0             ((uint8_t) (0x01 << 4))        ///< TIM2 Output compare 3 mode [0] (in _TIM2_CCMR3)
  #define _TIM2_IC3F1             ((uint8_t) (0x01 << 5))        ///< TIM2 Output compare 3 mode [1] (in _TIM2_CCMR3)
  #define _TIM2_IC3F2             ((uint8_t) (0x01 << 6))        ///< TIM2 Output compare 3 mode [2] (in _TIM2_CCMR3)
  #define _TIM2_IC3F3             ((uint8_t) (0x01 << 7))        ///< TIM2 Output compare 3 mode [3] (in _TIM2_CCMR3)

  /*  TIM2 Capture/compare enable register 1 (TIM2_CCER1) */
  #define _TIM2_CC1E              ((uint8_t) (0x01 << 0))        ///< TIM2 Capture/compare 1 output enable [0] (in _TIM2_CCER1)
  #define _TIM2_CC1P              ((uint8_t) (0x01 << 1))        ///< TIM2 Capture/compare 1 output polarity [0] (in _TIM2_CCER1)
  // reserved [3:2]
  #define _TIM2_CC2E              ((uint8_t) (0x01 << 4))        ///< TIM2 Capture/compare 2 output enable [0] (in _TIM2_CCER1)
  #define _TIM2_CC2P              ((uint8_t) (0x01 << 5))        ///< TIM2 Capture/compare 2 output polarity [0] (in _TIM2_CCER1)
  // reserved [7:6]

  /*  TIM2 Capture/compare enable register 2 (TIM2_CCER2) */
  #define _TIM2_CC3E              ((uint8_t) (0x01 << 0))        ///< TIM2 Capture/compare 3 output enable [0] (in _TIM2_CCER2)
  #define _TIM2_CC3P              ((uint8_t) (0x01 << 1))        ///< TIM2 Capture/compare 3 output polarity [0] (in _TIM2_CCER2)
  // reserved [7:2]

  /*  TIM2 prescaler (TIM2_PSCR) */
  #define _TIM2_PSC               ((uint8_t) (0x0F << 0))        ///< TIM2 prescaler [3:0] (in _TIM2_PSCR)
  #define _TIM2_PSC0              ((uint8_t) (0x01 << 0))        ///< TIM2 prescaler [0] (in _TIM2_PSCR)
  #define _TIM2_PSC1              ((uint8_t) (0x01 << 1))        ///< TIM2 prescaler [1] (in _TIM2_PSCR)
  #define _TIM2_PSC2              ((uint8_t) (0x01 << 2))        ///< TIM2 prescaler [2] (in _TIM2_PSCR)
  #define _TIM2_PSC3              ((uint8_t) (0x01 << 3))        ///< TIM2 prescaler [3] (in _TIM2_PSCR)
  // reserved [7:6]

#endif // TIM2_AddressBase



//------------------------
// 16-Bit Timer 3 (TIM3)
//------------------------
#if defined(TIM3_AddressBase)

  /** @brief struct for controlling 16-Bit Timer 3 (TIM3) */
  typedef struct {

    /** @brief TIM3 Control register 1 (TIM3_CR1) */
    struct {
      _BITS   CEN     : 1;    ///< Counter enable
      _BITS   UDIS    : 1;    ///< Update disable
      _BITS   URS     : 1;    ///< Update request source
      _BITS   OPM     : 1;    ///< One-pulse mode
      _BITS   res     : 3;    ///< Reserved
      _BITS   ARPE    : 1;    ///< Auto-reload preload enable
    } CR1;


    /** @brief TIM3 Interrupt enable register (TIM3_IER) */
    struct {
      _BITS   UIE     : 1;    ///< Update interrupt enable
      _BITS   CC1IE   : 1;    ///< Capture/compare 1 interrupt enable
      _BITS   CC2IE   : 1;    ///< Capture/compare 2 interrupt enable
      _BITS   res     : 5;    ///< Reserved
    } IER;


    /** @brief TIM3 Status register 1 (TIM3_SR1) */
    struct {
      _BITS   UIF     : 1;    ///< Update interrupt flag
      _BITS   CC1IF   : 1;    ///< Capture/compare 1 interrupt flag
      _BITS   CC2IF   : 1;    ///< Capture/compare 2 interrupt flag
      _BITS   res     : 5;    ///< Reserved
    } SR1;


    /** @brief TIM3 Status register 2 (TIM3_SR2) */
    struct {
      _BITS   res     : 1;    ///< Reserved, must be kept cleared
      _BITS   CC1OF   : 1;    ///< Capture/compare 1 overcapture flag
      _BITS   CC2OF   : 1;    ///< Capture/compare 2 overcapture flag
      _BITS   res2    : 5;    ///< Reserved, must be kept cleared
    } SR2;


    /** @brief TIM3 Event generation register (TIM3_EGR) */
    struct {
      _BITS   UG      : 1;    ///< Update generation
      _BITS   CC1G    : 1;    ///< Capture/compare 1 generation
      _BITS   CC2G    : 1;    ///< Capture/compare 2 generation
      _BITS   res     : 5;    ///< Reserved
    } EGR;


    /** @brief TIM3 Capture/compare mode register 1 (TIM3_CCMR1) */
    union {

      /** @brief bitwise access to register (output mode) */
      struct {
        _BITS   CC1S    : 2;    ///< Compare 1 selection
        _BITS   res     : 1;    ///< Reserved
        _BITS   OC1PE   : 1;    ///< Output compare 1 preload enable
        _BITS   OC1M    : 3;    ///< Output compare 1 mode
        _BITS   res2    : 1;    ///< Reserved
      } OUT;

      /** @brief bitwise access to register (input mode) */
      struct {
        _BITS   CC1S    : 2;    ///< Capture 1 selection
        _BITS   IC1PSC  : 2;    ///< Input capture 1 prescaler
        _BITS   IC1F    : 4;    ///< Input capture 1 filter
      } IN;

    } CCMR1;


    /** @brief TIM3 Capture/compare mode register 2 (TIM3_CCMR2) */
    union {

      /** @brief bitwise access to register (output mode) */
      struct {
       _BITS   CC2S    : 2;    ///< Capture/compare 2 selection
       _BITS   res     : 1;    ///< Reserved
       _BITS   OC2PE   : 1;    ///< Output compare 2 preload enable
       _BITS   OC2M    : 3;    ///< Output compare 2 mode
       _BITS   res2    : 1;    ///< Reserved
     } OUT;

      /** @brief bitwise access to register (input mode) */
      struct {
        _BITS   CC2S    : 2;    ///< Capture/compare 2 selection
        _BITS   IC2PSC  : 2;    ///< Input capture 2 prescaler
        _BITS   IC2F    : 4;    ///< Input capture 2 filter
      } IN;

    } CCMR2;


    /** @brief TIM3 Capture/compare enable register 1 (TIM3_CCER1) */
    struct {
      _BITS   CC1E    : 1;    ///< Capture/compare 1 output enable
      _BITS   CC1P    : 1;    ///< Capture/compare 1 output polarity
      _BITS   res     : 2;    ///< Reserved
      _BITS   CC2E    : 1;    ///< Capture/compare 2 output enable
      _BITS   CC2P    : 1;    ///< Capture/compare 2 output polarity
      _BITS   res2    : 2;    ///< Reserved
    } CCER1;


    /** @brief TIM3 16-bit counter high byte (TIM3_CNTRH) */
    struct {
      _BITS   CNT     : 8;    ///< 16-bit counter [15:8]
    } CNTRH;


    /** @brief TIM3 16-bit counter low byte (TIM3_CNTRL) */
    struct {
      _BITS   CNT     : 8;    ///< 16-bit counter [7:0]
    } CNTRL;


    /** @brief TIM3 16-bit prescaler high byte (TIM3_PSCR) */
    struct {
      _BITS   PSC     : 4;    ///< prescaler [3:0]
      _BITS   res     : 4;    ///< Reserved
    } PSCR;


    /** @brief TIM3 16-bit auto-reload value high byte (TIM3_ARRH) */
    struct {
      _BITS   ARR     : 8;    ///< 16-bit auto-reload value [15:8]
    } ARRH;


    /** @brief TIM3 16-bit auto-reload value low byte (TIM3_ARRL) */
    struct {
      _BITS   ARR     : 8;    ///< 16-bit auto-reload value [7:0]
    } ARRL;


    /** @brief TIM3 16-bit capture/compare value 1 high byte (TIM3_CCR1H) */
    struct {
      _BITS   CCR1    : 8;    ///< 16-bit capture/compare value 1 [15:8]
    } CCR1H;


    /** @brief TIM3 16-bit capture/compare value 1 low byte (TIM3_CCR1L) */
    struct {
      _BITS   CCR1    : 8;    ///< 16-bit capture/compare value 1 [7:0]
    } CCR1L;


    /** @brief TIM3 16-bit capture/compare value 2 high byte (TIM3_CCR2H) */
    struct {
      _BITS   CCR2    : 8;  ///< 16-bit capture/compare value 2 [15:8]
    } CCR2H;


    /** @brief TIM3 16-bit capture/compare value 2 low byte (TIM3_CCR2L) */
    struct {
      _BITS   CCR2    : 8;    ///< 16-bit capture/compare value 2 [7:0]
    } CCR2L;

  } TIM3_t;

  /* Pointer to TIM3 registers */
  #define _TIM3        _SFR(TIM3_t,    TIM3_AddressBase)         ///< TIM3 struct/bit access
  #define _TIM3_CR1    _SFR(uint8_t,   TIM3_AddressBase+0x00)    ///< TIM3 control register 1
  #define _TIM3_IER    _SFR(uint8_t,   TIM3_AddressBase+0x01)    ///< TIM3 interrupt enable register
  #define _TIM3_SR1    _SFR(uint8_t,   TIM3_AddressBase+0x02)    ///< TIM3 status register 1
  #define _TIM3_SR2    _SFR(uint8_t,   TIM3_AddressBase+0x03)    ///< TIM3 status register 2
  #define _TIM3_EGR    _SFR(uint8_t,   TIM3_AddressBase+0x04)    ///< TIM3 Event generation register
  #define _TIM3_CCMR1  _SFR(uint8_t,   TIM3_AddressBase+0x05)    ///< TIM3 Capture/compare mode register 1
  #define _TIM3_CCMR2  _SFR(uint8_t,   TIM3_AddressBase+0x06)    ///< TIM3 Capture/compare mode register 2
  #define _TIM3_CCER1  _SFR(uint8_t,   TIM3_AddressBase+0x08)    ///< TIM3 Capture/compare enable register 1
  #define _TIM3_CNTRH  _SFR(uint8_t,   TIM3_AddressBase+0x0A)    ///< TIM3 counter register high byte
  #define _TIM3_CNTRL  _SFR(uint8_t,   TIM3_AddressBase+0x0B)    ///< TIM3 counter register low byte
  #define _TIM3_PSCR   _SFR(uint8_t,   TIM3_AddressBase+0x0C)    ///< TIM3 clock prescaler register
  #define _TIM3_ARRH   _SFR(uint8_t,   TIM3_AddressBase+0x0D)    ///< TIM3 auto-reload register high byte
  #define _TIM3_ARRL   _SFR(uint8_t,   TIM3_AddressBase+0x0E)    ///< TIM3 auto-reload register low byte
  #define _TIM3_CCR1H  _SFR(uint8_t,   TIM3_AddressBase+0x0F)    ///< TIM3 16-bit capture/compare value 1 high byte
  #define _TIM3_CCR1L  _SFR(uint8_t,   TIM3_AddressBase+0x10)    ///< TIM3 16-bit capture/compare value 1 low byte
  #define _TIM3_CCR2H  _SFR(uint8_t,   TIM3_AddressBase+0x11)    ///< TIM3 16-bit capture/compare value 2 high byte
  #define _TIM3_CCR2L  _SFR(uint8_t,   TIM3_AddressBase+0x12)    ///< TIM3 16-bit capture/compare value 2 low byte

  /* TIM3 Module Reset Values */
  #define _TIM3_CR1_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM3 control register 1 reset value
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
  #define _TIM3_CCR1H_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM3 16-bit capture/compare value 1 high byte reset value
  #define _TIM3_CCR1L_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM3 16-bit capture/compare value 1 low byte reset value
  #define _TIM3_CCR2H_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM3 16-bit capture/compare value 2 high byte reset value
  #define _TIM3_CCR2L_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM3 16-bit capture/compare value 2 low byte reset value

  /* TIM3 Control register (TIM3_CR1) */
  #define _TIM3_CEN               ((uint8_t) (0x01 << 0))        ///< TIM3 Counter enable [0] (in _TIM3_CR1)
  #define _TIM3_UDIS              ((uint8_t) (0x01 << 1))        ///< TIM3 Update disable [0] (in _TIM3_CR1)
  #define _TIM3_URS               ((uint8_t) (0x01 << 2))        ///< TIM3 Update request source [0] (in _TIM3_CR1)
  #define _TIM3_OPM               ((uint8_t) (0x01 << 3))        ///< TIM3 One-pulse mode [0] (in _TIM3_CR1)
  // reserved [6:4]
  #define _TIM3_ARPE              ((uint8_t) (0x01 << 7))        ///< TIM3 Auto-reload preload enable [0] (in _TIM3_CR1)

  /* TIM3 Interrupt enable (TIM3_IER) */
  #define _TIM3_UIE               ((uint8_t) (0x01 << 0))        ///< TIM3 Update interrupt enable [0] (in _TIM3_IER)
  #define _TIM3_CC1IE             ((uint8_t) (0x01 << 1))        ///< TIM3 Capture/compare 1 interrupt enable [0] (in _TIM3_IER)
  #define _TIM3_CC2IE             ((uint8_t) (0x01 << 2))        ///< TIM3 Capture/compare 2 interrupt enable [0] (in _TIM3_IER)
  // reserved [7:3]

  /*  TIM3 Status register 1 (TIM3_SR1) */
  #define _TIM3_UIF               ((uint8_t) (0x01 << 0))        ///< TIM3 Update interrupt flag [0] (in _TIM3_SR1)
  #define _TIM3_CC1IF             ((uint8_t) (0x01 << 1))        ///< TIM3 Capture/compare 1 interrupt flag [0] (in _TIM3_SR1)
  #define _TIM3_CC2IF             ((uint8_t) (0x01 << 2))        ///< TIM3 Capture/compare 2 interrupt flag [0] (in _TIM3_SR1)
  // reserved [7:3]

  /*  TIM3 Status register 2 (TIM3_SR2) */
  // reserved [0]
  #define _TIM3_CC1OF             ((uint8_t) (0x01 << 1))        ///< TIM3 Capture/compare 1 overcapture flag [0] (in _TIM3_SR2)
  #define _TIM3_CC2OF             ((uint8_t) (0x01 << 2))        ///< TIM3 Capture/compare 2 overcapture flag [0] (in _TIM3_SR2)
  // reserved [7:3]

  /*  TIM3 Event generation register (TIM3_EGR) */
  #define _TIM3_UG                ((uint8_t) (0x01 << 0))        ///< TIM3 Update generation [0] (in _TIM3_EGR)
  #define _TIM3_CC1G              ((uint8_t) (0x01 << 1))        ///< TIM3 Capture/compare 1 generation [0] (in _TIM3_EGR)
  #define _TIM3_CC2G              ((uint8_t) (0x01 << 2))        ///< TIM3 Capture/compare 2 generation [0] (in _TIM3_EGR)
  // reserved [7:3]

  /*  TIM3 Capture/compare mode register 1 (TIM3_CCMR1). Output mode */
  #define _TIM3_CC1S              ((uint8_t) (0x03 << 0))        ///< TIM3 Compare 1 selection [1:0] (in _TIM3_CCMR1)
  #define _TIM3_CC1S0             ((uint8_t) (0x01 << 0))        ///< TIM3 Compare 1 selection [0] (in _TIM3_CCMR1)
  #define _TIM3_CC1S1             ((uint8_t) (0x01 << 1))        ///< TIM3 Compare 1 selection [1] (in _TIM3_CCMR1)
  // reserved [2]
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
  #define _TIM3_IC1F0             ((uint8_t) (0x01 << 4))        ///< TIM3 Output compare 1 mode [0] (in _TIM3_CCMR1)
  #define _TIM3_IC1F1             ((uint8_t) (0x01 << 5))        ///< TIM3 Output compare 1 mode [1] (in _TIM3_CCMR1)
  #define _TIM3_IC1F2             ((uint8_t) (0x01 << 6))        ///< TIM3 Output compare 1 mode [2] (in _TIM3_CCMR1)
  #define _TIM3_IC1F3             ((uint8_t) (0x01 << 7))        ///< TIM3 Output compare 1 mode [3] (in _TIM3_CCMR1)

  /*  TIM3 Capture/compare mode register 2 (TIM3_CCMR2). Output mode */
  #define _TIM3_CC2S              ((uint8_t) (0x03 << 0))        ///< TIM3 Compare 2 selection [1:0] (in _TIM3_CCMR2)
  #define _TIM3_CC2S0             ((uint8_t) (0x01 << 0))        ///< TIM3 Compare 2 selection [0] (in _TIM3_CCMR2)
  #define _TIM3_CC2S1             ((uint8_t) (0x01 << 1))        ///< TIM3 Compare 2 selection [1] (in _TIM3_CCMR2)
  // reserved [2]
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
  #define _TIM3_IC2F0             ((uint8_t) (0x01 << 4))        ///< TIM3 Output compare 2 mode [0] (in _TIM3_CCMR2)
  #define _TIM3_IC2F1             ((uint8_t) (0x01 << 5))        ///< TIM3 Output compare 2 mode [1] (in _TIM3_CCMR2)
  #define _TIM3_IC2F2             ((uint8_t) (0x01 << 6))        ///< TIM3 Output compare 2 mode [2] (in _TIM3_CCMR2)
  #define _TIM3_IC2F3             ((uint8_t) (0x01 << 7))        ///< TIM3 Output compare 2 mode [3] (in _TIM3_CCMR2)

  /*  TIM3 Capture/compare enable register 1 (TIM3_CCER1) */
  #define _TIM3_CC1E              ((uint8_t) (0x01 << 0))        ///< TIM3 Capture/compare 1 output enable [0] (in _TIM3_CCER1)
  #define _TIM3_CC1P              ((uint8_t) (0x01 << 1))        ///< TIM3 Capture/compare 1 output polarity [0] (in _TIM3_CCER1)
  // reserved [3:2]
  #define _TIM3_CC2E              ((uint8_t) (0x01 << 4))        ///< TIM3 Capture/compare 2 output enable [0] (in _TIM3_CCER1)
  #define _TIM3_CC2P              ((uint8_t) (0x01 << 5))        ///< TIM3 Capture/compare 2 output polarity [0] (in _TIM3_CCER1)
  // reserved [7:6]

  /*  TIM3 prescaler (TIM3_PSCR) */
  #define _TIM3_PSC               ((uint8_t) (0x0F << 0))        ///< TIM3 clock prescaler [3:0] (in _TIM3_PSCR)
  #define _TIM3_PSC0              ((uint8_t) (0x01 << 0))        ///< TIM3 clock prescaler [0] (in _TIM3_PSCR)
  #define _TIM3_PSC1              ((uint8_t) (0x01 << 1))        ///< TIM3 clock prescaler [1] (in _TIM3_PSCR)
  #define _TIM3_PSC2              ((uint8_t) (0x01 << 2))        ///< TIM3 clock prescaler [2] (in _TIM3_PSCR)
  #define _TIM3_PSC3              ((uint8_t) (0x01 << 3))        ///< TIM3 clock prescaler [3] (in _TIM3_PSCR)
  // reserved [7:6]

#endif // TIM3_AddressBase



//------------------------
// 8-Bit Timer 4 (TIM4)
//------------------------
#if defined(TIM4_AddressBase)

  /** @brief struct for controlling 8-Bit Timer 4 (TIM4) */
  typedef struct {

    /** @brief TIM4 Control register (TIM4_CR) */
    struct {
      _BITS   CEN     : 1;    ///< Counter enable
      _BITS   UDIS    : 1;    ///< Update disable
      _BITS   URS     : 1;    ///< Update request source
      _BITS   OPM     : 1;    ///< One-pulse mode
      _BITS   res     : 3;    ///< Reserved
      _BITS   ARPE    : 1;    ///< Auto-reload preload enable
    } CR;


    /** @brief Reserved registers on selected devices (2B) */
    #if defined(STM8S103) || defined(STM8S003) || defined(STM8S001)
      uint8_t   res[2];
    #endif


    /** @brief TIM4 Interrupt enable (TIM4_IER) */
    struct {
      _BITS   UIE     : 1;    ///< Update interrupt enable
      _BITS   res     : 7;    ///< Reserved
    } IER;


    /** @brief TIM4 Status register (TIM4_SR) */
    struct {
      _BITS   UIF     : 1;    ///< Update interrupt flag
      _BITS   res     : 7;    ///< Reserved
    } SR;


    /** @brief TIM4 Event Generation (TIM4_EGR) */
    struct {
      _BITS   UG      : 1;    ///< Update generation
      _BITS   res     : 7;    ///< Reserved
    } EGR;


    /** @brief TIM4 8-bit counter register (TIM4_CNTR) */
    struct {
      _BITS   CNT     : 8;    ///< 8-bit counter
    } CNTR;

    /** @brief TIM4 clock prescaler (TIM4_PSCR) */
    struct {
      _BITS   PSC     : 3;    ///< clock prescaler
      _BITS   res     : 5;    ///< Reserved
    } PSCR;


    /** @brief TIM4 8-bit auto-reload register (TIM4_ARR) */
    struct {
      _BITS   ARR     : 8;    ///< auto-reload value
    } ARR;

  } TIM4_t;

  /* Pointer to TIM4 registers */
  #define _TIM4      _SFR(TIM4_t,      TIM4_AddressBase)         ///< TIM4 struct/bit access
  #define _TIM4_CR   _SFR(uint8_t,     TIM4_AddressBase+0x00)    ///< TIM4 control register
  #if defined(STM8S103) || defined(STM8S003) || defined(STM8S001)  // 2B offset for selected devices
    // 2B reserved
    #define _TIM4_IER   _SFR(uint8_t,  TIM4_AddressBase+0x03)    ///< TIM4 interrupt enable register
    #define _TIM4_SR    _SFR(uint8_t,  TIM4_AddressBase+0x04)    ///< TIM4 status register
    #define _TIM4_EGR   _SFR(uint8_t,  TIM4_AddressBase+0x05)    ///< TIM4 event generation register
    #define _TIM4_CNTR  _SFR(uint8_t,  TIM4_AddressBase+0x06)    ///< TIM4 counter register
    #define _TIM4_PSCR  _SFR(uint8_t,  TIM4_AddressBase+0x07)    ///< TIM4 clock prescaler register
    #define _TIM4_ARR   _SFR(uint8_t,  TIM4_AddressBase+0x08)    ///< TIM4 auto-reload register
  #else
    #define _TIM4_IER   _SFR(uint8_t,  TIM4_AddressBase+0x01)    ///< TIM4 interrupt enable register
    #define _TIM4_SR    _SFR(uint8_t,  TIM4_AddressBase+0x02)    ///< TIM4 status register
    #define _TIM4_EGR   _SFR(uint8_t,  TIM4_AddressBase+0x03)    ///< TIM4 event generation register
    #define _TIM4_CNTR  _SFR(uint8_t,  TIM4_AddressBase+0x04)    ///< TIM4 counter register
    #define _TIM4_PSCR  _SFR(uint8_t,  TIM4_AddressBase+0x05)    ///< TIM4 clock prescaler register
    #define _TIM4_ARR   _SFR(uint8_t,  TIM4_AddressBase+0x06)    ///< TIM4 auto-reload register
  #endif

  /* TIM4 Module Reset Values */
  #define _TIM4_CR_RESET_VALUE         ((uint8_t) 0x00)          ///< TIM4 control register reset value
  #define _TIM4_IER_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM4 interrupt enable register reset value
  #define _TIM4_SR_RESET_VALUE         ((uint8_t) 0x00)          ///< TIM4 status register reset value
  #define _TIM4_EGR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM4 event generation register reset value
  #define _TIM4_CNTR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM4 counter register reset value
  #define _TIM4_PSCR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM4 clock prescaler register reset value
  #define _TIM4_ARR_RESET_VALUE        ((uint8_t) 0xFF)          ///< TIM4 auto-reload register reset value

  /* TIM4 Control register (TIM4_CR) */
  #define _TIM4_CEN               ((uint8_t) (0x01 << 0))        ///< TIM4 Counter enable [0] (in _TIM4_CR)
  #define _TIM4_UDIS              ((uint8_t) (0x01 << 1))        ///< TIM4 Update disable [0] (in _TIM4_CR)
  #define _TIM4_URS               ((uint8_t) (0x01 << 2))        ///< TIM4 Update request source [0] (in _TIM4_CR)
  #define _TIM4_OPM               ((uint8_t) (0x01 << 3))        ///< TIM4 One-pulse mode [0] (in _TIM4_CR)
  // reserved [6:4]
  #define _TIM4_ARPE              ((uint8_t) (0x01 << 7))        ///< TIM4 Auto-reload preload enable [0] (in _TIM4_CR)

  /*  TIM4 Interrupt enable (TIM4_IER) */
  #define _TIM4_UIE               ((uint8_t) (0x01 << 0))        ///< TIM4 Update interrupt enable [0] (in _TIM4_IER)
  // reserved [7:1]

  /*  TIM4 Status register (TIM4_SR) */
  #define _TIM4_UIF               ((uint8_t) (0x01 << 0))        ///< TIM4 Update interrupt flag [0] (in _TIM4_SR)
  // reserved [7:1]

  /*  TIM4 Event generation register (TIM4_EGR) */
  #define _TIM4_UG                ((uint8_t) (0x01 << 0))        ///< TIM4 Update generation [0] (in _TIM4_EGR)
  // reserved [7:1]

  /* TIM4 Prescaler register (TIM4_PSCR) */
  #define _TIM4_PSC               ((uint8_t) (0x07 << 0))        ///< TIM4 clock prescaler [2:0] (in _TIM4_PSCR)
  #define _TIM4_PSC0              ((uint8_t) (0x01 << 0))        ///< TIM4 clock prescaler [0] (in _TIM4_PSCR)
  #define _TIM4_PSC1              ((uint8_t) (0x01 << 1))        ///< TIM4 clock prescaler [1] (in _TIM4_PSCR)
  #define _TIM4_PSC2              ((uint8_t) (0x01 << 2))        ///< TIM4 clock prescaler [2] (in _TIM4_PSCR)
  // reserved [7:3]

#endif // TIM4_AddressBase



//------------------------
// 16-Bit Timer 5 (TIM5)
//------------------------
#if defined(TIM5_AddressBase)

  /** @brief struct for controlling 16-Bit Timer 5 (TIM5) */
  typedef struct {

    /** @brief TIM5 Control register 1 (TIM5_CR1) */
    struct {
      _BITS   CEN     : 1;    ///< Counter enable
      _BITS   UDIS    : 1;    ///< Update disable
      _BITS   URS     : 1;    ///< Update request source
      _BITS   OPM     : 1;    ///< One-pulse mode
      _BITS   res     : 3;    ///< Reserved
      _BITS   ARPE    : 1;    ///< Auto-reload preload enable
    } CR1;


    /** @brief TIM5 Control register 2 (TIM5_CR2) */
    struct {
      _BITS   CCPC    : 1;    ///< Capture/compare preloaded control
      _BITS   res     : 1;    ///< Reserved, forced by hardware to 0
      _BITS   COMS    : 1;    ///< Capture/compare control update selection
      _BITS   res2    : 1;    ///< Reserved, must be kept cleared
      _BITS   MMS     : 3;    ///< Master mode selection
      _BITS   res3    : 1;    ///< Reserved
    } CR2;


    /** @brief Slave mode control register (TIM5_SMCR) */
    struct {
      _BITS   SMS     : 3;    ///< Clock/trigger/slave mode selection
      _BITS   res     : 1;    ///< Reserved
      _BITS   TS      : 3;    ///< Trigger selection
      _BITS   MSM     : 1;    ///< Master/slave mode
    } SMCR;


    /** @brief TIM5 Interrupt enable register (TIM5_IER) */
    struct {
      _BITS   UIE     : 1;    ///< Update interrupt enable
      _BITS   CC1IE   : 1;    ///< Capture/compare 1 interrupt enable
      _BITS   CC2IE   : 1;    ///< Capture/compare 2 interrupt enable
      _BITS   CC3IE   : 1;    ///< Capture/compare 3 interrupt enable
      _BITS   res     : 2;    ///< Reserved
      _BITS   TIE     : 1;    ///< Trigger interrupt enable
      _BITS   res2    : 1;    ///< Reserved
    } IER;


    /** @brief TIM5 Status register 1 (TIM5_SR1) */
    struct {
      _BITS   UIF     : 1;    ///< Update interrupt flag
      _BITS   CC1IF   : 1;    ///< Capture/compare 1 interrupt flag
      _BITS   CC2IF   : 1;    ///< Capture/compare 2 interrupt flag
      _BITS   CC3IF   : 1;    ///< Capture/compare 3 interrupt flag
      _BITS   res     : 2;    ///< Reserved
      _BITS   TIF     : 1;    ///< Trigger interrupt flag
      _BITS   res2    : 1;    ///< Reserved
    } SR1;


    /** @brief TIM5 Status register 2 (TIM5_SR2) */
    struct {
      _BITS   res     : 1;    ///< Reserved
      _BITS   CC1OF   : 1;    ///< Capture/compare 1 overcapture flag
      _BITS   CC2OF   : 1;    ///< Capture/compare 2 overcapture flag
      _BITS   CC3OF   : 1;    ///< Capture/compare 3 overcapture flag
      _BITS   res2    : 4;    ///< Reserved
    } SR2;


    /** @brief TIM5 Event generation register (TIM5_EGR) */
    struct {
      _BITS   UG      : 1;    ///< Update generation
      _BITS   CC1G    : 1;    ///< Capture/compare 1 generation
      _BITS   CC2G    : 1;    ///< Capture/compare 2 generation
      _BITS   CC3G    : 1;    ///< Capture/compare 3 generation
      _BITS   res     : 2;    ///< Reserved
      _BITS   TG      : 1;    ///< Trigger generation
      _BITS   res2    : 1;    ///< Reserved
    } EGR;


    /** @brief TIM5 Capture/compare mode register 1 (TIM5_CCMR1) */
    union {

      /** @brief bitwise access to register (output mode) */
      struct {
        _BITS   CC1S    : 2;    ///< Compare 1 selection
        _BITS   res     : 1;    ///< Reserved
        _BITS   OC1PE   : 1;    ///< Output compare 1 preload enable
        _BITS   OC1M    : 3;    ///< Output compare 1 mode
        _BITS   res2    : 1;    ///< Reserved
      } OUT;

      /** @brief bitwise access to register (input mode) */
      struct {
        _BITS   CC1S    : 2;    ///< Capture 1 selection
        _BITS   IC1PSC  : 2;    ///< Input capture 1 prescaler
        _BITS   IC1F    : 4;    ///< Input capture 1 filter
      } IN;

    } CCMR1;


    /** @brief TIM5 Capture/compare mode register 2 (TIM5_CCMR2) */
    union {

      /** @brief bitwise access to register (output mode) */
      struct {
       _BITS   CC2S    : 2;    ///< Capture/compare 2 selection
       _BITS   res     : 1;    ///< Reserved
       _BITS   OC2PE   : 1;    ///< Output compare 2 preload enable
       _BITS   OC2M    : 3;    ///< Output compare 2 mode
       _BITS   res2    : 1;    ///< Reserved
     } OUT;

      /** @brief bitwise access to register (input mode) */
      struct {
        _BITS   CC2S    : 2;    ///< Capture/compare 2 selection
        _BITS   IC2PSC  : 2;    ///< Input capture 2 prescaler
        _BITS   IC2F    : 4;    ///< Input capture 2 filter
      } IN;

    } CCMR2;


    /** @brief TIM5 Capture/compare mode register 3 (TIM5_CCMR3) */
    union {

      /** @brief bitwise access to register (output mode) */
      struct {
        _BITS   CC3S    : 2;    ///< Capture/compare 3 selection
        _BITS   res     : 1;    ///< Reserved
        _BITS   OC3PE   : 1;    ///< Output compare 3 preload enable
        _BITS   OC3M    : 3;    ///< Output compare 3 mode
        _BITS   res2    : 1;    ///< Reserved
      } OUT;

      /** @brief bitwise access to register (input mode) */
      struct {
        _BITS   CC3S    : 2;    ///< Capture/compare 3 selection
        _BITS   IC3PSC  : 2;    ///< Input capture 3 prescaler
        _BITS   IC3F    : 4;    ///< Input capture 3 filter
      } IN;

    } CCMR3;


    /** @brief TIM5 Capture/compare enable register 1 (TIM5_CCER1) */
    struct {
      _BITS   CC1E    : 1;    ///< Capture/compare 1 output enable
      _BITS   CC1P    : 1;    ///< Capture/compare 1 output polarity
      _BITS   res     : 2;    ///< Reserved
      _BITS   CC2E    : 1;    ///< Capture/compare 2 output enable
      _BITS   CC2P    : 1;    ///< Capture/compare 2 output polarity
      _BITS   res2    : 2;    ///< Reserved
    } CCER1;


    /** @brief TIM5 Capture/compare enable register 2 (TIM5_CCER2) */
    struct {
      _BITS   CC3E    : 1;    ///< Capture/compare 3 output enable
      _BITS   CC3P    : 1;    ///< Capture/compare 3 output polarity
      _BITS   res     : 6;    ///< Reserved
    } CCER2;


    /** @brief TIM5 16-bit counter high byte (TIM5_CNTRH) */
    struct {
      _BITS   CNT     : 8;    ///< 16-bit counter [15:8]
    } CNTRH;


    /** @brief TIM5 16-bit counter low byte (TIM5_CNTRL) */
    struct {
      _BITS   CNT     : 8;    ///< 16-bit counter [7:0]
    } CNTRL;


    /** @brief TIM5 prescaler (TIM5_PSCR) */
    struct {
      _BITS   PSC     : 4;    ///< clock prescaler
      _BITS   res     : 4;    ///< Reserved
    } PSCR;


    /** @brief TIM5 16-bit auto-reload value high byte (TIM5_ARRH) */
    struct {
      _BITS   ARR     : 8;  ///< 16-bit auto-reload value [15:8]
    } ARRH;


    /** @brief TIM5 16-bit auto-reload value low byte (TIM5_ARRL) */
    struct {
      _BITS   ARR     : 8;  ///< 16-bit auto-reload value [7:0]
    } ARRL;


    /** @brief TIM5 16-bit capture/compare value 1 high byte (TIM5_CCR1H) */
    struct {
      _BITS   CCR1    : 8;  ///< 16-bit capture/compare value 1 [15:8]
    } CCR1H;


    /** @brief TIM5 16-bit capture/compare value 1 low byte (TIM5_CCR1L) */
    struct {
      _BITS   CCR1    : 8;  ///< 16-bit capture/compare value 1 [7:0]
    } CCR1L;


    /** @brief TIM5 16-bit capture/compare value 2 high byte (TIM5_CCR2H) */
    struct {
      _BITS   CCR2    : 8;  ///< 16-bit capture/compare value 2 [15:8]
    } CCR2H;


    /** @brief TIM5 16-bit capture/compare value 2 low byte (TIM5_CCR2L) */
    struct {
      _BITS   CCR2    : 8;  ///< 16-bit capture/compare value 2 [7:0]
    } CCR2L;


    /** @brief TIM5 16-bit capture/compare value 3 high byte (TIM5_CCR3H) */
    struct {
      _BITS   CCR3    : 8;  ///< 16-bit capture/compare value 3 [15:8]
    } CCR3H;


    /** @brief TIM5 16-bit capture/compare value 3 low byte (TIM5_CCR3L) */
    struct {
      _BITS   CCR3    : 8;  ///< 16-bit capture/compare value 3 [7:0]
    } CCR3L;

  } TIM5_t;

  /* Pointer to TIM5 registers */
  #define _TIM5        _SFR(TIM5_t,    TIM5_AddressBase)         ///< TIM5 struct/bit access
  #define _TIM5_CR1    _SFR(uint8_t,   TIM5_AddressBase+0x00)    ///< TIM5 control register 1
  #define _TIM5_CR2    _SFR(uint8_t,   TIM5_AddressBase+0x01)    ///< TIM5 control register 2
  #define _TIM5_SMCR   _SFR(uint8_t,   TIM5_AddressBase+0x02)    ///< TIM5 Slave mode control register
  #define _TIM5_IER    _SFR(uint8_t,   TIM5_AddressBase+0x03)    ///< TIM5 interrupt enable register
  #define _TIM5_SR1    _SFR(uint8_t,   TIM5_AddressBase+0x04)    ///< TIM5 status register 1
  #define _TIM5_SR2    _SFR(uint8_t,   TIM5_AddressBase+0x05)    ///< TIM5 status register 2
  #define _TIM5_EGR    _SFR(uint8_t,   TIM5_AddressBase+0x06)    ///< TIM5 Event generation register
  #define _TIM5_CCMR1  _SFR(uint8_t,   TIM5_AddressBase+0x07)    ///< TIM5 Capture/compare mode register 1
  #define _TIM5_CCMR2  _SFR(uint8_t,   TIM5_AddressBase+0x08)    ///< TIM5 Capture/compare mode register 2
  #define _TIM5_CCMR3  _SFR(uint8_t,   TIM5_AddressBase+0x09)    ///< TIM5 Capture/compare mode register 3
  #define _TIM5_CCER1  _SFR(uint8_t,   TIM5_AddressBase+0x0A)    ///< TIM5 Capture/compare enable register 1
  #define _TIM5_CCER2  _SFR(uint8_t,   TIM5_AddressBase+0x0B)    ///< TIM5 Capture/compare enable register 2
  #define _TIM5_CNTRH  _SFR(uint8_t,   TIM5_AddressBase+0x0C)    ///< TIM5 counter register high byte
  #define _TIM5_CNTRL  _SFR(uint8_t,   TIM5_AddressBase+0x0D)    ///< TIM5 counter register low byte
  #define _TIM5_PSCR   _SFR(uint8_t,   TIM5_AddressBase+0x0E)    ///< TIM5 clock prescaler register
  #define _TIM5_ARRH   _SFR(uint8_t,   TIM5_AddressBase+0x0F)    ///< TIM5 auto-reload register high byte
  #define _TIM5_ARRL   _SFR(uint8_t,   TIM5_AddressBase+0x10)    ///< TIM5 auto-reload register low byte
  #define _TIM5_CCR1H  _SFR(uint8_t,   TIM5_AddressBase+0x11)    ///< TIM5 16-bit capture/compare value 1 high byte
  #define _TIM5_CCR1L  _SFR(uint8_t,   TIM5_AddressBase+0x12)    ///< TIM5 16-bit capture/compare value 1 low byte
  #define _TIM5_CCR2H  _SFR(uint8_t,   TIM5_AddressBase+0x13)    ///< TIM5 16-bit capture/compare value 2 high byte
  #define _TIM5_CCR2L  _SFR(uint8_t,   TIM5_AddressBase+0x14)    ///< TIM5 16-bit capture/compare value 2 low byte
  #define _TIM5_CCR3H  _SFR(uint8_t,   TIM5_AddressBase+0x15)    ///< TIM5 16-bit capture/compare value 3 high byte
  #define _TIM5_CCR3L  _SFR(uint8_t,   TIM5_AddressBase+0x16)    ///< TIM5 16-bit capture/compare value 3 low byte

  /* TIM5 Module Reset Values */
  #define _TIM5_CR1_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM5 control register 1 reset value
  #define _TIM5_CR2_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM5 control register 2 reset value
  #define _TIM5_SMCR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM5 Slave mode control register reset value
  #define _TIM5_IER_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM5 interrupt enable register reset value
  #define _TIM5_SR1_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM5 status register 1 reset value
  #define _TIM5_SR2_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM5 status register 2 reset value
  #define _TIM5_EGR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM5 Event generation register reset value
  #define _TIM5_CCMR1_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM5 Capture/compare mode register 1 reset value
  #define _TIM5_CCMR2_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM5 Capture/compare mode register 2 reset value
  #define _TIM5_CCMR3_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM5 Capture/compare mode register 3 reset value
  #define _TIM5_CCER1_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM5 Capture/compare enable register 1 reset value
  #define _TIM5_CCER2_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM5 Capture/compare enable register 2 reset value
  #define _TIM5_CNTRH_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM5 counter register high byte reset value
  #define _TIM5_CNTRL_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM5 counter register low byte reset value
  #define _TIM5_PSCR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM5 clock prescaler register reset value
  #define _TIM5_ARRH_RESET_VALUE       ((uint8_t) 0xFF)          ///< TIM5 auto-reload register high byte reset value
  #define _TIM5_ARRL_RESET_VALUE       ((uint8_t) 0xFF)          ///< TIM5 auto-reload register low byte reset value
  #define _TIM5_CCR1H_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM5 16-bit capture/compare value 1 high byte reset value
  #define _TIM5_CCR1L_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM5 16-bit capture/compare value 1 low byte reset value
  #define _TIM5_CCR2H_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM5 16-bit capture/compare value 2 high byte reset value
  #define _TIM5_CCR2L_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM5 16-bit capture/compare value 2 low byte reset value
  #define _TIM5_CCR3H_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM5 16-bit capture/compare value 3 high byte reset value
  #define _TIM5_CCR3L_RESET_VALUE      ((uint8_t) 0x00)          ///< TIM5 16-bit capture/compare value 3 low byte reset value

  /* TIM5 Control register (TIM5_CR1) */
  #define _TIM5_CEN               ((uint8_t) (0x01 << 0))        ///< TIM5 Counter enable [0] (in _TIM5_CR1)
  #define _TIM5_UDIS              ((uint8_t) (0x01 << 1))        ///< TIM5 Update disable [0] (in _TIM5_CR1)
  #define _TIM5_URS               ((uint8_t) (0x01 << 2))        ///< TIM5 Update request source [0] (in _TIM5_CR1)
  #define _TIM5_OPM               ((uint8_t) (0x01 << 3))        ///< TIM5 One-pulse mode [0] (in _TIM5_CR1)
  // reserved [6:4]
  #define _TIM5_ARPE              ((uint8_t) (0x01 << 7))        ///< TIM5 Auto-reload preload enable [0] (in _TIM5_CR1)

  /* TIM5 Control register (TIM5_CR2) */
  #define _TIM5_CCPC              ((uint8_t) (0x01 << 0))        ///< TIM5 Capture/compare preloaded control [0] (in _TIM5_CR2)
  // reserved [1]
  #define _TIM5_COMS              ((uint8_t) (0x01 << 2))        ///< TIM5 Capture/compare control update selection [0] (in _TIM5_CR2)
  // reserved [3]
  #define _TIM5_MMS               ((uint8_t) (0x07 << 4))        ///< TIM5 Master mode selection [2:0] (in _TIM5_CR2)
  #define _TIM5_MMS0              ((uint8_t) (0x01 << 4))        ///< TIM5 Master mode selection [0] (in _TIM5_CR2)
  #define _TIM5_MMS1              ((uint8_t) (0x01 << 5))        ///< TIM5 Master mode selection [1] (in _TIM5_CR2)
  #define _TIM5_MMS2              ((uint8_t) (0x01 << 6))        ///< TIM5 Master mode selection [2] (in _TIM5_CR2)
  // reserved [7]

  /* TIM5 Slave mode control register (TIM5_SMCR) */
  #define _TIM5_SMS               ((uint8_t) (0x07 << 0))        ///< TIM5 Clock/trigger/slave mode selection [2:0] (in _TIM5_SMCR)
  #define _TIM5_SMS0              ((uint8_t) (0x01 << 0))        ///< TIM5 Clock/trigger/slave mode selection [0] (in _TIM5_SMCR)
  #define _TIM5_SMS1              ((uint8_t) (0x01 << 1))        ///< TIM5 Clock/trigger/slave mode selection [1] (in _TIM5_SMCR)
  #define _TIM5_SMS2              ((uint8_t) (0x01 << 2))        ///< TIM5 Clock/trigger/slave mode selection [2] (in _TIM5_SMCR)
  // reserved [3]
  #define _TIM5_TS                ((uint8_t) (0x07 << 4))        ///< TIM5 Trigger selection [2:0] (in _TIM5_SMCR)
  #define _TIM5_TS0               ((uint8_t) (0x01 << 4))        ///< TIM5 Trigger selection [0] (in _TIM5_SMCR)
  #define _TIM5_TS1               ((uint8_t) (0x01 << 5))        ///< TIM5 Trigger selection [1] (in _TIM5_SMCR)
  #define _TIM5_TS2               ((uint8_t) (0x01 << 6))        ///< TIM5 Trigger selection [2] (in _TIM5_SMCR)
  #define _TIM5_MSM               ((uint8_t) (0x01 << 7))        ///< TIM5 Master/slave mode [0] (in _TIM5_SMCR)

  /* TIM5 Interrupt enable (TIM5_IER) */
  #define _TIM5_UIE               ((uint8_t) (0x01 << 0))        ///< TIM5 Update interrupt enable [0] (in _TIM5_IER)
  #define _TIM5_CC1IE             ((uint8_t) (0x01 << 1))        ///< TIM5 Capture/compare 1 interrupt enable [0] (in _TIM5_IER)
  #define _TIM5_CC2IE             ((uint8_t) (0x01 << 2))        ///< TIM5 Capture/compare 2 interrupt enable [0] (in _TIM5_IER)
  #define _TIM5_CC3IE             ((uint8_t) (0x01 << 3))        ///< TIM5 Capture/compare 3 interrupt enable [0] (in _TIM5_IER)
  // reserved [5:4]
  #define _TIM5_TIE               ((uint8_t) (0x01 << 6))        ///< TIM5 Trigger interrupt enable [0] (in _TIM5_IER)
  // reserved [7]

  /*  TIM5 Status register 1 (TIM5_SR1) */
  #define _TIM5_UIF               ((uint8_t) (0x01 << 0))        ///< TIM5 Update interrupt flag [0] (in _TIM5_SR1)
  #define _TIM5_CC1IF             ((uint8_t) (0x01 << 1))        ///< TIM5 Capture/compare 1 interrupt flag [0] (in _TIM5_SR1)
  #define _TIM5_CC2IF             ((uint8_t) (0x01 << 2))        ///< TIM5 Capture/compare 2 interrupt flag [0] (in _TIM5_SR1)
  #define _TIM5_CC3IF             ((uint8_t) (0x01 << 3))        ///< TIM5 Capture/compare 3 interrupt flag [0] (in _TIM5_SR1)
  // reserved [5:4]
  #define _TIM5_TIF               ((uint8_t) (0x01 << 6))        ///< TIM5 Trigger interrupt flag [0] (in _TIM5_SR1)
  // reserved [7]

  /*  TIM5 Status register 2 (TIM5_SR2) */
  // reserved [0]
  #define _TIM5_CC1OF             ((uint8_t) (0x01 << 1))        ///< TIM5 Capture/compare 1 overcapture flag [0] (in _TIM5_SR2)
  #define _TIM5_CC2OF             ((uint8_t) (0x01 << 2))        ///< TIM5 Capture/compare 2 overcapture flag [0] (in _TIM5_SR2)
  #define _TIM5_CC3OF             ((uint8_t) (0x01 << 3))        ///< TIM5 Capture/compare 3 overcapture flag [0] (in _TIM5_SR2)
  // reserved [7:4]

  /*  TIM5 Event generation register (TIM5_EGR) */
  #define _TIM5_UG                ((uint8_t) (0x01 << 0))        ///< TIM5 Update generation [0] (in _TIM5_EGR)
  #define _TIM5_CC1G              ((uint8_t) (0x01 << 1))        ///< TIM5 Capture/compare 1 generation [0] (in _TIM5_EGR)
  #define _TIM5_CC2G              ((uint8_t) (0x01 << 2))        ///< TIM5 Capture/compare 2 generation [0] (in _TIM5_EGR)
  #define _TIM5_CC3G              ((uint8_t) (0x01 << 3))        ///< TIM5 Capture/compare 3 generation [0] (in _TIM5_EGR)
  // reserved [5:4]
  #define _TIM5_TG                ((uint8_t) (0x01 << 6))        ///< TIM5 Trigger generation [0] (in _TIM5_EGR)
  // reserved [7]

  /*  TIM5 Capture/compare mode register 1 (TIM5_CCMR1). Output mode */
  #define _TIM5_CC1S              ((uint8_t) (0x03 << 0))        ///< TIM5 Compare 1 selection [1:0] (in _TIM5_CCMR1)
  #define _TIM5_CC1S0             ((uint8_t) (0x01 << 0))        ///< TIM5 Compare 1 selection [0] (in _TIM5_CCMR1)
  #define _TIM5_CC1S1             ((uint8_t) (0x01 << 1))        ///< TIM5 Compare 1 selection [1] (in _TIM5_CCMR1)
  // reserved [2]
  #define _TIM5_OC1PE             ((uint8_t) (0x01 << 3))        ///< TIM5 Output compare 1 preload enable [0] (in _TIM5_CCMR1)
  #define _TIM5_OC1M              ((uint8_t) (0x07 << 4))        ///< TIM5 Output compare 1 mode [2:0] (in _TIM5_CCMR1)
  #define _TIM5_OC1M0             ((uint8_t) (0x01 << 4))        ///< TIM5 Output compare 1 mode [0] (in _TIM5_CCMR1)
  #define _TIM5_OC1M1             ((uint8_t) (0x01 << 5))        ///< TIM5 Output compare 1 mode [1] (in _TIM5_CCMR1)
  #define _TIM5_OC1M2             ((uint8_t) (0x01 << 6))        ///< TIM5 Output compare 1 mode [2] (in _TIM5_CCMR1)
  // reserved [7]

  /*  TIM5 Capture/compare mode register 1 (TIM5_CCMR1). Input mode */
  // _TIM5_CC1S [1:0] defined above
  #define _TIM5_IC1PSC            ((uint8_t) (0x03 << 2))        ///< TIM5 Input capture 1 prescaler [1:0] (in _TIM5_CCMR1)
  #define _TIM5_IC1PSC0           ((uint8_t) (0x01 << 2))        ///< TIM5 Input capture 1 prescaler [0] (in _TIM5_CCMR1)
  #define _TIM5_IC1PSC1           ((uint8_t) (0x01 << 3))        ///< TIM5 Input capture 1 prescaler [1] (in _TIM5_CCMR1)
  #define _TIM5_IC1F              ((uint8_t) (0x0F << 4))        ///< TIM5 Output compare 1 mode [3:0] (in _TIM5_CCMR1)
  #define _TIM5_IC1F0             ((uint8_t) (0x01 << 4))        ///< TIM5 Input capture 1 filter [0] (in _TIM5_CCMR1)
  #define _TIM5_IC1F1             ((uint8_t) (0x01 << 5))        ///< TIM5 Input capture 1 filter [1] (in _TIM5_CCMR1)
  #define _TIM5_IC1F2             ((uint8_t) (0x01 << 6))        ///< TIM5 Input capture 1 filter [2] (in _TIM5_CCMR1)
  #define _TIM5_IC1F3             ((uint8_t) (0x01 << 7))        ///< TIM5 Input capture 1 filter [3] (in _TIM5_CCMR1)

  /*  TIM5 Capture/compare mode register 2 (TIM5_CCMR2). Output mode */
  #define _TIM5_CC2S              ((uint8_t) (0x03 << 0))        ///< TIM5 Compare 2 selection [1:0] (in _TIM5_CCMR2)
  #define _TIM5_CC2S0             ((uint8_t) (0x01 << 0))        ///< TIM5 Compare 2 selection [0] (in _TIM5_CCMR2)
  #define _TIM5_CC2S1             ((uint8_t) (0x01 << 1))        ///< TIM5 Compare 2 selection [1] (in _TIM5_CCMR2)
  // reserved [2]
  #define _TIM5_OC2PE             ((uint8_t) (0x01 << 3))        ///< TIM5 Output compare 2 preload enable [0] (in _TIM5_CCMR2)
  #define _TIM5_OC2M              ((uint8_t) (0x07 << 4))        ///< TIM5 Output compare 2 mode [2:0] (in _TIM5_CCMR2)
  #define _TIM5_OC2M0             ((uint8_t) (0x01 << 4))        ///< TIM5 Output compare 2 mode [0] (in _TIM5_CCMR2)
  #define _TIM5_OC2M1             ((uint8_t) (0x01 << 5))        ///< TIM5 Output compare 2 mode [1] (in _TIM5_CCMR2)
  #define _TIM5_OC2M2             ((uint8_t) (0x01 << 6))        ///< TIM5 Output compare 2 mode [2] (in _TIM5_CCMR2)
  // reserved [7]

  /*  TIM5 Capture/compare mode register 2 (TIM5_CCMR2). Input mode */
  // _TIM5_CC2S [1:0] defined above
  #define _TIM5_IC2PSC            ((uint8_t) (0x03 << 2))        ///< TIM5 Input capture 2 prescaler [1:0] (in _TIM5_CCMR2)
  #define _TIM5_IC2PSC0           ((uint8_t) (0x01 << 2))        ///< TIM5 Input capture 2 prescaler [0] (in _TIM5_CCMR2)
  #define _TIM5_IC2PSC1           ((uint8_t) (0x01 << 3))        ///< TIM5 Input capture 2 prescaler [1] (in _TIM5_CCMR2)
  #define _TIM5_IC2F              ((uint8_t) (0x0F << 4))        ///< TIM5 Output compare 2 mode [3:0] (in _TIM5_CCMR2)
  #define _TIM5_IC2F0             ((uint8_t) (0x01 << 4))        ///< TIM5 Input capture 2 filter [0] (in _TIM5_CCMR2)
  #define _TIM5_IC2F1             ((uint8_t) (0x01 << 5))        ///< TIM5 Input capture 2 filter [1] (in _TIM5_CCMR2)
  #define _TIM5_IC2F2             ((uint8_t) (0x01 << 6))        ///< TIM5 Input capture 2 filter [2] (in _TIM5_CCMR2)
  #define _TIM5_IC2F3             ((uint8_t) (0x01 << 7))        ///< TIM5 Input capture 2 filter [3] (in _TIM5_CCMR2)

  /*  TIM5 Capture/compare mode register 3 (TIM5_CCMR3). Output mode */
  #define _TIM5_CC3S              ((uint8_t) (0x03 << 0))        ///< TIM5 Compare 3 selection [1:0] (in _TIM5_CCMR3)
  #define _TIM5_CC3S0             ((uint8_t) (0x01 << 0))        ///< TIM5 Compare 3 selection [0] (in _TIM5_CCMR3)
  #define _TIM5_CC3S1             ((uint8_t) (0x01 << 1))        ///< TIM5 Compare 3 selection [1] (in _TIM5_CCMR3)
  // reserved [2]
  #define _TIM5_OC3PE             ((uint8_t) (0x01 << 3))        ///< TIM5 Output compare 3 preload enable [0] (in _TIM5_CCMR3)
  #define _TIM5_OC3M              ((uint8_t) (0x07 << 4))        ///< TIM5 Output compare 3 mode [2:0] (in _TIM5_CCMR3)
  #define _TIM5_OC3M0             ((uint8_t) (0x01 << 4))        ///< TIM5 Output compare 3 mode [0] (in _TIM5_CCMR3)
  #define _TIM5_OC3M1             ((uint8_t) (0x01 << 5))        ///< TIM5 Output compare 3 mode [1] (in _TIM5_CCMR3)
  #define _TIM5_OC3M2             ((uint8_t) (0x01 << 6))        ///< TIM5 Output compare 3 mode [2] (in _TIM5_CCMR3)
  // reserved [7]

  /*  TIM5 Capture/compare mode register 3 (TIM5_CCMR3). Input mode */
  // _TIM5_CC3S [1:0] defined above
  #define _TIM5_IC3PSC            ((uint8_t) (0x03 << 2))        ///< TIM5 Input capture 3 prescaler [1:0] (in _TIM5_CCMR3)
  #define _TIM5_IC3PSC0           ((uint8_t) (0x01 << 2))        ///< TIM5 Input capture 3 prescaler [0] (in _TIM5_CCMR3)
  #define _TIM5_IC3PSC1           ((uint8_t) (0x01 << 3))        ///< TIM5 Input capture 3 prescaler [1] (in _TIM5_CCMR3)
  #define _TIM5_IC3F              ((uint8_t) (0x0F << 4))        ///< TIM5 Output compare 3 mode [3:0] (in _TIM5_CCMR3)
  #define _TIM5_IC3F0             ((uint8_t) (0x01 << 4))        ///< TIM5 Input capture 3 filter [0] (in _TIM5_CCMR3)
  #define _TIM5_IC3F1             ((uint8_t) (0x01 << 5))        ///< TIM5 Input capture 3 filter [1] (in _TIM5_CCMR3)
  #define _TIM5_IC3F2             ((uint8_t) (0x01 << 6))        ///< TIM5 Input capture 3 filter [2] (in _TIM5_CCMR3)
  #define _TIM5_IC3F3             ((uint8_t) (0x01 << 7))        ///< TIM5 Input capture 3 filter [3] (in _TIM5_CCMR3)

  /*  TIM5 Capture/compare enable register 1 (TIM5_CCER1) */
  #define _TIM5_CC1E              ((uint8_t) (0x01 << 0))        ///< TIM5 Capture/compare 1 output enable [0] (in _TIM5_CCER1)
  #define _TIM5_CC1P              ((uint8_t) (0x01 << 1))        ///< TIM5 Capture/compare 1 output polarity [0] (in _TIM5_CCER1)
  // reserved [3:2]
  #define _TIM5_CC2E              ((uint8_t) (0x01 << 4))        ///< TIM5 Capture/compare 2 output enable [0] (in _TIM5_CCER1)
  #define _TIM5_CC2P              ((uint8_t) (0x01 << 5))        ///< TIM5 Capture/compare 2 output polarity [0] (in _TIM5_CCER1)
  // reserved [7:6]

  /*  TIM5 Capture/compare enable register 2 (TIM5_CCER2) */
  #define _TIM5_CC3E              ((uint8_t) (0x01 << 0))        ///< TIM5 Capture/compare 3 output enable [0] (in _TIM5_CCER2)
  #define _TIM5_CC3P              ((uint8_t) (0x01 << 1))        ///< TIM5 Capture/compare 3 output polarity [0] (in _TIM5_CCER2)
  // reserved [7:2]

  /*  TIM5 prescaler (TIM5_PSCR) */
  #define _TIM5_PSC               ((uint8_t) (0x0F << 0))        ///< TIM5 clock prescaler [3:0] (in _TIM5_PSCR)
  #define _TIM5_PSC0              ((uint8_t) (0x01 << 0))        ///< TIM5 clock prescaler [0] (in _TIM5_PSCR)
  #define _TIM5_PSC1              ((uint8_t) (0x01 << 1))        ///< TIM5 clock prescaler [1] (in _TIM5_PSCR)
  #define _TIM5_PSC2              ((uint8_t) (0x01 << 2))        ///< TIM5 clock prescaler [2] (in _TIM5_PSCR)
  #define _TIM5_PSC3              ((uint8_t) (0x01 << 3))        ///< TIM5 clock prescaler [3] (in _TIM5_PSCR)
  // reserved [7:6]

#endif // TIM5_AddressBase



//------------------------
// 8-Bit Timer 6 (TIM6)
//------------------------
#if defined(TIM6_AddressBase)

  /** @brief struct for controlling 8-Bit Timer 6 (TIM6) */
  typedef struct {

    /** @brief TIM6 Control register (TIM6_CR1) */
    struct {
      _BITS   CEN     : 1;    ///< Counter enable
      _BITS   UDIS    : 1;    ///< Update disable
      _BITS   URS     : 1;    ///< Update request source
      _BITS   OPM     : 1;    ///< One-pulse mode
      _BITS   res     : 3;    ///< Reserved
      _BITS   ARPE    : 1;    ///< Auto-reload preload enable
    } CR1;


    /** @brief TIM6 Control register (TIM6_CR2) */
    struct {
      _BITS   res     : 4;    ///< Reserved
      _BITS   MMS     : 3;    ///< Master mode selection
      _BITS   res2    : 1;    ///< Reserved
    } CR2;


    /** @brief Slave mode control register (TIM6_SMCR) */
    struct {
      _BITS   SMS     : 3;    ///< Clock/trigger/slave mode selection
      _BITS   res     : 1;    ///< Reserved
      _BITS   TS      : 3;    ///< Trigger selection
      _BITS   MSM     : 1;    ///< Master/slave mode  #define _TIM5_CR1_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM5 control register 1 reset value

    } SMCR;


    /** @brief TIM6 Interrupt enable (TIM6_IER) */
    struct {
      _BITS   UIE     : 1;    ///< Update interrupt enable
      _BITS   res     : 7;    ///< Reserved
    } IER;


    /** @brief TIM6 Status register (TIM6_SR) */
    struct {
      _BITS   UIF     : 1;    ///< Update interrupt flag
      _BITS   res     : 7;    ///< Reserved
    } SR;


    /** @brief TIM6 Event Generation (TIM6_EGR) */
    struct {
      _BITS   UG      : 1;    ///< Update generation
      _BITS   res     : 7;    ///< Reserved
    } EGR;


    /** @brief TIM6 8-bit counter register (TIM6_CNTR) */
    struct {
      _BITS   CNT     : 8;  ///< 8-bit counter
    } CNTR;

    /** @brief TIM6 clock prescaler (TIM6_PSCR) */
    struct {
      _BITS   PSC     : 3;    ///< clock prescaler
      _BITS   res     : 5;    ///< Reserved
    } PSCR;


    /** @brief TIM6 8-bit auto-reload register (TIM6_ARR) */
    struct {
      _BITS   ARR     : 8;    ///< auto-reload value
    } ARR;

  } TIM6_t;

  /* Pointer to TIM6 registers */
  #define _TIM6      _SFR(TIM6_t,      TIM6_AddressBase)         ///< TIM6 struct/bit access
  #define _TIM6_CR   _SFR(uint8_t,     TIM6_AddressBase+0x00)    ///< TIM6 control register
  #define _TIM6_IER  _SFR(uint8_t,     TIM6_AddressBase+0x01)    ///< TIM6 interrupt enable register
  #define _TIM6_SR   _SFR(uint8_t,     TIM6_AddressBase+0x02)    ///< TIM6 status register
  #define _TIM6_EGR  _SFR(uint8_t,     TIM6_AddressBase+0x03)    ///< TIM6 event generation register
  #define _TIM6_CNTR _SFR(uint8_t,     TIM6_AddressBase+0x04)    ///< TIM6 counter register
  #define _TIM6_PSCR _SFR(uint8_t,     TIM6_AddressBase+0x05)    ///< TIM6 clock prescaler register
  #define _TIM6_ARR  _SFR(uint8_t,     TIM6_AddressBase+0x06)    ///< TIM6 auto-reload register

  /* TIM6 Module Reset Values */
  #define _TIM6_CR_RESET_VALUE         ((uint8_t) 0x00)          ///< TIM6 control register reset value
  #define _TIM6_IER_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM6 interrupt enable register reset value
  #define _TIM6_SR_RESET_VALUE         ((uint8_t) 0x00)          ///< TIM6 status register reset value
  #define _TIM6_EGR_RESET_VALUE        ((uint8_t) 0x00)          ///< TIM6 event generation register reset value
  #define _TIM6_CNTR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM6 counter register reset value
  #define _TIM6_PSCR_RESET_VALUE       ((uint8_t) 0x00)          ///< TIM6 clock prescaler register reset value
  #define _TIM6_ARR_RESET_VALUE        ((uint8_t) 0xFF)          ///< TIM6 auto-reload register reset value

  /* TIM6 Control register (TIM6_CR1) */
  #define _TIM6_CEN               ((uint8_t) (0x01 << 0))        ///< TIM6 Counter enable [0] (in _TIM6_CR1)
  #define _TIM6_UDIS              ((uint8_t) (0x01 << 1))        ///< TIM6 Update disable [0] (in _TIM6_CR1)
  #define _TIM6_URS               ((uint8_t) (0x01 << 2))        ///< TIM6 Update request source [0] (in _TIM6_CR1)
  #define _TIM6_OPM               ((uint8_t) (0x01 << 3))        ///< TIM6 One-pulse mode [0] (in _TIM6_CR1)
  // reserved [6:4]
  #define _TIM6_ARPE              ((uint8_t) (0x01 << 7))        ///< TIM6 Auto-reload preload enable [0] (in _TIM6_CR1)

  /* TIM6 Control register (TIM6_CR2) */
  // reserved [3:0]
  #define _TIM6_MMS               ((uint8_t) (0x07 << 4))        ///< TIM6 Master mode selection [2:0] (in _TIM6_CR2)
  #define _TIM6_MMS0              ((uint8_t) (0x01 << 4))        ///< TIM6 Master mode selection [0] (in _TIM6_CR2)
  #define _TIM6_MMS1              ((uint8_t) (0x01 << 5))        ///< TIM6 Master mode selection [1] (in _TIM6_CR2)
  #define _TIM6_MMS2              ((uint8_t) (0x01 << 6))        ///< TIM6 Master mode selection [2] (in _TIM6_CR2)
  // reserved [7]

  /* Slave mode control register (TIM6_SMCR) */
  #define _TIM6_SMS               ((uint8_t) (0x07 << 0))        ///< TIM6 Clock/trigger/slave mode selection [2:0] (in _TIM6_SMCR)
  #define _TIM6_SMS0              ((uint8_t) (0x01 << 0))        ///< TIM6 Clock/trigger/slave mode selection [0] (in _TIM6_SMCR)
  #define _TIM6_SMS1              ((uint8_t) (0x01 << 1))        ///< TIM6 Clock/trigger/slave mode selection [1] (in _TIM6_SMCR)
  #define _TIM6_SMS2              ((uint8_t) (0x01 << 2))        ///< TIM6 Clock/trigger/slave mode selection [2] (in _TIM6_SMCR)
  // reserved [3]
  #define _TIM6_TS                ((uint8_t) (0x07 << 4))        ///< TIM6 Trigger selection [2:0] (in _TIM6_SMCR)
  #define _TIM6_TS0               ((uint8_t) (0x01 << 4))        ///< TIM6 Trigger selection [0] (in _TIM6_SMCR)
  #define _TIM6_TS1               ((uint8_t) (0x01 << 5))        ///< TIM6 Trigger selection [1] (in _TIM6_SMCR)
  #define _TIM6_TS2               ((uint8_t) (0x01 << 6))        ///< TIM6 Trigger selection [2] (in _TIM6_SMCR)
  // reserved [7]

  /*  TIM6 Interrupt enable (TIM6_IER) */
  #define _TIM6_UIE               ((uint8_t) (0x01 << 0))        ///< TIM6 Update interrupt enable [0] (in _TIM6_IER)
  // reserved [7:1]

  /*  TIM6 Status register (TIM6_SR) */
  #define _TIM6_UIF               ((uint8_t) (0x01 << 0))        ///< TIM6 Update interrupt flag [0] (in _TIM6_SR)
  // reserved [7:1]

  /*  TIM6 Event generation register (TIM6_EGR) */
  #define _TIM6_UG                ((uint8_t) (0x01 << 0))        ///< TIM6 Update generation [0] (in _TIM6_EGR)
  // reserved [7:1]

  /* TIM6 Prescaler register (TIM6_PSCR) */
  #define _TIM6_PSC               ((uint8_t) (0x07 << 0))        ///< TIM6 clock prescaler [2:0] (in _TIM6_PSCR)
  #define _TIM6_PSC0              ((uint8_t) (0x01 << 0))        ///< TIM6 clock prescaler [0] (in _TIM6_PSCR)
  #define _TIM6_PSC1              ((uint8_t) (0x01 << 1))        ///< TIM6 clock prescaler [1] (in _TIM6_PSCR)
  #define _TIM6_PSC2              ((uint8_t) (0x01 << 2))        ///< TIM6 clock prescaler [2] (in _TIM6_PSCR)
  // reserved [7:3]

#endif // TIM6_AddressBase



//------------------------
// Analog Digital Converter 1 (ADC1)
//------------------------
#if defined(ADC1_AddressBase)

  /** @brief struct containing Analog Digital Converter 1 (ADC1) */
  typedef struct {

    /** @brief ADC1 10-bit Data Buffer Register 0 (ADC1_DB0RH) */
    struct {
      _BITS   DATA     : 2;   ///< Data buffer 0 value [9:8]
      _BITS   res      : 6;   ///< reserved
    } DB0RH;

    /** @brief ADC1 10-bit Data Buffer Register 0 (ADC1_DB0RL) */
    struct {
      _BITS   DATA     : 8;   ///< Data buffer 0 value (low)
    } DB0RL;


    /** @brief ADC1 10-bit Data Buffer Register 1 (ADC1_DB1RH) */
    struct {
      _BITS   DATA     : 2;   ///< Data buffer 1 value [9:8]
      _BITS   res      : 6;   ///< reserved
    } DB1RH;


    /** @brief ADC1 10-bit Data Buffer Register 1 (ADC1_DB1RL) */
    struct {
      _BITS   DATA     : 8;   ///< Data buffer 1 value (low)
    } DB1RL;


    /** @brief ADC1 10-bit Data Buffer Register 2 (ADC1_DB2RH) */
    struct {
      _BITS   DATA     : 2;   ///< Data buffer 2 value [9:8]
      _BITS   res      : 6;   ///< reserved
    } DB2RH;


    /** @brief ADC1 10-bit Data Buffer Register 2 (ADC1_DB2RL) */
    struct {
      _BITS   DATA     : 8;   ///< Data buffer 2 value (low)
    } DB2RL;


    /** @brief ADC1 10-bit Data Buffer Register 3 (ADC1_DB3RH) */
    struct {
      _BITS   DATA     : 2;   ///< Data buffer 3 value [9:8]
      _BITS   res      : 6;   ///< reserved
    } DB3RH;


    /** @brief ADC1 10-bit Data Buffer Register 3 (ADC1_DB3RL) */
    struct {
      _BITS   DATA     : 8;   ///< Data buffer 3 value (low)
    } DB3RL;


    /** @brief ADC1 10-bit Data Buffer Register 4 (ADC1_DB4RH) */
    struct {
      _BITS   DATA     : 2;   ///< Data buffer 4 value [9:8]
      _BITS   res      : 6;   ///< reserved
    } DB4RH;


    /** @brief ADC1 10-bit Data Buffer Register 4 (ADC1_DB4RL) */
    struct {
      _BITS   DATA     : 8;   ///< Data buffer 4 value (low)
    } DB4RL;


    /** @brief ADC1 10-bit Data Buffer Register 5 (ADC1_DB5RH) */
    struct {
      _BITS   DATA     : 2;   ///< Data buffer 5 value [9:8]
      _BITS   res      : 6;   ///< reserved
    } DB5RH;


    /** @brief ADC1 10-bit Data Buffer Register 5 (ADC1_DB5RL) */
    struct {
      _BITS   DATA     : 8;   ///< Data buffer 5 value (low)
    } DB5RL;


    /** @brief ADC1 10-bit Data Buffer Register 6 (ADC1_DB6RH) */
    struct {
      _BITS   DATA     : 2;   ///< Data buffer 6 value [9:8]
      _BITS   res      : 6;   ///< reserved
    } DB6RH;


    /** @brief ADC1 10-bit Data Buffer Register 6 (ADC1_DB6RL) */
    struct {
      _BITS   DATA     : 8;   ///< Data buffer 6 value (low)
    } DB6RL;


    /** @brief ADC1 10-bit Data Buffer Register 7 (ADC1_DB7RH) */
    struct {
      _BITS   DATA     : 2;   ///< Data buffer 7 value [9:8]
      _BITS   res      : 6;   ///< reserved
    } DB7RH;


    /** @brief ADC1 10-bit Data Buffer Register 7 (ADC1_DB7RL) */
    struct {
      _BITS   DATA     : 8;   ///< Data buffer 7 value (low)
    } DB7RL;


    /** @brief ADC1 10-bit Data Buffer Register 8 (ADC1_DB8RH) */
    struct {
      _BITS   DATA     : 2;   ///< Data buffer 8 value [9:8]
      _BITS   res      : 6;   ///< reserved
    } DB8RH;


    /** @brief ADC1 10-bit Data Buffer Register 8 (ADC1_DB8RL) */
    struct {
      _BITS   DATA     : 8;   ///< Data buffer 8 value (low)
    } DB8RL;


    /** @brief ADC1 10-bit Data Buffer Register 9 (ADC1_DB9RH) */
    struct {
      _BITS   DATA     : 2;   ///< Data buffer 9 value [9:8]
      _BITS   res      : 6;   ///< reserved
    } DB9RH;


    /** @brief ADC1 10-bit Data Buffer Register 9 (ADC1_DB8RL) */
    struct {
      _BITS   DATA     : 8;   ///< Data buffer 9 value (low)
    } DB9RL;


    /** @brief Reserved register (12B) */
    uint8_t   res[12];


    /** @brief ADC1 control/status register (ADC1_CSR) */
    struct {
      _BITS   CH        : 4;        ///< Channel selection bits
      _BITS   AWDIE        : 1;        ///< Analog watchdog interrupt enable
      _BITS   EOCIE        : 1;        ///< Interrupt enable for EOC
      _BITS   AWD        : 1;        ///< Analog Watchdog flag
      _BITS   EOC        : 1;        ///< End of conversion
    } CSR;


    /** @brief ADC1 Configuration Register 1 (ADC1_CR1) */
    struct {
      _BITS   ADON        : 1;        ///< A/D Converter on/off
      _BITS   CONT        : 1;        ///< Continuous conversion
      _BITS   res        : 2;        ///< Reserved, always read as 0
      _BITS   SPSEL        : 3;        ///< Clock prescaler selection
      _BITS   res2        : 1;        ///< Reserved, always read as 0
    } CR1;


    /** @brief ADC1 Configuration Register 2 (ADC1_CR2) */
    struct {
      _BITS   res        : 1;        ///< Reserved, must be kept cleared
      _BITS   SCAN        : 1;        ///< Scan mode enable
      _BITS   res2        : 1;        ///< Reserved, must be kept cleared
      _BITS   ALIGN        : 1;        ///< Data alignment
      _BITS   EXTSEL    : 2;        ///< External event selection
      _BITS   EXTTRIG    : 1;        ///< External trigger enable
      _BITS   res3        : 1;        ///< Reserved, must be kept cleared
    } CR2;


    /** @brief ADC1 Configuration Register 3 (ADC1_CR3) */
    struct {
      _BITS   res        : 6;        ///< Reserved, must be kept cleared
      _BITS   OVR        : 1;        ///< Overrun flag
      _BITS   DBUF        : 1;        ///< Data buffer enable
    } CR3;


    /** @brief ADC1 (unbuffered) 10-bit measurement result (ADC1_DRH) */
    struct {
      _BITS   DATA     : 2;   ///< Data value [9:8]
      _BITS   res      : 6;   ///< reserved
    } DRH;


    /** @brief ADC1 (unbuffered) 10-bit measurement result (ADC1_DRL) */
    struct {
      _BITS   DATA     : 8;   ///< Data value [7:0]
    } DRL;


    /** @brief ADC1 Schmitt trigger disable register (ADC1_TDRH) */
    struct {
      _BITS   TDH      : 8;   ///< Schmitt trigger disable [15:8]
    } TDRH;


    /** @brief ADC1 Schmitt trigger disable register (ADC1_TDRL) */
    struct {
      _BITS   TDL      : 8;   ///< Schmitt trigger disable [7:0]
    } TDRL;


    /** @brief ADC1 watchdog high threshold register (ADC1_HTRH) */
    struct {
      _BITS   HT       : 8;   ///< watchdog high threshold [9:2]
    } HTRH;


    /** @brief ADC1 watchdog high threshold register (ADC1_HTRL) */
    struct {
      _BITS   HT        : 2;  ///< watchdog high threshold [1:0]
      _BITS   res         : 6;  ///< Reserved

    } HTRL;


    /** @brief ADC1 watchdog low threshold register (ADC1_LTRH) */
    struct {
      _BITS   LT        : 8;  ///< watchdog low threshold [9:2]
    } LTRH;


    /** @brief ADC1 watchdog low threshold register (ADC1_LTRL) */
    struct {
      _BITS   LT        : 2;  ///< watchdog low threshold [1:0]
      _BITS   res         : 6;  ///< Reserved
    } LTRL;


    /** @brief ADC1 watchdog status register (ADC1_AWSRH) */
    struct {
      _BITS   AWS       : 2;  ///< watchdog status register [9:8]
      _BITS   res         : 6;  ///< Reserved
    } AWSRH;


    /** @brief ADC1 watchdog status register (ADC1_AWSRL) */
    struct {
      _BITS   AWS       : 8;  ///< watchdog status register [7:0]
    } AWSRL;


    /** @brief ADC1 watchdog control register (ADC1_AWCRH) */
    struct {
      _BITS   AWEN      : 2;  ///< watchdog control register [9:8]
      _BITS   res         : 6;  ///< Reserved
    } AWCRH;


    /** @brief ADC1 watchdog control register (ADC1_AWCRL) */
    struct {
      _BITS   AWEN      : 8;  ///< watchdog control register [7:0]
    } AWCRL;

  } ADC1_t;

  /* Pointer to ADC1 registers */
  #define _ADC1         _SFR(ADC1_t,   ADC1_AddressBase)         ///< ADC1 struct/bit access
  #define _ADC1_DB0RH   _SFR(uint8_t,  ADC1_AddressBase+0x00)    ///< ADC1 10-bit Data Buffer Register 0
  #define _ADC1_DB0RL   _SFR(uint8_t,  ADC1_AddressBase+0x01)    ///< ADC1 10-bit Data Buffer Register 0
  #define _ADC1_DB1RH   _SFR(uint8_t,  ADC1_AddressBase+0x02)    ///< ADC1 10-bit Data Buffer Register 1
  #define _ADC1_DB1RL   _SFR(uint8_t,  ADC1_AddressBase+0x03)    ///< ADC1 10-bit Data Buffer Register 1
  #define _ADC1_DB2RH   _SFR(uint8_t,  ADC1_AddressBase+0x04)    ///< ADC1 10-bit Data Buffer Register 2
  #define _ADC1_DB2RL   _SFR(uint8_t,  ADC1_AddressBase+0x05)    ///< ADC1 10-bit Data Buffer Register 2
  #define _ADC1_DB3RH   _SFR(uint8_t,  ADC1_AddressBase+0x06)    ///< ADC1 10-bit Data Buffer Register 3
  #define _ADC1_DB3RL   _SFR(uint8_t,  ADC1_AddressBase+0x07)    ///< ADC1 10-bit Data Buffer Register 3
  #define _ADC1_DB4RH   _SFR(uint8_t,  ADC1_AddressBase+0x08)    ///< ADC1 10-bit Data Buffer Register 4
  #define _ADC1_DB4RL   _SFR(uint8_t,  ADC1_AddressBase+0x09)    ///< ADC1 10-bit Data Buffer Register 4
  #define _ADC1_DB5RH   _SFR(uint8_t,  ADC1_AddressBase+0x0A)    ///< ADC1 10-bit Data Buffer Register 5
  #define _ADC1_DB5RL   _SFR(uint8_t,  ADC1_AddressBase+0x0B)    ///< ADC1 10-bit Data Buffer Register 5
  #define _ADC1_DB6RH   _SFR(uint8_t,  ADC1_AddressBase+0x0C)    ///< ADC1 10-bit Data Buffer Register 6
  #define _ADC1_DB6RL   _SFR(uint8_t,  ADC1_AddressBase+0x0D)    ///< ADC1 10-bit Data Buffer Register 6
  #define _ADC1_DB7RH   _SFR(uint8_t,  ADC1_AddressBase+0x0E)    ///< ADC1 10-bit Data Buffer Register 7
  #define _ADC1_DB7RL   _SFR(uint8_t,  ADC1_AddressBase+0x0F)    ///< ADC1 10-bit Data Buffer Register 7
  #define _ADC1_DB8RH   _SFR(uint8_t,  ADC1_AddressBase+0x10)    ///< ADC1 10-bit Data Buffer Register 8
  #define _ADC1_DB8RL   _SFR(uint8_t,  ADC1_AddressBase+0x11)    ///< ADC1 10-bit Data Buffer Register 8
  #define _ADC1_DB9RH   _SFR(uint8_t,  ADC1_AddressBase+0x12)    ///< ADC1 10-bit Data Buffer Register 9
  #define _ADC1_DB9RL   _SFR(uint8_t,  ADC1_AddressBase+0x13)    ///< ADC1 10-bit Data Buffer Register 9
  // reserved (12B)
  #define _ADC1_CSR     _SFR(uint8_t,  ADC1_AddressBase+0x20)    ///< ADC1 control/status register
  #define _ADC1_CR1     _SFR(uint8_t,  ADC1_AddressBase+0x21)    ///< ADC1 Configuration Register 1
  #define _ADC1_CR2     _SFR(uint8_t,  ADC1_AddressBase+0x22)    ///< ADC1 Configuration Register 2
  #define _ADC1_CR3     _SFR(uint8_t,  ADC1_AddressBase+0x23)    ///< ADC1 Configuration Register 3
  #define _ADC1_DRH     _SFR(uint8_t,  ADC1_AddressBase+0x24)    ///< ADC1 (unbuffered) 10-bit measurement result
  #define _ADC1_DRL     _SFR(uint8_t,  ADC1_AddressBase+0x25)    ///< ADC1 (unbuffered) 10-bit measurement result
  #define _ADC1_TDRH    _SFR(uint8_t,  ADC1_AddressBase+0x26)    ///< ADC1 Schmitt trigger disable register
  #define _ADC1_TDRL    _SFR(uint8_t,  ADC1_AddressBase+0x27)    ///< ADC1 Schmitt trigger disable register
  #define _ADC1_HTRH    _SFR(uint8_t,  ADC1_AddressBase+0x28)    ///< ADC1 watchdog high threshold register
  #define _ADC1_HTRL    _SFR(uint8_t,  ADC1_AddressBase+0x29)    ///< ADC1 watchdog high threshold register
  #define _ADC1_LTRH    _SFR(uint8_t,  ADC1_AddressBase+0x2A)    ///< ADC1 watchdog low threshold register
  #define _ADC1_LTRL    _SFR(uint8_t,  ADC1_AddressBase+0x2B)    ///< ADC1 watchdog low threshold register
  #define _ADC1_AWSRH   _SFR(uint8_t,  ADC1_AddressBase+0x2C)    ///< ADC1 watchdog status register
  #define _ADC1_AWSRL   _SFR(uint8_t,  ADC1_AddressBase+0x2D)    ///< ADC1 watchdog status register
  #define _ADC1_AWCRH   _SFR(uint8_t,  ADC1_AddressBase+0x2E)    ///< ADC1 watchdog control register
  #define _ADC1_AWCRL   _SFR(uint8_t,  ADC1_AddressBase+0x2F)    ///< ADC1 watchdog control register

  /* ADC1 Module Reset Values */
  #define  _ADC1_CSR_RESET_VALUE       ((uint8_t) 0x00)          ///< ADC1 control/status register reset value
  #define  _ADC1_CR1_RESET_VALUE       ((uint8_t) 0x00)          ///< ADC1 Configuration Register 1  reset value
  #define  _ADC1_CR2_RESET_VALUE       ((uint8_t) 0x00)          ///< ADC1 Configuration Register 2  reset value
  #define  _ADC1_CR3_RESET_VALUE       ((uint8_t) 0x00)          ///< ADC1 Configuration Register 3  reset value
  #define  _ADC1_TDRH_RESET_VALUE      ((uint8_t) 0x00)          ///< ADC1 Schmitt trigger disable register reset value
  #define  _ADC1_TDRL_RESET_VALUE      ((uint8_t) 0x00)          ///< ADC1 Schmitt trigger disable register reset value
  #define  _ADC1_HTRH_RESET_VALUE      ((uint8_t) 0xFF)          ///< ADC1 watchdog high threshold register reset value
  #define  _ADC1_HTRL_RESET_VALUE      ((uint8_t) 0x03)          ///< ADC1 watchdog high threshold register reset value
  #define  _ADC1_LTRH_RESET_VALUE      ((uint8_t) 0x00)          ///< ADC1 watchdog low threshold register reset value
  #define  _ADC1_LTRL_RESET_VALUE      ((uint8_t) 0x00)          ///< ADC1 watchdog low threshold register reset value
  #define  _ADC1_AWCRH_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC1 watchdog control register reset value
  #define  _ADC1_AWCRL_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC1 watchdog control register reset value

  /* ADC1 control/status register (ADC1_CSR) */
  #define _ADC1_CH                ((uint8_t) (0x0F << 0))        ///< ADC1 Channel selection bits [3:0] (in _ADC1_CSR)
  #define _ADC1_CH0               ((uint8_t) (0x01 << 0))        ///< ADC1 Channel selection bits [0] (in _ADC1_CSR)
  #define _ADC1_CH1               ((uint8_t) (0x01 << 1))        ///< ADC1 Channel selection bits [1] (in _ADC1_CSR)
  #define _ADC1_CH2               ((uint8_t) (0x01 << 2))        ///< ADC1 Channel selection bits [2] (in _ADC1_CSR)
  #define _ADC1_CH3               ((uint8_t) (0x01 << 3))        ///< ADC1 Channel selection bits [3] (in _ADC1_CSR)
  #define _ADC1_AWDIE             ((uint8_t) (0x01 << 4))        ///< ADC1 Analog watchdog interrupt enable [0] (in _ADC1_CSR)
  #define _ADC1_EOCIE             ((uint8_t) (0x01 << 5))        ///< ADC1 Interrupt enable for EOC [0] (in _ADC1_CSR)
  #define _ADC1_AWD               ((uint8_t) (0x01 << 6))        ///< ADC1 Analog Watchdog flag [0] (in _ADC1_CSR)
  #define _ADC1_EOC               ((uint8_t) (0x01 << 7))        ///< ADC1 End of conversion [0] (in _ADC1_CSR)

  /* ADC1 Configuration Register 1 (ADC1_CR1) */
  #define _ADC1_ADON              ((uint8_t) (0x01 << 0))        ///< ADC1 Conversion on/off [0] (in _ADC1_CR1)
  #define _ADC1_CONT              ((uint8_t) (0x01 << 1))        ///< ADC1 Continuous conversion [0] (in _ADC1_CR1)
  // reserved [3:2]
  #define _ADC1_SPSEL             ((uint8_t) (0x07 << 4)         ///< ADC1 clock prescaler selection [2:0] (in _ADC1_CR1)
  #define _ADC1_SPSEL0            ((uint8_t) (0x01 << 4)         ///< ADC1 clock prescaler selection [0] (in _ADC1_CR1)
  #define _ADC1_SPSEL1            ((uint8_t) (0x01 << 5)         ///< ADC1 clock prescaler selection [1] (in _ADC1_CR1)
  #define _ADC1_SPSEL2            ((uint8_t) (0x01 << 6)         ///< ADC1 clock prescaler selection [2] (in _ADC1_CR1)
  // reserved [7]

  /* ADC1 Configuration Register 2 (ADC1_CR2) */
  // reserved [0]
  #define _ADC1_SCAN              ((uint8_t) (0x01 << 1))        ///< ADC1 Scan mode enable [0] (in _ADC1_CR2)
  // reserved [2]
  #define _ADC1_ALIGN             ((uint8_t) (0x01 << 3))        ///< ADC1 Data alignment [0] (in _ADC1_CR2)
  #define _ADC1_EXTSEL            ((uint8_t) (0x03 << 4)         ///< ADC1 External event selection [1:0] (in _ADC1_CR2)
  #define _ADC1_EXTSEL0           ((uint8_t) (0x01 << 4)         ///< ADC1 External event selection [0] (in _ADC1_CR2)
  #define _ADC1_EXTSEL1           ((uint8_t) (0x01 << 5)         ///< ADC1 External event selection [1] (in _ADC1_CR2)
  #define _ADC1_EXTTRIG           ((uint8_t) (0x01 << 6))        ///< ADC1 External trigger enable [0] (in _ADC1_CR2)
  // reserved [7]

  /* ADC1 Configuration Register 3 (ADC1_CR3) */
  // reserved [5:0]
  #define _ADC1_OVR               ((uint8_t) (0x01 << 6))        ///< ADC1 Overrun flag [0] (in _ADC1_CR3)
  #define _ADC1_DBUF              ((uint8_t) (0x01 << 7))        ///< ADC1 Data buffer enable [0] (in _ADC1_CR3)

#endif // ADC1_AddressBase



//------------------------
// Analog Digital Converter 2 (ADC2)
//------------------------
#if defined(ADC2_AddressBase)

  /** @brief struct containing Analog Digital Converter 2 (ADC2) */
  typedef struct {

    /** @brief ADC2 control/status register (ADC2_CSR) */
    struct {
      _BITS   CH        : 4;        ///< Channel selection bits
      _BITS   res       : 1;        ///< Reserved
      _BITS   EOCIE     : 1;        ///< Interrupt enable for EOC
      _BITS   res2      : 1;        ///< Reserved
      _BITS   EOC       : 1;        ///< End of conversion
    } CSR;


    /** @brief ADC2 Configuration Register 1 (ADC2_CR1) */
    struct {
      _BITS   ADON      : 1;        ///< A/D Converter on/off
      _BITS   CONT      : 1;        ///< Continuous conversion
      _BITS   res       : 2;        ///< Reserved, always read as 0
      _BITS   SPSEL     : 3;        ///< Clock prescaler selection
      _BITS   res2      : 1;        ///< Reserved, always read as 0
    } CR1;


    /** @brief ADC2 Configuration Register 2 (ADC2_CR2) */
    struct {
      _BITS   res       : 3;        ///< Reserved, must be kept cleared
      _BITS   ALIGN     : 1;        ///< Data alignment
      _BITS   EXTSEL    : 2;        ///< External event selection
      _BITS   EXTTRIG   : 1;        ///< External trigger enable
      _BITS   res3      : 1;        ///< Reserved, must be kept cleared
    } CR2;


    /** @brief Reserved register (1B) */
    uint8_t       res[1];


    /** @brief ADC2 (unbuffered) 10-bit measurement result (ADC2_DRH) */
    struct {
      _BITS DATA      : 2;  ///< Data value [9:8]
      _BITS res       : 6;  ///< Reserved
    } DRH;


    /** @brief ADC2 (unbuffered) 10-bit measurement result (ADC2_DRL) */
    struct {
      _BITS DATA      : 8;  ///< Data value [7:0]
    } DRL;


    /** @brief ADC2 Schmitt trigger disable register (ADC2_TDRH) */
    struct {
      _BITS TDH       : 8;  ///< Schmitt trigger disable [9:8]
    } TDRH;


    /** @brief ADC2 Schmitt trigger disable register (ADC2_TDRL) */
    struct {
      _BITS TDL       : 8;  ///< Schmitt trigger disable [7:0]
    } TDRL;

  } ADC2_t;

  /* Pointer to ADC2 registers */
  #define _ADC2         _SFR(ADC2_t,   ADC2_AddressBase)         ///< ADC2 struct/bit access
  #define _ADC2_CSR     _SFR(uint8_t,  ADC2_AddressBase+0x00)    ///< ADC2 control/status register
  #define _ADC2_CR1     _SFR(uint8_t,  ADC2_AddressBase+0x01)    ///< ADC2 Configuration Register 1
  #define _ADC2_CR2     _SFR(uint8_t,  ADC2_AddressBase+0x02)    ///< ADC2 Configuration Register 2
  // reserved (1B)
  #define _ADC2_DRH     _SFR(uint8_t,  ADC2_AddressBase+0x04)    ///< ADC2 (unbuffered) 10-bit measurement result
  #define _ADC2_DRL     _SFR(uint8_t,  ADC2_AddressBase+0x05)    ///< ADC2 (unbuffered) 10-bit measurement result
  #define _ADC2_TDRH    _SFR(uint8_t,  ADC2_AddressBase+0x06)    ///< ADC2 Schmitt trigger disable register
  #define _ADC2_TDRL    _SFR(uint8_t,  ADC2_AddressBase+0x07)    ///< ADC2 Schmitt trigger disable register

  /* ADC2 Module Reset Values */
  #define  _ADC2_CSR_RESET_VALUE       ((uint8_t) 0x00)          ///< ADC2 control/status register reset value
  #define  _ADC2_CR1_RESET_VALUE       ((uint8_t) 0x00)          ///< ADC2 Configuration Register 1 reset value
  #define  _ADC2_CR2_RESET_VALUE       ((uint8_t) 0x00)          ///< ADC2 Configuration Register 2 reset value
  #define  _ADC2_TDRL_RESET_VALUE      ((uint8_t) 0x00)          ///< ADC2 Schmitt trigger disable register reset value
  #define  _ADC2_TDRH_RESET_VALUE      ((uint8_t) 0x00)          ///< ADC2 Schmitt trigger disable register reset value

  /* ADC2 control/status register (ADC2_CSR) */
  #define _ADC2_CH                ((uint8_t) (0x0F << 0))        ///< ADC2 Channel selection bits [3:0] (in _ADC2_CSR)
  #define _ADC2_CH0               ((uint8_t) (0x01 << 0))        ///< ADC2 Channel selection bits [0] (in _ADC2_CSR)
  #define _ADC2_CH1               ((uint8_t) (0x01 << 1))        ///< ADC2 Channel selection bits [1] (in _ADC2_CSR)
  #define _ADC2_CH2               ((uint8_t) (0x01 << 2))        ///< ADC2 Channel selection bits [2] (in _ADC2_CSR)
  #define _ADC2_CH3               ((uint8_t) (0x01 << 3))        ///< ADC2 Channel selection bits [3] (in _ADC2_CSR)
  // reserved [4]
  #define _ADC2_EOCIE             ((uint8_t) (0x01 << 5))        ///< ADC2 Interrupt enable for EOC [0] (in _ADC2_CSR)
  // reserved [6]
  #define _ADC2_EOC               ((uint8_t) (0x01 << 7))        ///< ADC2 End of conversion [0] (in _ADC2_CSR)

  /* ADC2 Configuration Register 1 (ADC2_CR1) */
  #define _ADC2_ADON              ((uint8_t) (0x01 << 0))        ///< ADC2 Conversion on/off [0] (in _ADC2_CR1)
  #define _ADC2_CONT              ((uint8_t) (0x01 << 1))        ///< ADC2 Continuous conversion [0] (in _ADC2_CR1)
  // reserved [3:2]
  #define _ADC2_SPSEL             ((uint8_t) (0x07 << 4)         ///< ADC2 clock prescaler selection [2:0] (in _ADC2_CR1)
  #define _ADC2_SPSEL0            ((uint8_t) (0x01 << 4)         ///< ADC2 clock prescaler selection [0] (in _ADC2_CR1)
  #define _ADC2_SPSEL1            ((uint8_t) (0x01 << 5)         ///< ADC2 clock prescaler selection [1] (in _ADC2_CR1)
  #define _ADC2_SPSEL2            ((uint8_t) (0x01 << 6)         ///< ADC2 clock prescaler selection [2] (in _ADC2_CR1)
  // reserved [7]

  /* ADC2 Configuration Register 2 (ADC2_CR2) */
  // reserved [2:0]
  #define _ADC2_ALIGN             ((uint8_t) (0x01 << 3))        ///< ADC2 Data alignment [0] (in _ADC2_CR2)
  #define _ADC2_EXTSEL            ((uint8_t) (0x03 << 4)         ///< ADC2 External event selection [1:0] (in _ADC2_CR2)
  #define _ADC2_EXTSEL0           ((uint8_t) (0x01 << 4)         ///< ADC2 External event selection [0] (in _ADC2_CR2)
  #define _ADC2_EXTSEL1           ((uint8_t) (0x01 << 5)         ///< ADC2 External event selection [1] (in _ADC2_CR2)
  #define _ADC2_EXTTRIG           ((uint8_t) (0x01 << 6))        ///< ADC2 External trigger enable [0] (in _ADC2_CR2)
  // reserved [7]

#endif // ADC2_AddressBase


//------------------------
// Controller Area Network Module (CAN)
//------------------------
#if defined(CAN_AddressBase)

  /** @brief struct for controlling Controller Area Network Module (CAN) */
  typedef struct {

    /** @brief CAN master control register (CAN_MCR) */
    struct {
      _BITS   INRQ      : 1;    ///< Initialization Request
      _BITS   SLEEP     : 1;    ///< Sleep Mode Request
      _BITS   TXFP      : 1;    ///< Transmit FIFO Priority
      _BITS   RFLM      : 1;    ///< Receive FIFO Locked Mode
      _BITS   NART      : 1;    ///< No Automatic Retransmission
      _BITS   AWUM      : 1;    ///< Automatic Wakeup Mode
      _BITS   ABOM      : 1;    ///< Automatic Bus-Off Management
      _BITS   TTCM      : 1;    ///< Time Triggered Communication Mode
    } MCR;


    /** @brief CAN master status register (CAN_MSR) */
    struct {
      _BITS   INAK      : 1;    ///< Initialization Acknowledge
      _BITS   SLAK      : 1;    ///< Sleep Acknowledge
      _BITS   ERRI      : 1;    ///< Error Interrupt
      _BITS   WKUI      : 1;    ///< Wakeup Interrupt
      _BITS   TX        : 1;    ///< Transmit
      _BITS   RX        : 1;    ///< Receive
      _BITS   res       : 2;    ///< Reserved
    } MSR;


    /** @brief CAN transmit status register (CAN_TSR) */
    struct {
      _BITS   RQCP0     : 1;    ///< Request Completed for Mailbox 0
      _BITS   RQCP1     : 1;    ///< Request Completed for Mailbox 1
      _BITS   RQCP2     : 1;    ///< Request Completed for Mailbox 2
      _BITS   res       : 1;    ///< Reserved
      _BITS   TXOK0     : 1;    ///< Transmission ok for Mailbox 0
      _BITS   TXOK1     : 1;    ///< Transmission ok for Mailbox 1
      _BITS   TXOK2     : 1;    ///< Transmission ok for Mailbox 2
      _BITS   res2      : 1;    ///< Reserved
    } TSR;


    /** @brief CAN transmit priority register (CAN_TPR) */
    struct {
      _BITS   CODE      : 2;    ///< Mailbox Code
      _BITS   TME0      : 1;    ///< Transmit Mailbox 0 Empty
      _BITS   TME1      : 1;    ///< Transmit Mailbox 1 Empty
      _BITS   TME2      : 1;    ///< Transmit Mailbox 2 Empty
      _BITS   LOW0      : 1;    ///< Lowest Priority Flag for Mailbox 0
      _BITS   LOW1      : 1;    ///< Lowest Priority Flag for Mailbox 1
      _BITS   LOW2      : 1;    ///< Lowest Priority Flag for Mailbox 2
    } TPR;


    /** @brief CAN receive FIFO register (CAN_RFR) */
    struct {
      _BITS   FMP       : 2;    ///< FIFO Message Pending
      _BITS   res       : 1;    ///< Reserved
      _BITS   FULL      : 1;    ///< FIFO Full
      _BITS   FOVR      : 1;    ///< FIFO Overrun
      _BITS   RFOM      : 1;    ///< Release FIFO Output Mailbox
      _BITS   res2      : 2;    ///< Reserved
    } RFR;


    /** @brief CAN interrupt enable register (CAN_IER) */
    struct {
      _BITS   TMEIE     : 1;    ///< Transmit Mailbox Empty Interrupt Enable
      _BITS   FMPIE     : 1;    ///< FIFO Message Pending Interrupt Enable
      _BITS   FFIE      : 1;    ///< FIFO Full Interrupt Enable
      _BITS   FOVIE     : 1;    ///< FIFO Overrun Interrupt Enable
      _BITS   res       : 3;    ///< Reserved
      _BITS   WKUIE     : 1;    ///< Wakeup Interrupt Enable
    } IER;


    /** @brief CAN diagnosis register (CAN_DGR) */
    struct {
      _BITS   LBKM      : 1;    ///< Loop back mode
      _BITS   SILM      : 1;    ///< Silent mode
      _BITS   SAMP      : 1;    ///< Last sample point
      _BITS   RX        : 1;    ///< CAN Rx Signal
      _BITS   TXM2E     : 1;    ///< TX Mailbox 2 enable
      _BITS   res       : 3;    ///< Reserved
    } DGR;


    /** @brief CAN page selection register for paged registers (CAN_PSR) */
    struct {
      _BITS   PS        : 3;    ///< Page select
      _BITS   res       : 5;    ///< Reserved
    } PSR;



    /** @brief paged CAN registers (selection via CAN_PSR) */
    union {

      /** @brief CAN page 0: Tx Mailbox 0 (CAN.PAGE_0) */
      struct {

        /** @brief CAN message control/status register (CAN_MCSR) */
        struct {
          _BITS   TXRQ      : 1;    ///< Transmission mailbox request
          _BITS   ABRQ      : 1;    ///< Abort request for mailbox
          _BITS   RQCP      : 1;    ///< Request completed
          _BITS   TXOK      : 1;    ///< Transmission OK
          _BITS   ALST      : 1;    ///< Arbitration lost
          _BITS   TERR      : 1;    ///< Transmission error
          _BITS   res       : 2;    ///< Reserved
        } MCSR;


        /** @brief CAN mailbox data length control register (CAN_MDLCR) */
        struct {
          _BITS   DLC       : 4;    ///< Data length code
          _BITS   res       : 3;    ///< Reserved
          _BITS   TGT       : 1;    ///< Transmit global time
        } MDLCR;


        /** @brief CAN mailbox identifier register 1 (CAN_MIDR1) */
        struct {
          _BITS   ID        : 5;    ///< STID[10:6] or EXID[28:24]
          _BITS   RTR       : 1;    ///< Remote transmission request
          _BITS   IDE       : 1;    ///< Extended identifier
          _BITS   res       : 1;    ///< Reserved
        } MIDR1;


        /** @brief CAN mailbox identifier register 2 (CAN_MIDR2) */
        struct {
          _BITS   EXID      : 2;    ///< EXID[17:16]
          _BITS   ID        : 6;    ///< STID[5:0] or EXID[23:18]
        } MIDR2;


        /** @brief CAN mailbox identifier register 3 (CAN_MIDR3) */
        struct {
          _BITS   EXID      : 8;    ///< EXID[15:8]
        } MIDR3;


        /** @brief CAN mailbox identifier register 4 (CAN_MIDR4) */
        struct {
          _BITS   EXID      : 8;    ///< EXID[7:0]
        } MIDR4;


        /** @brief CAN mailbox data register 1 (CAN_MDAR1) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR1;


        /** @brief CAN mailbox data register 2 (CAN_MDAR2) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR2;


        /** @brief CAN mailbox data register 3 (CAN_MDAR3) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR3;


        /** @brief CAN mailbox data register 4 (CAN_MDAR4) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR4;


        /** @brief CAN mailbox data register 5 (CAN_MDAR5) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR5;


        /** @brief CAN mailbox data register 6 (CAN_MDAR6) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR6;


        /** @brief CAN mailbox data register 7 (CAN_MDAR7) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR7;


        /** @brief CAN mailbox data register 8 (CAN_MDAR8) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR8;


        /** @brief CAN mailbox time stamp register low byte (CAN_MTSRL) */
        struct {
          _BITS   TIME      : 8;    ///< Message time stamp [7:0]
        } MTSRL;


        /** @brief CAN mailbox time stamp register high byte (CAN_MTSRH) */
        struct {
          _BITS   TIME      : 8;    ///< Message time stamp [15:8]
        } MTSRH;

      } PAGE_0;



      /** @brief CAN page 1: Tx Mailbox 1 (CAN.PAGE_1) */
      struct {

        /** @brief CAN message control/status register (CAN_MCSR) */
        struct {
          _BITS   TXRQ      : 1;    ///< Transmission mailbox request
          _BITS   ABRQ      : 1;    ///< Abort request for mailbox
          _BITS   RQCP      : 1;    ///< Request completed
          _BITS   TXOK      : 1;    ///< Transmission OK
          _BITS   ALST      : 1;    ///< Arbitration lost
          _BITS   TERR      : 1;    ///< Transmission error
          _BITS   res       : 2;    ///< Reserved
        } MCSR;


        /** @brief CAN mailbox data length control register (CAN_MDLCR) */
        struct {
          _BITS   DLC       : 4;    ///< Data length code
          _BITS   res       : 3;    ///< Reserved
          _BITS   TGT       : 1;    ///< Transmit global time
        } MDLCR;


        /** @brief CAN mailbox identifier register 1 (CAN_MIDR1) */
        struct {
          _BITS   ID        : 5;    ///< STID[10:6] or EXID[28:24]
          _BITS   RTR       : 1;    ///< Remote transmission request
          _BITS   IDE       : 1;    ///< Extended identifier
          _BITS   res       : 1;    ///< Reserved
        } MIDR1;


        /** @brief CAN mailbox identifier register 2 (CAN_MIDR2) */
        struct {
          _BITS   EXID      : 2;    ///< EXID[17:16]
          _BITS   ID        : 6;    ///< STID[5:0] or EXID[23:18]
        } MIDR2;


        /** @brief CAN mailbox identifier register 3 (CAN_MIDR3) */
        struct {
          _BITS   EXID      : 8;    ///< EXID[15:8]
        } MIDR3;


        /** @brief CAN mailbox identifier register 4 (CAN_MIDR4) */
        struct {
          _BITS   EXID      : 8;    ///< EXID[7:0]
        } MIDR4;


        /** @brief CAN mailbox data register 1 (CAN_MDAR1) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR1;


        /** @brief CAN mailbox data register 2 (CAN_MDAR2) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR2;


        /** @brief CAN mailbox data register 3 (CAN_MDAR3) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR3;


        /** @brief CAN mailbox data register 4 (CAN_MDAR4) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR4;


        /** @brief CAN mailbox data register 5 (CAN_MDAR5) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR5;


        /** @brief CAN mailbox data register 6 (CAN_MDAR6) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR6;


        /** @brief CAN mailbox data register 7 (CAN_MDAR7) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR7;


        /** @brief CAN mailbox data register 8 (CAN_MDAR8) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR8;


        /** @brief CAN mailbox time stamp register low byte (CAN_MTSRL) */
        struct {
          _BITS   TIME      : 8;    ///< Message time stamp [7:0]
        } MTSRL;


        /** @brief CAN mailbox time stamp register high byte (CAN_MTSRH) */
        struct {
          _BITS   TIME      : 8;    ///< Message time stamp [15:8]
        } MTSRH;

      } PAGE_1;



      /** @brief CAN page 2: Acceptance Filter 0:1 (CAN.PAGE_2) */
      struct {

        /** @brief CAN reception filter 0/1 (CAN_F0R1) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 0/1
        } F0R1;

        /** @brief CAN reception filter 0/2 (CAN_F0R2) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 0/2
        } F0R2;

        /** @brief CAN reception filter 0/3 (CAN_F0R3) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 0/3
        } F0R3;

        /** @brief CAN reception filter 0/4 (CAN_F0R4) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 0/4
        } F0R4;

        /** @brief CAN reception filter 0/5 (CAN_F0R5) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 0/5
        } F0R5;

        /** @brief CAN reception filter 0/6 (CAN_F0R6) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 0/6
        } F0R6;

        /** @brief CAN reception filter 0/7 (CAN_F0R7) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 0/7
        } F0R7;

        /** @brief CAN reception filter 0/8 (CAN_F0R8) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 0/8
        } F0R8;


        /** @brief CAN reception filter 1/1 (CAN_F1R1) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 1/1
        } F1R1;

        /** @brief CAN reception filter 1/2 (CAN_F1R2) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 1/2
        } F1R2;

        /** @brief CAN reception filter 1/3 (CAN_F1R3) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 1/3
        } F1R3;

        /** @brief CAN reception filter 1/4 (CAN_F1R4) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 1/4
        } F1R4;

        /** @brief CAN reception filter 1/5 (CAN_F1R5) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 1/5
        } F1R5;

        /** @brief CAN reception filter 1/6 (CAN_F1R6) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 1/6
        } F1R6;

        /** @brief CAN reception filter 1/7 (CAN_F1R7) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 1/7
        } F1R7;

        /** @brief CAN reception filter 1/8 (CAN_F1R8) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 1/8
        } F1R8;

      } PAGE_2;



      /** @brief CAN page 3: Acceptance Filter 2:3 (CAN.PAGE_3) */
      struct {

        /** @brief CAN reception filter 2/1 (CAN_F2R1) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 2/1
        } F2R1;

        /** @brief CAN reception filter 2/2 (CAN_F2R2) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 2/2
        } F2R2;

        /** @brief CAN reception filter 2/3 (CAN_F2R3) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 2/3
        } F2R3;

        /** @brief CAN reception filter 2/4 (CAN_F2R4) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 2/4
        } F2R4;

        /** @brief CAN reception filter 2/5 (CAN_F2R5) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 2/5
        } F2R5;

        /** @brief CAN reception filter 2/6 (CAN_F2R6) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 2/6
        } F2R6;

        /** @brief CAN reception filter 2/7 (CAN_F2R7) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 2/7
        } F2R7;

        /** @brief CAN reception filter 2/8 (CAN_F2R8) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 2/8
        } F2R8;


        /** @brief CAN reception filter 3/1 (CAN_F3R1) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 3/1
        } F3R1;

        /** @brief CAN reception filter 3/2 (CAN_F3R2) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 3/2
        } F3R2;

        /** @brief CAN reception filter 3/3 (CAN_F3R3) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 3/3
        } F3R3;

        /** @brief CAN reception filter 3/4 (CAN_F3R4) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 3/4
        } F3R4;

        /** @brief CAN reception filter 3/5 (CAN_F3R5) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 3/5
        } F3R5;

        /** @brief CAN reception filter 3/6 (CAN_F3R6) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 3/6
        } F3R6;

        /** @brief CAN reception filter 3/7 (CAN_F3R7) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 3/7
        } F3R7;

        /** @brief CAN reception filter 3/8 (CAN_F3R8) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 3/8
        } F3R8;

      } PAGE_3;



      /** @brief CAN page 4: Acceptance Filter 4:5 (CAN.PAGE_4) */
      struct {

        /** @brief CAN reception filter 4/1 (CAN_F4R1) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 4/1
        } F4R1;

        /** @brief CAN reception filter 4/2 (CAN_F4R2) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 4/2
        } F4R2;

        /** @brief CAN reception filter 4/3 (CAN_F4R3) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 4/3
        } F4R3;

        /** @brief CAN reception filter 4/4 (CAN_F4R4) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 4/4
        } F4R4;

        /** @brief CAN reception filter 4/5 (CAN_F4R5) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 4/5
        } F4R5;

        /** @brief CAN reception filter 4/6 (CAN_F4R6) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 4/6
        } F4R6;

        /** @brief CAN reception filter 4/7 (CAN_F4R7) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 4/7
        } F4R7;

        /** @brief CAN reception filter 4/8 (CAN_F4R8) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 4/8
        } F4R8;


        /** @brief CAN reception filter 5/1 (CAN_F5R1) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 5/1
        } F5R1;

        /** @brief CAN reception filter 5/2 (CAN_F5R2) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 5/2
        } F5R2;

        /** @brief CAN reception filter 5/3 (CAN_F5R3) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 5/3
        } F5R3;

        /** @brief CAN reception filter 5/4 (CAN_F5R4) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 5/4
        } F5R4;

        /** @brief CAN reception filter 5/5 (CAN_F5R5) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 5/5
        } F5R5;

        /** @brief CAN reception filter 5/6 (CAN_F5R6) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 5/6
        } F5R6;

        /** @brief CAN reception filter 5/7 (CAN_F5R7) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 5/7
        } F5R7;

        /** @brief CAN reception filter 5/8 (CAN_F5R8) */
        struct {
          _BITS   DATA      : 8;    ///< CAN reception filter 5/8
        } F5R8;

      } PAGE_4;



      /** @brief CAN page 5: Tx Mailbox 2 (CAN.PAGE_5) */
      struct {

        /** @brief CAN message control/status register (CAN_MCSR) */
        struct {
          _BITS   TXRQ      : 1;    ///< Transmission mailbox request
          _BITS   ABRQ      : 1;    ///< Abort request for mailbox
          _BITS   RQCP      : 1;    ///< Request completed
          _BITS   TXOK      : 1;    ///< Transmission OK
          _BITS   ALST      : 1;    ///< Arbitration lost
          _BITS   TERR      : 1;    ///< Transmission error
          _BITS   res       : 2;    ///< Reserved
        } MCSR;


        /** @brief CAN mailbox data length control register (CAN_MDLCR) */
        struct {
          _BITS   DLC       : 4;    ///< Data length code
          _BITS   res       : 3;    ///< Reserved
          _BITS   TGT       : 1;    ///< Transmit global time
        } MDLCR;


        /** @brief CAN mailbox identifier register 1 (CAN_MIDR1) */
        struct {
          _BITS   ID        : 5;    ///< STID[10:6] or EXID[28:24]
          _BITS   RTR       : 1;    ///< Remote transmission request
          _BITS   IDE       : 1;    ///< Extended identifier
          _BITS   res       : 1;    ///< Reserved
        } MIDR1;


        /** @brief CAN mailbox identifier register 2 (CAN_MIDR2) */
        struct {
          _BITS   EXID      : 2;    ///< EXID[17:16]
          _BITS   ID        : 6;    ///< STID[5:0] or EXID[23:18]
        } MIDR2;


        /** @brief CAN mailbox identifier register 3 (CAN_MIDR3) */
        struct {
          _BITS   EXID      : 8;    ///< EXID[15:8]
        } MIDR3;


        /** @brief CAN mailbox identifier register 4 (CAN_MIDR4) */
        struct {
          _BITS   EXID      : 8;    ///< EXID[7:0]
        } MIDR4;


        /** @brief CAN mailbox data register 1 (CAN_MDAR1) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR1;


        /** @brief CAN mailbox data register 2 (CAN_MDAR2) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR2;


        /** @brief CAN mailbox data register 3 (CAN_MDAR3) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR3;


        /** @brief CAN mailbox data register 4 (CAN_MDAR4) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR4;


        /** @brief CAN mailbox data register 5 (CAN_MDAR5) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR5;


        /** @brief CAN mailbox data register 6 (CAN_MDAR6) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR6;


        /** @brief CAN mailbox data register 7 (CAN_MDAR7) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR7;


        /** @brief CAN mailbox data register 8 (CAN_MDAR8) */
        struct {
          _BITS   DATA      : 8;    ///< data[7:0]
        } MDAR8;


        /** @brief CAN mailbox time stamp register low byte (CAN_MTSRL) */
        struct {
          _BITS   TIME      : 8;    ///< Message time stamp [7:0]
        } MTSRL;


        /** @brief CAN mailbox time stamp register high byte (CAN_MTSRH) */
        struct {
          _BITS   TIME      : 8;    ///< Message time stamp [15:8]
        } MTSRH;

      } PAGE_5;



      /** @brief CAN page 6: Configuration/Diagnostics (CAN.PAGE_6) */
      struct {

        /** @brief CAN error status register (CAN_ESR) */
        struct {
          _BITS   EWGF      : 1;    ///< Error warning flag
          _BITS   EPVF      : 1;    ///< Error passive flag
          _BITS   BOFF      : 1;    ///< Bus off flag
          _BITS   res       : 1;    ///< Reserved
          _BITS   LEC       : 3;    ///< Last error code
          _BITS   res2      : 1;    ///< Reserved
        } ESR;


        /** @brief CAN error interrupt enable register (CAN_EIER) */
        struct {
          _BITS   EWGIE     : 1;    ///< Error warning interrupt enable
          _BITS   EPVIE     : 1;    ///< Error passive  interrupt enable
          _BITS   BOFIE     : 1;    ///< Bus-Off  interrupt enable
          _BITS   res       : 1;    ///< Reserved
          _BITS   LECIE     : 1;    ///< Last error code interrupt enable
          _BITS   res2      : 2;    ///< Reserved
          _BITS   ERRIE     : 1;    ///< Error interrupt enable
        } EIER;


        /** @brief CAN transmit error counter register (CAN_TECR) */
        struct {
          _BITS   TEC       : 8;    ///< Transmit error counter
        } TECR;


        /** @brief CAN receive error counter register (CAN_RECR) */
        struct {
          _BITS   REC       : 8;    ///< Receive error counter
        } RECR;


        /** @brief CAN bit timing register 1 (CAN_BTR1) */
        struct {
          _BITS   BRP       : 6;    ///< Baud rate prescaler
          _BITS   SJW       : 2;    ///< Resynchronization jump width
        } BTR1;


        /** @brief CAN bit timing register 2 (CAN_BTR2) */
        struct {
          _BITS   BS1       : 4;    ///< Bit segment 1
          _BITS   BS2       : 3;    ///< Bit segment 2
          _BITS   res       : 1;    ///< Reserved, must be kept cleared
        } BTR2;


        /** @brief Reserved registers (2B) */
        uint8_t         res[2];


        /** @brief CAN filter mode register 1 (CAN_FMR1) */
        struct {
          _BITS   FML0      : 1;    ///< Filter 0 mode low
          _BITS   FMH0      : 1;    ///< Filter 0 mode high
          _BITS   FML1      : 1;    ///< Filter 1 mode low
          _BITS   FMH1      : 1;    ///< Filter 1 mode high
          _BITS   FML2      : 1;    ///< Filter 2 mode low
          _BITS   FMH2      : 1;    ///< Filter 2 mode high
          _BITS   FML3      : 1;    ///< Filter 3 mode low
          _BITS   FMH3      : 1;    ///< Filter 3 mode high
        } FMR1;


        /** @brief CAN filter mode register 2 (CAN_FMR2) */
        struct {
          _BITS   FML4      : 1;    ///< Filter 4 mode low
          _BITS   FMH4      : 1;    ///< Filter 4 mode high
          _BITS   FML5      : 1;    ///< Filter 5 mode low
          _BITS   FMH5      : 1;    ///< Filter 5 mode high
          _BITS   res       : 4;    ///< Reserved
        } FMR2;


        /** @brief CAN filter configuration register 1 (CAN_FCR1) */
        struct {
          _BITS   FACT0     : 1;    ///< Filter 0 active
          _BITS   FSC0      : 2;    ///< Filter 0 scale configuration
          _BITS   res       : 1;    ///< Reserved
          _BITS   FACT1     : 1;    ///< Filter 1 active
          _BITS   FSC1      : 2;    ///< Filter 1 scale configuration
          _BITS   res2      : 1;    ///< Reserved
        } FCR1;


        /** @brief CAN filter configuration register 2 (CAN_FCR2) */
        struct {
          _BITS   FACT2     : 1;    ///< Filter 2 active
          _BITS   FSC2      : 2;    ///< Filter 2 scale configuration
          _BITS   res       : 1;    ///< Reserved
          _BITS   FACT3     : 1;    ///< Filter 3 active
          _BITS   FSC3      : 2;    ///< Filter 3 scale configuration
          _BITS   res2      : 1;    ///< Reserve
        } FCR2;


        /** @brief CAN filter configuration register 3 (CAN_FCR3) */
        struct {
          _BITS   FACT4     : 1;    ///< Filter 4 active
          _BITS   FSC4      : 2;    ///< Filter 4 scale configuration
          _BITS   res       : 1;    ///< Reserved
          _BITS   FACT5     : 1;    ///< Filter 5 active
          _BITS   FSC5      : 2;    ///< Filter 5 scale configuration
          _BITS   res2      : 1;    ///< Reserve
        } FCR3;


        /** @brief Reserved registers (3B) */
        _BITS         res2[3];

      } PAGE_6;



      /** @brief CAN page 7: Receive FIFO (CAN.PAGE_7) */
      struct {

        /** @brief CAN mailbox filter match index register (CAN_MFMIR) */
        struct {
          _BITS   FMI     : 8;    ///< Filter match index
        } MFMIR;


        /** @brief CAN mailbox data length control register (CAN_MDLCR) */
        struct {
          _BITS   DLC       : 4;    ///< Data length code
          _BITS   res       : 3;    ///< Reserved
          _BITS   TGT       : 1;    ///< Transmit global time
        } MDLCR;


        /** @brief CAN mailbox identifier register 1 (CAN_MIDR1)*/
        struct {
          _BITS   ID        : 5;    ///< STID[10:6] or EXID[28:24]
          _BITS   RTR       : 1;    ///< Remote transmission request
          _BITS   IDE       : 1;    ///< Extended identifier
          _BITS   res       : 1;    ///< Reserved
        } MIDR1;


        /** @brief CAN mailbox identifier register 2 (CAN_MIDR2) */
        struct {
          _BITS   EXID      : 2;    ///< EXID[17:16]
          _BITS   ID        : 6;    ///< STID[5:0] or EXID[23:18]
        } MIDR2;


        /** @brief CAN mailbox identifier register 3 (CAN_MIDR3) */
        struct {
          _BITS   EXID      : 8;    ///< EXID[15:8]
        } MIDR3;


        /** @brief CAN mailbox identifier register 4 (CAN_MIDR4) */
        struct {
          _BITS   EXID      : 8;    ///< EXID[7:0]
        } MIDR4;


        /** @brief CAN mailbox data register 1 (CAN_MDAR1) */
        struct {
          _BITS   DATA      : 8;    ///< CAN mailbox data register 1
        } MDAR1;


        /** @brief CAN mailbox data register 2 (CAN_MDAR2) */
        struct {
          _BITS   DATA      : 8;    ///< CAN mailbox data register 2
        } MDAR2;


        /** @brief CAN mailbox data register 3 (CAN_MDAR3) */
        struct {
          _BITS   DATA      : 8;    ///< CAN mailbox data register 3
        } MDAR3;


        /** @brief CAN mailbox data register 4 (CAN_MDAR4) */
        struct {
          _BITS   DATA      : 8;    ///< CAN mailbox data register 4
        } MDAR4;


        /** @brief CAN mailbox data register 5 (CAN_MDAR5) */
        struct {
          _BITS   DATA      : 8;    ///< CAN mailbox data register 5
        } MDAR5;


        /** @brief CAN mailbox data register 6 (CAN_MDAR6) */
        struct {
          _BITS   DATA      : 8;    ///< CAN mailbox data register 6
        } MDAR6;


        /** @brief CAN mailbox data register 7 (CAN_MDAR7) */
        struct {
          _BITS   DATA      : 8;    ///< CAN mailbox data register 7
        } MDAR7;


        /** @brief CAN mailbox data register 8 (CAN_MDAR8) */
        struct {
          _BITS   DATA      : 8;    ///< CAN mailbox data register 8
        } MDAR8;


        /** @brief CAN mailbox time stamp register low byte (CAN_MTSRL) */
        struct {
          _BITS   TIME      : 8;    ///< Message time stamp [7:0]
        } MTSRL;


        /** @brief CAN mailbox time stamp register high byte (CAN_MTSRH) */
        struct {
          _BITS   TIME      : 8;    ///< Message time stamp [15:8]
        } MTSRH;

      } PAGE_7;

    } Page;

  } CAN_t;

  /* Pointer to CAN registers */
  #define _CAN        _SFR(CAN_t,      CAN_AddressBase)         ///< CAN struct/bit access
  #define _CAN_MCR    _SFR(uint8_t,    CAN_AddressBase+0x00)    ///< CAN master control register
  #define _CAN_MSR    _SFR(uint8_t,    CAN_AddressBase+0x01)    ///< CAN master status register
  #define _CAN_TSR    _SFR(uint8_t,    CAN_AddressBase+0x02)    ///< CAN transmit status register
  #define _CAN_TPR    _SFR(uint8_t,    CAN_AddressBase+0x03)    ///< CAN transmit priority register
  #define _CAN_RFR    _SFR(uint8_t,    CAN_AddressBase+0x04)    ///< CAN receive FIFO register
  #define _CAN_IER    _SFR(uint8_t,    CAN_AddressBase+0x05)    ///< CAN interrupt enable register
  #define _CAN_DGR    _SFR(uint8_t,    CAN_AddressBase+0x06)    ///< CAN diagnosis register
  #define _CAN_PSR    _SFR(uint8_t,    CAN_AddressBase+0x07)    ///< CAN page selection for below paged registers

  // CAN page 0,1,5: Tx Mailbox 0,1,2 (via _CAN_PSR)
  #define _CAN_MCSR   _SFR(uint8_t,    CAN_AddressBase+0x08+0x00)    ///< CAN message control/status register (page 0,1,5)
  #define _CAN_MDLCR  _SFR(uint8_t,    CAN_AddressBase+0x08+0x01)    ///< CAN mailbox data length control register (page 0,1,5,7)
  #define _CAN_MIDR1  _SFR(uint8_t,    CAN_AddressBase+0x08+0x02)    ///< CAN mailbox identifier register 1 (page 0,1,5,7)
  #define _CAN_MIDR2  _SFR(uint8_t,    CAN_AddressBase+0x08+0x03)    ///< CAN mailbox identifier register 2 (page 0,1,5,7)
  #define _CAN_MIDR3  _SFR(uint8_t,    CAN_AddressBase+0x08+0x04)    ///< CAN mailbox identifier register 3 (page 0,1,5,7)
  #define _CAN_MIDR4  _SFR(uint8_t,    CAN_AddressBase+0x08+0x05)    ///< CAN mailbox identifier register 4 (page 0,1,5,7)
  #define _CAN_MDAR1  _SFR(uint8_t,    CAN_AddressBase+0x08+0x06)    ///< CAN mailbox data register 1 (page 0,1,5,7) */
  #define _CAN_MDAR2  _SFR(uint8_t,    CAN_AddressBase+0x08+0x07)    ///< CAN mailbox data register 2 (page 0,1,5,7) */
  #define _CAN_MDAR3  _SFR(uint8_t,    CAN_AddressBase+0x08+0x08)    ///< CAN mailbox data register 3 (page 0,1,5,7) */
  #define _CAN_MDAR4  _SFR(uint8_t,    CAN_AddressBase+0x08+0x09)    ///< CAN mailbox data register 4 (page 0,1,5,7) */
  #define _CAN_MDAR5  _SFR(uint8_t,    CAN_AddressBase+0x08+0x0A)    ///< CAN mailbox data register 5 (page 0,1,5,7) */
  #define _CAN_MDAR6  _SFR(uint8_t,    CAN_AddressBase+0x08+0x0B)    ///< CAN mailbox data register 6 (page 0,1,5,7) */
  #define _CAN_MDAR7  _SFR(uint8_t,    CAN_AddressBase+0x08+0x0C)    ///< CAN mailbox data register 7 (page 0,1,5,7) */
  #define _CAN_MDAR8  _SFR(uint8_t,    CAN_AddressBase+0x08+0x0D)    ///< CAN mailbox data register 8 (page 0,1,5,7) */
  #define _CAN_MTSRL  _SFR(uint8_t,    CAN_AddressBase+0x08+0x0E)    ///< CAN mailbox time stamp register low byte (page 0,1,5,7) */
  #define _CAN_MTSRH  _SFR(uint8_t,    CAN_AddressBase+0x08+0x0F)    ///< CAN mailbox time stamp register high byte (page 0,1,5,7) */

  // CAN page 1: Tx Mailbox 1 (via _CAN_PSR). Already defined for page 0

  // CAN page 2: Acceptance Filter 0:1 (via _CAN_PSR)
  #define _CAN_F0R1   _SFR(uint8_t,    CAN_AddressBase+0x08+0x00)    ///< CAN acceptance filter 0/1 (page 2)
  #define _CAN_F0R2   _SFR(uint8_t,    CAN_AddressBase+0x08+0x01)    ///< CAN acceptance filter 0/2 (page 2)
  #define _CAN_F0R3   _SFR(uint8_t,    CAN_AddressBase+0x08+0x02)    ///< CAN acceptance filter 0/3 (page 2)
  #define _CAN_F0R4   _SFR(uint8_t,    CAN_AddressBase+0x08+0x03)    ///< CAN acceptance filter 0/4 (page 2)
  #define _CAN_F0R5   _SFR(uint8_t,    CAN_AddressBase+0x08+0x04)    ///< CAN acceptance filter 0/5 (page 2)
  #define _CAN_F0R6   _SFR(uint8_t,    CAN_AddressBase+0x08+0x05)    ///< CAN acceptance filter 0/6 (page 2)
  #define _CAN_F0R7   _SFR(uint8_t,    CAN_AddressBase+0x08+0x06)    ///< CAN acceptance filter 0/7 (page 2)
  #define _CAN_F0R8   _SFR(uint8_t,    CAN_AddressBase+0x08+0x07)    ///< CAN acceptance filter 0/8 (page 2)
  #define _CAN_F1R1   _SFR(uint8_t,    CAN_AddressBase+0x08+0x08)    ///< CAN acceptance filter 1/1 (page 2)
  #define _CAN_F1R2   _SFR(uint8_t,    CAN_AddressBase+0x08+0x09)    ///< CAN acceptance filter 1/2 (page 2)
  #define _CAN_F1R3   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0A)    ///< CAN acceptance filter 1/3 (page 2)
  #define _CAN_F1R4   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0B)    ///< CAN acceptance filter 1/4 (page 2)
  #define _CAN_F1R5   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0C)    ///< CAN acceptance filter 1/5 (page 2)
  #define _CAN_F1R6   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0D)    ///< CAN acceptance filter 1/6 (page 2)
  #define _CAN_F1R7   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0E)    ///< CAN acceptance filter 1/7 (page 2)
  #define _CAN_F1R8   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0F)    ///< CAN acceptance filter 1/8 (page 2)

  // CAN page 3: Acceptance Filter 2:3 (via _CAN_PSR)
  #define _CAN_F2R1   _SFR(uint8_t,    CAN_AddressBase+0x08+0x00)    ///< CAN acceptance filter 2/1 (page 3)
  #define _CAN_F2R2   _SFR(uint8_t,    CAN_AddressBase+0x08+0x01)    ///< CAN acceptance filter 2/2 (page 3)
  #define _CAN_F2R3   _SFR(uint8_t,    CAN_AddressBase+0x08+0x02)    ///< CAN acceptance filter 2/3 (page 3)
  #define _CAN_F2R4   _SFR(uint8_t,    CAN_AddressBase+0x08+0x03)    ///< CAN acceptance filter 2/4 (page 3)
  #define _CAN_F2R5   _SFR(uint8_t,    CAN_AddressBase+0x08+0x04)    ///< CAN acceptance filter 2/5 (page 3)
  #define _CAN_F2R6   _SFR(uint8_t,    CAN_AddressBase+0x08+0x05)    ///< CAN acceptance filter 2/6 (page 3)
  #define _CAN_F2R7   _SFR(uint8_t,    CAN_AddressBase+0x08+0x06)    ///< CAN acceptance filter 2/7 (page 3)
  #define _CAN_F2R8   _SFR(uint8_t,    CAN_AddressBase+0x08+0x07)    ///< CAN acceptance filter 2/8 (page 3)
  #define _CAN_F3R1   _SFR(uint8_t,    CAN_AddressBase+0x08+0x08)    ///< CAN acceptance filter 3/1 (page 3)
  #define _CAN_F3R2   _SFR(uint8_t,    CAN_AddressBase+0x08+0x09)    ///< CAN acceptance filter 3/2 (page 3)
  #define _CAN_F3R3   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0A)    ///< CAN acceptance filter 3/3 (page 3)
  #define _CAN_F3R4   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0B)    ///< CAN acceptance filter 3/4 (page 3)
  #define _CAN_F3R5   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0C)    ///< CAN acceptance filter 3/5 (page 3)
  #define _CAN_F3R6   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0D)    ///< CAN acceptance filter 3/6 (page 3)
  #define _CAN_F3R7   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0E)    ///< CAN acceptance filter 3/7 (page 3)
  #define _CAN_F3R8   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0F)    ///< CAN acceptance filter 3/8 (page 3)

  // CAN page 4: Acceptance Filter 4:5 (via _CAN_PSR)
  #define _CAN_F4R1   _SFR(uint8_t,    CAN_AddressBase+0x08+0x00)    ///< CAN acceptance filter 4/1 (page 4)
  #define _CAN_F4R2   _SFR(uint8_t,    CAN_AddressBase+0x08+0x01)    ///< CAN acceptance filter 4/2 (page 4)
  #define _CAN_F4R3   _SFR(uint8_t,    CAN_AddressBase+0x08+0x02)    ///< CAN acceptance filter 4/3 (page 4)
  #define _CAN_F4R4   _SFR(uint8_t,    CAN_AddressBase+0x08+0x03)    ///< CAN acceptance filter 4/4 (page 4)
  #define _CAN_F4R5   _SFR(uint8_t,    CAN_AddressBase+0x08+0x04)    ///< CAN acceptance filter 4/5 (page 4)
  #define _CAN_F4R6   _SFR(uint8_t,    CAN_AddressBase+0x08+0x05)    ///< CAN acceptance filter 4/6 (page 4)
  #define _CAN_F4R7   _SFR(uint8_t,    CAN_AddressBase+0x08+0x06)    ///< CAN acceptance filter 4/7 (page 4)
  #define _CAN_F4R8   _SFR(uint8_t,    CAN_AddressBase+0x08+0x07)    ///< CAN acceptance filter 4/8 (page 4)
  #define _CAN_F5R1   _SFR(uint8_t,    CAN_AddressBase+0x08+0x08)    ///< CAN acceptance filter 5/1 (page 4)
  #define _CAN_F5R2   _SFR(uint8_t,    CAN_AddressBase+0x08+0x09)    ///< CAN acceptance filter 5/2 (page 4)
  #define _CAN_F5R3   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0A)    ///< CAN acceptance filter 5/3 (page 4)
  #define _CAN_F5R4   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0B)    ///< CAN acceptance filter 5/4 (page 4)
  #define _CAN_F5R5   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0C)    ///< CAN acceptance filter 5/5 (page 4)
  #define _CAN_F5R6   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0D)    ///< CAN acceptance filter 5/6 (page 4)
  #define _CAN_F5R7   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0E)    ///< CAN acceptance filter 5/7 (page 4)
  #define _CAN_F5R8   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0F)    ///< CAN acceptance filter 5/8 (page 4)

  // CAN page 5: Tx Mailbox 2 (via _CAN_PSR). Already defined for page 0

  // CAN page 6: Configuration/Diagnostics (via _CAN_PSR)
  #define _CAN_ESR    _SFR(uint8_t,    CAN_AddressBase+0x08+0x00)    ///< CAN error status register (page 6)
  #define _CAN_EIER   _SFR(uint8_t,    CAN_AddressBase+0x08+0x01)    ///< CAN error interrupt enable register (page 6)
  #define _CAN_TECR   _SFR(uint8_t,    CAN_AddressBase+0x08+0x02)    ///< CAN transmit error counter register (page 6)
  #define _CAN_RECR   _SFR(uint8_t,    CAN_AddressBase+0x08+0x03)    ///< CAN receive error counter register (page 6)
  #define _CAN_BTR1   _SFR(uint8_t,    CAN_AddressBase+0x08+0x04)    ///< CAN bit timing register 1 (page 6)
  #define _CAN_BTR2   _SFR(uint8_t,    CAN_AddressBase+0x08+0x05)    ///< CAN bit timing register 2 (page 6)
  // reserved 2B
  #define _CAN_FMR1   _SFR(uint8_t,    CAN_AddressBase+0x08+0x08)    ///< CAN filter mode register 1 (page 6)
  #define _CAN_FMR2   _SFR(uint8_t,    CAN_AddressBase+0x08+0x09)    ///< CAN filter mode register 2 (page 6)
  #define _CAN_FCR1   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0A)    ///< CAN filter configuration register 1 (page 6)
  #define _CAN_FCR2   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0B)    ///< CAN filter configuration register 2 (page 6)
  #define _CAN_FCR3   _SFR(uint8_t,    CAN_AddressBase+0x08+0x0C)    ///< CAN filter configuration register 3 (page 6)
  // reserved 3B

  // CAN page 7: Receive FIFO (via _CAN_PSR)
  #define _CAN_MFMIR  _SFR(uint8_t,    CAN_AddressBase+0x08+0x00)    ///< CAN mailbox filter match index register (page 7)
  //#define _CAN_MDLCR  _SFR(uint8_t,    CAN_AddressBase+0x08+0x01)    ///< CAN mailbox data length control register (page 0,1,5,7)
  //#define _CAN_MIDR1  _SFR(uint8_t,    CAN_AddressBase+0x08+0x02)    ///< CAN mailbox identifier register 1 (page 0,1,5,7)
  //#define _CAN_MIDR2  _SFR(uint8_t,    CAN_AddressBase+0x08+0x03)    ///< CAN mailbox identifier register 2 (page 0,1,5,7)
  //#define _CAN_MIDR3  _SFR(uint8_t,    CAN_AddressBase+0x08+0x04)    ///< CAN mailbox identifier register 3 (page 0,1,5,7)
  //#define _CAN_MIDR4  _SFR(uint8_t,    CAN_AddressBase+0x08+0x05)    ///< CAN mailbox identifier register 4 (page 0,1,5,7)
  //#define _CAN_MDAR1  _SFR(uint8_t,    CAN_AddressBase+0x08+0x06)    ///< CAN mailbox data register 1 (page 0,1,5,7)
  //#define _CAN_MDAR2  _SFR(uint8_t,    CAN_AddressBase+0x08+0x07)    ///< CAN mailbox data register 2 (page 0,1,5,7)
  //#define _CAN_MDAR3  _SFR(uint8_t,    CAN_AddressBase+0x08+0x08)    ///< CAN mailbox data register 3 (page 0,1,5,7)
  //#define _CAN_MDAR4  _SFR(uint8_t,    CAN_AddressBase+0x08+0x09)    ///< CAN mailbox data register 4 (page 0,1,5,7)
  //#define _CAN_MDAR5  _SFR(uint8_t,    CAN_AddressBase+0x08+0x0A)    ///< CAN mailbox data register 5 (page 0,1,5,7)
  //#define _CAN_MDAR6  _SFR(uint8_t,    CAN_AddressBase+0x08+0x0B)    ///< CAN mailbox data register 6 (page 0,1,5,7)
  //#define _CAN_MDAR7  _SFR(uint8_t,    CAN_AddressBase+0x08+0x0C)    ///< CAN mailbox data register 7 (page 0,1,5,7)
  //#define _CAN_MDAR8  _SFR(uint8_t,    CAN_AddressBase+0x08+0x0D)    ///< CAN mailbox data register 8 (page 0,1,5,7)
  //#define _CAN_MTSRL  _SFR(uint8_t,    CAN_AddressBase+0x0A+0x0E)    ///< CAN mailbox time stamp register low byte (page 0,1,5,7) */
  //#define _CAN_MTSRH  _SFR(uint8_t,    CAN_AddressBase+0x0A+0x0F)    ///< CAN mailbox time stamp register high byte (page 0,1,5,7) */

  /* CAN Module Reset Values */
  #define  _CAN_MCR_RESET_VALUE        ((uint8_t) 0x02)          ///< CAN master control register reset value
  #define  _CAN_MSR_RESET_VALUE        ((uint8_t) 0x02)          ///< CAN master status register reset value
  #define  _CAN_TSR_RESET_VALUE        ((uint8_t) 0x00)          ///< CAN transmit status register reset value
  #define  _CAN_TPR_RESET_VALUE        ((uint8_t) 0x0C)          ///< CAN transmit priority register reset value
  #define  _CAN_RFR_RESET_VALUE        ((uint8_t) 0x00)          ///< CAN receive FIFO register reset value
  #define  _CAN_IER_RESET_VALUE        ((uint8_t) 0x00)          ///< CAN interrupt enable register reset value
  #define  _CAN_DGR_RESET_VALUE        ((uint8_t) 0x0C)          ///< CAN diagnosis register reset value
  #define  _CAN_PSR_RESET_VALUE        ((uint8_t) 0x00)          ///< CAN page selection reset value

  #define  _CAN_MCSR_RESET_VALUE       ((uint8_t) 0x00)          ///< CAN message control/status register (page 0,1,5) reset value
  #define  _CAN_MDLCR_RESET_VALUE      ((uint8_t) 0x00)          ///< CAN mailbox data length control register (page 0,1,5,7) reset value
  #define  _CAN_ESR_RESET_VALUE        ((uint8_t) 0x00)          ///< CAN error status register (page 6) reset value
  #define  _CAN_EIER_RESET_VALUE       ((uint8_t) 0x00)          ///< CAN error interrupt enable register (page 6) reset value
  #define  _CAN_TECR_RESET_VALUE       ((uint8_t) 0x00)          ///< CAN transmit error counter register (page 6) reset value
  #define  _CAN_RECR_RESET_VALUE       ((uint8_t) 0x00)          ///< CAN receive error counter register (page 6) reset value
  #define  _CAN_BTR1_RESET_VALUE       ((uint8_t) 0x40)          ///< CAN bit timing register 1 (page 6) reset value
  #define  _CAN_BTR2_RESET_VALUE       ((uint8_t) 0x23)          ///< CAN bit timing register 2 (page 6) reset value
  #define  _CAN_FMR1_RESET_VALUE       ((uint8_t) 0x00)          ///< CAN filter mode register 1 (page 6) reset value
  #define  _CAN_FMR2_RESET_VALUE       ((uint8_t) 0x00)          ///< CAN filter mode register 2 (page 6) reset value
  #define  _CAN_FCR_RESET_VALUE        ((uint8_t) 0x00)          ///< CAN filter configuration register reset value
  #define  _CAN_MFMIR_RESET_VALUE      ((uint8_t) 0x00)          ///< CAN mailbox filter match index register reset value

  /* CAN master control register (CAN_MCR) */
  #define _CAN_INRQ               ((uint8_t) (0x01 << 0))        ///< CAN Channel Initialization Request [0] (in _CAN_MCR)
  #define _CAN_SLEEP              ((uint8_t) (0x01 << 1))        ///< CAN Channel Sleep Mode Request [0] (in _CAN_MCR)
  #define _CAN_TXFP               ((uint8_t) (0x01 << 2))        ///< CAN Channel Transmit FIFO Priority [0] (in _CAN_MCR)
  #define _CAN_RFLM               ((uint8_t) (0x01 << 3))        ///< CAN Channel Receive FIFO Locked Mode [0] (in _CAN_MCR)
  #define _CAN_NART               ((uint8_t) (0x01 << 4))        ///< CAN Channel No Automatic Retransmission [0] (in _CAN_MCR)
  #define _CAN_AWUM               ((uint8_t) (0x01 << 5))        ///< CAN Channel Automatic Wakeup Mode [0] (in _CAN_MCR)
  #define _CAN_ABOM               ((uint8_t) (0x01 << 6))        ///< CAN Channel Automatic Bus-Off Management [0] (in _CAN_MCR)
  #define _CAN_TTCM               ((uint8_t) (0x01 << 7))        ///< CAN Channel Time Triggered Communication Mode [0] (in _CAN_MCR)

  /* CAN master status register (CAN_MSR) */
  #define _CAN_INAK               ((uint8_t) (0x01 << 0))        ///< CAN Initialization Acknowledge [0] (in _CAN_MSR)
  #define _CAN_SLAK               ((uint8_t) (0x01 << 1))        ///< CAN Sleep Acknowledge [0] (in _CAN_MSR)
  #define _CAN_ERRI               ((uint8_t) (0x01 << 2))        ///< CAN Error Interrupt [0] (in _CAN_MSR)
  #define _CAN_WKUI               ((uint8_t) (0x01 << 3))        ///< CAN Wakeup Interrupt [0] (in _CAN_MSR)
  #define _CAN_TX                 ((uint8_t) (0x01 << 4))        ///< CAN Transmit [0] (in _CAN_MSR)
  #define _CAN_RX                 ((uint8_t) (0x01 << 5))        ///< CAN Receive [0] (in _CAN_MSR)
  // reserved [7:6]

  /* CAN transmit status register (CAN_TSR) */
  #define _CAN_RQCP0              ((uint8_t) (0x01 << 0))        ///< CAN Request Completed for Mailbox 0 [0] (in _CAN_TSR)
  #define _CAN_RQCP1              ((uint8_t) (0x01 << 1))        ///< CAN Request Completed for Mailbox 1 [0] (in _CAN_TSR)
  #define _CAN_RQCP2              ((uint8_t) (0x01 << 2))        ///< CAN Request Completed for Mailbox 2 [0] (in _CAN_TSR)
  // reserved [3]
  #define _CAN_TXOK0              ((uint8_t) (0x01 << 4))        ///< CAN Transmission ok for Mailbox 0 [0] (in _CAN_TSR)
  #define _CAN_TXOK1              ((uint8_t) (0x01 << 5))        ///< CAN Transmission ok for Mailbox 1 [0] (in _CAN_TSR)
  #define _CAN_TXOK2              ((uint8_t) (0x01 << 6))        ///< CAN Transmission ok for Mailbox 2 [0] (in _CAN_TSR)
  // reserved [7]

  /* CAN transmit priority register (CAN_TPR) */
  #define _CAN_CODE              ((uint8_t) (0x03 << 0))        ///< CAN Mailbox Code [1:0] (in _CAN_TPR)
  #define _CAN_CODE0             ((uint8_t) (0x01 << 0))        ///< CAN Mailbox Code [0] (in _CAN_TPR)
  #define _CAN_CODE1             ((uint8_t) (0x01 << 1))        ///< CAN Mailbox Code [1] (in _CAN_TPR)
  #define _CAN_TME0              ((uint8_t) (0x01 << 2))        ///< CAN Transmit Mailbox 0 Empty [0] (in _CAN_TPR)
  #define _CAN_TME1              ((uint8_t) (0x01 << 3))        ///< CAN Transmit Mailbox 1 Empty [0] (in _CAN_TPR)
  #define _CAN_TME2              ((uint8_t) (0x01 << 4))        ///< CAN Transmit Mailbox 2 Empty [0] (in _CAN_TPR)
  #define _CAN_LOW0              ((uint8_t) (0x01 << 5))        ///< CAN Lowest Priority Flag for Mailbox 0 [0] (in _CAN_TPR)
  #define _CAN_LOW1              ((uint8_t) (0x01 << 6))        ///< CAN Lowest Priority Flag for Mailbox 1 [0] (in _CAN_TPR)
  #define _CAN_LOW2              ((uint8_t) (0x01 << 7))        ///< CAN Lowest Priority Flag for Mailbox 2 [0] (in _CAN_TPR)

  /* CAN receive FIFO register (CAN_RFR) */
  #define _CAN_FMP               ((uint8_t) (0x03 << 0))        ///< CAN FIFO Message Pending [1:0] (in _CAN_RFR)
  #define _CAN_FMP0              ((uint8_t) (0x01 << 0))        ///< CAN FIFO Message Pending [0] (in _CAN_RFR)
  #define _CAN_FMP1              ((uint8_t) (0x01 << 1))        ///< CAN FIFO Message Pending [1] (in _CAN_RFR)
  // reserved [2]
  #define _CAN_FULL              ((uint8_t) (0x01 << 3))        ///< CAN FIFO Full [0] (in _CAN_RFR)
  #define _CAN_FOVR              ((uint8_t) (0x01 << 4))        ///< CAN FIFO Overrun [0] (in _CAN_RFR)
  #define _CAN_RFOM              ((uint8_t) (0x01 << 5))        ///< CAN Release FIFO Output Mailbox [0] (in _CAN_RFR)
  // reserved [7:6]

  /* CAN interrupt enable register (CAN_IER) */
  #define _CAN_TMEIE             ((uint8_t) (0x01 << 0))        ///< CAN Transmit Mailbox Empty Interrupt Enable [0] (in _CAN_IER)
  #define _CAN_FMPIE             ((uint8_t) (0x01 << 1))        ///< CAN FIFO Message Pending Interrupt Enable [0] (in _CAN_IER)
  #define _CAN_FFIE              ((uint8_t) (0x01 << 2))        ///< CAN FIFO Full Interrupt Enable [0] (in _CAN_IER)
  #define _CAN_FOVIE             ((uint8_t) (0x01 << 3))        ///< CAN FIFO Overrun Interrupt Enable [0] (in _CAN_IER)
  // reserved [6:4]
  #define _CAN_WKUIE             ((uint8_t) (0x01 << 7))        ///< CAN Wakeup Interrupt Enable [0] (in _CAN_IER)

  /* CAN diagnosis register (CAN_DGR) */
  #define _CAN_LBKM              ((uint8_t) (0x01 << 0))        ///< CAN Loop back mode [0] (in _CAN_DGR)
  #define _CAN_SILM              ((uint8_t) (0x01 << 1))        ///< CAN Silent mode [0] (in _CAN_DGR)
  #define _CAN_SAMP              ((uint8_t) (0x01 << 2))        ///< CAN Last sample point [0] (in _CAN_DGR)
  #define _CAN_RXS               ((uint8_t) (0x01 << 3))        ///< CAN Rx Signal (=pin status) [0] (in _CAN_DGR)
  #define _CAN_TXM2E             ((uint8_t) (0x01 << 4))        ///< CAN TX Mailbox 2 enable [0] (in _CAN_DGR)
  // reserved [7:5]

  /* CAN page selection register for paged registers (CAN_PSR) */
  #define _CAN_PS                ((uint8_t) (0x07 << 0))        ///< CAN Page select [2:0] (in _CAN_PSR)
  #define _CAN_PS0               ((uint8_t) (0x01 << 0))        ///< CAN Page select [0] (in _CAN_PSR)
  #define _CAN_PS1               ((uint8_t) (0x01 << 1))        ///< CAN Page select [1] (in _CAN_PSR)
  #define _CAN_PS2               ((uint8_t) (0x01 << 2))        ///< CAN Page select [2] (in _CAN_PSR)
  // reserved [7:3]

  /* CAN message control/status register (CAN_MCSR, page 0,1,5) */
  #define _CAN_TXRQ              ((uint8_t) (0x01 << 0))        ///< CAN Transmission mailbox request [0] (in _CAN_MCSR, page 0,1,5)
  #define _CAN_ABRQ              ((uint8_t) (0x01 << 1))        ///< CAN Abort request for mailbox [0] (in _CAN_MCSR, page 0,1,5)
  #define _CAN_RQCP              ((uint8_t) (0x01 << 2))        ///< CAN Request completed [0] (in _CAN_MCSR, page 0,1,5)
  #define _CAN_TXOK              ((uint8_t) (0x01 << 3))        ///< CAN Transmission OK [0] (in _CAN_MCSR, page 0,1,5)
  #define _CAN_ALST              ((uint8_t) (0x01 << 4))        ///< CAN Arbitration lost [0] (in _CAN_MCSR, page 0,1,5)
  #define _CAN_TERR              ((uint8_t) (0x01 << 5))        ///< CAN Transmission error [0] (in _CAN_MCSR, page 0,1,5)
  // reserved [7:6]

  /* CAN mailbox data length control register (CAN_MDLCR, page 0,1,5,7) */
  #define _CAN_DLC               ((uint8_t) (0x0F << 0))        ///< CAN Data length code [3:0] (in _CAN_MDLCR, page 0,1,5,7)
  #define _CAN_DLC0              ((uint8_t) (0x01 << 0))        ///< CAN Data length code [0] (in _CAN_MDLCR, page 0,1,5,7)
  #define _CAN_DLC1              ((uint8_t) (0x01 << 1))        ///< CAN Data length code [1] (in _CAN_MDLCR, page 0,1,5,7)
  #define _CAN_DLC2              ((uint8_t) (0x01 << 2))        ///< CAN Data length code [2] (in _CAN_MDLCR, page 0,1,5,7)
  #define _CAN_DLC3              ((uint8_t) (0x01 << 3))        ///< CAN Data length code [3] (in _CAN_MDLCR, page 0,1,5,7)
  // reserved [6:4]
  #define _CAN_TGT               ((uint8_t) (0x01 << 7))        ///< CAN Transmit global time [0] (in _CAN_MDLCR, page 0,1,5,7)

  /* CAN mailbox identifier register 1 (CAN_MIDR1, page 0,1,5) */
  // skip ID [4:0], as it depends on addressing mode
  #define _CAN_RTR               ((uint8_t) (0x01 << 5))        ///< CAN Remote transmission request [0] (in _CAN_MIDR1, page 0,1,5)
  #define _CAN_IDE               ((uint8_t) (0x01 << 6))        ///< CAN Extended identifier [0] (in _CAN_MIDR1, page 0,1,5)
  // reserved [7]

  /* CAN error status register (CAN_ESR, page 6) */
  #define _CAN_EWGF              ((uint8_t) (0x01 << 0))        ///< CAN Error warning flag [0] (in _CAN_ESR, page 6)
  #define _CAN_EPVF              ((uint8_t) (0x01 << 1))        ///< CAN Error passive flag [0] (in _CAN_ESR, page 6)
  #define _CAN_BOFF              ((uint8_t) (0x01 << 2))        ///< CAN Bus off flag [0] (in _CAN_ESR, page 6)
  // reserved [3]
  #define _CAN_LEC               ((uint8_t) (0x07 << 4))        ///< CAN Last error code [2:0] (in _CAN_ESR, page 6)
  #define _CAN_LEC0              ((uint8_t) (0x01 << 4))        ///< CAN Last error code [0] (in _CAN_ESR, page 6)
  #define _CAN_LEC1              ((uint8_t) (0x01 << 5))        ///< CAN Last error code [1] (in _CAN_ESR, page 6)
  #define _CAN_LEC2              ((uint8_t) (0x01 << 6))        ///< CAN Last error code [3] (in _CAN_ESR, page 6)
  // reserved [7]

  /* CAN error interrupt enable register (CAN_EIER, page 6) */
  #define _CAN_EWGIE             ((uint8_t) (0x01 << 0))        ///< CAN Error warning interrupt enable [0] (in _CAN_EIER, page 6)
  #define _CAN_EPVIE             ((uint8_t) (0x01 << 1))        ///< CAN Error passive  interrupt enable [0] (in _CAN_EIER, page 6)
  #define _CAN_BOFIE             ((uint8_t) (0x01 << 2))        ///< CAN Bus-Off  interrupt enable [0] (in _CAN_EIER, page 6)
  // reserved [3]
  #define _CAN_LECIE             ((uint8_t) (0x01 << 4))        ///< CAN Last error code interrupt enable [0] (in _CAN_EIER, page 6)
  // reserved [6:5]
  #define _CAN_ERRIE             ((uint8_t) (0x01 << 6))        ///< CAN Error interrupt enable [0] (in _CAN_EIER, page 6)
  // reserved [7]

  /* CAN bit timing register 1 (CAN_BTR1, page 6) */
  #define _CAN_BRP               ((uint8_t) (0x3F << 0))        ///< CAN Baud rate prescaler [5:0] (in _CAN_BTR1, page 6)
  #define _CAN_BRP0              ((uint8_t) (0x01 << 0))        ///< CAN Baud rate prescaler [0] (in _CAN_BTR1, page 6)
  #define _CAN_BRP1              ((uint8_t) (0x01 << 1))        ///< CAN Baud rate prescaler [1] (in _CAN_BTR1, page 6)
  #define _CAN_BRP2              ((uint8_t) (0x01 << 2))        ///< CAN Baud rate prescaler [2] (in _CAN_BTR1, page 6)
  #define _CAN_BRP3              ((uint8_t) (0x01 << 3))        ///< CAN Baud rate prescaler [3] (in _CAN_BTR1, page 6)
  #define _CAN_BRP4              ((uint8_t) (0x01 << 4))        ///< CAN Baud rate prescaler [4] (in _CAN_BTR1, page 6)
  #define _CAN_BRP5              ((uint8_t) (0x01 << 5))        ///< CAN Baud rate prescaler [5] (in _CAN_BTR1, page 6)
  #define _CAN_SJW               ((uint8_t) (0x03 << 6))        ///< CAN Resynchronization jump width [1:0] (in _CAN_EIER, page 6)
  #define _CAN_SJW0              ((uint8_t) (0x01 << 6))        ///< CAN Resynchronization jump width [0] (in _CAN_EIER, page 6)
  #define _CAN_SJW1              ((uint8_t) (0x01 << 7))        ///< CAN Resynchronization jump width [1] (in _CAN_EIER, page 6)

  /* CAN bit timing register 2 (CAN_BTR2, page 6) */
  #define _CAN_BS1               ((uint8_t) (0x0F << 0))        ///< CAN Bit segment 1 [3:0] (in _CAN_BTR2, page 6)
  #define _CAN_BS10              ((uint8_t) (0x01 << 0))        ///< CAN Bit segment 1 [0] (in _CAN_BTR2, page 6)
  #define _CAN_BS11              ((uint8_t) (0x01 << 1))        ///< CAN Bit segment 1 [1] (in _CAN_BTR2, page 6)
  #define _CAN_BS12              ((uint8_t) (0x01 << 2))        ///< CAN Bit segment 1 [2] (in _CAN_BTR2, page 6)
  #define _CAN_BS13              ((uint8_t) (0x01 << 3))        ///< CAN Bit segment 1 [3] (in _CAN_BTR2, page 6)
  #define _CAN_BS2               ((uint8_t) (0x07 << 4))        ///< CAN Bit segment 2 [2:0] (in _CAN_BTR2, page 6)
  #define _CAN_BS20              ((uint8_t) (0x01 << 4))        ///< CAN Bit segment 2 [0] (in _CAN_BTR2, page 6)
  #define _CAN_BS21              ((uint8_t) (0x01 << 5))        ///< CAN Bit segment 2 [1] (in _CAN_BTR2, page 6)
  #define _CAN_BS22              ((uint8_t) (0x01 << 6))        ///< CAN Bit segment 2 [2] (in _CAN_BTR2, page 6)
  // reserved [7]

  /* CAN filter mode register 1 (CAN_FMR1, page 6) */
  #define _CAN_FML0              ((uint8_t) (0x01 << 0))        ///< CAN Filter 0 mode low [0] (in _CAN_FMR1, page 6)
  #define _CAN_FMH0              ((uint8_t) (0x01 << 1))        ///< CAN Filter 0 mode high [0] (in _CAN_FMR1, page 6)
  #define _CAN_FML1              ((uint8_t) (0x01 << 2))        ///< CAN Filter 1 mode low [0] (in _CAN_FMR1, page 6)
  #define _CAN_FMH1              ((uint8_t) (0x01 << 3))        ///< CAN Filter 1 mode high [0] (in _CAN_FMR1, page 6)
  #define _CAN_FML2              ((uint8_t) (0x01 << 4))        ///< CAN Filter 2 mode low [0] (in _CAN_FMR1, page 6)
  #define _CAN_FMH2              ((uint8_t) (0x01 << 5))        ///< CAN Filter 2 mode high [0] (in _CAN_FMR1, page 6)
  #define _CAN_FML3              ((uint8_t) (0x01 << 6))        ///< CAN Filter 3 mode low [0] (in _CAN_FMR1, page 6)
  #define _CAN_FMH3              ((uint8_t) (0x01 << 7))        ///< CAN Filter 3 mode high [0] (in _CAN_FMR1, page 6)

  /* CAN filter mode register 2 (CAN_FMR2, page 6) */
  #define _CAN_FML4              ((uint8_t) (0x01 << 0))        ///< CAN Filter 4 mode low [0] (in _CAN_FMR2, page 6)
  #define _CAN_FMH4              ((uint8_t) (0x01 << 1))        ///< CAN Filter 4 mode high [0] (in _CAN_FMR2, page 6)
  #define _CAN_FML5              ((uint8_t) (0x01 << 2))        ///< CAN Filter 5 mode low [0] (in _CAN_FMR2, page 6)
  #define _CAN_FMH5              ((uint8_t) (0x01 << 3))        ///< CAN Filter 5 mode high [0] (in _CAN_FMR2, page 6)
  // reserved [7:4]

  /* CAN filter configuration register 1 (CAN_FCR1, page 6) */
  #define _CAN_FACT0             ((uint8_t) (0x01 << 0))        ///< CAN Filter 0 active [0] (in _CAN_FCR1, page 6)
  #define _CAN_FSC0              ((uint8_t) (0x03 << 1))        ///< CAN Filter 0 scale configuration [1:0] (in _CAN_FCR1, page 6)
  #define _CAN_FSC00             ((uint8_t) (0x01 << 1))        ///< CAN Filter 0 scale configuration [0] (in _CAN_FCR1, page 6)
  #define _CAN_FSC01             ((uint8_t) (0x01 << 2))        ///< CAN Filter 0 scale configuration [1] (in _CAN_FCR1, page 6)
  // reserved [3]
  #define _CAN_FACT1             ((uint8_t) (0x01 << 4))        ///< CAN Filter 1 active [0] (in _CAN_FCR1, page 6)
  #define _CAN_FSC1              ((uint8_t) (0x03 << 5))        ///< CAN Filter 1 scale configuration [1:0] (in _CAN_FCR1, page 6)
  #define _CAN_FSC10             ((uint8_t) (0x01 << 5))        ///< CAN Filter 1 scale configuration [0] (in _CAN_FCR1, page 6)
  #define _CAN_FSC11             ((uint8_t) (0x01 << 6))        ///< CAN Filter 1 scale configuration [1] (in _CAN_FCR1, page 6)
  // reserved [7]

  /* CAN filter configuration register 2 (CAN_FCR2, page 6) */
  #define _CAN_FACT2             ((uint8_t) (0x01 << 0))        ///< CAN Filter 2 active [0] (in _CAN_FCR2, page 6)
  #define _CAN_FSC2              ((uint8_t) (0x03 << 1))        ///< CAN Filter 2 scale configuration [1:0] (in _CAN_FCR2, page 6)
  #define _CAN_FSC20             ((uint8_t) (0x01 << 1))        ///< CAN Filter 2 scale configuration [0] (in _CAN_FCR2, page 6)
  #define _CAN_FSC21             ((uint8_t) (0x01 << 2))        ///< CAN Filter 2 scale configuration [1] (in _CAN_FCR2, page 6)
  // reserved [3]
  #define _CAN_FACT3             ((uint8_t) (0x01 << 4))        ///< CAN Filter 3 active [0] (in _CAN_FCR2, page 6)
  #define _CAN_FSC3              ((uint8_t) (0x03 << 5))        ///< CAN Filter 3 scale configuration [1:0] (in _CAN_FCR2, page 6)
  #define _CAN_FSC30             ((uint8_t) (0x01 << 5))        ///< CAN Filter 3 scale configuration [0] (in _CAN_FCR2, page 6)
  #define _CAN_FSC31             ((uint8_t) (0x01 << 6))        ///< CAN Filter 3 scale configuration [1] (in _CAN_FCR2, page 6)
  // reserved [7]

  /* CAN filter configuration register 3 (CAN_FCR3, page 6) */
  #define _CAN_FACT4             ((uint8_t) (0x01 << 0))        ///< CAN Filter 4 active [0] (in _CAN_FCR3, page 6)
  #define _CAN_FSC4              ((uint8_t) (0x03 << 1))        ///< CAN Filter 4 scale configuration [1:0] (in _CAN_FCR3, page 6)
  #define _CAN_FSC40             ((uint8_t) (0x01 << 1))        ///< CAN Filter 4 scale configuration [0] (in _CAN_FCR3, page 6)
  #define _CAN_FSC41             ((uint8_t) (0x01 << 2))        ///< CAN Filter 4 scale configuration [1] (in _CAN_FCR3, page 6)
  // reserved [3]
  #define _CAN_FACT5             ((uint8_t) (0x01 << 4))        ///< CAN Filter 5 active [0] (in _CAN_FCR2, page 6)
  #define _CAN_FSC5              ((uint8_t) (0x03 << 5))        ///< CAN Filter 5 scale configuration [1:0] (in _CAN_FCR3, page 6)
  #define _CAN_FSC50             ((uint8_t) (0x01 << 5))        ///< CAN Filter 5 scale configuration [0] (in _CAN_FCR3, page 6)
  #define _CAN_FSC51             ((uint8_t) (0x01 << 6))        ///< CAN Filter 5 scale configuration [1] (in _CAN_FCR3, page 6)
  // reserved [7]

#endif // CAN_AddressBase



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
      _BITS   res     : 6;    ///< Reserved
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
      _BITS   res       : 2;    ///< Reserved (TLI always highest prio)
      _BITS   VECT1SPR  : 2;    ///< interrupt priority vector 1
      _BITS   VECT2SPR  : 2;    ///< interrupt priority vector 2
      _BITS   VECT3SPR  : 2;    ///< interrupt priority vector 3
    } SPR1;


    /** @brief interrupt priority register 2 (ITC_SPR2) */
    struct {
      _BITS   VECT4SPR  : 2;    ///< interrupt priority vector 4
      _BITS   VECT5SPR  : 2;    ///< interrupt priority vector 5
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
      _BITS   VECT16SPR : 2;    ///< interrupt priority vector 16
      _BITS   VECT17SPR : 2;    ///< interrupt priority vector 17
      _BITS   VECT18SPR : 2;    ///< interrupt priority vector 18
      _BITS   VECT19SPR : 2;    ///< interrupt priority vector 19
    } SPR5;


    /** @brief interrupt priority register 6 (ITC_SPR6) */
    struct {
      _BITS   VECT20SPR : 2;    ///< interrupt priority vector 20
      _BITS   VECT21SPR : 2;    ///< interrupt priority vector 21
      _BITS   VECT22SPR : 2;    ///< interrupt priority vector 22
      _BITS   VECT23SPR : 2;    ///< interrupt priority vector 23
    } SPR6;


    /** @brief interrupt priority register 7 (ITC_SPR7) */
    struct {
      _BITS   VECT24SPR : 2;    ///< interrupt priority vector 24
      _BITS   VECT25SPR : 2;    ///< interrupt priority vector 25
      _BITS   VECT26SPR : 2;    ///< interrupt priority vector 26
      _BITS   VECT27SPR : 2;    ///< interrupt priority vector 27
    } SPR7;


    /** @brief interrupt priority register 8 (ITC_SPR8) */
    struct {
      _BITS   VECT28SPR : 2;    ///< interrupt priority vector 28
      _BITS   VECT29SPR : 2;    ///< interrupt priority vector 29
      _BITS   res       : 4;    ///< Reserved
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
  #define _ITC_VECT2SPR         ((uint8_t) (0x03 << 4))          ///< ITC interrupt priority vector 2 [1:0] (in _ITC_SPR1)
  #define _ITC_VECT2SPR0        ((uint8_t) (0x01 << 4))          ///< ITC interrupt priority vector 2 [0] (in _ITC_SPR1)
  #define _ITC_VECT2SPR1        ((uint8_t) (0x01 << 5))          ///< ITC interrupt priority vector 2 [1] (in _ITC_SPR1)
  #define _ITC_VECT3SPR         ((uint8_t) (0x03 << 6))          ///< ITC interrupt priority vector 3 [1:0] (in _ITC_SPR1)
  #define _ITC_VECT3SPR0        ((uint8_t) (0x01 << 6))          ///< ITC interrupt priority vector 3 [0] (in _ITC_SPR1)
  #define _ITC_VECT3SPR1        ((uint8_t) (0x01 << 7))          ///< ITC interrupt priority vector 3 [1] (in _ITC_SPR1)

  /* Software priority register 2 (ITC_SPR2) */
  #define _ITC_VECT4SPR         ((uint8_t) (0x03 << 0))          ///< ITC interrupt priority vector 4 [1:0] (in _ITC_SPR2)
  #define _ITC_VECT4SPR0        ((uint8_t) (0x01 << 0))          ///< ITC interrupt priority vector 4 [0] (in _ITC_SPR2)
  #define _ITC_VECT4SPR1        ((uint8_t) (0x01 << 1))          ///< ITC interrupt priority vector 4 [1] (in _ITC_SPR2)
  #define _ITC_VECT5SPR         ((uint8_t) (0x03 << 2))          ///< ITC interrupt priority vector 5 [1:0] (in _ITC_SPR2)
  #define _ITC_VECT5SPR0        ((uint8_t) (0x01 << 2))          ///< ITC interrupt priority vector 5 [0] (in _ITC_SPR2)
  #define _ITC_VECT5SPR1        ((uint8_t) (0x01 << 3))          ///< ITC interrupt priority vector 5 [1] (in _ITC_SPR2)
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
  #define _ITC_VECT16SPR        ((uint8_t) (0x03 << 0))          ///< ITC interrupt priority vector 16 [1:0] (in _ITC_SPR5)
  #define _ITC_VECT16SPR0       ((uint8_t) (0x01 << 0))          ///< ITC interrupt priority vector 16 [0] (in _ITC_SPR5)
  #define _ITC_VECT16SPR1       ((uint8_t) (0x01 << 1))          ///< ITC interrupt priority vector 16 [1] (in _ITC_SPR5)
  #define _ITC_VECT17SPR        ((uint8_t) (0x03 << 2))          ///< ITC interrupt priority vector 17 [1:0] (in _ITC_SPR5)
  #define _ITC_VECT17SPR0       ((uint8_t) (0x01 << 2))          ///< ITC interrupt priority vector 17 [0] (in _ITC_SPR5)
  #define _ITC_VECT17SPR1       ((uint8_t) (0x01 << 3))          ///< ITC interrupt priority vector 17 [1] (in _ITC_SPR5)
  #define _ITC_VECT18SPR        ((uint8_t) (0x03 << 4))          ///< ITC interrupt priority vector 18 [1:0] (in _ITC_SPR5)
  #define _ITC_VECT18SPR0       ((uint8_t) (0x01 << 4))          ///< ITC interrupt priority vector 18 [0] (in _ITC_SPR5)
  #define _ITC_VECT18SPR1       ((uint8_t) (0x01 << 5))          ///< ITC interrupt priority vector 18 [1] (in _ITC_SPR5)
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
  #define _ITC_VECT23SPR        ((uint8_t) (0x03 << 6))          ///< ITC interrupt priority vector 23 [1:0] (in _ITC_SPR6)
  #define _ITC_VECT23SPR0       ((uint8_t) (0x01 << 6))          ///< ITC interrupt priority vector 23 [0] (in _ITC_SPR6)
  #define _ITC_VECT23SPR1       ((uint8_t) (0x01 << 7))          ///< ITC interrupt priority vector 23 [1] (in _ITC_SPR6)

  /* Software priority register 7 (ITC_SPR7) */
  #define _ITC_VECT24SPR        ((uint8_t) (0x03 << 0))          ///< ITC interrupt priority vector 24 [1:0] (in _ITC_SPR7)
  #define _ITC_VECT24SPR0       ((uint8_t) (0x01 << 0))          ///< ITC interrupt priority vector 24 [0] (in _ITC_SPR7)
  #define _ITC_VECT24SPR1       ((uint8_t) (0x01 << 1))          ///< ITC interrupt priority vector 24 [1] (in _ITC_SPR7)
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
#endif // _STM8AF_STM8S_H_

/** @}*/
