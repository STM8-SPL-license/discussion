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


/**
  \fn void putchar(char byte)
   
  \brief output routine for printf()
  
  \param[in]  byte   data to send
  
  \return  always zero (Cosmic & SDCC >=3.6.0)

  implementation of putchar() for printf(), using selected output channel.
  Use send routine set via putchar_attach()
  Return type depends on used compiler (see respective stdio.h)
*/
#if defined(_SDCC_)
  int putchar(int c) {
#elif defined(_COSMIC_)
  char putchar(char c) {
#elif defined(_RAISONANCE_)
  int putchar(char c) {
#else // IAR
  int putchar(int c) {
#endif

  // wait until TX buffer is available
  while (!(_UART2_SR & _UART2_TXE));
  //while (!(_UART2.SR.TXE));
  
  // send byte
  _UART2_DR = c;

  // Wait until sent
  //while (!(_UART2_SR & _UART2_TC));
  //while (!(_UART2.SR.TC));

  // echo sent bytes
  return(c);
  
  
} // putchar

/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
