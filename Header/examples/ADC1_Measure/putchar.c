/**
  \file putchar.c

  \author G. Icking-Konert
  \date 2015-04-09
  \version 0.1

  \brief implementation of putchar() function for printf()

  implementation of putchar() function required for stdio.h
  functions, e.g. printf().
*/

/*-----------------------------------------------------------------------------
    INCLUDE FILES
-----------------------------------------------------------------------------*/
#include "main.h"

// define data type, depending on compiler
#if defined(_SDCC_)
  #define RETURN_TYPE int
  #define INPUT_TYPE  int
#elif defined(_COSMIC_)
  #define RETURN_TYPE char
  #define INPUT_TYPE  char
#elif defined(_RAISONANCE_)
  #define RETURN_TYPE int
  #define INPUT_TYPE  char
#else // IAR
  #define RETURN_TYPE int
  #define INPUT_TYPE  int
#endif


/**
  \fn void putchar(char byte)

  \brief output routine for printf()

  \param[in]  byte   data to send

  \return  always zero (Cosmic & SDCC >=3.6.0)

  implementation of putchar() for printf(), using selected output channel.
  Use send routine set via putchar_attach()
  Return type depends on used compiler (see respective stdio.h)
*/
RETURN_TYPE putchar(INPUT_TYPE c) {

  // wait until TX buffer is available
  while (!(_UART2_SR & _UART2_TXE));
  //while (!(_UART2.SR.TXE));

  // send byte
  _UART2_DR = c;
  //_UART2.DR.DATA = c;

  // echo sent bytes
  return(c);

} // putchar

/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
