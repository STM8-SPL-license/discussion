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

REM PAUSE
