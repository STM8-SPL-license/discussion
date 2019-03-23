/**********************
  STM8 send/receive via UART2
  Demonstrate byte and bit access to registers

  Functionality:
  - init FCPU to 16MHz
  - init UART2
  - init LED pin to output
  - no interrupts
  - on byte received via UART2
    - send echo + 1
    - blink LED

  Boards:
  - sduino-UNO       https://github.com/roybaer/sduino_uno
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "STM8S105K6.h"

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
  _CLK_CKDIVR = 0x00;                                 // clear complete register
  //_CLK_CKDIVR &= ~(_CLK_CPUDIV | _CLK_HSIDIV);        // using bitmasks
  //_CLK.CKDIVR.CPUDIV = 0; _CLK.CKDIVR.HSIDIV  = 0;    // direct access

  // configure LED pin (=PC5) to output push-pull (bitmasks)
  _GPIOC_DDR |= _GPIO_PIN5;        // input(=0) or output(=1)
  _GPIOC_CR1 |= _GPIO_PIN5;        // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
  _GPIOC_CR2 |= _GPIO_PIN5;        // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope

  // set UART2 baudrate (note: BRR2 must be written before BRR1!)
  BRR = (uint16_t) (((uint32_t) 16000000L)/BAUDRATE);
  _UART2_BRR2 = (uint8_t) (((BRR & 0xF000) >> 8) | (BRR & 0x000F));
  _UART2_BRR1 = (uint8_t) ((BRR & 0x0FF0) >> 4);

  // enable UART2 receiver & sender
  //_UART2_CR2 |= (_UART2_REN | _UART2_TEN);
  _UART2.CR2.REN = 1; _UART2.CR2.TEN = 1;


  ////
  // main loop
  ////
  while (1) {

    // if byte received, reply echo+1 and blink LED
    if (_UART2.SR.RXNE) {
    //if (_UART2_SR & _UART2_RXNE) {

      // toggle LED (=PC5)
      _GPIOC.ODR.PIN5 ^= 1;

      // read byte from receive buffer
      c = _UART2_DR;

      // send echo+1
      _UART2_DR = c +1;

    } // byte received

  } // main loop

} // main()
