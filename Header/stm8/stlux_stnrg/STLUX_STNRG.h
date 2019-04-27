/** \addtogroup STLUX_STNRG
 *  @{
 */

/*-------------------------------------------------------------------------
  STLUX_STNRG.h - Register Declarations for STLUX & STNRG family

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
#ifndef STLUX_STNRG_H
#define STLUX_STNRG_H

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
  #error in 'STLUX_STNRG.h': compiler not supported
#endif


/*-----------------------------------------------------------------------------
    MEMORY WIDTH
-----------------------------------------------------------------------------*/

// if memory sizes [B] are not given, assume smallest available in family
#if !defined(STM8_PFLASH_SIZE)
  #warning undefined STM8_PFLASH_SIZE, assume minimum
  #define STM8_PFLASH_SIZE      32768         ///< size of program flash [B]
#endif
#if !defined(STM8_RAM_SIZE)
  #warning undefined STM8_RAM_SIZE, assume minimum
  #define STM8_RAM_SIZE         2048          ///< size of RAM [B]
#endif
#if !defined(STM8_EEPROM_SIZE)
  #warning undefined STM8_EEPROM_SIZE, assume minimum
  #define STM8_EEPROM_SIZE      1024          ///< size of data EEPROM [B]
#endif

// memory start / end addresses
#define STM8_PFLASH_START       0x8000                                      ///< first address in program flash
#define STM8_PFLASH_END         (STM8_PFLASH_START + STM8_PFLASH_SIZE - 1)  ///< last address in program flash
#define STM8_RAM_START          0x0000                                      ///< first address in RAM
#define STM8_RAM_END            (STM8_RAM_START + STM8_RAM_SIZE - 1)        ///< last address in RAM
#define STM8_EEPROM_START       0x4000                                      ///< first address in EEPROM
#define STM8_EEPROM_END         (STM8_EEPROM_START + STM8_EEPROM_SIZE - 1)  ///< last address in EEPROM (part of P-flash, configure via option byte)

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
  #define SW_RESET()             (_WWDG_CR = _WWDG_CR_WDGA)           ///< reset controller via WWDG module

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
  #define SW_RESET()             (_WWDG_CR = _WWDG_CR_WDGA)           ///< reset controller via WWDG module

  // data type in bit fields
  #define _BITS                  unsigned int                         ///< data type in bit structs (follow C90 standard)

// IAR Compiler
#elif defined(_IAR_)
input mo
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
  #define SW_RESET()             (_WWDG_CR = _WWDG_CR_WDGA)           ///< reset controller via WWDG module

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
  #define SW_RESET()             (_WWDG_CR = _WWDG_CR_WDGA)           ///< reset controller via WWDG module

  // data type in bit fields
  #define _BITS                  unsigned int                         ///< data type in bit structs (follow C90 standard)

#endif


/*-----------------------------------------------------------------------------
    DEFINITION OF GLOBAL MACROS/#DEFINES
-----------------------------------------------------------------------------*/

// general macros
#define _SFR(type, addr)       (*((volatile type*) (addr)))           ///< peripheral register
#if defined(_DOXYGEN) || defined(UID_AddressBase)
  #define _UID(N)              _SFR(uint8_t,  UID_AddressBase+N)      ///< read unique identifier byte N
#endif
#if defined(_DOXYGEN) || defined(DEVID_AddressBase)
  #define _DEVID(N)            _SFR(uint8_t,  DEVID_AddressBase+N)    ///< read device identifier byte N
#endif


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
#define __AWU_VECTOR__            1       ///< irq1 - Auto Wake Up from Halt interrupt (AWU)
#define __CLK_VECTOR__            2       ///< irq2 - Clock controller interrupt
#define __PORT0_VECTOR__          3       ///< irq3 - External interrupt port 0
#define __AUXTIM_VECTOR__         4       ///< irq4 - Auxiliary timer interrupt
#define __PORT2_VECTOR__          5       ///< irq5 - External interrupt port 2
#define __SMED0_VECTOR__          6       ///< irq6 - SMED-0 interrupt
#define __SMED1_VECTOR__          7       ///< irq7 - SMED-1 control logic interrupt
// reserved                       8
// reserved                       9
// reserved                       10
// reserved                       11
// reserved                       12
// reserved                       13
// reserved                       14
#define __SMED2_VECTOR__          15      ///< irq15 - SMED-2 control logic interrupt
#define __SMED3_VECTOR__          16      ///< irq16 - SMED-3 control logic interrupt
#define __UART_TXE_VECTOR__       17      ///< irq17 - UART send (TX empty) interrupt
#define __UART_RXF_VECTOR__       18      ///< irq18 - UART receive (RX full) interrupt
#define __I2C_VECTOR__            19      ///< irq19 - I2C interrupt
// reserved                       20
// reserved                       21
#define __ADC_VECTOR__            22      ///< irq22 - ADC end of conversion interrupt
#define __SYSTIM_UPD_OVF_VECTOR__ 23      ///< irq23 - system timer Update/overflow interrupt
#define __FLASH_VECTOR__          24      ///< irq24 - flash interrupt
#define __DALI_VECTOR__           25      ///< irq25 - DALI interrupt line
#define __SMED4_VECTOR__          26      ///< irq26 - SMED-3 control logic interrupt
#define __SMED5_VECTOR__          27      ///< irq27 - SMED-4 control logic interrupt
// reserved                       28
// reserved                       29


/*-----------------------------------------------------------------------------
    DEFINITION OF STM8 PERIPHERAL REGISTERS
-----------------------------------------------------------------------------*/

//------------------------
// General purpose input/output pins (_PORT)
//------------------------
#if defined(_DOXYGEN) || defined(PORT0_AddressBase) || defined(PORT1_AddressBase)

  /** @brief structure for controlling pins in port mode (_PORTx, x=0..1) */
  typedef struct {

    /** @brief PORTx output data register (_PORTx_ODR) */
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

    /** @brief PORTx input data register (_PORTx_IDR) */
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

    /** @brief PORTx data direction data register (_PORTx_DDR) */
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

    /** @brief PORTx control register 1 (_PORTx_CR1) */
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

    /** @brief PORTx control register 1 (_PORTx_CR2) */
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

  } _PORT_t;

  /* Pointer to PORT0 registers */
  #if defined(PORT0_AddressBase)
    #define _PORT0      _SFR(_PORT_t,  PORT0_AddressBase)        ///< PORT0 struct/bit access
    #define _PORT0_ODR  _SFR(uint8_t,  PORT0_AddressBase+0x00)   ///< PORT0 output register
    #define _PORT0_IDR  _SFR(uint8_t,  PORT0_AddressBase+0x01)   ///< PORT0 input register
    #define _PORT0_DDR  _SFR(uint8_t,  PORT0_AddressBase+0x02)   ///< PORT0 direction register
    #define _PORT0_CR1  _SFR(uint8_t,  PORT0_AddressBase+0x03)   ///< PORT0 control register 1
    #define _PORT0_CR2  _SFR(uint8_t,  PORT0_AddressBase+0x04)   ///< PORT0 control register 2
  #endif

  /* Pointer to PORT1 registers */
  #if defined(PORT1_AddressBase)
    #define _PORT1      _SFR(_PORT_t,  PORT1_AddressBase)        ///< PORT1 struct/bit access
    #define _PORT1_ODR  _SFR(uint8_t,  PORT1_AddressBase+0x00)   ///< PORT1 output register
    #define _PORT1_IDR  _SFR(uint8_t,  PORT1_AddressBase+0x01)   ///< PORT1 input register
    #define _PORT1_DDR  _SFR(uint8_t,  PORT1_AddressBase+0x02)   ///< PORT1 direction register
    #define _PORT1_CR1  _SFR(uint8_t,  PORT1_AddressBase+0x03)   ///< PORT1 control register 1
    #define _PORT1_CR2  _SFR(uint8_t,  PORT1_AddressBase+0x04)   ///< PORT1 control register 2
  #endif


  /* PORT Module Reset Values (all ports) */
  #define _PORT_ODR_RESET_VALUE        ((uint8_t) 0x00)          ///< PORTx output register reset value
  #define _PORT_DDR_RESET_VALUE        ((uint8_t) 0x00)          ///< PORTx direction register reset value
  #define _PORT_CR1_RESET_VALUE        ((uint8_t) 0x00)          ///< PORTx control register 1 reset value
  #define _PORT_CR2_RESET_VALUE        ((uint8_t) 0x00)          ///< PORTx control register 2 reset value


  /* macro for accessing pin registers (all ports) */
  #define _PORT_PIN0                   ((uint8_t) (0x01 << 0))   ///< PORTx bit mask for pin 0 (in _PORTx_ODR, _PORTx_IDR, _PORTx_DDR, _PORTx_CR1, _PORTx_CR2)
  #define _PORT_PIN1                   ((uint8_t) (0x01 << 1))   ///< PORTx bit mask for pin 1 (in _PORTx_ODR, _PORTx_IDR, _PORTx_DDR, _PORTx_CR1, _PORTx_CR2)
  #define _PORT_PIN2                   ((uint8_t) (0x01 << 2))   ///< PORTx bit mask for pin 2 (in _PORTx_ODR, _PORTx_IDR, _PORTx_DDR, _PORTx_CR1, _PORTx_CR2)
  #define _PORT_PIN3                   ((uint8_t) (0x01 << 3))   ///< PORTx bit mask for pin 3 (in _PORTx_ODR, _PORTx_IDR, _PORTx_DDR, _PORTx_CR1, _PORTx_CR2)
  #define _PORT_PIN4                   ((uint8_t) (0x01 << 4))   ///< PORTx bit mask for pin 4 (in _PORTx_ODR, _PORTx_IDR, _PORTx_DDR, _PORTx_CR1, _PORTx_CR2)
  #define _PORT_PIN5                   ((uint8_t) (0x01 << 5))   ///< PORTx bit mask for pin 5 (in _PORTx_ODR, _PORTx_IDR, _PORTx_DDR, _PORTx_CR1, _PORTx_CR2)
  #define _PORT_PIN6                   ((uint8_t) (0x01 << 6))   ///< PORTx bit mask for pin 6 (in _PORTx_ODR, _PORTx_IDR, _PORTx_DDR, _PORTx_CR1, _PORTx_CR2)
  #define _PORT_PIN7                   ((uint8_t) (0x01 << 7))   ///< PORTx bit mask for pin 7 (in _PORTx_ODR, _PORTx_IDR, _PORTx_DDR, _PORTx_CR1, _PORTx_CR2)

#endif // PORTx_AddressBase



//------------------------
// Miscellaneous direct registers, several modules (_MSC)
//------------------------
#if defined(_DOXYGEN) || defined(MSC_AddressBase)

  /** @brief struct for miscellaneous direct register access (_MSC) */
  typedef struct {

    /** @brief port 0 interrupt control 0 (_MSC_CFGP00) */
    struct {
      _BITS  INT_LEV  : 1;    ///< interrupt request active level
      _BITS  INT_SEL  : 2;    ///< interrupt source configuration
      _BITS  INT_ENB  : 1;    ///< interrupt enable
      _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
      _BITS           : 3;    //   Reserved, must be kept clear
    } CFGP00;


    /** @brief port 0 interrupt control 1 (_MSC_CFGP01) */
    struct {
      _BITS  INT_LEV  : 1;    ///< interrupt request active level
      _BITS  INT_SEL  : 2;    ///< interrupt source configuration
      _BITS  INT_ENB  : 1;    ///< interrupt enable
      _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
      _BITS           : 3;    //   Reserved, must be kept clear
    } CFGP01;


    /** @brief port 0 interrupt control 2 (_MSC_CFGP02) */
    struct {
      _BITS  INT_LEV  : 1;    ///< interrupt request active level
      _BITS  INT_SEL  : 2;    ///< interrupt source configuration
      _BITS  INT_ENB  : 1;    ///< interrupt enable
      _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
      _BITS           : 3;    //   Reserved, must be kept clear
    } CFGP02;


    /** @brief port 0 interrupt control 3 (_MSC_CFGP03) */
    struct {
      _BITS  INT_LEV  : 1;    ///< interrupt request active level
      _BITS  INT_SEL  : 2;    ///< interrupt source configuration
      _BITS  INT_ENB  : 1;    ///< interrupt enable
      _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
      _BITS           : 3;    //   Reserved, must be kept clear
    } CFGP03;


    /** @brief port 0 interrupt control 4 (_MSC_CFGP04) */
    struct {
      _BITS  INT_LEV  : 1;    ///< interrupt request active level
      _BITS  INT_SEL  : 2;    ///< interrupt source configuration
      _BITS  INT_ENB  : 1;    ///< interrupt enable
      _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
      _BITS           : 3;    //   Reserved, must be kept clear
    } CFGP04;


    /** @brief port 0 interrupt control 5 (_MSC_CFGP05) */
    struct {
      _BITS  INT_LEV  : 1;    ///< interrupt request active level
      _BITS  INT_SEL  : 2;    ///< interrupt source configuration
      _BITS  INT_ENB  : 1;    ///< interrupt enable
      _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
      _BITS           : 3;    //   Reserved, must be kept clear
    } CFGP05;


    /** @brief port 2 interrupt control 0 (_MSC_CFGP20) */
    struct {
      _BITS  INT_LEV  : 1;    ///< interrupt request active level
      _BITS  INT_SEL  : 2;    ///< interrupt source configuration
      _BITS  INT_ENB  : 1;    ///< interrupt enable
      _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
      _BITS           : 3;    //   Reserved, must be kept clear
    } CFGP20;


    /** @brief port 2 interrupt control 1 (_MSC_CFGP21) */
    struct {
      _BITS  INT_LEV  : 1;    ///< interrupt request active level
      _BITS  INT_SEL  : 2;    ///< interrupt source configuration
      _BITS  INT_ENB  : 1;    ///< interrupt enable
      _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
      _BITS           : 3;    //   Reserved, must be kept clear
    } CFGP21;


    /** @brief port 2 interrupt control 2 (_MSC_CFGP22) */
    struct {
      _BITS  INT_LEV  : 1;    ///< interrupt request active level
      _BITS  INT_SEL  : 2;    ///< interrupt source configuration
      _BITS  INT_ENB  : 1;    ///< interrupt enable
      _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
      _BITS           : 3;    //   Reserved, must be kept clear
    } CFGP22;


    /** @brief port 2 interrupt control 3 (_MSC_CFGP23) */
    struct {
      _BITS  INT_LEV  : 1;    ///< interrupt request active level
      _BITS  INT_SEL  : 2;    ///< interrupt source configuration
      _BITS  INT_ENB  : 1;    ///< interrupt enable
      _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
      _BITS           : 3;    //   Reserved, must be kept clear
    } CFGP23;


    /** @brief port 2 interrupt control 4 (_MSC_CFGP24) */
    struct {
      _BITS  INT_LEV  : 1;    ///< interrupt request active level
      _BITS  INT_SEL  : 2;    ///< interrupt source configuration
      _BITS  INT_ENB  : 1;    ///< interrupt enable
      _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
      _BITS           : 3;    //   Reserved, must be kept clear
    } CFGP24;


    /** @brief port 2 interrupt control 5 (_MSC_CFGP25) */
    struct {
      _BITS  INT_LEV  : 1;    ///< interrupt request active level
      _BITS  INT_SEL  : 2;    ///< interrupt source configuration
      _BITS  INT_ENB  : 1;    ///< interrupt enable
      _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
      _BITS           : 3;    //   Reserved, must be kept clear
    } CFGP25;


    /** @brief port 0 interrupt status (_MSC_STSP0) */
    struct {
      _BITS  INT_0    : 1;    ///< interrupt pending for port0[0] or timer 0
      _BITS  INT_1    : 1;    ///< interrupt pending for port0[1] or timer 1
      _BITS  INT_2    : 1;    ///< interrupt pending for port0[2]
      _BITS  INT_3    : 1;    ///< interrupt pending for port0[3]
      _BITS  INT_4    : 1;    ///< interrupt pending for port0[4]
      _BITS  INT_5    : 1;    ///< interrupt pending for port0[5] or CCO aux timer
      _BITS           : 2;    //   Reserved, must be kept clear
    } STSP0;


    /** @brief port 2 interrupt status (_MSC_STSP2) */
    struct {
      _BITS  INT_0    : 1;    ///< interrupt pending for port2[0] or timer 0
      _BITS  INT_1    : 1;    ///< interrupt pending for port2[1] or timer 1
      _BITS  INT_2    : 1;    ///< interrupt pending for port2[2]
      _BITS  INT_3    : 1;    ///< interrupt pending for port2[3]
      _BITS  INT_4    : 1;    ///< interrupt pending for port2[4]
      _BITS  INT_5    : 1;    ///< interrupt pending for port2[5] or CCO aux timer
      _BITS           : 2;    //   Reserved, must be kept clear
    } STSP2;


    /** @brief port 2 input data register (_MSC_INPP2) */
    struct {
      _BITS   IN_0    : 1;    ///< port 2 [0] input signal
      _BITS   IN_1    : 1;    ///< port 2 [1] input signal
      _BITS   IN_2    : 1;    ///< port 2 [2] input signal
      _BITS   IN_3    : 1;    ///< port 2 [3] input signal
      _BITS   IN_4    : 1;    ///< port 2 [4] input signal
      _BITS   IN_5    : 1;    ///< port 2 [5] input signal
      _BITS           : 2;    //   Reserved, must be kept clear
    } INPP2;


    /* Reserved registers (1B) */
    uint8_t res     [1];


    /* different register layout on seletced devices */
    #if !defined(STNRG) && !defined(STLUX285A)

      /** @brief DAC & comparator control register (_MSC_DACCTR) */
      struct {
        _BITS   DAC0_EN     : 1;    ///< enable DAC0 and COMP0
        _BITS   DAC1_EN     : 1;    ///< enable DAC1 and COMP1
        _BITS   DAC2_EN     : 1;    ///< enable DAC2 and COMP2
        _BITS   DAC3_EN     : 1;    ///< enable DAC3
        _BITS   CP3_EN      : 1;    ///< enable COMP3
        _BITS   CP3_SEL     : 1;    ///< COMP3 reference voltage selection
        _BITS               : 1;    //   Reserved, must be kept clear
        _BITS   DAC_BIAS_EN : 1;    ///< DAC bias enable
      } DACCTR;

    #else // STNRG || STLUX285A

      /** @brief DAC & comparator control register (_MSC_DACCTR) */
      struct {
        _BITS   DAC0_EN     : 1;    ///< enable DAC0 & CMP0, and select internal reference
        _BITS   DAC1_EN     : 1;    ///< enable DAC1 & CMP1, and select internal reference
        _BITS   DAC2_EN     : 1;    ///< enable DAC2 & CMP2, and select internal reference
        _BITS   DAC3_EN     : 1;    ///< enable DAC3 & CMP3, and select internal reference
        _BITS   CP0_EN_EREF : 1;    ///< enable CMP0 and select external reference when DAC0_EN=0
        _BITS   CP1_EN_EREF : 1;    ///< enable CMP1 and select external reference when DAC1_EN=0
        _BITS   CP2_EN_EREF : 1;    ///< enable CMP2 and select external reference when DAC2_EN=0
        _BITS   CP3_EN_EREF : 1;    ///< enable CMP3 and select external reference when DAC3_EN=0
      } DACCTR;

    #endif // STNRG || STLUX285A


    /** @brief DAC0 input data register (_MSC_DACIN0) */
    struct {
      _BITS   DAC_IN  : 4;    ///< DAC input value
      _BITS           : 4;    //   Reserved, must be kept clear
    } DACIN0;


    /** @brief DAC1 input data register (_MSC_DACIN1) */
    struct {
      _BITS   DAC_IN  : 4;    ///< DAC input value
      _BITS           : 4;    //   Reserved, must be kept clear
    } DACIN1;


    /** @brief DAC2 input data register (_MSC_DACIN2) */
    struct {
      _BITS   DAC_IN  : 4;    ///< DAC input value
      _BITS           : 4;    //   Reserved, must be kept clear
    } DACIN2;


    /** @brief DAC3 input data register (_MSC_DACIN3) */
    struct {
      _BITS   DAC_IN  : 4;    ///< DAC input value
      _BITS           : 4;    //   Reserved, must be kept clear
    } DACIN3;


    /** @brief SMED0 & SMED1 global configuration register (_MSC_SMDCFG01) */
    struct {
      _BITS   SMD0_DRVOUT  : 1;    ///< PWM0 control merge output signal in coupled mode
      _BITS   SMD0_GLBCONF : 3;    ///< SMED0 global configuration
      _BITS   SMD1_DRVOUT  : 1;    ///< PWM1 control merge output signal in coupled mode
      _BITS   SMD1_GLBCONF : 3;    ///< SMED1 global configuration
    } SMDCFG01;


    /** @brief SMED2 & SMED3 global configuration register (_MSC_SMDCFG23) */
    struct {
      _BITS   SMD2_DRVOUT  : 1;    ///< PWM2 control merge output signal in coupled mode
      _BITS   SMD2_GLBCONF : 3;    ///< SMED2 global configuration
      _BITS   SMD3_DRVOUT  : 1;    ///< PWM3 control merge output signal in coupled mode
      _BITS   SMD3_GLBCONF : 3;    ///< SMED3 global configuration
    } SMDCFG23;


    /** @brief SMED4 & SMED5 global configuration register (_MSC_SMDCFG45) */
    struct {
      _BITS   SMD4_DRVOUT  : 1;    ///< PWM4 control merge output signal in coupled mode
      _BITS   SMD4_GLBCONF : 3;    ///< SMED4 global configuration
      _BITS   SMD5_DRVOUT  : 1;    ///< PWM5 control merge output signal in coupled mode
      _BITS   SMD5_GLBCONF : 3;    ///< SMED5 global configuration
    } SMDCFG45;


    /** @brief SMEDs software events (_MSC_SMSWEV) */
    struct {
      _BITS   SW0     : 1;    ///< software event on SMED0
      _BITS   SW1     : 1;    ///< software event on SMED1
      _BITS   SW2     : 1;    ///< software event on SMED2
      _BITS   SW3     : 1;    ///< software event on SMED3
      _BITS   SW4     : 1;    ///< software event on SMED4
      _BITS   SW5     : 1;    ///< software event on SMED5
      _BITS           : 2;    //   Reserved, must be kept clear
    } SMSWEV;


    /** @brief SMEDs unlock register (_MSC_SMUNLOCK) */
    struct {
      _BITS   USE_UNLOCK_01 : 1;    ///< enable unlock for SMED0/1 (only in coupled mode)
      _BITS   USE_UNLOCK_23 : 1;    ///< enable unlock for SMED2/3 (only in coupled mode)
      _BITS   USE_UNLOCK_45 : 1;    ///< enable unlock for SMED4/5 (only in coupled mode)
      _BITS   UNLOCK_01     : 1;    ///< unlock SMED0/1  (only in coupled mode)
      _BITS   UNLOCK_23     : 1;    ///< unlock SMED2/3  (only in coupled mode)
      _BITS   UNLOCK_45     : 1;    ///< unlock SMED4/5  (only in coupled mode)
      _BITS                 : 2;    //   Reserved, must be kept clear
    } SMUNLOCK;


    /** @brief connection matrix selection for SMED0 (_MSC_CBOXS0) */
    struct {
      _BITS   CONB_S0_0 : 2;    ///< connection box selection line for SMED0 input InSig[0]
      _BITS   CONB_S0_1 : 2;    ///< connection box selection line for SMED0 input InSig[1]
      _BITS   CONB_S0_2 : 2;    ///< connection box selection line for SMED0 input InSig[2]
      _BITS             : 2;    //   Reserved, must be kept clear
    } CBOXS0;


    /** @brief connection matrix selection for SMED1 (_MSC_CBOXS1) */
    struct {
      _BITS   CONB_S1_0 : 2;    ///< connection box selection line for SMED1 input InSig[0]
      _BITS   CONB_S1_1 : 2;    ///< connection box selection line for SMED1 input InSig[1]
      _BITS   CONB_S1_2 : 2;    ///< connection box selection line for SMED1 input InSig[2]
      _BITS             : 2;    //   Reserved, must be kept clear
    } CBOXS1;


    /** @brief connection matrix selection for SMED2 (_MSC_CBOXS2) */
    struct {
      _BITS   CONB_S2_0 : 2;    ///< connection box selection line for SMED2 input InSig[0]
      _BITS   CONB_S2_1 : 2;    ///< connection box selection line for SMED2 input InSig[1]
      _BITS   CONB_S2_2 : 2;    ///< connection box selection line for SMED2 input InSig[2]
      _BITS             : 2;    //   Reserved, must be kept clear
    } CBOXS2;


    /** @brief connection matrix selection for SMED3 (_MSC_CBOXS3) */
    struct {
      _BITS   CONB_S3_0 : 2;    ///< connection box selection line for SMED3 input InSig[0]
      _BITS   CONB_S3_1 : 2;    ///< connection box selection line for SMED3 input InSig[1]
      _BITS   CONB_S3_2 : 2;    ///< connection box selection line for SMED3 input InSig[2]
      _BITS             : 2;    //   Reserved, must be kept clear
    } CBOXS3;


    /** @brief connection matrix selection for SMED4 (_MSC_CBOXS4) */
    struct {
      _BITS   CONB_S4_0 : 2;    ///< connection box selection line for SMED4 input InSig[0]
      _BITS   CONB_S4_1 : 2;    ///< connection box selection line for SMED4 input InSig[1]
      _BITS   CONB_S4_2 : 2;    ///< connection box selection line for SMED4 input InSig[2]
      _BITS             : 2;    //   Reserved, must be kept clear
    } CBOXS4;


    /** @brief connection matrix selection for SMED5 (_MSC_CBOXS5) */
    struct {
      _BITS   CONB_S5_0 : 2;    ///< connection box selection line for SMED5 input InSig[0]
      _BITS   CONB_S5_1 : 2;    ///< connection box selection line for SMED5 input InSig[1]
      _BITS   CONB_S5_2 : 2;    ///< connection box selection line for SMED5 input InSig[2]
      _BITS             : 2;    //   Reserved, must be kept clear
    } CBOXS5;


    /** @brief SMED I/O MUX control register (_MSC_IOMXSMD) */
    struct {
      _BITS   SMD_FSMSL0 : 3;    ///< SMED FSM status multiplexing output selection line for P0[2:0]
      _BITS   SEL_FSMEN0 : 1;    ///< enable SMED FSM status signals multiplexing on P0[2:0]
      _BITS   SMD_FSMSL1 : 3;    ///< SMED FSM status multiplexing output selection line for P0[5:3]
      _BITS   SEL_FSMEN1 : 1;    ///< enable SMED FSM status signals multiplexing on P0[5:3]
    } IOMXSMD;



    #if defined(STNRG)

      /** @brief port 1 interrupt control 0 register (_MSC_CFGP10) */
      struct {
        _BITS  INT_LEV  : 1;    ///< interrupt request active level
        _BITS  INT_SEL  : 2;    ///< interrupt source configuration
        _BITS  INT_ENB  : 1;    ///< interrupt enable
        _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
        _BITS           : 3;    //   Reserved, must be kept clear
      } CFGP10;

      /** @brief port 1 interrupt control 1 register (_MSC_CFGP11) */
      struct {
        _BITS  INT_LEV  : 1;    ///< interrupt request active level
        _BITS  INT_SEL  : 2;    ///< interrupt source configuration
        _BITS  INT_ENB  : 1;    ///< interrupt enable
        _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
        _BITS           : 3;    //   Reserved, must be kept clear
      } CFGP11;

      /** @brief port 1 interrupt control 2 register (_MSC_CFGP12) */
      struct {
        _BITS  INT_LEV  : 1;    ///< interrupt request active level
        _BITS  INT_SEL  : 2;    ///< interrupt source configuration
        _BITS  INT_ENB  : 1;    ///< interrupt enable
        _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
        _BITS           : 3;    //   Reserved, must be kept clear
      } CFGP12;

      /** @brief port 1 interrupt control 3 register (_MSC_CFGP13) */
      struct {
        _BITS  INT_LEV  : 1;    ///< interrupt request active level
        _BITS  INT_SEL  : 2;    ///< interrupt source configuration
        _BITS  INT_ENB  : 1;    ///< interrupt enable
        _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
        _BITS           : 3;    //   Reserved, must be kept clear
      } CFGP13;

      /** @brief port 1 interrupt control 4 register (_MSC_CFGP14) */
      struct {
        _BITS  INT_LEV  : 1;    ///< interrupt request active level
        _BITS  INT_SEL  : 2;    ///< interrupt source configuration
        _BITS  INT_ENB  : 1;    ///< interrupt enable
        _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
        _BITS           : 3;    //   Reserved, must be kept clear
      } CFGP14;

    #else // STNRG

      /* Reserved registers (5B) */
      uint8_t res2    [5];

    #endif // STNRG


    /** @brief port 1 interrupt control 5 & AUX timer register (_MSC_CFGP15) */
    struct {
      _BITS  INT_LEV  : 1;    ///< interrupt request active level
      _BITS  INT_SEL  : 2;    ///< interrupt source configuration
      _BITS  INT_ENB  : 1;    ///< interrupt enable
      _BITS  INT_TYPE : 1;    ///< interrupt type configuration IRQ/NMI
      _BITS           : 3;    //   Reserved, must be kept clear
    } CFGP15;


    /** @brief port 1 & AUX timer interrupt status register (_MSC_STSP1) */
    struct {
      _BITS  INT_0    : 1;    ///< interrupt pending for port1[0] or timer 0
      _BITS  INT_1    : 1;    ///< interrupt pending for port1[1] or timer 1
      _BITS  INT_2    : 1;    ///< interrupt pending for port1[2]
      _BITS  INT_3    : 1;    ///< interrupt pending for port1[3]
      _BITS  INT_4    : 1;    ///< interrupt pending for port1[4]
      _BITS  INT_5    : 1;    ///< interrupt pending for port1[5] or CCO aux timer
      _BITS           : 2;    //   Reserved, must be kept clear
    } STSP1;


    /* Reserved registers (1B) */
    uint8_t res3    [1];


    /** @brief port 3 (COMP) input register (_MSC_INPP3) */
    struct {
      _BITS   COMP_0  : 1;    ///< COMP0 output signal
      _BITS   COMP_1  : 1;    ///< COMP1 output signal
      _BITS   COMP_2  : 1;    ///< COMP2 output signal
      _BITS   COMP_3  : 1;    ///< COMP3 output signal
      _BITS           : 4;    //   Reserved, must be kept clear
    } INPP3;


    /** @brief port P0 alternate function MUX control register (_MSC_IOMXP0) */
    struct {
      _BITS   SEL_P010 : 2;   ///< Port0 [1:0] I/O multiplexing scheme
      _BITS   SEL_P032 : 2;   ///< Port0 [3:2] I/O multiplexing scheme
      _BITS   SEL_P054 : 2;   ///< Port0 [5:4] I/O multiplexing scheme
      _BITS            : 2;   //   Reserved, must be kept clear
    } IOMXP0;


    /** @brief port P1 alternate function MUX control register (_MSC_IOMXP1) */
    struct {
      _BITS   SEL_P10 : 1;   ///< Port1 [0] I/O multiplexing scheme
      _BITS   SEL_P11 : 1;   ///< Port1 [1] I/O multiplexing scheme
      _BITS   SEL_P12 : 1;   ///< Port1 [2] I/O multiplexing scheme
      _BITS   SEL_P13 : 1;   ///< Port1 [3] I/O multiplexing scheme
      _BITS   SEL_P14 : 1;   ///< Port1 [4] I/O multiplexing scheme
      _BITS   SEL_P15 : 1;   ///< Port1 [5] I/O multiplexing scheme
      _BITS           : 2;   //   Reserved, must be kept clear
    } IOMXP1;


    /** @brief MSC indirect address register (_MSC_IDXADD) */
    struct {
      _BITS   ADDR    : 8;   ///< indirect address for _MSC_IDXDAT
    } IDXADD;


    /** @brief MCR indirect data registers (_MSC_IDXDAT), indirect adressing via _MSC_IDXADD */
    union {

      // basic timer 0 & 1 only for STNRG
      #if defined(STNRG)

        /** @brief basic timer 1/0 source clock selection (_MSC_FTM0CKSEL, indirect access via _MSC_IDXADD=0x00) */
        struct {
          _BITS   CLK_SEL0   : 3;   ///< Basic Timer 0 clock source configuration
          _BITS   EN0        : 1;   ///< Basic Timer 0 enable
          _BITS   CLK_SEL1   : 3;   ///< Basic Timer 1 clock source configuration
          _BITS   EN1        : 1;   ///< Basic Timer 1 enable
        } FTM0CKSEL;


        /** @brief basic timer 0 clock prescale (_MSC_FTM0CKDIV, indirect access via _MSC_IDXADD=0x01) */
        struct {
          _BITS   DIV        : 8;   ///< Basic Timer 0 clock prescaler (fTIM=clk/(N+1))
        } FTM0CKDIV;


        /** @brief basic timer 0 counter value (_MSC_FTM0CONF, indirect access via _MSC_IDXADD=0x02) */
        struct {
          _BITS   COUNT      : 6;   ///< Basic Timer 0 counter value (cnt=N+2)
          _BITS              : 2;   //   Reserved, must be kept clear
        } FTM0CONF;


        /** @brief basic timer 1 clock prescale (_MSC_FTM1CKDIV, indirect access via _MSC_IDXADD=0x03) */
        struct {
          _BITS   DIV        : 8;   ///< Basic Timer 1 clock prescaler (fTIM=clk/(N+1))
        } FTM1CKDIV;


        /** @brief basic timer 1 counter value (_MSC_FTM1CONF, indirect access via _MSC_IDXADD=0x04) */
        struct {
          _BITS   COUNT       : 6;   ///< Basic Timer 1 counter value (cnt=N+2)
          _BITS   ADC_AFLUSH  : 1;   ///< ADC FIFO auto-flush for single conversion mode
          _BITS   ADC_ARELOAD : 1;   ///< ADC FIFO auto-reload enable (requires ADC_CFG.CIRCULAR=1)
        } FTM1CONF;

      #endif // STNRG


      /** @brief DALI clock selection (_MSC_DALICKSEL, indirect access via _MSC_IDXADD=0x05) */
      struct {
        _BITS   CLK_SEL    : 3;   ///< DALI filter clock source configuration
        _BITS   EN         : 1;   ///< DALI filter logic enable
        _BITS   ADC_TRGSEL : 4;   ///< DALI configure ADC trigger sources
      } DALICKSEL;


      /** @brief DALI filter clock division factor (_MSC_DALICKDIV, indirect access via _MSC_IDXADD=0x06) */
      struct {
        _BITS   DIV     : 8;      ///< DALI filter clock division factor (CLKFLT = CLK(CLK_SEL) / (DIV + 1))
      } DALICKDIV;


      /** @brief DALI filter mode configuration (_MSC_DALICONF, indirect access via _MSC_IDXADD=0x07) */
      struct {
        _BITS   COUNT   : 6;   ///< DALI filter counter timer value
        _BITS   MODE    : 2;   ///< DALI filter mode selection
      } DALICONF;


      /** @brief INPP2 aux. register 1 (_MSC_INPP2AUX1, indirect access via _MSC_IDXADD=0x08) */
      struct {
        _BITS   PULLUP0 : 1;   ///< INPP2[0] pull-up enable
        _BITS   PULLUP1 : 1;   ///< INPP2[1] pull-up enable
        _BITS   PULLUP2 : 1;   ///< INPP2[2] pull-up enable
        _BITS   PULLUP3 : 1;   ///< INPP2[3] pull-up enable
        _BITS   PULLUP4 : 1;   ///< INPP2[4] pull-up enable
        _BITS   PULLUP5 : 1;   ///< INPP2[5] pull-up enable
        #if defined(STNRG)
          _BITS             : 1; //   Reserved, must be kept clear
          _BITS   ADCTRG_EN : 1; ///< ADC HW triggered conversion request
        #else
          _BITS             : 2; //   Reserved, must be kept clear
        #endif
      } INPP2AUX1;


      /** @brief INPP2 aux. register 2 (_MSC_INPP2AUX2, indirect access via _MSC_IDXADD=0x09) */
      struct {
        _BITS   INPP2_IMSK0 : 1;  ///< INPP2[0] interrupt mask enable
        _BITS   INPP2_IMSK1 : 1;  ///< INPP2[1] interrupt mask enable
        _BITS   INPP2_IMSK2 : 1;  ///< INPP2[2] interrupt mask enable
        _BITS   INPP2_IMSK3 : 1;  ///< INPP2[3] interrupt mask enable
        _BITS   INPP2_IMSK4 : 1;  ///< INPP2[4] interrupt mask enable
        _BITS   INPP2_IMSK5 : 1;  ///< INPP2[5] interrupt mask enable
        _BITS   INPP0_IMSK  : 1;  ///< INPP0 interrupt mask enable
        _BITS               : 1; //   Reserved, must be kept clear
      } INPP2AUX2;


      // only for STNRG devices
      #if defined(STNRG)

        /** @brief DAC0 hysteresis selection (_MSC_DAC0_HYST, indirect access via _MSC_IDXADD=0x0A) */
        struct {
          _BITS   HYSTDN  : 3;      ///< comparator hysteresis on falling signals
          _BITS           : 1;      //   Reserved, must be kept clear
          _BITS   HYSTUP  : 3;      ///< comparator hysteresis on rising signals
          _BITS           : 1;      //   Reserved, must be kept clear
        } DAC0_HYST;


        /** @brief DAC1 hysteresis selection (_MSC_DAC1_HYST, indirect access via _MSC_IDXADD=0x0B) */
        struct {
          _BITS   HYSTDN  : 3;      ///< comparator hysteresis on falling signals
          _BITS           : 1;      //   Reserved, must be kept clear
          _BITS   HYSTUP  : 3;      ///< comparator hysteresis on rising signals
          _BITS           : 1;      //   Reserved, must be kept clear
        } DAC1_HYST;


        /** @brief DAC2 hysteresis selection (_MSC_DAC2_HYST, indirect access via _MSC_IDXADD=0x0C) */
        struct {
          _BITS   HYSTDN  : 3;      ///< comparator hysteresis on falling signals
          _BITS           : 1;      //   Reserved, must be kept clear
          _BITS   HYSTUP  : 3;      ///< comparator hysteresis on rising signals
          _BITS           : 1;      //   Reserved, must be kept clear
        } DAC2_HYST;


        /** @brief DAC3 hysteresis selection (_MSC_DAC3_HYST, indirect access via _MSC_IDXADD=0x0D) */
        struct {
          _BITS   HYSTDN  : 3;      ///< comparator hysteresis on falling signals
          _BITS           : 1;      //   Reserved, must be kept clear
          _BITS   HYSTUP  : 3;      ///< comparator hysteresis on rising signals
          _BITS           : 1;      //   Reserved, must be kept clear
        } DAC3_HYST;

      #endif // STNRG

      /** @brief P3-0 control register input line (CMP0) (_MSC_CFGP30, indirect access via _MSC_IDXADD=0x0E) */
      struct {
        _BITS  INT_LEV  : 1;    ///< interrupt request active level
        _BITS  INT_SEL  : 2;    ///< interrupt source configuration
        _BITS  INT_ENB  : 1;    ///< interrupt enable
        _BITS  INT_MSK  : 1;    ///< interrupt mask enable for polling
        _BITS           : 3;    //   Reserved, must be kept clear
      } CFGP30;


      /** @brief P3-1 control register input line (CMP1) (_MSC_CFGP31, indirect access via _MSC_IDXADD=0x0F) */
      struct {
        _BITS  INT_LEV  : 1;    ///< interrupt request active level
        _BITS  INT_SEL  : 2;    ///< interrupt source configuration
        _BITS  INT_ENB  : 1;    ///< interrupt enable
        _BITS  INT_MSK  : 1;    ///< interrupt mask enable for polling
        _BITS           : 3;    //   Reserved, must be kept clear
      } CFGP31;


      /** @brief P3-2 control register input line (CMP2) (_MSC_CFGP32, indirect access via _MSC_IDXADD=0x10) */
      struct {
        _BITS  INT_LEV  : 1;    ///< interrupt request active level
        _BITS  INT_SEL  : 2;    ///< interrupt source configuration
        _BITS  INT_ENB  : 1;    ///< interrupt enable
        _BITS  INT_MSK  : 1;    ///< interrupt mask enable for polling
        _BITS           : 3;    //   Reserved, must be kept clear
      } CFGP32;


      /** @brief P3-3 control register input line (CMP3) (_MSC_CFGP33, indirect access via _MSC_IDXADD=0x11) */
      struct {
        _BITS  INT_LEV  : 1;    ///< interrupt request active level
        _BITS  INT_SEL  : 2;    ///< interrupt source configuration
        _BITS  INT_ENB  : 1;    ///< interrupt enable
        _BITS  INT_MSK  : 1;    ///< interrupt mask enable for polling
        _BITS           : 3;    //   Reserved, must be kept clear
      } CFGP33;


      /** @brief Port 3 status register (CMP) (_MSC_STSP3, indirect access via _MSC_IDXADD=0x12) */
      struct {
        _BITS  INT_0    : 1;    ///< interrupt pending for port0[0]
        _BITS  INT_1    : 1;    ///< interrupt pending for port0[1]
        _BITS  INT_2    : 1;    ///< interrupt pending for port0[2]
        _BITS  INT_3    : 1;    ///< interrupt pending for port0[3]
        _BITS           : 4;    //   Reserved, must be kept clear
      } STSP3;


      /** @brief Port P2 alternate function MUX control register (_MSC_IOMXP2, indirect access via _MSC_IDXADD=0x13). Note: missing in RM v1 */
      struct {
        _BITS   SEL_P20 : 1;   ///< Port2 [0] I/O multiplexing scheme
        _BITS   SEL_P21 : 1;   ///< Port2 [1] I/O multiplexing scheme
        _BITS   SEL_P22 : 1;   ///< Port2 [2] I/O multiplexing scheme
        _BITS   SEL_P23 : 1;   ///< Port2 [3] I/O multiplexing scheme
        _BITS   SEL_P24 : 1;   ///< Port2 [4] I/O multiplexing scheme
        _BITS   SEL_P25 : 1;   ///< Port2 [5] I/O multiplexing scheme
        _BITS   SEL_P26 : 1;   ///< Port2 [6] I/O multiplexing scheme
        _BITS   SEL_P27 : 1;   ///< Port2 [7] I/O multiplexing scheme
      } IOMXP2;

    } IDXDAT;

  } _MSC_t;

  /* Pointer to flash registers */
  #define _MSC          _SFR(_MSC_t,   MSC_AddressBase)        ///< MSC misc register struct/bit access
  #define _MSC_CFGP00   _SFR(uint8_t,  MSC_AddressBase+0x00)   ///< MSC port 0 interrupt control 0
  #define _MSC_CFGP01   _SFR(uint8_t,  MSC_AddressBase+0x01)   ///< MSC port 0 interrupt control 1
  #define _MSC_CFGP02   _SFR(uint8_t,  MSC_AddressBase+0x02)   ///< MSC port 0 interrupt control 2
  #define _MSC_CFGP03   _SFR(uint8_t,  MSC_AddressBase+0x03)   ///< MSC port 0 interrupt control 3
  #define _MSC_CFGP04   _SFR(uint8_t,  MSC_AddressBase+0x04)   ///< MSC port 0 interrupt control 4
  #define _MSC_CFGP05   _SFR(uint8_t,  MSC_AddressBase+0x05)   ///< MSC port 0 interrupt control 5
  #define _MSC_CFGP20   _SFR(uint8_t,  MSC_AddressBase+0x06)   ///< MSC port 2 interrupt control 0
  #define _MSC_CFGP21   _SFR(uint8_t,  MSC_AddressBase+0x07)   ///< MSC port 2 interrupt control 1
  #define _MSC_CFGP22   _SFR(uint8_t,  MSC_AddressBase+0x08)   ///< MSC port 2 interrupt control 2
  #define _MSC_CFGP23   _SFR(uint8_t,  MSC_AddressBase+0x09)   ///< MSC port 2 interrupt control 3
  #define _MSC_CFGP24   _SFR(uint8_t,  MSC_AddressBase+0x0A)   ///< MSC port 2 interrupt control 4
  #define _MSC_CFGP25   _SFR(uint8_t,  MSC_AddressBase+0x0B)   ///< MSC port 2 interrupt control 5
  #define _MSC_STSP0    _SFR(uint8_t,  MSC_AddressBase+0x0C)   ///< MSC port 0 interrupt status
  #define _MSC_STSP2    _SFR(uint8_t,  MSC_AddressBase+0x0D)   ///< MSC port 2 interrupt status
  #define _MSC_INPP2    _SFR(uint8_t,  MSC_AddressBase+0x0E)   ///< MSC port 2 input data register
  // reserved (1B)
  #define _MSC_DACCTR   _SFR(uint8_t,  MSC_AddressBase+0x10)   ///< MSC DAC & comparator control register
  #define _MSC_DACIN0   _SFR(uint8_t,  MSC_AddressBase+0x11)   ///< MSC DAC0 input data register
  #define _MSC_DACIN1   _SFR(uint8_t,  MSC_AddressBase+0x12)   ///< MSC DAC1 input data register
  #define _MSC_DACIN2   _SFR(uint8_t,  MSC_AddressBase+0x13)   ///< MSC DAC2 input data register
  #define _MSC_DACIN3   _SFR(uint8_t,  MSC_AddressBase+0x14)   ///< MSC DAC3 input data register
  #define _MSC_SMDCFG01 _SFR(uint8_t,  MSC_AddressBase+0x15)   ///< MSC SMED0 & SMED1 global configuration register
  #define _MSC_SMDCFG23 _SFR(uint8_t,  MSC_AddressBase+0x16)   ///< MSC SMED2 & SMED3 global configuration register
  #define _MSC_SMDCFG45 _SFR(uint8_t,  MSC_AddressBase+0x17)   ///< MSC SMED4 & SMED5 global configuration register
  #define _MSC_SMSWEV   _SFR(uint8_t,  MSC_AddressBase+0x18)   ///< MSC SMEDs software events
  #define _MSC_SMUNLOCK _SFR(uint8_t,  MSC_AddressBase+0x19)   ///< MSC SMEDs unlock register
  #define _MSC_CBOXS0   _SFR(uint8_t,  MSC_AddressBase+0x1A)   ///< MSC connection matrix selection for SMED0
  #define _MSC_CBOXS1   _SFR(uint8_t,  MSC_AddressBase+0x1B)   ///< MSC connection matrix selection for SMED1
  #define _MSC_CBOXS2   _SFR(uint8_t,  MSC_AddressBase+0x1C)   ///< MSC connection matrix selection for SMED2
  #define _MSC_CBOXS3   _SFR(uint8_t,  MSC_AddressBase+0x1D)   ///< MSC connection matrix selection for SMED3
  #define _MSC_CBOXS4   _SFR(uint8_t,  MSC_AddressBase+0x1E)   ///< MSC connection matrix selection for SMED4
  #define _MSC_CBOXS5   _SFR(uint8_t,  MSC_AddressBase+0x1F)   ///< MSC connection matrix selection for SMED5
  #define _MSC_IOMXSMD  _SFR(uint8_t,  MSC_AddressBase+0x20)   ///< MSC SMED I/O MUX control register
  #if defined(STNRG)
    #define _MSC_CFGP10   _SFR(uint8_t,  MSC_AddressBase+0x21)   ///< MSC port 1 interrupt control 0
    #define _MSC_CFGP11   _SFR(uint8_t,  MSC_AddressBase+0x22)   ///< MSC port 1 interrupt control 1
    #define _MSC_CFGP12   _SFR(uint8_t,  MSC_AddressBase+0x23)   ///< MSC port 1 interrupt control 2
    #define _MSC_CFGP13   _SFR(uint8_t,  MSC_AddressBase+0x24)   ///< MSC port 1 interrupt control 3
    #define _MSC_CFGP14   _SFR(uint8_t,  MSC_AddressBase+0x25)   ///< MSC port 1 interrupt control 4
  #else
    // reserved (5B)
  #endif
  #define _MSC_CFGP15   _SFR(uint8_t,  MSC_AddressBase+0x26)   ///< MSC port 1 interrupt control 5 & AUX timer register
  #define _MSC_STSP1    _SFR(uint8_t,  MSC_AddressBase+0x27)   ///< MSC port 1 & AUX timer interrupt status register
  // reserved (1B)
  #define _MSC_INPP3    _SFR(uint8_t,  MSC_AddressBase+0x29)   ///< MSC port 3 (COMP) input register
  #define _MSC_IOMXP0   _SFR(uint8_t,  MSC_AddressBase+0x2A)   ///< MSC port P0 alternate function MUX control register
  #define _MSC_IOMXP1   _SFR(uint8_t,  MSC_AddressBase+0x2B)   ///< MSC port P1 alternate function MUX control register
  #define _MSC_IDXADD   _SFR(uint8_t,  MSC_AddressBase+0x2C)   ///< MSC indirect address register
  #define _MSC_IDXDAT   _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC indirect data register

  // indirect MSC register (adressing via _MSC_IDXADD)
  #define _MSC_FTM0CKSEL  _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC basic timer 1/0 source clock selection (indirect addressing via _MSC_IDXADD=0x00)
  #define _MSC_FTM0CKDIV  _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC basic timer 0 clock prescale (indirect addressing via _MSC_IDXADD=0x01)
  #define _MSC_FTM0CONF   _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC basic timer 0 counter value (indirect addressing via _MSC_IDXADD=0x02)
  #define _MSC_FTM1CKDIV  _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC basic timer 1 clock prescale (indirect addressing via _MSC_IDXADD=0x03)
  #define _MSC_FTM1CONF   _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC basic timer 1 counter value (indirect addressing via _MSC_IDXADD=0x04)
  #define _MSC_DALICKSEL  _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC DALI clock selection (indirect addressing via _MSC_IDXADD=0x05)
  #define _MSC_DALICKDIV  _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC DALI filter clock division factor (indirect addressing via _MSC_IDXADD=0x06)
  #define _MSC_DALICONF   _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC DALI filter mode configuration (indirect addressing via _MSC_IDXADD=0x07)
  #define _MSC_INPP2AUX1  _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC INPP2 aux. register 1 (indirect addressing via _MSC_IDXADD=0x08)
  #define _MSC_INPP2AUX2  _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC INPP2 aux. register 2 (indirect addressing via _MSC_IDXADD=0x09)
  #if defined(STNRG)
    #define _MSC_DAC0_HYST  _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC DAC0 hysteresis selection (indirect addressing via _MSC_IDXADD=0x0A)
    #define _MSC_DAC1_HYST  _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC DAC1 hysteresis selection (indirect addressing via _MSC_IDXADD=0x0B)
    #define _MSC_DAC2_HYST  _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC DAC2 hysteresis selection (indirect addressing via _MSC_IDXADD=0x0C)
    #define _MSC_DAC3_HYST  _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC DAC3 hysteresis selection (indirect addressing via _MSC_IDXADD=0x0D)
  #endif
  #define _MSC_CFGP30     _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC P3-0 control register input line (CMP0) (indirect addressing via _MSC_IDXADD=0x0E)
  #define _MSC_CFGP31     _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC P3-1 control register input line (CMP1) (indirect addressing via _MSC_IDXADD=0x0F)
  #define _MSC_CFGP32     _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC P3-2 control register input line (CMP2) (indirect addressing via _MSC_IDXADD=0x10)
  #define _MSC_CFGP33     _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC P3-3 control register input line (CMP3) (indirect addressing via _MSC_IDXADD=0x11)
  #define _MSC_STSP3      _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC Port 3 status register (CMP) (indirect addressing via _MSC_IDXADD=0x12)
  #define _MSC_IOMXP2     _SFR(uint8_t,  MSC_AddressBase+0x2D)   ///< MSC Port P2 alternate function MUX control register (indirect addressing via _MSC_IDXADD=0x13. Note: missing in RM v1)


  /* MSC Module Reset Values (all are 0x00) */
  #define _MSC_RESET_VALUE           ((uint8_t) 0x00)          ///< MSC registers reset value (all 0x00)


  /* MSC control register 1 (_MSC_CFGPxy) */
  #define _MSC_CFGPXY_INT_LEV        ((uint8_t) (0x01 << 0))   ///< interrupt request active level [0]
  #define _MSC_CFGPXY_INT_SEL        ((uint8_t) (0x03 << 1))   ///< interrupt request active level [1:0]
  #define _MSC_CFGPXY_INT_SEL0       ((uint8_t) (0x01 << 1))   ///< interrupt request active level [0]
  #define _MSC_CFGPXY_INT_SEL1       ((uint8_t) (0x01 << 2))   ///< interrupt request active level [1]
  #define _MSC_CFGPXY_INT_ENB        ((uint8_t) (0x01 << 3))   ///< interrupt enable [0]
  #define _MSC_CFGPXY_INT_TYPE       ((uint8_t) (0x01 << 4))   ///< interrupt type configuration IRQ/NMI (x=0..2) [0]
  #define _MSC_CFGPXY_INT_MSK        ((uint8_t) (0x01 << 4))   ///< interrupt mask enable for polling (x=3) [0]
  // reserved [7:5]

  /* MSC port x interrupt status (_MSC_STSPx) */
  #define _MSC_STSPX_INT_0           ((uint8_t) (0x01 << 0))   ///< interrupt pending for port2[0] or timer 0 [0]
  #define _MSC_STSPX_INT_1           ((uint8_t) (0x01 << 1))   ///< interrupt pending for port2[1] or timer 1 [0]
  #define _MSC_STSPX_INT_2           ((uint8_t) (0x01 << 2))   ///< interrupt pending for port2[2] [0]
  #define _MSC_STSPX_INT_3           ((uint8_t) (0x01 << 3))   ///< interrupt pending for port2[3] [0]
  #define _MSC_STSPX_INT_4           ((uint8_t) (0x01 << 4))   ///< interrupt pending for port2[4] [0]
  #define _MSC_STSPX_INT_5           ((uint8_t) (0x01 << 5))   ///< interrupt pending for port2[5] or CCO aux timer [0]
  // reserved [7:6]

  /* MSC port 2 input data register (_MSC_INPP2) */
  #define _MSC_INPP2_IN_0            ((uint8_t) (0x01 << 0))   ///< port 2 [0] input signal [0]
  #define _MSC_INPP2_IN_1            ((uint8_t) (0x01 << 1))   ///< port 2 [1] input signal [0]
  #define _MSC_INPP2_IN_2            ((uint8_t) (0x01 << 2))   ///< port 2 [2] input signal [0]
  #define _MSC_INPP2_IN_3            ((uint8_t) (0x01 << 3))   ///< port 2 [3] input signal [0]
  #define _MSC_INPP2_IN_4            ((uint8_t) (0x01 << 4))   ///< port 2 [4] input signal [0]
  #define _MSC_INPP2_IN_5            ((uint8_t) (0x01 << 5))   ///< port 2 [5] input signal [0]
  // reserved [7:6]

  #if !defined(STNRG) && !defined(STLUX285A)

    /* MSC port 2 input data register (_MSC_INPP2) */
    #define _MSC_DACCTR_DAC0_EN        ((uint8_t) (0x01 << 0))   ///< enable DAC0 and COMP0 [0]
    #define _MSC_DACCTR_DAC1_EN        ((uint8_t) (0x01 << 1))   ///< enable DAC1 and COMP1 [0]
    #define _MSC_DACCTR_DAC2_EN        ((uint8_t) (0x01 << 2))   ///< enable DAC2 and COMP2 [0]
    #define _MSC_DACCTR_DAC3_EN        ((uint8_t) (0x01 << 3))   ///< enable DAC3 [0]
    #define _MSC_DACCTR_CP3_EN         ((uint8_t) (0x01 << 4))   ///< enable COMP3 [0]
    #define _MSC_DACCTR_CP3_SEL        ((uint8_t) (0x01 << 5))   ///< COMP3 reference voltage selection [0]
    // reserved [6]
    #define _MSC_DACCTR_DAC_BIAS_EN    ((uint8_t) (0x01 << 7))   ///< DAC bias enable [0]

  #else // STNRG || STLUX285A

    /* MSC port 2 input data register (_MSC_INPP2) */
    #define _MSC_DACCTR_DAC0_EN        ((uint8_t) (0x01 << 0))   ///< enable DAC0 & CMP0, and select internal reference [0]
    #define _MSC_DACCTR_DAC1_EN        ((uint8_t) (0x01 << 1))   ///< enable DAC1 & CMP1, and select internal reference [0]
    #define _MSC_DACCTR_DAC2_EN        ((uint8_t) (0x01 << 2))   ///< enable DAC2 & CMP2, and select internal reference [0]
    #define _MSC_DACCTR_DAC3_EN        ((uint8_t) (0x01 << 3))   ///< enable DAC3 & CMP3, and select internal reference [0]
    #define _MSC_DACCTR_CP0_EN_EREF    ((uint8_t) (0x01 << 4))   ///< enable CMP0 and select external reference when DAC0_EN=0 [0]
    #define _MSC_DACCTR_CP1_EN_EREF    ((uint8_t) (0x01 << 5))   ///< enable CMP1 and select external reference when DAC1_EN=0 [0]
    #define _MSC_DACCTR_CP2_EN_EREF    ((uint8_t) (0x01 << 6))   ///< enable CMP2 and select external reference when DAC2_EN=0 [0]
    #define _MSC_DACCTR_CP3_EN_EREF    ((uint8_t) (0x01 << 7))   ///< enable CMP3 and select external reference when DAC3_EN=0 [0]

  #endif // STNRG || STLUX285A

  /* MSC DACx input data register (_MSC_DACINx) */
  #define _MSC_DACINX_DAC_IN         ((uint8_t) (0x0F << 0))   ///< DAC input value [3:0]
  #define _MSC_DACINX_DAC_IN0        ((uint8_t) (0x01 << 0))   ///< DAC input value [0]
  #define _MSC_DACINX_DAC_IN1        ((uint8_t) (0x01 << 1))   ///< DAC input value [1]
  #define _MSC_DACINX_DAC_IN2        ((uint8_t) (0x01 << 2))   ///< DAC input value [2]
  #define _MSC_DACINX_DAC_IN3        ((uint8_t) (0x01 << 3))   ///< DAC input value [3]
  // reserved [7:4]

  /* MSC SMED0 & SMED1 global configuration register (_MSC_SMDCFG01) */
  #define _MSC_SMDCFG01_SMD0_DRVOUT   ((uint8_t) (0x01 << 0))   ///< PWM0 control merge output signal in coupled mode [0]
  #define _MSC_SMDCFG01_SMD0_GLBCONF  ((uint8_t) (0x07 << 1))   ///< SMED0 global configuration [2:0]
  #define _MSC_SMDCFG01_SMD0_GLBCONF0 ((uint8_t) (0x01 << 1))   ///< SMED0 global configuration [0]
  #define _MSC_SMDCFG01_SMD0_GLBCONF1 ((uint8_t) (0x01 << 2))   ///< SMED0 global configuration [1]
  #define _MSC_SMDCFG01_SMD0_GLBCONF2 ((uint8_t) (0x01 << 3))   ///< SMED0 global configuration [2]
  #define _MSC_SMDCFG01_SMD1_DRVOUT   ((uint8_t) (0x01 << 4))   ///< PWM1 control merge output signal in coupled mode [0]
  #define _MSC_SMDCFG01_SMD1_GLBCONF  ((uint8_t) (0x07 << 5))   ///< SMED1 global configuration [2:0]
  #define _MSC_SMDCFG01_SMD1_GLBCONF0 ((uint8_t) (0x01 << 5))   ///< SMED1 global configuration [0]
  #define _MSC_SMDCFG01_SMD1_GLBCONF1 ((uint8_t) (0x01 << 6))   ///< SMED1 global configuration [1]
  #define _MSC_SMDCFG01_SMD1_GLBCONF2 ((uint8_t) (0x01 << 7))   ///< SMED1 global configuration [2]

  /* MSC SMED2 & SMED3 global configuration register (_MSC_SMDCFG23) */
  #define _MSC_SMDCFG23_SMD2_DRVOUT   ((uint8_t) (0x01 << 0))   ///< PWM2 control merge output signal in coupled mode [0]
  #define _MSC_SMDCFG23_SMD2_GLBCONF  ((uint8_t) (0x07 << 1))   ///< SMED2 global configuration [2:0]
  #define _MSC_SMDCFG23_SMD2_GLBCONF0 ((uint8_t) (0x01 << 1))   ///< SMED2 global configuration [0]
  #define _MSC_SMDCFG23_SMD2_GLBCONF1 ((uint8_t) (0x01 << 2))   ///< SMED2 global configuration [1]
  #define _MSC_SMDCFG23_SMD2_GLBCONF2 ((uint8_t) (0x01 << 3))   ///< SMED2 global configuration [2]
  #define _MSC_SMDCFG23_SMD3_DRVOUT   ((uint8_t) (0x01 << 4))   ///< PWM3 control merge output signal in coupled mode [0]
  #define _MSC_SMDCFG23_SMD3_GLBCONF  ((uint8_t) (0x07 << 5))   ///< SMED3 global configuration [2:0]
  #define _MSC_SMDCFG23_SMD3_GLBCONF0 ((uint8_t) (0x01 << 5))   ///< SMED3 global configuration [0]
  #define _MSC_SMDCFG23_SMD3_GLBCONF1 ((uint8_t) (0x01 << 6))   ///< SMED3 global configuration [1]
  #define _MSC_SMDCFG23_SMD3_GLBCONF2 ((uint8_t) (0x01 << 7))   ///< SMED3 global configuration [2]

  /* MSC SMED4 & SMED5 global configuration register (_MSC_SMDCFG45) */
  #define _MSC_SMDCFG45_SMD4_DRVOUT   ((uint8_t) (0x01 << 0))   ///< PWM4 control merge output signal in coupled mode [0]
  #define _MSC_SMDCFG45_SMD4_GLBCONF  ((uint8_t) (0x07 << 1))   ///< SMED4 global configuration [2:0]
  #define _MSC_SMDCFG45_SMD4_GLBCONF0 ((uint8_t) (0x01 << 1))   ///< SMED4 global configuration [0]
  #define _MSC_SMDCFG45_SMD4_GLBCONF1 ((uint8_t) (0x01 << 2))   ///< SMED4 global configuration [1]
  #define _MSC_SMDCFG45_SMD4_GLBCONF2 ((uint8_t) (0x01 << 3))   ///< SMED4 global configuration [2]
  #define _MSC_SMDCFG45_SMD5_DRVOUT   ((uint8_t) (0x01 << 4))   ///< PWM5 control merge output signal in coupled mode [0]
  #define _MSC_SMDCFG45_SMD5_GLBCONF  ((uint8_t) (0x07 << 5))   ///< SMED5 global configuration [2:0]
  #define _MSC_SMDCFG45_SMD5_GLBCONF0 ((uint8_t) (0x01 << 5))   ///< SMED5 global configuration [0]
  #define _MSC_SMDCFG45_SMD5_GLBCONF1 ((uint8_t) (0x01 << 6))   ///< SMED5 global configuration [1]
  #define _MSC_SMDCFG45_SMD5_GLBCONF2 ((uint8_t) (0x01 << 7))   ///< SMED5 global configuration [2]

  /* MSC SMEDs software events (_MSC_SMSWEV) */
  #define _MSC_SMSWEV_SW0         ((uint8_t) (0x01 << 0))   ///< software event on SMED0 [0]
  #define _MSC_SMSWEV_SW1         ((uint8_t) (0x01 << 1))   ///< software event on SMED1 [0]
  #define _MSC_SMSWEV_SW2         ((uint8_t) (0x01 << 2))   ///< software event on SMED2 [0]
  #define _MSC_SMSWEV_SW3         ((uint8_t) (0x01 << 3))   ///< software event on SMED3 [0]
  #define _MSC_SMSWEV_SW4         ((uint8_t) (0x01 << 4))   ///< software event on SMED4 [0]
  #define _MSC_SMSWEV_SW5         ((uint8_t) (0x01 << 5))   ///< software event on SMED5 [0]
  // reserved [7:6]

  /* MSC SMEDs unlock register (_MSC_SMUNLOCK) */
  #define _MSC_SMUNLOCK_USE_UNLOCK_01 ((uint8_t) (0x01 << 0))   ///< enable unlock for SMED0/1 (only in coupled mode)[0]
  #define _MSC_SMUNLOCK_USE_UNLOCK_23 ((uint8_t) (0x01 << 1))   ///< enable unlock for SMED2/3 (only in coupled mode)[0]
  #define _MSC_SMUNLOCK_USE_UNLOCK_45 ((uint8_t) (0x01 << 2))   ///< enable unlock for SMED4/5 (only in coupled mode)[0]
  #define _MSC_SMUNLOCK_UNLOCK_01     ((uint8_t) (0x01 << 3))   ///< unlock SMED0/1  (only in coupled mode)[0]
  #define _MSC_SMUNLOCK_UNLOCK_23     ((uint8_t) (0x01 << 4))   ///< unlock SMED2/3  (only in coupled mode)[0]
  #define _MSC_SMUNLOCK_UNLOCK_45     ((uint8_t) (0x01 << 5))   ///< unlock SMED4/5  (only in coupled mode)[0]
  // reserved [7:6]

  /* MSC connection matrix selection for SMEDx (_MSC_CBOXSx) */
  #define __MSC_CBOXSX_CONB_S0_0  ((uint8_t) (0x03 << 0))   ///< connection box selection line for SMEDx input InSig[0] [1:0]
  #define __MSC_CBOXSX_CONB_S0_00 ((uint8_t) (0x01 << 0))   ///< connection box selection line for SMEDx input InSig[0] [0]
  #define __MSC_CBOXSX_CONB_S0_01 ((uint8_t) (0x01 << 1))   ///< connection box selection line for SMEDx input InSig[0] [1]
  #define __MSC_CBOXSX_CONB_S0_1  ((uint8_t) (0x03 << 2))   ///< connection box selection line for SMEDx input InSig[1] [1:0]
  #define __MSC_CBOXSX_CONB_S0_10 ((uint8_t) (0x01 << 2))   ///< connection box selection line for SMEDx input InSig[1] [0]
  #define __MSC_CBOXSX_CONB_S0_11 ((uint8_t) (0x01 << 3))   ///< connection box selection line for SMEDx input InSig[1] [1]
  #define __MSC_CBOXSX_CONB_S0_2  ((uint8_t) (0x03 << 4))   ///< connection box selection line for SMEDx input InSig[2] [1:0]
  #define __MSC_CBOXSX_CONB_S0_20 ((uint8_t) (0x01 << 4))   ///< connection box selection line for SMEDx input InSig[2] [0]
  #define __MSC_CBOXSX_CONB_S0_21 ((uint8_t) (0x01 << 5))   ///< connection box selection line for SMEDx input InSig[2] [1]
  // reserved [7:6]

  /* MSC SMED I/O MUX control register (_MSC_IOMXSMD) */
  #define _MSC_IOMXSMD_SMD_FSMSL0  ((uint8_t) (0x07 << 0))  ///< SMED FSM status multiplexing output selection line for P0[2:0] [2:0]
  #define _MSC_IOMXSMD_SMD_FSMSL00 ((uint8_t) (0x01 << 0))  ///< SMED FSM status multiplexing output selection line for P0[2:0] [0]
  #define _MSC_IOMXSMD_SMD_FSMSL01 ((uint8_t) (0x01 << 1))  ///< SMED FSM status multiplexing output selection line for P0[2:0] [1]
  #define _MSC_IOMXSMD_SMD_FSMSL02 ((uint8_t) (0x01 << 2))  ///< SMED FSM status multiplexing output selection line for P0[2:0] [2]
  #define _MSC_IOMXSMD_SEL_FSMEN0  ((uint8_t) (0x01 << 3))  ///< enable SMED FSM status signals multiplexing on P0[2:0] [0]
  #define _MSC_IOMXSMD_SMD_FSMSL1  ((uint8_t) (0x07 << 4))  ///< SMED FSM status multiplexing output selection line for P0[5:3] [2:0]
  #define _MSC_IOMXSMD_SMD_FSMSL10 ((uint8_t) (0x01 << 4))  ///< SMED FSM status multiplexing output selection line for P0[5:3] [0]
  #define _MSC_IOMXSMD_SMD_FSMSL11 ((uint8_t) (0x01 << 5))  ///< SMED FSM status multiplexing output selection line for P0[5:3] [1]
  #define _MSC_IOMXSMD_SMD_FSMSL12 ((uint8_t) (0x01 << 6))  ///< SMED FSM status multiplexing output selection line for P0[5:3] [2]
  #define _MSC_IOMXSMD_SEL_FSMEN1  ((uint8_t) (0x01 << 7))  ///< enable SMED FSM status signals multiplexing on P0[5:3] [0]

  /* MSC port 3 (COMP) input register (_MSC_INPP3) */
  #define _MSC_INPP3_COMP_0        ((uint8_t) (0x01 << 0))   ///< COMP0 output signal [0]
  #define _MSC_INPP3_COMP_1        ((uint8_t) (0x01 << 1))   ///< COMP1 output signal [0]
  #define _MSC_INPP3_COMP_2        ((uint8_t) (0x01 << 2))   ///< COMP2 output signal [0]
  #define _MSC_INPP3_COMP_3        ((uint8_t) (0x01 << 3))   ///< COMP3 output signal [0]
  // reserved [7:3]

  /* MSC port P0 alternate function MUX control register (_MSC_IOMXP0) */
  #define _MSC_IOMXP0_SEL_P010     ((uint8_t) (0x03 << 0))   ///< Port0 [1:0] I/O multiplexing scheme [1:0]
  #define _MSC_IOMXP0_SEL_P010_0   ((uint8_t) (0x01 << 0))   ///< Port0 [1:0] I/O multiplexing scheme [0]
  #define _MSC_IOMXP0_SEL_P010_1   ((uint8_t) (0x01 << 1))   ///< Port0 [1:0] I/O multiplexing scheme [1]
  #define _MSC_IOMXP0_SEL_P032     ((uint8_t) (0x03 << 2))   ///< Port0 [3:2] I/O multiplexing scheme [1:0]
  #define _MSC_IOMXP0_SEL_P032_0   ((uint8_t) (0x01 << 2))   ///< Port0 [3:2] I/O multiplexing scheme [0]
  #define _MSC_IOMXP0_SEL_P032_1   ((uint8_t) (0x01 << 3))   ///< Port0 [3:2] I/O multiplexing scheme [1]
  #define _MSC_IOMXP0_SEL_P054     ((uint8_t) (0x03 << 4))   ///< Port0 [5:4] I/O multiplexing scheme [1:0]
  #define _MSC_IOMXP0_SEL_P054_0   ((uint8_t) (0x01 << 4))   ///< Port0 [5:4] I/O multiplexing scheme [0]
  #define _MSC_IOMXP0_SEL_P054_1   ((uint8_t) (0x01 << 5))   ///< Port0 [5:4] I/O multiplexing scheme [1]
  // reserved [7:6]

  /* MSC port P1 alternate function MUX control register (_MSC_IOMXP1) */
  #define _MSC_IOMXP1_SEL_P10      ((uint8_t) (0x01 << 0))   ///< Port1 [0] I/O multiplexing scheme [0]
  #define _MSC_IOMXP1_SEL_P11      ((uint8_t) (0x01 << 1))   ///< Port1 [1] I/O multiplexing scheme [0]
  #define _MSC_IOMXP1_SEL_P12      ((uint8_t) (0x01 << 2))   ///< Port1 [2] I/O multiplexing scheme [0]
  #define _MSC_IOMXP1_SEL_P13      ((uint8_t) (0x01 << 3))   ///< Port1 [3] I/O multiplexing scheme [0]
  #define _MSC_IOMXP1_SEL_P14      ((uint8_t) (0x01 << 4))   ///< Port1 [4] I/O multiplexing scheme [0]
  #define _MSC_IOMXP1_SEL_P15      ((uint8_t) (0x01 << 5))   ///< Port1 [5] I/O multiplexing scheme [0]
  // reserved [7:6]

  // basic timer 0 & 1 only for STNRG
  #if defined(STNRG)

    /* MSC basic timer 1/0 source clock selection (_MSC_FTM0CKSEL, indirect access via _MSC_IDXADD=0x00) */
    #define _MSC_FTM0CKSEL_CLK_SEL0  ((uint8_t) (0x07 << 0))   ///< Basic Timer 0 clock source configuration [2:0]
    #define _MSC_FTM0CKSEL_CLK_SEL00 ((uint8_t) (0x01 << 0))   ///< Basic Timer 0 clock source configuration [0]
    #define _MSC_FTM0CKSEL_CLK_SEL01 ((uint8_t) (0x01 << 1))   ///< Basic Timer 0 clock source configuration [1]
    #define _MSC_FTM0CKSEL_CLK_SEL02 ((uint8_t) (0x01 << 2))   ///< Basic Timer 0 clock source configuration [2]
    #define _MSC_FTM0CKSEL_EN0       ((uint8_t) (0x01 << 3))   ///< Basic Timer 0 enable [0]
    #define _MSC_FTM0CKSEL_CLK_SEL1  ((uint8_t) (0x07 << 0))   ///< Basic Timer 1 clock source configuration [2:0]
    #define _MSC_FTM0CKSEL_CLK_SEL10 ((uint8_t) (0x01 << 0))   ///< Basic Timer 1 clock source configuration [0]
    #define _MSC_FTM0CKSEL_CLK_SEL11 ((uint8_t) (0x01 << 1))   ///< Basic Timer 1 clock source configuration [1]
    #define _MSC_FTM0CKSEL_CLK_SEL12 ((uint8_t) (0x01 << 2))   ///< Basic Timer 1 clock source configuration [2]
    #define _MSC_FTM0CKSEL_EN1       ((uint8_t) (0x01 << 3))   ///< Basic Timer 1 enable [0]

    /* MSC basic timer 1 counter value (_MSC_FTM1CONF, indirect access via _MSC_IDXADD=0x04) */
    #define _MSC_FTM1CONF_ADC_AFLUSH  ((uint8_t) (0x01 << 3)) ///< ADC FIFO auto-flush for single conversion mode [0]
    #define _MSC_FTM1CONF_ADC_ARELOAD ((uint8_t) (0x01 << 3)) ///< ADC FIFO auto-reload enable (requires ADC_CFG.CIRCULAR=1) [0]

  #endif // STNRG

  /* MSC INPP2 aux. register 1 (_MSC_INPP2AUX1, indirect access via _MSC_IDXADD=0x08) */
  #define _MSC_INPP2AUX1_PULLUP0   ((uint8_t) (0x01 << 0))   ///< INPP2[0] pull-up enable [0]
  #define _MSC_INPP2AUX1_PULLUP1   ((uint8_t) (0x01 << 1))   ///< INPP2[1] pull-up enable [0]
  #define _MSC_INPP2AUX1_PULLUP2   ((uint8_t) (0x01 << 2))   ///< INPP2[2] pull-up enable [0]
  #define _MSC_INPP2AUX1_PULLUP3   ((uint8_t) (0x01 << 3))   ///< INPP2[3] pull-up enable [0]
  #define _MSC_INPP2AUX1_PULLUP4   ((uint8_t) (0x01 << 4))   ///< INPP2[4] pull-up enable [0]
  #define _MSC_INPP2AUX1_PULLUP5   ((uint8_t) (0x01 << 5))   ///< INPP2[5] pull-up enable [0]
  #if defined(STNRG)
    // reserved [6]
    #define _MSC_INPP2AUX1_ADCTRG_EN ((uint8_t) (0x01 << 7))   ///< ADC HW triggered conversion request [0]
  #else
    // reserved [7:6]
  #endif

  /* MSC INPP2 aux. register 2 (_MSC_INPP2AUX2, indirect access via _MSC_IDXADD=0x09) */
  #define _MSC_INPP2AUX2_INPP2_IMSK0 ((uint8_t) (0x01 << 0)) ///< INPP2[0] interrupt mask enable [0]
  #define _MSC_INPP2AUX2_INPP2_IMSK1 ((uint8_t) (0x01 << 1)) ///< INPP2[1] interrupt mask enable [0]
  #define _MSC_INPP2AUX2_INPP2_IMSK2 ((uint8_t) (0x01 << 2)) ///< INPP2[2] interrupt mask enable [0]
  #define _MSC_INPP2AUX2_INPP2_IMSK3 ((uint8_t) (0x01 << 3)) ///< INPP2[3] interrupt mask enable [0]
  #define _MSC_INPP2AUX2_INPP2_IMSK4 ((uint8_t) (0x01 << 4)) ///< INPP2[4] interrupt mask enable [0]
  #define _MSC_INPP2AUX2_INPP2_IMSK5 ((uint8_t) (0x01 << 5)) ///< INPP2[5] interrupt mask enable [0]
  #define _MSC_INPP2AUX2_INPP0_IMSK  ((uint8_t) (0x01 << 6)) ///< INPP0 interrupt mask enable [0]
  // reserved [7]

#endif // MSC_AddressBase



//------------------------
// Non-volative memory (_FLASH)
//------------------------
#if defined(_DOXYGEN) || defined(FLASH_AddressBase)

  /** @brief struct to control write/erase of flash memory (_FLASH) */
  typedef struct {

    /** @brief Flash control register 1 (_FLASH_CR1) */
    struct {
      _BITS   FIX   : 1;    ///< Fixed Byte programming time
      _BITS   IE    : 1;    ///< Flash Interrupt enable
      _BITS   AHALT : 1;    ///< Power-down in Active-halt mode
      _BITS   HALT  : 1;    ///< Power-down in Halt mode
      _BITS         : 4;    //   Reserved
    } CR1;


    /** @brief Flash control register 2 (_FLASH_CR2) */
    struct {
      _BITS   PRG   : 1;    ///< Standard block programming
      _BITS         : 3;    //   Reserved
      _BITS   FPRG  : 1;    ///< Fast block programming
      _BITS   ERASE : 1;    ///< Block erasing
      _BITS   WPRG  : 1;    ///< Word programming
      _BITS   OPT   : 1;    ///< Write option bytes
    } CR2;


    /** @brief Complementary flash control register 2 (_FLASH_NCR2) */
    struct {
      _BITS   NPRG   : 1;   ///< Standard block programming
      _BITS          : 3;   //   Reserved
      _BITS   NFPRG  : 1;   ///< Fast block programming
      _BITS   NERASE : 1;   ///< Block erasing
      _BITS   NWPRG  : 1;   ///< Word programming
      _BITS   NOPT   : 1;   ///< Write option bytes
    } NCR2;


    /** @brief Flash protection register (_FLASH_FPR) */
    struct {
      _BITS   WPB    : 8;   ///< User boot code area protection
    } FPR;


    /** @brief Complementary flash protection register (_FLASH_NFPR) */
    struct {
      _BITS   NWPB   : 8;   ///< User boot code area protection
    } NFPR;


    /** @brief Flash status register (_FLASH_IAPSR) */
    struct {
      _BITS   WR_PG_DIS : 1;  ///< Write attempted to protected page flag
      _BITS   PUL       : 1;  ///< Flash Program memory unlocked flag
      _BITS   EOP       : 1;  ///< End of programming (write or erase operation) flag
      _BITS   DUL       : 1;  ///< Data EEPROM area unlocked flag
      _BITS             : 2;  //   Reserved, forced by hardware to 0
      _BITS   HVOFF     : 1;  ///< End of high voltage flag
      _BITS             : 1;  //   Reserved
    } IAPSR;


    /* Reserved registers (2B) */
    uint8_t res     [2];


    /** @brief Flash program memory unprotecting key register (_FLASH_PUKR) */
    struct {
      _BITS   WP    : 8;   ///< Program memory write unlock key
    } PUKR;


    /** @brief Reserved register (1B) */
    uint8_t res2    [1];


    /** @brief Data EEPROM unprotection key register (_FLASH_DUKR) */
    struct {
      _BITS   WD    : 8;   ///< Data EEPROM write unlock key
    } DUKR;


    /* Reserved registers (2B) */
    uint8_t res3    [2];


    /** @brief Flash wait state register (_FLASH_WAIT) */
    struct {
      _BITS   WAIT  : 2;   ///< Data EEPROM write unlock key
      _BITS         : 6;   //   Reserved
    } WAIT;

  } _FLASH_t;

  /* Pointer to flash registers */
  #define _FLASH        _SFR(_FLASH_t, FLASH_AddressBase)        ///< Flash struct/bit access
  #define _FLASH_CR1    _SFR(uint8_t,  FLASH_AddressBase+0x00)   ///< Flash control register 1
  #define _FLASH_CR2    _SFR(uint8_t,  FLASH_AddressBase+0x01)   ///< Flash control register 2
  #define _FLASH_NCR2   _SFR(uint8_t,  FLASH_AddressBase+0x02)   ///< Flash Complementary control register 2
  #define _FLASH_FRP    _SFR(uint8_t,  FLASH_AddressBase+0x03)   ///< Flash protection register
  #define _FLASH_NFRP   _SFR(uint8_t,  FLASH_AddressBase+0x04)   ///< Flash Complementary protection register
  #define _FLASH_IAPSR  _SFR(uint8_t,  FLASH_AddressBase+0x05)   ///< Flash status register
  // reserved (2B)
  #define _FLASH_PUKR   _SFR(uint8_t,  FLASH_AddressBase+0x08)   ///< Flash program memory unprotecting key register
  // reserved (1B)
  #define _FLASH_DUKR   _SFR(uint8_t,  FLASH_AddressBase+0x0A)   ///< Data EEPROM unprotection key register
  // reserved (2B)
  #define _FLASH_WAIT   _SFR(uint8_t,  FLASH_AddressBase+0x0D)   ///< Flash Wait state register


  /* FLASH Module Reset Values */
  #define _FLASH_CR1_RESET_VALUE       ((uint8_t) 0x00)          ///< Flash control register 1 reset value
  #define _FLASH_CR2_RESET_VALUE       ((uint8_t) 0x00)          ///< Flash control register 2 reset value
  #define _FLASH_NCR2_RESET_VALUE      ((uint8_t) 0xFF)          ///< complementary Flash control register 2 reset value
  #define _FLASH_FRR_RESET_VALUE       ((uint8_t) 0x00)          ///< Flash protection register reset value
  #define _FLASH_NFRR_RESET_VALUE      ((uint8_t) 0xFF)          ///< Flash Complementary protection register reset value
  #define _FLASH_IAPSR_RESET_VALUE     ((uint8_t) 0x40)          ///< Flash status register reset value
  #define _FLASH_PUKR_RESET_VALUE      ((uint8_t) 0x00)          ///< Flash program memory unprotecting key reset value
  #define _FLASH_DUKR_RESET_VALUE      ((uint8_t) 0x00)          ///< Data EEPROM unprotection key reset value
  #define _FLASH_WAIT_RESET_VALUE      ((uint8_t) 0x00)          ///< Flash Wait state register reset value


  /* Flash control register 1 (_FLASH_CR1) */
  #define _FLASH_CR1_FIX               ((uint8_t) (0x01 << 0))   ///< Fixed Byte programming time [0]
  #define _FLASH_CR1_IE                ((uint8_t) (0x01 << 1))   ///< Flash Interrupt enable [0]
  #define _FLASH_CR1_AHALT             ((uint8_t) (0x01 << 2))   ///< Power-down in Active-halt mode [0]
  #define _FLASH_CR1_HALT              ((uint8_t) (0x01 << 3))   ///< Power-down in Halt mode [0]
  // reserved [7:4]

  /* Flash control register 2 and complement (_FLASH_CR2 and _FLASH_NCR2) */
  #define _FLASH_CR2_PRG               ((uint8_t) (0x01 << 0))   ///< Standard block programming [0] (in _FLASH_CR2 and _FLASH_NCR2)
  // reserved [3:1]
  #define _FLASH_CR2_FPRG              ((uint8_t) (0x01 << 4))   ///< Fast block programming [0] (in _FLASH_CR2 and _FLASH_NCR2)
  #define _FLASH_CR2_ERASE             ((uint8_t) (0x01 << 5))   ///< Block erasing [0] (in _FLASH_CR2 and _FLASH_NCR2)
  #define _FLASH_CR2_WPRG              ((uint8_t) (0x01 << 6))   ///< Word programming [0] (in _FLASH_CR2 and _FLASH_NCR2)
  #define _FLASH_CR2_OPT               ((uint8_t) (0x01 << 7))   ///< Write option bytes [0] (in _FLASH_CR2 and _FLASH_NCR2)

  /* Flash status register (_FLASH_IAPSR) */
  #define _FLASH_IAPSR_WR_PG_DIS       ((uint8_t) (0x01 << 0))   ///< Write attempted to protected page flag [0]
  #define _FLASH_IAPSR_PUL             ((uint8_t) (0x01 << 1))   ///< Flash Program memory unlocked flag [0]
  #define _FLASH_IAPSR_EOP             ((uint8_t) (0x01 << 2))   ///< End of programming (write or erase operation) flag [0]
  #define _FLASH_IAPSR_DUL             ((uint8_t) (0x01 << 3))   ///< Data EEPROM area unlocked flag [0]
  // reserved [5:4]
  #define _FLASH_IAPSR_HVOFF           ((uint8_t) (0x01 << 6))   ///< End of high voltage flag [0]
  // reserved [7]

  /* Flash wait state register (_FLASH_WAIT) */
  #define _FLASH_WAIT_WAIT             ((uint8_t) (0x03 << 0))   ///< Flash wait state [1:0]
  #define _FLASH_WAIT_WAIT0            ((uint8_t) (0x01 << 0))   ///< Flash wait state [0]
  #define _FLASH_WAIT_WAIT1            ((uint8_t) (0x01 << 1))   ///< Flash wait state [1]
  // reserved [7:2]

#endif // FLASH_AddressBase



//------------------------
// Reset status (_RST)
//------------------------
#if defined(_DOXYGEN) || defined(RST_AddressBase)

  /** @brief struct for determining reset source (_RST) */
  typedef struct {

    /** @brief Reset status register (_RST_SR) */
    struct {
      _BITS   WWDGF   : 1;    ///< Window Watchdog reset flag
      _BITS   IWDGF   : 1;    ///< Independent Watchdog reset flag
      _BITS   ILLOPF  : 1;    ///< Illegal opcode reset flag
      _BITS   SWIMF   : 1;    ///< SWIM reset flag
      _BITS   EMCF    : 1;    ///< EMC reset flag
      _BITS           : 3;    //   Reserved
    } SR;

  } _RST_t;

  /* Pointer to reset status register */
  #define _RST     _SFR(_RST_t,        RST_AddressBase)          ///< Reset module struct/bit access
  #define _RST_SR  _SFR(uint8_t,       RST_AddressBase+0x00)     ///< Reset module status register

  /*  Reset module status register (_RST_SR) */
  #define _RST_SR_WWDGF                ((uint8_t) (0x01 << 0))   ///< Window Watchdog reset flag [0]
  #define _RST_SR_IWDGF                ((uint8_t) (0x01 << 1))   ///< Independent Watchdog reset flag [0]
  #define _RST_SR_ILLOPF               ((uint8_t) (0x01 << 2))   ///< Illegal opcode reset flag [0]
  #define _RST_SR_SWIMF                ((uint8_t) (0x01 << 3))   ///< SWIM reset flag [0]
  #define _RST_SR_EMCF                 ((uint8_t) (0x01 << 4))   ///< EMC reset flag [0]
  // reserved [7:5]

#endif // RST_AddressBase



//------------------------
// Clock control (_CLK)
//------------------------
#if defined(_DOXYGEN) || defined(CLK_AddressBase)

  /** @brief struct for configuring/monitoring clock module (_CLK) */
  typedef struct {

    /** @brief SMED0 clock configiguration (_CLK_SMD0) */
    struct {
      _BITS   CK_SW    : 2;    ///< High speed internal RC oscillator enable
      _BITS            : 2;    //   Reserved
      _BITS   SMED_DIV : 3;    ///< SMED division factor
      _BITS            : 1;    //   Reserved
    } SMD0;


    /** @brief SMED1 clock configiguration (_CLK_SMD1) */
    struct {
      _BITS   CK_SW    : 2;    ///< High speed internal RC oscillator enable
      _BITS            : 2;    //   Reserved
      _BITS   SMED_DIV : 3;    ///< SMED division factor
      _BITS            : 1;    //   Reserved
    } SMD1;


    /** @brief SMED2 clock configiguration (_CLK_SMD2) */
    struct {
      _BITS   CK_SW    : 2;    ///< High speed internal RC oscillator enable
      _BITS            : 2;    //   Reserved
      _BITS   SMED_DIV : 3;    ///< SMED division factor
      _BITS            : 1;    //   Reserved
    } SMD2;


    /** @brief SMED3 clock configiguration (_CLK_SMD3) */
    struct {
      _BITS   CK_SW    : 2;    ///< High speed internal RC oscillator enable
      _BITS            : 2;    //   Reserved
      _BITS   SMED_DIV : 3;    ///< SMED division factor
      _BITS            : 1;    //   Reserved
    } SMD3;


    /** @brief SMED4 clock configiguration (_CLK_SMD4) */
    struct {
      _BITS   CK_SW    : 2;    ///< High speed internal RC oscillator enable
      _BITS            : 2;    //   Reserved
      _BITS   SMED_DIV : 3;    ///< SMED division factor
      _BITS            : 1;    //   Reserved
    } SMD4;


    /** @brief SMED5 clock configiguration (_CLK_SMD5) */
    struct {
      _BITS   CK_SW    : 2;    ///< High speed internal RC oscillator enable
      _BITS            : 2;    //   Reserved
      _BITS   SMED_DIV : 3;    ///< SMED division factor
      _BITS            : 1;    //   Reserved
    } SMD5;


    /* Reserved registers (4B) */
    uint8_t res     [4];


    /** @brief PLL divider/prescaler register (_CLK_PLLDIV) */
    struct {
      _BITS   PLL_DIV      : 2;    ///< PLL at 96 MHz clock division factor (CLK PLL_DIV = CLK PLL/(4+n))
      _BITS                : 1;    //   Reserved, must be kept cleared
      _BITS   PLL_PRES_DIV : 2;    ///< PLL clock prescaled factor (CLK PLL_PRES_DIV = CLK PLL_DIV/2^n)
      _BITS                : 3;    //   Reserved, must be kept cleared
    } PLLDIV;


    /** @brief AWU clock prescaler configuration (_CLK_AWUDIV) */
    struct {
      _BITS   AWUDIV    : 4;    ///< AWU clock post divider
      _BITS             : 3;    //   Reserved, must be kept cleared
    } AWUDIV;


    /** @brief Internal clock register (_CLK_ICKR) */
    struct {
      _BITS   HSIEN   : 1;    ///< High speed internal RC oscillator enable
      _BITS   HSIRDY  : 1;    ///< High speed internal oscillator ready flag
      _BITS   FHWU    : 1;    ///< Fast wakeup from Halt/Active-halt modes enable
      _BITS   LSIEN   : 1;    ///< Low speed internal RC oscillator enable
      _BITS   LSIRDY  : 1;    ///< Low speed internal oscillator ready flag
      _BITS   REGAH   : 1;    ///< Regulator power off in Active-halt mode enable
      _BITS           : 2;    //   Reserved, must be kept cleared
    } ICKR;


    /** @brief External clock register (_CLK_ECKR) */
    struct {
      _BITS   HSEEN   : 1;    ///< High speed external crystal oscillator enable
      _BITS   HSERDY  : 1;    ///< High speed external crystal oscillator ready
      _BITS           : 6;    //   Reserved, must be kept cleared
    } ECKR;


    /** @brief PLL status register (_CLK_PLLR) */
    struct {
      _BITS   PLLON        : 1;    ///< PLL power-down
      _BITS   LOCKP        : 1;    ///< PLL lock signal
      _BITS   REF_SEL      : 1;    ///< PLL clock input reference clock selection
      _BITS   BYPASS       : 1;    ///< PLL bypass
      _BITS   SSCG_CTRL    : 1;    ///< Spread spectrum control modulation
      _BITS   SPREAD_CTRL  : 1;    ///< PLL spread input
      _BITS   PLL_LOCK_INT : 1;    ///< PLL unlock interrupt enable
      _BITS                : 1;    //   Reserved, must be kept cleared
    } PLLR;


    /** @brief Clock master status register (_CLK_CMSR) */
    struct {
      _BITS   CKM     : 4;    ///< Clock master status
      _BITS   NCKM    : 4;    ///< Complementary clock master status
    } CMSR;


    /** @brief Clock master switch register (_CLK_SWR) */
    struct {
      _BITS   SWI     : 4;    ///< Clock master selection
      _BITS   NSWI    : 4;    ///< Complementary clock master selection
    } SWR;


    /** @brief Switch control register (_CLK_SWCR) */
    struct {
      _BITS   SWBSY   : 1;    ///< Switch busy flag
      _BITS   SWEN    : 1;    ///< Switch start/stop enable
      _BITS   SWIEN   : 1;    ///< Clock switch interrupt enable
      _BITS   SWIF    : 1;    ///< Clock switch interrupt flag
      _BITS           : 4;    //   Reserved
    } SWCR;


    /** @brief Clock divider register (_CLK_CKDIVR) */
    struct {
      _BITS   CPUDIV  : 3;    ///< CPU clock prescaler
      _BITS   HSIDIV  : 2;    ///< High speed internal clock prescaler
      _BITS           : 3;    //   Reserved, must be kept cleared.
    } CKDIVR;


    /** @brief Peripheral clock gating register 1 (_CLK_PCKENR1) */
    struct {
      _BITS   PCKEN_I2C    : 1;    ///< clock enable I2C
      _BITS   PCKEN_PORT0  : 1;    ///< clock enable PORT0
      _BITS   PCKEN_UART   : 1;    ///< clock enable UART
      _BITS   PCKEN_DALI   : 1;    ///< clock enable DALI
      _BITS   PCKEN_STMR   : 1;    ///< clock enable STMR
      _BITS   PCKEN_PORT1  : 1;    ///< clock enable PORT1
      _BITS   PCKEN_AWU    : 1;    ///< clock enable AWU
      _BITS   PCKEN_ADC    : 1;    ///< clock enable ADC
    } PCKENR1;


    /** @brief Clock security system register (_CLK_CSSR) */
    struct {
      _BITS   CSSEN   : 1;    ///< Clock security system enable
      _BITS   AUX     : 1;    ///< Auxiliary oscillator connected to master clock
      _BITS   CSSDIE  : 1;    ///< Clock security system detection interrupt enable
      _BITS   CSSD    : 1;    ///< Clock security system detection
      _BITS           : 4;    //   Reserved, must be kept cleared.
    } CSSR;


    /** @brief Configurable clock output register (_CLK_CCOR) */
    struct {
      _BITS   CCOEN   : 1;    ///< Configurable clock output enable
      _BITS   CCOSEL  : 4;    ///< Configurable clock output selection.
      _BITS   CCORDY  : 1;    ///< Configurable clock output ready
      _BITS   CCOBSY  : 1;    ///< Configurable clock output busy
      _BITS           : 1;    //   Reserved, must be kept cleared.
    } CCOR;


    /** @brief Peripheral clock gating register 2 (_CLK_PCKENR2) */
    struct {
      _BITS   PCKEN_SMED0 : 1;  ///< clock enable SMED0
      _BITS   PCKEN_SMED1 : 1;  ///< clock enable SMED1
      _BITS   PCKEN_SMED2 : 1;  ///< clock enable SMED2
      _BITS   PCKEN_SMED3 : 1;  ///< clock enable SMED3
      _BITS   PCKEN_SMED4 : 1;  ///< clock enable SMED4
      _BITS   PCKEN_SMED5 : 1;  ///< clock enable SMED5
      _BITS               : 1;  //   Reserved
      _BITS   PCKEN_MISC  : 1;  ///< clock enable MISC
    } PCKENR2;


    /** @brief Reserved register (1B) */
    uint8_t res2    [1];


    /** @brief HSI clock calibration trimming register (_CLK_HSITRIMR) */
    struct {
      _BITS   HSITRIM : 3;    ///< HSI trimming value
      _BITS           : 5;    //   Reserved, must be kept cleared.
    } HSITRIMR;


    /** @brief SWIM clock control register (_CLK_SWIMCCR) */
    struct {
      _BITS   SWIMCLK : 1;    ///< SWIM clock divider
      _BITS           : 7;    //   Reserved.
    } SWIMCCR;


    /** @brief CCO divider register (_CLK_CCODIVR) */
    struct {
      _BITS   CCODIV  : 8;    ///< Divisor for CCO clock (_CLK_CCO = CLK/(n+1))
    } CCODIVR;


    /** @brief ADC clock configuration register (_CLK_ADCR) */
    struct {
      _BITS   SEL     : 2;    ///< ADC clock selection
      _BITS           : 2;    //   Reserved.
      _BITS   ADC_DIV : 4;    ///< Divisor for ADC clock (_CLK_ADC = CLK_SEL/(n+1))
    } ADCR;


    /** @brief Reserved register (1B) */
    uint8_t res3    [1];

  } CLK_t;

  /* Pointer to CLK registers */
  #define _CLK          _SFR(_CLK_t,   CLK_AddressBase)          ///< Clock module struct/bit access
  #define _CLK_SMD0     _SFR(uint8_t,  CLK_AddressBase+0x00)     ///< SMED0 RTC clock config. register
  #define _CLK_SMD1     _SFR(uint8_t,  CLK_AddressBase+0x01)     ///< SMED1 RTC clock config. register
  #define _CLK_SMD2     _SFR(uint8_t,  CLK_AddressBase+0x02)     ///< SMED2 RTC clock config. register
  #define _CLK_SMD3     _SFR(uint8_t,  CLK_AddressBase+0x03)     ///< SMED3 RTC clock config. register
  #define _CLK_SMD4     _SFR(uint8_t,  CLK_AddressBase+0x04)     ///< SMED4 RTC clock config. register
  #define _CLK_SMD5     _SFR(uint8_t,  CLK_AddressBase+0x05)     ///< SMED5 RTC clock config. register
  // reserved (4B)
  #define _CLK_PLLDIV   _SFR(uint8_t,  CLK_AddressBase+0x0A)     ///< PLL divider/prescaler register
  #define _CLK_AWUDIV   _SFR(uint8_t,  CLK_AddressBase+0x0B)     ///< AWU divider register
  #define _CLK_ICKR     _SFR(uint8_t,  CLK_AddressBase+0x0C)     ///< Internal clock register
  #define _CLK_ECKR     _SFR(uint8_t,  CLK_AddressBase+0x0D)     ///< External clock register
  #define _CLK_PLLR     _SFR(uint8_t,  CLK_AddressBase+0x0E)     ///< PLL status register
  #define _CLK_CMSR     _SFR(uint8_t,  CLK_AddressBase+0x0F)     ///< Clock master status register
  #define _CLK_SWR      _SFR(uint8_t,  CLK_AddressBase+0x10)     ///< Clock master switch register
  #define _CLK_SWCR     _SFR(uint8_t,  CLK_AddressBase+0x11)     ///< Clock switch control register
  #define _CLK_CKDIVR   _SFR(uint8_t,  CLK_AddressBase+0x12)     ///< Clock divider register
  #define _CLK_PCKENR1  _SFR(uint8_t,  CLK_AddressBase+0x13)     ///< Peripheral clock gating register 1
  #define _CLK_CSSR     _SFR(uint8_t,  CLK_AddressBase+0x14)     ///< Clock security system register
  #define _CLK_CCOR     _SFR(uint8_t,  CLK_AddressBase+0x15)     ///< Configurable clock output register
  #define _CLK_PCKENR2  _SFR(uint8_t,  CLK_AddressBase+0x16)     ///< Peripheral clock gating register 2
  // reserved (1B)
  #define _CLK_HSITRIMR _SFR(uint8_t,  CLK_AddressBase+0x18)     ///< HSI clock calibration trimming register
  #define _CLK_SWIMCCR  _SFR(uint8_t,  CLK_AddressBase+0x19)     ///< SWIM clock control register
  #define _CLK_CCODIVR  _SFR(uint8_t,  CLK_AddressBase+0x1A)     ///< CCO divider register
  #define _CLK_ADCR     _SFR(uint8_t,  CLK_AddressBase+0x1B)     ///< ADC clock configuration register
  // reserved (1B)


  /* CLK Module Reset Values */
  #define _CLK_SMD0_RESET_VALUE        ((uint8_t) 0x00)          ///< SMED0 RTC clock config. register reset value
  #define _CLK_SMD1_RESET_VALUE        ((uint8_t) 0x00)          ///< SMED1 RTC clock config. register reset value
  #define _CLK_SMD2_RESET_VALUE        ((uint8_t) 0x00)          ///< SMED2 RTC clock config. register reset value
  #define _CLK_SMD3_RESET_VALUE        ((uint8_t) 0x00)          ///< SMED3 RTC clock config. register reset value
  #define _CLK_SMD4_RESET_VALUE        ((uint8_t) 0x00)          ///< SMED4 RTC clock config. register reset value
  #define _CLK_SMD5_RESET_VALUE        ((uint8_t) 0x00)          ///< SMED5 RTC clock config. register reset value
  #define _CLK_PLLDIV_RESET_VALUE      ((uint8_t) 0x00)          ///< PLL divider/prescaler register reset value
  #define _CLK_AWUDIV_RESET_VALUE      ((uint8_t) 0x00)          ///< AWU divider register reset value
  #define _CLK_ICKR_RESET_VALUE        ((uint8_t) 0x01)          ///< Internal clock register reset value
  #define _CLK_ECKR_RESET_VALUE        ((uint8_t) 0x00)          ///< External clock register reset value
  #define _CLK_PLLR_RESET_VALUE        ((uint8_t) 0x01)          ///< PLL status register reset value
  #define _CLK_CMSR_RESET_VALUE        ((uint8_t) 0xE1)          ///< Clock master status register reset value
  #define _CLK_SWR_RESET_VALUE         ((uint8_t) 0xE1)          ///< Clock master switch register reset value
  #define _CLK_SWCR_RESET_VALUE        ((uint8_t) 0x00)          ///< Clock switch control register reset value
  #define _CLK_CKDIVR_RESET_VALUE      ((uint8_t) 0x18)          ///< Clock divider register reset value
  #define _CLK_PCKENR1_RESET_VALUE     ((uint8_t) 0xFF)          ///< Peripheral clock gating register 1 reset value
  #define _CLK_CSSR_RESET_VALUE        ((uint8_t) 0x00)          ///< Clock security system register reset value
  #define _CLK_CCOR_RESET_VALUE        ((uint8_t) 0x00)          ///< Configurable clock output register reset value
  #define _CLK_PCKENR2_RESET_VALUE     ((uint8_t) 0xFF)          ///< Peripheral clock gating register 2 reset value
  #define _CLK_HSITRIMR_RESET_VALUE    ((uint8_t) 0x00)          ///< HSI clock calibration trimming register reset value
  #define _CLK_SWIMCCR_RESET_VALUE     ((uint8_t) 0x00)          ///< SWIM clock control register reset value
  #define _CLK_CCODIVR_RESET_VALUE     ((uint8_t) 0x00)          ///< CCO divider register reset value
  #define _CLK_ADCR_RESET_VALUE        ((uint8_t) 0x20)          ///< ADC clock configuration register reset value


  /* SMEDn clock configiguration (_CLK_SMDn) */
  #define _CLK_SMD_CK_SW               ((uint8_t) (0x03 << 0))   ///< High speed internal RC oscillator enable [1:0]
  #define _CLK_SMD_CK_SW0              ((uint8_t) (0x01 << 0))   ///< High speed internal RC oscillator enable [0]
  #define _CLK_SMD_CK_SW1              ((uint8_t) (0x01 << 1))   ///< High speed internal RC oscillator enable [1]
  // reserved [3:2]
  #define _CLK_SMD_SMED_DIV            ((uint8_t) (0x07 << 4))   ///< SMED division factor [2:0]
  #define _CLK_SMD_SMED_DIV0           ((uint8_t) (0x01 << 4))   ///< SMED division factor [0]
  #define _CLK_SMD_SMED_DIV1           ((uint8_t) (0x01 << 5))   ///< SMED division factor [1]
  #define _CLK_SMD_SMED_DIV2           ((uint8_t) (0x01 << 6))   ///< SMED division factor [2]
  // reserved [7]

  /* PLL divider/prescaler register (_CLK_PLLDIV) */
  #define _CLK_PLLDIV_DIV              ((uint8_t) (0x03 << 0))   ///< PLL at 96 MHz clock division factor [1:0]
  #define _CLK_PLLDIV_DIV0             ((uint8_t) (0x01 << 0))   ///< PLL at 96 MHz clock division factor [0]
  #define _CLK_PLLDIV_DIV1             ((uint8_t) (0x01 << 1))   ///< PLL at 96 MHz clock division factor [1]
  // reserved [2]
  #define _CLK_PLLDIV_PRES_DIV         ((uint8_t) (0x03 << 3))   ///< SMED division factor [2:0]
  #define _CLK_PLLDIV_PRES_DIV0        ((uint8_t) (0x01 << 3))   ///< SMED division factor [0]
  #define _CLK_PLLDIV_PRES_DIV1        ((uint8_t) (0x01 << 4))   ///< SMED division factor [1]
  // reserved [7:5]

  /* AWU clock prescaler configuration (_CLK_AWUDIV) */
  #define _CLK_AWUDIV_DIV              ((uint8_t) (0x0F << 0))   ///< AWU clock post divider [3:0]
  #define _CLK_AWUDIV_DIV0             ((uint8_t) (0x01 << 0))   ///< AWU clock post divider [0]
  #define _CLK_AWUDIV_DIV1             ((uint8_t) (0x01 << 1))   ///< AWU clock post divider [1]
  #define _CLK_AWUDIV_DIV2             ((uint8_t) (0x01 << 2))   ///< AWU clock post divider [2]
  #define _CLK_AWUDIV_DIV3             ((uint8_t) (0x01 << 3))   ///< AWU clock post divider [3]
  // reserved [7:4]

  /* Internal clock register (_CLK_ICKR) */
  #define _CLK_ICKR_HSIEN              ((uint8_t) (0x01 << 0))   ///< High speed internal RC oscillator enable [0]
  #define _CLK_ICKR_HSIRDY             ((uint8_t) (0x01 << 1))   ///< High speed internal oscillator ready [0]
  #define _CLK_ICKR_FHWU               ((uint8_t) (0x01 << 2))   ///< Fast wakeup from Halt/Active-halt modes [0]
  #define _CLK_ICKR_LSIEN              ((uint8_t) (0x01 << 3))   ///< Low speed internal RC oscillator enable [0]
  #define _CLK_ICKR_LSIRDY             ((uint8_t) (0x01 << 4))   ///< Low speed internal oscillator ready [0]
  #define _CLK_ICKR_REGAH              ((uint8_t) (0x01 << 5))   ///< Regulator power off in Active-halt mode [0]
  // reserved [7:6]

  /* External clock register (_CLK_ECKR) */
  #define _CLK_ECKR_HSEEN              ((uint8_t) (0x01 << 0))   ///< High speed external crystal oscillator enable [0]
  #define _CLK_ECKR_HSERDY             ((uint8_t) (0x01 << 1))   ///< High speed external crystal oscillator ready [0]
  // reserved [7:2]

  /* PLL status register (_CLK_PLLR) */
  #define _CLK_PLLR_PLLON              ((uint8_t) (0x01 << 0))   ///< PLL power-down [0]
  #define _CLK_PLLR_LOCKP              ((uint8_t) (0x01 << 1))   ///< PLL lock signal [0]
  #define _CLK_PLLR_REF_SEL            ((uint8_t) (0x01 << 2))   ///< PLL clock input reference clock selection [0]
  #define _CLK_PLLR_BYPASS             ((uint8_t) (0x01 << 3))   ///< PLL bypass [0]
  #define _CLK_PLLR_SSCG_CTRL          ((uint8_t) (0x01 << 4))   ///< Spread spectrum control modulation [0]
  #define _CLK_PLLR_SPREAD_CTRL        ((uint8_t) (0x01 << 5))   ///< PLL spread input [0]
  #define _CLK_PLLR_PLL_LOCK_INT       ((uint8_t) (0x01 << 6))   ///< PLL unlock interrupt enable [0]
  // reserved [7]

  /* Switch control register (_CLK_SWCR) */
  #define _CLK_SWCR_SWBSY              ((uint8_t) (0x01 << 0))   ///< Switch busy flag [0]
  #define _CLK_SWCR_SWEN               ((uint8_t) (0x01 << 1))   ///< Switch start/stop enable [0]
  #define _CLK_SWCR_SWIEN              ((uint8_t) (0x01 << 2))   ///< Clock switch interrupt enable [0]
  #define _CLK_SWCR_SWIF               ((uint8_t) (0x01 << 3))   ///< Clock switch interrupt flag [0]
  // reserved [7:4]

  /* Clock divider register (_CLK_CKDIVR) */
  #define _CLK_CKDIVR_CPUDIV           ((uint8_t) (0x07 << 0))   ///< CPU clock prescaler [2:0]
  #define _CLK_CKDIVR_CPUDIV0          ((uint8_t) (0x01 << 0))   ///< CPU clock prescaler [0]
  #define _CLK_CKDIVR_CPUDIV1          ((uint8_t) (0x01 << 1))   ///< CPU clock prescaler [1]
  #define _CLK_CKDIVR_CPUDIV2          ((uint8_t) (0x01 << 2))   ///< CPU clock prescaler [2]
  #define _CLK_CKDIVR_HSIDIV           ((uint8_t) (0x03 << 3))   ///< High speed internal clock prescaler [1:0]
  #define _CLK_CKDIVR_HSIDIV0          ((uint8_t) (0x01 << 3))   ///< High speed internal clock prescaler [0]
  #define _CLK_CKDIVR_HSIDIV1          ((uint8_t) (0x01 << 4))   ///< High speed internal clock prescaler [1]
  // reserved [7:5]

  /* Peripheral clock gating register 1 (_CLK_PCKENR1) */
  #define _CLK_PCKENR1_I2C             ((uint8_t) (0x01 << 0))   ///< clock enable I2C [0]
  #define _CLK_PCKENR1_PORT0           ((uint8_t) (0x01 << 1))   ///< clock enable PORT0 [0]
  #define _CLK_PCKENR1_UART            ((uint8_t) (0x01 << 2))   ///< clock enable UART [0]
  #define _CLK_PCKENR1_DALI            ((uint8_t) (0x01 << 3))   ///< clock enable DALI [0]
  #define _CLK_PCKENR1_STMR            ((uint8_t) (0x01 << 4))   ///< clock enable STMR [0]
  #define _CLK_PCKENR1_PORT1           ((uint8_t) (0x01 << 5))   ///< clock enable PORT1 [0]
  #define _CLK_PCKENR1_AWU             ((uint8_t) (0x01 << 6))   ///< clock enable AWU [0]
  #define _CLK_PCKENR1_ADC             ((uint8_t) (0x01 << 7))   ///< clock enable ADC [0]

  /* Clock security system register (_CLK_CSSR) */
  #define _CLK_CSSR_CSSEN              ((uint8_t) (0x01 << 0))   ///< Clock security system enable [0]
  #define _CLK_CSSR_AUX                ((uint8_t) (0x01 << 1))   ///< Auxiliary oscillator connected to master clock [0]
  #define _CLK_CSSR_CSSDIE             ((uint8_t) (0x01 << 2))   ///< Clock security system detection interrupt enable [0]
  #define _CLK_CSSR_CSSD               ((uint8_t) (0x01 << 3))   ///< Clock security system detection [0]
  // reserved [7:4]

  /* Configurable clock output register (_CLK_CCOR) */
  #define _CLK_CCOR_CCOEN              ((uint8_t) (0x01 << 0))   ///< Configurable clock output enable [0]
  #define _CLK_CCOR_CCOSEL             ((uint8_t) (0x0F << 1))   ///< Configurable clock output selection [3:0]
  #define _CLK_CCOR_CCOSEL0            ((uint8_t) (0x01 << 1))   ///< Configurable clock output selection [0]
  #define _CLK_CCOR_CCOSEL1            ((uint8_t) (0x01 << 2))   ///< Configurable clock output selection [1]
  #define _CLK_CCOR_CCOSEL2            ((uint8_t) (0x01 << 3))   ///< Configurable clock output selection [2]
  #define _CLK_CCOR_CCOSEL3            ((uint8_t) (0x01 << 4))   ///< Configurable clock output selection [3]
  #define _CLK_CCOR_CCORDY             ((uint8_t) (0x01 << 5))   ///< Configurable clock output ready [0]
  #define _CLK_CCOR_CCOBSY             ((uint8_t) (0x01 << 6))   ///< Configurable clock output busy [0]
  // reserved [7]

  /* Peripheral clock gating register 2 (_CLK_PCKENR2) */
  #define _CLK_PCKENR2_SMED0           ((uint8_t) (0x01 << 0))   ///< clock enable SMED0 [0]
  #define _CLK_PCKENR2_SMED1           ((uint8_t) (0x01 << 1))   ///< clock enable SMED1 [0]
  #define _CLK_PCKENR2_SMED2           ((uint8_t) (0x01 << 2))   ///< clock enable SMED2 [0]
  #define _CLK_PCKENR2_SMED3           ((uint8_t) (0x01 << 3))   ///< clock enable SMED3 [0]
  #define _CLK_PCKENR2_SMED4           ((uint8_t) (0x01 << 4))   ///< clock enable SMED4 [0]
  #define _CLK_PCKENR2_SMED5           ((uint8_t) (0x01 << 5))   ///< clock enable SMED5 [0]
  // reserved [6]
  #define _CLK_PCKENR2_MISC            ((uint8_t) (0x01 << 6))   ///< clock enable MISC [0]

  /* HSI clock calibration trimming register (_CLK_HSITRIMR) */
  #define _CLK_HSITRIMR_TRIM           ((uint8_t) (0x0F << 0))   ///< HSI trimming value [2:0]
  #define _CLK_HSITRIMR_TRIM0          ((uint8_t) (0x01 << 0))   ///< HSI trimming value [0]
  #define _CLK_HSITRIMR_TRIM1          ((uint8_t) (0x01 << 1))   ///< HSI trimming value [1]
  #define _CLK_HSITRIMR_TRIM2          ((uint8_t) (0x01 << 2))   ///< HSI trimming value [2]
  // reserved [7:3]

  /* SWIM clock control register (_CLK_SWIMCCR) */
  #define _CLK_SWIMCCR_SWIMCLK         ((uint8_t) (0x01 << 0))   ///< SWIM clock divider [0]
  // reserved [7:2]

  /* ADC clock configuration register (_CLK_ADCR) */
  #define _CLK_ADCR_SEL                ((uint8_t) (0x03 << 0))    ///< ADC clock selection [1:0]
  #define _CLK_ADCR_SEL0               ((uint8_t) (0x01 << 0))    ///< ADC clock selection [0]
  #define _CLK_ADCR_SEL1               ((uint8_t) (0x01 << 1))    ///< ADC clock selection [1]
  // reserved [3:2]
  #define _CLK_ADCR_ADC_DIV            ((uint8_t) (0x03 << 0))    ///< Divisor for ADC clock [3:0]
  #define _CLK_ADCR_ADC_DIV0           ((uint8_t) (0x01 << 0))    ///< Divisor for ADC clock [0]
  #define _CLK_ADCR_ADC_DIV1           ((uint8_t) (0x01 << 1))    ///< Divisor for ADC clock [1]
  #define _CLK_ADCR_ADC_DIV2           ((uint8_t) (0x01 << 2))    ///< Divisor for ADC clock [2]
  #define _CLK_ADCR_ADC_DIV3           ((uint8_t) (0x01 << 3))    ///< Divisor for ADC clock [3]

#endif // CLK_AddressBase



//------------------------
// Window Watchdog (_WWDG)
//------------------------
#if defined(_DOXYGEN) || defined(WWDG_AddressBase)

  /** @brief struct for access to Window Watchdog registers (_WWDG) */
  typedef struct {

    /** @brief WWDG Control register (_WWDG_CR) */
    struct {
      _BITS   T       : 7;    ///< 7-bit WWDG counter
      _BITS   WDGA    : 1;    ///< WWDG activation (n/a if WWDG enabled by option byte)
    } CR;


    /** @brief WWDR Window register (_WWDG_WR) */
    struct {
      _BITS   W       : 7;    ///< 7-bit window value
      _BITS           : 1;    //   Reserved
    } WR;

  } _WWDG_t;

  /* Pointer to Window Watchdog registers */
  #define _WWDG         _SFR(_WWDG_t,  WWDG_AddressBase)         ///< Window Watchdog struct/bit access
  #define _WWDG_CR      _SFR(uint8_t,  WWDG_AddressBase+0x00)    ///< Window Watchdog Control register
  #define _WWDG_WR      _SFR(uint8_t,  WWDG_AddressBase+0x01)    ///< Window Watchdog Window register


  /* WWDG Module Reset Values */
  #define _WWDG_CR_RESET_VALUE         ((uint8_t) 0x7F)          ///< Window Watchdog Control register reset value
  #define _WWDG_WR_RESET_VALUE         ((uint8_t) 0x7F)          ///< Window Watchdog Window register reset value


  /* WWDG Control register (_WWDG_CR) */
  #define _WWDG_CR_T                   ((uint8_t) (0x7F << 0))   ///< Window Watchdog 7-bit counter [6:0]
  #define _WWDG_CR_T0                  ((uint8_t) (0x01 << 0))   ///< Window Watchdog 7-bit counter [0]
  #define _WWDG_CR_T1                  ((uint8_t) (0x01 << 1))   ///< Window Watchdog 7-bit counter [1]
  #define _WWDG_CR_T2                  ((uint8_t) (0x01 << 2))   ///< Window Watchdog 7-bit counter [2]
  #define _WWDG_CR_T3                  ((uint8_t) (0x01 << 3))   ///< Window Watchdog 7-bit counter [3]
  #define _WWDG_CR_T4                  ((uint8_t) (0x01 << 4))   ///< Window Watchdog 7-bit counter [4]
  #define _WWDG_CR_T5                  ((uint8_t) (0x01 << 5))   ///< Window Watchdog 7-bit counter [5]
  #define _WWDG_CR_T6                  ((uint8_t) (0x01 << 6))   ///< Window Watchdog 7-bit counter [6]
  #define _WWDG_CR_WDGA                ((uint8_t) (0x01 << 7))   ///< Window Watchdog activation (n/a if WWDG enabled by option byte) [0]

  /* WWDR Window register (_WWDG_WR) */
  #define _WWDG_WR_W                   ((uint8_t) (0x7F << 0))   ///< Window Watchdog 7-bit window value [6:0]
  #define _WWDG_WR_W0                  ((uint8_t) (0x01 << 0))   ///< Window Watchdog 7-bit window value [0]
  #define _WWDG_WR_W1                  ((uint8_t) (0x01 << 1))   ///< Window Watchdog 7-bit window value [1]
  #define _WWDG_WR_W2                  ((uint8_t) (0x01 << 2))   ///< Window Watchdog 7-bit window value [2]
  #define _WWDG_WR_W3                  ((uint8_t) (0x01 << 3))   ///< Window Watchdog 7-bit window value [3]
  #define _WWDG_WR_W4                  ((uint8_t) (0x01 << 4))   ///< Window Watchdog 7-bit window value [4]
  #define _WWDG_WR_W5                  ((uint8_t) (0x01 << 5))   ///< Window Watchdog 7-bit window value [5]
  #define _WWDG_WR_W6                  ((uint8_t) (0x01 << 6))   ///< Window Watchdog 7-bit window value [6]
  // reserved [7]

#endif // WWDG_AddressBase



//------------------------
// Independent Timeout Watchdog (_IWDG)
//------------------------
#if defined(_DOXYGEN) || defined(IWDG_AddressBase)

  /** @brief struct for access to Independent Timeout Watchdog registers (_IWDG) */
  typedef struct {

    /** @brief IWDG Key register (_IWDG_KR) */
    struct {
      _BITS KEY       : 8;  ///< IWDG Key
    } KR;


    /** @brief IWDG Prescaler register (_IWDG_PR) */
    struct {
      _BITS   PRE     : 3;  ///< Prescaler divider
      _BITS           : 5;  //   Reserved
    } PR;


    /** @brief IWDG Reload register (_IWDG_RLR) */
    struct {
      _BITS RL        : 8;  ///< IWDG Reload value
    } RLR;

  } _IWDG_t;

  /* Pointer to Independent Timeout Watchdog registers */
  #define _IWDG         _SFR(_IWDG_t,  IWDG_AddressBase)         ///< Independent Timeout Watchdog struct/bit access
  #define _IWDG_KR      _SFR(uint8_t,  IWDG_AddressBase+0x00)    ///< Independent Timeout Watchdog Key register
  #define _IWDG_PR      _SFR(uint8_t,  IWDG_AddressBase+0x01)    ///< Independent Timeout Watchdog Prescaler register
  #define _IWDG_RLR     _SFR(uint8_t,  IWDG_AddressBase+0x02)    ///< Independent Timeout Watchdog Reload register


  /* IWDG Module Reset Values */
  #define _IWDG_PR_RESET_VALUE         ((uint8_t) 0x00)          ///< Independent Timeout Watchdog Prescaler register reset value
  #define _IWDG_RLR_RESET_VALUE        ((uint8_t) 0xFF)          ///< Independent Timeout Watchdog Reload register reset value


  /* IWDG Key register (_IWDG_KR) keys */
  #define _IWDG_KR_KEY_ENABLE          ((uint8_t) 0xCC)          ///< Independent Timeout Watchdog enable
  #define _IWDG_KR_KEY_REFRESH         ((uint8_t) 0xAA)          ///< Independent Timeout Watchdog refresh
  #define _IWDG_KR_KEY_ACCESS          ((uint8_t) 0x55)          ///< Independent Timeout Watchdog unlock write to _IWDG_PR and _IWDG_RLR

  /* IWDG Prescaler register (_IWDG_PR) */
  #define _IWDG_PR_PRE                 ((uint8_t) (0x07 << 0))   ///< Independent Timeout Watchdog Prescaler divider [2:0]
  #define _IWDG_PR_PRE0                ((uint8_t) (0x01 << 0))   ///< Independent Timeout Watchdog Prescaler divider [0]
  #define _IWDG_PR_PRE1                ((uint8_t) (0x01 << 1))   ///< Independent Timeout Watchdog Prescaler divider [1]
  #define _IWDG_PR_PRE2                ((uint8_t) (0x01 << 2))   ///< Independent Timeout Watchdog Prescaler divider [2]
  // reserved [7:3]

#endif // IWDG_AddressBase



//------------------------
// Auto Wake-Up Module (_AWU)
//------------------------
#if defined(_DOXYGEN) || defined(AWU_AddressBase)

  /** @brief struct for cofiguring the Auto Wake-Up Module (_AWU) */
  typedef struct {

    /** @brief AWU Control/status register (_AWU_CSR) */
    struct {
      _BITS           : 4;    //   Reserved
      _BITS   AWUEN   : 1;    ///< Auto-wakeup enable
      _BITS   AWUF    : 1;    ///< Auto-wakeup flag
      _BITS           : 2;    //   Reserved
    } CSR;


    /** @brief AWU Asynchronous prescaler register (_AWU_APR) */
    struct {
      _BITS   APRE    : 6;    ///< Asynchronous prescaler divider
      _BITS           : 2;    //   Reserved
    } APR;


    /** @brief AWU Timebase selection register (_AWU_TBR) */
    struct {
      _BITS   AWUTB   : 4;    ///< Auto-wakeup timebase selection
      _BITS           : 4;    //   Reserved
    } TBR;

  } _AWU_t;

  /* Pointer to AWU registers */
  #define _AWU          _SFR(_AWU_t,   AWU_AddressBase)          ///< Auto Wake-Up struct/bit access
  #define _AWU_CSR      _SFR(uint8_t,  AWU_AddressBase+0x00)     ///< Auto Wake-Up Control/status register
  #define _AWU_APR      _SFR(uint8_t,  AWU_AddressBase+0x01)     ///< Auto Wake-Up Asynchronous prescaler register
  #define _AWU_TBR      _SFR(uint8_t,  AWU_AddressBase+0x02)     ///< Auto Wake-Up Timebase selection register


  /* AWU Module Reset Values */
  #define _AWU_CSR_RESET_VALUE         ((uint8_t) 0x00)          ///< Auto Wake-Up Control/status register reset value
  #define _AWU_APR_RESET_VALUE         ((uint8_t) 0x3F)          ///< Auto Wake-Up Asynchronous prescaler register reset value
  #define _AWU_TBR_RESET_VALUE         ((uint8_t) 0x00)          ///< Auto Wake-Up Timebase selection register reset value


  /* AWU Control/status register (_AWU_CSR) */
  // reserved [3:0]
  #define _AWU_CSR_AWUEN               ((uint8_t) (0x01 << 4))   ///< Auto-wakeup enable [0]
  #define _AWU_CSR_AWUF                ((uint8_t) (0x01 << 5))   ///< Auto-wakeup status flag [0]
  // reserved [7:6]

  /* AWU Asynchronous prescaler register (_AWU_APR) */
  #define _AWU_APR_APRE                ((uint8_t) (0x3F << 0))   ///< Auto-wakeup asynchronous prescaler divider [5:0]
  #define _AWU_APR_APRE0               ((uint8_t) (0x01 << 0))   ///< Auto-wakeup asynchronous prescaler divider [0]
  #define _AWU_APR_APRE1               ((uint8_t) (0x01 << 1))   ///< Auto-wakeup asynchronous prescaler divider [1]
  #define _AWU_APR_APRE2               ((uint8_t) (0x01 << 2))   ///< Auto-wakeup asynchronous prescaler divider [2]
  #define _AWU_APR_APRE3               ((uint8_t) (0x01 << 3))   ///< Auto-wakeup asynchronous prescaler divider [3]
  #define _AWU_APR_APRE4               ((uint8_t) (0x01 << 4))   ///< Auto-wakeup asynchronous prescaler divider [4]
  #define _AWU_APR_APRE5               ((uint8_t) (0x01 << 5))   ///< Auto-wakeup asynchronous prescaler divider [5]
  // reserved [7:6]

  /* AWU Asynchronous prescaler register (_AWU_APR) */
  #define _AWU_APR_AWUTB               ((uint8_t) (0x0F << 0))   ///< Auto-wakeup timebase selection [3:0]
  #define _AWU_APR_AWUTB0              ((uint8_t) (0x01 << 0))   ///< Auto-wakeup timebase selection [0]
  #define _AWU_APR_AWUTB1              ((uint8_t) (0x01 << 1))   ///< Auto-wakeup timebase selection [1]
  #define _AWU_APR_AWUTB2              ((uint8_t) (0x01 << 2))   ///< Auto-wakeup timebase selection [2]
  #define _AWU_APR_AWUTB3              ((uint8_t) (0x01 << 3))   ///< Auto-wakeup timebase selection [3]
  // reserved [7:4]

#endif // AWU_AddressBase



//------------------------
// I2C Bus Interface (_I2C)
//------------------------
#if defined(_DOXYGEN) || defined(I2C_AddressBase)

  /** @brief struct for controlling I2C module (_I2C) */
  typedef struct {

    /** @brief I2C Control register 1 (_I2C_CR1) */
    struct {
      _BITS   PE        : 1;    ///< Peripheral enable
      _BITS             : 5;    //   Reserved
      _BITS   ENGC      : 1;    ///< General call enable
      _BITS   NOSTRETCH : 1;    ///< Clock stretching disable (Slave mode)
    } CR1;


    /** @brief I2C Control register 2 (_I2C_CR2) */
    struct {
      _BITS   START     : 1;    ///< Start generation
      _BITS   STOP      : 1;    ///< Stop generation
      _BITS   ACK       : 1;    ///< Acknowledge enable
      _BITS   POS       : 1;    ///< Acknowledge position (for data reception)
      _BITS             : 3;    //   Reserved
      _BITS   SWRST     : 1;    ///< Software reset
    } CR2;


    /** @brief I2C Frequency register (_I2C_FREQR) */
    struct {
      _BITS   FREQ      : 6;    ///< Peripheral clock frequency
      _BITS             : 2;    //   Reserved
    } FREQR;


    /** @brief I2C own address register low byte (_I2C_OARL) */
    struct {
      _BITS   ADD0      : 1;    ///< Interface address [0] (in 10-bit address mode)
      _BITS   ADD       : 7;    ///< Interface address [7:1]
    } OARL;


    /** @brief I2C own address register high byte (_I2C_OARH) */
    struct {
      _BITS             : 1;    //   Reserved
      _BITS   ADD       : 2;    ///< Interface address [9:8] (in 10-bit address mode)
      _BITS             : 3;    //   Reserved
      _BITS   ADDCONF   : 1;    ///< Address mode configuration (must always be written as ‘1’)
      _BITS   ADDMODE   : 1;    ///< 7-/10-bit addressing mode (Slave mode)
    } OARH;


    /** @brief Reserved register (1B) */
    uint8_t res     [1];


    /** @brief I2C data register (_I2C_DR) */
    struct {
      _BITS   DATA    : 8;    ///< I2C data
    } DR;


    /** @brief I2C Status register 1 (_I2C_SR1) */
    struct {
      _BITS   SB        : 1;    ///< Start bit (Master mode)
      _BITS   ADDR      : 1;    ///< Address sent (Master mode) / matched (slave mode)
      _BITS   BTF       : 1;    ///< Byte transfer finished
      _BITS   ADD10     : 1;    ///< 10-bit header sent (Master mode)
      _BITS   STOPF     : 1;    ///< Stop detection (Slave mode)
      _BITS             : 1;    //   Reserved
      _BITS   RXNE      : 1;    ///< Data register not empty (receivers)
      _BITS   TXE       : 1;    ///< Data register empty (transmitters)
    } SR1;


    /** @brief I2C Status register 2 (_I2C_SR2) */
    struct {
      _BITS   BERR      : 1;    ///< Bus error
      _BITS   ARLO      : 1;    ///< Arbitration lost (Master mode)
      _BITS   AF        : 1;    ///< Acknowledge failure
      _BITS   OVR       : 1;    ///< Overrun/underrun
      _BITS             : 1;    //   Reserved
      _BITS   WUFH      : 1;    ///< Wakeup from Halt
      _BITS             : 2;    //   Reserved
    } SR2;


    /** @brief I2C Status register 3 (_I2C_SR3) */
    struct {
      _BITS   MSL       : 1;    ///< Master/Slave
      _BITS   BUSY      : 1;    ///< Bus busy
      _BITS   TRA       : 1;    ///< Transmitter/Receiver
      _BITS             : 1;    //   Reserved
      _BITS   GENCALL   : 1;    ///< General call header (Slavemode)
      _BITS             : 3;    //   Reserved
    } SR3;


    /** @brief I2C Interrupt register (_I2C_ITR) */
    struct {
      _BITS   ITERREN   : 1;    ///< Error interrupt enable
      _BITS   ITEVTEN   : 1;    ///< Event interrupt enable
      _BITS   ITBUFEN   : 1;    ///< Buffer interrupt enable
      _BITS             : 5;    //   Reserved
    } ITR;


    /** @brief I2C Clock control register low byte (_I2C_CCRL) */
    struct {
      _BITS   CCR       : 8;    ///< Clock control register (Master mode)
    } CCRL;


    /** @brief I2C Clock control register high byte (_I2C_CCRH) */
    struct {
      _BITS   CCR       : 4;    ///< Clock control register in Fast/Standard mode (Master mode)
      _BITS             : 2;    //   Reserved
      _BITS   DUTY      : 1;    ///< Fast mode duty cycle
      _BITS   FS        : 1;    ///< I2C Master mode selection
    } CCRH;


    /** @brief I2C rise time register (_I2C_TRISER) */
    struct {
      _BITS   TRISE     : 6;    ///< Maximum rise time in Fast/Standard mode (Master mode)
      _BITS             : 2;    //   Reserved
    } TRISER;

  } _I2C_t;

  /// register for SPI control
  #define _I2C        _SFR(_I2C_t,     I2C_AddressBase)          ///< I2C struct/bit access
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


  /* I2C Control register 1 (_I2C_CR1) */
  #define _I2C_CR1_PE                  ((uint8_t) (0x01 << 0))   ///< I2C Peripheral enable [0]
  // reserved [5:1]
  #define _I2C_CR1_ENGC                ((uint8_t) (0x01 << 6))   ///< I2C General call enable [0]
  #define _I2C_CR1_NOSTRETCH           ((uint8_t) (0x01 << 7))   ///< I2C Clock stretching disable (Slave mode) [0]

  /* I2C Control register 2 (_I2C_CR2) */
  #define _I2C_CR2_START               ((uint8_t) (0x01 << 0))   ///< I2C Start generation [0]
  #define _I2C_CR2_STOP                ((uint8_t) (0x01 << 1))   ///< I2C Stop generation [0]
  #define _I2C_CR2_ACK                 ((uint8_t) (0x01 << 2))   ///< I2C Acknowledge enable [0]
  #define _I2C_CR2_POS                 ((uint8_t) (0x01 << 3))   ///< I2C Acknowledge position (for data reception) [0]
  // reserved [6:4]
  #define _I2C_CR2_SWRST               ((uint8_t) (0x01 << 7))   ///< I2C Software reset [0]

  /* I2C Frequency register (_I2C_FREQR) */
  #define _I2C_FREQR_FREQ              ((uint8_t) (0x3F << 0))   ///< I2C Peripheral clock frequency [5:0]
  #define _I2C_FREQR_FREQ0             ((uint8_t) (0x01 << 0))   ///< I2C Peripheral clock frequency [0]
  #define _I2C_FREQR_FREQ1             ((uint8_t) (0x01 << 1))   ///< I2C Peripheral clock frequency [1]
  #define _I2C_FREQR_FREQ2             ((uint8_t) (0x01 << 2))   ///< I2C Peripheral clock frequency [2]
  #define _I2C_FREQR_FREQ3             ((uint8_t) (0x01 << 3))   ///< I2C Peripheral clock frequency [3]
  #define _I2C_FREQR_FREQ4             ((uint8_t) (0x01 << 4))   ///< I2C Peripheral clock frequency [4]
  #define _I2C_FREQR_FREQ5             ((uint8_t) (0x01 << 5))   ///< I2C Peripheral clock frequency [5]
  // reserved [7:6]

  /* I2C own address register low byte (_I2C_OARL) */
  #define _I2C_OARL_ADD0               ((uint8_t) (0x01 << 0))   ///< I2C Interface address [0] (in 10-bit address mode)
  #define _I2C_OARL_ADD1               ((uint8_t) (0x01 << 1))   ///< I2C Interface address [1]
  #define _I2C_OARL_ADD2               ((uint8_t) (0x01 << 2))   ///< I2C Interface address [2]
  #define _I2C_OARL_ADD3               ((uint8_t) (0x01 << 3))   ///< I2C Interface address [3]
  #define _I2C_OARL_ADD4               ((uint8_t) (0x01 << 4))   ///< I2C Interface address [4]
  #define _I2C_OARL_ADD5               ((uint8_t) (0x01 << 5))   ///< I2C Interface address [5]
  #define _I2C_OARL_ADD6               ((uint8_t) (0x01 << 6))   ///< I2C Interface address [6]
  #define _I2C_OARL_ADD7               ((uint8_t) (0x01 << 7))   ///< I2C Interface address [7]

  /* I2C own address register high byte (_I2C_OARH) */
  // reserved [0]
  #define _I2C_OARH_ADD_8_9            ((uint8_t) (0x03 << 1))   ///< I2C Interface address [9:8] (in 10-bit address mode)
  #define _I2C_OARH_ADD8               ((uint8_t) (0x01 << 1))   ///< I2C Interface address [8]
  #define _I2C_OARH_ADD9               ((uint8_t) (0x01 << 2))   ///< I2C Interface address [9]
  // reserved [5:3]
  #define _I2C_OARH_ADDCONF            ((uint8_t) (0x01 << 6))   ///< I2C Address mode configuration [0]
  #define _I2C_OARH_ADDMODE            ((uint8_t) (0x01 << 7))   ///< I2C 7-/10-bit addressing mode (Slave mode) [0]

  /* I2C Status register 1 (_I2C_SR1) */
  #define _I2C_SR1_SB                  ((uint8_t) (0x01 << 0))   ///< I2C Start bit (Master mode) [0]
  #define _I2C_SR1_ADDR                ((uint8_t) (0x01 << 1))   ///< I2C Address sent (Master mode) / matched (Slave mode) [0]
  #define _I2C_SR1_BTF                 ((uint8_t) (0x01 << 2))   ///< I2C Byte transfer finished [0]
  #define _I2C_SR1_ADD10               ((uint8_t) (0x01 << 3))   ///< I2C 10-bit header sent (Master mode) [0]
  #define _I2C_SR1_STOPF               ((uint8_t) (0x01 << 4))   ///< I2C Stop detection (Slave mode) [0]
  // reserved [5]
  #define _I2C_SR1_RXNE                ((uint8_t) (0x01 << 6))   ///< I2C Data register not empty (receivers) [0]
  #define _I2C_SR1_TXE                 ((uint8_t) (0x01 << 7))   ///< I2C Data register empty (transmitters) [0]

  /* I2C Status register 2 (_I2C_SR2) */
  #define _I2C_SR2_BERR                ((uint8_t) (0x01 << 0))   ///< I2C Bus error [0]
  #define _I2C_SR2_ARLO                ((uint8_t) (0x01 << 1))   ///< I2C Arbitration lost (Master mode) [0]
  #define _I2C_SR2_AF                  ((uint8_t) (0x01 << 2))   ///< I2C Acknowledge failure [0]
  #define _I2C_SR2_OVR                 ((uint8_t) (0x01 << 3))   ///< I2C Overrun/underrun [0]
  // reserved [4]
  #define _I2C_SR2_WUFH                ((uint8_t) (0x01 << 5))   ///< I2C Wakeup from Halt [0]
  // reserved [7:6]

  /* I2C Status register 3 (_I2C_SR3) */
  #define _I2C_SR3_MSL                 ((uint8_t) (0x01 << 0))   ///< I2C Master/Slave [0]
  #define _I2C_SR3_BUSY                ((uint8_t) (0x01 << 1))   ///< I2C Bus busy [0]
  #define _I2C_SR3_TRA                 ((uint8_t) (0x01 << 2))   ///< I2C Transmitter/Receiver [0]
  // reserved [3]
  #define _I2C_SR3_GENCALL             ((uint8_t) (0x01 << 4))   ///< I2C General call header (Slavemode) [0]
  // reserved [7:5]

  /* I2C Interrupt register (_I2C_ITR) */
  #define _I2C_ITR_ITERREN             ((uint8_t) (0x01 << 0))   ///< I2C Error interrupt enable [0]
  #define _I2C_ITR_ITEVTEN             ((uint8_t) (0x01 << 1))   ///< I2C Event interrupt enable [0]
  #define _I2C_ITR_ITBUFEN             ((uint8_t) (0x01 << 2))   ///< I2C Buffer interrupt enable [0]
  // reserved [7:5]

  /* I2C Clock control register high byte (_I2C_CCRH) */
  #define _I2C_CCRH_CCR                ((uint8_t) (0x0F << 0))   ///< I2C Clock control register (Master mode) [3:0]
  #define _I2C_CCRH_CCR0               ((uint8_t) (0x01 << 0))   ///< I2C Clock control register (Master mode) [0]
  #define _I2C_CCRH_CCR1               ((uint8_t) (0x01 << 1))   ///< I2C Clock control register (Master mode) [1]
  #define _I2C_CCRH_CCR2               ((uint8_t) (0x01 << 2))   ///< I2C Clock control register (Master mode) [2]
  #define _I2C_CCRH_CCR3               ((uint8_t) (0x01 << 3))   ///< I2C Clock control register (Master mode) [3]
  // reserved [5:4]
  #define _I2C_CCRH_DUTY               ((uint8_t) (0x01 << 6))   ///< I2C Fast mode duty cycle [0]
  #define _I2C_CCRH_FS                 ((uint8_t) (0x01 << 7))   ///< I2C Master mode selection [0]

  /* I2C rise time register (_I2C_TRISER) */
  #define _I2C_TRISER_TRISE            ((uint8_t) (0x3F << 0))   ///< I2C Maximum rise time (Master mode) [5:0]
  #define _I2C_TRISER_TRISE0           ((uint8_t) (0x01 << 0))   ///< I2C Maximum rise time (Master mode) [0]
  #define _I2C_TRISER_TRISE1           ((uint8_t) (0x01 << 1))   ///< I2C Maximum rise time (Master mode) [1]
  #define _I2C_TRISER_TRISE2           ((uint8_t) (0x01 << 2))   ///< I2C Maximum rise time (Master mode) [2]
  #define _I2C_TRISER_TRISE3           ((uint8_t) (0x01 << 3))   ///< I2C Maximum rise time (Master mode) [3]
  #define _I2C_TRISER_TRISE4           ((uint8_t) (0x01 << 4))   ///< I2C Maximum rise time (Master mode) [4]
  #define _I2C_TRISER_TRISE5           ((uint8_t) (0x01 << 5))   ///< I2C Maximum rise time (Master mode) [5]
  // reserved [7:6]

#endif // I2C_AddressBase



//------------------------
// Universal Asynchronous Receiver Transmitter (_UART)
//------------------------
#if defined(_DOXYGEN) || defined(UART_AddressBase)

  /** @brief struct for controlling Universal Asynchronous Receiver Transmitter (_UART) */
  typedef struct {

    /** @brief UART Status register (_UART_SR) */
    struct {
      _BITS   PE      : 1;    ///< Parity error
      _BITS   FE      : 1;    ///< Framing error
      _BITS   NF      : 1;    ///< Noise flag
      _BITS   OR      : 1;    ///< Overrun error
      _BITS   IDLE    : 1;    ///< IDLE line detected
      _BITS   RXNE    : 1;    ///< Read data register not empty
      _BITS   TC      : 1;    ///< Transmission complete
      _BITS   TXE     : 1;    ///< Transmit data register empty
    } SR;


    /** @brief UART data register (_UART_DR) */
    struct {
      _BITS   DATA    : 8;    ///< UART data
    } DR;


    /** @brief UART Baud rate register 1 (_UART_BRR1) */
    struct {
      _BITS   DIV_4_11 : 8;    ///< UART_DIV [11:4]
    } BRR1;


    /** @brief UART Baud rate register 2 (_UART_BRR2) */
    struct {
      _BITS   DIV_0_3   : 4;    ///< UART_DIV [3:0]
      _BITS   DIV_12_15 : 4;    ///< UART_DIV [15:12]
    } BRR2;


    /** @brief UART Control register 1 (_UART_CR1) */
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


    /** @brief UART Control register 2 (_UART_CR2) */
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


    /** @brief UART Control register 3 (_UART_CR3) */
    struct {
      _BITS           : 4;    //   Reserved
      _BITS   STOP    : 2;    ///< STOP bits
      _BITS           : 2;    //   Reserved
    } CR3;


    /** @brief UART Control register 4 (_UART_CR4) */
    struct {
      _BITS   ADD     : 4;    ///< Address of the UART node
      _BITS           : 4;    //   Reserved
    } CR4;

  } _UART_t;

  /* Pointer to UART registers */
  #define _UART      _SFR(_UART_t,     UART_AddressBase)         ///< UART struct/bit access
  #define _UART_SR   _SFR(uint8_t,     UART_AddressBase+0x00)    ///< UART Status register
  #define _UART_DR   _SFR(uint8_t,     UART_AddressBase+0x01)    ///< UART data register
  #define _UART_BRR1 _SFR(uint8_t,     UART_AddressBase+0x02)    ///< UART Baud rate register 1
  #define _UART_BRR2 _SFR(uint8_t,     UART_AddressBase+0x03)    ///< UART Baud rate register 2
  #define _UART_CR1  _SFR(uint8_t,     UART_AddressBase+0x04)    ///< UART Control register 1
  #define _UART_CR2  _SFR(uint8_t,     UART_AddressBase+0x05)    ///< UART Control register 2
  #define _UART_CR3  _SFR(uint8_t,     UART_AddressBase+0x06)    ///< UART Control register 3
  #define _UART_CR4  _SFR(uint8_t,     UART_AddressBase+0x07)    ///< UART Control register 4


  /* UART Module Reset Values */
  #define _UART_SR_RESET_VALUE         ((uint8_t) 0xC0)          ///< UART Status register reset value
  #define _UART_BRR1_RESET_VALUE       ((uint8_t) 0x00)          ///< UART Baud rate register 1 reset value
  #define _UART_BRR2_RESET_VALUE       ((uint8_t) 0x00)          ///< UART Baud rate register 2 reset value
  #define _UART_CR1_RESET_VALUE        ((uint8_t) 0x00)          ///< UART Control register 1 reset value
  #define _UART_CR2_RESET_VALUE        ((uint8_t) 0x00)          ///< UART Control register 2 reset value
  #define _UART_CR3_RESET_VALUE        ((uint8_t) 0x00)          ///< UART Control register 3 reset value
  #define _UART_CR4_RESET_VALUE        ((uint8_t) 0x00)          ///< UART Control register 4 reset value


  /* UART Status register (_UART_SR) */
  #define _UART_SR_PE                  ((uint8_t) (0x01 << 0))   ///< UART Parity error [0]
  #define _UART_SR_FE                  ((uint8_t) (0x01 << 1))   ///< UART Framing error [0]
  #define _UART_SR_NF                  ((uint8_t) (0x01 << 2))   ///< UART Noise flag [0]
  #define _UART_SR_OR                  ((uint8_t) (0x01 << 3))   ///< UART Overrun error [0]
  #define _UART_SR_IDLE                ((uint8_t) (0x01 << 4))   ///< UART IDLE line detected [0]
  #define _UART_SR_RXNE                ((uint8_t) (0x01 << 5))   ///< UART Read data register not empty [0]
  #define _UART_SR_TC                  ((uint8_t) (0x01 << 6))   ///< UART Transmission complete [0]
  #define _UART_SR_TXE                 ((uint8_t) (0x01 << 7))   ///< UART Transmit data register empty [0]

  /* UART Control register 1 (_UART_CR1) */
  #define _UART_CR1_PIEN               ((uint8_t) (0x01 << 0))   ///< UART Parity interrupt enable [0]
  #define _UART_CR1_PS                 ((uint8_t) (0x01 << 1))   ///< UART Parity selection [0]
  #define _UART_CR1_PCEN               ((uint8_t) (0x01 << 2))   ///< UART Parity control enable [0]
  #define _UART_CR1_WAKE               ((uint8_t) (0x01 << 3))   ///< UART Wakeup method [0]
  #define _UART_CR1_M                  ((uint8_t) (0x01 << 4))   ///< UART word length [0]
  #define _UART_CR1_UARTD              ((uint8_t) (0x01 << 5))   ///< UART Disable (for low power consumption) [0]
  #define _UART_CR1_T8                 ((uint8_t) (0x01 << 6))   ///< UART Transmit Data bit 8 (in 9-bit mode) [0]
  #define _UART_CR1_R8                 ((uint8_t) (0x01 << 7))   ///< UART Receive Data bit 8 (in 9-bit mode) [0]

  /* UART Control register 2 (_UART_CR2) */
  #define _UART_CR2_SBK                ((uint8_t) (0x01 << 0))   ///< UART Send break [0]
  #define _UART_CR2_RWU                ((uint8_t) (0x01 << 1))   ///< UART Receiver wakeup [0]
  #define _UART_CR2_REN                ((uint8_t) (0x01 << 2))   ///< UART Receiver enable [0]
  #define _UART_CR2_TEN                ((uint8_t) (0x01 << 3))   ///< UART Transmitter enable [0]
  #define _UART_CR2_ILIEN              ((uint8_t) (0x01 << 4))   ///< UART IDLE Line interrupt enable [0]
  #define _UART_CR2_RIEN               ((uint8_t) (0x01 << 5))   ///< UART Receiver interrupt enable [0]
  #define _UART_CR2_TCIEN              ((uint8_t) (0x01 << 6))   ///< UART Transmission complete interrupt enable [0]
  #define _UART_CR2_TIEN               ((uint8_t) (0x01 << 7))   ///< UART Transmitter interrupt enable [0]

  /* UART Control register 3 (_UART_CR3) */
  // reserved [3:0]
  #define _UART_CR3_STOP               ((uint8_t) (0x03 << 4))   ///< UART STOP bits [1:0]
  #define _UART_CR3_STOP0              ((uint8_t) (0x01 << 4))   ///< UART STOP bits [0]
  #define _UART_CR3_STOP1              ((uint8_t) (0x01 << 5))   ///< UART STOP bits [1]
  // reserved [7:6]

  /* UART Control register 4 (_UART_CR4) */
  #define _UART_CR4_ADD                ((uint8_t) (0x0F << 0))   ///< UART Address of the UART node [3:0]
  #define _UART_CR4_ADD0               ((uint8_t) (0x01 << 0))   ///< UART Address of the UART node [0]
  #define _UART_CR4_ADD1               ((uint8_t) (0x01 << 1))   ///< UART Address of the UART node [1]
  #define _UART_CR4_ADD2               ((uint8_t) (0x01 << 2))   ///< UART Address of the UART node [2]
  #define _UART_CR4_ADD3               ((uint8_t) (0x01 << 3))   ///< UART Address of the UART node [3]
  // reserved [7:4]

#endif // UART_AddressBase



//------------------------
// 16-Bit system timer (_SYSTIM)
//------------------------
#if defined(_DOXYGEN) || defined(SYSTIM_AddressBase)

  /** @brief struct for controlling 16-Bit Timer (_SYSTIM) */
  typedef struct {

    /** @brief SYSTIM Control register 1 (_SYSTIM_CR1) */
    struct {
      _BITS   CEN     : 1;    ///< Counter enable
      _BITS   UDIS    : 1;    ///< Update disable
      _BITS   URS     : 1;    ///< Update request source
      _BITS   OPM     : 1;    ///< One-pulse mode
      _BITS           : 3;    //   Reserved
      _BITS   ARPE    : 1;    ///< Auto-reload preload enable
    } CR1;


    /** @brief SYSTIM Interrupt enable register (_SYSTIM_IER) */
    struct {
      _BITS   UIE     : 1;    ///< Update interrupt enable
      _BITS           : 7;    //   Reserved
    } IER;


    /** @brief SYSTIM Status register 1 (_SYSTIM_SR1) */
    struct {
      _BITS   UIF     : 1;    ///< Update interrupt flag
      _BITS           : 7;    //   Reserved
    } SR1;


    /** @brief SYSTIM Event generation register (_SYSTIM_EGR) */
    struct {
      _BITS   UG      : 1;    ///< Update generation
      _BITS           : 7;    //   Reserved
    } EGR;


    /** @brief SYSTIM 16-bit counter high byte (_SYSTIM_CNTRH) */
    struct {
      _BITS   CNT     : 8;    ///< 16-bit counter [15:8]
    } CNTRH;


    /** @brief SYSTIM 16-bit counter low byte (_SYSTIM_CNTRL) */
    struct {
      _BITS   CNT     : 8;    ///< 16-bit counter [7:0]
    } CNTRL;


    /** @brief SYSTIM prescaler (_SYSTIM_PSCR) */
    struct {
      _BITS   PSC     : 3;    ///< prescaler [2:0]
      _BITS           : 5;    //   Reserved
    } PSCR;


    /** @brief SYSTIM 16-bit auto-reload value high byte (_SYSTIM_ARRH) */
    struct {
      _BITS   ARR     : 8;    ///< 16-bit auto-reload value [15:8]
    } ARRH;


    /** @brief SYSTIM 16-bit auto-reload value low byte (_SYSTIM_ARRL) */
    struct {
      _BITS   ARR     : 8;    ///< 16-bit auto-reload value [7:0]
    } ARRL;

  } _SYSTIM_t;

  /* Pointer to SYSTIM registers */
  #define _SYSTIM        _SFR(_SYSTIM_t,SYSTIM_AddressBase)      ///< SYSTIM struct/bit access
  #define _SYSTIM_CR1    _SFR(uint8_t,  SYSTIM_AddressBase+0x00) ///< SYSTIM control register 1
  #define _SYSTIM_IER    _SFR(uint8_t,  SYSTIM_AddressBase+0x01) ///< SYSTIM interrupt enable register
  #define _SYSTIM_SR1    _SFR(uint8_t,  SYSTIM_AddressBase+0x02) ///< SYSTIM status register 1
  #define _SYSTIM_EGR    _SFR(uint8_t,  SYSTIM_AddressBase+0x03) ///< SYSTIM Event generation register
  #define _SYSTIM_CNTRH  _SFR(uint8_t,  SYSTIM_AddressBase+0x04) ///< SYSTIM counter register high byte
  #define _SYSTIM_CNTRL  _SFR(uint8_t,  SYSTIM_AddressBase+0x05) ///< SYSTIM counter register low byte
  #define _SYSTIM_PSCR   _SFR(uint8_t,  SYSTIM_AddressBase+0x06) ///< SYSTIM clock prescaler register
  #define _SYSTIM_ARRH   _SFR(uint8_t,  SYSTIM_AddressBase+0x07) ///< SYSTIM auto-reload register high byte
  #define _SYSTIM_ARRL   _SFR(uint8_t,  SYSTIM_AddressBase+0x08) ///< SYSTIM auto-reload register low byte


  /* SYSTIM Module Reset Values */
  #define _SYSTIM_CR1_RESET_VALUE      ((uint8_t) 0x00)          ///< SYSTIM control register 1 reset value
  #define _SYSTIM_IER_RESET_VALUE      ((uint8_t) 0x00)          ///< SYSTIM interrupt enable register reset value
  #define _SYSTIM_SR1_RESET_VALUE      ((uint8_t) 0x00)          ///< SYSTIM status register 1 reset value
  #define _SYSTIM_EGR_RESET_VALUE      ((uint8_t) 0x00)          ///< SYSTIM Event generation register reset value
  #define _SYSTIM_CNTRH_RESET_VALUE    ((uint8_t) 0x00)          ///< SYSTIM counter register high byte reset value
  #define _SYSTIM_CNTRL_RESET_VALUE    ((uint8_t) 0x00)          ///< SYSTIM counter register low byte reset value
  #define _SYSTIM_PSCR_RESET_VALUE     ((uint8_t) 0x00)          ///< SYSTIM clock prescaler register reset value
  #define _SYSTIM_ARRH_RESET_VALUE     ((uint8_t) 0xFF)          ///< SYSTIM auto-reload register high byte reset value
  #define _SYSTIM_ARRL_RESET_VALUE     ((uint8_t) 0xFF)          ///< SYSTIM auto-reload register low byte reset value


  /* SYSTIM Control register (_SYSTIM_CR1) */
  #define _SYSTIM_CR1_CEN              ((uint8_t) (0x01 << 0))   ///< SYSTIM Counter enable [0]
  #define _SYSTIM_CR1_UDIS             ((uint8_t) (0x01 << 1))   ///< SYSTIM Update disable [0]
  #define _SYSTIM_CR1_URS              ((uint8_t) (0x01 << 2))   ///< SYSTIM Update request source [0]
  #define _SYSTIM_CR1_OPM              ((uint8_t) (0x01 << 3))   ///< SYSTIM One-pulse mode [0]
  // reserved [6:4]
  #define _SYSTIM_CR1_ARPE             ((uint8_t) (0x01 << 7))   ///< SYSTIM Auto-reload preload enable [0]

  /* SYSTIM Interrupt enable (_SYSTIM_IER) */
  #define _SYSTIM_IER_UIE              ((uint8_t) (0x01 << 0))   ///< SYSTIM Update interrupt enable [0]
  // reserved [7:1]

  /*  SYSTIM Status register 1 (_SYSTIM_SR1) */
  #define _SYSTIM_SR1_UIF              ((uint8_t) (0x01 << 0))   ///< SYSTIM Update interrupt flag [0]
  // reserved [7:1]

  /*  SYSTIM Event generation register (_SYSTIM_EGR) */
  #define _SYSTIM_EGR_UG               ((uint8_t) (0x01 << 0))   ///< SYSTIM Update generation [0]
  // reserved [7:1]

  /*  SYSTIM prescaler (_SYSTIM_PSCR) */
  #define _SYSTIM_PSCR_PSC             ((uint8_t) (0x07 << 0))   ///< SYSTIM prescaler [2:0]
  #define _SYSTIM_PSCR_PSC0            ((uint8_t) (0x01 << 0))   ///< SYSTIM prescaler [0]
  #define _SYSTIM_PSCR_PSC1            ((uint8_t) (0x01 << 1))   ///< SYSTIM prescaler [1]
  #define _SYSTIM_PSCR_PSC2            ((uint8_t) (0x01 << 2))   ///< SYSTIM prescaler [2]
  // reserved [7:3]

#endif // SYSTIM_AddressBase



//------------------------
// Digital addressable lighting interface (DALI)
//------------------------
#if defined(_DOXYGEN) || defined(DALI_AddressBase)

  /** @brief struct for controlling lighting interface (_DALI) */
  typedef struct {

    /** @brief DALI data rate control register low byte (_DALI_CLK_L) */
    struct {
      _BITS   CLK     : 8;    ///< clock prescaler [7:0] (fdata = fmaster/[(N+1)*16])
    } CLK_L;


    /** @brief DALI Data rate control register high byte (_DALI_CLK_H) */
    struct {
      _BITS   CLK     : 2;    ///< clock prescaler [9:8] (fdata = fmaster/[(N+1)*16])
      _BITS           : 6;    //   Reserved, must be kept clear
    } CLK_H;


    /** @brief DALI Message byte 0 register (_DALI_FB0) */
    struct {
      _BITS   DATA    : 8;    ///< received message [7:0]
    } FB0;


    /** @brief DALI Message byte 1 register (_DALI_FB1) */
    struct {
      _BITS   DATA    : 8;    ///< received message [15:8]
    } FB1;


    /** @brief DALI Message byte 2 register (_DALI_FB2) */
    struct {
      _BITS   DATA    : 8;    ///<  received message [23:16]
    } FB2;


    /** @brief DALI Backward data register (_DALI_BD) */
    struct {
      _BITS   DATA    : 8;    ///< data to transmit [7:0]
    } BD;


    /** @brief DALI Control register (_DALI_CR) */
    struct {
      _BITS   FTS      : 1;    ///< force transmit state
      _BITS   RTS      : 1;    ///< receive/transmit state
      _BITS   RTA      : 1;    ///< receive/transmit acknowledge
      _BITS   DCME     : 1;    ///< DALI communication enable
      _BITS   MLN      : 2;    ///< message length
      _BITS   SMK      : 1;    ///< mask start bit
      _BITS   LNWDG_EN : 1;    ///< monitor watchdog on receiver line
    } CR;


    /** @brief DALI Status and control register (_DALI_CSR) */
    struct {
      _BITS           : 2;    //   Reserved, must be kept clear
      _BITS   WDGF    : 1;    ///< watchdog receiver line interrupt status flag
      _BITS   WDGE    : 1;    ///< watchdog receiver line interrupt enable
      _BITS   RTF     : 1;    ///< receive/transmit flag
      _BITS   EF      : 1;    ///< error flag
      _BITS   ITF     : 1;    ///< interrupt flag
      _BITS   IEN     : 1;    ///< interrupt enable
    } CSR;


    /** @brief DALI Status and control register 1 (_DALI_CSR1) */
    struct {
      _BITS   WDG_PRSC : 3;    ///< watchdog DALI prescaler timer
      _BITS   RDY_REC  : 1;    ///< ready to receive
      _BITS   CKS      : 4;    ///< clock counter value
    } CSR1;


    /** @brief DALI Control reverse signal line (_DALI_REVLN) */
    struct {
      _BITS   EN_REV  : 1;    ///< reverse DALI reverse signal line
      _BITS   REV_DIN : 1;    ///< reverse DALI_rx signal line
      _BITS   REVDOUT : 1;    ///< reverse DALI_tx signal line
      _BITS           : 5;    //   Reserved, must be kept clear
    } REVLN;

  } _DALI_t;

  /* Pointer to DALI registers */
  #define _DALI        _SFR(_DALI_t,   DALI_AddressBase)         ///< DALI struct/bit access
  #define _DALI_CLK_L  _SFR(uint8_t,   DALI_AddressBase+0x00)    ///< DALI data rate control register low byte
  #define _DALI_CLK_H  _SFR(uint8_t,   DALI_AddressBase+0x01)    ///< DALI data rate control register high byte
  #define _DALI_FB0    _SFR(uint8_t,   DALI_AddressBase+0x02)    ///< DALI Message byte 0 register
  #define _DALI_FB1    _SFR(uint8_t,   DALI_AddressBase+0x03)    ///< DALI Message byte 1 register
  #define _DALI_FB2    _SFR(uint8_t,   DALI_AddressBase+0x04)    ///< DALI Message byte 2 register
  #define _DALI_BD     _SFR(uint8_t,   DALI_AddressBase+0x05)    ///< DALI Backward data register
  #define _DALI_CR     _SFR(uint8_t,   DALI_AddressBase+0x06)    ///< DALI Control register
  #define _DALI_CSR    _SFR(uint8_t,   DALI_AddressBase+0x07)    ///< DALI Status and control register
  #define _DALI_CSR1   _SFR(uint8_t,   DALI_AddressBase+0x08)    ///< DALI Status and control register 1
  #define _DALI_REVLN  _SFR(uint8_t,   DALI_AddressBase+0x09)    ///< DALI Control reverse signal line


  /* DALI Module Reset Values */
  #define _DALI_CLK_L_RESET_VALUE      ((uint8_t) 0x00)          ///< DALI data rate control register low byte reset value
  #define _DALI_CLK_H_RESET_VALUE      ((uint8_t) 0x00)          ///< DALI data rate control register high byte reset value
  #define _DALI_FB0_RESET_VALUE        ((uint8_t) 0x00)          ///< DALI Message byte 0 register reset value
  #define _DALI_FB1_RESET_VALUE        ((uint8_t) 0x00)          ///< DALI Message byte 1 register reset value
  #define _DALI_FB2_RESET_VALUE        ((uint8_t) 0x00)          ///< DALI Message byte 2 register reset value
  #define _DALI_BD_RESET_VALUE         ((uint8_t) 0x00)          ///< DALI Backward data register reset value
  #define _DALI_CR_RESET_VALUE         ((uint8_t) 0x00)          ///< DALI Control register reset value
  #define _DALI_CSR_RESET_VALUE        ((uint8_t) 0x00)          ///< DALI Status and control register reset value
  #define _DALI_CSR1_RESET_VALUE       ((uint8_t) 0x00)          ///< DALI Status and control register 1 reset value
  #define _DALI_REVLN_RESET_VALUE      ((uint8_t) 0x00)          ///< DALI Control reverse signal line reset value


  /* DALI Control register (_DALI_CR) */
  #define _DALI_CR_FTS                 ((uint8_t) (0x01 << 0))   ///< DALI force transmit state [0]
  #define _DALI_CR_RTS                 ((uint8_t) (0x01 << 1))   ///< DALI receive/transmit state [0]
  #define _DALI_CR_RTA                 ((uint8_t) (0x01 << 2))   ///< DALI receive/transmit acknowledge [0]
  #define _DALI_CR_DCME                ((uint8_t) (0x01 << 3))   ///< DALI DALI communication enable [0]
  #define _DALI_CR_MLN                 ((uint8_t) (0x03 << 4))   ///< DALI message length [1:0]
  #define _DALI_CR_MLN0                ((uint8_t) (0x01 << 4))   ///< DALI message length [0]
  #define _DALI_CR_MLN1                ((uint8_t) (0x01 << 5))   ///< DALI message length [1]
  #define _DALI_CR_SMK                 ((uint8_t) (0x01 << 6))   ///< DALI mask start bit [0]
  #define _DALI_CR_LNWDG_EN            ((uint8_t) (0x01 << 7))   ///< DALI monitor watchdog on receiver line [0]

  /* DALI Status and control register (_DALI_CSR) */
  // reserved [1:0]
  #define _DALI_CSR_WDGF               ((uint8_t) (0x01 << 0))   ///< DALI watchdog receiver line interrupt status flag [0]
  #define _DALI_CSR_WDGE               ((uint8_t) (0x01 << 0))   ///< DALI watchdog receiver line interrupt enable [0]
  #define _DALI_CSR_RTF                ((uint8_t) (0x01 << 0))   ///< DALI receive/transmit flag [0]
  #define _DALI_CSR_EF                 ((uint8_t) (0x01 << 0))   ///< DALI error flag [0]
  #define _DALI_CSR_ITF                ((uint8_t) (0x01 << 0))   ///< DALI interrupt flag [0]
  #define _DALI_CSR_IEN                ((uint8_t) (0x01 << 0))   ///< DALI interrupt enable [0]

  /* DALI Status register 1 (_DALI_CSR1) */
  #define _DALI_CSR1_WDG_PRSC          ((uint8_t) (0x07 << 0))   ///< DALI watchdog DALI prescaler timer [2:0]
  #define _DALI_CSR1_WDG_PRSC0         ((uint8_t) (0x01 << 0))   ///< DALI watchdog DALI prescaler timer [0]
  #define _DALI_CSR1_WDG_PRSC1         ((uint8_t) (0x01 << 1))   ///< DALI watchdog DALI prescaler timer [1]
  #define _DALI_CSR1_WDG_PRSC2         ((uint8_t) (0x01 << 2))   ///< DALI watchdog DALI prescaler timer [2]
  #define _DALI_CSR1_RDY_REC           ((uint8_t) (0x01 << 3))   ///< DALI ready to receive [0]
  #define _DALI_CSR1_CKS               ((uint8_t) (0x0F << 4))   ///< DALI clock counter value [3:0]
  #define _DALI_CSR1_CKS0              ((uint8_t) (0x01 << 4))   ///< DALI clock counter value [0]
  #define _DALI_CSR1_CKS1              ((uint8_t) (0x01 << 5))   ///< DALI clock counter value [1]
  #define _DALI_CSR1_CKS2              ((uint8_t) (0x01 << 6))   ///< DALI clock counter value [2]
  #define _DALI_CSR1_CKS3              ((uint8_t) (0x01 << 7))   ///< DALI clock counter value [3]

  /* DALI Control reverse signal line (_DALI_REVLN) */
  #define _DALI_REVLN_EN_REV           ((uint8_t) (0x01 << 0))   ///< DALI reverse DALI reverse signal line [0]
  #define _DALI_REVLN_REV_DIN          ((uint8_t) (0x01 << 1))   ///< DALI reverse DALI_rx signal line [0]
  #define _DALI_REVLN_REVDOUT          ((uint8_t) (0x01 << 2))   ///< DALI reverse DALI_tx signal line [0]
  // reserved [7:3]

#endif // DALI_AddressBase



//------------------------
// Analog Digital Converter (_ADC)
//------------------------
#if defined(_DOXYGEN) || defined(ADC_AddressBase)

  /** @brief struct containing Analog Digital Converter (_ADC) */
  typedef struct {

    /** @brief ADC configuration register (_ADC_CFG) */
    struct {
      _BITS   PD          : 1;  ///< power-down
      _BITS   STOP        : 1;  ///< stop ADC sequencer
      _BITS   FIFO_FLUSH  : 1;  ///< flush data buffer
      _BITS   CIRCULAR    : 1;  ///< circular sequencer mode
      _BITS   DATA_FORMAT : 1;  ///< data aligment
      _BITS               : 3;  //   Reserved
    } CFG;


    /** @brief ADC start of conversion (_ADC_SOC) */
    struct {
      _BITS   SOC     : 1;      ///< start of conversion sequence
      _BITS           : 7;      //   Reserved, must be kept cleared
    } SOC;


    /** @brief ADC interrupt enable register (_ADC_IER) */
    struct {
      _BITS   EOC_EN      : 1;  ///< ADC end of conversion mode interrupt enable
      _BITS   EOS_EN      : 1;  ///< ADC end of sequence mode interrupt enable
      _BITS   SEQ_FULL_EN : 1;  ///< ADC sequencer buffer full interrupt enable
      _BITS               : 5;  //   Reserved, must be kept cleared
    } IER;


    /** @brief ADC sequencer register (_ADC_SEQ) */
    struct {
      _BITS CH          : 3;    ///< channel selection
      _BITS GAIN        : 1;    ///< gain for selected channel (0=x1; 1=x4)
      _BITS             : 4;    //   Reserved, must be kept cleared
    } SEQ;


    /** @brief ADC result 0 low byte (_ADC_DATL_0) */
    struct {
      _BITS DATA        : 8;    ///< ADC value [7:0]
    } DATL_0;


    /** @brief ADC result 0 high byte (_ADC_DATH_0) */
    struct {
      _BITS DATA        : 8;    ///< ADC value [15:8]
    } DATH_0;


    /** @brief ADC result 1 low byte (_ADC_DATL_1) */
    struct {
      _BITS DATA        : 8;    ///< ADC value [7:0]
    } DATL_1;


    /** @brief ADC result 1 high byte (_ADC_DATH_1) */
    struct {
      _BITS DATA        : 8;    ///< ADC value [15:8]
    } DATH_1;


    /** @brief ADC result 2 low byte (_ADC_DATL_2) */
    struct {
      _BITS DATA        : 8;    ///< ADC value [7:0]
    } DATL_2;


    /** @brief ADC result 2 high byte (_ADC_DATH_2) */
    struct {
      _BITS DATA        : 8;    ///< ADC value [15:8]
    } DATH_2;


    /** @brief ADC result 3 low byte (_ADC_DATL_3) */
    struct {
      _BITS DATA        : 8;    ///< ADC value [7:0]
    } DATL_3;


    /** @brief ADC result 3 high byte (_ADC_DATH_3) */
    struct {
      _BITS DATA        : 8;    ///< ADC value [15:8]
    } DATH_3;


    /** @brief ADC result 4 low byte (_ADC_DATL_4) */
    struct {
      _BITS DATA        : 8;    ///< ADC value [7:0]
    } DATL_4;


    /** @brief ADC result 4 high byte (_ADC_DATH_4) */
    struct {
      _BITS DATA        : 8;    ///< ADC value [15:8]
    } DATH_4;


    /** @brief ADC result 5 low byte (_ADC_DATL_5) */
    struct {
      _BITS DATA        : 8;    ///< ADC value [7:0]
    } DATL_5;


    /** @brief ADC result 5 high byte (_ADC_DATH_5) */
    struct {
      _BITS DATA        : 8;    ///< ADC value [15:8]
    } DATH_5;


    /** @brief ADC result 6 low byte (_ADC_DATL_6) */
    struct {
      _BITS DATA        : 8;    ///< ADC value [7:0]
    } DATL_6;


    /** @brief ADC result 6 high byte (_ADC_DATH_6) */
    struct {
      _BITS DATA        : 8;    ///< ADC value [15:8]
    } DATH_6;


    /** @brief ADC result 7 low byte (_ADC_DATL_7) */
    struct {
      _BITS DATA        : 8;    ///< ADC value [7:0]
    } DATL_7;


    /** @brief ADC result 7 high byte (_ADC_DATH_7) */
    struct {
      _BITS DATA        : 8;    ///< ADC value [15:8]
    } DATH_7;


    /** @brief ADC status register (_ADC_SR) */
    struct {
      _BITS   EOC       : 1;    ///< ADC end of conversion mode
      _BITS   EOS       : 1;    ///< ADC end of sequence mode
      _BITS   SEQ_FULL  : 1;    ///< ADC sequencer buffer full
      _BITS             : 5;    //   Reserved, must be kept cleared
    } SR;


    /** @brief SOC delay counter register (_ADC_DLYCNT) */
    struct {
      _BITS   SOC_DLY_CNT : 8;  ///< ADC SOC delay counter
    } DLYCNT;

  } _ADC_t;

  /* Pointer to ADC registers */
  #define _ADC         _SFR(_ADC_t,    ADC_AddressBase)          ///< ADC struct/bit access
  #define _ADC_CFG     _SFR(uint8_t,   ADC_AddressBase+0x00)     ///< ADC configuration register
  #define _ADC_SOC     _SFR(uint8_t,   ADC_AddressBase+0x01)     ///< ADC start of conversion
  #define _ADC_IER     _SFR(uint8_t,   ADC_AddressBase+0x02)     ///< ADC interrupt enable register
  #define _ADC_SEQ     _SFR(uint8_t,   ADC_AddressBase+0x03)     ///< ADC sequencer register
  #define _ADC_DATL_0  _SFR(uint8_t,   ADC_AddressBase+0x04)     ///< ADC result 0 low byte
  #define _ADC_DATH_0  _SFR(uint8_t,   ADC_AddressBase+0x05)     ///< ADC result 0 high byte
  #define _ADC_DATL_1  _SFR(uint8_t,   ADC_AddressBase+0x06)     ///< ADC result 1 low byte
  #define _ADC_DATH_1  _SFR(uint8_t,   ADC_AddressBase+0x07)     ///< ADC result 1 high byte
  #define _ADC_DATL_2  _SFR(uint8_t,   ADC_AddressBase+0x08)     ///< ADC result 2 low byte
  #define _ADC_DATH_2  _SFR(uint8_t,   ADC_AddressBase+0x09)     ///< ADC result 2 high byte
  #define _ADC_DATL_3  _SFR(uint8_t,   ADC_AddressBase+0x0A)     ///< ADC result 3 low byte
  #define _ADC_DATH_3  _SFR(uint8_t,   ADC_AddressBase+0x0B)     ///< ADC result 3 high byte
  #define _ADC_DATL_4  _SFR(uint8_t,   ADC_AddressBase+0x0C)     ///< ADC result 4 low byte
  #define _ADC_DATH_4  _SFR(uint8_t,   ADC_AddressBase+0x0D)     ///< ADC result 4 high byte
  #define _ADC_DATL_5  _SFR(uint8_t,   ADC_AddressBase+0x0E)     ///< ADC result 5 low byte
  #define _ADC_DATH_5  _SFR(uint8_t,   ADC_AddressBase+0x0F)     ///< ADC result 5 high byte
  #define _ADC_DATL_6  _SFR(uint8_t,   ADC_AddressBase+0x10)     ///< ADC result 6 low byte
  #define _ADC_DATH_6  _SFR(uint8_t,   ADC_AddressBase+0x11)     ///< ADC result 6 high byte
  #define _ADC_DATL_7  _SFR(uint8_t,   ADC_AddressBase+0x12)     ///< ADC result 7 low byte
  #define _ADC_DATH_7  _SFR(uint8_t,   ADC_AddressBase+0x13)     ///< ADC result 7 high byte
  #define _ADC_SR      _SFR(uint8_t,   ADC_AddressBase+0x14)     ///< ADC status register
  #define _ADC_DLYCNT  _SFR(uint8_t,   ADC_AddressBase+0x15)     ///< ADC SOC delay counter register


  /* ADC Module Reset Values */
  #define _ADC_CFG_RESET_VALUE        ((uint8_t) 0x01)          ///< ADC configuration register reset value
  #define _ADC_SOC_RESET_VALUE        ((uint8_t) 0x00)          ///< ADC start of conversion reset value
  #define _ADC_IER_RESET_VALUE        ((uint8_t) 0x00)          ///< ADC interrupt enable register reset value
  #define _ADC_SEQ_RESET_VALUE        ((uint8_t) 0x00)          ///< ADC sequencer register reset value
  #define _ADC_DATL_0_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC result 0 low byte reset value
  #define _ADC_DATH_0_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC result 0 high byte reset value
  #define _ADC_DATL_1_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC result 1 low byte reset value
  #define _ADC_DATH_1_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC result 1 high byte reset value
  #define _ADC_DATL_2_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC result 2 low byte reset value
  #define _ADC_DATH_2_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC result 2 high byte reset value
  #define _ADC_DATL_3_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC result 3 low byte reset value
  #define _ADC_DATH_3_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC result 3 high byte reset value
  #define _ADC_DATL_4_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC result 4 low byte reset value
  #define _ADC_DATH_4_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC result 4 high byte reset value
  #define _ADC_DATL_5_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC result 5 low byte reset value
  #define _ADC_DATH_5_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC result 5 high byte reset value
  #define _ADC_DATL_6_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC result 6 low byte reset value
  #define _ADC_DATH_6_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC result 6 high byte reset value
  #define _ADC_DATL_7_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC result 7 low byte reset value
  #define _ADC_DATH_7_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC result 7 high byte reset value
  #define _ADC_SR_RESET_VALUE         ((uint8_t) 0x00)          ///< ADC status register reset value
  #define _ADC_DLYCNT_RESET_VALUE     ((uint8_t) 0x00)          ///< ADC SOC delay counter register reset value


  /* ADC configuration register (_ADC_CFG) */
  #define _ADC_CFG_PD                  ((uint8_t) (0x01 << 0))   ///< ADC power-down [0]
  #define _ADC_CFG_STOP                ((uint8_t) (0x01 << 1))   ///< ADC stop sequencer [0]
  #define _ADC_CFG_FIFO_FLUSH          ((uint8_t) (0x01 << 2))   ///< ADC flush data buffer [0]
  #define _ADC_CFG_CIRCULAR            ((uint8_t) (0x01 << 3))   ///< ADC circular sequencer mode [0]
  #define _ADC_CFG_DATA_FORMAT         ((uint8_t) (0x01 << 4))   ///< ADC data aligment [0]
  // reserved [7:5]

  /* ADC start of conversion (_ADC_SOC) */
  #define _ADC_SOC_SOC                 ((uint8_t) (0x01 << 0))   ///< ADC start of conversion sequence [0]
  // reserved [7:1]

  /* ADC interrupt enable register (_ADC_IER) */
  #define _ADC_IER_EOC_EN              ((uint8_t) (0x01 << 0))   ///< ADC end of conversion mode interrupt enable [0]
  #define _ADC_IER_EOS_EN              ((uint8_t) (0x01 << 1))   ///< ADC end of sequence mode interrupt enable [0]
  #define _ADC_IER_SEQ_FULL_EN         ((uint8_t) (0x01 << 2))   ///< ADC sequencer buffer full interrupt enable [0]
  // reserved [7:3]

  /* ADC ADC sequencer register (_ADC_SEQ) */
  #define _ADC_SEQ_CH                  ((uint8_t) (0x07 << 0))   ///< ADC channel selection [2:0]
  #define _ADC_SEQ_CH0                 ((uint8_t) (0x01 << 0))   ///< ADC channel selection [0]
  #define _ADC_SEQ_CH1                 ((uint8_t) (0x01 << 1))   ///< ADC channel selection [1]
  #define _ADC_SEQ_CH2                 ((uint8_t) (0x01 << 2))   ///< ADC channel selection [2]
  #define _ADC_SEQ_GAIN                ((uint8_t) (0x01 << 3))   ///< ADC gain for selected channel (0=x1; 1=x4) [0]
  // reserved [7:4]

  /* ADC status register (_ADC_SR) */
  #define _ADC_SR_EOC                  ((uint8_t) (0x01 << 0))   ///< ADC end of conversion mode [0]
  #define _ADC_SR_EOS                  ((uint8_t) (0x01 << 1))   ///< ADC end of sequence mode [0]
  #define _ADC_SR_SEQ_FULL             ((uint8_t) (0x01 << 2))   ///< ADC sequencer buffer full [0]
  // reserved [7:3]

#endif // ADC_AddressBase



//------------------------
// State machine, event driven (_SMED)
//------------------------
#if defined(_DOXYGEN) || defined(SMED0_AddressBase) || defined(SMED1_AddressBase) || defined(SMED2_AddressBase) || defined(SMED3_AddressBase) || defined(SMED4_AddressBase) || defined(SMED5_AddressBase)

  /** @brief struct for State machine, event driven (_SMEDn) */
  typedef struct {

    /** @brief SMEDn Control register (_SMEDn_CTR) */
    struct {
      _BITS   START_CNT   : 1;    ///< start counter
      _BITS   FSM_ENA     : 1;    ///< synchronous FSM enable
      _BITS               : 6;    //   Reserved, must be kept clear
    } CTR;

    /** @brief SMEDn Control timer register (_SMEDn_CTR_TMR) */
    struct {
      _BITS   TIME_T0_VAL : 1;    ///< validation of the TMR_T0 register
      _BITS   TIME_T1_VAL : 1;    ///< validation of the TMR_T1 register
      _BITS   TIME_T2_VAL : 1;    ///< validation of the TMR_T2 register
      _BITS   TIME_T3_VAL : 1;    ///< validation of the TMR_T3 register
      _BITS   DITHER_VAL  : 1;    ///< validation of the CTR_DTHR register
      _BITS               : 3;    //   Reserved, must be kept clear
    } CTR_TMR;

    /** @brief SMEDn Control input register (_SMEDn_CTR_INP) */
    struct {
      _BITS   RS_INSIG    : 3;    ///< external event input polarity level definition
      _BITS   RAIS_EN     : 1;    ///< Asy_Enable input polarity level definition
      _BITS   EL_INSIG    : 3;    ///< external InSig input characteristic definition
      _BITS   EL_EN       : 1;    ///< Asy_Enable input characteristic definition
    } CTR_INP;

    /** @brief SMEDn Dithering register (_SMEDn_CTR_DTR) */
    struct {
      _BITS   DITH        : 8;    ///< dithering register
    } CTR_DTR;

    /** @brief SMEDn Time T0 LSB register (_SMEDn_TMR_T0L) */
    struct {
      _BITS   T0          : 8;    ///< time 0 constant register low byte
    } TMR_T0L;

    /** @brief SMEDn Time T0 MSB register (_SMEDn_TMR_T0H) */
    struct {
      _BITS   T0          : 8;    ///< time 0 constant register high byte
    } TMR_T0H;

    /** @brief SMEDn Time T1 LSB register (_SMEDn_TMR_T1L) */
    struct {
      _BITS   T1          : 8;    ///< time 1 constant register low byte
    } TMR_T1L;

    /** @brief SMEDn Time T1 MSB register (_SMEDn_TMR_T1H) */
    struct {
      _BITS   T1          : 8;    ///< time 1 constant register high byte
    } TMR_T1H;

    /** @brief SMEDn Time T2 LSB register (_SMEDn_TMR_T2L) */
    struct {
      _BITS   T2          : 8;    ///< time 2 constant register low byte
    } TMR_T2L;

    /** @brief SMEDn Time T2 MSB register (_SMEDn_TMR_T2H) */
    struct {
      _BITS   T2          : 8;    ///< time 2 constant register high byte
    } TMR_T2H;

    /** @brief SMEDn Time T3 LSB register (_SMEDn_TMR_T3L) */
    struct {
      _BITS   T3          : 8;    ///< time 3 constant register low byte
    } TMR_T3L;

    /** @brief SMEDn Time T3 MSB register (_SMEDn_TMR_T3H) */
    struct {
      _BITS   T3          : 8;    ///< time 3 constant register high byte
    } TMR_T3H;

    /** @brief SMEDn Parameter 0 IDLE register (_SMEDn_PRM_ID0) */
    struct {
      _BITS   NX_STAT     : 2;    ///< idle next event controlled state definition
      _BITS   EDGE        : 2;    ///< idle external edge jumping selection
      _BITS   CNT_RSTE    : 1;    ///< idle counter reset definition
      _BITS   PULS_EDG    : 1;    ///< idle PWM pulse out level definition
      _BITS   HOLD_JMP    : 1;    ///< idle hold state definition
      _BITS   AND_OR      : 1;    ///< idle jumping Boolean function definition
    } PRM_ID0;

    /** @brief SMEDn Parameter 1 IDLE register (_SMEDn_PRM_ID1) */
    struct {
      _BITS               : 2;    //   Reserved
      _BITS   CEDGE       : 2;    ///< idle external edge jumping selection
      _BITS   CNT_RSTC    : 1;    ///< idle counter reset definition
      _BITS   PULS_CMP    : 1;    ///< idle PWM pulse out level definition
      _BITS   HOLD_EXIT   : 1;    ///< idle HOLD state exit cause definition
      _BITS               : 1;    //   Reserved
    } PRM_ID1;

    /** @brief SMEDn Parameter 2 IDLE register (_SMEDn_PRM_ID2) */
    struct {
      _BITS   LATCH_RS    : 1;    ///< idle latch reset definition
      _BITS               : 6;    //   Reserved
      _BITS   QCOUP_ST    : 1;    ///< idle quad couple start mode configuration
    } PRM_ID2;

    /** @brief SMEDn Parameter 0 S0 register (_SMEDn_PRM_S00) */
    struct {
      _BITS   NX_STAT     : 2;    ///< next event controlled state definition
      _BITS   EDGE        : 2;    ///< external edge jumping selection
      _BITS   CNT_RSTE    : 1;    ///< counter reset definition
      _BITS   PULS_EDG    : 1;    ///< PWM pulse out level definition
      _BITS   HOLD_JMP    : 1;    ///< hold state definition
      _BITS   AND_OR      : 1;    ///< jumping Boolean function definition
    } PRM_S00;

    /** @brief SMEDn Parameter 1 S0 register (_SMEDn_PRM_S01) */
    struct {
      _BITS               : 2;    //   Reserved, must be kept clear
      _BITS   CEDGE       : 2;    ///< external edge jumping selection
      _BITS   CNT_RSTC    : 1;    ///< counter reset definition
      _BITS   PULS_CMP    : 1;    ///< PWM pulse out level definition
      _BITS   HOLD_EXIT   : 1;    ///< HOLD state exit cause definition
      _BITS               : 1;    //   Reserved, must be kept clear
    } PRM_S01;

    /** @brief SMEDn Parameter 2 S0 register (_SMEDn_PRM_S02) */
    struct {
      _BITS   LATCH_RS    : 1;    ///< latch reset definition
      _BITS               : 7;    //   Reserved, must be kept clear
    } PRM_S02;

    /** @brief SMEDn Parameter 0 S1 register (_SMEDn_PRM_S10) */
    struct {
      _BITS   NX_STAT     : 2;    ///< next event controlled state definition
      _BITS   EDGE        : 2;    ///< external edge jumping selection
      _BITS   CNT_RSTE    : 1;    ///< counter reset definition
      _BITS   PULS_EDG    : 1;    ///< PWM pulse out level definition
      _BITS   HOLD_JMP    : 1;    ///< hold state definition
      _BITS   AND_OR      : 1;    ///< jumping Boolean function definition
    } PRM_S10;

    /** @brief SMEDn Parameter 1 S1 register (_SMEDn_PRM_S11) */
    struct {
      _BITS               : 2;    //   Reserved, must be kept clear
      _BITS   CEDGE       : 2;    ///< external edge jumping selection
      _BITS   CNT_RSTC    : 1;    ///< counter reset definition
      _BITS   PULS_CMP    : 1;    ///< PWM pulse out level definition
      _BITS   HOLD_EXIT   : 1;    ///< HOLD state exit cause definition
      _BITS               : 1;    //   Reserved, must be kept clear
    } PRM_S11;

    /** @brief SMEDn Parameter 2 S1 register (_SMEDn_PRM_S12) */
    struct {
      _BITS   LATCH_RS    : 1;    ///< latch reset definition
      _BITS               : 7;    //   Reserved, must be kept clear
    } PRM_S12;

    /** @brief SMEDn Parameter 0 S2 register (_SMEDn_PRM_S20) */
    struct {
      _BITS   NX_STAT     : 2;    ///< next event controlled state definition
      _BITS   EDGE        : 2;    ///< external edge jumping selection
      _BITS   CNT_RSTE    : 1;    ///< counter reset definition
      _BITS   PULS_EDG    : 1;    ///< PWM pulse out level definition
      _BITS   HOLD_JMP    : 1;    ///< hold state definition
      _BITS   AND_OR      : 1;    ///< jumping Boolean function definition
    } PRM_S20;

    /** @brief SMEDn Parameter 1 S2 register (_SMEDn_PRM_S21) */
    struct {
      _BITS               : 2;    //   Reserved, must be kept clear
      _BITS   CEDGE       : 2;    ///< external edge jumping selection
      _BITS   CNT_RSTC    : 1;    ///< counter reset definition
      _BITS   PULS_CMP    : 1;    ///< PWM pulse out level definition
      _BITS   HOLD_EXIT   : 1;    ///< HOLD state exit cause definition
      _BITS               : 1;    //   Reserved, must be kept clear
    } PRM_S21;

    /** @brief SMEDn Parameter 2 S2 register (_SMEDn_PRM_S22) */
    struct {
      _BITS   LATCH_RS    : 1;    ///< latch reset definition
      _BITS               : 7;    //   Reserved, must be kept clear
    } PRM_S22;

    /** @brief SMEDn Parameter 0 S3 register (_SMEDn_PRM_S30) */
    struct {
      _BITS   NX_STAT     : 2;    ///< next event controlled state definition
      _BITS   EDGE        : 2;    ///< external edge jumping selection
      _BITS   CNT_RSTE    : 1;    ///< counter reset definition
      _BITS   PULS_EDG    : 1;    ///< PWM pulse out level definition
      _BITS   HOLD_JMP    : 1;    ///< hold state definition
      _BITS   AND_OR      : 1;    ///< jumping Boolean function definition
    } PRM_S30;

    /** @brief SMEDn Parameter 1 S3 register (_SMEDn_PRM_S31) */
    struct {
      _BITS               : 2;    //   Reserved, must be kept clear
      _BITS   CEDGE       : 2;    ///< external edge jumping selection
      _BITS   CNT_RSTC    : 1;    ///< counter reset definition
      _BITS   PULS_CMP    : 1;    ///< PWM pulse out level definition
      _BITS   HOLD_EXIT   : 1;    ///< HOLD state exit cause definition
      _BITS               : 1;    //   Reserved, must be kept clear
    } PRM_S31;

    /** @brief SMEDn Parameter 2 S3 register (_SMEDn_PRM_S32) */
    struct {
      _BITS   LATCH_RS    : 1;    ///< latch reset definition
      _BITS               : 7;    //   Reserved, must be kept clear
    } PRM_S32;

    /** @brief SMEDn Timer configuration register (_SMEDn_CFG) */
    struct {
      _BITS   TIM_NUM     : 2;    ///< time registers to be temporary incremented selection
      _BITS   TIM_UPD     : 2;    ///< time registers update mode
      _BITS               : 4;    //   Reserved, must be kept clear
    } CFG;

    /** @brief SMEDn Dump counter LSB register (_SMEDn_DMPL) */
    struct {
      _BITS   CNT         : 8;    ///< counter dump value low byte
    } DMPL;

    /** @brief SMEDn Dump counter MSB register (_SMEDn_DMPH) */
    struct {
      _BITS   CNT         : 8;    ///< counter dump value high byte
    } DMPH;

    /** @brief SMEDn General status register (_SMEDn_GSTS) */
    struct {
      _BITS   EX0_DUMP    : 1;    ///< dumping cause flag InSig[0]
      _BITS   EX1_DUMP    : 1;    ///< dumping cause flag InSig[1]
      _BITS   EX2_DUMP    : 1;    ///< dumping cause flag InSig[2]
      _BITS   CNT_FLAG    : 1;    ///< counter reset flag when dump is enabled
      _BITS   DMP_LK      : 2;    ///< counter dump status
      _BITS   EVENT_OV    : 1;    ///< event overflow flag
      _BITS               : 1;    //   Reserved, must be kept clear
    } GSTS;

    /** @brief SMEDn Interrupt request register (_SMEDn_IRQ) */
    struct {
      _BITS   CNT_OVER    : 1;    ///< counter overflow interrupt request
      _BITS   EXT0_INT    : 1;    ///< InSig[0] capture interrupt request
      _BITS   EXT1_INT    : 1;    ///< InSig[1] capture interrupt request
      _BITS   EXT2_INT    : 1;    ///< InSig[2] capture interrupt request
      _BITS   STA_S0_IT   : 1;    ///< Tmr(0) transition interrupt request
      _BITS   STA_S1_IT   : 1;    ///< Tmr(1) transition interrupt request
      _BITS   STA_S2_IT   : 1;    ///< Tmr(2) transition interrupt request
      _BITS   STA_S3_IT   : 1;    ///< Tmr(3) transition interrupt request
    } IRQ;

    /** @brief SMEDn Interrupt enable register (_SMEDn_IER) */
    struct {
      _BITS   CNT_OV_E    : 1;    ///< counter overflow interrupt enable
      _BITS   IT_EN_EX0   : 1;    ///< InSig[0] capture interrupt enable
      _BITS   IT_EN_EX1   : 1;    ///< InSig[1] capture interrupt enable
      _BITS   IT_EN_EX2   : 1;    ///< InSig[2] capture interrupt enable
      _BITS   IT_STA_S0   : 1;    ///< Tmr(0) transition interrupt enable
      _BITS   IT_STA_S1   : 1;    ///< Tmr(1) transition interrupt enable
      _BITS   IT_STA_S2   : 1;    ///< Tmr(2) transition interrupt enable
      _BITS   IT_STA_S3   : 1;    ///< Tmr(3) transition interrupt enable
    } IER;

    /** @brief SMEDn External event control register (_SMEDn_ISEL) */
    struct {
      _BITS   INPUT0_EN   : 1;    ///< InSig[0] input enable
      _BITS   INPUT1_EN   : 1;    ///< InSig[1] input enable
      _BITS   INPUT2_EN   : 1;    ///< InSig[2] input enable
      _BITS   INPUT_LAT   : 1;    ///< enable latch function on InSig[0] input
      _BITS               : 4;    //   Reserved, must be kept clear
    } ISEL;

    /** @brief SMEDn Dump enable register (_SMEDn_DMP) */
    struct {
      _BITS   DMPE_EX0    : 1;    ///< dump update mode for InSig[0] event
      _BITS   DMPE_EX1    : 1;    ///< dump update mode for InSig[1] event
      _BITS   DMPE_EX2    : 1;    ///< dump update mode for InSig[2] event
      _BITS   DMP_EVER    : 1;    ///< dump update mode all
      _BITS   CPL_IT_GE   : 1;    ///< lock together SMEDn_GSTS and SMEDn_IRQ reset signal
      _BITS               : 3;    //   Reserved, must be kept clear
    } DMP;

    /** @brief SMEDn finite state machine status register (_SMEDn_FSM_STS) */
    struct {
      _BITS   FSM         : 3;    ///< finite state machine state variable
      _BITS   PWM         : 1;    ///< PWM output signal
      _BITS   EVINP       : 3;    ///< event input signals
      _BITS               : 1;    //   Reserved, must be kept clear
    } FSM_STS;

  } _SMED_t;

  /* Pointer to SMED0 registers */
  #if defined(SMED0_AddressBase)
    #define _SMED0            _SFR(_SMED_t,  SMED0_AddressBase)          ///< SMED0 struct/bit access
    #define _SMED0_CTR        _SFR(uint8_t,  SMED0_AddressBase+0x00)     ///< SMED0 Control register
    #define _SMED0_CTR_TMR    _SFR(uint8_t,  SMED0_AddressBase+0x01)     ///< SMED0 Control timer register
    #define _SMED0_CTR_INP    _SFR(uint8_t,  SMED0_AddressBase+0x02)     ///< SMED0 Control input register
    #define _SMED0_CTR_DTR    _SFR(uint8_t,  SMED0_AddressBase+0x03)     ///< SMED0 Dithering register
    #define _SMED0_TMR_T0L    _SFR(uint8_t,  SMED0_AddressBase+0x04)     ///< SMED0 Time T0 LSB register
    #define _SMED0_TMR_T0H    _SFR(uint8_t,  SMED0_AddressBase+0x05)     ///< SMED0 Time T0 MSB register
    #define _SMED0_TMR_T1L    _SFR(uint8_t,  SMED0_AddressBase+0x06)     ///< SMED0 Time T1 LSB register
    #define _SMED0_TMR_T1H    _SFR(uint8_t,  SMED0_AddressBase+0x07)     ///< SMED0 Time T1 MSB register
    #define _SMED0_TMR_T2L    _SFR(uint8_t,  SMED0_AddressBase+0x08)     ///< SMED0 Time T2 LSB register
    #define _SMED0_TMR_T2H    _SFR(uint8_t,  SMED0_AddressBase+0x09)     ///< SMED0 Time T2 MSB register
    #define _SMED0_TMR_T3L    _SFR(uint8_t,  SMED0_AddressBase+0x0A)     ///< SMED0 Time T3 LSB register
    #define _SMED0_TMR_T3H    _SFR(uint8_t,  SMED0_AddressBase+0x0B)     ///< SMED0 Time T3 MSB register
    #define _SMED0_PRM_ID0    _SFR(uint8_t,  SMED0_AddressBase+0x0C)     ///< SMED0 Parameter 0 IDLE register
    #define _SMED0_PRM_ID1    _SFR(uint8_t,  SMED0_AddressBase+0x0D)     ///< SMED0 Parameter 1 IDLE register
    #define _SMED0_PRM_ID2    _SFR(uint8_t,  SMED0_AddressBase+0x0E)     ///< SMED0 Parameter 2 IDLE register
    #define _SMED0_PRM_S00    _SFR(uint8_t,  SMED0_AddressBase+0x0F)     ///< SMED0 Parameter 0 S0 register
    #define _SMED0_PRM_S01    _SFR(uint8_t,  SMED0_AddressBase+0x10)     ///< SMED0 Parameter 1 S0 register
    #define _SMED0_PRM_S02    _SFR(uint8_t,  SMED0_AddressBase+0x11)     ///< SMED0 Parameter 2 S0 register
    #define _SMED0_PRM_S10    _SFR(uint8_t,  SMED0_AddressBase+0x12)     ///< SMED0 Parameter 0 S1 register
    #define _SMED0_PRM_S11    _SFR(uint8_t,  SMED0_AddressBase+0x13)     ///< SMED0 Parameter 1 S1 register
    #define _SMED0_PRM_S12    _SFR(uint8_t,  SMED0_AddressBase+0x14)     ///< SMED0 Parameter 2 S1 register
    #define _SMED0_PRM_S20    _SFR(uint8_t,  SMED0_AddressBase+0x15)     ///< SMED0 Parameter 0 S2 register
    #define _SMED0_PRM_S21    _SFR(uint8_t,  SMED0_AddressBase+0x16)     ///< SMED0 Parameter 1 S2 register
    #define _SMED0_PRM_S22    _SFR(uint8_t,  SMED0_AddressBase+0x17)     ///< SMED0 Parameter 2 S2 register
    #define _SMED0_PRM_S30    _SFR(uint8_t,  SMED0_AddressBase+0x18)     ///< SMED0 Parameter 0 S3 register
    #define _SMED0_PRM_S31    _SFR(uint8_t,  SMED0_AddressBase+0x19)     ///< SMED0 Parameter 1 S3 register
    #define _SMED0_PRM_S32    _SFR(uint8_t,  SMED0_AddressBase+0x1A)     ///< SMED0 Parameter 2 S3 register
    #define _SMED0_CFG        _SFR(uint8_t,  SMED0_AddressBase+0x1B)     ///< SMED0 Timer configuration register
    #define _SMED0_DMPL       _SFR(uint8_t,  SMED0_AddressBase+0x1C)     ///< SMED0 Dump counter LSB register
    #define _SMED0_DMPH       _SFR(uint8_t,  SMED0_AddressBase+0x1D)     ///< SMED0 Dump counter MSB register
    #define _SMED0_GSTS       _SFR(uint8_t,  SMED0_AddressBase+0x1E)     ///< SMED0 General status register
    #define _SMED0_IRQ        _SFR(uint8_t,  SMED0_AddressBase+0x1F)     ///< SMED0 Interrupt request register
    #define _SMED0_IER        _SFR(uint8_t,  SMED0_AddressBase+0x20)     ///< SMED0 Interrupt enable register
    #define _SMED0_ISEL       _SFR(uint8_t,  SMED0_AddressBase+0x21)     ///< SMED0 External event control register
    #define _SMED0_DMP        _SFR(uint8_t,  SMED0_AddressBase+0x22)     ///< SMED0 Dump enable register
    #define _SMED0_FSM_STS    _SFR(uint8_t,  SMED0_AddressBase+0x23)     ///< SMED0 finite state machine status register
  #endif // SMED0_AddressBase

  /* Pointer to SMED1 registers */
  #if defined(SMED1_AddressBase)
    #define _SMED1            _SFR(_SMED_t,  SMED1_AddressBase)          ///< SMED1 struct/bit access
    #define _SMED1_CTR        _SFR(uint8_t,  SMED1_AddressBase+0x00)     ///< SMED1 Control register
    #define _SMED1_CTR_TMR    _SFR(uint8_t,  SMED1_AddressBase+0x01)     ///< SMED1 Control timer register
    #define _SMED1_CTR_INP    _SFR(uint8_t,  SMED1_AddressBase+0x02)     ///< SMED1 Control input register
    #define _SMED1_CTR_DTR    _SFR(uint8_t,  SMED1_AddressBase+0x03)     ///< SMED1 Dithering register
    #define _SMED1_TMR_T0L    _SFR(uint8_t,  SMED1_AddressBase+0x04)     ///< SMED1 Time T0 LSB register
    #define _SMED1_TMR_T0H    _SFR(uint8_t,  SMED1_AddressBase+0x05)     ///< SMED1 Time T0 MSB register
    #define _SMED1_TMR_T1L    _SFR(uint8_t,  SMED1_AddressBase+0x06)     ///< SMED1 Time T1 LSB register
    #define _SMED1_TMR_T1H    _SFR(uint8_t,  SMED1_AddressBase+0x07)     ///< SMED1 Time T1 MSB register
    #define _SMED1_TMR_T2L    _SFR(uint8_t,  SMED1_AddressBase+0x08)     ///< SMED1 Time T2 LSB register
    #define _SMED1_TMR_T2H    _SFR(uint8_t,  SMED1_AddressBase+0x09)     ///< SMED1 Time T2 MSB register
    #define _SMED1_TMR_T3L    _SFR(uint8_t,  SMED1_AddressBase+0x0A)     ///< SMED1 Time T3 LSB register
    #define _SMED1_TMR_T3H    _SFR(uint8_t,  SMED1_AddressBase+0x0B)     ///< SMED1 Time T3 MSB register
    #define _SMED1_PRM_ID0    _SFR(uint8_t,  SMED1_AddressBase+0x0C)     ///< SMED1 Parameter 0 IDLE register
    #define _SMED1_PRM_ID1    _SFR(uint8_t,  SMED1_AddressBase+0x0D)     ///< SMED1 Parameter 1 IDLE register
    #define _SMED1_PRM_ID2    _SFR(uint8_t,  SMED1_AddressBase+0x0E)     ///< SMED1 Parameter 2 IDLE register
    #define _SMED1_PRM_S00    _SFR(uint8_t,  SMED1_AddressBase+0x0F)     ///< SMED1 Parameter 0 S0 register
    #define _SMED1_PRM_S01    _SFR(uint8_t,  SMED1_AddressBase+0x10)     ///< SMED1 Parameter 1 S0 register
    #define _SMED1_PRM_S02    _SFR(uint8_t,  SMED1_AddressBase+0x11)     ///< SMED1 Parameter 2 S0 register
    #define _SMED1_PRM_S10    _SFR(uint8_t,  SMED1_AddressBase+0x12)     ///< SMED1 Parameter 0 S1 register
    #define _SMED1_PRM_S11    _SFR(uint8_t,  SMED1_AddressBase+0x13)     ///< SMED1 Parameter 1 S1 register
    #define _SMED1_PRM_S12    _SFR(uint8_t,  SMED1_AddressBase+0x14)     ///< SMED1 Parameter 2 S1 register
    #define _SMED1_PRM_S20    _SFR(uint8_t,  SMED1_AddressBase+0x15)     ///< SMED1 Parameter 0 S2 register
    #define _SMED1_PRM_S21    _SFR(uint8_t,  SMED1_AddressBase+0x16)     ///< SMED1 Parameter 1 S2 register
    #define _SMED1_PRM_S22    _SFR(uint8_t,  SMED1_AddressBase+0x17)     ///< SMED1 Parameter 2 S2 register
    #define _SMED1_PRM_S30    _SFR(uint8_t,  SMED1_AddressBase+0x18)     ///< SMED1 Parameter 0 S3 register
    #define _SMED1_PRM_S31    _SFR(uint8_t,  SMED1_AddressBase+0x19)     ///< SMED1 Parameter 1 S3 register
    #define _SMED1_PRM_S32    _SFR(uint8_t,  SMED1_AddressBase+0x1A)     ///< SMED1 Parameter 2 S3 register
    #define _SMED1_CFG        _SFR(uint8_t,  SMED1_AddressBase+0x1B)     ///< SMED1 Timer configuration register
    #define _SMED1_DMP_L      _SFR(uint8_t,  SMED1_AddressBase+0x1C)     ///< SMED1 Dump counter LSB register
    #define _SMED1_DMP_H      _SFR(uint8_t,  SMED1_AddressBase+0x1D)     ///< SMED1 Dump counter MSB register
    #define _SMED1_GSTS       _SFR(uint8_t,  SMED1_AddressBase+0x1E)     ///< SMED1 General status register
    #define _SMED1_IRQ        _SFR(uint8_t,  SMED1_AddressBase+0x1F)     ///< SMED1 Interrupt regquest register
    #define _SMED1_IER        _SFR(uint8_t,  SMED1_AddressBase+0x20)     ///< SMED1 Interrupt enable register
    #define _SMED1_ISEL       _SFR(uint8_t,  SMED1_AddressBase+0x21)     ///< SMED1 Externalevent control register
    #define _SMED1_DMP        _SFR(uint8_t,  SMED1_AddressBase+0x22)     ///< SMED1 Dump enable register
    #define _SMED1_FSM_STS    _SFR(uint8_t,  SMED1_AddressBase+0x23)     ///< SMED1 finite state machine status register
  #endif // SMED1_AddressBase

  /* Pointer to SMED2 registers */
  #if defined(SMED2_AddressBase)
    #define _SMED2            _SFR(_SMED_t,  SMED2_AddressBase)          ///< SMED2 struct/bit access
    #define _SMED2_CTR        _SFR(uint8_t,  SMED2_AddressBase+0x00)     ///< SMED2 Control register
    #define _SMED2_CTR_TMR    _SFR(uint8_t,  SMED2_AddressBase+0x01)     ///< SMED2 Control timer register
    #define _SMED2_CTR_INP    _SFR(uint8_t,  SMED2_AddressBase+0x02)     ///< SMED2 Control input register
    #define _SMED2_CTR_DTR    _SFR(uint8_t,  SMED2_AddressBase+0x03)     ///< SMED2 Dithering register
    #define _SMED2_TMR_T0L    _SFR(uint8_t,  SMED2_AddressBase+0x04)     ///< SMED2 Time T0 LSB register
    #define _SMED2_TMR_T0H    _SFR(uint8_t,  SMED2_AddressBase+0x05)     ///< SMED2 Time T0 MSB register
    #define _SMED2_TMR_T1L    _SFR(uint8_t,  SMED2_AddressBase+0x06)     ///< SMED2 Time T1 LSB register
    #define _SMED2_TMR_T1H    _SFR(uint8_t,  SMED2_AddressBase+0x07)     ///< SMED2 Time T1 MSB register
    #define _SMED2_TMR_T2L    _SFR(uint8_t,  SMED2_AddressBase+0x08)     ///< SMED2 Time T2 LSB register
    #define _SMED2_TMR_T2H    _SFR(uint8_t,  SMED2_AddressBase+0x09)     ///< SMED2 Time T2 MSB register
    #define _SMED2_TMR_T3L    _SFR(uint8_t,  SMED2_AddressBase+0x0A)     ///< SMED2 Time T3 LSB register
    #define _SMED2_TMR_T3H    _SFR(uint8_t,  SMED2_AddressBase+0x0B)     ///< SMED2 Time T3 MSB register
    #define _SMED2_PRM_ID0    _SFR(uint8_t,  SMED2_AddressBase+0x0C)     ///< SMED2 Parameter 0 IDLE register
    #define _SMED2_PRM_ID1    _SFR(uint8_t,  SMED2_AddressBase+0x0D)     ///< SMED2 Parameter 1 IDLE register
    #define _SMED2_PRM_ID2    _SFR(uint8_t,  SMED2_AddressBase+0x0E)     ///< SMED2 Parameter 2 IDLE register
    #define _SMED2_PRM_S00    _SFR(uint8_t,  SMED2_AddressBase+0x0F)     ///< SMED2 Parameter 0 S0 register
    #define _SMED2_PRM_S01    _SFR(uint8_t,  SMED2_AddressBase+0x10)     ///< SMED2 Parameter 1 S0 register
    #define _SMED2_PRM_S02    _SFR(uint8_t,  SMED2_AddressBase+0x11)     ///< SMED2 Parameter 2 S0 register
    #define _SMED2_PRM_S10    _SFR(uint8_t,  SMED2_AddressBase+0x12)     ///< SMED2 Parameter 0 S1 register
    #define _SMED2_PRM_S11    _SFR(uint8_t,  SMED2_AddressBase+0x13)     ///< SMED2 Parameter 1 S1 register
    #define _SMED2_PRM_S12    _SFR(uint8_t,  SMED2_AddressBase+0x14)     ///< SMED2 Parameter 2 S1 register
    #define _SMED2_PRM_S20    _SFR(uint8_t,  SMED2_AddressBase+0x15)     ///< SMED2 Parameter 0 S2 register
    #define _SMED2_PRM_S21    _SFR(uint8_t,  SMED2_AddressBase+0x16)     ///< SMED2 Parameter 1 S2 register
    #define _SMED2_PRM_S22    _SFR(uint8_t,  SMED2_AddressBase+0x17)     ///< SMED2 Parameter 2 S2 register
    #define _SMED2_PRM_S30    _SFR(uint8_t,  SMED2_AddressBase+0x18)     ///< SMED2 Parameter 0 S3 register
    #define _SMED2_PRM_S31    _SFR(uint8_t,  SMED2_AddressBase+0x19)     ///< SMED2 Parameter 1 S3 register
    #define _SMED2_PRM_S32    _SFR(uint8_t,  SMED2_AddressBase+0x1A)     ///< SMED2 Parameter 2 S3 register
    #define _SMED2_CFG        _SFR(uint8_t,  SMED2_AddressBase+0x1B)     ///< SMED2 Timer configuration register
    #define _SMED2_DMP_L      _SFR(uint8_t,  SMED2_AddressBase+0x1C)     ///< SMED2 Dump counter LSB register
    #define _SMED2_DMP_H      _SFR(uint8_t,  SMED2_AddressBase+0x1D)     ///< SMED2 Dump counter MSB register
    #define _SMED2_GSTS       _SFR(uint8_t,  SMED2_AddressBase+0x1E)     ///< SMED2 General status register
    #define _SMED2_IRQ        _SFR(uint8_t,  SMED2_AddressBase+0x1F)     ///< SMED2 Interrupt regquest register
    #define _SMED2_IER        _SFR(uint8_t,  SMED2_AddressBase+0x20)     ///< SMED2 Interrupt enable register
    #define _SMED2_ISEL       _SFR(uint8_t,  SMED2_AddressBase+0x21)     ///< SMED2 Externalevent control register
    #define _SMED2_DMP        _SFR(uint8_t,  SMED2_AddressBase+0x22)     ///< SMED2 Dump enable register
    #define _SMED2_FSM_STS    _SFR(uint8_t,  SMED2_AddressBase+0x23)     ///< SMED2 finite state machine status register
  #endif // SMED2_AddressBase

  /* Pointer to SMED3 registers */
  #if defined(SMED3_AddressBase)
    #define _SMED3            _SFR(_SMED_t,  SMED3_AddressBase)          ///< SMED3 struct/bit access
    #define _SMED3_CTR        _SFR(uint8_t,  SMED3_AddressBase+0x00)     ///< SMED3 Control register
    #define _SMED3_CTR_TMR    _SFR(uint8_t,  SMED3_AddressBase+0x01)     ///< SMED3 Control timer register
    #define _SMED3_CTR_INP    _SFR(uint8_t,  SMED3_AddressBase+0x02)     ///< SMED3 Control input register
    #define _SMED3_CTR_DTR    _SFR(uint8_t,  SMED3_AddressBase+0x03)     ///< SMED3 Dithering register
    #define _SMED3_TMR_T0L    _SFR(uint8_t,  SMED3_AddressBase+0x04)     ///< SMED3 Time T0 LSB register
    #define _SMED3_TMR_T0H    _SFR(uint8_t,  SMED3_AddressBase+0x05)     ///< SMED3 Time T0 MSB register
    #define _SMED3_TMR_T1L    _SFR(uint8_t,  SMED3_AddressBase+0x06)     ///< SMED3 Time T1 LSB register
    #define _SMED3_TMR_T1H    _SFR(uint8_t,  SMED3_AddressBase+0x07)     ///< SMED3 Time T1 MSB register
    #define _SMED3_TMR_T2L    _SFR(uint8_t,  SMED3_AddressBase+0x08)     ///< SMED3 Time T2 LSB register
    #define _SMED3_TMR_T2H    _SFR(uint8_t,  SMED3_AddressBase+0x09)     ///< SMED3 Time T2 MSB register
    #define _SMED3_TMR_T3L    _SFR(uint8_t,  SMED3_AddressBase+0x0A)     ///< SMED3 Time T3 LSB register
    #define _SMED3_TMR_T3H    _SFR(uint8_t,  SMED3_AddressBase+0x0B)     ///< SMED3 Time T3 MSB register
    #define _SMED3_PRM_ID0    _SFR(uint8_t,  SMED3_AddressBase+0x0C)     ///< SMED3 Parameter 0 IDLE register
    #define _SMED3_PRM_ID1    _SFR(uint8_t,  SMED3_AddressBase+0x0D)     ///< SMED3 Parameter 1 IDLE register
    #define _SMED3_PRM_ID2    _SFR(uint8_t,  SMED3_AddressBase+0x0E)     ///< SMED3 Parameter 2 IDLE register
    #define _SMED3_PRM_S00    _SFR(uint8_t,  SMED3_AddressBase+0x0F)     ///< SMED3 Parameter 0 S0 register
    #define _SMED3_PRM_S01    _SFR(uint8_t,  SMED3_AddressBase+0x10)     ///< SMED3 Parameter 1 S0 register
    #define _SMED3_PRM_S02    _SFR(uint8_t,  SMED3_AddressBase+0x11)     ///< SMED3 Parameter 2 S0 register
    #define _SMED3_PRM_S10    _SFR(uint8_t,  SMED3_AddressBase+0x12)     ///< SMED3 Parameter 0 S1 register
    #define _SMED3_PRM_S11    _SFR(uint8_t,  SMED3_AddressBase+0x13)     ///< SMED3 Parameter 1 S1 register
    #define _SMED3_PRM_S12    _SFR(uint8_t,  SMED3_AddressBase+0x14)     ///< SMED3 Parameter 2 S1 register
    #define _SMED3_PRM_S20    _SFR(uint8_t,  SMED3_AddressBase+0x15)     ///< SMED3 Parameter 0 S2 register
    #define _SMED3_PRM_S21    _SFR(uint8_t,  SMED3_AddressBase+0x16)     ///< SMED3 Parameter 1 S2 register
    #define _SMED3_PRM_S22    _SFR(uint8_t,  SMED3_AddressBase+0x17)     ///< SMED3 Parameter 2 S2 register
    #define _SMED3_PRM_S30    _SFR(uint8_t,  SMED3_AddressBase+0x18)     ///< SMED3 Parameter 0 S3 register
    #define _SMED3_PRM_S31    _SFR(uint8_t,  SMED3_AddressBase+0x19)     ///< SMED3 Parameter 1 S3 register
    #define _SMED3_PRM_S32    _SFR(uint8_t,  SMED3_AddressBase+0x1A)     ///< SMED3 Parameter 2 S3 register
    #define _SMED3_CFG        _SFR(uint8_t,  SMED3_AddressBase+0x1B)     ///< SMED3 Timer configuration register
    #define _SMED3_DMP_L      _SFR(uint8_t,  SMED3_AddressBase+0x1C)     ///< SMED3 Dump counter LSB register
    #define _SMED3_DMP_H      _SFR(uint8_t,  SMED3_AddressBase+0x1D)     ///< SMED3 Dump counter MSB register
    #define _SMED3_GSTS       _SFR(uint8_t,  SMED3_AddressBase+0x1E)     ///< SMED3 General status register
    #define _SMED3_IRQ        _SFR(uint8_t,  SMED3_AddressBase+0x1F)     ///< SMED3 Interrupt regquest register
    #define _SMED3_IER        _SFR(uint8_t,  SMED3_AddressBase+0x20)     ///< SMED3 Interrupt enable register
    #define _SMED3_ISEL       _SFR(uint8_t,  SMED3_AddressBase+0x21)     ///< SMED3 Externalevent control register
    #define _SMED3_DMP        _SFR(uint8_t,  SMED3_AddressBase+0x22)     ///< SMED3 Dump enable register
    #define _SMED3_FSM_STS    _SFR(uint8_t,  SMED3_AddressBase+0x23)     ///< SMED3 finite state machine status register
  #endif // SMED3_AddressBase

  /* Pointer to SMED4 registers */
  #if defined(SMED4_AddressBase)
    #define _SMED4            _SFR(_SMED_t,  SMED4_AddressBase)          ///< SMED4 struct/bit access
    #define _SMED4_CTR        _SFR(uint8_t,  SMED4_AddressBase+0x00)     ///< SMED4 Control register
    #define _SMED4_CTR_TMR    _SFR(uint8_t,  SMED4_AddressBase+0x01)     ///< SMED4 Control timer register
    #define _SMED4_CTR_INP    _SFR(uint8_t,  SMED4_AddressBase+0x02)     ///< SMED4 Control input register
    #define _SMED4_CTR_DTR    _SFR(uint8_t,  SMED4_AddressBase+0x03)     ///< SMED4 Dithering register
    #define _SMED4_TMR_T0L    _SFR(uint8_t,  SMED4_AddressBase+0x04)     ///< SMED4 Time T0 LSB register
    #define _SMED4_TMR_T0H    _SFR(uint8_t,  SMED4_AddressBase+0x05)     ///< SMED4 Time T0 MSB register
    #define _SMED4_TMR_T1L    _SFR(uint8_t,  SMED4_AddressBase+0x06)     ///< SMED4 Time T1 LSB register
    #define _SMED4_TMR_T1H    _SFR(uint8_t,  SMED4_AddressBase+0x07)     ///< SMED4 Time T1 MSB register
    #define _SMED4_TMR_T2L    _SFR(uint8_t,  SMED4_AddressBase+0x08)     ///< SMED4 Time T2 LSB register
    #define _SMED4_TMR_T2H    _SFR(uint8_t,  SMED4_AddressBase+0x09)     ///< SMED4 Time T2 MSB register
    #define _SMED4_TMR_T3L    _SFR(uint8_t,  SMED4_AddressBase+0x0A)     ///< SMED4 Time T3 LSB register
    #define _SMED4_TMR_T3H    _SFR(uint8_t,  SMED4_AddressBase+0x0B)     ///< SMED4 Time T3 MSB register
    #define _SMED4_PRM_ID0    _SFR(uint8_t,  SMED4_AddressBase+0x0C)     ///< SMED4 Parameter 0 IDLE register
    #define _SMED4_PRM_ID1    _SFR(uint8_t,  SMED4_AddressBase+0x0D)     ///< SMED4 Parameter 1 IDLE register
    #define _SMED4_PRM_ID2    _SFR(uint8_t,  SMED4_AddressBase+0x0E)     ///< SMED4 Parameter 2 IDLE register
    #define _SMED4_PRM_S00    _SFR(uint8_t,  SMED4_AddressBase+0x0F)     ///< SMED4 Parameter 0 S0 register
    #define _SMED4_PRM_S01    _SFR(uint8_t,  SMED4_AddressBase+0x10)     ///< SMED4 Parameter 1 S0 register
    #define _SMED4_PRM_S02    _SFR(uint8_t,  SMED4_AddressBase+0x11)     ///< SMED4 Parameter 2 S0 register
    #define _SMED4_PRM_S10    _SFR(uint8_t,  SMED4_AddressBase+0x12)     ///< SMED4 Parameter 0 S1 register
    #define _SMED4_PRM_S11    _SFR(uint8_t,  SMED4_AddressBase+0x13)     ///< SMED4 Parameter 1 S1 register
    #define _SMED4_PRM_S12    _SFR(uint8_t,  SMED4_AddressBase+0x14)     ///< SMED4 Parameter 2 S1 register
    #define _SMED4_PRM_S20    _SFR(uint8_t,  SMED4_AddressBase+0x15)     ///< SMED4 Parameter 0 S2 register
    #define _SMED4_PRM_S21    _SFR(uint8_t,  SMED4_AddressBase+0x16)     ///< SMED4 Parameter 1 S2 register
    #define _SMED4_PRM_S22    _SFR(uint8_t,  SMED4_AddressBase+0x17)     ///< SMED4 Parameter 2 S2 register
    #define _SMED4_PRM_S30    _SFR(uint8_t,  SMED4_AddressBase+0x18)     ///< SMED4 Parameter 0 S3 register
    #define _SMED4_PRM_S31    _SFR(uint8_t,  SMED4_AddressBase+0x19)     ///< SMED4 Parameter 1 S3 register
    #define _SMED4_PRM_S32    _SFR(uint8_t,  SMED4_AddressBase+0x1A)     ///< SMED4 Parameter 2 S3 register
    #define _SMED4_CFG        _SFR(uint8_t,  SMED4_AddressBase+0x1B)     ///< SMED4 Timer configuration register
    #define _SMED4_DMP_L      _SFR(uint8_t,  SMED4_AddressBase+0x1C)     ///< SMED4 Dump counter LSB register
    #define _SMED4_DMP_H      _SFR(uint8_t,  SMED4_AddressBase+0x1D)     ///< SMED4 Dump counter MSB register
    #define _SMED4_GSTS       _SFR(uint8_t,  SMED4_AddressBase+0x1E)     ///< SMED4 General status register
    #define _SMED4_IRQ        _SFR(uint8_t,  SMED4_AddressBase+0x1F)     ///< SMED4 Interrupt regquest register
    #define _SMED4_IER        _SFR(uint8_t,  SMED4_AddressBase+0x20)     ///< SMED4 Interrupt enable register
    #define _SMED4_ISEL       _SFR(uint8_t,  SMED4_AddressBase+0x21)     ///< SMED4 Externalevent control register
    #define _SMED4_DMP        _SFR(uint8_t,  SMED4_AddressBase+0x22)     ///< SMED4 Dump enable register
    #define _SMED4_FSM_STS    _SFR(uint8_t,  SMED4_AddressBase+0x23)     ///< SMED4 finite state machine status register
  #endif // SMED4_AddressBase

  /* Pointer to SMED5 registers */
  #if defined(SMED5_AddressBase)
    #define _SMED5            _SFR(_SMED_t,  SMED5_AddressBase)          ///< SMED5 struct/bit access
    #define _SMED5_CTR        _SFR(uint8_t,  SMED5_AddressBase+0x00)     ///< SMED5 Control register
    #define _SMED5_CTR_TMR    _SFR(uint8_t,  SMED5_AddressBase+0x01)     ///< SMED5 Control timer register
    #define _SMED5_CTR_INP    _SFR(uint8_t,  SMED5_AddressBase+0x02)     ///< SMED5 Control input register
    #define _SMED5_CTR_DTR    _SFR(uint8_t,  SMED5_AddressBase+0x03)     ///< SMED5 Dithering register
    #define _SMED5_TMR_T0L    _SFR(uint8_t,  SMED5_AddressBase+0x04)     ///< SMED5 Time T0 LSB register
    #define _SMED5_TMR_T0H    _SFR(uint8_t,  SMED5_AddressBase+0x05)     ///< SMED5 Time T0 MSB register
    #define _SMED5_TMR_T1L    _SFR(uint8_t,  SMED5_AddressBase+0x06)     ///< SMED5 Time T1 LSB register
    #define _SMED5_TMR_T1H    _SFR(uint8_t,  SMED5_AddressBase+0x07)     ///< SMED5 Time T1 MSB register
    #define _SMED5_TMR_T2L    _SFR(uint8_t,  SMED5_AddressBase+0x08)     ///< SMED5 Time T2 LSB register
    #define _SMED5_TMR_T2H    _SFR(uint8_t,  SMED5_AddressBase+0x09)     ///< SMED5 Time T2 MSB register
    #define _SMED5_TMR_T3L    _SFR(uint8_t,  SMED5_AddressBase+0x0A)     ///< SMED5 Time T3 LSB register
    #define _SMED5_TMR_T3H    _SFR(uint8_t,  SMED5_AddressBase+0x0B)     ///< SMED5 Time T3 MSB register
    #define _SMED5_PRM_ID0    _SFR(uint8_t,  SMED5_AddressBase+0x0C)     ///< SMED5 Parameter 0 IDLE register
    #define _SMED5_PRM_ID1    _SFR(uint8_t,  SMED5_AddressBase+0x0D)     ///< SMED5 Parameter 1 IDLE register
    #define _SMED5_PRM_ID2    _SFR(uint8_t,  SMED5_AddressBase+0x0E)     ///< SMED5 Parameter 2 IDLE register
    #define _SMED5_PRM_S00    _SFR(uint8_t,  SMED5_AddressBase+0x0F)     ///< SMED5 Parameter 0 S0 register
    #define _SMED5_PRM_S01    _SFR(uint8_t,  SMED5_AddressBase+0x10)     ///< SMED5 Parameter 1 S0 register
    #define _SMED5_PRM_S02    _SFR(uint8_t,  SMED5_AddressBase+0x11)     ///< SMED5 Parameter 2 S0 register
    #define _SMED5_PRM_S10    _SFR(uint8_t,  SMED5_AddressBase+0x12)     ///< SMED5 Parameter 0 S1 register
    #define _SMED5_PRM_S11    _SFR(uint8_t,  SMED5_AddressBase+0x13)     ///< SMED5 Parameter 1 S1 register
    #define _SMED5_PRM_S12    _SFR(uint8_t,  SMED5_AddressBase+0x14)     ///< SMED5 Parameter 2 S1 register
    #define _SMED5_PRM_S20    _SFR(uint8_t,  SMED5_AddressBase+0x15)     ///< SMED5 Parameter 0 S2 register
    #define _SMED5_PRM_S21    _SFR(uint8_t,  SMED5_AddressBase+0x16)     ///< SMED5 Parameter 1 S2 register
    #define _SMED5_PRM_S22    _SFR(uint8_t,  SMED5_AddressBase+0x17)     ///< SMED5 Parameter 2 S2 register
    #define _SMED5_PRM_S30    _SFR(uint8_t,  SMED5_AddressBase+0x18)     ///< SMED5 Parameter 0 S3 register
    #define _SMED5_PRM_S31    _SFR(uint8_t,  SMED5_AddressBase+0x19)     ///< SMED5 Parameter 1 S3 register
    #define _SMED5_PRM_S32    _SFR(uint8_t,  SMED5_AddressBase+0x1A)     ///< SMED5 Parameter 2 S3 register
    #define _SMED5_CFG        _SFR(uint8_t,  SMED5_AddressBase+0x1B)     ///< SMED5 Timer configuration register
    #define _SMED5_DMP_L      _SFR(uint8_t,  SMED5_AddressBase+0x1C)     ///< SMED5 Dump counter LSB register
    #define _SMED5_DMP_H      _SFR(uint8_t,  SMED5_AddressBase+0x1D)     ///< SMED5 Dump counter MSB register
    #define _SMED5_GSTS       _SFR(uint8_t,  SMED5_AddressBase+0x1E)     ///< SMED5 General status register
    #define _SMED5_IRQ        _SFR(uint8_t,  SMED5_AddressBase+0x1F)     ///< SMED5 Interrupt regquest register
    #define _SMED5_IER        _SFR(uint8_t,  SMED5_AddressBase+0x20)     ///< SMED5 Interrupt enable register
    #define _SMED5_ISEL       _SFR(uint8_t,  SMED5_AddressBase+0x21)     ///< SMED5 Externalevent control register
    #define _SMED5_DMP        _SFR(uint8_t,  SMED5_AddressBase+0x22)     ///< SMED5 Dump enable register
    #define _SMED5_FSM_STS    _SFR(uint8_t,  SMED5_AddressBase+0x23)     ///< SMED5 finite state machine status register
  #endif // SMED5_AddressBase


  /* SMEDx Module Reset Values */
  #define _SMED_RESET_VALUE                  ((uint8_t)0x00)          ///< SMEDn struct/bit access reset value
  #define _SMED_CTR_RESET_VALUE              ((uint8_t)0x00)          ///< SMEDn Control register reset value
  #define _SMED_CTR_TMR_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Control timer register reset value
  #define _SMED_CTR_INP_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Control input register reset value
  #define _SMED_CTR_DTR_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Dithering register reset value
  #define _SMED_TMR_T0L_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Time T0 LSB register reset value
  #define _SMED_TMR_T0H_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Time T0 MSB register reset value
  #define _SMED_TMR_T1L_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Time T1 LSB register reset value
  #define _SMED_TMR_T1H_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Time T1 MSB register reset value
  #define _SMED_TMR_T2L_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Time T2 LSB register reset value
  #define _SMED_TMR_T2H_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Time T2 MSB register reset value
  #define _SMED_TMR_T3L_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Time T3 LSB register reset value
  #define _SMED_TMR_T3H_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Time T3 MSB register reset value
  #define _SMED_PRM_ID0_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Parameter 0 IDLE register reset value
  #define _SMED_PRM_ID1_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Parameter 1 IDLE register reset value
  #define _SMED_PRM_ID2_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Parameter 2 IDLE register reset value
  #define _SMED_PRM_S00_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Parameter 0 S0 register reset value
  #define _SMED_PRM_S01_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Parameter 1 S0 register reset value
  #define _SMED_PRM_S02_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Parameter 2 S0 register reset value
  #define _SMED_PRM_S10_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Parameter 0 S1 register reset value
  #define _SMED_PRM_S11_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Parameter 1 S1 register reset value
  #define _SMED_PRM_S12_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Parameter 2 S1 register reset value
  #define _SMED_PRM_S20_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Parameter 0 S2 register reset value
  #define _SMED_PRM_S21_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Parameter 1 S2 register reset value
  #define _SMED_PRM_S22_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Parameter 2 S2 register reset value
  #define _SMED_PRM_S30_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Parameter 0 S3 register reset value
  #define _SMED_PRM_S31_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Parameter 1 S3 register reset value
  #define _SMED_PRM_S32_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn Parameter 2 S3 register reset value
  #define _SMED_CFG_RESET_VALUE              ((uint8_t)0x00)          ///< SMEDn Timer configuration register reset value
  #define _SMED_DMP_L_RESET_VALUE            ((uint8_t)0x00)          ///< SMEDn Dump counter LSB register reset value
  #define _SMED_DMP_H_RESET_VALUE            ((uint8_t)0x00)          ///< SMEDn Dump counter MSB register reset value
  #define _SMED_GSTS_RESET_VALUE             ((uint8_t)0x00)          ///< SMEDn General status register reset value
  #define _SMED_IRQ_RESET_VALUE              ((uint8_t)0x00)          ///< SMEDn Interrupt regquest register reset value
  #define _SMED_IER_RESET_VALUE              ((uint8_t)0x00)          ///< SMEDn Interrupt enable register reset value
  #define _SMED_ISEL_RESET_VALUE             ((uint8_t)0x00)          ///< SMEDn Externalevent control register reset value
  #define _SMED_DMP_RESET_VALUE              ((uint8_t)0x00)          ///< SMEDn Dump enable register reset value
  #define _SMED_FSM_STS_RESET_VALUE          ((uint8_t)0x00)          ///< SMEDn FSM status register reset value


  /* SMEDn Control register (_SMEDn_CTR) */
  #define _SMED_CTR_START_CNT          ((uint8_t) (0x01 << 0))   ///< start counter [0]
  #define _SMED_CTR_FSM_ENA            ((uint8_t) (0x01 << 1))   ///< synchronous FSM enable [0]
  // reserved [7:2]

  /* SMEDn Control timer register (_SMEDn_CTR_TMR) */
  #define _SMED_CTR_TMR_TIME_T0_VAL    ((uint8_t) (0x01 << 0))   ///< validation of the TMR_T0 register [0]
  #define _SMED_CTR_TMR_TIME_T1_VAL    ((uint8_t) (0x01 << 1))   ///< validation of the TMR_T1 register [0]
  #define _SMED_CTR_TMR_TIME_T2_VAL    ((uint8_t) (0x01 << 2))   ///< validation of the TMR_T2 register [0]
  #define _SMED_CTR_TMR_TIME_T3_VAL    ((uint8_t) (0x01 << 3))   ///< validation of the TMR_T3 register [0]
  #define _SMED_CTR_TMR_DITHER_VAL     ((uint8_t) (0x01 << 4))   ///< validation of the CTR_DTHR register [0]
  // reserved [7:5]

  /* SMEDn Control input register (_SMEDn_CTR_INP) */
  #define _SMED_CTR_INP_RS_INSIG       ((uint8_t) (0x07 << 0))   ///< external event input polarity level definition [2:0]
  #define _SMED_CTR_INP_RS_INSIG0      ((uint8_t) (0x01 << 0))   ///< external event input polarity level definition [0]
  #define _SMED_CTR_INP_RS_INSIG1      ((uint8_t) (0x01 << 1))   ///< external event input polarity level definition [1]
  #define _SMED_CTR_INP_RS_INSIG2      ((uint8_t) (0x01 << 2))   ///< external event input polarity level definition [2]
  #define _SMED_CTR_INP_RAIS_EN        ((uint8_t) (0x01 << 3))   ///< Asy_Enable input polarity level definition [0]
  #define _SMED_CTR_INP_EL_INSIG       ((uint8_t) (0x07 << 4))   ///< external InSig input characteristic definition [2:0]
  #define _SMED_CTR_INP_EL_INSIG0      ((uint8_t) (0x01 << 4))   ///< external InSig input characteristic definition [0]
  #define _SMED_CTR_INP_EL_INSIG1      ((uint8_t) (0x01 << 5))   ///< external InSig input characteristic definition [1]
  #define _SMED_CTR_INP_EL_INSIG2      ((uint8_t) (0x01 << 6))   ///< external InSig input characteristic definition [2]
  #define _SMED_CTR_INP_EL_EN          ((uint8_t) (0x01 << 7))   ///< Asy_Enable input characteristic definition [0]
  // reserved [7:5]

  /* SMEDn Parameter 0 registers (_SMEDn_PRM_ID0, SMEDn_PRM_Sx0) */
  #define _SMED_PRM_ID0_NX_STAT        ((uint8_t) (0x03 << 0))   ///< next event controlled state definition [1:0] (in _SMEDn_PRM_ID0, _SMEDn_PRM_Sx0)
  #define _SMED_PRM_ID0_NX_STAT0       ((uint8_t) (0x01 << 0))   ///< next event controlled state definition [0] (in _SMEDn_PRM_ID0, _SMEDn_PRM_Sx0)
  #define _SMED_PRM_ID0_NX_STAT1       ((uint8_t) (0x01 << 1))   ///< next event controlled state definition [1] (in _SMEDn_PRM_ID0, _SMEDn_PRM_Sx0)
  #define _SMED_PRM_ID0_EDGE           ((uint8_t) (0x03 << 2))   ///< external edge jumping selection [1:0] (in _SMEDn_PRM_ID0, _SMEDn_PRM_Sx0)
  #define _SMED_PRM_ID0_EDGE0          ((uint8_t) (0x01 << 2))   ///< external edge jumping selection [0] (in _SMEDn_PRM_ID0, _SMEDn_PRM_Sx0)
  #define _SMED_PRM_ID0_EDGE1          ((uint8_t) (0x01 << 3))   ///< external edge jumping selection [1] (in _SMEDn_PRM_ID0, _SMEDn_PRM_Sx0)
  #define _SMED_PRM_ID0_CNT_RSTE       ((uint8_t) (0x01 << 4))   ///< counter reset definition [1] (in _SMEDn_PRM_ID0, _SMEDn_PRM_Sx0)
  #define _SMED_PRM_ID0_PULS_EDG       ((uint8_t) (0x01 << 5))   ///< PWM pulse out level definition [0] (in _SMEDn_PRM_ID0, _SMEDn_PRM_Sx0)
  #define _SMED_PRM_ID0_HOLD_JMP       ((uint8_t) (0x01 << 6))   ///< hold state definition [0] (in _SMEDn_PRM_ID0, _SMEDn_PRM_Sx0)
  #define _SMED_PRM_ID0_AND_OR         ((uint8_t) (0x01 << 7))   ///< jumping Boolean function definition [0] (in _SMEDn_PRM_ID0, _SMEDn_PRM_Sx0)

  /* SMEDn Parameter 1 registers (_SMEDn_PRM_ID1, SMEDn_PRM_Sx1) */
  // reserved [1:0]
  #define _SMED_PRM_ID1_CEDGE          ((uint8_t) (0x03 << 2))   ///< validation of the TMR_T0 register [1:0] (in _SMEDn_PRM_ID1, _SMEDn_PRM_Sx1)
  #define _SMED_PRM_ID1_CEDGE0         ((uint8_t) (0x01 << 2))   ///< validation of the TMR_T0 register [0] (in _SMEDn_PRM_ID1, _SMEDn_PRM_Sx1)
  #define _SMED_PRM_ID1_CEDGE1         ((uint8_t) (0x01 << 3))   ///< validation of the TMR_T0 register [1] (in _SMEDn_PRM_ID1, _SMEDn_PRM_Sx1)
  #define _SMED_PRM_ID1_CNT_RSTC       ((uint8_t) (0x01 << 4))   ///< validation of the TMR_T1 register [0] (in _SMEDn_PRM_ID1, _SMEDn_PRM_Sx1)
  #define _SMED_PRM_ID1_PULS_CMP       ((uint8_t) (0x01 << 5))   ///< validation of the TMR_T2 register [0] (in _SMEDn_PRM_ID1, _SMEDn_PRM_Sx1)
  #define _SMED_PRM_ID1_HOLD_EXIT      ((uint8_t) (0x01 << 6))   ///< validation of the TMR_T3 register [0] (in _SMEDn_PRM_ID1, _SMEDn_PRM_Sx1)
  // reserved [7]

  /* SMEDn Parameter 2 registers (_SMEDn_PRM_ID2, SMEDn_PRM_Sx2) */
  #define _SMED_PRM_ID2_LATCH_RS       ((uint8_t) (0x01 << 0))   ///< latch reset definition [0] (in _SMEDn_PRM_ID2, _SMEDn_PRM_Sx2)
  // reserved [6:1]
  #define _SMED_PRM_ID2_QCOUP_ST       ((uint8_t) (0x01 << 7))   ///< idle quad couple start mode configuration [0] (in _SMEDn_PRM_ID2)

  /* SMEDn Timer configuration register (_SMEDn_CFG) */
  #define _SMED_CFG_TIM_NUM            ((uint8_t) (0x03 << 0))   ///< time registers to be temporary incremented selection [1:0]
  #define _SMED_CFG_TIM_NUM0           ((uint8_t) (0x01 << 0))   ///< time registers to be temporary incremented selection [0]
  #define _SMED_CFG_TIM_NUM1           ((uint8_t) (0x01 << 1))   ///< time registers to be temporary incremented selection [1]
  #define _SMED_CFG_TIM_UPD            ((uint8_t) (0x03 << 2))   ///< time registers update mode [1:0]
  #define _SMED_CFG_TIM_UPD0           ((uint8_t) (0x01 << 2))   ///< time registers update mode [0]
  #define _SMED_CFG_TIM_UPD1           ((uint8_t) (0x01 << 3))   ///< time registers update mode [1]
  // reserved [7:4]

  /* SMEDn General status register (_SMEDn_GSTS) */
  #define _SMED_GSTS_EX0_DUMP          ((uint8_t) (0x01 << 0))   ///< dumping cause flag InSig[0] [0]
  #define _SMED_GSTS_EX1_DUMP          ((uint8_t) (0x01 << 1))   ///< dumping cause flag InSig[1] [0]
  #define _SMED_GSTS_EX2_DUMP          ((uint8_t) (0x01 << 2))   ///< dumping cause flag InSig[2] [0]
  #define _SMED_GSTS_CNT_FLAG          ((uint8_t) (0x01 << 3))   ///< counter reset flag when dump is enabled [0]
  #define _SMED_GSTS_DMP_LK            ((uint8_t) (0x03 << 4))   ///< counter dump status [1:0]
  #define _SMED_GSTS_DMP_LK0           ((uint8_t) (0x01 << 4))   ///< counter dump status [0]
  #define _SMED_GSTS_DMP_LK1           ((uint8_t) (0x01 << 5))   ///< counter dump status [1]
  #define _SMED_GSTS_EVENT_OV          ((uint8_t) (0x01 << 6))   ///< event overflow flag [0]
  // reserved [7]

  /* SMEDn Interrupt request register (_SMEDn_IRQ) */
  #define _SMED_IRQ_CNT_OVER           ((uint8_t) (0x01 << 0))   ///< counter overflow interrupt request [0]
  #define _SMED_IRQ_EXT0_INT           ((uint8_t) (0x01 << 1))   ///< InSig[0] capture interrupt request [0]
  #define _SMED_IRQ_EXT1_INT           ((uint8_t) (0x01 << 2))   ///< InSig[1] capture interrupt request [0]
  #define _SMED_IRQ_EXT2_INT           ((uint8_t) (0x01 << 3))   ///< InSig[2] capture interrupt request [0]
  #define _SMED_IRQ_STA_S0_IT          ((uint8_t) (0x01 << 4))   ///< Tmr(0) transition interrupt request [0]
  #define _SMED_IRQ_STA_S1_IT          ((uint8_t) (0x01 << 5))   ///< Tmr(1) transition interrupt request [0]
  #define _SMED_IRQ_STA_S2_IT          ((uint8_t) (0x01 << 6))   ///< Tmr(2) transition interrupt request [0]
  #define _SMED_IRQ_STA_S3_IT          ((uint8_t) (0x01 << 7))   ///< Tmr(3) transition interrupt request [0]

  /* SMEDn Interrupt enable register (_SMEDn_IER) */
  #define _SMED_IER_CNT_OV_E           ((uint8_t) (0x01 << 0))   ///< counter overflow interrupt enable [0]
  #define _SMED_IER_IT_EN_EX0          ((uint8_t) (0x01 << 1))   ///< InSig[0] capture interrupt enable [0]
  #define _SMED_IER_IT_EN_EX1          ((uint8_t) (0x01 << 2))   ///< InSig[1] capture interrupt enable [0]
  #define _SMED_IER_IT_EN_EX2          ((uint8_t) (0x01 << 3))   ///< InSig[2] capture interrupt enable [0]
  #define _SMED_IER_IT_STA_S0          ((uint8_t) (0x01 << 4))   ///< Tmr(0) transition interrupt enable [0]
  #define _SMED_IER_IT_STA_S1          ((uint8_t) (0x01 << 5))   ///< Tmr(1) transition interrupt enable [0]
  #define _SMED_IER_IT_STA_S2          ((uint8_t) (0x01 << 6))   ///< Tmr(2) transition interrupt enable [0]
  #define _SMED_IER_IT_STA_S3          ((uint8_t) (0x01 << 7))   ///< Tmr(3) transition interrupt enable [0]

  /* SMEDn External event control register (_SMEDn_ISEL) */
  #define _SMED_ISEL_INPUT0_EN         ((uint8_t) (0x01 << 0))   ///< InSig[0] input enable [0]
  #define _SMED_ISEL_INPUT1_EN         ((uint8_t) (0x01 << 1))   ///< InSig[1] input enable [0]
  #define _SMED_ISEL_INPUT2_EN         ((uint8_t) (0x01 << 2))   ///< InSig[2] input enable [0]
  #define _SMED_ISEL_INPUT_LAT         ((uint8_t) (0x01 << 3))   ///< enable latch function on InSig[0] input [0]
  // reserved [7:4]

  /* SMEDn Dump enable register (_SMEDn_DMP) */
  #define _SMED_DMP_DMPE_EX0           ((uint8_t) (0x01 << 0))   ///< dump update mode for InSig[0] event [0]
  #define _SMED_DMP_DMPE_EX1           ((uint8_t) (0x01 << 1))   ///< dump update mode for InSig[1] event [0]
  #define _SMED_DMP_DMPE_EX2           ((uint8_t) (0x01 << 2))   ///< dump update mode for InSig[2] event [0]
  #define _SMED_DMP_DMP_EVER           ((uint8_t) (0x01 << 3))   ///< dump update mode all [0]
  #define _SMED_DMP_CPL_IT_GE          ((uint8_t) (0x01 << 4))   ///< lock together SMEDn_GSTS and SMEDn_IRQ reset signal [0]
  // reserved [7:5]

  /* SMEDn finite state machine status register (_SMEDn_FSM_STS) */
  #define _SMED_FSM_STS_FSM            ((uint8_t) (0x07 << 0))   ///< finite state machine state variable [2:0]
  #define _SMED_FSM_STS_FSM0           ((uint8_t) (0x01 << 0))   ///< finite state machine state variable [0]
  #define _SMED_FSM_STS_FSM1           ((uint8_t) (0x01 << 1))   ///< finite state machine state variable [1]
  #define _SMED_FSM_STS_FSM2           ((uint8_t) (0x01 << 2))   ///< finite state machine state variable [2]
  #define _SMED_FSM_STS_PWM            ((uint8_t) (0x01 << 3))   ///< PWM output signal [0]
  #define _SMED_FSM_STS_EVINP          ((uint8_t) (0x07 << 4))   ///< event input signals [2:0]
  #define _SMED_FSM_STS_EVINP0         ((uint8_t) (0x01 << 4))   ///< event input signals [0]
  #define _SMED_FSM_STS_EVINP1         ((uint8_t) (0x01 << 5))   ///< event input signals [1]
  #define _SMED_FSM_STS_EVINP2         ((uint8_t) (0x01 << 6))   ///< event input signals [2]
  // reserved [7]

#endif // SMEDn_AddressBase



//------------------------
// Global Configuration (_CFG)
//------------------------
#if defined(_DOXYGEN) || defined(CFG_AddressBase)

  /** @brief struct for Global Configuration registers (_CFG) */
  typedef struct {

    /** @brief Global configuration register (_CFG_GCR) */
    struct {
      _BITS   SWD     : 1;    ///< SWIM disable
      _BITS   AL      : 1;    ///< Activation level
      _BITS           : 5;    //   Reserved
      _BITS   HSIT    : 1;    ///< High-speed oscillator trimmed
    } GCR;

  } _CFG_t;

  /* Pointer to CFG registers */
  #define _CFG          _SFR(_CFG_t,   CFG_AddressBase)          ///< CFG struct/bit access
  #define _CFG_GCR      _SFR(uint8_t,  CFG_AddressBase+0x00)     ///< Global configuration register


  /* CFG Module Reset Values */
  #define _CFG_GCR_RESET_VALUE         ((uint8_t)0x00)           ///< Global configuration register reset value


  /* Global configuration register (_CFG_GCR) */
  #define _CFG_GCR_SWD                 ((uint8_t) (0x01 << 0))   ///< SWIM disable [0]
  #define _CFG_GCR_AL                  ((uint8_t) (0x01 << 1))   ///< Activation level [0]
  // reserved [6:2]
  #define _CFG_GCR_HSIT                ((uint8_t) (0x01 << 1))   ///< High-speed oscillator trimmed [0]

#endif // CFG_AddressBase



//------------------------
// Device Identifier (DEVID). Only bytewise access
//------------------------
#if defined(_DOXYGEN) || defined(DEVID_AddressBase)

  /* Pointer to DEVID array */
  #define _DEVID0       _SFR(uint8_t,  DEVID_AddressBase+0x00)   ///< Device Identifier byte 0
  #define _NDEVID0      _SFR(uint8_t,  DEVID_AddressBase+0x01)   ///< Complementary Device Identifier byte 0
  #define _DEVID1       _SFR(uint8_t,  DEVID_AddressBase+0x02)   ///< Device Identifier byte 1
  #define _NDEVID1      _SFR(uint8_t,  DEVID_AddressBase+0x03)   ///< ComplementaryDevice Identifier byte 1

#endif // DEVID_AddressBase



//------------------------
// Interrupt Priority Module (_ITC)
//------------------------
#if defined(_DOXYGEN) || defined(ITC_AddressBase)

  /** @brief struct for setting interrupt Priority (_ITC) */
  typedef struct {

    /** @brief interrupt priority register 1 (_ITC_SPR1) */
    struct {
      _BITS             : 2;    //   Reserved (TLI always highest prio)
      _BITS   VECT1SPR  : 2;    ///< interrupt priority vector 1
      _BITS   VECT2SPR  : 2;    ///< interrupt priority vector 2
      _BITS   VECT3SPR  : 2;    ///< interrupt priority vector 3
    } SPR1;


    /** @brief interrupt priority register 2 (_ITC_SPR2) */
    struct {
      _BITS   VECT4SPR  : 2;    ///< interrupt priority vector 4
      _BITS   VECT5SPR  : 2;    ///< interrupt priority vector 5
      _BITS   VECT6SPR  : 2;    ///< interrupt priority vector 6
      _BITS   VECT7SPR  : 2;    ///< interrupt priority vector 7
    } SPR2;


    /** @brief interrupt priority register 3 (_ITC_SPR3) */
    struct {
      _BITS   VECT8SPR  : 2;    ///< interrupt priority vector 8
      _BITS   VECT9SPR  : 2;    ///< interrupt priority vector 9
      _BITS   VECT10SPR : 2;    ///< interrupt priority vector 10
      _BITS   VECT11SPR : 2;    ///< interrupt priority vector 11
    } SPR3;


    /** @brief interrupt priority register 4 (_ITC_SPR4) */
    struct {
      _BITS   VECT12SPR : 2;    ///< interrupt priority vector 12
      _BITS   VECT13SPR : 2;    ///< interrupt priority vector 13
      _BITS   VECT14SPR : 2;    ///< interrupt priority vector 14
      _BITS   VECT15SPR : 2;    ///< interrupt priority vector 15
    } SPR4;


    /** @brief interrupt priority register 5 (_ITC_SPR5) */
    struct {
      _BITS   VECT16SPR : 2;    ///< interrupt priority vector 16
      _BITS   VECT17SPR : 2;    ///< interrupt priority vector 17
      _BITS   VECT18SPR : 2;    ///< interrupt priority vector 18
      _BITS   VECT19SPR : 2;    ///< interrupt priority vector 19
    } SPR5;


    /** @brief interrupt priority register 6 (_ITC_SPR6) */
    struct {
      _BITS   VECT20SPR : 2;    ///< interrupt priority vector 20
      _BITS   VECT21SPR : 2;    ///< interrupt priority vector 21
      _BITS   VECT22SPR : 2;    ///< interrupt priority vector 22
      _BITS   VECT23SPR : 2;    ///< interrupt priority vector 23
    } SPR6;


    /** @brief interrupt priority register 7 (_ITC_SPR7) */
    struct {
      _BITS   VECT24SPR : 2;    ///< interrupt priority vector 24
      _BITS   VECT25SPR : 2;    ///< interrupt priority vector 25
      _BITS   VECT26SPR : 2;    ///< interrupt priority vector 26
      _BITS   VECT27SPR : 2;    ///< interrupt priority vector 27
    } SPR7;


    /** @brief interrupt priority register 8 (_ITC_SPR8) */
    struct {
      _BITS   VECT28SPR : 2;    ///< interrupt priority vector 28
      _BITS   VECT29SPR : 2;    ///< interrupt priority vector 29
      _BITS   VECT30SPR : 2;    ///< interrupt priority vector 30
      _BITS   VECT31SPR : 2;    ///< interrupt priority vector 31
    } SPR8;

  } _ITC_t;

  /* Pointer to ITC registers */
  #define _ITC          _SFR(_ITC_t,   ITC_AddressBase)          ///< ITC struct/bit access
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


  /* Software priority register 1 (_ITC_SPR1) */
  // reserved [1:0]
  #define _ITC_SPR1_VECT1SPR           ((uint8_t) (0x03 << 2))   ///< ITC interrupt priority vector 1 [1:0]
  #define _ITC_SPR1_VECT1SPR0          ((uint8_t) (0x01 << 2))   ///< ITC interrupt priority vector 1 [0]
  #define _ITC_SPR1_VECT1SPR1          ((uint8_t) (0x01 << 3))   ///< ITC interrupt priority vector 1 [1]
  #define _ITC_SPR1_VECT2SPR           ((uint8_t) (0x03 << 4))   ///< ITC interrupt priority vector 2 [1:0]
  #define _ITC_SPR1_VECT2SPR0          ((uint8_t) (0x01 << 4))   ///< ITC interrupt priority vector 2 [0]
  #define _ITC_SPR1_VECT2SPR1          ((uint8_t) (0x01 << 5))   ///< ITC interrupt priority vector 2 [1]
  #define _ITC_SPR1_VECT3SPR           ((uint8_t) (0x03 << 6))   ///< ITC interrupt priority vector 3 [1:0]
  #define _ITC_SPR1_VECT3SPR0          ((uint8_t) (0x01 << 6))   ///< ITC interrupt priority vector 3 [0]
  #define _ITC_SPR1_VECT3SPR1          ((uint8_t) (0x01 << 7))   ///< ITC interrupt priority vector 3 [1]

  /* Software priority register 2 (_ITC_SPR2) */
  #define _ITC_SPR2_VECT4SPR           ((uint8_t) (0x03 << 0))   ///< ITC interrupt priority vector 4 [1:0]
  #define _ITC_SPR2_VECT4SPR0          ((uint8_t) (0x01 << 0))   ///< ITC interrupt priority vector 4 [0]
  #define _ITC_SPR2_VECT4SPR1          ((uint8_t) (0x01 << 1))   ///< ITC interrupt priority vector 4 [1]
  #define _ITC_SPR2_VECT5SPR           ((uint8_t) (0x03 << 2))   ///< ITC interrupt priority vector 5 [1:0]
  #define _ITC_SPR2_VECT5SPR0          ((uint8_t) (0x01 << 2))   ///< ITC interrupt priority vector 5 [0]
  #define _ITC_SPR2_VECT5SPR1          ((uint8_t) (0x01 << 3))   ///< ITC interrupt priority vector 5 [1]
  #define _ITC_SPR2_VECT6SPR           ((uint8_t) (0x03 << 4))   ///< ITC interrupt priority vector 6 [1:0]
  #define _ITC_SPR2_VECT6SPR0          ((uint8_t) (0x01 << 4))   ///< ITC interrupt priority vector 6 [0]
  #define _ITC_SPR2_VECT6SPR1          ((uint8_t) (0x01 << 5))   ///< ITC interrupt priority vector 6 [1]
  #define _ITC_SPR2_VECT7SPR           ((uint8_t) (0x03 << 6))   ///< ITC interrupt priority vector 7 [1:0]
  #define _ITC_SPR2_VECT7SPR0          ((uint8_t) (0x01 << 6))   ///< ITC interrupt priority vector 7 [0]
  #define _ITC_SPR2_VECT7SPR1          ((uint8_t) (0x01 << 7))   ///< ITC interrupt priority vector 7 [1]

  /* Software priority register 3 (_ITC_SPR3) */
  #define _ITC_SPR3_VECT8SPR           ((uint8_t) (0x03 << 0))   ///< ITC interrupt priority vector 8 [1:0]
  #define _ITC_SPR3_VECT8SPR0          ((uint8_t) (0x01 << 0))   ///< ITC interrupt priority vector 8 [0]
  #define _ITC_SPR3_VECT8SPR1          ((uint8_t) (0x01 << 1))   ///< ITC interrupt priority vector 8 [1]
  #define _ITC_SPR3_VECT9SPR           ((uint8_t) (0x03 << 2))   ///< ITC interrupt priority vector 9 [1:0]
  #define _ITC_SPR3_VECT9SPR0          ((uint8_t) (0x01 << 2))   ///< ITC interrupt priority vector 9 [0]
  #define _ITC_SPR3_VECT9SPR1          ((uint8_t) (0x01 << 3))   ///< ITC interrupt priority vector 9 [1]
  #define _ITC_SPR3_VECT10SPR          ((uint8_t) (0x03 << 4))   ///< ITC interrupt priority vector 10 [1:0]
  #define _ITC_SPR3_VECT10SPR0         ((uint8_t) (0x01 << 4))   ///< ITC interrupt priority vector 10 [0]
  #define _ITC_SPR3_VECT10SPR1         ((uint8_t) (0x01 << 5))   ///< ITC interrupt priority vector 10 [1]
  #define _ITC_SPR3_VECT11SPR          ((uint8_t) (0x03 << 6))   ///< ITC interrupt priority vector 11 [1:0]
  #define _ITC_SPR3_VECT11SPR0         ((uint8_t) (0x01 << 6))   ///< ITC interrupt priority vector 11 [0]
  #define _ITC_SPR3_VECT11SPR1         ((uint8_t) (0x01 << 7))   ///< ITC interrupt priority vector 11 [1]

  /* Software priority register 4 (_ITC_SPR4) */
  #define _ITC_SPR4_VECT12SPR          ((uint8_t) (0x03 << 0))   ///< ITC interrupt priority vector 12 [1:0]
  #define _ITC_SPR4_VECT12SPR0         ((uint8_t) (0x01 << 0))   ///< ITC interrupt priority vector 12 [0]
  #define _ITC_SPR4_VECT12SPR1         ((uint8_t) (0x01 << 1))   ///< ITC interrupt priority vector 12 [1]
  #define _ITC_SPR4_VECT13SPR          ((uint8_t) (0x03 << 2))   ///< ITC interrupt priority vector 13 [1:0]
  #define _ITC_SPR4_VECT13SPR0         ((uint8_t) (0x01 << 2))   ///< ITC interrupt priority vector 13 [0]
  #define _ITC_SPR4_VECT13SPR1         ((uint8_t) (0x01 << 3))   ///< ITC interrupt priority vector 13 [1]
  #define _ITC_SPR4_VECT14SPR          ((uint8_t) (0x03 << 4))   ///< ITC interrupt priority vector 14 [1:0]
  #define _ITC_SPR4_VECT14SPR0         ((uint8_t) (0x01 << 4))   ///< ITC interrupt priority vector 14 [0]
  #define _ITC_SPR4_VECT14SPR1         ((uint8_t) (0x01 << 5))   ///< ITC interrupt priority vector 14 [1]
  #define _ITC_SPR4_VECT15SPR          ((uint8_t) (0x03 << 6))   ///< ITC interrupt priority vector 15 [1:0]
  #define _ITC_SPR4_VECT15SPR0         ((uint8_t) (0x01 << 6))   ///< ITC interrupt priority vector 15 [0]
  #define _ITC_SPR4_VECT15SPR1         ((uint8_t) (0x01 << 7))   ///< ITC interrupt priority vector 15 [1]

  /* Software priority register 5 (_ITC_SPR5) */
  #define _ITC_SPR5_VECT16SPR          ((uint8_t) (0x03 << 0))   ///< ITC interrupt priority vector 16 [1:0]
  #define _ITC_SPR5_VECT16SPR0         ((uint8_t) (0x01 << 0))   ///< ITC interrupt priority vector 16 [0]
  #define _ITC_SPR5_VECT16SPR1         ((uint8_t) (0x01 << 1))   ///< ITC interrupt priority vector 16 [1]
  #define _ITC_SPR5_VECT17SPR          ((uint8_t) (0x03 << 2))   ///< ITC interrupt priority vector 17 [1:0]
  #define _ITC_SPR5_VECT17SPR0         ((uint8_t) (0x01 << 2))   ///< ITC interrupt priority vector 17 [0]
  #define _ITC_SPR5_VECT17SPR1         ((uint8_t) (0x01 << 3))   ///< ITC interrupt priority vector 17 [1]
  #define _ITC_SPR5_VECT18SPR          ((uint8_t) (0x03 << 4))   ///< ITC interrupt priority vector 18 [1:0]
  #define _ITC_SPR5_VECT18SPR0         ((uint8_t) (0x01 << 4))   ///< ITC interrupt priority vector 18 [0]
  #define _ITC_SPR5_VECT18SPR1         ((uint8_t) (0x01 << 5))   ///< ITC interrupt priority vector 18 [1]
  #define _ITC_SPR5_VECT19SPR          ((uint8_t) (0x03 << 6))   ///< ITC interrupt priority vector 19 [1:0]
  #define _ITC_SPR5_VECT19SPR0         ((uint8_t) (0x01 << 6))   ///< ITC interrupt priority vector 19 [0]
  #define _ITC_SPR5_VECT19SPR1         ((uint8_t) (0x01 << 7))   ///< ITC interrupt priority vector 19 [1]

  /* Software priority register 6 (_ITC_SPR6) */
  #define _ITC_SPR6_VECT20SPR          ((uint8_t) (0x03 << 0))   ///< ITC interrupt priority vector 20 [1:0]
  #define _ITC_SPR6_VECT20SPR0         ((uint8_t) (0x01 << 0))   ///< ITC interrupt priority vector 20 [0]
  #define _ITC_SPR6_VECT20SPR1         ((uint8_t) (0x01 << 1))   ///< ITC interrupt priority vector 20 [1]
  #define _ITC_SPR6_VECT21SPR          ((uint8_t) (0x03 << 2))   ///< ITC interrupt priority vector 21 [1:0]
  #define _ITC_SPR6_VECT21SPR0         ((uint8_t) (0x01 << 2))   ///< ITC interrupt priority vector 21 [0]
  #define _ITC_SPR6_VECT21SPR1         ((uint8_t) (0x01 << 3))   ///< ITC interrupt priority vector 21 [1]
  #define _ITC_SPR6_VECT22SPR          ((uint8_t) (0x03 << 4))   ///< ITC interrupt priority vector 22 [1:0]
  #define _ITC_SPR6_VECT22SPR0         ((uint8_t) (0x01 << 4))   ///< ITC interrupt priority vector 22 [0]
  #define _ITC_SPR6_VECT22SPR1         ((uint8_t) (0x01 << 5))   ///< ITC interrupt priority vector 22 [1]
  #define _ITC_SPR6_VECT23SPR          ((uint8_t) (0x03 << 6))   ///< ITC interrupt priority vector 23 [1:0]
  #define _ITC_SPR6_VECT23SPR0         ((uint8_t) (0x01 << 6))   ///< ITC interrupt priority vector 23 [0]
  #define _ITC_SPR6_VECT23SPR1         ((uint8_t) (0x01 << 7))   ///< ITC interrupt priority vector 23 [1]

  /* Software priority register 7 (_ITC_SPR7) */
  #define _ITC_SPR7_VECT24SPR          ((uint8_t) (0x03 << 0))   ///< ITC interrupt priority vector 24 [1:0]
  #define _ITC_SPR7_VECT24SPR0         ((uint8_t) (0x01 << 0))   ///< ITC interrupt priority vector 24 [0]
  #define _ITC_SPR7_VECT24SPR1         ((uint8_t) (0x01 << 1))   ///< ITC interrupt priority vector 24 [1]
  #define _ITC_SPR7_VECT25SPR          ((uint8_t) (0x03 << 2))   ///< ITC interrupt priority vector 25 [1:0]
  #define _ITC_SPR7_VECT25SPR0         ((uint8_t) (0x01 << 2))   ///< ITC interrupt priority vector 25 [0]
  #define _ITC_SPR7_VECT25SPR1         ((uint8_t) (0x01 << 3))   ///< ITC interrupt priority vector 25 [1]
  #define _ITC_SPR7_VECT26SPR          ((uint8_t) (0x03 << 4))   ///< ITC interrupt priority vector 26 [1:0]
  #define _ITC_SPR7_VECT26SPR0         ((uint8_t) (0x01 << 4))   ///< ITC interrupt priority vector 26 [0]
  #define _ITC_SPR7_VECT26SPR1         ((uint8_t) (0x01 << 5))   ///< ITC interrupt priority vector 26 [1]
  #define _ITC_SPR7_VECT27SPR          ((uint8_t) (0x03 << 6))   ///< ITC interrupt priority vector 27 [1:0]
  #define _ITC_SPR7_VECT27SPR0         ((uint8_t) (0x01 << 6))   ///< ITC interrupt priority vector 27 [0]
  #define _ITC_SPR7_VECT27SPR1         ((uint8_t) (0x01 << 7))   ///< ITC interrupt priority vector 27 [1]

  /* Software priority register 8 (_ITC_SPR8) */
  #define _ITC_SPR8_VECT28SPR          ((uint8_t) (0x03 << 0))   ///< ITC interrupt priority vector 28 [1:0]
  #define _ITC_SPR8_VECT28SPR0         ((uint8_t) (0x01 << 0))   ///< ITC interrupt priority vector 28 [0]
  #define _ITC_SPR8_VECT28SPR1         ((uint8_t) (0x01 << 1))   ///< ITC interrupt priority vector 28 [1]
  #define _ITC_SPR8_VECT29SPR          ((uint8_t) (0x03 << 2))   ///< ITC interrupt priority vector 29 [1:0]
  #define _ITC_SPR8_VECT29SPR0         ((uint8_t) (0x01 << 2))   ///< ITC interrupt priority vector 29 [0]
  #define _ITC_SPR8_VECT29SPR1         ((uint8_t) (0x01 << 3))   ///< ITC interrupt priority vector 29 [1]
  #define _ITC_SPR8_VECT30SPR          ((uint8_t) (0x03 << 4))   ///< ITC interrupt priority vector 30 [1:0]
  #define _ITC_SPR8_VECT30SPR0         ((uint8_t) (0x01 << 4))   ///< ITC interrupt priority vector 30 [0]
  #define _ITC_SPR8_VECT30SPR1         ((uint8_t) (0x01 << 5))   ///< ITC interrupt priority vector 30 [1]
  #define _ITC_SPR8_VECT31SPR          ((uint8_t) (0x03 << 6))   ///< ITC interrupt priority vector 31 [1:0]
  #define _ITC_SPR8_VECT31SPR0         ((uint8_t) (0x01 << 6))   ///< ITC interrupt priority vector 31 [0]
  #define _ITC_SPR8_VECT31SPR1         ((uint8_t) (0x01 << 7))   ///< ITC interrupt priority vector 31 [1]

#endif // ITC_AddressBase


// undefine local macros
#undef _BITS

/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // STLUX_STNRG_H

/** @}*/
