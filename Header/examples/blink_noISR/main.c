/**********************
  STM8 blink project for STM8S-Discovery board

  Functionality:
  - init FCPU to 16MHz
  - configure LED pin as output
  - no interrupts
  - blink pin with delay
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
//#include "STM8S208MB.h"   // muBoard
#include "STM8S105C6.h"   // STM8S-Discovery

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
    GLOBAL FUNCTIONS
----------------------------------------------------------*/
  
__at(PORTD_BaseAddress) uint8_t   portD_ODR;
__at(PORTD_BaseAddress+1) uint8_t   portD_IDR;
__at(PORTD_BaseAddress+2) uint8_t   portD_DDR;
__at(PORTD_BaseAddress+3) uint8_t   portD_CR1;
__at(PORTD_BaseAddress+4) uint8_t   portD_CR2;


////////
// main routine
////////
void main(void) {

  ////
  // initialization
  ////

  // switch to 16MHz clock (reset is 2MHz)
  _CLK.CKDIVR.byte = 0x00;

  // set LED pin to output (bytewise access)
  LED_PORT.DDR.byte |= LED_MASK;     // input(=0) or output(=1)
  LED_PORT.CR1.byte |= LED_MASK;     // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
  LED_PORT.CR2.byte |= LED_MASK;     // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope


  ////
  // main loop
  ////
  while (1) {
    
    // blink LED (bitwise access)
    LED_PIN ^= 1;
    //LED_PORT.ODR.byte ^= LED_MASK;
    //portD_ODR ^= 0x01;

    // wait a bit
    for (uint32_t i=0; i<200000L; i++)
      NOP;

  } // main loop

} // main()
