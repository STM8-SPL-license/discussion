/**********************
  STM8 blink project for STM8S-Discovery board

  Functionality:
  - init FCPU to 16MHz
  - configure LED pin as output
  - no interrupts
  - blink pin with delay

  Boards:
  - STM8SDiscovery   https://www.st.com/en/evaluation-tools/stm8s-discovery.html
  - sduino-UNO       https://github.com/roybaer/sduino_uno
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "STM8S105C6.h"   // STM8S-Discovery
//#include "STM8S105K6.h"   // sduino-UNO


// Select access method. Comment out for bytewise access
#define BIT_ACCESS

// define access to LED pin 
#if defined(STM8S105K6)               // sduino-UNO -> PC5
  #define LED_PORT  _GPIOC
  #define LED_MASK  (0x01 << 5)
  #define LED_PIN   _GPIOC.ODR.bit.b5
#elif defined(STM8S105C6)             // STM8S-Discovery -> PD0
  #define LED_PORT  _GPIOD
  #define LED_MASK  (0x01 << 0)
  #define LED_PIN   _GPIOD.ODR.bit.b0
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
  _CLK.CKDIVR.byte = 0x00;

  // set LED pin to output push-pull (bytewise access)
  LED_PORT.DDR.byte |= LED_MASK;    // input(=0) or output(=1)
  LED_PORT.CR1.byte |= LED_MASK;    // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
  LED_PORT.CR2.byte |= LED_MASK;    // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope


  ////
  // main loop
  ////
  while (1) {
    
    // blink LED (bit- or bytewise access)
    #ifdef BIT_ACCESS
      LED_PIN ^= 1;
    #else
      LED_PORT.ODR.byte ^= LED_MASK;
    #endif
    
    // wait a bit
    for (i=200000L; i; i--)
      NOP;

  } // main loop

} // main()
