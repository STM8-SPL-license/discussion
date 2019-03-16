/**********************
  STM8 blink LED with TIM4 interrupt.
  Demonstrate byte and bit access to registers

  Functionality:
  - init FCPU to 16MHz
  - configure LED pin as output
  - set up TIM4 interrupt for time-keeping
  - blink pin every 500ms

  Boards:
  - STM8SDiscovery   https://www.st.com/en/evaluation-tools/stm8s-discovery.html
  - sduino-UNO       https://github.com/roybaer/sduino_uno
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "main.h"    // defines board & STM8 device


/*----------------------------------------------------------
    GLOBAL FUNCTIONS
----------------------------------------------------------*/

// SDCC requires ISR declaration in main file!!!
ISR_HANDLER(TIM4_UPD_ISR, __TIM4_UPD_OVF_VECTOR__);


/*----------------------------------------------------------
    GLOBAL VARIABLES
----------------------------------------------------------*/

// global ms counter (increased in TIM4_UPD_ISR)
uint32_t   g_millis = 0;


////////
// main routine
////////
void main(void) {

  ////
  // initialization
  ////

  // disable interrupts for initialization
  DISABLE_INTERRUPTS();

  // switch to 16MHz clock (reset is 2MHz)
  //_CLK_CKDIVR = 0x00;                                 // clear complete register
  //_CLK_CKDIVR &= ~(_CLK_CPUDIV | _CLK_HSIDIV);        // using bitmasks
  _CLK.CKDIVR.CPUDIV = 0; _CLK.CKDIVR.HSIDIV  = 0;    // direct access

  // configure LED pin to output push-pull (bitmasks)
  #if BOARD == STM8S_DISCOVERY     // STM8S-Discovery -> PD0
    _GPIOD_DDR |= _GPIO_PIN0;        // input(=0) or output(=1)
    _GPIOD_CR1 |= _GPIO_PIN0;        // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
    _GPIOD_CR2 |= _GPIO_PIN0;        // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope
  #else                            // sduino-UNO -> PC5
    _GPIOC_DDR |= _GPIO_PIN5;        // input(=0) or output(=1)
    _GPIOC_CR1 |= _GPIO_PIN5;        // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
    _GPIOC_CR2 |= _GPIO_PIN5;        // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope
  #endif

    // init TIM4 for 1ms interrupt
  _TIM4_CR      |= _TIM4_ARPE;         // auto-reload value buffered
  _TIM4.PSCR.PSC = 6;                  // set clock prescaler to 6 -> 16Mhz/2^6 = 250kHz -> 4us resolution
  _TIM4_ARR      = 250;                // set autoreload period to 1ms (=250*4us)
  //_TIM4_IER  |= _TIM4_UIE;             // enable timer 4 interrupt (bit mask)
  _TIM4.IER.UIE = 1;                   // enable timer 4 interrupt (direct access)
  _TIM4_CR   |= _TIM4_CEN;             // start timer

  // enable interrupts after initialization
  ENABLE_INTERRUPTS();


  ////
  // main loop
  ////
  while (1) {

    // blink LED every 500ms
    if (g_millis > 500) {
      g_millis = 0;

      // toggle LED
      #if BOARD == STM8S_DISCOVERY     // STM8S-Discovery -> PD0
        //_GPIOD_ODR ^= _GPIO_PIN0;        // byte access (smaller)
        _GPIOD.ODR.PIN0 ^= 1;            // bit access (more convenient)
      #else                            // sduino-UNO -> PC5
        //_GPIOC_ODR ^= _GPIO_PIN5;        // byte access (smaller)
        _GPIOC.ODR.PIN5 ^= 1;            // bit access (more convenient)
      #endif

    } // 500ms loop

  } // main loop

} // main()
