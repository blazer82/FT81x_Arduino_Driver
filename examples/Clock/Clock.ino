/**
 * FT81x on ST7701S Arduino Driver
 *
 * MIT License
 *
 * Copyright (c) 2020 Raphael Stäbler
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

#include "FT81x.h"

#if defined(ESP32)
FT81x ft81x = FT81x(5, 17, 16);  // NodeMCU-32 pin configuration
#else
FT81x ft81x = FT81x(10, 9, 8);  // Arduino default pin configuration
#endif

void setup() {
    SPI.begin();
    ft81x.begin();

    ft81x.setRotation(FT81x_ROTATE_LANDSCAPE);

    ft81x.beginDisplayList();
    ft81x.clear(FT81x_COLOR_RGB(0, 0, 0));
    ft81x.swapScreen();
}

void loop() {
    const uint32_t uptime = millis();
    const uint32_t seconds = uptime / 10;
    const uint32_t minutes = seconds / 60;
    const uint32_t hours = minutes / 60;

    const uint16_t clockSeconds = (uint16_t)(seconds % 60);
    const uint16_t clockMinutes = (uint16_t)(minutes % 60);
    const uint16_t clockHours = (uint16_t)(hours % 12);

    ft81x.beginDisplayList();
    ft81x.clear(FT81x_COLOR_RGB(0, 0, 0));
    ft81x.drawClock(240, 240, 200, FT81x_COLOR_RGB(0, 0, 0), FT81x_COLOR_RGB(0xFF, 0xFF, 0xDD), FT81x_OPT_FLAT | FT81x_OPT_NOHANDS, 0, 0, 0);
    ft81x.drawClock(240, 240, 200, FT81x_COLOR_RGB(0x00, 0x6A, 0x71), FT81x_COLOR_RGB(0, 0, 0), FT81x_OPT_NOBACK | FT81x_OPT_NOTICKS, clockHours, clockMinutes, clockSeconds);
    ft81x.swapScreen();
}
