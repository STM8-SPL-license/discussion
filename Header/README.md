# Proposal for free STM8 Device Headers

## Files

- Folder [stm8](https://github.com/STM8-SPL-license/discussion/tree/master/Header/stm8) contains (some) STM8 device headers for review

- Folder [examples](https://github.com/STM8-SPL-license/discussion/tree/master/Header/examples) contains 3 variants of the blink example projects with:
  - byte- and bitwise access to peripherals
  - with and without interrupts
  - with and without additional SPL calls

- File [generate_header.py](https://github.com/STM8-SPL-license/discussion/blob/master/Header/generate_header.py)
  - imports a product list downloaded from [STM Homepage](https://www.st.com/content/st_com/en/products/microcontrollers/stm8-8-bit-mcus.html)
  - extracts names, descriptions and memory sizes
  - for each (known) device exports a header with the implemented peripherals & memory. Also imports the respective generic family header (currently only STM8S/STM8AF) 


## Background

Practically I combined my existing headers from [STM8_templates](https://github.com/gicking/STM8_templates) with the above Python script


## Limitations

- Of the 138 STM8 devices in the STM8 list, currently only 34 are recognized (but growing) 
- This is currently only a proposal with minimal testing, namely the above 3 examples 

