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
    ft81x.clear(FT81x_COLOR_RGB(255, 255, 255));
    ft81x.drawText(240, 20, 24, FT81x_COLOR_RGB(0, 0, 0), FT81x_OPT_CENTERX, "Scrollbar example\0");
    ft81x.drawScrollbar(20, 20, 10, 420, FT81x_COLOR_RGB(0, 150, 250), FT81x_COLOR_RGB(0, 200, 250), FT81x_OPT_3D, 80, 20, 100);
    ft81x.drawScrollbar(40, 460, 420, 10, FT81x_COLOR_RGB(0, 150, 250), FT81x_COLOR_RGB(0, 200, 250), FT81x_OPT_3D, 0, 40, 100);
    ft81x.swapScreen();
}

void loop() {
    static uint16_t y = 400;
    static bool direction = 1;

    y = direction ? y - 1 : y + 1;
    direction = y == 0 || y == 400 ? !direction : direction;

    ft81x.beginDisplayList();
    ft81x.clear(FT81x_COLOR_RGB(255, 255, 255));
    ft81x.drawText(240, y + 20, 24, FT81x_COLOR_RGB(0, 0, 0), FT81x_OPT_CENTERX, "Scrollbar example\0");
    ft81x.drawScrollbar(20, 20, 10, 420, FT81x_COLOR_RGB(0, 150, 250), FT81x_COLOR_RGB(0, 200, 250), FT81x_OPT_3D, 400 - y, 100, 500);
    ft81x.drawScrollbar(40, 460, 420, 10, FT81x_COLOR_RGB(0, 150, 250), FT81x_COLOR_RGB(0, 200, 250), FT81x_OPT_3D, 0, 40, 100);
    ft81x.swapScreen();

    delay(10);
}
