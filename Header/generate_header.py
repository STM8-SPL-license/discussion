#!/usr/bin/env python3

import os, sys, shutil
import argparse
import logging
import pandas

# commandline parameters with defaults
parser = argparse.ArgumentParser(description="STM8 header creator")
parser.add_argument('-f', '--file',    type=str,   help='import Excel', required=False, default='ProductsList.xlsx')
args = parser.parse_args()


# import Excel device list as downloaded from https://www.st.com/content/st_com/en/products/microcontrollers/stm8-8-bit-mcus.html
# table structure:
#  pars start in line 6 (=index 5)
#  columns indices:
#    0: Part Number
#    1: General Description
#    2: Marketing Status
#    3: Package
#    4: Operating Frequency (MHz)
#    5: FLASH Size (kB)
#    6: RAM Size (kB)
#    7: Data E2PROM (B) nom
#    8: Timers (8 bit) typ
#    9: Timers (16 bit) typ
#   10: Other timer functions
#   11: Number of A/D Converters (10-bit channels)
#   12: Number of A/D Converters (12-bit channels)
#   13: D/A Converters  (12 bit) typ
#   14: Comparator
#   15: I/Os (High Current)
#   16: I2C typ
#   17: SPI typ
#   18: CAN  typ
#   19: USART  typ
#   20: UART typ
#   21: LIN-UART typ
#   22: Crypto-HASH
#   23: Supply Voltage (V) min
#   24: Supply Voltage (V) max
#   25: Supply Current (uA) (Lowest power mode) typ
#   26: Supply Current (uA) (Run mode (per Mhz)) typ
#   27: Display controller
#   28: Operating Temperature (C) min
#   29: Operating Temperature (C) max
#   30: Touch sensing FW library
Filename = args.file
ProductList = pandas.read_excel(Filename, sheet_name=0)
start_line = 5
part        = ProductList.iloc[start_line:, 0]  # device name for header file name 
num_parts   = len(part.iloc[:])                 # number of devices
description = ProductList.iloc[start_line:, 1]  # comment in header
status      = ProductList.iloc[start_line:, 2]  # status = Active, Proposal
flash_size  = ProductList.iloc[start_line:, 5]  # for flash memory size 
RAM_size    = ProductList.iloc[start_line:, 6]  # for RAM memory size 
EEPROM_size = ProductList.iloc[start_line:, 7]  # for EEPROM memory size 
print('imported ' + str(num_parts) + ' devices from \'' + Filename + '\'')


# create new output folder for headers (avoid left-overs)
directory = 'stm8'
try:
    shutil.rmtree(directory)
except OSError:
    pass
try:
    os.makedirs(directory)
except OSError:
    print('cannot create folder \'' + directory + '\', exit!')
    exit(1);
try:
    shutil.copyfile('STM8AF_STM8S.h', directory+'/STM8AF_STM8S.h')
except OSError:
    print('cannot copy family headers to \'' + directory + '\', exit!')
    exit(1);


'''
  check if device exists
'''
def check_device( idx ):

    # check if device is active
    if status.iloc[idx].find('Active') != 0:
        return 'inactive'

    # STM8S103     standard line low density device
    if part.iloc[idx].find('STM8S103') == 0:     
        return 'supported'

    # STM8S903     standard line low density device
    elif part.iloc[idx].find('STM8S903') == 0:
        return 'supported'
    
    # STM8S105     standard line medium density device
    elif part.iloc[idx].find('STM8S105') == 0:
        return 'supported'
    
    # STM8S208     standard line high density device with CAN
    elif part.iloc[idx].find('STM8S208') == 0:
        return 'supported'
    
    # STM8S207     standard line high density device without CAN
    elif part.iloc[idx].find('STM8S207') == 0:
        return 'supported'
    
    # STM8AF622x / STM8AF621x   automotive low density devices without CAN
    elif part.iloc[idx].find('STM8AF622') == 0  or  part.iloc[idx].find('STM8AF621') == 0:
        return 'supported'
    
    # STM8AF626x   automotive medium density devices without CAN
    elif part.iloc[idx].find('STM8AF626') == 0:
        return 'supported'
    
    # STM8AF52Ax   automotive high density devices with CAN
    elif part.iloc[idx].find('STM8AF52A') == 0:
        return 'supported'
    
    # STM8AF62Ax   automotive high density devices without CAN
    elif part.iloc[idx].find('STM8AF62A') == 0:
        return 'supported'
    
    # STM8S003x    value line low density device
    elif part.iloc[idx].find('STM8S003') == 0:
        return 'supported'
    
    # STM8S005x    value line medium density device
    elif part.iloc[idx].find('STM8S005') == 0:
        return 'supported'
    
    # STM8S007x    value line high density device
    elif part.iloc[idx].find('STM8S007') == 0:
        return 'supported'

    # device not yet supported
    else:
        return 'pending'

    # check_device



'''
  save header file
'''
def save_header( idx ):

    # print optional message
    device = part.iloc[idx]
    #logging.warning('export %s.h', device)

    # create new file
    f = open(directory + '/' + device+'.h','w+')

    # write file header
    f.write('/*-------------------------------------------------------------------------\n')
    f.write('  ' + part.iloc[idx]+'.h - Register Declarations for STM ' + part.iloc[idx] + '\n')
    f.write('\n')
    f.write('  ' + description.iloc[idx] + '\n')
    f.write('\n')

    f.write('  Copyright (C) 2019, Georg Icking-Konert\n')
    f.write('\n')
    f.write('  This library is free software: you can redistribute it and/or modify\n')
    f.write('  it under the terms of the GNU General Public License as published by\n')
    f.write('  the Free Software Foundation, either version 3 of the License, or\n')
    f.write('  (at your option) any later version.\n')
    f.write('\n')
    f.write('  This library is distributed in the hope that it will be useful,\n')
    f.write('  but WITHOUT ANY WARRANTY; without even the implied warranty of\n')
    f.write('  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n')
    f.write('  GNU General Public License for more details.\n')
    f.write('\n')
    f.write('  You should have received a copy of the GNU General Public License\n')
    f.write('  along with this program. If not, see <https://www.gnu.org/licenses/>.\n')
    f.write('\n')
    f.write('  As a special exception, if you link this library with other files\n')
    f.write('  to produce an executable, this library does not by itself cause the\n')
    f.write('  resulting executable to be covered by the GNU General Public License.\n')
    f.write('  This exception does not however invalidate any other reasons why the\n')
    f.write('  executable file might be covered by the GNU General Public License.\n')
    f.write('-------------------------------------------------------------------------*/\n')
    f.write('\n')

    f.write('/*-----------------------------------------------------------------------------\n')
    f.write('    MODULE DEFINITION FOR MULTIPLE INCLUSION\n')
    f.write('-----------------------------------------------------------------------------*/\n')
    f.write('#ifndef ' + part.iloc[idx] + '_H\n')
    f.write('#define ' + part.iloc[idx] + '_H  1\n')
    f.write('\n')

    f.write('// device specific memory sizes [B]\n')
    f.write('#define STM8_PFLASH_SIZE ' + flash_size.iloc[idx] + '*1024\n')
    f.write('#define STM8_RAM_SIZE    ' + RAM_size.iloc[idx] + '*1024\n')
    f.write('#define STM8_EEPROM_SIZE ' + EEPROM_size.iloc[idx] + '\n')
    f.write('\n')


    # STM8S103: standard line low density device
    # datasheet number: DS6120
    # reference manual number: RM0016
    if device.find('STM8S103') == 0:
        f.write('// define device line\n')
        f.write('#if !defined(STM8S103)\n')
        f.write('  #define STM8S103\n')
        f.write('#endif\n')
        f.write('\n')
        f.write('// device specific base addresses\n')
        f.write('#define OPT_BaseAddress         0x4800\n')
        f.write('#define PORTA_BaseAddress       0x5000\n')
        f.write('#define PORTB_BaseAddress       0x5005\n')
        f.write('#define PORTC_BaseAddress       0x500A\n')
        f.write('#define PORTD_BaseAddress       0x500F\n')
        f.write('#define PORTE_BaseAddress       0x5014\n')
        f.write('#define PORTF_BaseAddress       0x5019\n')
        #f.write('#define PORTG_BaseAddress       0x501E\n')
        #f.write('#define PORTH_BaseAddress       0x5023\n')
        #f.write('#define PORTI_BaseAddress       0x5028\n')
        f.write('#define FLASH_BaseAddress       0x505A\n')
        f.write('#define EXTI_BaseAddress        0x50A0\n')
        f.write('#define RST_BaseAddress         0x50B3\n')
        f.write('#define CLK_BaseAddress         0x50C0\n')
        f.write('#define WWDG_BaseAddress        0x50D1\n')
        f.write('#define IWDG_BaseAddress        0x50E0\n')
        f.write('#define AWU_BaseAddress         0x50F0\n')
        f.write('#define BEEP_BaseAddress        0x50F3\n')
        f.write('#define SPI_BaseAddress         0x5200\n')
        f.write('#define I2C_BaseAddress         0x5210\n')
        f.write('#define UART1_BaseAddress       0x5230\n')
        #f.write('#define UART2_BaseAddress       0x5240\n')
        #f.write('#define UART3_BaseAddress       0x5240\n')
        #f.write('#define UART4_BaseAddress       0x5230\n')
        f.write('#define TIM1_BaseAddress        0x5250\n')
        f.write('#define TIM2_BaseAddress        0x5300\n')
        #f.write('#define TIM3_BaseAddress        0x5320\n')
        f.write('#define TIM4_BaseAddress        0x5340\n')
        #f.write('#define TIM5_BaseAddress        0x5300\n')
        #f.write('#define TIM6_BaseAddress        0x5340\n')
        f.write('#define ADC1_BaseAddress        0x53E0\n')
        #f.write('#define ADC2_BaseAddress        0x5400\n')
        #f.write('#define CAN_BaseAddress         0x5420\n')
        f.write('#define CFG_BaseAddress         0x7F60\n')
        f.write('#define ITC_BaseAddress         0x7F70\n')
        #f.write('#define DM_BaseAddress          0x7F90\n')
        f.write('\n')
        f.write('// unique ID start address\n')
        f.write('#define UID_BaseAddress         0x4865\n')
        f.write('\n')
        f.write('#include \"STM8AF_STM8S.h\"\n')
        # STM8S103


    # STM8S903     standard line low density device
    # datasheet number: DS6210
    # reference manual number: RM0016
    elif device.find('STM8S903') == 0:
        f.write('// define device line\n')
        f.write('#if !defined(STM8S903)\n')
        f.write('  #define STM8S903\n')
        f.write('#endif\n')
        f.write('\n')
        f.write('// device specific base addresses\n')
        f.write('#define OPT_BaseAddress         0x4800\n')
        f.write('#define PORTA_BaseAddress       0x5000\n')
        f.write('#define PORTB_BaseAddress       0x5005\n')
        f.write('#define PORTC_BaseAddress       0x500A\n')
        f.write('#define PORTD_BaseAddress       0x500F\n')
        f.write('#define PORTE_BaseAddress       0x5014\n')
        f.write('#define PORTF_BaseAddress       0x5019\n')
        #f.write('#define PORTG_BaseAddress       0x501E\n')
        #f.write('#define PORTH_BaseAddress       0x5023\n')
        #f.write('#define PORTI_BaseAddress       0x5028\n')
        f.write('#define FLASH_BaseAddress       0x505A\n')
        f.write('#define EXTI_BaseAddress        0x50A0\n')
        f.write('#define RST_BaseAddress         0x50B3\n')
        f.write('#define CLK_BaseAddress         0x50C0\n')
        f.write('#define WWDG_BaseAddress        0x50D1\n')
        f.write('#define IWDG_BaseAddress        0x50E0\n')
        f.write('#define AWU_BaseAddress         0x50F0\n')
        f.write('#define BEEP_BaseAddress        0x50F3\n')
        f.write('#define SPI_BaseAddress         0x5200\n')
        f.write('#define I2C_BaseAddress         0x5210\n')
        f.write('#define UART1_BaseAddress       0x5230\n')
        #f.write('#define UART2_BaseAddress       0x5240\n')
        #f.write('#define UART3_BaseAddress       0x5240\n')
        #f.write('#define UART4_BaseAddress       0x5230\n')
        f.write('#define TIM1_BaseAddress        0x5250\n')
        #f.write('#define TIM2_BaseAddress        0x5300\n')
        #f.write('#define TIM3_BaseAddress        0x5320\n')
        #f.write('#define TIM4_BaseAddress        0x5340\n')
        f.write('#define TIM5_BaseAddress        0x5300\n')
        f.write('#define TIM6_BaseAddress        0x5340\n')
        f.write('#define ADC1_BaseAddress        0x53E0\n')
        #f.write('#define ADC2_BaseAddress        0x5400\n')
        #f.write('#define CAN_BaseAddress         0x5420\n')
        f.write('#define CFG_BaseAddress         0x7F60\n')
        f.write('#define ITC_BaseAddress         0x7F70\n')
        #f.write('#define DM_BaseAddress          0x7F90\n')
        f.write('\n')
        f.write('// unique ID start address\n')
        f.write('#define UID_BaseAddress         0x4865\n')
        f.write('\n')
        f.write('#include \"STM8AF_STM8S.h\"\n')
        # STM8S903


    # STM8S105     standard line medium density device
    # datasheet number: DS5855
    # reference manual number: RM0016
    elif device.find('STM8S105') == 0:
        f.write('// define device line\n')
        f.write('#if !defined(STM8S105)\n')
        f.write('  #define STM8S105\n')
        f.write('#endif\n')
        f.write('\n')
        f.write('// device specific base addresses\n')
        f.write('#define OPT_BaseAddress         0x4800\n')
        f.write('#define PORTA_BaseAddress       0x5000\n')
        f.write('#define PORTB_BaseAddress       0x5005\n')
        f.write('#define PORTC_BaseAddress       0x500A\n')
        f.write('#define PORTD_BaseAddress       0x500F\n')
        f.write('#define PORTE_BaseAddress       0x5014\n')
        f.write('#define PORTF_BaseAddress       0x5019\n')
        f.write('#define PORTG_BaseAddress       0x501E\n')
        #f.write('#define PORTH_BaseAddress       0x5023\n')
        #f.write('#define PORTI_BaseAddress       0x5028\n')
        f.write('#define FLASH_BaseAddress       0x505A\n')
        f.write('#define EXTI_BaseAddress        0x50A0\n')
        f.write('#define RST_BaseAddress         0x50B3\n')
        f.write('#define CLK_BaseAddress         0x50C0\n')
        f.write('#define WWDG_BaseAddress        0x50D1\n')
        f.write('#define IWDG_BaseAddress        0x50E0\n')
        f.write('#define AWU_BaseAddress         0x50F0\n')
        f.write('#define BEEP_BaseAddress        0x50F3\n')
        f.write('#define SPI_BaseAddress         0x5200\n')
        f.write('#define I2C_BaseAddress         0x5210\n')
        #f.write('#define UART1_BaseAddress       0x5230\n')
        f.write('#define UART2_BaseAddress       0x5240\n')
        #f.write('#define UART3_BaseAddress       0x5240\n')
        #f.write('#define UART4_BaseAddress       0x5230\n')
        f.write('#define TIM1_BaseAddress        0x5250\n')
        f.write('#define TIM2_BaseAddress        0x5300\n')
        f.write('#define TIM3_BaseAddress        0x5320\n')
        f.write('#define TIM4_BaseAddress        0x5340\n')
        #f.write('#define TIM5_BaseAddress        0x5300\n')
        #f.write('#define TIM6_BaseAddress        0x5340\n')
        f.write('#define ADC1_BaseAddress        0x53E0\n')
        #f.write('#define ADC2_BaseAddress        0x5400\n')
        #f.write('#define CAN_BaseAddress         0x5420\n')
        f.write('#define CFG_BaseAddress         0x7F60\n')
        f.write('#define ITC_BaseAddress         0x7F70\n')
        #f.write('#define DM_BaseAddress          0x7F90\n')
        f.write('\n')
        f.write('// unique ID start address\n')
        f.write('#define UID_BaseAddress         0x48CD\n')
        f.write('\n')
        f.write('// include generic header for series\n')
        f.write('#include \"STM8AF_STM8S.h\"\n')
        # STM8S105


    # STM8S208     standard line high density device with CAN
    # datasheet number: DS5839
    # reference manual number: RM0016
    elif device.find('STM8S208') == 0:
        f.write('// define device line\n')
        f.write('#if !defined(STM8S208)\n')
        f.write('  #define STM8S208\n')
        f.write('#endif\n')
        f.write('\n')
        f.write('// device specific base addresses\n')
        f.write('#define OPT_BaseAddress         0x4800\n')
        f.write('#define PORTA_BaseAddress       0x5000\n')
        f.write('#define PORTB_BaseAddress       0x5005\n')
        f.write('#define PORTC_BaseAddress       0x500A\n')
        f.write('#define PORTD_BaseAddress       0x500F\n')
        f.write('#define PORTE_BaseAddress       0x5014\n')
        f.write('#define PORTF_BaseAddress       0x5019\n')
        f.write('#define PORTG_BaseAddress       0x501E\n')
        f.write('#define PORTH_BaseAddress       0x5023\n')
        f.write('#define PORTI_BaseAddress       0x5028\n')
        f.write('#define FLASH_BaseAddress       0x505A\n')
        f.write('#define EXTI_BaseAddress        0x50A0\n')
        f.write('#define RST_BaseAddress         0x50B3\n')
        f.write('#define CLK_BaseAddress         0x50C0\n')
        f.write('#define WWDG_BaseAddress        0x50D1\n')
        f.write('#define IWDG_BaseAddress        0x50E0\n')
        f.write('#define AWU_BaseAddress         0x50F0\n')
        f.write('#define BEEP_BaseAddress        0x50F3\n')
        f.write('#define SPI_BaseAddress         0x5200\n')
        f.write('#define I2C_BaseAddress         0x5210\n')
        f.write('#define UART1_BaseAddress       0x5230\n')
        #f.write('#define UART2_BaseAddress       0x5240\n')
        f.write('#define UART3_BaseAddress       0x5240\n')
        #f.write('#define UART4_BaseAddress       0x5230\n')
        f.write('#define TIM1_BaseAddress        0x5250\n')
        f.write('#define TIM2_BaseAddress        0x5300\n')
        f.write('#define TIM3_BaseAddress        0x5320\n')
        f.write('#define TIM4_BaseAddress        0x5340\n')
        #f.write('#define TIM5_BaseAddress        0x5300\n')
        #f.write('#define TIM6_BaseAddress        0x5340\n')
        #f.write('#define ADC1_BaseAddress        0x53E0\n')
        f.write('#define ADC2_BaseAddress        0x5400\n')
        f.write('#define CAN_BaseAddress         0x5420\n')
        f.write('#define CFG_BaseAddress         0x7F60\n')
        f.write('#define ITC_BaseAddress         0x7F70\n')
        #f.write('#define DM_BaseAddress          0x7F90\n')
        f.write('\n')
        f.write('// unique ID start address\n')
        f.write('#define UID_BaseAddress         0x48CD\n')
        f.write('\n')
        f.write('// include generic header for series\n')
        f.write('#include \"STM8AF_STM8S.h\"\n')
        # STM8S208


    # STM8S207     standard line high density device without CAN
    # datasheet number: DS5839
    # reference manual number: RM0016
    elif device.find('STM8S207') == 0:
        f.write('// define device line\n')
        f.write('#if !defined(STM8S207)\n')
        f.write('  #define STM8S207\n')
        f.write('#endif\n')
        f.write('\n')
        f.write('// device specific base addresses\n')
        f.write('#define OPT_BaseAddress         0x4800\n')
        f.write('#define PORTA_BaseAddress       0x5000\n')
        f.write('#define PORTB_BaseAddress       0x5005\n')
        f.write('#define PORTC_BaseAddress       0x500A\n')
        f.write('#define PORTD_BaseAddress       0x500F\n')
        f.write('#define PORTE_BaseAddress       0x5014\n')
        f.write('#define PORTF_BaseAddress       0x5019\n')
        f.write('#define PORTG_BaseAddress       0x501E\n')
        f.write('#define PORTH_BaseAddress       0x5023\n')
        f.write('#define PORTI_BaseAddress       0x5028\n')
        f.write('#define FLASH_BaseAddress       0x505A\n')
        f.write('#define EXTI_BaseAddress        0x50A0\n')
        f.write('#define RST_BaseAddress         0x50B3\n')
        f.write('#define CLK_BaseAddress         0x50C0\n')
        f.write('#define WWDG_BaseAddress        0x50D1\n')
        f.write('#define IWDG_BaseAddress        0x50E0\n')
        f.write('#define AWU_BaseAddress         0x50F0\n')
        f.write('#define BEEP_BaseAddress        0x50F3\n')
        f.write('#define SPI_BaseAddress         0x5200\n')
        f.write('#define I2C_BaseAddress         0x5210\n')
        f.write('#define UART1_BaseAddress       0x5230\n')
        #f.write('#define UART2_BaseAddress       0x5240\n')
        f.write('#define UART3_BaseAddress       0x5240\n')
        #f.write('#define UART4_BaseAddress       0x5230\n')
        f.write('#define TIM1_BaseAddress        0x5250\n')
        f.write('#define TIM2_BaseAddress        0x5300\n')
        f.write('#define TIM3_BaseAddress        0x5320\n')
        f.write('#define TIM4_BaseAddress        0x5340\n')
        #f.write('#define TIM5_BaseAddress        0x5300\n')
        #f.write('#define TIM6_BaseAddress        0x5340\n')
        #f.write('#define ADC1_BaseAddress        0x53E0\n')
        f.write('#define ADC2_BaseAddress        0x5400\n')
        #f.write('#define CAN_BaseAddress         0x5420\n')
        f.write('#define CFG_BaseAddress         0x7F60\n')
        f.write('#define ITC_BaseAddress         0x7F70\n')
        #f.write('#define DM_BaseAddress          0x7F90\n')
        f.write('\n')
        f.write('// unique ID start address\n')
        f.write('#define UID_BaseAddress         0x48CD\n')
        f.write('\n')
        f.write('// include generic header for series\n')
        f.write('#include \"STM8AF_STM8S.h\"\n')
        # STM8S207


    # STM8AF622x / STM8AF621x   automotive low density devices without CAN
    # datasheet number: DS9884
    # reference manual number: RM0016
    elif device.find('STM8AF622') == 0  or  device.find('STM8AF621') == 0:
        f.write('// define device line\n')
        f.write('#if !defined(STM8AF622x)\n')
        f.write('  #define STM8AF622x\n')
        f.write('#endif\n')
        f.write('\n')
        f.write('// device specific base addresses\n')
        f.write('#define OPT_BaseAddress         0x4800\n')
        f.write('#define PORTA_BaseAddress       0x5000\n')
        f.write('#define PORTB_BaseAddress       0x5005\n')
        f.write('#define PORTC_BaseAddress       0x500A\n')
        f.write('#define PORTD_BaseAddress       0x500F\n')
        f.write('#define PORTE_BaseAddress       0x5014\n')
        f.write('#define PORTF_BaseAddress       0x5019\n')
        #f.write('#define PORTG_BaseAddress       0x501E\n')
        #f.write('#define PORTH_BaseAddress       0x5023\n')
        #f.write('#define PORTI_BaseAddress       0x5028\n')
        f.write('#define FLASH_BaseAddress       0x505A\n')
        f.write('#define EXTI_BaseAddress        0x50A0\n')
        f.write('#define RST_BaseAddress         0x50B3\n')
        f.write('#define CLK_BaseAddress         0x50C0\n')
        f.write('#define WWDG_BaseAddress        0x50D1\n')
        f.write('#define IWDG_BaseAddress        0x50E0\n')
        f.write('#define AWU_BaseAddress         0x50F0\n')
        f.write('#define BEEP_BaseAddress        0x50F3\n')
        f.write('#define SPI_BaseAddress         0x5200\n')
        f.write('#define I2C_BaseAddress         0x5210\n')
        #f.write('#define UART1_BaseAddress       0x5230\n')
        #f.write('#define UART2_BaseAddress       0x5240\n')
        #f.write('#define UART3_BaseAddress       0x5240\n')
        f.write('#define UART4_BaseAddress       0x5230\n')
        f.write('#define TIM1_BaseAddress        0x5250\n')
        #f.write('#define TIM2_BaseAddress        0x5300\n')
        #f.write('#define TIM3_BaseAddress        0x5320\n')
        #f.write('#define TIM4_BaseAddress        0x5340\n')
        f.write('#define TIM5_BaseAddress        0x5300\n')
        f.write('#define TIM6_BaseAddress        0x5340\n')
        f.write('#define ADC1_BaseAddress        0x53E0\n')
        #f.write('#define ADC2_BaseAddress        0x5400\n')
        #f.write('#define CAN_BaseAddress         0x5420\n')
        f.write('#define CFG_BaseAddress         0x7F60\n')
        f.write('#define ITC_BaseAddress         0x7F70\n')
        #f.write('#define DM_BaseAddress          0x7F90\n')
        f.write('\n')
        f.write('// unique ID start address\n')
        f.write('#define UID_BaseAddress         0x4865\n')
        f.write('\n')
        f.write('// include generic header for series\n')
        f.write('#include \"STM8AF_STM8S.h\"\n')
        # STM8AF622x


    # STM8AF626x   automotive medium density devices without CAN
    # datasheet number: DS5913
    # reference manual number: RM0016
    elif device.find('STM8AF626') == 0:
        f.write('// define device line\n')
        f.write('#if !defined(STM8AF626x)\n')
        f.write('  #define STM8AF626x\n')
        f.write('#endif\n')
        f.write('\n')
        f.write('// device specific base addresses\n')
        f.write('#define OPT_BaseAddress         0x4800\n')
        f.write('#define PORTA_BaseAddress       0x5000\n')
        f.write('#define PORTB_BaseAddress       0x5005\n')
        f.write('#define PORTC_BaseAddress       0x500A\n')
        f.write('#define PORTD_BaseAddress       0x500F\n')
        f.write('#define PORTE_BaseAddress       0x5014\n')
        f.write('#define PORTF_BaseAddress       0x5019\n')
        f.write('#define PORTG_BaseAddress       0x501E\n')
        #f.write('#define PORTH_BaseAddress       0x5023\n')
        #f.write('#define PORTI_BaseAddress       0x5028\n')
        f.write('#define FLASH_BaseAddress       0x505A\n')
        f.write('#define EXTI_BaseAddress        0x50A0\n')
        f.write('#define RST_BaseAddress         0x50B3\n')
        f.write('#define CLK_BaseAddress         0x50C0\n')
        f.write('#define WWDG_BaseAddress        0x50D1\n')
        f.write('#define IWDG_BaseAddress        0x50E0\n')
        f.write('#define AWU_BaseAddress         0x50F0\n')
        f.write('#define BEEP_BaseAddress        0x50F3\n')
        f.write('#define SPI_BaseAddress         0x5200\n')
        f.write('#define I2C_BaseAddress         0x5210\n')
        #f.write('#define UART1_BaseAddress       0x5230\n')
        f.write('#define UART2_BaseAddress       0x5240\n')
        #f.write('#define UART3_BaseAddress       0x5240\n')
        #f.write('#define UART4_BaseAddress       0x5230\n')
        f.write('#define TIM1_BaseAddress        0x5250\n')
        f.write('#define TIM2_BaseAddress        0x5300\n')
        f.write('#define TIM3_BaseAddress        0x5320\n')
        f.write('#define TIM4_BaseAddress        0x5340\n')
        #f.write('#define TIM5_BaseAddress        0x5300\n')
        #f.write('#define TIM6_BaseAddress        0x5340\n')
        f.write('#define ADC1_BaseAddress        0x53E0\n')
        #f.write('#define ADC2_BaseAddress        0x5400\n')
        #f.write('#define CAN_BaseAddress         0x5420\n')
        f.write('#define CFG_BaseAddress         0x7F60\n')
        f.write('#define ITC_BaseAddress         0x7F70\n')
        #f.write('#define DM_BaseAddress          0x7F90\n')
        f.write('\n')
        f.write('// unique ID start address\n')
        f.write('#define UID_BaseAddress         xxx\n')
        f.write('\n')
        f.write('// include generic header for series\n')
        f.write('#include \"STM8AF_STM8S.h\"\n')
        # STM8AF626x


    # STM8AF52Ax   automotive high density devices with CAN
    # datasheet number: xxx
    # reference manual number: RM0016
    elif device.find('STM8AF52A') == 0:
        f.write('// define device line\n')
        f.write('#if !defined(STM8AF52Ax)\n')
        f.write('  #define STM8AF52Ax\n')
        f.write('#endif\n')
        f.write('\n')
        f.write('// device specific base addresses\n')
        f.write('#define OPT_BaseAddress         0x4800\n')
        f.write('#define PORTA_BaseAddress       0x5000\n')
        f.write('#define PORTB_BaseAddress       0x5005\n')
        f.write('#define PORTC_BaseAddress       0x500A\n')
        f.write('#define PORTD_BaseAddress       0x500F\n')
        f.write('#define PORTE_BaseAddress       0x5014\n')
        f.write('#define PORTF_BaseAddress       0x5019\n')
        f.write('#define PORTG_BaseAddress       0x501E\n')
        f.write('#define PORTH_BaseAddress       0x5023\n')
        f.write('#define PORTI_BaseAddress       0x5028\n')
        f.write('#define FLASH_BaseAddress       0x505A\n')
        f.write('#define EXTI_BaseAddress        0x50A0\n')
        f.write('#define RST_BaseAddress         0x50B3\n')
        f.write('#define CLK_BaseAddress         0x50C0\n')
        f.write('#define WWDG_BaseAddress        0x50D1\n')
        f.write('#define IWDG_BaseAddress        0x50E0\n')
        f.write('#define AWU_BaseAddress         0x50F0\n')
        f.write('#define BEEP_BaseAddress        0x50F3\n')
        f.write('#define SPI_BaseAddress         0x5200\n')
        f.write('#define I2C_BaseAddress         0x5210\n')
        f.write('#define UART1_BaseAddress       0x5230\n')
        #f.write('#define UART2_BaseAddress       0x5240\n')
        f.write('#define UART3_BaseAddress       0x5240\n')
        #f.write('#define UART4_BaseAddress       0x5230\n')
        f.write('#define TIM1_BaseAddress        0x5250\n')
        f.write('#define TIM2_BaseAddress        0x5300\n')
        f.write('#define TIM3_BaseAddress        0x5320\n')
        f.write('#define TIM4_BaseAddress        0x5340\n')
        #f.write('#define TIM5_BaseAddress        0x5300\n')
        #f.write('#define TIM6_BaseAddress        0x5340\n')
        #f.write('#define ADC1_BaseAddress        0x53E0\n')
        f.write('#define ADC2_BaseAddress        0x5400\n')
        f.write('#define CAN_BaseAddress         0x5420\n')
        f.write('#define CFG_BaseAddress         0x7F60\n')
        f.write('#define ITC_BaseAddress         0x7F70\n')
        #f.write('#define DM_BaseAddress          0x7F90\n')
        f.write('\n')
        f.write('// unique ID start address\n')
        f.write('#define UID_BaseAddress         xxx\n')
        f.write('\n')
        f.write('// include generic header for series\n')
        f.write('#include \"STM8AF_STM8S.h\"\n')
        # STM8AF52Ax


    # STM8AF62Ax   automotive high density devices without CAN
    # datasheet number: xxx
    # reference manual number: RM0016
    elif device.find('STM8AF62A') == 0:
        f.write('// define device line\n')
        f.write('#if !defined(STM8AF62Ax)\n')
        f.write('  #define STM8AF62Ax\n')
        f.write('#endif\n')
        f.write('\n')
        f.write('// device specific base addresses\n')
        f.write('#define OPT_BaseAddress         0x4800\n')
        f.write('#define PORTA_BaseAddress       0x5000\n')
        f.write('#define PORTB_BaseAddress       0x5005\n')
        f.write('#define PORTC_BaseAddress       0x500A\n')
        f.write('#define PORTD_BaseAddress       0x500F\n')
        f.write('#define PORTE_BaseAddress       0x5014\n')
        f.write('#define PORTF_BaseAddress       0x5019\n')
        f.write('#define PORTG_BaseAddress       0x501E\n')
        f.write('#define PORTH_BaseAddress       0x5023\n')
        f.write('#define PORTI_BaseAddress       0x5028\n')
        f.write('#define FLASH_BaseAddress       0x505A\n')
        f.write('#define EXTI_BaseAddress        0x50A0\n')
        f.write('#define RST_BaseAddress         0x50B3\n')
        f.write('#define CLK_BaseAddress         0x50C0\n')
        f.write('#define WWDG_BaseAddress        0x50D1\n')
        f.write('#define IWDG_BaseAddress        0x50E0\n')
        f.write('#define AWU_BaseAddress         0x50F0\n')
        f.write('#define BEEP_BaseAddress        0x50F3\n')
        f.write('#define SPI_BaseAddress         0x5200\n')
        f.write('#define I2C_BaseAddress         0x5210\n')
        f.write('#define UART1_BaseAddress       0x5230\n')
        #f.write('#define UART2_BaseAddress       0x5240\n')
        f.write('#define UART3_BaseAddress       0x5240\n')
        #f.write('#define UART4_BaseAddress       0x5230\n')
        f.write('#define TIM1_BaseAddress        0x5250\n')
        f.write('#define TIM2_BaseAddress        0x5300\n')
        f.write('#define TIM3_BaseAddress        0x5320\n')
        f.write('#define TIM4_BaseAddress        0x5340\n')
        #f.write('#define TIM5_BaseAddress        0x5300\n')
        #f.write('#define TIM6_BaseAddress        0x5340\n')
        #f.write('#define ADC1_BaseAddress        0x53E0\n')
        f.write('#define ADC2_BaseAddress        0x5400\n')
        #f.write('#define CAN_BaseAddress         0x5420\n')
        f.write('#define CFG_BaseAddress         0x7F60\n')
        f.write('#define ITC_BaseAddress         0x7F70\n')
        #f.write('#define DM_BaseAddress          0x7F90\n')
        f.write('\n')
        f.write('// unique ID start address\n')
        f.write('#define UID_BaseAddress         xxx\n')
        f.write('\n')
        f.write('// include generic header for series\n')
        f.write('#include \"STM8AF_STM8S.h\"\n')
        # STM8AF62Ax


    # STM8S003x    value line low density device
    # datasheet number: xxx
    # reference manual number: xxx
    elif device.find('STM8S003') == 0:
        f.write('// define device line\n')
        f.write('#if !defined(STM8S003)\n')
        f.write('  #define STM8S003\n')
        f.write('#endif\n')
        f.write('\n')
        f.write('// device specific base addresses\n')
        f.write('#define OPT_BaseAddress         0x4800\n')
        f.write('#define PORTA_BaseAddress       0x5000\n')
        f.write('#define PORTB_BaseAddress       0x5005\n')
        f.write('#define PORTC_BaseAddress       0x500A\n')
        f.write('#define PORTD_BaseAddress       0x500F\n')
        f.write('#define PORTE_BaseAddress       0x5014\n')
        f.write('#define PORTF_BaseAddress       0x5019\n')
        #f.write('#define PORTG_BaseAddress       0x501E\n')
        #f.write('#define PORTH_BaseAddress       0x5023\n')
        #f.write('#define PORTI_BaseAddress       0x5028\n')
        f.write('#define FLASH_BaseAddress       0x505A\n')
        f.write('#define EXTI_BaseAddress        0x50A0\n')
        f.write('#define RST_BaseAddress         0x50B3\n')
        f.write('#define CLK_BaseAddress         0x50C0\n')
        f.write('#define WWDG_BaseAddress        0x50D1\n')
        f.write('#define IWDG_BaseAddress        0x50E0\n')
        f.write('#define AWU_BaseAddress         0x50F0\n')
        f.write('#define BEEP_BaseAddress        0x50F3\n')
        f.write('#define SPI_BaseAddress         0x5200\n')
        f.write('#define I2C_BaseAddress         0x5210\n')
        f.write('#define UART1_BaseAddress       0x5230\n')
        #f.write('#define UART2_BaseAddress       0x5240\n')
        #f.write('#define UART3_BaseAddress       0x5240\n')
        #f.write('#define UART4_BaseAddress       0x5230\n')
        f.write('#define TIM1_BaseAddress        0x5250\n')
        f.write('#define TIM2_BaseAddress        0x5300\n')
        #f.write('#define TIM3_BaseAddress        0x5320\n')
        f.write('#define TIM4_BaseAddress        0x5340\n')
        #f.write('#define TIM5_BaseAddress        0x5300\n')
        #f.write('#define TIM6_BaseAddress        0x5340\n')
        f.write('#define ADC1_BaseAddress        0x53E0\n')
        #f.write('#define ADC2_BaseAddress        0x5400\n')
        #f.write('#define CAN_BaseAddress         0x5420\n')
        f.write('#define CFG_BaseAddress         0x7F60\n')
        f.write('#define ITC_BaseAddress         0x7F70\n')
        #f.write('#define DM_BaseAddress          0x7F90\n')
        f.write('\n')
        f.write('// unique ID start address\n')
        f.write('#define UID_BaseAddress         xxx\n')
        f.write('\n')
        f.write('// include generic header for series\n')
        f.write('#include \"STM8AF_STM8S.h\"\n')
        # STM8S003x


    # STM8S005x    value line medium density device
    # datasheet number: xxx
    # reference manual number: xxx
    elif device.find('STM8S005') == 0:
        f.write('// define device line\n')
        f.write('#if !defined(STM8S005)\n')
        f.write('  #define STM8S005\n')
        f.write('#endif\n')
        f.write('\n')
        f.write('// device specific base addresses\n')
        f.write('#define OPT_BaseAddress         0x4800\n')
        f.write('#define PORTA_BaseAddress       0x5000\n')
        f.write('#define PORTB_BaseAddress       0x5005\n')
        f.write('#define PORTC_BaseAddress       0x500A\n')
        f.write('#define PORTD_BaseAddress       0x500F\n')
        f.write('#define PORTE_BaseAddress       0x5014\n')
        f.write('#define PORTF_BaseAddress       0x5019\n')
        f.write('#define PORTG_BaseAddress       0x501E\n')
        #f.write('#define PORTH_BaseAddress       0x5023\n')
        #f.write('#define PORTI_BaseAddress       0x5028\n')
        f.write('#define FLASH_BaseAddress       0x505A\n')
        f.write('#define EXTI_BaseAddress        0x50A0\n')
        f.write('#define RST_BaseAddress         0x50B3\n')
        f.write('#define CLK_BaseAddress         0x50C0\n')
        f.write('#define WWDG_BaseAddress        0x50D1\n')
        f.write('#define IWDG_BaseAddress        0x50E0\n')
        f.write('#define AWU_BaseAddress         0x50F0\n')
        f.write('#define BEEP_BaseAddress        0x50F3\n')
        f.write('#define SPI_BaseAddress         0x5200\n')
        f.write('#define I2C_BaseAddress         0x5210\n')
        #f.write('#define UART1_BaseAddress       0x5230\n')
        f.write('#define UART2_BaseAddress       0x5240\n')
        #f.write('#define UART3_BaseAddress       0x5240\n')
        #f.write('#define UART4_BaseAddress       0x5230\n')
        f.write('#define TIM1_BaseAddress        0x5250\n')
        f.write('#define TIM2_BaseAddress        0x5300\n')
        f.write('#define TIM3_BaseAddress        0x5320\n')
        f.write('#define TIM4_BaseAddress        0x5340\n')
        #f.write('#define TIM5_BaseAddress        0x5300\n')
        #f.write('#define TIM6_BaseAddress        0x5340\n')
        f.write('#define ADC1_BaseAddress        0x53E0\n')
        #f.write('#define ADC2_BaseAddress        0x5400\n')
        #f.write('#define CAN_BaseAddress         0x5420\n')
        f.write('#define CFG_BaseAddress         0x7F60\n')
        f.write('#define ITC_BaseAddress         0x7F70\n')
        #f.write('#define DM_BaseAddress          0x7F90\n')
        f.write('\n')
        f.write('// unique ID start address\n')
        f.write('#define UID_BaseAddress         xxx\n')
        f.write('\n')
        f.write('// include generic header for series\n')
        f.write('#include \"STM8AF_STM8S.h\"\n')
        # STM8S005x


    # STM8S007x    value line high density device
    # datasheet number: xxx
    # reference manual number: xxx
    elif device.find('STM8S007') == 0:
        f.write('// define device line\n')
        f.write('#if !defined(STM8S007)\n')
        f.write('  #define STM8S007\n')
        f.write('#endif\n')
        f.write('\n')
        f.write('// device specific base addresses\n')
        f.write('#define OPT_BaseAddress         0x4800\n')
        f.write('#define PORTA_BaseAddress       0x5000\n')
        f.write('#define PORTB_BaseAddress       0x5005\n')
        f.write('#define PORTC_BaseAddress       0x500A\n')
        f.write('#define PORTD_BaseAddress       0x500F\n')
        f.write('#define PORTE_BaseAddress       0x5014\n')
        f.write('#define PORTF_BaseAddress       0x5019\n')
        f.write('#define PORTG_BaseAddress       0x501E\n')
        f.write('#define PORTH_BaseAddress       0x5023\n')
        f.write('#define PORTI_BaseAddress       0x5028\n')
        f.write('#define FLASH_BaseAddress       0x505A\n')
        f.write('#define EXTI_BaseAddress        0x50A0\n')
        f.write('#define RST_BaseAddress         0x50B3\n')
        f.write('#define CLK_BaseAddress         0x50C0\n')
        f.write('#define WWDG_BaseAddress        0x50D1\n')
        f.write('#define IWDG_BaseAddress        0x50E0\n')
        f.write('#define AWU_BaseAddress         0x50F0\n')
        f.write('#define BEEP_BaseAddress        0x50F3\n')
        f.write('#define SPI_BaseAddress         0x5200\n')
        f.write('#define I2C_BaseAddress         0x5210\n')
        f.write('#define UART1_BaseAddress       0x5230\n')
        #f.write('#define UART2_BaseAddress       0x5240\n')
        f.write('#define UART3_BaseAddress       0x5240\n')
        #f.write('#define UART4_BaseAddress       0x5230\n')
        f.write('#define TIM1_BaseAddress        0x5250\n')
        f.write('#define TIM2_BaseAddress        0x5300\n')
        f.write('#define TIM3_BaseAddress        0x5320\n')
        f.write('#define TIM4_BaseAddress        0x5340\n')
        #f.write('#define TIM5_BaseAddress        0x5300\n')
        #f.write('#define TIM6_BaseAddress        0x5340\n')
        #f.write('#define ADC1_BaseAddress        0x53E0\n')
        f.write('#define ADC2_BaseAddress        0x5400\n')
        #f.write('#define CAN_BaseAddress         0x5420\n')
        f.write('#define CFG_BaseAddress         0x7F60\n')
        f.write('#define ITC_BaseAddress         0x7F70\n')
        #f.write('#define DM_BaseAddress          0x7F90\n')
        f.write('\n')
        f.write('// unique ID start address\n')
        f.write('#define UID_BaseAddress         xxx\n')
        f.write('\n')
        f.write('// include generic header for series\n')
        f.write('#include \"STM8AF_STM8S.h\"\n')
        # STM8S007x


    # device not yet supported
    else:
        logging.error('device ' + device + ' from unknown line')
        exit(1)

    f.write('\n')
    f.write('/*-----------------------------------------------------------------------------\n')
    f.write('END OF MODULE DEFINITION FOR MULTIPLE INLUSION\n')
    f.write('-----------------------------------------------------------------------------*/\n')
    f.write('#endif // _' + device + '_H\n')

    # close file
    f.close()

    return;
    # save_header



# set verbosity level for output (CRITICAL, ERROR, WARNING, INFO, DEBUG, NOTSET)
logger = logging.getLogger()
hdlr = logging.FileHandler('./export.log')
logger.addHandler(hdlr) 
logger.setLevel(logging.INFO)

# for each device create header file with same name with (some) properties  
num_ok       = 0
num_inactive = 0
num_error    = 0
for idx in range(num_parts):

    # check status of support
    state = check_device( idx )

    # device in list is not active -> skip
    if state == 'inactive':
        num_inactive = num_inactive + 1
        logging.debug(part.iloc[idx] + ' not active, skip')

    # device already supported -> export header file
    elif state == 'supported':
        num_ok = num_ok + 1
        logging.info('export ' + part.iloc[idx])
        save_header(idx)
    
    # device not yet supported -> skip
    else:
        num_error = num_error + 1
        logging.debug(part.iloc[idx] + ' not yet supported, skip')
        #if part.iloc[idx].find('STM8AF') ==0:
        #    logging.debug(part.iloc[idx] + ' not yet supported, skip')


# print message
print('export finished: ' + str(num_ok) + ' ok, ' + str(num_inactive) + ' inactive, ' + str(num_error) + ' failed\n\n')




