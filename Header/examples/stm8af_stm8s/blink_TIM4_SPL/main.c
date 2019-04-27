/**********************
  STM8 blink LED with interrupts using SPL and register access.
  Demonstrate byte and bit access to registers

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
    BOARD SELECTION
----------------------------------------------------------*/
#define STM8S_DISCOVERY 1
#define SDUINO_UNO      2
#define BOARD           SDUINO_UNO


/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#if BOARD == STM8S_DISCOVERY
  #warning STM8S-Discovery
  #include "STM8S105C6.h"
#elif BOARD == SDUINO_UNO
  #warning sduino-UNO
  #include "STM8S105K6.h"
#else
  #error please select supported device or adapt pinning
  #include <stophere>
#endif

// SPL headers
#include "stm8s_it.h"
#include "stm8s_gpio.h"
#include "stm8s_tim4.h"


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
  //_CLK_CKDIVR = 0x00;                                         // clear complete register
  //_CLK_CKDIVR &= ~(_CLK_CKDIVR_CPUDIV | _CLK_CKDIVR_HSIDIV);  // using bitmasks
  _CLK.CKDIVR.CPUDIV = 0; _CLK.CKDIVR.HSIDIV  = 0;            // direct access

  // configure LED pin to output push-pull (SPL)
  #if BOARD == STM8S_DISCOVERY     // STM8S-Discovery -> PD0
    GPIO_Init(GPIOD, (GPIO_Pin_TypeDef) GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_FAST);
  #else                            // sduino-UNO -> PC5
    GPIO_Init(GPIOC, (GPIO_Pin_TypeDef) GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);
  #endif

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

      // toggle LED
      #if BOARD == STM8S_DISCOVERY     // STM8S-Discovery -> PD0
        //_PORTD_ODR ^= _PORT_PIN0;        // byte access (smaller)
        _PORTD.ODR.PIN0 ^= 1;            // bit access (more convenient)
      #else                            // sduino-UNO -> PC5
        //_PORTC_ODR ^= _PORT_PIN5;        // byte access (smaller)
        _PORTC.ODR.PIN5 ^= 1;            // bit access (more convenient)
      #endif

    } // 500ms loop

  } // main loop

} // main()
