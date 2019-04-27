/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "main.h"    // defines device / board


/*----------------------------------------------------------
    GLOBAL VARIABLES
----------------------------------------------------------*/

// global ms counter (increased in TIM4_UPD_ISR)
extern uint32_t   g_millis;


/*----------------------------------------------------------
    GLOBAL FUNCTIONS
----------------------------------------------------------*/

/**
  \fn void TIM4_UPD_ISR(void)
   
  \brief ISR for TIM4 update
   
  Timer TIM4 ISR, increase global ms counter.
  
  Notes:
    - for Cosmic compiler, add TIM4_UPD_ISR also to 'stm8_interrupt_vector.c'
    - IAR compiler has an IRQ offset of +2 compared to STM8 datasheet (see below)
*/
#if defined(_IAR_)
   #pragma vector = 2+__TIM4_UPD_OVF_VECTOR__    // IAR with +2 IRQ offset!
#endif
ISR_HANDLER(TIM4_UPD_ISR, __TIM4_UPD_OVF_VECTOR__)
{
  // reset ISR flag
  _TIM4_SR  &= ~_TIM4_SR_UIF;
  
  // set/increase global variables
  g_millis++;
    
} // TIM4_UPD_ISR
