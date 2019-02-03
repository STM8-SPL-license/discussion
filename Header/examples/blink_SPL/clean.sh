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
