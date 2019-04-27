/**
  \file getchar.c

  \author G. Icking-Konert
  \date 2017-04-15
  \version 0.1

  \brief implementation of getchar() function for gets()

  implementation of getchar() function required for stdio.h
  functions, e.g. gets().
*/

/*-----------------------------------------------------------------------------
    INCLUDE FILES
-----------------------------------------------------------------------------*/
#include "main.h"

// define data type, depending on compiler
#if defined(_SDCC_)
  #define RETURN_TYPE int
#elif defined(_COSMIC_)
  #define RETURN_TYPE char
#elif defined(_RAISONANCE_)
  #define RETURN_TYPE char
#else // IAR
  #define RETURN_TYPE int
#endif


/**
  \fn char getchar(void)

  \brief receive byte

  \return  character received or zero on error.

  implementation of getchar() for gets(), using selected input channel.
  Use receive routine set via getchar_attach()
  Return type depends on used compiler (see respective stdio.h)
*/
RETURN_TYPE getchar(void) {

  uint8_t   c;

  // wait until byte received
  while (!(_UART2_SR & _UART2_SR_RXNE));
  //while (!(_UART2.SR.RXNE));

  // read Rx buffer
  c = _UART2_DR;
  //c = _UART2.DR.DATA;

  // compiler specific: echo character & terminate with NL
  #if defined(_COSMIC_) || defined(_RAISONANCE_) || defined(_IAR_)

    // echo to console
    putchar(c);

    // Cosmic gets() terminates on NL
    if (c == 13)
      c = 10;

  #endif // _COSMIC_ || _RAISONANCE_ || _IAR_

  // return received byte
  return (RETURN_TYPE) c;

} // getchar

/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
