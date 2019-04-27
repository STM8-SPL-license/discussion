# Proposal for free STM8 Device Headers

## Files

- Folder [stm8](https://github.com/STM8-SPL-license/discussion/tree/master/Header/stm8) contains (some) STM8 device headers for review

- Folder [examples](https://github.com/STM8-SPL-license/discussion/tree/master/Header/examples/stm8af_stm8s) contains SDCC, Cosmic, Raisonance and IAR projects:
  - several variants of the blink example with:
    - with and without timer interrupts
    - with and without additional SPL calls
  - UART projects with and without printf/gets
  - ADC projects 
  - PWM output project 
  - Flash write/read project

- Reference via Doxygen
  - HTML under [doxygen/html/index.html](https://github.com/STM8-SPL-license/discussion/tree/master/Header/doxygen/html/index.html)
  - PDF under [doxygen/refman.pdf](https://github.com/STM8-SPL-license/discussion/blob/master/Header/doxygen/refman.pdf)

- File [generate_header_STM8AF_STM8S.py](https://github.com/STM8-SPL-license/discussion/blob/master/Header/generate_header_STM8AF_STM8S.py)
  - imports a product list downloaded from [STM Homepage](https://www.st.com/content/st_com/en/products/microcontrollers/stm8-8-bit-mcus.html)
  - extracts names, descriptions and memory sizes for STM8S/STM8AF devices
  - imports a (manually created) family list with peripherals "STM8AF_STM8S_Peripherals.xlsx"
  - for each supported device, export a header with the implemented peripherals & memory, and import the respective family header
  - extension to other series, e.g. STM8L or STM8AL, is pending


## Background

Practically I combined my existing headers from [STM8_templates](https://github.com/gicking/STM8_templates) with the above Python script


## Limitations

- Of the STM8 (137) + STLUX (4) + STNRG (3) + STM8SPLNB (1) devices in the STM product lists, the current status is:
  - STM8AFxx: 30 supported, 0 pending
  - STM8Sxx: 40 supported, 0 pending
  - STM8L10xx: 6 supported, 0 pending
  - STM8TL5xx: 5 supported, 0 pending
  - STM8ALxx: 0 supported, 22 pending
  - STM8L15xx: 0 supported, 27 pending
  - STM8L16xx: 0 supported, 2 pending
  - STM8L05xx: 0 supported, 4 pending
  - STM8L001x: 0 supported, 1 pending
  - STLUXxx: 4 implemented, 0 pending
  - STNRGxx: 3 implemented, 0 pending
  - STM8SPLNB: 0 implemented, 1 pending
  - Overall: 88 implemented, 57 pending
- My Raisonance demo license has expired, and cannot be "refreshed" by re-installing. Therefore testing with Ride is currently not possible
- This is currently only a proposal with minimal testing, namely via the above examples 

