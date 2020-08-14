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
    Serial.begin(9600);

    Serial.println("Initialize SPI...");
    SPI.begin();

    Serial.print("SPI clock set to: ");
    Serial.println(FT81x_SPI_CLOCK_SPEED);

    Serial.println("");

    Serial.println("Initialize FT81x and display...");
    ft81x.begin();

    Serial.println("");

    Serial.println("Read chip ID...");

    const uint32_t chipID = ft81x.read32(0x0C0000);

    Serial.print("0x0C0000: ");
    Serial.print(chipID & 0xFF, HEX);
    Serial.println(" (supposed to be 0x8)");

    Serial.print("0x0C0001: ");
    Serial.print((chipID >> 8) & 0xFF, HEX);
    Serial.println(" (supposed to be 0x12 or 0x13)");

    Serial.print("0x0C0002: ");
    Serial.print((chipID >> 16) & 0xFF, HEX);
    Serial.println(" (supposed to be 0x1)");

    Serial.print("0x0C0003: ");
    Serial.print((chipID >> 24) & 0xFF, HEX);
    Serial.println(" (supposed to be 0x0)");

    Serial.println("");

    Serial.println("Read FT81x configuration...");

    Serial.print("REG_ID ");
    Serial.print(ft81x.read8(FT81x_REG_ID), HEX);
    Serial.println(" (supposed to be 0x7C)");

    Serial.print("REG_HCYCLE ");
    Serial.print(ft81x.read16(FT81x_REG_HCYCLE));
    Serial.println(" (supposed to be 548)");

    Serial.print("REG_HSIZE ");
    Serial.print(ft81x.read16(FT81x_REG_HSIZE));
    Serial.println(" (supposed to be 480)");

    Serial.print("REG_VCYCLE ");
    Serial.print(ft81x.read16(FT81x_REG_VCYCLE));
    Serial.println(" (supposed to be 518)");

    Serial.print("REG_VSIZE ");
    Serial.print(ft81x.read16(FT81x_REG_VSIZE));
    Serial.println(" (supposed to be 480)");

    Serial.println("");

    Serial.println("Read display parameters...");

    Serial.print("Power mode: ");
    Serial.print(ft81x.queryDisplay(ST7701_RDDPM), HEX);
    Serial.println(" (supposed to be 0x9C)");
}

void loop() {
    // do nothing
}
