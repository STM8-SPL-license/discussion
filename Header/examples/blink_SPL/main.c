/**********************
  STM8 blink LED using both SPL and direct access and interrupt

  Functionality:
  - init FCPU to 16MHz using direct access
  - configure LED pin as output using SPL
  - use TIM4 interrupt using SPL
  - blink pin every 500ms using direct access

  Boards:
  - STM8SDiscovery   https://www.st.com/en/evaluation-tools/stm8s-discovery.html
  - sduino-UNO       https://github.com/roybaer/sduino_uno
*********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/

// select board in Makefile 
#if defined(STM8S105C6)
  #include "../../stm8/STM8S105C6.h"   // STM8S-Discovery
#else
  #include "../../stm8/STM8S105K6.h"   // sduino-UNO
#endif

// SPL headers
#include "stm8s_it.h"
#include "stm8s_gpio.h"
#include "stm8s_tim4.h"

// define LED pin 
#if defined(STM8S105K6)               // sduino-UNO -> PC5
  #warning sduino-UNO
  #define LED_PORT_SPL  (GPIOC)
  #define LED_PIN_SPL   (GPIO_PIN_5)
  #define LED_PORT      _GPIOC
  #define LED_PIN       PIN5
#elif defined(STM8S105C6)             // STM8S-Discovery -> PD0
  #warning STM8S-Discovery
  #define LED_PORT_SPL  (GPIOD)
  #define LED_PIN_SPL   (GPIO_PIN_0)
  #define LED_PORT      _GPIOD
  #define LED_PIN       PIN0
#else
  #error please select supported device or adapt pinning
  #include <stophere>
#endif


/*----------------------------------------------------------
    GLOBAL VARIABLES
----------------------------------------------------------*/

// global ms counter (increased in TIM4_UPD_OVF_IRQHandler)
extern uint32_t   g_millis;


/*----------------------------------------------------------
    GLOBAL FUNCTIONS
----------------------------------------------------------*/

// ISR already declared by SPL in stm8s_it.h


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

  // Initialize LED pin to output
  GPIO_Init(LED_PORT_SPL, (GPIO_Pin_TypeDef) LED_PIN_SPL, GPIO_MODE_OUT_PP_LOW_FAST);

  // init TIM4 for 1ms interrupt
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124);
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  TIM4_Cmd(ENABLE);

  // enable interrupts after initialization
  ENABLE_INTERRUPTS();


  ////
  // main loop
  ////
  while (1) {
    
    // blink LED every 500ms (direct bitwise access)
    if (g_millis > 500) {
      g_millis = 0;

      // blink LED
      LED_PORT.ODR ^= LED_PIN;

    } // 500ms 

  } // main loop

} // main()
