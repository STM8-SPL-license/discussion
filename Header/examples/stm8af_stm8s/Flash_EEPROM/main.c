/**********************
  STM8 write to flash / EEPROM
  Demonstrate byte and bit access to registers

  Functionality:
  - init FCPU to 16MHz
  - init UART2
  - no interrupts
  - on byte received via UART2
    - if 'r' print byte from fixed address in flash & EEPROM
    - else save byte to EEPROM and byte+1 to flash

  Boards:
  - sduino-UNO       https://github.com/roybaer/sduino_uno
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "main.h"    // sduino-UNO

// define communication speed
#define BAUDRATE   9600

// addresses to write to / read from
#define FLASH_ADDR    0xF000              // for flash assert 16b address space
#define EEPROM_ADDR   STM8_EEPROM_START   // for EEPROM use first byte


////////
// main routine
////////
void main(void) {

  uint16_t  BRR;

  ////
  // initialization
  ////

  // switch to 16MHz clock (reset is 2MHz)
  //_CLK_CKDIVR = 0x00;                                          // clear complete register
  _CLK_CKDIVR &= ~(_CLK_CKDIVR_CPUDIV | _CLK_CKDIVR_HSIDIV);   // using bitmasks
  //_CLK.CKDIVR.CPUDIV = 0; _CLK.CKDIVR.HSIDIV  = 0;             // direct access

  // set UART2 baudrate (note: BRR2 must be written before BRR1!)
  BRR = (uint16_t) (((uint32_t) 16000000L)/BAUDRATE);
  _UART2_BRR2 = (uint8_t) (((BRR & 0xF000) >> 8) | (BRR & 0x000F));
  _UART2_BRR1 = (uint8_t) ((BRR & 0x0FF0) >> 4);

  // enable UART2 receiver & sender
  _UART2_CR2 |= (_UART2_CR2_REN | _UART2_CR2_TEN);
  //_UART2.CR2.REN = 1; _UART2.CR2.TEN = 1;


  ////
  // main loop
  ////
  while (1) {

    // if byte received, reply echo+1 and blink LED
    if (_UART2.SR.RXNE) {
    //if (_UART2_SR & _UART2_SR_RXNE) {

      // read byte from receive buffer
      uint8_t c = _UART2_DR;

      // read from flash
      if (c == 'r') {
        uint8_t val;
        val = *((uint8_t*) EEPROM_ADDR);
        printf("read '%c' (0x%02x) from EEPROM at 0x%04x\n", val, (int) val, EEPROM_ADDR);
        val = *((uint8_t*) FLASH_ADDR);
        printf("read '%c' (0x%02x) from flash at 0x%04x\n", val, (int) val, FLASH_ADDR);
      } // read from flash

      // write to flash and EEPROM
      else {

        // EEPROM
        printf("write '%c' (0x%02x) to EEPROM at 0x%04X ... ", c, (int) c, EEPROM_ADDR);
        _FLASH_DUKR = 0xAE;               // unlock w/e access to EEPROM
        _FLASH_DUKR = 0x56;
        while(!_FLASH.IAPSR.DUL);         // wait until unlocked
        *((uint8_t*) EEPROM_ADDR) = c;    // write byte
        while (!_FLASH.IAPSR.EOP);        // wait until done
        _FLASH.IAPSR.DUL = 0;             // lock EEPROM again
        printf("done\n");

        // flash
        c++;
        printf("write '%c' (0x%02x) to flash at 0x%04x ... ", c, (int) c, FLASH_ADDR);
        _FLASH_PUKR = 0x56;               // unlock w/e access to flash (opposite from EEPROM!)
        _FLASH_PUKR = 0xAE;
        while(!_FLASH.IAPSR.PUL);         // wait until unlocked
        *((uint8_t*) FLASH_ADDR) = c;     // write byte (note: direct write only works for 16bit addresses)
        while (!_FLASH.IAPSR.EOP);        // wait until done
        _FLASH.IAPSR.PUL = 0;             // lock flash again
        printf("done\n");

      } // write to flash and EEPROM

    } // byte received

  } // main loop

} // main()
