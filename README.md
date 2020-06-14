# WE ARE CLOSED

This discussion is obsolete due to the availability of FOSS STM8 device headers under 
[https://github.com/gicking/STM8_headers](https://github.com/gicking/STM8_headers).
Therefore I close this repo/discussion.

Thanks for your valuable input! Have a great day  :-)


# discussion
Discussion of STM8 SPL license status and next steps

There seem to be several parallel discussions with STM about the STM8-SPL license. I set up this repo (hoping this is the correct tool) to 
bring together the involved people I am aware of. Please feel free to participate in the discussion and to invite additional people as required.

In addition, patching the SPL for SDCC (like [here](https://github.com/gicking/STM8-SPL_SDCC_patch)) seems to become inappropriate, as 
Michael Mayer (aka tenbaht) has just released [a tool](https://github.com/tenbaht/spl-splitter) to split the SPL functions into separate 
files for linker optimization. If I publish this version as patch, I might just as well publich the complete SPL... 

Please open the "[Issues](https://github.com/STM8-SPL-license/discussion/issues)" page for the actual discussion - or tell me a better tool 
for a team discussion...  ;-)

Thanks and have a great day,
Georg


## Update 2019-04-00:

I have just updated my open-source STM8 device headers to [Headers/stm8](https://github.com/STM8-SPL-license/discussion/tree/master/Header). 

Changes
- added STLUX & STNRG devices
- fixed minor bugs in other headers
- updated reference for STLUX & STNRG

New status: of the 145 devices with STM8 core, 88 are implemented, 56 are pending and 1 is n/a. For details see [Readme](https://github.com/STM8-SPL-license/discussion/tree/master/Header)


## Update 2019-03-27:

I have just updated my open-source STM8 device headers to [Headers/stm8](https://github.com/STM8-SPL-license/discussion/tree/master/Header). 

Changes
- added STM8TL5x devices
- fixed some bugs in STM8L10x header
- updated reference for STM8TL5x 

Currently all STMAF, STM8S, STML10x and STM8TL5x devices (=81 out of 138) are supported, and 57 are pending 


## Update 2019-03-25:

I have just updated my open-source STM8 device headers to [Headers/stm8](https://github.com/STM8-SPL-license/discussion/tree/master/Header). 

Changes
- added STM8L10x devices
- updated reference for STM8L10x 

Currently all STMAF, STM8S and STML10x devices (=76 out of 138) are supported, and 62 are pending 


## Update 2019-03-23:

I have just updated my open-source STM8 device headers to [Headers/stm8](https://github.com/STM8-SPL-license/discussion/tree/master/Header). 

Changes
- implemented some proposals from the [discussion](https://github.com/STM8-SPL-license/discussion/issues)
- added support for all STM8AF and STM8S devices, and all peripherals
- added HTML reference (via Doxygen) under [Headers/doxygen/html/index.html](https://github.com/STM8-SPL-license/discussion/tree/master/Header/doxygen/html/index.html)
- added PDF reference (via Doxygen) under [Headers/doxygen/refman.pdf](https://github.com/STM8-SPL-license/discussion/blob/master/Header/doxygen/refman.pdf)
- added example projects under [Headers/examples](https://github.com/STM8-SPL-license/discussion/tree/master/Header/examples)

My final goal is still to have these headers bundled into SDCC, in order to facilitate development and code exchange within the STM8 community.
As expected, the development takes a lot of effort (and is rather dull), so please let me know if this is an option. 

Actually I would prefer a different way to support STM8 out of the box, e.g. via SPL headers or another OSS alternative, however I 
understand from our [discussion](https://github.com/STM8-SPL-license/discussion/issues) that this is not to be expected anytime soon...?



## Update 2019-01-27:

I have just uploaded my proposal for such STM8 OS-headers under [Headers/stm8](https://github.com/STM8-SPL-license/discussion/tree/master/Header/stm8). Simple blink example projects can be found under [Headers/examples](https://github.com/STM8-SPL-license/discussion/tree/master/Header/examples). Basically these are 3 variants of the blink-example with:
- byte- and bitwise access to peripherals
- with and without interrupts
- with and without additional SPL calls

Practically I combined my existing headers from [STM8_templates](https://github.com/gicking/STM8_templates) with a new Python script to extract device data from the STM8 product list from [STM Homepage](https://www.st.com/content/st_com/en/products/microcontrollers/stm8-8-bit-mcus.html). Of the 138 STM8 devices in the list, 34 are supported and 104 are pending. 

For the discussion if this is a viable proposal, please see [here](https://github.com/STM8-SPL-license/discussion/issues/1)