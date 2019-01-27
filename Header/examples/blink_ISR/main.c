/**********************
  STM8 blink project with timer interrupt for STM8S-Discovery board

  Functionality:
  - init FCPU to 16MHz
  - configure LED pin as output
  - use TIM4 interrupt
  - blink pin every 500ms
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "STM8S208MB.h"   // muBoard
//#include "STM8S105C6.h"   // STM8S-Discovery

// define access to LED pin 
#if defined(STM8S208)               // muBoard -> PH2
  #define LED_PORT  _GPIOH
  #define LED_MASK  (0x01 << 2)
  #define LED_PIN   _GPIOH.ODR.bit.b2
#else                               // STM8S-Discovery -> PD0
  #define LED_PORT  _GPIOD
  #define LED_MASK  (0x01 << 0)
  #define LED_PIN   _GPIOD.ODR.bit.b0
#endif


/*----------------------------------------------------------
    GLOBAL VARIABLES
----------------------------------------------------------*/

// global ms counter (increased in TIM4_UPD_ISR)
extern uint32_t   g_millis;


/*----------------------------------------------------------
    GLOBAL FUNCTIONS
----------------------------------------------------------*/

// SDCC requires ISR declaration in main file!!!
ISR_HANDLER(TIM4_UPD_ISR, __TIM4_UPD_OVF_VECTOR__);


////////
// main routine
////////
void main(void) {
  
  ////
  // initialization
  ////

  // disable interrupts for initialization
  DISABLE_INTERRUPTS;
  
  // switch to 16MHz clock (reset is 2MHz)
  _CLK.CKDIVR.byte = 0x00;

  // set LED pin to output (bytewise access)
  LED_PORT.DDR.byte |= LED_MASK;     // input(=0) or output(=1)
  LED_PORT.CR1.byte |= LED_MASK;     // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
  LED_PORT.CR2.byte |= LED_MASK;     // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope

  // init TIM4 for 1ms interrupt
  _TIM4.CR1.reg.ARPE = 1;    // auto-reload value buffered
  _TIM4.PSCR.reg.PSC = 6;    // set clock to 16Mhz/2^6 = 250kHz -> 4us resolution
  _TIM4.ARR.byte  = 250;     // set autoreload period to 1ms (=250*4us)
  _TIM4.IER.reg.UIE = 1;     // enable timer 4 interrupt
  _TIM4.CR1.reg.CEN = 1;     // start timer

  // enable interrupts after initialization
  ENABLE_INTERRUPTS;


  ////
  // main loop
  ////
  while (1) {
    
    // blink LED every 500ms (bitwise access)
    if (g_millis > 500) {
      g_millis = 0;
      LED_PIN ^= 1;
    }

  } // main loop

} // main()
