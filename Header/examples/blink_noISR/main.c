/**********************
  STM8 blink LED w/o interrupts

  Functionality:
  - init FCPU to 16MHz
  - configure LED pin as output
  - no interrupts
  - blink pin with blocking delay

  Boards:
  - STM8SDiscovery   https://www.st.com/en/evaluation-tools/stm8s-discovery.html
  - sduino-UNO       https://github.com/roybaer/sduino_uno
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "../../stm8/STM8S105C6.h"   // STM8S-Discovery
//#include "../../stm8/STM8S105K6.h"   // sduino-UNO
    

// define LED pin 
#if defined(STM8S105K6)               // sduino-UNO -> PC5
  #warning sduino-UNO
  #define LED_PORT  _GPIOC
  #define LED_PIN   PIN5
#elif defined(STM8S105C6)             // STM8S-Discovery -> PD0
  #warning STM8S-Discovery
  #define LED_PORT  _GPIOD
  #define LED_PIN   PIN0
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
  _CLK.CKDIVR &= ~(_CLK_CPUDIV | _CLK_HSIDIV);

  // set LED pin to output push-pull
  LED_PORT.DDR |= LED_PIN;    // input(=0) or output(=1)
  LED_PORT.CR1 |= LED_PIN;    // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
  LED_PORT.CR2 |= LED_PIN;    // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope

  ////
  // main loop
  ////
  while (1) {
    
    // blink LED
    LED_PORT.ODR ^= LED_PIN;
    
    // wait a bit
    for (i=200000L; i; i--)
      NOP();
      

  } // main loop

} // main()
