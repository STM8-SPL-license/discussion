/**********************
  STM8 generate PWM using TIM2
  Demonstrate byte and bit access to registers

  Functionality:
  - init FCPU to 16MHz
  - configure pin D6 as outputs
  - generate PWM via TIM2 (100Hz, 30%)

  Boards:
  - sduino-UNO       https://github.com/roybaer/sduino_uno
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "STM8S105K6.h"


////////
// main routine
////////
void main(void) {

  uint16_t ARR;                       // reload value (=period)
  uint16_t CC2;                       // duty cycle (fraction of ARR)

  ////
  // initialization
  ////

  // switch to 16MHz clock (reset is 2MHz)
  _CLK_CKDIVR = 0x00;                                 // clear complete register

  // configure PWM output pin (D6=PD3=TIM2_CC2) as push-pull (bitmasks)
  _GPIOD_DDR |= _GPIO_PIN3;        // input(=0) or output(=1)
  _GPIOD_CR1 |= _GPIO_PIN3;        // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
  _GPIOD_CR2 |= _GPIO_PIN3;        // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope


  // configure TIM2_CC2 for 100Hz / 30%
  ARR   = 625;                                // reload value (=period)
  CC2   = 187;                                // duty cycle (fraction of ARR)
  _TIM2_CR1 = _TIM2_CEN | _TIM2_ARPE;         // stop timer & buffer registers
  _TIM2_PSCR = 8;                             // set prescaler to 2^8 -> 62.5kHz
  _TIM2_ARRH = (uint8_t) (ARR >> 8);          // set period (HB)
  _TIM2_ARRL = (uint8_t) (ARR & 0xFF);        // set period (LB)
  _TIM2_CCR2H = (uint8_t) (CC2 >> 8);         // duty cycle (HB)
  _TIM2_CCR2L = (uint8_t) (CC2 & 0xFF);       // duty cycle (LB)
  _TIM2.CCER1.CC2E = 1;                       // enable output for TIM2
  _TIM2.CCMR2.OUT.CC2S = 0;                   // CC2 channel is configured as output
  _TIM2.CCMR2.OUT.OC2PE = 1;                  // Output compare 2 preload enable
  _TIM2.CCMR2.OUT.OC2M = 6;                   // PWM mode 1
  _TIM2.CR1.CEN  = 1;                         // start timer

  ////
  // main loop
  ////
  while (1) {

    // dummy

  } // main loop

} // main()
