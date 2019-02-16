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


/**
  \fn char getchar(void)

  \brief receive byte

  \return  character received or zero on error.

  implementation of getchar() for gets(), using selected input channel.
  Use receive routine set via getchar_attach()
  Return type depends on used compiler (see respective stdio.h)
*/
#if defined(_SDCC_)
  int getchar() {
#elif defined(_COSMIC_)
  char getchar() {
#elif defined(_RAISONANCE_)
  char getchar() {
#else // IAR
  int getchar() {
#endif

  char   c;

  // wait until byte received
  while (!(_UART2.SR.RXNE));

  // read Rx buffer
  c = _UART2.DR;

  // compiler specific: echo character
  #if defined(_COSMIC_) || defined(_RAISONANCE_) || defined(_IAR_)

    // echo to console
    putchar(c);

    // Cosmic gets() terminates on NL
    if (c == 13)
      c = 10;

  #endif // __CSMC__

  // return received byte
  return c;

} // getchar


/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
