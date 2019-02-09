/**********************
  STM8 send/receive via UART2 

  Functionality:
  - init FCPU to 16MHz
  - init UART2
  - no interrupts
  - receive byte via UART2 and echo char+1

  Boards:
  - sduino-UNO       https://github.com/roybaer/sduino_uno
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "../../stm8/STM8S105K6.h"   // sduino-UNO

// define communication speed
#define BAUDRATE   9600


////////
// main routine
////////
void main(void) {

  uint16_t  BRR;

  ////
  // initialization
  ////
  
  // switch to 16MHz clock (reset is 2MHz)
  _CLK.CKDIVR &= ~(_CLK_CPUDIV | _CLK_HSIDIV);

  // set UART2 baudrate (note: BRR2 must be written before BRR1!)
  BRR = (uint16_t) (((uint32_t) 16000000L)/BAUDRATE);
  _UART2.BRR2 = (uint8_t) (((BRR & 0xF000) >> 8) | (BRR & 0x000F));
  _UART2.BRR1 = (uint8_t) ((BRR & 0x0FF0) >> 4);
  
  // enable UART2 receiver & sender
  _UART2.CR2 |= (_UART2_REN | _UART2_TEN);


  ////
  // main loop
  ////
  while (1) {
    
    // if byte received, reply echo+1 and blink LED
    if (_UART2.SR & _UART2_RXNE) {

      // read byte from receive buffer
      uint8_t c = _UART2.DR;

      // send echo
      _UART2.DR = _UART2.DR +1;

    } // byte received

  } // main loop

} // main()
