/**********************
  STM8 use scanf/printf via UART2
  Demonstrate byte and bit access to registers

  Functionality:
  - init FCPU to 16MHz
  - init UART2
  - no interrupts
  - use scanf to read a number, then print it via printf (both UART2)

  Boards:
  - sduino-UNO       https://github.com/roybaer/sduino_uno
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "main.h"
#include "string.h"

// define communication speed
#define BAUDRATE   9600


////////
// main routine
////////
void main(void) {

  uint16_t  BRR;
  char      str[20];
  int       num;


  ////
  // initialization
  ////

  // switch to 16MHz clock (reset is 2MHz)
  _CLK_CKDIVR = 0x00;                                          // clear complete register
  //_CLK_CKDIVR &= ~(_CLK_CKDIVR_CPUDIV | _CLK_CKDIVR_HSIDIV);   // using bitmasks
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

    // read a string via UART2
    printf("Enter a number: ");
    gets(str);

    // if terminal terminates with NL & LF, skip 2nd EOL char
    //getchar();

    // convert to integer [-2^16; 2^16-1]
    num = atoi(str);

    // print result via UART2
    printf("value: %d\n\n", num);

  } // main loop

} // main()
