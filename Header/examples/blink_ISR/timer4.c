/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
//#include "STM8S208MB.h"   // muBoard
#include "STM8S105C6.h"   // STM8S-Discovery


/*----------------------------------------------------------
    GLOBAL VARIABLES
----------------------------------------------------------*/

// global ms counter (increased in TIM4_UPD_ISR)
uint32_t   g_millis = 0;


/*----------------------------------------------------------
    GLOBAL FUNCTIONS
----------------------------------------------------------*/

/**
  \fn void TIM4_UPD_ISR(void)
   
  \brief ISR for TIM4 update
   
  Timer TIM4 ISR, increase global ms counter.
  
  Notes:
    - for Cosmic compiler, add TIM4_UPD_ISR also to 'stm8_interrupt_vector.c'
*/
ISR_HANDLER(TIM4_UPD_ISR, __TIM4_UPD_OVF_VECTOR__)
{
  // reset ISR flag
  _TIM4.SR1.reg.UIF = 0;

  // set/increase global variables
  g_millis++;
    
} // TIM4_UPD_ISR
