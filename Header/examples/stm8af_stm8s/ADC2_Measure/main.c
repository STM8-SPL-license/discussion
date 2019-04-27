/**********************
  STM8 measure via ADC2 and printf result via UART1
  Demonstrate byte and bit access to registers

  Functionality:
  - init FCPU to 16MHz
  - init ADC2
  - init UART1
  - no interrupts
  - measure ADC2 and print result via UART1

  Boards:
  - muBoard (http://www.cream-tea.de/presentations/160305_PiAndMore.pdf)
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "main.h"
#include "string.h"

// define communication speed
#define BAUDRATE   9600

// define ADC channel to measure (poti=AIN5)
#define poti      5


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
  _CLK_CKDIVR = 0x00;                                          // clear complete register
  //_CLK_CKDIVR &= ~(_CLK_CKDIVR_CPUDIV | _CLK_CKDIVR_HSIDIV);   // using bitmasks
  //_CLK.CKDIVR.CPUDIV = 0; _CLK.CKDIVR.HSIDIV  = 0;             // direct access

  // switch to external 16MHz clock (no timeout!)
  _CLK.SWCR.SWIF = 0;                 // reset clock ISR flag
  _CLK.SWR.SWI   = _CLK_SWR_SWI_HSE;  // select target clock source to external
  while ((!_CLK.SWCR.SWIF));          // wait for external clock stable
  _CLK.SWCR.SWEN = 1;                 // execute clock switch


  // set UART1 baudrate (note: BRR2 must be written before BRR1!)
  BRR = (uint16_t) (((uint32_t) 16000000L)/BAUDRATE);
  _UART1_BRR2 = (uint8_t) (((BRR & 0xF000) >> 8) | (BRR & 0x000F));
  _UART1_BRR1 = (uint8_t) ((BRR & 0x0FF0) >> 4);

  // enable UART1 receiver & sender
  //_UART1_CR2 |= (_UART1_CR2_REN | _UART1_CR2_TEN);
  _UART1.CR2.REN = 1; _UART1.CR2.TEN = 1;

  // init ADC
  _ADC2.CSR.CH = poti;               // switch to selected ADC channel
  _ADC2.CR1.SPSEL = 7;               // set ADC clock to 1/18*fMaster to minimize noise
  _ADC2.CR2.ALIGN = 1;               // right alignment (read DRL, then DRH)
  _ADC2.TDRL.TDL  = 0xFF;            // disable all Schmitt triggers, (recommended for EMC)
  _ADC2.TDRH.TDH  = 0xFF;


  ////
  // main loop
  ////
  while (1) {

    // perform ADC measurement
    _ADC2.CSR.EOC  = 0;              // reset EOC flag, set by ADC
    _ADC2.CR1.ADON = 1;              // start conversion. Needed twice for unknown reason
    _ADC2.CR1.ADON = 1;
    while(!_ADC2.CSR.EOC);           // wait for "conversion ready" flag

    // get ADC result (read low byte first for right alignment!)
    value  =  (uint16_t)(_ADC2.DRL.DATA);
    value += ((uint16_t)(_ADC2.DRH.DATA)) << 8;

    // print result via UART1
    printf("%d \n", (int) value);

    // wait a while
    for (i=0; i<500000L; i++)
      NOP();
    
  } // main loop

} // main()
