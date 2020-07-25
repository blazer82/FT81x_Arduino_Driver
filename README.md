# FT81x Arduino Driver

[![build-badge]](https://github.com/blazer82/FT81x_Arduino_Driver/actions?workflow=build)
[![docs-badge]](https://github.com/blazer82/FT81x_Arduino_Driver/actions?workflow=docs)

FT81x on ST7701S Arduino Driver

## Hardware

![Board](https://raw.githubusercontent.com/blazer82/FT81x_Arduino_Driver/master/assets/board.png)

Schematics, KiCad files and Gerber files can be found inside the [hardware folder](hardware) in this repository.

The board is compatible with this [4 inch TFT LCD display with 480x480 pixels](https://www.buydisplay.com/4-inch-tft-lcd-display-480x480-pixel-with-mipi-interface-for-iot-devices).

## Tested Boards

- :heavy_check_mark: Arduino Uno (ATmega328P)
- :heavy_check_mark: Arduino Nano (ATmega328)
- :heavy_check_mark: Arduino Nano Every (ATMega4809)
- :heavy_check_mark: Arduino Due (AT91SAM3X8E)
- :heavy_check_mark: NodeMCU-32S (ESP32)
- :heavy_check_mark: Teensy 4.0 (ARM Cortex M7)

## Features

- [x] Circle
- [x] Rectangle
- [x] Text
- [x] Bitmap images
- [x] Animated spinner
- [x] Sound effects
- [x] Buttons
- [x] Analog clock
- [ ] Gauge
- [ ] Keys
- [ ] Progress bar
- [ ] Scrollbar
- [ ] Slider
- [ ] Dial
- [ ] Toggle
- [ ] Custom fonts
- [ ] JPEG support
- [ ] PNG support
- [ ] Video playback
- [ ] Audio playback

## Usage

```c++
#include "FT81x.h"

FT81x ft81x = FT81x(10, 9, 8, 7);

void setup() {
    SPI.begin();
    ft81x.begin();

    ft81x.beginDisplayList();
    ft81x.clear(FT81x_COLOR_RGB(0, 0, 0));
    ft81x.drawText(240, 200, 31, FT81x_COLOR_RGB(255, 255, 255), FT81x_OPT_CENTER, "Hello World\0");
    ft81x.swapScreen();
}
```

For more information see the [example sketches](examples) as well as the [API Documentation](https://blazer82.github.io/FT81x_Arduino_Driver/html/class_f_t81x.html).

## License Summary

### Software

MIT License

### Hardware

The hardware is licensed under [CERN-OHL-S v2](https://cern.ch/cern-ohl).

[build-badge]: https://github.com/blazer82/FT81x_Arduino_Driver/workflows/build/badge.svg
[docs-badge]: https://github.com/blazer82/FT81x_Arduino_Driver/workflows/docs/badge.svg
