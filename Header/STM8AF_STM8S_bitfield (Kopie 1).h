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
  #define NOP()                _asm("nop")                          ///< perform a nop() operation (=minimum delay)
  #define DISABLE_INTERRUPTS() _asm("sim")                          ///< disable interrupt handling
  #define ENABLE_INTERRUPTS()  _asm("rim")                          ///< enable interrupt handling
  #define TRIGGER_TRAP         _asm("trap")                         ///< trigger a trap (=soft interrupt) e.g. for EMC robustness (see AN1015)
  #define WAIT_FOR_INTERRUPT() _asm("wfi")                          ///< stop code execution and wait for interrupt
  #define ENTER_HALT()         _asm("halt")                         ///< put controller to HALT mode
  #define SW_RESET()           _WWDG.CR=0xBF                        ///< reset controller via WWGD module


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

#endif


/*-----------------------------------------------------------------------------
    DEFINITION OF GLOBAL MACROS/#DEFINES
-----------------------------------------------------------------------------*/

// general macros
#define SFR(type, addr)         (*((type*) (addr)))              ///< peripheral register


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
    #define 	INT8_MAX   0x7f
    #define 	INT8_MIN   (-INT8_MAX - 1)
  
    #define 	UINT8_MAX   0xFF
    #define 	UINT8_MIN   0
  
    #define 	INT16_MAX   0x7fff
    #define 	INT16_MIN   (-INT16_MAX - 1)
  
    #define 	UINT16_MAX  0xFFFF
    #define 	UINT16_MIN  0
  
    #define 	INT32_MAX   0x7fffffffL
    #define 	INT32_MIN   (-INT32_MAX - 1L)

    #define 	UINT32_MAX  0xFFFFFFFF
    #define 	UINT32_MIN  0

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
    DECLARATION OF GLOBAL TYPEDEFS
-----------------------------------------------------------------------------*/

/** union for bit- or bytewise r/w-access to 8-bit data (byte_t) */
typedef union {

  /// for byte access
  uint8_t  Byte;
  
  /// for bitwise access
  struct {
    int    b0 : 1;     ///< bit 0 in byte
    int    b1 : 1;     ///< bit 1 in byte
    int    b2 : 1;     ///< bit 2 in byte
    int    b3 : 1;     ///< bit 3 in byte
    int    b4 : 1;     ///< bit 4 in byte
    int    b5 : 1;     ///< bit 5 in byte
    int    b6 : 1;     ///< bit 6 in byte
    int    b7 : 1;     ///< bit 7 in byte
  } Bit;
  
} byte_t;


/** struct for bytewise r/w access to 16bit data (word_t)
 \note
   order of r/w access is important and cannot be guaranteed in C --> don't use union like for byte_t
     - write: HB+LB ok / LB+HB and word fails! \n
     - read: LB+HB and word ok / HB+LB fails! \n
*/
typedef struct {
  uint8_t byteH;        ///< high byte in 16b word
  uint8_t byteL;        ///< low byte in 16b word
} word_t;


/*-----------------------------------------------------------------------------
    DEFINITION OF STM8 PERIPHERAL REGISTERS
-----------------------------------------------------------------------------*/

//------------------------
// PORT ports (implemented on all devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** structure for controlling/monitoring pins in PORT mode (PORT_t) */
  typedef struct {
    byte_t ODR;     ///< Port x output data register (Px_ODR)
    byte_t IDR;     ///< Port x pin input register (Px_IDR)
    byte_t DDR;     ///< Port x data direction register (Px_DDR)
    byte_t CR1;     ///< Port x control register 1 (Px_CR1)
    byte_t CR2;     ///< Port x control register 2 (Px_CR2)
  } PORT_t;

  // port A..F implemented on all devices
  #if defined(PORTA_BaseAddress)
    #define _GPIOA  (*((PORT_t *) PORTA_BaseAddress))   ///< registers for port A access
  #endif
  #if defined(PORTB_BaseAddress)
    #define _GPIOB  (*((PORT_t *) PORTB_BaseAddress))   ///< registers for port B access
  #endif
  #if defined(PORTC_BaseAddress)
    #define _GPIOC  (*((PORT_t *) PORTC_BaseAddress))   ///< registers for port C access
  #endif
  #if defined(PORTD_BaseAddress)
    #define _GPIOD  (*((PORT_t *) PORTD_BaseAddress))   ///< registers for port D access
  #endif
  #if defined(PORTE_BaseAddress)
    #define _GPIOE  (*((PORT_t *) PORTE_BaseAddress))   ///< registers for port E access
  #endif
  #if defined(PORTF_BaseAddress)
    #define _GPIOF  (*((PORT_t *) PORTF_BaseAddress))   ///< registers for port F access
  #endif

  // port G+H+I implemented on selected devices
  #if defined(PORTG_BaseAddress)
    #define _GPIOG  (*((PORT_t *) PORTG_BaseAddress))   ///< registers for port G access
  #endif
  #if defined(PORTH_BaseAddress)
    #define _GPIOH  (*((PORT_t *) PORTH_BaseAddress))   ///< registers for port H access
  #endif
  #if defined(PORTI_BaseAddress)
    #define _GPIOI  (*((PORT_t *) PORTI_BaseAddress))   ///< registers for port I access
  #endif

  /* macro for readability (all ports) */
  #define Pin0   Bit.b0
  #define Pin1   Bit.b1
  #define Pin2   Bit.b2
  #define Pin3   Bit.b3
  #define Pin4   Bit.b4
  #define Pin5   Bit.b5
  #define Pin6   Bit.b6
  #define Pin7   Bit.b7

  /* PORT Module Reset Values (all ports) */
  #define PORT_ODR_RESET_VALUE ((uint8_t)0x00)
  #define PORT_DDR_RESET_VALUE ((uint8_t)0x00)
  #define PORT_CR1_RESET_VALUE ((uint8_t)0x00)
  #define PORT_CR2_RESET_VALUE ((uint8_t)0x00)

#endif // (1)



//------------------------
// Clock module CLK (implemented on all devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing clock module registers (CLK) */
  typedef struct {

    /** Internal clock register (CLK_ICKR) */
    union {
    
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    HSIEN   : 1;    ///< High speed internal RC oscillator enable
        int    HSIRDY  : 1;    ///< High speed internal oscillator ready flag
        int    FHW     : 1;    ///< Fast wakeup from Halt/Active-halt modes enable
        int    LSIEN   : 1;    ///< Low speed internal RC oscillator enable
        int    LSIRDY  : 1;    ///< Low speed internal oscillator ready flag
        int    REGAH   : 1;    ///< Regulator power off in Active-halt mode enable
        int    res     : 2;    ///< Reserved, must be kept cleared
      } Reg;
      
    } ICKR;


    /** External clock register (CLK_ECKR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    HSEEN   : 1;    ///< High speed external crystal oscillator enable
        int    HSERDY  : 1;    ///< High speed external crystal oscillator ready
        int    res     : 6;    ///< Reserved, must be kept cleared
      } Reg;
      
    } ECKR;


    /** Reserved register (1B) */
    uint8_t         res[1]; 


    /** Clock master status register (CLK_CMSR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CKM     : 8;    ///< Clock master status bits
      } Reg;
      
    } CMSR;


    /** Clock master switch register (CLK_SWR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    SWI     : 8;    ///< Clock master selection bits
      } Reg;
      
    } SWR;


    /** Switch control register (CLK_SWCR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    SWBSY   : 1;    ///< Switch busy flag
        int    SWEN    : 1;    ///< Switch start/stop enable
        int    SWIEN   : 1;    ///< Clock switch interrupt enable
        int    SWIF    : 1;    ///< Clock switch interrupt flag
        int    res     : 4;    ///< Reserved
      } Reg;

    } SWCR;


    /** Clock divider register (CLK_CKDIVR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CPUDIV  : 3;    ///< CPU clock prescaler
        int    HSIDIV  : 2;    ///< High speed internal clock prescaler
        int    res     : 3;    ///< Reserved, must be kept cleared.
      } Reg;

    } CKDIVR;


    /** Peripheral clock gating register 1 (CLK_PCKENR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PCKEN_I2C       : 1;    ///< clock enable I2C
        int    PCKEN_SPI       : 1;    ///< clock enable SPI
        int    PCKEN_UART1     : 1;    ///< clock enable UART1
        int    PCKEN_UART2     : 1;    ///< clock enable UART2
        int    PCKEN_TIM4_TIM6 : 1;    ///< clock enable TIM4/TIM6
        int    PCKEN_TIM2_TIM5 : 1;    ///< clock enable TIM4/TIM6
        int    PCKEN_TIM3      : 1;    ///< clock enable TIM3
        int    PCKEN_TIM1      : 1;    ///< clock enable TIM1
      } Reg;
      
    } PCKENR1;


    /** Clock security system register (CLK_CSSR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CSSEN   : 1;    ///< Clock security system enable
        int    AUX     : 1;    ///< Auxiliary oscillator connected to master clock
        int    CSSDIE  : 1;    ///< Clock security system detection interrupt enable
        int    CSSD    : 1;    ///< Clock security system detection
        int    res     : 4;    ///< Reserved, must be kept cleared.
      } Reg;
      
    } CSSR;


    /** Configurable clock output register (CLK_CCOR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CCOEN   : 1;    ///< Configurable clock output enable
        int    CCOSEL  : 4;    ///< Configurable clock output selection.
        int    CCORDY  : 1;    ///< Configurable clock output ready
        int    CCOBSY  : 1;    ///< Configurable clock output busy
        int    res     : 1;    ///< Reserved, must be kept cleared.
      } Reg;

    } CCOR;


    /** Peripheral clock gating register 2 (CLK_PCKENR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    res       : 2;   ///< Reserved
        int    PCKEN_AWU : 1;   ///< clock enable AWU
        int    PCKEN_ADC : 1;   ///< clock enable ADC
        int    res2      : 3;   ///< Reserved
        int    PCKEN_CAN : 1;   ///< clock enable CAN
      } Reg;

    } PCKENR2;


    /// Reserved register (1B). Was CAN clock control (obsolete as of STM8 UM rev 7)
    uint8_t         res2[1]; 
    /*
    union {
      uint8_t  Byte;
      struct {
        int    CANDIV  : 3;
        int    res     : 5;
      } Reg;
    } CANCCR;
    */
    

    /** HSI clock calibration trimming register (CLK_HSITRIMR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    HSITRIM : 4;    ///< HSI trimming value (some devices only support 3 bits, see DS!)
        int    res     : 4;    ///< Reserved, must be kept cleared.
      } Reg;
      
    } HSITRIMR;


    /** SWIM clock control register (CLK_SWIMCCR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    SWIMCLK : 1;    ///< SWIM clock divider
        int    res     : 7;    ///< Reserved.
      } Reg;
      
    } SWIMCCR;

  } CLK_t;

  /// pointer to all CLK registers (all devices)
  #if defined(CLK_BaseAddress)
    #define _CLK  (*((CLK_t *) CLK_BaseAddress))
  #endif
  
  /* CLK Module Reset Values */
  #define CLK_ICKR_RESET_VALUE     ((uint8_t)0x01)
  #define CLK_ECKR_RESET_VALUE     ((uint8_t)0x00)
  #define CLK_CMSR_RESET_VALUE     ((uint8_t)0xE1)
  #define CLK_SWR_RESET_VALUE      ((uint8_t)0xE1)
  #define CLK_SWCR_RESET_VALUE     ((uint8_t)0x00)
  #define CLK_CKDIVR_RESET_VALUE   ((uint8_t)0x18)
  #define CLK_PCKENR1_RESET_VALUE  ((uint8_t)0xFF)
  #define CLK_PCKENR2_RESET_VALUE  ((uint8_t)0xFF)
  #define CLK_CSSR_RESET_VALUE     ((uint8_t)0x00)
  #define CLK_CCOR_RESET_VALUE     ((uint8_t)0x00)
  #define CLK_HSITRIMR_RESET_VALUE ((uint8_t)0x00)
  #define CLK_SWIMCCR_RESET_VALUE  ((uint8_t)0x00)

#endif // (1)



//------------------------
// Window Watchdog WWDG (implemented on all devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing Window Watchdog registers (WWDG) */
  typedef struct {

    /** WWDG Control register (WWDG_CR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    T       : 7;    ///< 7-bit WWDG counter (MSB to LSB)
        int    WDGA    : 1;    ///< WWDG activation bit (not used if WWDG is enabled by option byte)
      } Reg;
      
    } CR;


    /** WWDR Window register (WWDG_WR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    W       : 7;    ///< 7-bit window value
        int    res     : 1;    ///< Reserved
      } Reg;
      
    } WR;

  } WWDG_t;

  /// pointer to all WWDG Window Watchdog registers (all devices)
  #if defined(WWDG_BaseAddress)
    #define _WWDG  (*((WWDG_t *) WWDG_BaseAddress))
  #endif

  /* WWDG Module Reset Values */
  #define WWDG_CR_RESET_VALUE ((uint8_t)0x7F)
  #define WWDG_WR_RESET_VALUE ((uint8_t)0x7F)

#endif // (1)



//------------------------
// Independent Timeout Watchdog IWDG (implemented on all devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing IWDG independent watchdog registers */
  typedef struct {

    /** IWDG Key register (IWDG_KR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    KEY     : 8;    ///< IWDG key value
      } Reg;
      
    } KR;


    /** IWDG Prescaler register (IWDG_PR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PR      : 3;    ///< Prescaler divider
        int    res     : 5;    ///< Reserved
      } Reg;
      
    } PR;


    /** IWDG Reload register (IWDG_RLR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    RL      : 8;    ///< Watchdog counter reload value
      } Reg;
      
    } RLR;

  } IWDG_t;

  /// pointer to all IWDG independent timeout watchdog registers (all devices)
  #if defined(IWDG_BaseAddress)
    #define _IWDG  (*((IWDG_t *) IWDG_BaseAddress))
  #endif

  /* IWDG Module Reset Values */
  #define IWDG_PR_RESET_VALUE  ((uint8_t)0x00)
  #define IWDG_RLR_RESET_VALUE ((uint8_t)0xFF)

#endif // (1)



//------------------------
// NVM Module FLASH (all devices, but differet sizes)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing FLASH memory registers */
  typedef struct {

    /** Flash control register 1 (FLASH_CR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    FIX     : 1;    ///< Fixed Byte programming time
        int    IE      : 1;    ///< Flash Interrupt enable
        int    AHALT   : 1;    ///< Power-down in Active-halt mode
        int    HALT    : 1;    ///< Power-down in Halt mode
        int    res     : 4;    ///< Reserved
      } Reg;
    } CR1;


    /** Flash control register 2 (FLASH_CR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PRG     : 1;    ///< Standard block programming
        int    res     : 3;    ///< Reserved
        int    FPRG    : 1;    ///< Fast block programming
        int    ERASE   : 1;    ///< Block erasing
        int    WPRG    : 1;    ///< Word programming
        int    OPT     : 1;    ///< Write option bytes
      } Reg;

    } CR2;


    /** complementary Flash control register 2 (FLASH_CR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PRG     : 1;    ///< Standard block programming
        int    res     : 3;    ///< Reserved
        int    FPRG    : 1;    ///< Fast block programming
        int    ERASE   : 1;    ///< Block erasing
        int    WPRG    : 1;    ///< Word programming
        int    OPT     : 1;    ///< Write option bytes
      } Reg;

    } NCR2;


    /** Flash protection register (FLASH_FPR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    WPB     : 6;    ///< User boot code area protection bits
        int    res     : 2;    ///< Reserved
      } Reg;

    } FPR;


    /** complementary Flash protection register (FLASH_FPR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    WPB     : 6;    ///< User boot code area protection bits
        int    res     : 2;    ///< Reserved
      } Reg;

    } NFPR;


    /** Flash status register (FLASH_IAPSR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    WR_PG_DIS : 1;  ///< Write attempted to protected page flag
        int    PUL       : 1;  ///< Flash Program memory unlocked flag
        int    EOP       : 1;  ///< End of programming (write or erase operation) flag
        int    DUL       : 1;  ///< Data EEPROM area unlocked flag
        int    res       : 2;  ///< Reserved, forced by hardware to 0
        int    HVOFF     : 1;  ///< End of high voltage flag
        int    res2      : 1;  ///< Reserved
      } Reg;

    } IAPSR;


    /** Reserved registers (2B) */
    uint8_t         res[2]; 


    /** Flash program memory unprotecting key register (FLASH_PUKR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PUK     : 8;    ///< Main program memory unlock keys
      } Reg;
      
    } PUKR;


    /** Reserved register (1B) */
    uint8_t         res2[1]; 


    /** Data EEPROM unprotection key register (FLASH_DUKR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    DUK     : 8;    ///< Data EEPROM write unlock keys
      } Reg;
      
    } DUKR;

  } FLASH_t;

  /// pointer to all Flash registers (all devices, but differet sizes)
  #if defined(FLASH_BaseAddress)
    #define _FLASH  (*((FLASH_t *) FLASH_BaseAddress))
  #endif

  /* FLASH Module Reset Values */
  #define FLASH_CR1_RESET_VALUE   ((uint8_t)0x00)
  #define FLASH_CR2_RESET_VALUE   ((uint8_t)0x00)
  #define FLASH_NCR2_RESET_VALUE  ((uint8_t)0xFF)
  #define FLASH_IAPSR_RESET_VALUE ((uint8_t)0x40)
  #define FLASH_PUKR_RESET_VALUE  ((uint8_t)0x00)
  #define FLASH_DUKR_RESET_VALUE  ((uint8_t)0x00)

#endif // (1)



//------------------------
// Auto Wake-Up Module AWU (implemented on all devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing AWU module registers */
  typedef struct {

    /** AWU Control/status register (AWU_CSR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    MSR     : 1;    ///< LSI measurement enable
        int    res     : 3;    ///< Reserved
        int    AWUEN   : 1;    ///< Auto-wakeup enable
        int    AWUF    : 1;    ///< Auto-wakeup flag
        int    res2    : 2;    ///< Reserved
      } Reg;

    } CSR;


    /** AWU Asynchronous prescaler register (AWU_APR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    APR     : 6;    ///< Asynchronous prescaler divider
        int    res     : 2;    ///< Reserved
      } Reg;
      
    } APR;


    /** AWU Timebase selection register (AWU_TBR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    AWUTB   : 4;    ///< Auto-wakeup timebase selection
        int    res     : 4;    ///< Reserved
      } Reg;

    } TBR;

  } AWU_t;

  /// pointer to all AWU registers (all devices)
  #if defined(AWU_BaseAddress)
    #define _AWU  (*((AWU_t *) AWU_BaseAddress))
  #endif

  /* AWU Module Reset Values */
  #define AWU_CSR_RESET_VALUE ((uint8_t)0x00)
  #define AWU_APR_RESET_VALUE ((uint8_t)0x3F)
  #define AWU_TBR_RESET_VALUE ((uint8_t)0x00)

#endif // (1)



//------------------------
// Beeper module BEEP (all devices, may require option byte change)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing BEEP registers */
  typedef struct {

    /** Beeper control/status register (BEEP_CSR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    BEEPDIV : 5;    ///< Beep clock prescaler divider
        int    BEEPEN  : 1;    ///< Beep enable
        int    BEEPSEL : 2;    ///< Beeper frequency selection
      } Reg;
      
    } CSR;

  } BEEP_t;

  /// register for beeper control (all devices)
  #if defined(BEEP_BaseAddress)
    #define _BEEP  (*((BEEP_t *) BEEP_BaseAddress))
  #endif

  /* BEEP Module Reset Values */
  #define BEEP_CSR_RESET_VALUE ((uint8_t)0x1F)

#endif // (1)



//------------------------
// Reset Module RST (all devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing RST registers */
  typedef struct {

    /** Reset status register (RST_SR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    WWDGF   : 1;    ///< Window Watchdog reset flag
        int    IWDGF   : 1;    ///< Independent Watchdog reset flag
        int    ILLOPF  : 1;    ///< Illegal opcode reset flag
        int    SWIMF   : 1;    ///< SWIM reset flag
        int    EMCF    : 1;    ///< EMC reset flag
        int    res     : 3;    ///< Reserved
      } Reg;

    } SR;

  } RST_t;

  /// register for reset status module (all devices)
  #if defined(RST_BaseAddress)
    #define _RST  (*((RST_t *) RST_BaseAddress))
  #endif

#endif // (1)



//------------------------
// Global Configuration CFG (all devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing CFG registers */
  typedef struct {

    /** Global configuration register (CFG_GCR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    SWD     : 1;    ///< SWIM disable
        int    AL      : 1;    ///< Activation level
        int    res     : 6;    ///< Reserved
      } Reg;
      
    } GCR;

  } CFG_t;

  /// register for CFG module (all devices)
  #if defined(CFG_BaseAddress)
    #define _CFG  (*((CFG_t *) CFG_BaseAddress))
  #endif

  /* CFG Module Reset Values */
  #define CFG_GCR_RESET_VALUE ((uint8_t)0x00)

#endif // (1)



//------------------------
// External Interrupt Control EXTI (all devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing EXTI registers */
  typedef struct {

    /** External interrupt control register 1 (EXTI_CR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PAIS    : 2;    ///< Port A external interrupt sensitivity bits
        int    PBIS    : 2;    ///< Port B external interrupt sensitivity bits
        int    PCIS    : 2;    ///< Port C external interrupt sensitivity bits
        int    PDIS    : 2;    ///< Port D external interrupt sensitivity bits
      } Reg;

    } CR1;


    /** External interrupt control register 2 (EXTI_CR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PEIS    : 2;    ///< Port E external interrupt sensitivity bits
        int    TLIS    : 1;    ///< Top level interrupt sensitivity
        int    res     : 5;    ///< Reserved
      } Reg;

    } CR2;

  } EXTI_t;

  /// pointer to all EXTI registers (all devices)
  #if defined(EXTI_BaseAddress)
    #define _EXTI  (*((EXTI_t *) EXTI_BaseAddress))
  #endif

  /* EXTI Module Reset Values */
  #define EXTI_CR1_RESET_VALUE ((uint8_t)0x00)
  #define EXTI_CR2_RESET_VALUE ((uint8_t)0x00)

#endif // (1)



//------------------------
// Interrupt Priority Module ITC (all devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing ITC registers */
  typedef struct {

    /** Software priority register 1/8 (ITC_SPR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    res       : 2;    ///< Reserved
        int    AWU       : 2;    ///< AWU (=irq1) interrupt priority 
        int    CLK       : 2;    ///< CLK (=irq2) interrupt priority 
        int    EXTI_A    : 2;    ///< EXINT on port A (=irq3) interrupt priority
      } Reg;

    } SPR1;


    /** Software priority register 2/8 (ITC_SPR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    EXTI_B    : 2;    ///< EXINT on port B (=irq4) interrupt priority
        int    EXTI_C    : 2;    ///< EXINT on port C (=irq5) interrupt priority
        int    EXTI_D    : 2;    ///< EXINT on port D (=irq6) interrupt priority
        int    EXTI_E    : 2;    ///< EXINT on port E (=irq7) interrupt priority
      } Reg;

    } SPR2;


    /** Software priority register 3/8 (ITC_SPR3) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CAN_RX    : 2;    ///< CAN Rx (=irq8) interrupt priority 
        int    CAN_TX    : 2;    ///< CAN Tx (=irq9) interrupt priority 
        int    SPI       : 2;    ///< SPI (=irq10) interrupt priority 
        int    TIM1_UPD  : 2;    ///< TIM1 Update (=irq11) interrupt priority 
      } Reg;

    } SPR3;


    /** Software priority register 4/8 (ITC_SPR4) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    TIM1_CAP  : 2;    ///< TIM1 Capture/Compare (=irq12) interrupt priority 
        int    TIM2_UPD  : 2;    ///< TIM2 Update (=irq13) interrupt priority 
        int    TIM2_CAP  : 2;    ///< TIM2 Capture/Compare (=irq14) interrupt priority 
        int    TIM3_UPD  : 2;    ///< TIM3 Update (=irq15) interrupt priority (Reserved)
      } Reg;

    } SPR4;


    /** Software priority register 5/8 (ITC_SPR5) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    TIM3_CAP  : 2;    ///< TIM3 Capture/Compare (=irq16) interrupt priority (Reserved)
        int    UART1_TX  : 2;    ///< UART1/USART Tx complete (=irq17) interrupt priority 
        int    UART1_RX  : 2;    ///< UART1/USART Rx full (=irq18) interrupt priority 
        int    I2C       : 2;    ///< I2C (=irq19) interrupt priority 
      } Reg;

    } SPR5;


    /** Software priority register 6/8 (ITC_SPR6) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UART234_TX : 2;    ///< UART234/LINUART Tx complete (=irq20) interrupt priority 
        int    UART234_RX : 2;    ///< UART234/LINUART Rx full (=irq21) interrupt priority 
        int    ADC        : 2;    ///< ADC (=irq22) interrupt priority 
        int    TIM4_UPD   : 2;    ///< TIM4 Update (=irq23) interrupt priority (Reserved)
      } Reg;

    } SPR6;


    /** Software priority register 7/8 (ITC_SPR7) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    FLASH     : 2;    ///< flash (=irq24) interrupt priority 
        int    VECT25    : 2;    ///< irq25 interrupt priority (not documented)
        int    VECT26    : 2;    ///< irq26 interrupt priority (not documented)
        int    VECT27    : 2;    ///< irq27 interrupt priority (not documented)
      } Reg;

    } SPR7;


    /** Software priority register 8/8 (ITC_SPR8) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    VECT28    : 2;    ///< irq28 interrupt priority (not documented)
        int    res       : 6;    ///< Reserved
      } Reg;

    } SPR8;

  } ITC_t;

  /// register for ITC control (all devices)
  #if defined(ITC_BaseAddress)
    #define _ITC  (*((ITC_t *) ITC_BaseAddress))
  #endif

  /* ITC Module Reset Values (all registers) */
  #define ITC_SPRX_RESET_VALUE ((uint8_t)0xFF)

#endif // (1)



//------------------------
// Serial Peripheral Interface SPI (all devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing SPI registers */
  typedef struct {

    /** SPI control register 1 (SPI_CR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CPHA     : 1;    ///< Clock phase
        int    CPOL     : 1;    ///< Clock polarity
        int    MSTR     : 1;    ///< Master/slave selection
        int    BR       : 3;    ///< Baudrate control
        int    SPE      : 1;    ///< SPI enable
        int    xLSBFIRST : 1;    ///< Frame format. Add x to avoid conflict with misc.h
      } Reg;

    } CR1;


    /** SPI control register 2 (SPI_CR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    SSI     : 1;    ///< Internal slave select
        int    SSM     : 1;    ///< Software slave management
        int    RXONLY  : 1;    ///< Receive only
        int    res     : 1;    ///< Reserved
        int    CRCNEXT : 1;    ///< Transmit CRC next
        int    CRCEN   : 1;    ///< Hardware CRC calculation enable
        int    BDOE    : 1;    ///< Input/Output enable in bidirectional mode
        int    BDM     : 1;    ///< Bidirectional data mode enable
      } Reg;

    } CR2;


    /** SPI interrupt control register (SPI_ICR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    res     : 4;    ///< Reserved
        int    WKIE    : 1;    ///< Wakeup interrupt enable
        int    ERRIE   : 1;    ///< Error interrupt enable
        int    RXIE    : 1;    ///< Rx buffer not empty interrupt enable
        int    TXIE    : 1;    ///< Tx buffer empty interrupt enable
      } Reg;

    } ICR;


    /** SPI status register (SPI_SR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    RXNE    : 1;    ///< Receive buffer not empty
        int    TXE     : 1;    ///< Transmit buffer empty
        int    res     : 1;    ///< Reserved
        int    WKUP    : 1;    ///< Wakeup flag
        int    CRCERR  : 1;    ///< CRC error flag
        int    MODF    : 1;    ///< Mode fault
        int    OVR     : 1;    ///< Overrun flag
        int    BSY     : 1;    ///< Busy flag
      } Reg;

    } SR;


    /** SPI data register (SPI_DR) */
    byte_t      DR; 
    
    
    /** SPI CRC polynomial register (SPI_CRCPR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CRCPOLY : 8;    ///< CRC polynomial register
      } Reg;
      
    } CRCPR;


    /** SPI Rx CRC register (SPI_RXCRCR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    RxCRC   : 8;    ///< Rx CRC Register
      } Reg;
      
    } RXCRCR;


    /** SPI Tx CRC register (SPI_TXCRCR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    TxCRC   : 8;    ///< Tx CRC register
      } Reg;
      
    } TXCRCR;

  } SPI_t;

  /// register for SPI control
  #if defined(SPI_BaseAddress)
    #define _SPI  (*((SPI_t *) SPI_BaseAddress))
  #endif

  /* SPI Module Reset Values */
  #define SPI_CR1_RESET_VALUE    ((uint8_t)0x00) /*!< Control Register 1 reset value */
  #define SPI_CR2_RESET_VALUE    ((uint8_t)0x00) /*!< Control Register 2 reset value */
  #define SPI_ICR_RESET_VALUE    ((uint8_t)0x00) /*!< Interrupt Control Register reset value */
  #define SPI_SR_RESET_VALUE     ((uint8_t)0x02) /*!< Status Register reset value */
  #define SPI_DR_RESET_VALUE     ((uint8_t)0x00) /*!< Data Register reset value */
  #define SPI_CRCPR_RESET_VALUE  ((uint8_t)0x07) /*!< Polynomial Register reset value */
  #define SPI_RXCRCR_RESET_VALUE ((uint8_t)0x00) /*!< RX CRC Register reset value */
  #define SPI_TXCRCR_RESET_VALUE ((uint8_t)0x00) /*!< TX CRC Register reset value */

#endif // (1)



//------------------------
// I2C Bus Interface (all devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing I2C registers */
  typedef struct {

    /** I2C Control register 1 (I2C_CR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PE        : 1;    ///< Peripheral enable
        int    res       : 5;    ///< Reserved
        int    ENGC      : 1;    ///< General call enable
        int    NOSTRETCH : 1;    ///< Clock stretching disable (Slave mode)
      } Reg;

    } CR1;


    /** I2C Control register 2 (I2C_CR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    START     : 1;    ///< Start generation
        int    STOP      : 1;    ///< Stop generation
        int    ACK       : 1;    ///< Acknowledge enable
        int    POS       : 1;    ///< Acknowledge position (for data reception)
        int    res       : 3;    ///< Reserved
        int    SWRST     : 1;    ///< Software reset
      } Reg;

    } CR2;


    /** I2C Frequency register (I2C_FREQR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    FREQ      : 6;    ///< Peripheral clock frequency
        int    res       : 2;    ///< Reserved
      } Reg;

    } FREQR;


    /** I2C Own address register LSB (I2C_OARL) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    ADD0      : 1;    ///< Interface address [10] (in 10-bit address mode)
        int    ADD       : 7;    ///< Interface address [7:1]
      } Reg;

    } OARL;


    /** I2C own address high (I2C_OARH) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    res       : 1;    ///< Reserved
        int    ADD       : 2;    ///< Interface address [9:8] (in 10-bit address mode)
        int    res2      : 3;    ///< Reserved
        int    ADDCONF   : 1;    ///< Address mode configuration (must always be written as ‘1’)
        int    ADDMODE   : 1;    ///< 7-/10-bit addressing mode (Slave mode)
      } Reg;
      
    } OARH;


    /** Reserved register (1B) */
    uint8_t         res[1]; 


    /** I2C data register (I2C_DR) */
    byte_t          DR; 


    /** I2C Status register 1 (I2C_SR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    SB        : 1;    ///< Start bit (Mastermode)
        int    ADDR      : 1;    ///< Address sent (master mode) / matched (slave mode)
        int    BTF       : 1;    ///< Byte transfer finished
        int    ADD10     : 1;    ///< 10-bit header sent (Master mode)
        int    STOPF     : 1;    ///< Stop detection (Slave mode)
        int    res       : 1;    ///< Reserved
        int    RXNE      : 1;    ///< Data register not empty (receivers)
        int    TXE       : 1;    ///< Data register empty (transmitters)
      } Reg;
      
    } SR1;


    /** I2C Status register 2 (I2C_SR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    BERR      : 1;    ///< Bus error
        int    ARLO      : 1;    ///< Arbitration lost (master mode)
        int    AF        : 1;    ///< Acknowledge failure
        int    OVR       : 1;    ///< Overrun/underrun
        int    res       : 1;    ///< Reserved
        int    WUFH      : 1;    ///< Wakeup from Halt
        int    res2      : 2;    ///< Reserved
      } Reg;
      
    } SR2;


    /** I2C Status register 3 (I2C_SR3) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    MSL       : 1;    ///< Master/Slave
        int    BUSY      : 1;    ///< Bus busy
        int    TRA       : 1;    ///< Transmitter/Receiver
        int    res       : 1;    ///< Reserved
        int    GENCALL   : 1;    ///< General call header (Slavemode)
        int    res2      : 3;    ///< Reserved
      } Reg;
      
    } SR3;


    /** I2C Interrupt register (I2C_ITR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    ITERREN   : 1;    ///< Error interrupt enable
        int    ITEVTEN   : 1;    ///< Event interrupt enable
        int    ITBUFEN   : 1;    ///< Buffer interrupt enable
        int    res       : 5;    ///< Reserved
      } Reg;
      
    } ITR;


    /** I2C Clock control register low (I2C_CCRL) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CCR       : 8;    ///< Clock control register (Master mode)
      } Reg;
      
    } CCRL;


    /** I2C Clock control register high (I2C_CCRH) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CCR       : 4;    ///< Clock control register in Fast/Standard mode (Master mode)
        int    res       : 2;    ///< Reserved
        int    DUTY      : 1;    ///< Fast mode duty cycle
        int    FS        : 1;    ///< I2C master mode selection
      } Reg;
      
    } CCRH;


    /** I2C TRISE register (I2C_TRISER) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    TRISE     : 6;    ///< Maximum rise time in Fast/Standard mode (Master mode)
        int    res       : 2;    ///< Reserved
      } Reg;
      
    } TRISER;


    /** Reserved register (1B). Was I2C packet error checking (undocumented in STM8 UM rev 9) */
    uint8_t         res2[1]; 
    /*
    union {
      uint8_t  Byte;
      struct {
        int    res       : 8;
      } Reg;
    } PECR;
    */

  } I2C_t;

  /// pointer to all I2C registers (all devices)
  #if defined(I2C_BaseAddress)
    #define _I2C  (*((I2C_t *) I2C_BaseAddress))
  #endif

  /* I2C Module Reset Values */
  #define I2C_CR1_RESET_VALUE    ((uint8_t)0x00)
  #define I2C_CR2_RESET_VALUE    ((uint8_t)0x00)
  #define I2C_FREQR_RESET_VALUE  ((uint8_t)0x00)
  #define I2C_OARL_RESET_VALUE   ((uint8_t)0x00)
  #define I2C_OARH_RESET_VALUE   ((uint8_t)0x00)
  #define I2C_DR_RESET_VALUE     ((uint8_t)0x00)
  #define I2C_SR1_RESET_VALUE    ((uint8_t)0x00)
  #define I2C_SR2_RESET_VALUE    ((uint8_t)0x00)
  #define I2C_SR3_RESET_VALUE    ((uint8_t)0x00)
  #define I2C_ITR_RESET_VALUE    ((uint8_t)0x00)
  #define I2C_CCRL_RESET_VALUE   ((uint8_t)0x00)
  #define I2C_CCRH_RESET_VALUE   ((uint8_t)0x00)
  #define I2C_TRISER_RESET_VALUE ((uint8_t)0x02)

#endif // (1)


//------------------------
// Controller Area Network CAN Module (selected devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing CAN registers (selected devices) */
  typedef struct {

    /** CAN master control register (CAN.MCR) */
    union {
  
      /// bytewise access to register
      uint8_t  Byte;
  
      /// bitwise access to register
      struct {
        int    INRQ      : 1;    ///< Initialization Request
        int    SLEEP     : 1;    ///< Sleep Mode Request
        int    TXFP      : 1;    ///< Transmit FIFO Priority
        int    RFLM      : 1;    ///< Receive FIFO Locked Mode
        int    NART      : 1;    ///< No Automatic Retransmission
        int    AWUM      : 1;    ///< Automatic Wakeup Mode
        int    ABOM      : 1;    ///< Automatic Bus-Off Management
        int    TTCM      : 1;    ///< Time Triggered Communication Mode
      } Reg;
  
    } MCR;


    /** CAN master status register (CAN.MSR) */
    union {
  
      /// bytewise access to register
      uint8_t  Byte;
  
      /// bitwise access to register
      struct {
        int    INAK      : 1;    ///< Initialization Acknowledge
        int    SLAK      : 1;    ///< Sleep Acknowledge
        int    ERRI      : 1;    ///< Error Interrupt
        int    WKUI      : 1;    ///< Wakeup Interrupt
        int    TX        : 1;    ///< Transmit
        int    RX        : 1;    ///< Receive
        int    res       : 2;    ///< Reserved
      } Reg;
  
    } MSR;


    /** CAN transmit status register (CAN.TSR) */
    union {
  
      /// bytewise access to register
      uint8_t  Byte;
  
      /// bitwise access to register
      struct {
        int    RQCP0     : 1;    ///< Request Completed for Mailbox 0 
        int    RQCP1     : 1;    ///< Request Completed for Mailbox 1
        int    RQCP2     : 1;    ///< Request Completed for Mailbox 2
        int    res       : 1;    ///< Reserved
        int    TXOK0     : 1;    ///< Transmission ok for Mailbox 0
        int    TXOK1     : 1;    ///< Transmission ok for Mailbox 1
        int    TXOK2     : 1;    ///< Transmission ok for Mailbox 2
        int    res2      : 1;    ///< Reserved
      } Reg;
  
    } TSR;


    /** CAN transmit priority register (CAN.TPR) */
    union {
  
      /// bytewise access to register
      uint8_t  Byte;
  
      /// bitwise access to register
      struct {
        int    CODE      : 2;    ///< Mailbox Code
        int    TME0      : 1;    ///< Transmit Mailbox 0 Empty
        int    TME1      : 1;    ///< Transmit Mailbox 1 Empty
        int    TME2      : 1;    ///< Transmit Mailbox 2 Empty
        int    LOW0      : 1;    ///< Lowest Priority Flag for Mailbox 0
        int    LOW1      : 1;    ///< Lowest Priority Flag for Mailbox 1
        int    LOW2      : 1;    ///< Lowest Priority Flag for Mailbox 2
      } Reg;
  
    } TPR;


    /** CAN receive FIFO register (CAN.RFR) */
    union {
  
      /// bytewise access to register
      uint8_t  Byte;
  
      /// bitwise access to register
      struct {
        int    FMP       : 2;    ///< FIFO Message Pending
        int    res       : 1;    ///< Reserved
        int    FULL      : 1;    ///< FIFO Full
        int    FOVR      : 1;    ///< FIFO Overrun
        int    RFOM      : 1;    ///< Release FIFO Output Mailbox
        int    res2      : 2;    ///< Reserved    
      } Reg;

    } RFR;


    /** CAN interrupt enable register (CAN.IER) */
    union {
  
      /// bytewise access to register
      uint8_t  Byte;
  
      /// bitwise access to register
      struct {
        int    TMEIE     : 1;    ///< Transmit Mailbox Empty Interrupt Enable
        int    FMPIE     : 1;    ///< FIFO Message Pending Interrupt Enable
        int    FFIE      : 1;    ///< FIFO Full Interrupt Enable
        int    FOVIE     : 1;    ///< FIFO Overrun Interrupt Enable
        int    res       : 3;    ///< Reserved
        int    WKUIE     : 1;    ///< Wakeup Interrupt Enable
      } Reg;

    } IER;


    /** CAN diagnosis register (CAN.DGR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    LBKM      : 1;    ///< Loop back mode
        int    SILM      : 1;    ///< Silent mode
        int    SAMP      : 1;    ///< Last sample point
        int    RX        : 1;    ///< CAN Rx Signal
        int    TXM2E     : 1;    ///< TX Mailbox 2 enable
        int    res       : 3;    ///< Reserved
      } Reg;
      
    } DGR;


    /** CAN page selection register for below paged registers (CAN.PSR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PS        : 3;    ///< Page select
        int    res       : 5;    ///< Reserved
      } Reg;
      
    } PSR;



    /** paged CAN registers (selection via CAN.PSR) */
    union {
    
      /** CAN page 0,1,5: Tx Mailbox 0,1,2 (CAN.Page.TxMailbox) */
      struct {

        /** CAN message control/status register (CAN.Page.TxMailbox.MCSR) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    TXRQ      : 1;    ///< Transmission mailbox request
            int    ABRQ      : 1;    ///< Abort request for mailbox
            int    RQCP      : 1;    ///< Request completed
            int    TXOK      : 1;    ///< Transmission OK
            int    ALST      : 1;    ///< Arbitration lost
            int    TERR      : 1;    ///< Transmission error
            int    res       : 2;    ///< Reserved
          } Reg;
          
        } MCSR;


        /** CAN mailbox data length control register (CAN.Page.TxMailbox.MDLCR) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    DLC       : 4;    ///< Data length code
            int    res       : 3;    ///< Reserved
            int    TGT       : 1;    ///< Transmit global time
          } Reg;
          
        } MDLCR;


        /** CAN mailbox identifier register 1 (CAN.Page.TxMailbox.MIDR1)*/
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    ID        : 5;    ///< STID[10:6] or EXID[28:24]
            int    RTR       : 1;    ///< Remote transmission request
            int    IDE       : 1;    ///< Extended identifier
            int    res       : 1;    ///< Reserved
          } Reg;
          
        } MIDR1;


        /** CAN mailbox identifier register 2 (CAN.Page.TxMailbox.MIDR2) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    EXID      : 2;    ///< EXID[17:16]
            int    ID        : 6;    ///< STID[5:0] or EXID[23:18]
          } Reg;
        } MIDR2;


        /** CAN mailbox identifier register 3 (CAN.Page.TxMailbox.MIDR3) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    EXID      : 8;    ///< EXID[15:8]
          } Reg;
          
        } MIDR3;


        /** CAN mailbox identifier register 4 (CAN.Page.TxMailbox.MIDR4) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    EXID      : 8;    ///< EXID[7:0]
          } Reg;
          
        } MIDR4;


        /** CAN mailbox data register 1 (CAN.Page.TxMailbox.MDAR1) */
        byte_t    MDAR1;


        /** CAN mailbox data register 2 (CAN.Page.TxMailbox.MDAR1) */
        byte_t    MDAR2;


        /** CAN mailbox data register 3 (CAN.Page.TxMailbox.MDAR1) */
        byte_t    MDAR3;


        /** CAN mailbox data register 4 (CAN.Page.TxMailbox.MDAR1) */
        byte_t    MDAR4;


        /** CAN mailbox data register 5 (CAN.Page.TxMailbox.MDAR1) */
        byte_t    MDAR5;


        /** CAN mailbox data register 6 (CAN.Page.TxMailbox.MDAR1) */
        byte_t    MDAR6;


        /** CAN mailbox data register 7 (CAN.Page.TxMailbox.MDAR1) */
        byte_t    MDAR7;


        /** CAN mailbox data register 8 (CAN.Page.TxMailbox.MDAR1) */
        byte_t    MDAR8;
        
        
        /** CAN mailbox time stamp register (CAN.Page.TxMailbox.MTSR) */
        word_t    MTSR;

      } TxMailbox;



      /** CAN page 2: Acceptance Filter 0:1 (CAN.Page.Filter01) */
      struct {

        // filter 0
        byte_t    FR01;
        byte_t    FR02;
        byte_t    FR03;
        byte_t    FR04;
        byte_t    FR05;
        byte_t    FR06;
        byte_t    FR07;
        byte_t    FR08;
      
        // filter 1
        byte_t    FR11;
        byte_t    FR12;
        byte_t    FR13;
        byte_t    FR14;
        byte_t    FR15;
        byte_t    FR16;
        byte_t    FR17;
        byte_t    FR18;

      } Filter01;



      /** CAN page 3: Acceptance Filter 2:3 (CAN.Page.Filter23) */
      struct {

        // filter 2
        byte_t    FR21;
        byte_t    FR22;
        byte_t    FR23;
        byte_t    FR24;
        byte_t    FR25;
        byte_t    FR26;
        byte_t    FR27;
        byte_t    FR28;
      
        // filter 3
        byte_t    FR31;
        byte_t    FR32;
        byte_t    FR33;
        byte_t    FR34;
        byte_t    FR35;
        byte_t    FR36;
        byte_t    FR37;
        byte_t    FR38;

      } Filter23;



      /** CAN page 4: Acceptance Filter 4:5 (CAN.Page.Filter45) */
      struct {

        // filter 4
        byte_t    FR41;
        byte_t    FR42;
        byte_t    FR43;
        byte_t    FR44;
        byte_t    FR45;
        byte_t    FR46;
        byte_t    FR47;
        byte_t    FR48;
      
        // filter 5
        byte_t    FR51;
        byte_t    FR52;
        byte_t    FR53;
        byte_t    FR54;
        byte_t    FR55;
        byte_t    FR56;
        byte_t    FR57;
        byte_t    FR58;

      } Filter45;



      /** CAN page 6: Configuration/Diagnostics (CAN.Page.Config) */
      struct {

        /** CAN error status register (CAN.Page.Config.ESR) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    EWGF      : 1;    ///< Error warning flag
            int    EPVF      : 1;    ///< Error passive flag
            int    BOFF      : 1;    ///< Bus off flag
            int    res       : 1;    ///< Reserved
            int    LEC       : 3;    ///< Last error code
            int    res2      : 1;    ///< Reserved
          } Reg;
          
        } ESR;


        /** CAN error interrupt enable register (CAN.Page.Config.EIER) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    EWGIE     : 1;    ///< Error warning interrupt enable
            int    EPVIE     : 1;    ///< Error passive  interrupt enable
            int    BOFIE     : 1;    ///< Bus-Off  interrupt enable
            int    res       : 1;    ///< Reserved
            int    LECIE     : 1;    ///< Last error code interrupt enable
            int    res2      : 2;    ///< Reserved
            int    ERRIE     : 1;    ///< Error interrupt enable
          } Reg;
          
        } EIER;


        /** CAN transmit error counter register (CAN.Page.Config.TECR) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    TEC       : 8;    ///< Transmit error counter
          } Reg;
          
        } TECR;


        /** CAN receive error counter register (CAN.Page.Config.RECR) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    REC       : 8;    ///< Receive error counter
          } Reg;
          
        } RECR;


        /** CAN bit timing register 1 (CAN.Page.Config.BTR1) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    BRP       : 6;    ///< Baud rate prescaler
            int    SJW       : 2;    ///< Resynchronization jump width
          } Reg;
          
        } BTR1;


        /** CAN bit timing register 2 (CAN.Page.Config.BTR2) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    BS1       : 4;    ///< Bit segment 1
            int    BS2       : 3;    ///< Bit segment 2
            int    res       : 1;    ///< Reserved, must be kept cleared
          } Reg;
          
        } BTR2;


        /** Reserved registers (2B) */
        uint8_t       res[2];
        
        
        /** CAN filter mode register 1 (CAN.Page.Config.FMR1) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    FML0      : 1;    ///< Filter 0 mode low
            int    FMH0      : 1;    ///< Filter 0 mode high
            int    FML1      : 1;    ///< Filter 1 mode low
            int    FMH1      : 1;    ///< Filter 1 mode high
            int    FML2      : 1;    ///< Filter 2 mode low
            int    FMH2      : 1;    ///< Filter 2 mode high
            int    FML3      : 1;    ///< Filter 3 mode low
            int    FMH3      : 1;    ///< Filter 3 mode high
          } Reg;
          
        } FMR1;


        /** CAN filter mode register 2 (CAN.Page.Config.FMR2) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    FML4      : 1;    ///< Filter 4 mode low
            int    FMH4      : 1;    ///< Filter 4 mode high
            int    FML5      : 1;    ///< Filter 5 mode low
            int    FMH5      : 1;    ///< Filter 5 mode high
            int    res       : 4;    ///< Reserved
          } Reg;
          
        } FMR2;


        /** CAN filter configuration register 1 (CAN.Page.Config.FCR1) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    FACT0     : 1;    ///< Filter 0 active
            int    FSC0      : 2;    ///< Filter 0 scale configuration
            int    res       : 1;    ///< Reserved
            int    FACT1     : 1;    ///< Filter 1 active
            int    FSC1      : 2;    ///< Filter 1 scale configuration
            int    res2      : 1;    ///< Reserved
          } Reg;
          
        } FCR1;


        /** CAN filter configuration register 2 (CAN.Page.Config.FCR2) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    FACT2     : 1;    ///< Filter 2 active
            int    FSC2      : 2;    ///< Filter 2 scale configuration
            int    res       : 1;    ///< Reserved
            int    FACT3     : 1;    ///< Filter 3 active
            int    FSC3      : 2;    ///< Filter 3 scale configuration
            int    res2      : 1;    ///< Reserve
          } Reg;
          
        } FCR2;




        /** CAN filter configuration register 3 (CAN.Page.Config.FCR3) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    FACT4     : 1;    ///< Filter 4 active
            int    FSC4      : 2;    ///< Filter 4 scale configuration
            int    res       : 1;    ///< Reserved
            int    FACT5     : 1;    ///< Filter 5 active
            int    FSC5      : 2;    ///< Filter 5 scale configuration
            int    res2      : 1;    ///< Reserve
          } Reg;
          
        } FCR3;


        /** Reserved registers (3B) */
        uint8_t       res2[3];

      } Config;



      /** CAN page 7: Receive FIFO (CAN.Page.RxFIFO) */
      struct {

        /** CAN mailbox filter match index register (CAN.Page.RxFIFO.MFMIR) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    FMI     : 8;    ///< Filter match index  
          } Reg;
          
        } MFMIR;


        /** CAN mailbox data length control register (CAN.Page.RxFIFO.MDLCR) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    DLC       : 4;    ///< Data length code
            int    res       : 3;    ///< Reserved
            int    TGT       : 1;    ///< Transmit global time
          } Reg;
          
        } MDLCR;


        /** CAN mailbox identifier register 1 (CAN.Page.RxFIFO.MIDR1)*/
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    ID        : 5;    ///< STID[10:6] or EXID[28:24]
            int    RTR       : 1;    ///< Remote transmission request
            int    IDE       : 1;    ///< Extended identifier
            int    res       : 1;    ///< Reserved
          } Reg;
          
        } MIDR1;


        /** CAN mailbox identifier register 2 (CAN.Page.RxFIFO.MIDR2) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    EXID      : 2;    ///< EXID[17:16]
            int    ID        : 6;    ///< STID[5:0] or EXID[23:18]
          } Reg;
        } MIDR2;


        /** CAN mailbox identifier register 3 (CAN.Page.RxFIFO.MIDR3) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            int    EXID      : 8;    ///< EXID[15:8]
          } Reg;
          
        } MIDR3;


        /** CAN mailbox identifier register 4 (CAN.Page.RxFIFO.MIDR4) */
        union {
          
          /// bytewise access to register
          uint8_t  Byte;
          
          /// bitwise access to register
          struct {
            uint8_t EXID      : 8;    ///< EXID[7:0]
          } Reg;
          
        } MIDR4;


        /** CAN mailbox data register 1 (CAN.Page.RxFIFO.MDAR1) */
        byte_t    MDAR1;


        /** CAN mailbox data register 2 (CAN.Page.RxFIFO.MDAR1) */
        byte_t    MDAR2;


        /** CAN mailbox data register 3 (CAN.Page.RxFIFO.MDAR1) */
        byte_t    MDAR3;


        /** CAN mailbox data register 4 (CAN.Page.RxFIFO.MDAR1) */
        byte_t    MDAR4;


        /** CAN mailbox data register 5 (CAN.Page.RxFIFO.MDAR1) */
        byte_t    MDAR5;


        /** CAN mailbox data register 6 (CAN.Page.RxFIFO.MDAR1) */
        byte_t    MDAR6;


        /** CAN mailbox data register 7 (CAN.Page.RxFIFO.MDAR1) */
        byte_t    MDAR7;


        /** CAN mailbox data register 8 (CAN.Page.RxFIFO.MDAR1) */
        byte_t    MDAR8;
        
        
        /** CAN mailbox time stamp register (CAN.Page.RxFIFO.MTSR) */
        word_t    MTSR;

      } RxFIFO;

    } Page; 

  } CAN_t;

  /// pointer to all CAN registers (all devices)
  #if defined(CAN_BaseAddress)
    #define _CAN  (*((CAN_t *) CAN_BaseAddress))
  #endif

  /* CAN Module Reset Values */
  #define   CAN_MCR_RESET_VALUE   ((uint8_t)0x02)
  #define   CAN_MSR_RESET_VALUE   ((uint8_t)0x02)
  #define   CAN_TSR_RESET_VALUE   ((uint8_t)0x00)
  #define   CAN_TPR_RESET_VALUE   ((uint8_t)0x0C)
  #define   CAN_RFR_RESET_VALUE   ((uint8_t)0x00)
  #define   CAN_IER_RESET_VALUE   ((uint8_t)0x00)
  #define   CAN_DGR_RESET_VALUE   ((uint8_t)0x0C)
  #define   CAN_PSR_RESET_VALUE   ((uint8_t)0x00)

  #define   CAN_ESR_RESET_VALUE   ((uint8_t)0x00)
  #define   CAN_EIER_RESET_VALUE  ((uint8_t)0x00)
  #define   CAN_TECR_RESET_VALUE  ((uint8_t)0x00)
  #define   CAN_RECR_RESET_VALUE  ((uint8_t)0x00)
  #define   CAN_BTR1_RESET_VALUE  ((uint8_t)0x40)
  #define   CAN_BTR2_RESET_VALUE  ((uint8_t)0x23)
  #define   CAN_FMR1_RESET_VALUE  ((uint8_t)0x00)
  #define   CAN_FMR2_RESET_VALUE  ((uint8_t)0x00)
  #define   CAN_FCR_RESET_VALUE   ((uint8_t)0x00)

  #define   CAN_MFMI_RESET_VALUE  ((uint8_t)0x00)
  #define   CAN_MDLC_RESET_VALUE  ((uint8_t)0x00)
  #define   CAN_MCSR_RESET_VALUE  ((uint8_t)0x00)

#endif // (1)



//------------------------
// UART1 Module (selected devices) with
//    asyncronous & synchronous mode
//    multiprocessor communication
//    SmartCard & IrDA mode
//    1-wire, half-duplex
//    LIN master mode
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing UART1 registers (selected devices) */
  typedef struct {

    /** UART1 Status register (UART1_SR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PE      : 1;    ///< Parity error
        int    FE      : 1;    ///< Framing error
        int    NF      : 1;    ///< Noise flag
        int    OR_LHE  : 1;    ///< LIN Header Error (LIN slave mode) / Overrun error
        int    IDLE    : 1;    ///< IDLE line detected
        int    RXNE    : 1;    ///< Read data register not empty
        int    TC      : 1;    ///< Transmission complete
        int    TXE     : 1;    ///< Transmit data register empty
      } Reg;
      
    } SR;


    /** UART1 data register (UART1_DR) */
    byte_t          DR; 


    /** UART1 Baud rate register 1 (UART1_BRR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UART_DIV_4_11 : 8;    ///< UART_DIV bits [11:4]
      } Reg;
      
    } BRR1;


    /** UART1 Baud rate register 2 (UART1_BRR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UART_DIV_0_3   : 4;    ///< UART_DIV bits [3:0]
        int    UART_DIV_12_15 : 4;    ///< UART_DIV bits [15:12]
      } Reg;
      
    } BRR2;


    /** UART1 Control register 1 (UART1_CR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PIEN    : 1;    ///< Parity interrupt enable
        int    PS      : 1;    ///< Parity selection
        int    PCEN    : 1;    ///< Parity control enable
        int    WAKE    : 1;    ///< Wakeup method
        int    M       : 1;    ///< word length
        int    UARTD   : 1;    ///< UART Disable (for low power consumption)
        int    T8      : 1;    ///< Transmit Data bit 8 (in 9-bit mode)
        int    R8      : 1;    ///< Receive Data bit 8 (in 9-bit mode)
      } Reg;
      
    } CR1;


    /** UART1 Control register 2 (UART1_CR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    SBK     : 1;    ///< Send break
        int    RWU     : 1;    ///< Receiver wakeup
        int    REN     : 1;    ///< Receiver enable
        int    TEN     : 1;    ///< Transmitter enable
        int    ILIEN   : 1;    ///< IDLE Line interrupt enable
        int    RIEN    : 1;    ///< Receiver interrupt enable
        int    TCIEN   : 1;    ///< Transmission complete interrupt enable
        int    TIEN    : 1;    ///< Transmitter interrupt enable
      } Reg;
      
    } CR2;


    /** UART1 Control register 3 (UART1_CR3) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    LBCL    : 1;    ///< Last bit clock pulse
        int    CPHA    : 1;    ///< Clock phase
        int    CPOL    : 1;    ///< Clock polarity
        int    CKEN    : 1;    ///< Clock enable
        int    STOP    : 2;    ///< STOP bits
        int    LINEN   : 1;    ///< LIN mode enable
        int    res     : 1;    ///< Reserved, must be kept cleared
      } Reg;
      
    } CR3;


    /** UART1 Control register 4 (UART1_CR4) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    ADD     : 4;    ///< Address of the UART node
        int    LBDF    : 1;    ///< LIN Break Detection Flag
        int    LBDL    : 1;    ///< LIN Break Detection Length
        int    LBDIEN  : 1;    ///< LIN Break Detection Interrupt Enable
        int    res     : 1;    ///< Reserved, must be kept cleared
      } Reg;
      
    } CR4;


    /** UART1 Control register 5 (UART1_CR5) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    res     : 1;    ///< Reserved, must be kept cleared
        int    IREN    : 1;    ///< IrDA mode Enable
        int    IRLP    : 1;    ///< IrDA Low Power
        int    HDSEL   : 1;    ///< Half-Duplex Selection
        int    NACK    : 1;    ///< Smartcard NACK enable
        int    SCEN    : 1;    ///< Smartcard mode enable
        int    res2    : 2;    ///< Reserved, must be kept cleared
      } Reg;
      
    } CR5;


    /** UART1 guard time register register (UART1_GTR) */
    byte_t          GTR;


    /** UART1 prescaler register register register (UART1_PSCR) */
    byte_t          PSCR;

  } UART1_t;
    
  /// pointer to UART1 registers
  #if defined(UART1_BaseAddress)
    #define _UART1  (*((UART1_t *) UART1_BaseAddress))
  #endif

  /* UART1 Module Reset Values */
  #define UART1_SR_RESET_VALUE   ((uint8_t)0xC0)
  #define UART1_BRR1_RESET_VALUE ((uint8_t)0x00)
  #define UART1_BRR2_RESET_VALUE ((uint8_t)0x00)
  #define UART1_CR1_RESET_VALUE  ((uint8_t)0x00)
  #define UART1_CR2_RESET_VALUE  ((uint8_t)0x00)
  #define UART1_CR3_RESET_VALUE  ((uint8_t)0x00)
  #define UART1_CR4_RESET_VALUE  ((uint8_t)0x00)
  #define UART1_CR5_RESET_VALUE  ((uint8_t)0x00)
  #define UART1_GTR_RESET_VALUE  ((uint8_t)0x00)
  #define UART1_PSCR_RESET_VALUE ((uint8_t)0x00)

#endif // (1)



//------------------------
// UART2 Module (selected devices) with
//    asyncronous & synchronous mode
//    multiprocessor communication
//    SmartCard & IrDA mode
//    LIN master & slave mode
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing UART2 registers (selected devices) */
  typedef struct   {

    /** UART2 Status register (UART2_SR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PE      : 1;    ///< Parity error
        int    FE      : 1;    ///< Framing error
        int    NF      : 1;    ///< Noise flag
        int    OR_LHE  : 1;    ///< LIN Header Error (LIN slave mode) / Overrun error
        int    IDLE    : 1;    ///< IDLE line detected
        int    RXNE    : 1;    ///< Read data register not empty
        int    TC      : 1;    ///< Transmission complete
        int    TXE     : 1;    ///< Transmit data register empty
      } Reg;
      
    } SR;


    /** UART2 data register (UART2_DR) */
    byte_t          DR; 


    /** UART2 Baud rate register 1 (UART2_BRR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UART_DIV_4_11 : 8;    ///< UART_DIV bits [11:4]
      } Reg;
      
    } BRR1;


    /** UART2 Baud rate register 2 (UART2_BRR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UART_DIV_0_3   : 4;    ///< UART_DIV bits [3:0]
        int    UART_DIV_12_15 : 4;    ///< UART_DIV bits [15:12]
      } Reg;
      
    } BRR2;


    /** UART2 Control register 1 (UART2_CR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PIEN    : 1;    ///< Parity interrupt enable
        int    PS      : 1;    ///< Parity selection
        int    PCEN    : 1;    ///< Parity control enable
        int    WAKE    : 1;    ///< Wakeup method
        int    M       : 1;    ///< word length
        int    UARTD   : 1;    ///< UART Disable (for low power consumption)
        int    T8      : 1;    ///< Transmit Data bit 8 (in 9-bit mode)
        int    R8      : 1;    ///< Receive Data bit 8 (in 9-bit mode)
      } Reg;
      
    } CR1;


    /** UART2 Control register 2 (UART2_CR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    SBK     : 1;    ///< Send break
        int    RWU     : 1;    ///< Receiver wakeup
        int    REN     : 1;    ///< Receiver enable
        int    TEN     : 1;    ///< Transmitter enable
        int    ILIEN   : 1;    ///< IDLE Line interrupt enable
        int    RIEN    : 1;    ///< Receiver interrupt enable
        int    TCIEN   : 1;    ///< Transmission complete interrupt enable
        int    TIEN    : 1;    ///< Transmitter interrupt enable
      } Reg;
      
    } CR2;


    /** UART2 Control register 3 (UART2_CR3) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    LBCL    : 1;    ///< Last bit clock pulse
        int    CPHA    : 1;    ///< Clock phase
        int    CPOL    : 1;    ///< Clock polarity
        int    CKEN    : 1;    ///< Clock enable
        int    STOP    : 2;    ///< STOP bits
        int    LINEN   : 1;    ///< LIN mode enable
        int    res     : 1;    ///< Reserved, must be kept cleared
      } Reg;
      
    } CR3;


    /** UART2 Control register 4 (UART2_CR4) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    ADD     : 4;    ///< Address of the UART node
        int    LBDF    : 1;    ///< LIN Break Detection Flag
        int    LBDL    : 1;    ///< LIN Break Detection Length
        int    LBDIEN  : 1;    ///< LIN Break Detection Interrupt Enable
        int    res     : 1;    ///< Reserved, must be kept cleared
      } Reg;
      
    } CR4;


    /** UART2 Control register 5 (UART2_CR5) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    res     : 1;    ///< Reserved, must be kept cleared
        int    IREN    : 1;    ///< IrDA mode Enable
        int    IRLP    : 1;    ///< IrDA Low Power
        int    res2    : 1;    ///< Reserved, must be kept cleared
        int    NACK    : 1;    ///< Smartcard NACK enable
        int    SCEN    : 1;    ///< Smartcard mode enable
        int    res3    : 2;    ///< Reserved, must be kept cleared
      } Reg;
      
    } CR5;


    /** UART2 Control register 6 (UART2_CR6) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    LSF     : 1;    ///< LIN Sync Field
        int    LHDF    : 1;    ///< LIN Header Detection Flag
        int    LHDIEN  : 1;    ///< LIN Header Detection Interrupt Enable
        int    res     : 1;    ///< Reserved
        int    LASE    : 1;    ///< LIN automatic resynchronisation enable
        int    LSLV    : 1;    ///< LIN Slave Enable
        int    res2    : 1;    ///< Reserved
        int    LDUM    : 1;    ///< LIN Divider Update Method
      } Reg;
      
    } CR6;


    /** UART2 guard time register register (UART2_GTR) */
    byte_t          GTR;


    /** UART2 prescaler register register register (UART2_PSCR) */
    byte_t          PSCR;

  } UART2_t;

  /// pointer to UART2 registers
  #if defined(UART2_BaseAddress)
    #define _UART2  (*((UART2_t *) UART2_BaseAddress))
  #endif

  /* UART2 Module Reset Values */
  #define UART2_SR_RESET_VALUE   ((uint8_t)0xC0)
  #define UART2_BRR1_RESET_VALUE ((uint8_t)0x00)
  #define UART2_BRR2_RESET_VALUE ((uint8_t)0x00)
  #define UART2_CR1_RESET_VALUE  ((uint8_t)0x00)
  #define UART2_CR2_RESET_VALUE  ((uint8_t)0x00)
  #define UART2_CR3_RESET_VALUE  ((uint8_t)0x00)
  #define UART2_CR4_RESET_VALUE  ((uint8_t)0x00)
  #define UART2_CR5_RESET_VALUE  ((uint8_t)0x00)
  #define UART2_CR6_RESET_VALUE  ((uint8_t)0x00)
  #define UART2_GTR_RESET_VALUE  ((uint8_t)0x00)
  #define UART2_PSCR_RESET_VALUE ((uint8_t)0x00)

#endif // (1)



//------------------------
// UART3 Module (selected devices) with
//    asyncronous mode
//    multiprocessor communication
//    LIN master & slave mode
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing UART3 registers (selected devices) */
  typedef struct {

    /** UART3 Status register (UART3_SR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PE      : 1;    ///< Parity error
        int    FE      : 1;    ///< Framing error
        int    NF      : 1;    ///< Noise flag
        int    OR_LHE  : 1;    ///< LIN Header Error (LIN slave mode) / Overrun error
        int    IDLE    : 1;    ///< IDLE line detected
        int    RXNE    : 1;    ///< Read data register not empty
        int    TC      : 1;    ///< Transmission complete
        int    TXE     : 1;    ///< Transmit data register empty
      } Reg;
      
    } SR;


    /** UART3 data register (UART3_DR) */
    byte_t          DR; 


    /** UART3 Baud rate register 1 (UART3_BRR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UART_DIV_4_11 : 8;    ///< UART_DIV bits [11:4]
      } Reg;
      
    } BRR1;


    /** UART3 Baud rate register 2 (UART3_BRR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UART_DIV_0_3   : 4;    ///< UART_DIV bits [3:0]
        int    UART_DIV_12_15 : 4;    ///< UART_DIV bits [15:12]
      } Reg;
      
    } BRR2;


    /** UART3 Control register 1 (UART3_CR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PIEN    : 1;    ///< Parity interrupt enable
        int    PS      : 1;    ///< Parity selection
        int    PCEN    : 1;    ///< Parity control enable
        int    WAKE    : 1;    ///< Wakeup method
        int    M       : 1;    ///< word length
        int    UARTD   : 1;    ///< UART Disable (for low power consumption)
        int    T8      : 1;    ///< Transmit Data bit 8 (in 9-bit mode)
        int    R8      : 1;    ///< Receive Data bit 8 (in 9-bit mode)
      } Reg;
      
    } CR1;


    /** UART3 Control register 2 (UART3_CR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    SBK     : 1;    ///< Send break
        int    RWU     : 1;    ///< Receiver wakeup
        int    REN     : 1;    ///< Receiver enable
        int    TEN     : 1;    ///< Transmitter enable
        int    ILIEN   : 1;    ///< IDLE Line interrupt enable
        int    RIEN    : 1;    ///< Receiver interrupt enable
        int    TCIEN   : 1;    ///< Transmission complete interrupt enable
        int    TIEN    : 1;    ///< Transmitter interrupt enable
      } Reg;
      
    } CR2;


    /** UART3 Control register 3 (UART3_CR3) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    res     : 4;    ///< Reserved, must be kept cleared
        int    STOP    : 2;    ///< STOP bits
        int    LINEN   : 1;    ///< LIN mode enable
        int    res2    : 1;    ///< Reserved, must be kept cleared
      } Reg;
      
    } CR3;


    /** UART3 Control register 4 (UART3_CR4) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    ADD     : 4;    ///< Address of the UART node
        int    LBDF    : 1;    ///< LIN Break Detection Flag
        int    LBDL    : 1;    ///< LIN Break Detection Length
        int    LBDIEN  : 1;    ///< LIN Break Detection Interrupt Enable
        int    res     : 1;    ///< Reserved, must be kept cleared
      } Reg;
      
    } CR4;


    /** Reserved register (1B) */
    uint8_t         res2[1]; 


    /** UART3 Control register 6 (UART3_CR6) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    LSF     : 1;    ///< LIN Sync Field
        int    LHDF    : 1;    ///< LIN Header Detection Flag
        int    LHDIEN  : 1;    ///< LIN Header Detection Interrupt Enable
        int    res     : 1;    ///< Reserved
        int    LASE    : 1;    ///< LIN automatic resynchronisation enable
        int    LSLV    : 1;    ///< LIN Slave Enable
        int    res2    : 1;    ///< Reserved
        int    LDUM    : 1;    ///< LIN Divider Update Method
      } Reg;
      
    } CR6;

  } UART3_t;

  /// pointer to UART3 registers
  #if defined(UART3_BaseAddress)
    #define _UART3  (*((UART3_t *) UART3_BaseAddress))
  #endif

  /* UART3 Module Reset Values */
  #define UART3_SR_RESET_VALUE   ((uint8_t)0xC0)
  #define UART3_BRR1_RESET_VALUE ((uint8_t)0x00)
  #define UART3_BRR2_RESET_VALUE ((uint8_t)0x00)
  #define UART3_CR1_RESET_VALUE  ((uint8_t)0x00)
  #define UART3_CR2_RESET_VALUE  ((uint8_t)0x00)
  #define UART3_CR3_RESET_VALUE  ((uint8_t)0x00)
  #define UART3_CR4_RESET_VALUE  ((uint8_t)0x00)
  #define UART3_CR6_RESET_VALUE  ((uint8_t)0x00)

#endif // (1)



//------------------------
// UART4 Module (selected devices) with
//    asyncronous & synchronous mode
//    multiprocessor communication
//    SmartCard & IrDA mode
//    1-wire, half-duplex
//    LIN master & slave mode
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing UART4 registers (selected devices) */
  typedef struct {

    /** UART4 Status register (UART4_SR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PE      : 1;    ///< Parity error
        int    FE      : 1;    ///< Framing error
        int    NF      : 1;    ///< Noise flag
        int    OR_LHE  : 1;    ///< LIN Header Error (LIN slave mode) / Overrun error
        int    IDLE    : 1;    ///< IDLE line detected
        int    RXNE    : 1;    ///< Read data register not empty
        int    TC      : 1;    ///< Transmission complete
        int    TXE     : 1;    ///< Transmit data register empty
      } Reg;
      
    } SR;


    /** UART4 data register (UART4_DR) */
    byte_t          DR; 


    /** UART4 Baud rate register 1 (UART4_BRR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UART_DIV_4_11 : 8;    ///< UART_DIV bits [11:4]
      } Reg;
      
    } BRR1;


    /** UART4 Baud rate register 2 (UART4_BRR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UART_DIV_0_3   : 4;    ///< UART_DIV bits [3:0]
        int    UART_DIV_12_15 : 4;    ///< UART_DIV bits [15:12]
      } Reg;
      
    } BRR2;


    /** UART4 Control register 1 (UART4_CR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PIEN    : 1;    ///< Parity interrupt enable
        int    PS      : 1;    ///< Parity selection
        int    PCEN    : 1;    ///< Parity control enable
        int    WAKE    : 1;    ///< Wakeup method
        int    M       : 1;    ///< word length
        int    UARTD   : 1;    ///< UART Disable (for low power consumption)
        int    T8      : 1;    ///< Transmit Data bit 8 (in 9-bit mode)
        int    R8      : 1;    ///< Receive Data bit 8 (in 9-bit mode)
      } Reg;
      
    } CR1;


    /** UART4 Control register 2 (UART4_CR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    SBK     : 1;    ///< Send break
        int    RWU     : 1;    ///< Receiver wakeup
        int    REN     : 1;    ///< Receiver enable
        int    TEN     : 1;    ///< Transmitter enable
        int    ILIEN   : 1;    ///< IDLE Line interrupt enable
        int    RIEN    : 1;    ///< Receiver interrupt enable
        int    TCIEN   : 1;    ///< Transmission complete interrupt enable
        int    TIEN    : 1;    ///< Transmitter interrupt enable
      } Reg;
      
    } CR2;


    /** UART4 Control register 3 (UART4_CR3) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    LBCL    : 1;    ///< Last bit clock pulse
        int    CPHA    : 1;    ///< Clock phase
        int    CPOL    : 1;    ///< Clock polarity
        int    CKEN    : 1;    ///< Clock enable
        int    STOP    : 2;    ///< STOP bits
        int    LINEN   : 1;    ///< LIN mode enable
        int    res     : 1;    ///< Reserved, must be kept cleared
      } Reg;
      
    } CR3;


    /** UART4 Control register 4 (UART4_CR4) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    ADD     : 4;    ///< Address of the UART node
        int    LBDF    : 1;    ///< LIN Break Detection Flag
        int    LBDL    : 1;    ///< LIN Break Detection Length
        int    LBDIEN  : 1;    ///< LIN Break Detection Interrupt Enable
        int    res     : 1;    ///< Reserved, must be kept cleared
      } Reg;
      
    } CR4;


    /** UART4 Control register 5 (UART4_CR5) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    res     : 1;    ///< Reserved, must be kept cleared
        int    IREN    : 1;    ///< IrDA mode Enable
        int    IRLP    : 1;    ///< IrDA Low Power
        int    HDSEL   : 1;    ///< Half-Duplex Selection
        int    NACK    : 1;    ///< Smartcard NACK enable
        int    SCEN    : 1;    ///< Smartcard mode enable
        int    res2    : 2;    ///< Reserved, must be kept cleared
      } Reg;
      
    } CR5;


    /** UART4 Control register 6 (UART4_CR6) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    LSF     : 1;    ///< LIN Sync Field
        int    LHDF    : 1;    ///< LIN Header Detection Flag
        int    LHDIEN  : 1;    ///< LIN Header Detection Interrupt Enable
        int    res     : 1;    ///< Reserved
        int    LASE    : 1;    ///< LIN automatic resynchronisation enable
        int    LSLV    : 1;    ///< LIN Slave Enable
        int    res2    : 1;    ///< Reserved
        int    LDUM    : 1;    ///< LIN Divider Update Method
      } Reg;
      
    } CR6;


    /** UART4 guard time register register (UART4_GTR) */
    byte_t          GTR;


    /** UART4 prescaler register register register (UART4_PSCR) */
    byte_t          PSCR;

  } UART4_t;
    
  /// pointer to UART4 registers
  #if defined(UART4_BaseAddress)
    #define _UART4  (*((UART4_t *) UART4_BaseAddress))
  #endif

  /* UART4 Module Reset Values */
  #define UART4_SR_RESET_VALUE   ((uint8_t)0xC0)
  #define UART4_BRR1_RESET_VALUE ((uint8_t)0x00)
  #define UART4_BRR2_RESET_VALUE ((uint8_t)0x00)
  #define UART4_CR1_RESET_VALUE  ((uint8_t)0x00)
  #define UART4_CR2_RESET_VALUE  ((uint8_t)0x00)
  #define UART4_CR3_RESET_VALUE  ((uint8_t)0x00)
  #define UART4_CR4_RESET_VALUE  ((uint8_t)0x00)
  #define UART4_CR5_RESET_VALUE  ((uint8_t)0x00)
  #define UART4_CR6_RESET_VALUE  ((uint8_t)0x00)
  #define UART4_GTR_RESET_VALUE  ((uint8_t)0x00)
  #define UART4_PSCR_RESET_VALUE ((uint8_t)0x00)
 
#endif // (1)



//------------------------
// Analog Digital Converter ADC1 (selected devices) with result buffer & analog watchdog etc
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing ADC1 registers (selected devices) */
  typedef struct {

    /** ADC1 10-bit Data Buffer Register 0 (ADC1_DB0R) */
    word_t        DB0R;                    
                                         
    /** ADC1 10-bit Data Buffer Register 1 (ADC1_DB1R) */
    word_t        DB1R;                    
                                         
    /** ADC1 10-bit Data Buffer Register 2 (ADC1_DB2R) */
    word_t        DB2R;                    
                                         
    /** ADC1 10-bit Data Buffer Register 3 (ADC1_DB3R) */
    word_t        DB3R;                    
                                         
    /** ADC1 10-bit Data Buffer Register 4 (ADC1_DB4R) */
    word_t        DB4R;                    
                                         
    /** ADC1 10-bit Data Buffer Register 5 (ADC1_DB5R) */
    word_t        DB5R;                    
                                         
    /** ADC1 10-bit Data Buffer Register 6 (ADC1_DB6R) */
    word_t        DB6R;                    
                                         
    /** ADC1 10-bit Data Buffer Register 7 (ADC1_DB7R) */
    word_t        DB7R;                    
                                         
    /** ADC1 10-bit Data Buffer Register 8 (ADC1_DB8R) */
    word_t        DB8R;                    
                                         
    /** ADC1 10-bit Data Buffer Register 9 (ADC1_DB9R) */
    word_t        DB9R;


    /** Reserved register (12B) */
    uint8_t       res[12]; 


    /** ADC1 control/status register (ADC1_CSR) */
    union {

      /// bytewise access to register
      uint8_t  Byte;

      /// bitwise access to register
      struct {
        int    CH     : 4;  ///< Channel selection bits
        int    AWDIE  : 1;  ///< Analog watchdog interrupt enable
        int    EOCIE  : 1;  ///< Interrupt enable for EOC
        int    AWD    : 1;  ///< Analog Watchdog flag
        int    EOC    : 1;  ///< End of conversion
      } Reg;

    } CSR;


    /** ADC1 Configuration Register 1 (ADC1_CR1) */
    union {

      /// bytewise access to register
      uint8_t  Byte;

      /// bitwise access to register
      struct {
        int    ADON   : 1;  ///< A/D Converter on/off
        int    CONT   : 1;  ///< Continuous conversion
        int    res    : 2;  ///< Reserved, always read as 0
        int    SPSEL  : 3;  ///< Clock prescaler selection
        int    res2   : 1;  ///< Reserved, always read as 0
      } Reg;

    } CR1;


    /** ADC1 Configuration Register 2 (ADC1_CR2) */
    union {

      /// bytewise access to register
      uint8_t  Byte;

      /// bitwise access to register
      struct {
        int    res     : 1;  ///< Reserved, must be kept cleared
        int    SCAN    : 1;  ///< Scan mode enable
        int    res2    : 1;  ///< Reserved, must be kept cleared
        int    ALIGN   : 1;  ///< Data alignment
        int    EXTSEL  : 2;  ///< External event selection
        int    EXTTRIG : 1;  ///< External trigger enable
        int    res3    : 1;  ///< Reserved, must be kept cleared
      } Reg;

    } CR2;


    /** ADC1 Configuration Register 3 (ADC1_CR3) */
    union {

      /// bytewise access to register
      uint8_t  Byte;

      /// bitwise access to register
      struct {
        int    res   : 6;  ///< Reserved, must be kept cleared
        int    OVR   : 1;  ///< Overrun flag
        int    DBUF  : 1;  ///< Data buffer enable
      } Reg;

    } CR3;


    /** ADC1 (unbuffered) 10-bit measurement result (ADC1_DR) */
    word_t        DR;

    /** ADC1 Schmitt trigger disable register (ADC1_TDR) */
    word_t        TDR;

    /** ADC1 watchdog high threshold register (ADC1_HTR) */
    word_t        HTR;

    /** ADC1 watchdog low threshold register (ADC1_LTR) */
    word_t        LTR;

    /** ADC1 watchdog status register (ADC1_AWSR) */
    word_t        AWSR;

    /** ADC1 watchdog control register (ADC1_AWCR) */
    word_t        AWCR;

  } ADC1_t;
    
  /// pointer to ADC1 registers
  #if defined(ADC1_BaseAddress)
    #define _ADC1  (*((ADC1_t *) ADC1_BaseAddress))
  #endif

  /* ADC1 Module Reset Values */
  #define  ADC1_CSR_RESET_VALUE    ((uint8_t)0x00)
  #define  ADC1_CR1_RESET_VALUE    ((uint8_t)0x00)
  #define  ADC1_CR2_RESET_VALUE    ((uint8_t)0x00)
  #define  ADC1_CR3_RESET_VALUE    ((uint8_t)0x00)
  #define  ADC1_TDRL_RESET_VALUE   ((uint8_t)0x00)
  #define  ADC1_TDRH_RESET_VALUE   ((uint8_t)0x00)
  #define  ADC1_HTRL_RESET_VALUE   ((uint8_t)0x03)
  #define  ADC1_HTRH_RESET_VALUE   ((uint8_t)0xFF)
  #define  ADC1_LTRH_RESET_VALUE   ((uint8_t)0x00)
  #define  ADC1_LTRL_RESET_VALUE   ((uint8_t)0x00)
  #define  ADC1_AWCRH_RESET_VALUE  ((uint8_t)0x00)
  #define  ADC1_AWCRL_RESET_VALUE  ((uint8_t)0x00)

#endif // (1)



//------------------------
// Analog Digital Converter ADC2 (selected devices) without result buffer, analog watchdog etc.
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing ADC2 registers (selected devices) */
  typedef struct {

    /** ADC2 control/status register (ADC2_CSR) */
    union {

      /// bytewise access to register
      uint8_t  Byte;

      /// bitwise access to register
      struct {
        int    CH      : 4;    ///< Channel selection bits
        int    res     : 1;    ///< Reserved
        int    EOCIE   : 1;    ///< Interrupt enable for EOC
        int    res2    : 1;    ///< Reserved
        int    EOC     : 1;    ///< End of conversion flag
      } Reg;

    } CSR;


    /** ADC2 Configuration Register 1 (ADC2_CR1) */
    union {

      /// bytewise access to register
      uint8_t  Byte;

      /// bitwise access to register
      struct {
        int    ADON   : 1;  ///< A/D Converter on/off
        int    CONT   : 1;  ///< Continuous conversion
        int    res    : 2;  ///< Reserved, always read as 0
        int    SPSEL  : 3;  ///< Clock prescaler selection
        int    res2   : 1;  ///< Reserved, always read as 0
      } Reg;

    } CR1;


    /** ADC2 Configuration Register 2 (ADC2_CR2) */
    union {

      /// bytewise access to register
      uint8_t  Byte;

      /// bitwise access to register
      struct {
        int    res     : 3;  ///< Reserved, must be kept cleared
        int    ALIGN   : 1;  ///< Data alignment
        int    EXTSEL  : 2;  ///< External event selection
        int    EXTTRIG : 1;  ///< External trigger enable
        int    res2    : 1;  ///< Reserved, must be kept cleared
      } Reg;

    } CR2;


    /** Reserved register (1B) */
    uint8_t         res[1]; 


    /** ADC2 (unbuffered) 10-bit measurement result (ADC2_DR) */
    word_t          DR;

    /** ADC2 Schmitt trigger disable register (ADC2_TDR) */
    word_t          TDR;

  } ADC2_t;
    
  /// pointer to all ADC2 registers
  #if defined(ADC2_BaseAddress)
    #define _ADC2  (*((ADC2_t *) ADC2_BaseAddress))
  #endif

  /* ADC2 Module Reset Values */
  #define  ADC2_CSR_RESET_VALUE  ((uint8_t)0x00)
  #define  ADC2_CR1_RESET_VALUE  ((uint8_t)0x00)
  #define  ADC2_CR2_RESET_VALUE  ((uint8_t)0x00)
  #define  ADC2_TDRL_RESET_VALUE ((uint8_t)0x00)
  #define  ADC2_TDRH_RESET_VALUE ((uint8_t)0x00)

#endif // (1)



//------------------------
// 16-Bit Timer TIM1 (all devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing TIM1 registers (all devices) */
  typedef struct {

    /** TIM1 Control register 1 (TIM1_CR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CEN     : 1;    ///< Counter enable
        int    UDIS    : 1;    ///< Update disable
        int    URS     : 1;    ///< Update request source
        int    OPM     : 1;    ///< One-pulse mode
        int    DIR     : 1;    ///< Direction
        int    CMS     : 2;    ///< Center-aligned mode selection
        int    ARPE    : 1;    ///< Auto-reload preload enable
      } Reg;
      
    } CR1;


    /** TIM1 Control register 2 (TIM1_CR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CCPC    : 1;    ///< Capture/compare preloaded control
        int    res     : 1;    ///< Reserved, forced by hardware to 0
        int    COMS    : 1;    ///< Capture/compare control update selection
        int    res2    : 1;    ///< Reserved, must be kept cleared
        int    MMS     : 3;    ///< Master mode selection
        int    res3    : 1;    ///< Reserved
      } Reg;
      
    } CR2;


    /** Slave mode control register (TIM1_SMCR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    SMS     : 3;    ///< Clock/trigger/slave mode selection
        int    res     : 1;    ///< Reserved
        int    TS      : 3;    ///< Trigger selection
        int    MSM     : 1;    ///< Master/slave mode
      } Reg;
      
    } SMCR;


    /** TIM1 External trigger register (TIM1_ETR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    ETF     : 4;    ///< External trigger filter
        int    ETPS    : 2;    ///< External trigger prescaler
        int    ECE     : 1;    ///< External clock enable
        int    ETP     : 1;    ///< External trigger polarity
      } Reg;
      
    } ETR;


    /** TIM1 Interrupt enable register (TIM1_IER) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UIE     : 1;    ///< Update interrupt enable
        int    CC1IE   : 1;    ///< Capture/compare 1 interrupt enable
        int    CC2IE   : 1;    ///< Capture/compare 2 interrupt enable
        int    CC3IE   : 1;    ///< Capture/compare 3 interrupt enable
        int    CC4IE   : 1;    ///< Capture/compare 4 interrupt enable
        int    COMIE   : 1;    ///< Commutation interrupt enable
        int    TIE     : 1;    ///< Trigger interrupt enable
        int    BIE     : 1;    ///< Break interrupt enable
      } Reg;
      
    } IER;


    /** TIM1 Status register 1 (TIM1_SR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UIF     : 1;    ///< Update interrupt flag
        int    CC1IF   : 1;    ///< Capture/compare 1 interrupt flag
        int    CC2IF   : 1;    ///< Capture/compare 2 interrupt flag
        int    CC3IF   : 1;    ///< Capture/compare 3 interrupt flag
        int    CC4IF   : 1;    ///< Capture/compare 4 interrupt flag
        int    COMIF   : 1;    ///< Commutation interrupt flag
        int    TIF     : 1;    ///< Trigger interrupt flag
        int    BIF     : 1;    ///< Break interrupt flag
      } Reg;
      
    } SR1;


    /** TIM1 Status register 2 (TIM1_SR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    res     : 1;    ///< Reserved, must be kept cleared
        int    CC1OF   : 1;    ///< Capture/compare 1 overcapture flag
        int    CC2OF   : 1;    ///< Capture/compare 2 overcapture flag
        int    CC3OF   : 1;    ///< Capture/compare 3 overcapture flag
        int    CC4OF   : 1;    ///< Capture/compare 4 overcapture flag
        int    res2    : 3;    ///< Reserved, must be kept cleared
      } Reg;
      
    } SR2;


    /** TIM1 Event generation register (TIM1_EGR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UG      : 1;    ///< Update generation
        int    CC1G    : 1;    ///< Capture/compare 1 generation
        int    CC2G    : 1;    ///< Capture/compare 2 generation
        int    CC3G    : 1;    ///< Capture/compare 3 generation
        int    CC4G    : 1;    ///< Capture/compare 4 generation
        int    COMG    : 1;    ///< Capture/compare control update generation
        int    TG      : 1;    ///< Trigger generation
        int    BG      : 1;    ///< Break generation
      } Reg;
      
    } EGR;


    /** TIM1 Capture/compare mode register 1 (TIM1_CCMR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register (output mode)
      struct {
        int    CC1S    : 2;    ///< Capture/compare 1 selection
        int    OC1FE   : 1;    ///< Output compare 1 fast enable
        int    OC1PE   : 1;    ///< Output compare 1 preload enable
        int    OC1M    : 3;    ///< Output compare 1 mode
        int    OC1CE   : 1;    ///< Output compare 1 clear enable
      } regOut;
      
      /// bitwise access to register (input mode)
      struct {
        int    CC1S    : 2;    ///< Capture/compare 1 selection
        int    IC1PSC  : 2;    ///< Input capture 1 prescaler
        int    IC1F    : 4;    ///< Input capture 1 filter
      } regIn;
      
    } CCMR1;


    /** TIM1 Capture/compare mode register 2 (TIM1_CCMR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register (output mode)
      struct {
        int    CC2S    : 2;    ///< Capture/compare 2 selection
        int    OC2FE   : 1;    ///< Output compare 2 fast enable
        int    OC2PE   : 1;    ///< Output compare 2 preload enable
        int    OC2M    : 3;    ///< Output compare 2 mode
        int    OC2CE   : 1;    ///< Output compare 2 clear enable
      } regOut;
      
      /// bitwise access to register (input mode)
      struct {
        int    CC2S    : 2;    ///< Capture/compare 2 selection
        int    IC2PSC  : 2;    ///< Input capture 2 prescaler
        int    IC2F    : 4;    ///< Input capture 2 filter
      } regIn;
      
    } CCMR2;


    /** TIM1 Capture/compare mode register 3 (TIM1_CCMR3) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register (output mode)
      struct {
        int    CC3S    : 2;    ///< Capture/compare 3 selection
        int    OC3FE   : 1;    ///< Output compare 3 fast enable
        int    OC3PE   : 1;    ///< Output compare 3 preload enable
        int    OC3M    : 3;    ///< Output compare 3 mode
        int    OC3CE   : 1;    ///< Output compare 3 clear enable
      } regOut;
        
      /// bitwise access to register (input mode)
      struct {
        int    CC3S    : 2;    ///< Capture/compare 3 selection
        int    IC3PSC  : 2;    ///< Input capture 3 prescaler
        int    IC3F    : 4;    ///< Input capture 3 filter
      } regIn;

    } CCMR3;


    /** TIM1 Capture/compare mode register 4 (TIM1_CCMR4) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register (output mode)
      struct {
        int    CC4S    : 2;    ///< Capture/compare 4 selection
        int    OC4FE   : 1;    ///< Output compare 4 fast enable
        int    OC4PE   : 1;    ///< Output compare 4 preload enable
        int    OC4M    : 3;    ///< Output compare 4 mode
        int    OC4CE   : 1;    ///< Output compare 4 clear enable
      } regOut;
        
      /// bitwise access to register (input mode)
      struct {
        int    CC4S    : 2;    ///< Capture/compare 4 selection
        int    IC4PSC  : 2;    ///< Input capture 4 prescaler
        int    IC4F    : 4;    ///< Input capture 4 filter
      } regIn;
      
    } CCMR4;


    /** TIM1 Capture/compare enable register 1 (TIM1_CCER1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CC1E    : 1;    ///< Capture/compare 1 output enable
        int    CC1P    : 1;    ///< Capture/compare 1 output polarity
        int    CC1NE   : 1;    ///< Capture/compare 1 complementary output enable
        int    CC1NP   : 1;    ///< Capture/compare 1 complementary output polarity
        int    CC2E    : 1;    ///< Capture/compare 2 output enable
        int    CC2P    : 1;    ///< Capture/compare 2 output polarity
        int    CC2NE   : 1;    ///< Capture/compare 2 complementary output enable
        int    CC2NP   : 1;    ///< Capture/compare 2 complementary output polarity
      } Reg;
      
    } CCER1;


    /** TIM1 Capture/compare enable register 2 (TIM1_CCER2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CC3E    : 1;    ///< Capture/compare 3 output enable
        int    CC3P    : 1;    ///< Capture/compare 3 output polarity
        int    CC3NE   : 1;    ///< Capture/compare 3 complementary output enable
        int    CC3NP   : 1;    ///< Capture/compare 3 complementary output polarity
        int    CC4E    : 1;    ///< Capture/compare 4 output enable
        int    CC4P    : 1;    ///< Capture/compare 4 output polarity
        int    res     : 2;    ///< Reserved
      } Reg;
      
    } CCER2;


    /** TIM1 16-bit counter (TIM1_CNTR) */
    word_t        CNTR;
    
    
    /** TIM1 16-bit prescaler (TIM1_PSCR) */
    word_t        PSCR;
    
    
    /** TIM1 16-bit re-load value (TIM1_ARR) */
    word_t        ARR;
    
    
    /** TIM1 Repetition counter (TIM1_RCR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    REP     : 8;    ///< Repetition counter value
      } Reg;
      
    } RCR;


    /** TIM1 16-bit capture/compare value 1 (TIM1_CCR1) */
    word_t        CCR1;


    /** TIM1 16-bit capture/compare value 2 (TIM1_CCR2) */
    word_t        CCR2;


    /** TIM1 16-bit capture/compare value 3 (TIM1_CCR3) */
    word_t        CCR3;


    /** TIM1 16-bit capture/compare value 4 (TIM1_CCR4) */
    word_t        CCR4;
    
    
    /** TIM1 Break register (TIM1_BKR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    LOCK    : 2;    ///< Lock configuration
        int    OSSI    : 1;    ///< Off state selection for idle mode
        int    OSSR    : 1;    ///< Off state selection for Run mode
        int    BKE     : 1;    ///< Break enable
        int    BKP     : 1;    ///< Break polarity
        int    AOE     : 1;    ///< Automatic output enable
        int    MOE     : 1;    ///< Main output enable
      } Reg;
      
    } BKR;


    /** TIM1 Dead-time register (TIM1_DTR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    DTG     : 8;    ///< Deadtime generator set-up
      } Reg;
      
    } DTR;


    /** TIM1 Output idle state register (TIM1_OISR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    OIS1    : 1;    ///< Output idle state 1 (OC1 output)
        int    OIS1N   : 1;    ///< Output idle state 1 (OC1N output)
        int    OIS2    : 1;    ///< Output idle state 2 (OC2 output)
        int    OIS2N   : 1;    ///< Output idle state 2 (OC2N output)
        int    OIS3    : 1;    ///< Output idle state 3 (OC3 output)
        int    OIS3N   : 1;    ///< Output idle state 3 (OC3N output)
        int    OIS4    : 1;    ///< Output idle state 4 (OC4 output)
        int    res     : 1;    ///< Reserved, forced by hardware to 0
      } Reg;
      
    } OISR;

  } TIM1_t;

  /// pointer to all TIM1 registers
  #if defined(TIM1_BaseAddress)
    #define _TIM1  (*((TIM1_t *) TIM1_BaseAddress))
  #endif

  /* TIM1 Module Reset Values */
  #define TIM1_CR1_RESET_VALUE   ((uint8_t)0x00)
  #define TIM1_CR2_RESET_VALUE   ((uint8_t)0x00)
  #define TIM1_SMCR_RESET_VALUE  ((uint8_t)0x00)
  #define TIM1_ETR_RESET_VALUE   ((uint8_t)0x00)
  #define TIM1_IER_RESET_VALUE   ((uint8_t)0x00)
  #define TIM1_SR1_RESET_VALUE   ((uint8_t)0x00)
  #define TIM1_SR2_RESET_VALUE   ((uint8_t)0x00)
  #define TIM1_EGR_RESET_VALUE   ((uint8_t)0x00)
  #define TIM1_CCMR1_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_CCMR2_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_CCMR3_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_CCMR4_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_CCER1_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_CCER2_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_CNTRH_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_CNTRL_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_PSCRH_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_PSCRL_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_ARRH_RESET_VALUE  ((uint8_t)0xFF)
  #define TIM1_ARRL_RESET_VALUE  ((uint8_t)0xFF)
  #define TIM1_RCR_RESET_VALUE   ((uint8_t)0x00)
  #define TIM1_CCR1H_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_CCR1L_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_CCR2H_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_CCR2L_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_CCR3H_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_CCR3L_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_CCR4H_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_CCR4L_RESET_VALUE ((uint8_t)0x00)
  #define TIM1_BKR_RESET_VALUE   ((uint8_t)0x00)
  #define TIM1_DTR_RESET_VALUE   ((uint8_t)0x00)
  #define TIM1_OISR_RESET_VALUE  ((uint8_t)0x00)

#endif // (1)



//------------------------
// 16-Bit Timer TIM2 (selected devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing TIM2 registers */
  typedef struct {

    /** TIM2 Control register (TIM2_CR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CEN     : 1;    ///< Counter enable
        int    UDIS    : 1;    ///< Update disable
        int    URS     : 1;    ///< Update request source
        int    OPM     : 1;    ///< One-pulse mode
        int    res     : 3;    ///< Reserved
        int    ARPE    : 1;    ///< Auto-reload preload enable
      } Reg;
      
    } CR1;

    
    /** Reserved registers on selected devices (2B) */
    #if defined(STM8S103) || defined(STM8S003) || defined(STM8S001)
      uint8_t         res[2];
    #endif
      

    /** TIM2 Interrupt enable (TIM2_IER) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UIE     : 1;    ///< Update interrupt enable
        int    CC1IE   : 1;    ///< Capture/compare 1 interrupt enable
        int    CC2IE   : 1;    ///< Capture/compare 2 interrupt enable
        int    CC3IE   : 1;    ///< Capture/compare 3 interrupt enable
        int    res     : 4;    ///< Reserved 
      } Reg;
      
    } IER;


    /** TIM2 Status register 1 (TIM2_SR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UIF     : 1;    ///< Update interrupt flag
        int    CC1IF   : 1;    ///< Capture/compare 1 interrupt flag
        int    CC2IF   : 1;    ///< Capture/compare 2 interrupt flag
        int    CC3IF   : 1;    ///< Capture/compare 3 interrupt flag
        int    res     : 4;    ///< Reserved
      } Reg;
      
    } SR1;


    /** TIM2 Status register 2 (TIM2_SR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    res     : 1;    ///< Reserved
        int    CC1OF   : 1;    ///< Capture/compare 1 overcapture flag
        int    CC2OF   : 1;    ///< Capture/compare 2 overcapture flag
        int    CC3OF   : 1;    ///< Capture/compare 3 overcapture flag
        int    res2    : 4;    ///< Reserved
      } Reg;
      
    } SR2;


    /** TIM2 Event Generation (TIM2_EGR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UG      : 1;    ///< Update generation
        int    CC1G    : 1;    ///< Capture/compare 1 generation
        int    CC2G    : 1;    ///< Capture/compare 2 generation
        int    CC3G    : 1;    ///< Capture/compare 3 generation
        int    res     : 4;    ///< Reserved
      } Reg;
      
    } EGR;


    /** TIM2 Capture/compare mode register 1 (TIM2_CCMR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register (output mode)
      struct {
        int    CC1S    : 2;    ///< Capture/compare 1 selection
        int    res     : 1;    ///< Reserved
        int    OC1PE   : 1;    ///< Output compare 1 preload enable
        int    OC1M    : 3;    ///< Output compare 1 mode
        int    res2    : 1;    ///< Reserved
      } regOut;
      
      /// bitwise access to register (input mode)
      struct {
        int    CC1S    : 2;    ///< Capture/compare 1 selection
        int    IC1PSC  : 2;    ///< Input capture 1 prescaler
        int    IC1F    : 4;    ///< Input capture 1 filter
      } regIn;
      
    } CCMR1;


    /** TIM2 Capture/compare mode register 2 (TIM2_CCMR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register (output mode)
      struct {
        int    CC2S    : 2;    ///< Capture/compare 2 selection
        int    res     : 1;    ///< Reserved
        int    OC2PE   : 1;    ///< Output compare 2 preload enable
        int    OC2M    : 3;    ///< Output compare 2 mode
        int    res2    : 1;    ///< Reserved
      } regOut;
      
      /// bitwise access to register (input mode)
      struct {
        int    CC2S    : 2;    ///< Capture/compare 2 selection
        int    IC2PSC  : 2;    ///< Input capture 2 prescaler
        int    IC2F    : 4;    ///< Input capture 2 filter
      } regIn;
      
    } CCMR2;


    /** TIM2 Capture/compare mode register 3 (TIM2_CCMR3) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register (output mode)
      struct {
        int    CC3S    : 2;    ///< Capture/compare 3 selection
        int    res     : 1;    ///< Reserved
        int    OC3PE   : 1;    ///< Output compare 3 preload enable
        int    OC3M    : 3;    ///< Output compare 3 mode
        int    OC3CE   : 1;    ///< Reserved
      } regOut;
        
      /// bitwise access to register (input mode)
      struct {
        int    CC3S    : 2;    ///< Capture/compare 3 selection
        int    IC3PSC  : 2;    ///< Input capture 3 prescaler
        int    IC3F    : 4;    ///< Input capture 3 filter
      } regIn;

    } CCMR3;


    /** TIM2 Capture/compare enable register 1 (TIM2_CCER1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CC1E    : 1;    ///< Capture/compare 1 output enable
        int    CC1P    : 1;    ///< Capture/compare 1 output polarity
        int    res     : 2;    ///< Reserved
        int    CC2E    : 1;    ///< Capture/compare 2 output enable
        int    CC2P    : 1;    ///< Capture/compare 2 output polarity
        int    res2    : 2;    ///< Reserved
      } Reg;
      
    } CCER1;


    /** TIM2 Capture/compare enable register 2 (TIM2_CCER2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CC3E    : 1;    ///< Capture/compare 3 output enable
        int    CC3P    : 1;    ///< Capture/compare 3 output polarity
        int    res     : 6;    ///< Reserved
      } Reg;
      
    } CCER2;


    /** TIM2 16-bit counter (TIM2_CNTR) */
    word_t        CNTR;


    /** TIM2 clock prescaler (TIM2_PSCR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PSC     : 4;    ///< clock prescaler
        int    res     : 4;    ///< Reserved
      } Reg;
      
    } PSCR;
    
    
    /** TIM2 16-bit re-load value (TIM2_ARR) */
    word_t        ARR;


    /** TIM2 16-bit capture/compare value 1 (TIM2_CCR1) */
    word_t        CCR1;


    /** TIM2 16-bit capture/compare value 2 (TIM2_CC2R) */
    word_t        CCR2;


    /** TIM2 16-bit capture/compare value 3 (TIM2_CC3R) */
    word_t        CCR3;

  } TIM2_t;

  /// pointer to all TIM2 registers (selected devices)
  #if defined(TIM2_BaseAddress)
    #define _TIM2  (*((TIM2_t *) TIM2_BaseAddress))
  #endif

  /* TIM2 Module Reset Values */
  #define TIM2_CR1_RESET_VALUE   ((uint8_t)0x00)
  #define TIM2_IER_RESET_VALUE   ((uint8_t)0x00)
  #define TIM2_SR1_RESET_VALUE   ((uint8_t)0x00)
  #define TIM2_SR2_RESET_VALUE   ((uint8_t)0x00)
  #define TIM2_EGR_RESET_VALUE   ((uint8_t)0x00)
  #define TIM2_CCMR1_RESET_VALUE ((uint8_t)0x00)
  #define TIM2_CCMR2_RESET_VALUE ((uint8_t)0x00)
  #define TIM2_CCMR3_RESET_VALUE ((uint8_t)0x00)
  #define TIM2_CCER1_RESET_VALUE ((uint8_t)0x00)
  #define TIM2_CCER2_RESET_VALUE ((uint8_t)0x00)
  #define TIM2_CNTRH_RESET_VALUE ((uint8_t)0x00)
  #define TIM2_CNTRL_RESET_VALUE ((uint8_t)0x00)
  #define TIM2_PSCR_RESET_VALUE  ((uint8_t)0x00)
  #define TIM2_ARRH_RESET_VALUE  ((uint8_t)0xFF)
  #define TIM2_ARRL_RESET_VALUE  ((uint8_t)0xFF)
  #define TIM2_CCR1H_RESET_VALUE ((uint8_t)0x00)
  #define TIM2_CCR1L_RESET_VALUE ((uint8_t)0x00)
  #define TIM2_CCR2H_RESET_VALUE ((uint8_t)0x00)
  #define TIM2_CCR2L_RESET_VALUE ((uint8_t)0x00)
  #define TIM2_CCR3H_RESET_VALUE ((uint8_t)0x00)
  #define TIM2_CCR3L_RESET_VALUE ((uint8_t)0x00)

#endif // (1)



//------------------------
// 16-Bit Timer TIM3 (selected devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing TIM3 registers */
  typedef struct {

    /** TIM3 Control register (TIM3_CR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CEN     : 1;    ///< Counter enable
        int    UDIS    : 1;    ///< Update disable
        int    URS     : 1;    ///< Update request source
        int    OPM     : 1;    ///< One-pulse mode
        int    res     : 3;    ///< Reserved
        int    ARPE    : 1;    ///< Auto-reload preload enable
      } Reg;
      
    } CR1;


    /** TIM3 Interrupt enable (TIM3_IER) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UIE     : 1;    ///< Update interrupt enable
        int    CC1IE   : 1;    ///< Capture/compare 1 interrupt enable
        int    CC2IE   : 1;    ///< Capture/compare 2 interrupt enable
        int    res     : 5;    ///< Reserved 
      } Reg;
      
    } IER;


    /** TIM3 Status register 1 (TIM3_SR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UIF     : 1;    ///< Update interrupt flag
        int    CC1IF   : 1;    ///< Capture/compare 1 interrupt flag
        int    CC2IF   : 1;    ///< Capture/compare 2 interrupt flag
        int    res     : 5;    ///< Reserved
      } Reg;
      
    } SR1;


    /** TIM3 Status register 2 (TIM3_SR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    res     : 1;    ///< Reserved
        int    CC1OF   : 1;    ///< Capture/compare 1 overcapture flag
        int    CC2OF   : 1;    ///< Capture/compare 2 overcapture flag
        int    res2    : 5;    ///< Reserved
      } Reg;
      
    } SR2;


    /** TIM3 Event Generation (TIM3_EGR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UG      : 1;    ///< Update generation
        int    CC1G    : 1;    ///< Capture/compare 1 generation
        int    CC2G    : 1;    ///< Capture/compare 2 generation
        int    res     : 5;    ///< Reserved
      } Reg;
      
    } EGR;


    /** TIM3 Capture/compare mode register 1 (TIM3_CCMR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register (output mode)
      struct {
        int    CC1S    : 2;    ///< Capture/compare 1 selection
        int    res     : 1;    ///< Reserved
        int    OC1PE   : 1;    ///< Output compare 1 preload enable
        int    OC1M    : 3;    ///< Output compare 1 mode
        int    res2    : 1;    ///< Reserved
      } regOut;
      
      /// bitwise access to register (input mode)
      struct {
        int    CC1S    : 2;    ///< Capture/compare 1 selection
        int    IC1PSC  : 2;    ///< Input capture 1 prescaler
        int    IC1F    : 4;    ///< Input capture 1 filter
      } regIn;
      
    } CCMR1;


    /** TIM3 Capture/compare mode register 2 (TIM3_CCMR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register (output mode)
      struct {
        int    CC2S    : 2;    ///< Capture/compare 2 selection
        int    res     : 1;    ///< Reserved
        int    OC2PE   : 1;    ///< Output compare 2 preload enable
        int    OC2M    : 3;    ///< Output compare 2 mode
        int    res2    : 1;    ///< Reserved
      } regOut;
      
      /// bitwise access to register (input mode)
      struct {
        int    CC2S    : 2;    ///< Capture/compare 2 selection
        int    IC2PSC  : 2;    ///< Input capture 2 prescaler
        int    IC2F    : 4;    ///< Input capture 2 filter
      } regIn;
      
    } CCMR2;


    /** TIM3 Capture/compare enable register 1 (TIM3_CCER1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CC1E    : 1;    ///< Capture/compare 1 output enable
        int    CC1P    : 1;    ///< Capture/compare 1 output polarity
        int    res     : 2;    ///< Reserved
        int    CC2E    : 1;    ///< Capture/compare 2 output enable
        int    CC2P    : 1;    ///< Capture/compare 2 output polarity
        int    res2    : 2;    ///< Reserved
      } Reg;
      
    } CCER1;


    /** TIM3 16-bit counter (TIM3_CNTR) */
    word_t        CNTR;


    /** TIM3 clock prescaler (TIM3_PSCR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PSC     : 4;    ///< clock prescaler
        int    res     : 4;    ///< Reserved
      } Reg;
      
    } PSCR;
    
    
    /** TIM3 16-bit re-load value (TIM3_ARR) */
    word_t        ARR;


    /** TIM3 16-bit capture/compare value 1 (TIM3_CCR1) */
    word_t        CCR1;


    /** TIM3 16-bit capture/compare value 2 (TIM3_CCR2) */
    word_t        CCR2;

  } TIM3_t;

  /// pointer to all TIM3 registers (selected devices)
  #if defined(TIM3_BaseAddress)
    #define _TIM3  (*((TIM3_t *) TIM3_BaseAddress))
  #endif

  /* TIM3 Module Reset Values */
  #define TIM3_CR1_RESET_VALUE   ((uint8_t)0x00)
  #define TIM3_IER_RESET_VALUE   ((uint8_t)0x00)
  #define TIM3_SR1_RESET_VALUE   ((uint8_t)0x00)
  #define TIM3_SR2_RESET_VALUE   ((uint8_t)0x00)
  #define TIM3_EGR_RESET_VALUE   ((uint8_t)0x00)
  #define TIM3_CCMR1_RESET_VALUE ((uint8_t)0x00)
  #define TIM3_CCMR2_RESET_VALUE ((uint8_t)0x00)
  #define TIM3_CCER1_RESET_VALUE ((uint8_t)0x00)
  #define TIM3_CNTRH_RESET_VALUE ((uint8_t)0x00)
  #define TIM3_CNTRL_RESET_VALUE ((uint8_t)0x00)
  #define TIM3_PSCR_RESET_VALUE  ((uint8_t)0x00)
  #define TIM3_ARRH_RESET_VALUE  ((uint8_t)0xFF)
  #define TIM3_ARRL_RESET_VALUE  ((uint8_t)0xFF)
  #define TIM3_CCR1H_RESET_VALUE ((uint8_t)0x00)
  #define TIM3_CCR1L_RESET_VALUE ((uint8_t)0x00)
  #define TIM3_CCR2H_RESET_VALUE ((uint8_t)0x00)
  #define TIM3_CCR2L_RESET_VALUE ((uint8_t)0x00)

#endif // (1)



//------------------------
// 8-Bit Timer TIM4 (selected devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing TIM4 registers (selected devices) */
  typedef struct {
  
    /** TIM4 Control register (TIM4_CR1) */
    union {
    
      /// bytewise access to register
      uint8_t  Byte;  
    
      /// bitwise access to register
      struct {
        int    CEN     : 1;    ///< Counter enable
        int    UDIS    : 1;    ///< Update disable
        int    URS     : 1;    ///< Update request source
        int    OPM     : 1;    ///< One-pulse mode
        int    res     : 3;    ///< Reserved
        int    ARPE    : 1;    ///< Auto-reload preload enable
      } Reg;
    
    } CR1;
  
  
    /** Reserved registers on selected devices (2B) */
    #if defined(STM8S103) || defined(STM8S003) || defined(STM8S001)
      uint8_t         res[2]; 
    #endif


    /** TIM4 Interrupt enable (TIM4_IER) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UIE     : 1;    ///< Update interrupt enable
        int    res     : 7;    ///< Reserved
      } Reg;
      
    } IER;


    /** TIM4 Status register (TIM4_SR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UIF     : 1;    ///< Update interrupt flag
        int    res     : 7;    ///< Reserved
      } Reg;
      
    } SR1;
  
  
    /** TIM4 Event Generation (TIM4_EGR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UG      : 1;    ///< Update generation
        int    res     : 7;    ///< Reserved
      } Reg;
      
    } EGR;
  
  
    /** TIM4 8-bit counter register (TIM4_CNTR) */
    byte_t            CNTR;
  
  
    /** TIM4 clock prescaler (TIM4_PSCR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PSC     : 3;    ///< clock prescaler
        int    res     : 5;    ///< Reserved
      } Reg;
      
    } PSCR;
  
  
    /** TIM4 8-bit auto-reload register (TIM4_ARR) */
    byte_t            ARR;
  
  } TIM4_t;
  
  /// pointer to TIM4 registers
  #if defined(TIM4_BaseAddress)
    #define _TIM4  (*((TIM4_t *) TIM4_BaseAddress))
  #endif

  /* TIM4 Module Reset Values */
  #define TIM4_CR1_RESET_VALUE  ((uint8_t)0x00)
  #define TIM4_IER_RESET_VALUE  ((uint8_t)0x00)
  #define TIM4_SR1_RESET_VALUE  ((uint8_t)0x00)
  #define TIM4_EGR_RESET_VALUE  ((uint8_t)0x00)
  #define TIM4_CNTR_RESET_VALUE ((uint8_t)0x00)
  #define TIM4_PSCR_RESET_VALUE ((uint8_t)0x00)
  #define TIM4_ARR_RESET_VALUE  ((uint8_t)0xFF)

#endif // (1)



//------------------------
// 16-Bit Timer TIM5 (selected devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing TIM5 registers */
  typedef struct {

    /** TIM5 Control register (TIM5_CR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CEN     : 1;    ///< Counter enable
        int    UDIS    : 1;    ///< Update disable
        int    URS     : 1;    ///< Update request source
        int    OPM     : 1;    ///< One-pulse mode
        int    res     : 3;    ///< Reserved
        int    ARPE    : 1;    ///< Auto-reload preload enable
      } Reg;
      
    } CR1;


    /** TIM5 Control register 2 (TIM5_CR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CCPC    : 1;    ///< Capture/compare preloaded control
        int    res     : 1;    ///< Reserved, forced by hardware to 0
        int    COMS    : 1;    ///< Capture/compare control update selection
        int    res2    : 1;    ///< Reserved, must be kept cleared
        int    MMS     : 3;    ///< Master mode selection
        int    res3    : 1;    ///< Reserved
      } Reg;
      
    } CR2;


    /** TIM5 Slave mode control register (TIM5_SMCR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    SMS     : 3;    ///< Clock/trigger/slave mode selection
        int    res     : 1;    ///< Reserved
        int    TS      : 3;    ///< Trigger selection
        int    MSM     : 1;    ///< Master/slave mode
      } Reg;
      
    } SMCR;


    /** TIM5 Interrupt enable (TIM5_IER) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UIE     : 1;    ///< Update interrupt enable
        int    CC1IE   : 1;    ///< Capture/compare 1 interrupt enable
        int    CC2IE   : 1;    ///< Capture/compare 2 interrupt enable
        int    CC3IE   : 1;    ///< Capture/compare 3 interrupt enable
        int    res     : 2;    ///< Reserved 
        int    TIE     : 1;    ///< Trigger interrupt enable
        int    res2    : 1;    ///< Reserved
      } Reg;
      
    } IER;


    /** TIM5 Status register 1 (TIM5_SR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UIF     : 1;    ///< Update interrupt flag
        int    CC1IF   : 1;    ///< Capture/compare 1 interrupt flag
        int    CC2IF   : 1;    ///< Capture/compare 2 interrupt flag
        int    CC3IF   : 1;    ///< Capture/compare 3 interrupt flag
        int    res     : 2;    ///< Reserved
        int    TIF     : 1;    ///< Trigger interrupt flag
        int    res2    : 1;    ///< Reserved
      } Reg;
      
    } SR1;


    /** TIM5 Status register 2 (TIM5_SR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    res     : 1;    ///< Reserved
        int    CC1OF   : 1;    ///< Capture/compare 1 overcapture flag
        int    CC2OF   : 1;    ///< Capture/compare 2 overcapture flag
        int    CC3OF   : 1;    ///< Capture/compare 3 overcapture flag
        int    res2    : 4;    ///< Reserved
      } Reg;
      
    } SR2;


    /** TIM5 Event Generation (TIM5_EGR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UG      : 1;    ///< Update generation
        int    CC1G    : 1;    ///< Capture/compare 1 generation
        int    CC2G    : 1;    ///< Capture/compare 2 generation
        int    CC3G    : 1;    ///< Capture/compare 3 generation
        int    res     : 2;    ///< Reserved
        int    TG      : 1;    ///< Trigger generation
        int    res2    : 1;    ///< Reserved
      } Reg;
      
    } EGR;


    /** TIM5 Capture/compare mode register 1 (TIM5_CCMR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register (output mode)
      struct {
        int    CC1S    : 2;    ///< Capture/compare 1 selection
        int    res     : 1;    ///< Reserved
        int    OC1PE   : 1;    ///< Output compare 1 preload enable
        int    OC1M    : 3;    ///< Output compare 1 mode
        int    res2    : 1;    ///< Reserved
      } regOut;
      
      /// bitwise access to register (input mode)
      struct {
        int    CC1S    : 2;    ///< Capture/compare 1 selection
        int    IC1PSC  : 2;    ///< Input capture 1 prescaler
        int    IC1F    : 4;    ///< Input capture 1 filter
      } regIn;
      
    } CCMR1;


    /** TIM5 Capture/compare mode register 2 (TIM5_CCMR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register (output mode)
      struct {
        int    CC2S    : 2;    ///< Capture/compare 2 selection
        int    res     : 1;    ///< Reserved
        int    OC2PE   : 1;    ///< Output compare 2 preload enable
        int    OC2M    : 3;    ///< Output compare 2 mode
        int    res2    : 1;    ///< Reserved
      } regOut;
      
      /// bitwise access to register (input mode)
      struct {
        int    CC2S    : 2;    ///< Capture/compare 2 selection
        int    IC2PSC  : 2;    ///< Input capture 2 prescaler
        int    IC2F    : 4;    ///< Input capture 2 filter
      } regIn;
      
    } CCMR2;


    /** TIM5 Capture/compare mode register 3 (TIM5_CCMR3) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register (output mode)
      struct {
        int    CC3S    : 2;    ///< Capture/compare 3 selection
        int    res     : 1;    ///< Reserved
        int    OC3PE   : 1;    ///< Output compare 3 preload enable
        int    OC3M    : 3;    ///< Output compare 3 mode
        int    OC3CE   : 1;    ///< Reserved
      } regOut;
        
      /// bitwise access to register (input mode)
      struct {
        int    CC3S    : 2;    ///< Capture/compare 3 selection
        int    IC3PSC  : 2;    ///< Input capture 3 prescaler
        int    IC3F    : 4;    ///< Input capture 3 filter
      } regIn;

    } CCMR3;


    /** TIM5 Capture/compare enable register 1 (TIM5_CCER1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CC1E    : 1;    ///< Capture/compare 1 output enable
        int    CC1P    : 1;    ///< Capture/compare 1 output polarity
        int    res     : 2;    ///< Reserved
        int    CC2E    : 1;    ///< Capture/compare 2 output enable
        int    CC2P    : 1;    ///< Capture/compare 2 output polarity
        int    res2    : 2;    ///< Reserved
      } Reg;
      
    } CCER1;


    /** TIM5 Capture/compare enable register 2 (TIM5_CCER2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CC3E    : 1;    ///< Capture/compare 3 output enable
        int    CC3P    : 1;    ///< Capture/compare 3 output polarity
        int    res     : 6;    ///< Reserved
      } Reg;
      
    } CCER2;


    /** TIM5 16-bit counter (TIM5_CNTR) */
    word_t        CNTR;


    /** TIM5 clock prescaler (TIM5_PSCR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    PSC     : 4;    ///< clock prescaler
        int    res     : 4;    ///< Reserved
      } Reg;
      
    } PSCR;


    /** TIM5 16-bit re-load value (TIM5_ARR) */
    word_t        ARR;


    /** TIM5 16-bit capture/compare value 1 (TIM5_CCR1) */
    word_t        CCR1;


    /** TIM5 16-bit capture/compare value 2 (TIM5_CCR2) */
    word_t        CCR2;


    /** TIM5 16-bit capture/compare value 3 (TIM5_CCR3) */
    word_t        CCR3;

  } TIM5_t;

  /// pointer to all TIM5 registers (selected devices)
  #if defined(TIM5_BaseAddress)
    #define _TIM5  (*((TIM5_t *) TIM5_BaseAddress))
  #endif

  /* TIM5 Module Reset Values */
  #define TIM5_CR1_RESET_VALUE   ((uint8_t)0x00)
  #define TIM5_CR2_RESET_VALUE   ((uint8_t)0x00)
  #define TIM5_SMCR_RESET_VALUE  ((uint8_t)0x00)
  #define TIM5_IER_RESET_VALUE   ((uint8_t)0x00)
  #define TIM5_SR1_RESET_VALUE   ((uint8_t)0x00)
  #define TIM5_SR2_RESET_VALUE   ((uint8_t)0x00)
  #define TIM5_EGR_RESET_VALUE   ((uint8_t)0x00)
  #define TIM5_CCMR1_RESET_VALUE ((uint8_t)0x00)
  #define TIM5_CCMR2_RESET_VALUE ((uint8_t)0x00)
  #define TIM5_CCMR3_RESET_VALUE ((uint8_t)0x00)
  #define TIM5_CCER1_RESET_VALUE ((uint8_t)0x00)
  #define TIM5_CCER2_RESET_VALUE ((uint8_t)0x00)
  #define TIM5_CNTRH_RESET_VALUE ((uint8_t)0x00)
  #define TIM5_CNTRL_RESET_VALUE ((uint8_t)0x00)
  #define TIM5_PSCR_RESET_VALUE  ((uint8_t)0x00)
  #define TIM5_ARRH_RESET_VALUE  ((uint8_t)0xFF)
  #define TIM5_ARRL_RESET_VALUE  ((uint8_t)0xFF)
  #define TIM5_CCR1H_RESET_VALUE ((uint8_t)0x00)
  #define TIM5_CCR1L_RESET_VALUE ((uint8_t)0x00)
  #define TIM5_CCR2H_RESET_VALUE ((uint8_t)0x00)
  #define TIM5_CCR2L_RESET_VALUE ((uint8_t)0x00)
  #define TIM5_CCR3H_RESET_VALUE ((uint8_t)0x00)
  #define TIM5_CCR3L_RESET_VALUE ((uint8_t)0x00)

#endif // (1) 



//------------------------
// 8-Bit Timer TIM6 (selected devices)
//------------------------
#if (1)   // dummy for filtering out in editor

  /** struct containing TIM6 registers (selected devices) */
  typedef struct {

    /** TIM6 Control register (TIM6_CR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    CEN     : 1;    ///< Counter enable
        int    UDIS    : 1;    ///< Update disable
        int    URS     : 1;    ///< Update request source
        int    OPM     : 1;    ///< One-pulse mode
        int    res     : 3;    ///< Reserved
        int    ARPE    : 1;    ///< Auto-reload preload enable
      } Reg;
      
    } CR1;


    /** TIM6 Control register 2 (TIM6_CR2) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    res     : 4;    ///< Reserved
        int    MMS     : 3;    ///< Master mode selection
        int    res3    : 1;    ///< Reserved
      } Reg;
      
    } CR2;


    /// Slave mode control register (TIM6_SMCR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    SMS     : 3;    ///< Clock/trigger/slave mode selection
        int    res     : 1;    ///< Reserved
        int    TS      : 3;    ///< Trigger selection
        int    MSM     : 1;    ///< Master/slave mode
      } Reg;
      
    } SMCR;


    /** TIM6 Interrupt enable (TIM6_IER) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UIE     : 1;    ///< Update interrupt enable
        int    res     : 7;    ///< Reserved
      } Reg;
      
    } IER;


    /** TIM6 Status register (TIM6_SR1) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UIF     : 1;    ///< Update interrupt flag
        int    res     : 7;    ///< Reserved
      } Reg;
      
    } SR1;


    /** TIM6 Event Generation (TIM6_EGR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        int    UG      : 1;    ///< Update generation
        int    res     : 7;    ///< Reserved
      } Reg;
      
    } EGR;


    /** TIM6 8-bit counter (TIM6_CNTR) */
    byte_t        CNTR;


    /** TIM6 clock prescaler (TIM6_PSCR) */
    union {
      
      /// bytewise access to register
      uint8_t  Byte;
      
      /// bitwise access to register
      struct {
        uint8_t PSC     : 3;    ///< clock prescaler
        uint8_t res     : 5;    ///< Reserved
      } Reg;
      
    } PSCR;


    /** TIM6 8-bit auto-reload register (TIM6_ARR) */
    byte_t        ARR;

  } TIM6_t;

  /// pointer to all TIM6 registers (selected devices)
  #if defined(TIM6_BaseAddress)
    #define _TIM6  (*((TIM6_t *) TIM6_BaseAddress))
  #endif
  
  /* TIM6 Module Reset Values */
  #define TIM6_CR1_RESET_VALUE    ((uint8_t)0x00)
  #define TIM6_CR2_RESET_VALUE    ((uint8_t)0x00)
  #define TIM6_SMCR_RESET_VALUE   ((uint8_t)0x00)
  #define TIM6_IER_RESET_VALUE    ((uint8_t)0x00)
  #define TIM6_SR1_RESET_VALUE    ((uint8_t)0x00)
  #define TIM6_EGR_RESET_VALUE    ((uint8_t)0x00)
  #define TIM6_CNTR_RESET_VALUE   ((uint8_t)0x00)
  #define TIM6_PSCR_RESET_VALUE   ((uint8_t)0x00)
  #define TIM6_ARR_RESET_VALUE    ((uint8_t)0xFF)

#endif // (1) 


/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _STM8AF_STM8S_H_
