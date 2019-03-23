REM clean all sub-projects

cd ADC1_Measure          & cmd /c ".\clean.bat" & cd ..
cd ADC2_Measure          & cmd /c ".\clean.bat" & cd ..
cd blink_noISR           & cmd /c ".\clean.bat" & cd ..
cd blink_TIM4_ISR        & cmd /c ".\clean.bat" & cd ..
cd blink_TIM4_SPL        & cmd /c ".\clean.bat" & cd ..
cd Flash_EEPROM          & cmd /c ".\clean.bat" & cd ..
cd TIM2_PWM              & cmd /c ".\clean.bat" & cd ..
cd UART1_echo            & cmd /c ".\clean.bat" & cd ..
cd UART1_Gets_Printf     & cmd /c ".\clean.bat" & cd ..
cd UART2_echo            & cmd /c ".\clean.bat" & cd ..
cd UART2_Gets_Printf     & cmd /c ".\clean.bat" & cd ..

REM PAUSE test
