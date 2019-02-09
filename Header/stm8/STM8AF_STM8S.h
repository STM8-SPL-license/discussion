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
#define STM8AF_STM8S_H   1

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
    uint8_t   ODR;                ///< Port x output data register (Px_ODR)
    uint8_t   IDR;                ///< Port x pin input register (Px_IDR)
    uint8_t   DDR;                ///< Port x data direction register (Px_DDR)
    uint8_t   CR1;                ///< Port x control register 1 (Px_CR1)
    uint8_t   CR2;                ///< Port x control register 2 (Px_CR2)
  } PORT_t;

  // port A..F implemented on all devices
  #if defined(PORTA_BaseAddress)
    #define _GPIOA      SFR(PORT_t, PORTA_BaseAddress)  ///< registers for port A access
  #endif
  #if defined(PORTB_BaseAddress)
    #define _GPIOB      SFR(PORT_t, PORTB_BaseAddress)  ///< registers for port B access
  #endif
  #if defined(PORTC_BaseAddress)
    #define _GPIOC      SFR(PORT_t, PORTC_BaseAddress)  ///< registers for port C access
  #endif
  #if defined(PORTD_BaseAddress)
    #define _GPIOD      SFR(PORT_t, PORTD_BaseAddress)  ///< registers for port D access
  #endif
  #if defined(PORTE_BaseAddress)
    #define _GPIOE      SFR(PORT_t, PORTE_BaseAddress)  ///< registers for port E access
  #endif
  #if defined(PORTF_BaseAddress)
    #define _GPIOF      SFR(PORT_t, PORTF_BaseAddress)  ///< registers for port F access
  #endif
  #if defined(PORTG_BaseAddress)
    #define _GPIOG      SFR(PORT_t, PORTG_BaseAddress)  ///< registers for port G access
  #endif
  #if defined(PORTH_BaseAddress)
    #define _GPIOH      SFR(PORT_t, PORTH_BaseAddress)  ///< registers for port H access
  #endif
  #if defined(PORTI_BaseAddress)
    #define _GPIOI      SFR(PORT_t, PORTI_BaseAddress)  ///< registers for port I access
  #endif

  /* PORT Module Reset Values (all ports) */
  #define PORT_ODR_RESET_VALUE ((uint8_t)0x00)
  #define PORT_DDR_RESET_VALUE ((uint8_t)0x00)
  #define PORT_CR1_RESET_VALUE ((uint8_t)0x00)
  #define PORT_CR2_RESET_VALUE ((uint8_t)0x00)

  /* macro for accessing pins (all ports) */
  #define PIN0   ((uint8_t) (0x01 << 0))
  #define PIN1   ((uint8_t) (0x01 << 1))
  #define PIN2   ((uint8_t) (0x01 << 2))
  #define PIN3   ((uint8_t) (0x01 << 3))
  #define PIN4   ((uint8_t) (0x01 << 4))
  #define PIN5   ((uint8_t) (0x01 << 5))
  #define PIN6   ((uint8_t) (0x01 << 6))
  #define PIN7   ((uint8_t) (0x01 << 7))

#endif // PORTx_BaseAddress



//------------------------
// Clock module CLK (implemented on all devices)
//------------------------
#if defined(CLK_BaseAddress)

  /** struct containing clock module registers (CLK) */
  typedef struct {

    uint8_t   ICKR;               ///< Internal clock register
    uint8_t   ECKR;               ///< External clock register
    uint8_t   res;                ///< Reserved register (1B)
    uint8_t   CMSR;               ///< Clock master status register
    uint8_t   SWR;                ///< Clock master switch register
    uint8_t   SWCR;               ///< Switch control register
    uint8_t   CKDIVR;             ///< Clock divider register
    uint8_t   PCKENR1;            ///< Peripheral clock gating register 1
    uint8_t   CSSR;               ///< Clock security system register
    uint8_t   CCOR;               ///< Configurable clock output register
    uint8_t   PCKENR2;            ///< Peripheral clock gating register 2
    uint8_t   res1;               ///< Reserved register (1B)
    uint8_t   HSITRIMR;           ///< HSI clock calibration trimming register
    uint8_t   SWIMCCR;            ///< SWIM clock control register

  } CLK_t;

  /// pointer to CLK registers
  #define _CLK        SFR(CLK_t, CLK_BaseAddress)
  
  /* CLK Module Reset Values */
  #define _CLK_ICKR_RESET_VALUE     ((uint8_t) 0x01)
  #define _CLK_ECKR_RESET_VALUE     ((uint8_t) 0x00)
  #define _CLK_CMSR_RESET_VALUE     ((uint8_t) 0xE1)
  #define _CLK_SWR_RESET_VALUE      ((uint8_t) 0xE1)
  #define _CLK_SWCR_RESET_VALUE     ((uint8_t) 0x00)
  #define _CLK_CKDIVR_RESET_VALUE   ((uint8_t) 0x18)
  #define _CLK_PCKENR1_RESET_VALUE  ((uint8_t) 0xFF)
  #define _CLK_PCKENR2_RESET_VALUE  ((uint8_t) 0xFF)
  #define _CLK_CSSR_RESET_VALUE     ((uint8_t) 0x00)
  #define _CLK_CCOR_RESET_VALUE     ((uint8_t) 0x00)
  #define _CLK_HSITRIMR_RESET_VALUE ((uint8_t) 0x00)
  #define _CLK_SWIMCCR_RESET_VALUE  ((uint8_t) 0x00)

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
// UART2 Module (selected devices) with
//    asyncronous & synchronous mode
//    multiprocessor communication
//    SmartCard & IrDA mode
//    LIN master & slave mode
//------------------------
#if defined(UART2_BaseAddress)

  /** struct containing UART2 registers (selected devices) */
  typedef struct   {

    uint8_t   SR;                 ///< UART2 Status register
    uint8_t   DR;                 ///< UART2 data register
    uint8_t   BRR1;               ///< UART2 Baud rate register 1
    uint8_t   BRR2;               ///< UART2 Baud rate register 2
    uint8_t   CR1;                ///< UART2 Control register 1
    uint8_t   CR2;                ///< UART2 Control register 2
    uint8_t   CR3;                ///< UART2 Control register 3
    uint8_t   CR4;                ///< UART2 Control register 4
    uint8_t   CR5;                ///< UART2 Control register 5
    uint8_t   CR6;                ///< UART2 Control register 6
    uint8_t   GTR;                ///< UART2 guard time register register
    uint8_t   PSCR;               ///< UART2 prescaler register register register

  } UART2_t;

  /// pointer to UART2 registers
  #define _UART2      SFR(UART2_t, UART2_BaseAddress)
  
  /* UART2 Module Reset Values */
  #define _UART2_SR_RESET_VALUE   ((uint8_t) 0xC0)
  #define _UART2_BRR1_RESET_VALUE ((uint8_t) 0x00)
  #define _UART2_BRR2_RESET_VALUE ((uint8_t) 0x00)
  #define _UART2_CR1_RESET_VALUE  ((uint8_t) 0x00)
  #define _UART2_CR2_RESET_VALUE  ((uint8_t) 0x00)
  #define _UART2_CR3_RESET_VALUE  ((uint8_t) 0x00)
  #define _UART2_CR4_RESET_VALUE  ((uint8_t) 0x00)
  #define _UART2_CR5_RESET_VALUE  ((uint8_t) 0x00)
  #define _UART2_CR6_RESET_VALUE  ((uint8_t) 0x00)
  #define _UART2_GTR_RESET_VALUE  ((uint8_t) 0x00)
  #define _UART2_PSCR_RESET_VALUE ((uint8_t) 0x00)

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
// 8-Bit Timer TIM4 (selected devices)
//------------------------
#if defined(TIM4_BaseAddress)

  /** struct containing UART2 registers (selected devices) */
  typedef struct   {

    uint8_t   CR1;                ///< Control register
    #if defined(STM8S103) || defined(STM8S003)
      uint8_t res[2];             // reserved (2B)
    #endif
    uint8_t   IER;                ///< Interrupt enable register
    uint8_t   SR;                 ///< Status register
    uint8_t   EGR;                ///< Event generation register
    uint8_t   CNTR;               ///< Counter register
    uint8_t   PSCR;               ///< Prescaler register
    uint8_t   ARR;                ///< Auto-reload register

  } TIM4_t;

  /// pointer to TIM4 registers
  #define _TIM4       SFR(TIM4_t, TIM4_BaseAddress)  

  /* TIM4 Module Reset Values */
  #define _TIM4_CR1_RESET_VALUE  ((uint8_t)0x00)
  #define _TIM4_IER_RESET_VALUE  ((uint8_t)0x00)
  #define _TIM4_SR1_RESET_VALUE  ((uint8_t)0x00)
  #define _TIM4_EGR_RESET_VALUE  ((uint8_t)0x00)
  #define _TIM4_CNTR_RESET_VALUE ((uint8_t)0x00)
  #define _TIM4_PSCR_RESET_VALUE ((uint8_t)0x00)
  #define _TIM4_ARR_RESET_VALUE  ((uint8_t)0xFF)

  /* TIM4 Control register (TIM4_CR1) */
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


/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _STM8AF_STM8S_H_
