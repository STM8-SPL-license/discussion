/**********************
  STM8 measure via ADC1 and printf result via UART2
  Demonstrate byte and bit access to registers

  Functionality:
  - init FCPU to 16MHz
  - init ADC1
  - init UART2
  - no interrupts
  - measure ADC1 and print result via UART2

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

// define ADC channel to measure (A0=PB0=AIN0)
#define A0      0


////////
// main routine
////////
void main(void) {

  uint16_t  BRR;
  uint16_t  value;
  uint32_t  i;


  ////
  // initialization
  ////

  // switch to 16MHz clock (reset is 2MHz)
  _CLK_CKDIVR = 0x00;                                 // clear complete register
  //_CLK_CKDIVR &= ~(_CLK_CPUDIV | _CLK_HSIDIV);        // using bitmasks
  //_CLK.CKDIVR.CPUDIV = 0; _CLK.CKDIVR.HSIDIV  = 0;    // direct access

  // set UART2 baudrate (note: BRR2 must be written before BRR1!)
  BRR = (uint16_t) (((uint32_t) 16000000L)/BAUDRATE);
  _UART2_BRR2 = (uint8_t) (((BRR & 0xF000) >> 8) | (BRR & 0x000F));
  _UART2_BRR1 = (uint8_t) ((BRR & 0x0FF0) >> 4);

  // enable UART2 receiver & sender
  //_UART2_CR2 |= (_UART2_REN | _UART2_TEN);
  _UART2.CR2.REN = 1; _UART2.CR2.TEN = 1;

  // init ADC
  _ADC1.CSR.CH = A0;                 // switch to selected ADC channel
  _ADC1.CR1.SPSEL = 7;               // set ADC clock to 1/18*fMaster to minimize noise
  _ADC1.CR2.ALIGN = 1;               // right alignment (read DRL, then DRH)
  _ADC1.TDRL.TDL  = 0xFF;            // disable all Schmitt triggers, (recommended for EMC)
  _ADC1.TDRH.TDH  = 0xFF;


  ////
  // main loop
  ////
  while (1) {

    // perform ADC measurement
    _ADC1.CSR.EOC  = 0;              // reset EOC flag, set by ADC
    _ADC1.CR1.ADON = 1;              // start conversion. Needed twice for unknown reason
    _ADC1.CR1.ADON = 1;
    while(!_ADC1.CSR.EOC);           // wait for "conversion ready" flag

    // get ADC result (read low byte first for right alignment!)
    value  =  (uint16_t)(_ADC1.DRL.DATA);
    value += ((uint16_t)(_ADC1.DRH.DATA)) << 8;

    // print result via UART1
    printf("%d \n", (int) value);

    // wait a while
    for (i=0; i<500000L; i++)
      NOP();

  } // main loop

} // main()
