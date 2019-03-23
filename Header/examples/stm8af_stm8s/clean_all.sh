#!/bin/bash

# change to current working directory
cd `dirname $0`

# clean all sub-projects

cd ADC1_Measure       ; ./clean.sh; cd ..
cd ADC2_Measure       ; ./clean.sh; cd ..
cd blink_noISR        ; ./clean.sh; cd ..
cd blink_TIM4_ISR     ; ./clean.sh; cd ..
cd blink_TIM4_SPL     ; ./clean.sh; cd ..
cd Flash_EEPROM       ; ./clean.sh; cd ..
cd TIM2_PWM           ; ./clean.sh; cd ..
cd UART1_echo         ; ./clean.sh; cd ..
cd UART1_Gets_Printf  ; ./clean.sh; cd ..
cd UART2_echo         ; ./clean.sh; cd ..
cd UART2_Gets_Printf  ; ./clean.sh; cd ..

#PAUSE test
