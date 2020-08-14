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

const char *menuItems[] = {
    "Dashboard\0",
    "Sensors\0",
    "Settings\0",
    "Turn off\0",
};

void setup() {
    SPI.begin();
    ft81x.begin();

    ft81x.setRotation(FT81x_ROTATE_LANDSCAPE);

    ft81x.beginDisplayList();
    ft81x.clear(FT81x_COLOR_RGB(255, 255, 255));
    ft81x.swapScreen();
}

void loop() {
    for (uint8_t i = 0; i < 4; i++) {
        ft81x.beginDisplayList();
        ft81x.clear(FT81x_COLOR_RGB(255, 255, 255));
        ft81x.drawText(240, 60, 31, FT81x_COLOR_RGB(0, 0, 0), FT81x_OPT_CENTER, "Menu\0");
        for (uint8_t j = 0; j < 4; j++) {
            const uint32_t color = i == j ? FT81x_COLOR_RGB(0, 0, 200) : FT81x_COLOR_RGB(64, 64, 92);
            ft81x.drawButton(140, 140 + 70 * j, 200, 40, 26, FT81x_COLOR_RGB(255, 255, 255), color, FT81x_OPT_3D, menuItems[j]);
        }
        ft81x.swapScreen();
        delay(1000);
    }
}
