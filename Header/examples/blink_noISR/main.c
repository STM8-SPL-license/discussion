/**********************
  STM8 blink LED w/o interrupts.
  Demonstrate byte and bit access to registers

  Functionality:
  - init FCPU to 16MHz
  - configure LED pin as output
  - blink pin with blocking delay

  Boards:
  - STM8SDiscovery   https://www.st.com/en/evaluation-tools/stm8s-discovery.html
  - sduino-UNO       https://github.com/roybaer/sduino_uno
**********************/

/*----------------------------------------------------------
    BOARD SELECTION
----------------------------------------------------------*/
#define STM8S_DISCOVERY 1
#define SDUINO_UNO      2
#define MUBOARD         3
#define BOARD           STM8S_DISCOVERY


/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#if BOARD == STM8S_DISCOVERY
  #warning STM8S-Discovery
  #include "../../stm8/STM8S105C6.h"
#elif BOARD == SDUINO_UNO
  #warning sduino-UNO
  #include "../../stm8/STM8S105K6.h"
#elif BOARD == MUBOARD
  #warning muBoard
  #include "../../stm8/STM8S207MB.h"
#else
  #error please select supported device or adapt pinning
  #include <stophere>
#endif


////////
// main routine
////////
void main(void) {

  // local variables
  uint32_t  i;

  ////
  // initialization
  ////

  // switch to 16MHz clock (reset is 2MHz)
  //_CLK_CKDIVR = 0x00;                                 // clear complete register
  //_CLK_CKDIVR &= ~(_CLK_CPUDIV | _CLK_HSIDIV);        // using bitmasks
  _CLK.CKDIVR.CPUDIV = 0; _CLK.CKDIVR.HSIDIV  = 0;    // direct access

  // configure LED pin to output push-pull (bitmasks)
  #if BOARD == STM8S_DISCOVERY     // STM8S-Discovery -> PD0
    _GPIOD_DDR |= _GPIO_PIN0;        // input(=0) or output(=1)
    _GPIOD_CR1 |= _GPIO_PIN0;        // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
    _GPIOD_CR2 |= _GPIO_PIN0;        // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope
  #elif BOARD == SDUINO_UNO        // sduino-UNO -> PC5
    _GPIOC_DDR |= _GPIO_PIN5;        // input(=0) or output(=1)
    _GPIOC_CR1 |= _GPIO_PIN5;        // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
    _GPIOC_CR2 |= _GPIO_PIN5;        // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope
  #elif BOARD == MUBOARD           // muBoard -> PH3 (red)
    _GPIOH_DDR |= _GPIO_PIN3;        // input(=0) or output(=1)
    _GPIOH_CR1 |= _GPIO_PIN3;        // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
    _GPIOH_CR2 |= _GPIO_PIN3;        // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope
  #endif


  ////
  // main loop
  ////
  while (1) {

    // toggle LED
    #if BOARD == STM8S_DISCOVERY     // STM8S-Discovery -> PD0
      //_GPIOD_ODR ^= _GPIO_PIN0;        // byte access (smaller)
      _GPIOD.ODR.PIN0 ^= 1;            // bit access (more convenient)
    #elif BOARD == SDUINO_UNO        // sduino-UNO -> PC5
      //_GPIOC_ODR ^= _GPIO_PIN5;        // byte access (smaller)
      _GPIOC.ODR.PIN5 ^= 1;            // bit access (more convenient)
    #elif BOARD == MUBOARD           // muBoard -> PH3 (red)
      //_GPIOH_ODR ^= _GPIO_PIN3;        // byte access (smaller)
      _GPIOH.ODR.PIN3 ^= 1;            // bit access (more convenient)
    #endif

    // wait a bit
    for (i=200000L; i; i--)
      NOP();

  } // main loop

} // main()
