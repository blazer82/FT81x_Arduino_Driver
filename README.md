# FT81x Arduino Driver

[![build-badge]](https://github.com/blazer82/FT81x_Arduino_Driver/actions?workflow=build)
[![docs-badge]](https://github.com/blazer82/FT81x_Arduino_Driver/actions?workflow=docs)

High quality display module for many Arduino project ideas!

## Hardware

![Hardware](https://raw.githubusercontent.com/blazer82/FT81x_Arduino_Driver/master/assets/hardware.jpg)

Schematics, KiCad files and Gerber files can be found inside the [hardware folder](hardware) in this repository.

The board is compatible with this [display](https://www.buydisplay.com/4-inch-tft-lcd-display-480x480-pixel-with-mipi-interface-for-iot-devices).

The complete hardware is [available for sale on Tindie](https://www.tindie.com/products/prodbld/40-inch-ft81x-tft-lcd-display-module-for-arduino/).

## Highlights

- 4 inch 480x480 pixel TFT display
- up to 24 bit color support
- built-in fonts and widgets (e.g. buttons, gauges, clock)
- hardware support for JPEG and PNG
- built-in sound effects (audio circuitry required)
- bitmap support with 1 MByte graphics RAM
- open source Arduino library with many example sketches
- compatible with 3.3V and 5V logic levels

## Supported Boards

The hardware and software has been tested with the following boards:

- :heavy_check_mark: Arduino Uno (ATmega328P)
- :heavy_check_mark: Arduino Nano (ATmega328)
- :heavy_check_mark: Arduino Nano Every (ATMega4809)
- :heavy_check_mark: Arduino Due (AT91SAM3X8E)
- :heavy_check_mark: NodeMCU-32S (ESP32)
- :heavy_check_mark: Teensy 4.0 (ARM Cortex M7)

If your board is missing and has full Arduino support chances are that it's fully supported by this library, too.

If you have tested additional boards (successfully or unsuccessfully) feel free to contact me or open an issue.

## Features

- [x] Line
- [x] Circle
- [x] Rectangle
- [x] Gradient
- [x] Text
- [x] Bitmap images
- [x] Animated spinner
- [x] Sound effects
- [x] Buttons
- [x] Analog clock
- [x] Gauge
- [x] Line strip
- [x] JPEG support
- [x] PNG support
- [ ] Keys
- [ ] Progress bar
- [ ] Scrollbar
- [ ] Slider
- [ ] Dial
- [ ] Toggle
- [ ] Custom fonts
- [ ] Video playback
- [ ] Audio playback

## Usage

```c++
#include "FT81x.h"

FT81x ft81x = FT81x(10, 9, 8);

void setup() {
    SPI.begin();
    ft81x.begin();

    ft81x.beginDisplayList();
    ft81x.clear(FT81x_COLOR_RGB(0, 0, 0));
    ft81x.drawText(240, 200, 31, FT81x_COLOR_RGB(255, 255, 255), FT81x_OPT_CENTER, "Hello World\0");
    ft81x.swapScreen();
}
```

For more information see the numerous [example sketches](examples) as well as the [API Documentation](https://blazer82.github.io/FT81x_Arduino_Driver/html/class_f_t81x.html).

## Pin Description and Hookup Guide

### Board PINs

| PIN # | PIN Name | Description                                      |
| -----:| -------- | ------------------------------------------------ | 
| **1** | **5V**   | Power input (5V, 100mA)                          | 
| **2** | **VCC**  | Reference voltage for logic signals (3.3V to 5V) | 
| **3** | **GND**  | Common ground                                    | 
| **4** | **SCK**  | SPI clock                                        | 
| **5** | **SDO**  | SPI data out (aka MISO)                          | 
| **6** | **SDI**  | SPI data in (aka MOSI)                           | 
| **7** | **CS1**  | SPI chip select 1                                | 
| **8** | **CS2**  | SPI chip select 2                                | 
| **9** | **DC**   | Data/command signal                              | 
| 10    | AUDIO    | Audio output                                     | 
| 11    | IRQ      | Interrupt signal                                 | 
| 12    | GND      | Common ground                                    | 
| 13    | GND      | Common ground                                    | 

***bold** connections are required.*

### Hookup to Common Boards

| PIN Name | Arduino Uno | Arduino Nano | NodeMCU-32S |
| -------- |:-----------:|:------------:|:-----------:|
| **5V**   | 5V          | 5V           | 5V / VIN    |
| **VCC**  | 5V          | 5V           | 3V3         |
| **GND**  | GND         | GND          | GND         |
| **SCK**  | 13          | D13          | GPIO18      |
| **SDO**  | 12          | D12          | GPIO19      |
| **SDI**  | 11          | D11          | GPIO23      |
| **CS1**  | 10          | D10          | GPIO5       |
| **CS2**  | 9           | D9           | GPIO17      |
| **DC**   | 8           | D8           | GPIO16      |

## License Summary

### Software

MIT License

### Hardware

The hardware is licensed under [CERN-OHL-S v2](https://cern.ch/cern-ohl).

[build-badge]: https://github.com/blazer82/FT81x_Arduino_Driver/workflows/build/badge.svg
[docs-badge]: https://github.com/blazer82/FT81x_Arduino_Driver/workflows/docs/badge.svg
