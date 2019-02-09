/**********************
  STM8 blink LED with TIM4 interrupt

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
#include "main.h"    // defines device / board


// define LED pin 
#if defined(STM8S105K6)               // sduino-UNO -> PC5
  #define LED_PORT  _GPIOC
  #define LED_PIN   PIN5
#elif defined(STM8S105C6)             // STM8S-Discovery -> PD0
  #define LED_PORT  _GPIOD
  #define LED_PIN   PIN0
#else
  #error please select supported device or adapt pinning
  #include <stophere>
#endif


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
  _CLK.CKDIVR &= ~(_CLK_CPUDIV | _CLK_HSIDIV);

  // set LED pin to output push-pull
  LED_PORT.DDR |= LED_PIN;    // input(=0) or output(=1)
  LED_PORT.CR1 |= LED_PIN;    // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
  LED_PORT.CR2 |= LED_PIN;    // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope

  // init TIM4 for 1ms interrupt
  _TIM4.CR1  |= _TIM4_ARPE;                    // auto-reload value buffered
  _TIM4.PSCR |= (_TIM4_PSC1 | _TIM4_PSC2);     // set clock prescaler to 6 -> 16Mhz/2^6 = 250kHz -> 4us resolution
  _TIM4.ARR   = 250;                           // set autoreload period to 1ms (=250*4us)
  _TIM4.IER  |= _TIM4_UIE;                     // enable timer 4 interrupt
  _TIM4.CR1  |= _TIM4_CEN;                     // start timer

  // enable interrupts after initialization
  ENABLE_INTERRUPTS();


  ////
  // main loop
  ////
  while (1) {
    
    // blink LED every 500ms (bitwise access)
    if (g_millis > 500) {
      g_millis = 0;
    
      // blink LED
      LED_PORT.ODR ^= LED_PIN;
    }

  } // main loop

} // main()
