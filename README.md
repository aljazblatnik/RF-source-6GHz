# DIY 6 GHz RF signal generator
![Two assembled prototypes](photo/front.JPEG "Two assembled prototypes")
## What's the difference?
You have probably seen this kind of RF signal source devices before. They come as a cheap module from Ebay or Aliexpress, usually with boult in graphical interface on a touch screen. Although they are easy to obtain, most of them lack serious shielding making them unusable in any serious RF measurement setup. 

This prototype is based on a [S53MV design](http://lea.hamradio.si/~s53mv/fpll/fpll.html) from 2017. The design was well tested by students over the past couple of semesters as it became a key building block in the composition of the laboratory practicum. Unfortunately, the original contains some shortcomings as it uses few outdated components that are very hard to get. At the same time all of the (otherwise very good) microcontroller code is written in pure assembly (ARM7TDMI), which is a nightmare for an unskilled engineer to fix.

This project uses up-to-date components that are available from almost all major online retailers, adds power calibration and refreshed GUI look, while maintaining 100% compatibility with the original S53MV design. The microcontroller source code was re-written in pure C using an EmBitz development enviroment and all PCBs have the same dimensions as the original S53MV prototype.
## How to use it?
There are two ways of controlling this RF generator. The first one is with four buttons located on the front panel and the second one is via serial interface (remote commands). Regardless of the method used, all functionalities are available, although controlling via a serial connection opens up the possibility of automation with, for example, a Python script.
#### Front buttons
There is only one menu screen constantely displayed with all information needed for a quick manipulation. You can move the position of a cursor (marked with an underline symbol) with left and right buttons Not all 16 characters on a LCD row can be altered though as only numbers can be changed. Frequency can be adjusted between 0 - 7 GHz with 1 kHz resolution (some settings might not produce successfull PLL lock as MAX2871 is specified only up to 6 GHz). A question mark `?` will appear in upper left if unlock condition is detected. Power setting depends on calibration value and is frequency dependant. Maximum output power at a specific frequency is measured during a calibration process and a maximum of -30 dB attenuation can be inserted (1 dB step size). Optional AM modulation (square wave, 100% depth, 50% duty cycle - specified up to 10 kHz) can be turn on raging from 1 Hz to 50 kHz (1 Hz resolution). Default state is `SW.OFF` meaning CW output with no modulation. The cursor will automaticaly jump to the bottom our upper line when reaching end of the screen.

At the power up the generator starts at a default program 0, denoted with `0` in upper most left position of a screen. Ther are total of 10 different program locations (0-9) with all of them being able to hold its own frequency, amplitude and modulation settings. User can therefore prepare different scenarios and quickly switch between them. All program values are stord in RAM, so power loss or a reset command will erase them. Pressing both left and right button together will save the data from RAM to a flash memory inside MCU. Please keep in mind that STM32F030 series have a limited flash write cycles (typicaly around 1000) after which the memory location will probably fail. Saving is intended only on a occasional basis. Write is performed only if there is a change between the RAM and the Flash memory. On power-up the program memory stored in flash is transfered to a RAM.
#### Serial connection
CH340G is added to the main PCB board for a USB to Serial (TTL) bridge. Virtaul COM port appears on all major operating systems. The default baud rate is 9600bps, no parity, 1 stop bit. Commands are presented below.
| Syntax  | Command description |
| ------------- | ------------- |
| `help`  | Lists out all commands with a brief description |
| `A xxx`  | Amplitude setting in dBm including a `-` or `+` sign  |
| `F xxxxxxx`  | Frequency in kHz  |
| `M xxxxx`  | Modulation in Hz  |
| `S x`  | Select program location (0-9) |
| `P 123456789`  | Save to Flash |
| `R`  | MCU software reset (might not work at hard-fault) |
| `Att xxx`  | Attenuator raw value (used at calibration) |

All commands must end with a `\n` or an `Enter key` press. Please be aware that there is almost no error correction implemented ia a command decode function. Serial commands are ment for automation not the user input via commant prompt. Device can go to Hard fault mode if the wrong command syntax is entered. Power reset will solve the problem.

Upon power-up the baisc diagnostic data is send first. Model `RF source 6 GHz\r\n` and version `v2.1\r\n` followed by a status report `0 ? 6000000kHz 0dBm 0Hz` or `0   5961010kHz -11dBm 11011Hz` (for example) if the PLL managed to lock (no `?` symbol). Firs number denotes selected program, followed by PLL lock status, frequency, output power, and modulation. Status line is automatically send when any of the four front buttons are pressed.

Note that this RF generator is intended to work at CW mode with only limited sweep posibility when doing it do by a remote control. Faster boud rates won't improve sweep times as PLL lock condition isn't instantenious.
## How to construct it?
There are two main PCB boards. RF and MCU section. RF circuit board is constructed on a 0.6 mm double sided FR4 with bottom being unetched to serve as a ground plane. It coud be easily replicated with a DIY home methods eg. toner transfer or photo method. MAX2871 and F1958 are soldered soldered first using a hot air. 3 mm diameter hole is drilled in the middle of the footprint so the ground pad of the device can be soldered to the gorundplane with a thin copper strip and a generous amount of solder. This serves as a good RF connection to ground an heat disipation pad (working better than any vias in profesional design!). Other components al soldered next. The circuit is first tested then put inside a brass housing, covered with a microwave absorber (black foam used as a shipping material of  most IC packaging) and a brass or copper cover. For even better RF shielding you can tape the metal cover with a conductive aluminium tape. Shielding is an important step in preventing RF signals to exit the device anywhere else as out of the front SMA connector.

Requirements for the MCU board are much sparse. It can be constructed on a single layer 1mm or 1.6 mm FR4 and mounted tirectly to the final enclousare withouth the need for a RF shield. Connection to other peripherals are maed with flat-flex cable and standard pin-header ladder to form a cheap connector. Four buttons are mounted on a seperate PCB board.

Everything is enclosed in a home-made aluminium box (1 mm semi-soft sheet). Find the stencil for drilling and bending above. To cover the enclousure a 0.6 mm thich sheet is used. Device casing serves as a heat sink for a linear regulator and can get worm to the touch after an extended period of time.

Detaild construction method is quite similar to the original S53MV design. You can find de procedure description [here](http://lea.hamradio.si/~s53mv/fpll/fpll.html).

All PCBs were designd in SprintLayout 6.0. You can find the free viewer version online. Try printing mask directly from the viewer as other programs might alter dimensions.  

## How it's designed?

### RF block
At the heart of the design lies the now well-known MAX2871 fractional PLL loop with a built-in bank of silicon oscillators. The output signal is amplified by a GVA-62+ amplifier, although we can use any other MMIC component in the same package. The output power is adjusted with the digital attenuator F1958 in a 0 dB to -30 dB range. The reference clock is provided by a 32 MHz crystal, preferably in the form of a TCXO (multiple SMD package sizes fit on the same PCB footprint). The entire circuit is powered from the +5V line. A +3.3V voltage rail is provided by one LD1117S33 (tested for the lowest noise level). FB indicates SMD ferrite bead with the highest resistance @ 100 MHz available to us.
![RF block schematic.](schematic_v2_PLL.png "RF schematic")

### MCU
The STM32F030K6 was chosen as the main MCU because at the time of development it was the cheapest microcontroller with the required features to implement the project. It is controlling a simple user interface on a classic 2x16 symbol LCD screen (version with larger letters) and four buttons (up, down, left, right). It sets the registers of the PLL chip via serial bus, controls the attenuator, and generates AM modulation (100% depth) for the RF output and reference BNC connection at the back of the device (TTL 3.3V). USB to RS-232(TTL) IC CH340G is also added to the same PCB for the remote control via Virtual COM port (see listed commands below).
![MCU schematic.](schematic_v2_MCU.png "MCU schematic")

### Making changes
