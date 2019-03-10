#!/bin/bash 

# change to current working directory
cd `dirname $0`

# SDCC output
rm -fr *.asm
rm -fr *.lst
rm -fr *.rel
rm -fr *.rst
rm -fr *.sym
rm -fr *.cdb
rm -fr *.ihx
rm -fr *.lk
rm -fr *.map
rm -fr *.SRC

# STVD_Cosmic output
rm -fr STVD_Cosmic/Release/*.o
rm -fr STVD_Cosmic/Release/*.sm8
rm -fr STVD_Cosmic/Release/*.s19
rm -fr STVD_Cosmic/Release/*.elf
rm -fr STVD_Cosmic/Release/*.map
rm -fr STVD_Cosmic/Release/*.ls
rm -fr STVD_Cosmic/Debug/*.o
rm -fr STVD_Cosmic/Debug/*.sm8
rm -fr STVD_Cosmic/Debug/*.s19
rm -fr STVD_Cosmic/Debug/*.elf
rm -fr STVD_Cosmic/Debug/*.map
rm -fr STVD_Cosmic/Debug/*.ls

# Ride_Raisonance output
rm -fr Ride_Raisonance/*.AOF
rm -fr Ride_Raisonance/*.BIN
rm -fr Ride_Raisonance/*.CSEG
rm -fr Ride_Raisonance/*.ctx
rm -fr Ride_Raisonance/*.d
rm -fr Ride_Raisonance/*.dbi
rm -fr Ride_Raisonance/*.HEX
rm -fr Ride_Raisonance/*.lst
rm -fr Ride_Raisonance/*.MAP
rm -fr Ride_Raisonance/*.obj
rm -fr Ride_Raisonance/*.xdb
rm -fr Ride_Raisonance/*.XRF
rm -fr Ride_Raisonance/*.html

# IAR output
rm -fr IAR/Debug
rm -fr IAR/Release
