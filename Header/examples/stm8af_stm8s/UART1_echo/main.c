/**********************
  STM8 send/receive via UART1
  Demonstrate byte and bit access to registers

  Functionality:
  - init FCPU to 16MHz
  - init UART1
  - init LED pin to output
  - no interrupts
  - on byte received via UART1
    - send echo + 1
    - blink LED

  Boards:
  - muBoard (http://www.cream-tea.de/presentations/160305_PiAndMore.pdf)
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "STM8S207MB.h"

// define communication speed
#define BAUDRATE   9600


////////
// main routine
////////
void main(void) {

  uint16_t  BRR;
  uint8_t   c;


  ////
  // initialization
  ////

  // switch to 16MHz clock (reset is 2MHz)
  _CLK_CKDIVR = 0x00;                                          // clear complete register
  //_CLK_CKDIVR &= ~(_CLK_CKDIVR_CPUDIV | _CLK_CKDIVR_HSIDIV);   // using bitmasks
  //_CLK.CKDIVR.CPUDIV = 0; _CLK.CKDIVR.HSIDIV  = 0;             // direct access

  // configure LED pin (=PC5) to output push-pull (bitmasks)
  _PORTC_DDR |= _PORT_PIN5;        // input(=0) or output(=1)
  _PORTC_CR1 |= _PORT_PIN5;        // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
  _PORTC_CR2 |= _PORT_PIN5;        // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope

  // set UART1 baudrate (note: BRR2 must be written before BRR1!)
  BRR = (uint16_t) (((uint32_t) 16000000L)/BAUDRATE);
  _UART1_BRR2 = (uint8_t) (((BRR & 0xF000) >> 8) | (BRR & 0x000F));
  _UART1_BRR1 = (uint8_t) ((BRR & 0x0FF0) >> 4);

  // enable UART1 receiver & sender
  _UART1_CR2 |= (_UART1_CR2_REN | _UART1_CR2_TEN);
  //_UART1.CR2.REN = 1; _UART1.CR2.TEN = 1;


  ////
  // main loop
  ////
  while (1) {

    // if byte received, reply echo+1 and blink LED
    //if (_UART1.SR.RXNE) {
    if (_UART1_SR & _UART1_SR_RXNE) {

      // toggle LED (=PC5)
      _PORTC.ODR.PIN5 ^= 1;

      // read byte from receive buffer
      c = _UART1_DR;

      // send echo+1
      _UART1_DR = c +1;

    } // byte received

  } // main loop

} // main()
