/**********************
  STM8 blink project with timer interrupt for STM8S-Discovery board
  Mix STM8S-SPL routines and direct access. 

  Functionality:
  - init FCPU to 16MHz using SPL
  - configure LED pin as output using SPL
  - use TIM4 interrupt using SPL
  - blink pin every 500ms using direct access
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/

// select in Makefile 
#if defined(STM8S208)               // muBoard
  #include "STM8S208MB.h"
#else
  #include "STM8S105C6.h"           // STM8S-Discovery
#endif

// SPL headers
#include "stm8s_it.h"
#include "stm8s_gpio.h"
#include "stm8s_tim4.h"

// define access to LED pin 
#if defined(STM8S208)               // muBoard -> PH2
  #define LED_PORT  (GPIOH)             // SPL
  #define LED_MASK  (GPIO_PIN_2)        // SPL
  #define LED_PIN   _GPIOH.ODR.bit.b2   // direct
#else                               // STM8S-Discovery -> PD0
  #define LED_PORT  (GPIOD)             // SPL
  #define LED_MASK  (GPIO_PIN_0)        // SPL
  #define LED_PIN   _GPIOD.ODR.bit.b0   // direct
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
  DISABLE_INTERRUPTS;
  
  // switch to 16MHz clock (reset is 2MHz)
  _CLK.CKDIVR.byte = 0x00;

  // Initialize LED pin to output
  GPIO_Init(LED_PORT, (GPIO_Pin_TypeDef)LED_MASK, GPIO_MODE_OUT_PP_LOW_FAST);

  // init TIM4 for 1ms interrupt
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124);
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  TIM4_Cmd(ENABLE);

  // enable interrupts after initialization
  ENABLE_INTERRUPTS;


  ////
  // main loop
  ////
  while (1) {
    
    // blink LED every 500ms (direct bitwise access)
    if (g_millis > 500) {
      g_millis = 0;
      LED_PIN ^= 1;

    } // 500ms 

  } // main loop

} // main()
