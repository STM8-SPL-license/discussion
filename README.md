# discussion
Discussion of STM8 SPL license status and next steps

There seem to be several parallel discussions with STM about the STM8-SPL license. I set up this repo (hoping this is the correct tool) to bring together the involved people I am aware of. Please feel free to participate in the discussion and to invite additional people as required.

In addition, patching the SPL for SDCC (like [here](https://github.com/gicking/STM8-SPL_SDCC_patch)) seems to become inappropriate, as Michael Mayer (aka tenbaht) has just released [a tool](https://github.com/tenbaht/spl-splitter) to split the SPL functions into separate files for linker optimization. If I publish this version as patch, I might just as well publich the complete SPL... 

Please open the "[Issues](https://github.com/STM8-SPL-license/discussion/issues)" page for the actual discussion - or tell me a better tool for a team discussion...  ;-)

Thanks and have a great day,
Georg


## Update 2019-01-27:

I have just uploaded my proposal for such STM8 OS-headers under [Headers/stm8](https://github.com/STM8-SPL-license/discussion/tree/master/Header/stm8). Simple blink example projects can be found under [Headers/examples](https://github.com/STM8-SPL-license/discussion/tree/master/Header/examples). Basically these are 3 variants of the blink-example with:
- byte- and bitwise access to peripherals
- with and without interrupts
- with and without additional SPL calls

Practically I combined my existing headers from [STM8_templates](https://github.com/gicking/STM8_templates) with a new Python script to extract device data from the STM8 product list from [STM Homepage](https://www.st.com/content/st_com/en/products/microcontrollers/stm8-8-bit-mcus.html). Of the 138 STM8 devices in the list, 34 are supported and 104 are pending. 

For the discussion if this is a viable proposal, please see [here](https://github.com/STM8-SPL-license/discussion/issues/1)