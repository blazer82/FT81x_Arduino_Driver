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
    ft81x.clear(FT81x_COLOR_RGB(35, 41, 49));
    ft81x.drawText(240, 190, 24, FT81x_COLOR_RGB(238, 238, 238), FT81x_OPT_CENTERX, "Progressbar demo in progress...\0");
    ft81x.drawProgressbar(60, 240, 420, 10, FT81x_COLOR_RGB(78, 204, 163), FT81x_COLOR_RGB(57, 62, 66), FT81x_OPT_3D, 0, 100);
    ft81x.swapScreen();
}

void loop() {
    const uint16_t progress = (millis() / 100) % 100;

    ft81x.beginDisplayList();
    ft81x.clear(FT81x_COLOR_RGB(35, 41, 49));
    ft81x.drawText(240, 190, 24, FT81x_COLOR_RGB(238, 238, 238), FT81x_OPT_CENTERX, "Progressbar demo in progress...\0");
    ft81x.drawProgressbar(60, 240, 360, 10, FT81x_COLOR_RGB(78, 204, 163), FT81x_COLOR_RGB(57, 62, 66), FT81x_OPT_3D, progress, 100);
    ft81x.swapScreen();
}
