REM SDCC output
rm -fr *.asm
DEL /Q *.lst
DEL /Q *.rel
DEL /Q *.rst
DEL /Q *.sym
DEL /Q *.cdb
DEL /Q *.ihx
DEL /Q *.lk
DEL /Q *.map
DEL /Q *.SRC

REM STVD_Cosmic output (keep *.lkf!)
DEL /Q STVD_Cosmic\Release\*.o
DEL /Q STVD_Cosmic\Release\*.sm8
DEL /Q STVD_Cosmic\Release\*.s19
DEL /Q STVD_Cosmic\Release\*.elf
DEL /Q STVD_Cosmic\Release\*.map
DEL /Q STVD_Cosmic\Release\*.ls
DEL /Q STVD_Cosmic\Debug\*.o
DEL /Q STVD_Cosmic\Debug\*.sm8
DEL /Q STVD_Cosmic\Debug\*.s19
DEL /Q STVD_Cosmic\Debug\*.elf
DEL /Q STVD_Cosmic\Debug\*.map
DEL /Q STVD_Cosmic\Debug\*.ls

REM Ride_Raisonance output
DEL /Q Ride_Raisonance\*.AOF
DEL /Q Ride_Raisonance\*.BIN
DEL /Q Ride_Raisonance\*.CSEG
DEL /Q Ride_Raisonance\*.ctx
DEL /Q Ride_Raisonance\*.d
DEL /Q Ride_Raisonance\*.dbi
DEL /Q Ride_Raisonance\*.HEX
DEL /Q Ride_Raisonance\*.lst
DEL /Q Ride_Raisonance\*.MAP
DEL /Q Ride_Raisonance\*.obj
DEL /Q Ride_Raisonance\*.xdb
DEL /Q Ride_Raisonance\*.XRF
DEL /Q Ride_Raisonance\*.html

REM IAR output
RMDIR /S /Q IAR\Debug
RMDIR /S /Q IAR\Release

REM PAUSE
