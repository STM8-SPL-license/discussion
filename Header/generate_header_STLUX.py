#!/usr/bin/python3
# -*- coding: utf-8 -*-
'''
  Utility to generate STLUX device headers from Excel device list downloaded from the STM homepage,
  and a corresponding peripherals list, manually created from respective datasheets.

  Copyright (C) 2019 Georg Icking-Konert

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.


  **history**

    - 2019-02-10 (v0.1.0) initial version


  **references**

    - `STLUX product list download <https://www.st.com/en/power-management/digital-power-control-drivers.html>`_

'''

# import required modules
import os, sys, shutil
import argparse
import logging
import pandas


# print disclaimer
print('')
print(sys.argv[0] + ', a small utility to generate STLUX device headers from an')
print('Excel device list downloaded from the STM homepage, and a corresponding')
print('peripherals list, manually created from respective datasheets.')
print('')
print('Copyright (C) 2019  Georg Icking-Konert')
print('')
print('This program comes with ABSOLUTELY NO WARRANTY!')
print('This is free software, and you are welcome to redistribute it')
print('under certain conditions; see source code for details.')
print('')


#-------------------------------------------------------------------
# STLUX Product List container
#-------------------------------------------------------------------
class ProductList:
  """ Class for storing the product list as downloaded from STM homepage.

  An instance of product list class, which stores and evaluates the devices as downloaded from the
  `STLUX product list download page <https://www.st.com/en/power-management/digital-power-control-drivers.html>`_

  :param filename:   Name of Excel (.xlsx) to download

  :return:           product list object

  """

  #########
  # constructor
  #########
  def __init__(self, filename):
    """ Create an object containing the product list as downloaded from STM homepage.

    Create an object containing the STLUX product list as downloaded from the
    `STLUX product list download page <https://www.st.com/en/power-management/digital-power-control-drivers.html>`_

    :param filename:   Name of Excel (.xlsx) to import

    :return:           device list object

    """
    self.filename = filename

    # import Excel device list as downloaded from https://www.st.com/en/power-management/digital-power-control-drivers.html
    # table structure:
    #  pars start in line 6 (=index 5)
    #  columns indices:
    #    0: Part Number
    #    1: General Description
    #    2: Marketing Status
    #    3: Package
    #    4: General Description
    #    5: Supply Voltage (V) min
    #    6: Supply Voltage (V) max
    #    7: Operating Temperature (°C) min
    #    8: Operating Temperature (°C) max
    #    9: SMED Number nom
    #   10: Hardware DALI
    #   11: A/D Converter
    #   12: Comparator
    #   13: FLASH Size (kB) (Prog)
    #   14: RAM Size (kB)
    #   15: EEPROM Size (kB) nom
    #   16: Other Functions
    self.list = pandas.read_excel(self.filename, sheet_name=0)
    start_line = 5
    self.part        = self.list.iloc[start_line:, 0]  # device name for header file name
    self.numParts    = len(self.part.iloc[:])          # number of devices
    self.description = self.list.iloc[start_line:, 1]  # comment in header
    self.status      = self.list.iloc[start_line:, 2]  # status = Active, Proposal
    self.flash_size  = self.list.iloc[start_line:, 13] # for flash memory size
    self.RAM_size    = self.list.iloc[start_line:, 14] # for RAM memory size
    self.EEPROM_size = self.list.iloc[start_line:, 15] # for EEPROM memory size

    # get number of active devices
    numActive = 0
    for idx in range(self.numParts):
      if self.getStatus(self.part.iloc[idx]) == 'Active':
        numActive = numActive + 1

    # print import summary
    print('read total ' + str(self.numParts) + ', ' + str(numActive) + ' active STLUX devices from \'' + self.filename + '\'')

  # __init__
  ##################

  #########
  # Get part index for device
  #########
  def getIdxPart(self, device):
    for idx in range(self.numParts):
      if self.part.iloc[idx].find(device) == 0:
        return idx
    return -1

  #########
  # Get description
  #########
  def getDescription(self, device):
    return self.description.iloc[self.getIdxPart(device)]

  #########
  # Get production status
  #########
  def getStatus(self, device):
    return self.status.iloc[self.getIdxPart(device)]

  #########
  # Get flash size
  #########
  def getFlashSize(self, device):
    return self.flash_size.iloc[self.getIdxPart(device)]

  #########
  # Get RAM size
  #########
  def getRAMSize(self, device):
    return self.RAM_size.iloc[self.getIdxPart(device)]

  #########
  # Get EEPROM size
  #########
  def getEEPROMSize(self, device):
    return self.EEPROM_size.iloc[self.getIdxPart(device)]

# class ProductList
##################



#-------------------------------------------------------------------
# STLUX Peripheral list container
#-------------------------------------------------------------------
class PeripheralList:
  """ Class for storing the peripherals for devices, as derived from datasheets.

  An instance of peripherals list class, which stores and evaluates the devices as extracted form the
  device datasheets downloaded from `STM homepahe <https://www.st.com>`_

  :param filename:   Name of Excel (.xlsx) to download

  :return:           product list object

  """

  #########
  # constructor
  #########
  def __init__(self, filename):
    """ Create an object containing the product list as downloaded from STM homepage.

    Create an object containing the STLUX product list as downloaded from the
    `STLUX product list download page <https://www.st.com/en/power-management/digital-power-control-drivers.html>`_

    :param filename:   Name of Excel (.xlsx) to import

    :return:           peripherals list object

    """
    self.filename = filename

    # import Excel peripherals list
    # table structure is interpreted on the go
    self.Devices  = pandas.read_excel(self.filename, sheet_name="Devices",  header=0)
    self.Families = pandas.read_excel(self.filename, sheet_name="Families", header=0)
    self.name     = self.Devices.iloc[0:, 0]    # device names  -> define
    self.family   = self.Devices.iloc[0:, 1]    # device family -> define and header name
    self.numParts = len(self.name.iloc[:])      # number of devices
    self.include  = self.Families.iloc[0:, 0]   # family -> base addresses
    self.numFamilies = len(self.include.iloc[:])      # number of families
    self.peripherals = self.Families.columns[2:]
    self.numPeripherals = len(self.peripherals)      # number of peripherals

    # print import summary
    print('read ' + str(self.numParts) + ' devices, ' + str(self.numFamilies) + ' families from \'' + self.filename + '\'')

  # __init__
  ##################

  #########
  # Get device index in Devices list
  #########
  def getIdxName(self, device):
    for idx in range(self.numParts):
      if self.name.iloc[idx].find(device) == 0:
        return idx
    return -1

  #########
  # Get family index in Families list
  #########
  def getIdxFamily(self, device):
    for idx in range(self.numFamilies):
      if self.include.iloc[idx].find(device) == 0:
        return idx
    return -1

  #########
  # Get family peripherals
  #########
  def getAddresses(self, family):
    idx = self.getIdxFamily(family)
    return self.Families.iloc[idx, 2:]

  #########
  # Get device peripherals
  #########
  def getDevice(self, device):
    idx     = self.getIdxName(device)
    family  = self.family[idx]
    addr    = self.getAddresses(family)
    module  = []
    address = []
    for idx in range(len(addr)):
      if not str(addr[idx]) == 'nan':
        module.append(str(peripherals.peripherals[idx]))
        address.append(str(addr[idx]))
    return family, module, address


# class PeripheralList
##################



'''
  save device header file
'''
def saveDeviceHeader(device):

  # print optional message
  logging.warning('export %s.h', device)

  # get device info
  description = devices.getDescription(device)
  flash_size  = devices.getFlashSize(device)
  RAM_size    = devices.getRAMSize(device)
  EEPROM_size = devices.getEEPROMSize(device)
  family, module, address = peripherals.getDevice(device)

  # convert memory sizes to byte
  flash_size = int(float(flash_size) * 1024)
  RAM_size = int(float(RAM_size) * 1024)
  if str(EEPROM_size).isdigit():
    EEPROM_size = int(float(EEPROM_size) * 1024)
  else:
    EEPROM_size = 0

  # create new file
  f = open(directory + '/' + device+'.h','w+')

  # write file header
  f.write('/** \\addtogroup STLUX_STNRG\n')
  f.write('*  @{\n')
  f.write('*/\n')
  f.write('\n')
  f.write('/*-------------------------------------------------------------------------\n')
  f.write('  ' + device +'.h - Register Declarations for STM ' + device + '\n')
  f.write('\n')
  f.write('  ' + description + '\n')
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
  f.write('#ifndef ' + device + '_H\n')
  f.write('#define ' + device + '_H\n')
  f.write('\n')

  f.write('/* define device & family */\n')
  f.write('#if !defined(' + device + ')\n')
  f.write('  #define ' + device + '\n')
  f.write('#endif\n')
  f.write('#if !defined(' + family + ')\n')
  f.write('  #define ' + family + '\n')
  f.write('#endif\n')
  f.write('\n')

  f.write('/* device memory sizes [B] */\n')
  f.write('#define STM8_PFLASH_SIZE ' + str(flash_size) + '\n')
  f.write('#define STM8_RAM_SIZE    ' + str(RAM_size) + '\n')
  f.write('#define STM8_EEPROM_SIZE ' + str(EEPROM_size) + '\n')
  f.write('\n')

  f.write('/* define device base addresses */\n')
  for idx in range(len(address)):
    line_new = '#define  {:>6}_AddressBase  {:>8}'.format(module[idx], address[idx])
    f.write(line_new + '\n')

  f.write('\n')
  f.write('/* include generic header for series */\n')
  f.write('#include \"STLUX_STNRG.h\"\n')

  f.write('\n')
  f.write('/*-----------------------------------------------------------------------------\n')
  f.write('END OF MODULE DEFINITION FOR MULTIPLE INLUSION\n')
  f.write('-----------------------------------------------------------------------------*/\n')
  f.write('#endif // _' + device + '_H\n')
  f.write('\n')
  f.write('/** @}*/\n')
  f.write('\n')

  # close file
  f.close()

  return;

  # saveDeviceHeader
  ##################



# commandline parameters with defaults
parser = argparse.ArgumentParser(description="STLUX header creator")
parser.add_argument('-d', '--devices',     type=str,   help='Product List Excel', required=False, default='ProductsList_STLUX.xlsx')
parser.add_argument('-p', '--peripherals', type=str,   help='Peripherals Excel',  required=False, default='Peripherals_STLUX.xlsx')
args = parser.parse_args()

# create new output folder for headers (avoid left-overs)
directory = 'stm8/stlux_stnrg'
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
    shutil.copyfile('STLUX_STNRG.h', directory + '/STLUX_STNRG.h')
except OSError:
    print('cannot copy family headers to \'' + directory + '\', exit!')
    exit(1);


# set verbosity level for output (CRITICAL, ERROR, WARNING, INFO, DEBUG, NOTSET)
logger = logging.getLogger()
hdlr = logging.FileHandler('./export_stlux.log')
logger.addHandler(hdlr)
logger.setLevel(logging.INFO)


# import product list and corresponding peripherals list
devices     = ProductList(args.devices)
peripherals = PeripheralList(args.peripherals)

numExport = 0
numSkip = 0
for part in devices.part:
  if peripherals.getIdxName(part) >= 0:
    #print(part)
    saveDeviceHeader(part)
    numExport = numExport + 1
  else:
    # print optional message
    logging.warning('skip %s', part)
    numSkip = numSkip + 1


print('\nfinished: exported ' + str(numExport) + ' headers, skipped ' + str(numSkip) + ' devices\n')
