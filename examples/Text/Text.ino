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

const static char text[] = "The quick, brown fox jumps over a lazy dog.\0";

void setup() {
    SPI.begin();
    ft81x.begin();

    ft81x.setRotation(FT81x_ROTATE_LANDSCAPE);

    ft81x.beginDisplayList();
    ft81x.clear(FT81x_COLOR_RGB(0, 0, 0));
    ft81x.swapScreen();
}

void loop() {
    static char text_buffer[16];
    static uint8_t fontIndex = 0;

    const uint8_t font = fontIndex + 16;

    sprintf(text_buffer, "Font %d", font);

    ft81x.beginDisplayList();
    ft81x.clear(FT81x_COLOR_RGB(0, 0, 0));
    ft81x.drawText(240, 200, 16, FT81x_COLOR_RGB(255, 255, 255), FT81x_OPT_CENTER, text_buffer);
    ft81x.drawText(240, 240, font, FT81x_COLOR_RGB(50, 255, 50), FT81x_OPT_CENTER, text);
    ft81x.swapScreen();

    fontIndex++;
    fontIndex %= 19;

    delay(500);
}
