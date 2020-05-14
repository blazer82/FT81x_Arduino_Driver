/**
 * FT81x on ST7701S Arduino Driver
 * Copyright (C) 2020  Raphael Stäbler
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **/

#include "FT81x.h"

#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 480

#define READ  0x000000
#define WRITE 0x800000

#define DLSTART()                    0xFFFFFF00
#define SWAP()                       0xFFFFFF01
#define MEMWRITE()                   0xFFFFFF1A
#define CLEAR(c, s, t)               ((0x26 << 24) | ((c) << 2) | ((s) << 1) | (t))
#define BEGIN(p)                     ((0x1F << 24) | (p))
#define END()                        (0x21 << 24)
#define END_DL()                     0x00
#define CLEAR_COLOR_RGB(r, g, b)     ((0x02 << 24) | ((r) << 16) | ((g) << 8) | (b))
#define CLEAR_COLOR(rgb)             ((0x02 << 24) | ((rgb) & 0xFFFFFF))
#define COLOR_RGB(r, g, b)           ((0x04 << 24) | ((r) << 16) | ((g) << 8) | (b))
#define COLOR(rgb)                   ((0x04 << 24) | ((rgb) & 0xFFFFFF))
#define POINT_SIZE(s)                ((0x0D << 24) | ((s) & 0xFFF))
#define LINE_WIDTH(w)                ((0x0E << 24) | ((w) & 0xFFF))
#define VERTEX2II(x, y, h, c)        ((1 << 31) | (((x) & 0xFFF) << 21) | (((y) & 0xFFF) << 12) | ((h) << 7) | (c))
#define VERTEX2F(x, y)               ((1 << 30) | (((x) & 0xFFFF) << 15) | ((y) & 0xFFFF))
#define BITMAP_SOURCE(a)             ((1 << 24) | (a))
#define BITMAP_LAYOUT(f, s, h)       ((7 << 24) | ((f) << 19) | (((s) & 0x1FF) << 9) | ((h) & 0x1FF))
#define BITMAP_SIZE(f, wx, wy, w, h) ((8 << 24) | (((f) & 1) << 20) | (((wx) & 1) << 19) | (((wy) & 1) << 18) | (((w) & 0x1FF) << 9)| ((h) & 0x1FF))
#define LOADIDENTITY()               0xFFFFFF26
#define SETMATRIX()                  0xFFFFFF2A
#define SCALE()                      0xFFFFFF28

#define BITMAPS      1
#define POINTS       2
#define LINES        3
#define LINE_STRIP   4
#define EDGE_STRIP_R 5
#define EDGE_STRIP_L 6
#define EDGE_STRIP_A 7
#define EDGE_STRIP_B 8
#define RECTS        9

#define DISPLAY_CMD(cmd, params...)                                     \
    {                                                                   \
        uint8_t d[] = { params };                                       \
        sendCommandToDisplay(cmd, sizeof(d) / sizeof(uint8_t), d);      \
    }

static uint32_t dli = 0;

void FT81x::init() {
    pinMode(FT81x_CS1, OUTPUT);
    digitalWrite(FT81x_CS1, HIGH);

    pinMode(FT81x_CS2, OUTPUT);
    digitalWrite(FT81x_CS2, HIGH);

    pinMode(FT81x_DC, OUTPUT);
    digitalWrite(FT81x_DC, LOW);

    FT81x::initFT81x();
    FT81x::initDisplay();
}

void FT81x::initFT81x() {
    // reset
    FT81x::sendCommand(FT81x_CMD_RST_PULSE);
    delay(300);

    // select clock
    FT81x::sendCommand(FT81x_CMD_CLKINT);
    delay(300);

    // activate
    FT81x::sendCommand(FT81x_CMD_ACTIVE);

    // wait for boot-up to complete
    delay(100);
    while (FT81x::read8(FT81x_REG_ID) != 0x7C) {
        __asm__ volatile ("nop");
    }
    while (FT81x::read8(FT81x_REG_CPURESET) != 0x00) {
        __asm__ volatile ("nop");
    }

    // configure rgb interface
    FT81x::write16(FT81x_REG_HCYCLE, DISPLAY_WIDTH + 8 + 8 + 50 + 2);
    FT81x::write16(FT81x_REG_HOFFSET, 8 + 8 + 50);
    FT81x::write16(FT81x_REG_HSYNC0, 8);
    FT81x::write16(FT81x_REG_HSYNC1, 8 + 8);
    FT81x::write16(FT81x_REG_HSIZE, DISPLAY_WIDTH);

    FT81x::write16(FT81x_REG_VCYCLE, DISPLAY_HEIGHT + 8 + 8 + 20 + 2);
    FT81x::write16(FT81x_REG_VOFFSET, 8 + 8 + 20);
    FT81x::write16(FT81x_REG_VSYNC0, 8);
    FT81x::write16(FT81x_REG_VSYNC1, 8 + 8);
    FT81x::write16(FT81x_REG_VSIZE, DISPLAY_HEIGHT);

    FT81x::write8(FT81x_REG_SWIZZLE, 0);
    FT81x::write8(FT81x_REG_PCLK_POL, 0);
    FT81x::write8(FT81x_REG_CSPREAD, 1);
    FT81x::write8(FT81x_REG_DITHER, 0);
    FT81x::write8(FT81x_REG_ROTATE, 0);

    // write first display list
    FT81x::begin();
    FT81x::clear(0);
    FT81x::swap();

    // enable pixel clock
    FT81x::write8(FT81x_REG_PCLK, 10);

    // reset display (somehow this generates a low pulse)
    FT81x::write16(FT81x_REG_GPIOX, 0x8000 | FT81x::read16(FT81x_REG_GPIOX));
    delay(300);
}

void FT81x::initDisplay() {
    // sleep mode off
    DISPLAY_CMD(ST7701_SLPOUT);
    delay(300);

    // Command2, BK0
    DISPLAY_CMD(ST7701_CND2BKxSEL, 0x77, 0x01, 0x00, 0x00, ST7701_CMD2BK0SEL);
    DISPLAY_CMD(ST7701_BK0_LNESET, 0x3B, 0x00);
    DISPLAY_CMD(ST7701_BK0_PORCTRL, 0x14, 0x0A);  // vbp, vfp
    DISPLAY_CMD(ST7701_BK0_INVSEL, 0x21, 0x08);
    DISPLAY_CMD(ST7701_BK0_PVGAMCTRL, 0x00, 0x11, 0x18, 0x0E, 0x11, 0x06, 0x07, 0x08, 0x07, 0x22, 0x04, 0x12, 0x0F, 0xAA, 0x31, 0x18);
    DISPLAY_CMD(ST7701_BK0_NVGAMCTRL, 0x00, 0x11, 0x19, 0x0E, 0x12, 0x07, 0x08, 0x08, 0x08, 0x22, 0x04, 0x11, 0x11, 0xA9, 0x32, 0x18);

    // Command2, BK1
    DISPLAY_CMD(ST7701_CND2BKxSEL, 0x77, 0x01, 0x00, 0x00, ST7701_CMD2BK1SEL);
    DISPLAY_CMD(ST7701_BK1_VRHS, 0x60);
    DISPLAY_CMD(ST7701_BK1_VCOM, 0x30);
    DISPLAY_CMD(ST7701_BK1_VGHSS, 0x87);
    DISPLAY_CMD(ST7701_BK1_TESTCMD, 0x80);
    DISPLAY_CMD(ST7701_BK1_VGLS, 0x49);
    DISPLAY_CMD(ST7701_BK1_PWCTLR1, 0x85);
    DISPLAY_CMD(ST7701_BK1_PWCTLR2, 0x21);
    DISPLAY_CMD(ST7701_BK1_SPD1, 0x78);
    DISPLAY_CMD(ST7701_BK1_SPD2, 0x78);

    DISPLAY_CMD(0xE0, 0x00, 0x1B, 0x02);
    DISPLAY_CMD(0xE1, 0x08, 0xA0, 0x00, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x00, 0x44, 0x44);
    DISPLAY_CMD(0xE2, 0x11, 0x11, 0x44, 0x44, 0xED, 0xA0, 0x00, 0x00, 0xEC, 0xA0, 0x00, 0x00);
    DISPLAY_CMD(0xE3, 0x00, 0x00, 0x11, 0x11);
    DISPLAY_CMD(0xE4, 0x44, 0x44);
    DISPLAY_CMD(0xE5, 0x0A, 0xE9, 0xD8, 0xA0, 0x0C, 0xEB, 0xD8, 0xA0, 0x0E, 0xED, 0xD8, 0xA0, 0x10, 0xEF, 0xD8, 0xA0);
    DISPLAY_CMD(0xE6, 0x00, 0x00, 0x11, 0x11);
    DISPLAY_CMD(0xE7, 0x44, 0x44);
    DISPLAY_CMD(0xE8, 0x09, 0xE8, 0xD8, 0xA0, 0x0B, 0xEA, 0xD8, 0xA0, 0x0D, 0xEC, 0xD8, 0xA0, 0x0F, 0xEE, 0xD8, 0xA0);
    DISPLAY_CMD(0xEB, 0x02, 0x00, 0xE4, 0xE4, 0x88, 0x00, 0x40);
    DISPLAY_CMD(0xEC, 0x3C, 0x00);
    DISPLAY_CMD(0xED, 0xAB, 0x89, 0x76, 0x54, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x45, 0x67, 0x98, 0xBA);

    DISPLAY_CMD(ST7701_CND2BKxSEL, 0x77, 0x01, 0x00, 0x00, ST7701_CMD2BKxSEL_NONE);

    // display on
    DISPLAY_CMD(ST7701_DISPON);

    // set pixel format
    DISPLAY_CMD(ST7701_COLMOD, 0x70);

    // DISPLAY_CMD(0x23); // all pixels on
}

void FT81x::clear(const uint32_t color) {
    cmd(CLEAR_COLOR(color));
    cmd(CLEAR(1, 1, 1));
}

void FT81x::drawCircle(const int16_t x, const int16_t y, const uint8_t size, const uint32_t color) {
    cmd(COLOR(color));
    cmd(POINT_SIZE(size * 16));
    cmd(BEGIN(POINTS));
    cmd(VERTEX2F(x * 16, y * 16));
    cmd(END());
}

void FT81x::drawRect(const int16_t x, const int16_t y, const uint16_t width, const uint16_t height, const uint8_t cornerRadius, const uint32_t color) {
    cmd(COLOR(color));
    cmd(LINE_WIDTH(cornerRadius * 16));
    cmd(BEGIN(RECTS));
    cmd(VERTEX2F(x * 16, y * 16));
    cmd(VERTEX2F((x + width) * 16, (y + height) * 16));
    cmd(END());
}

void FT81x::drawLetter(const int16_t x, const int16_t y, const uint8_t size, const uint32_t color, const uint8_t letter) {
    cmd(COLOR(color));
    cmd(BEGIN(BITMAPS));
    cmd(VERTEX2II(x, y, size, letter));
    cmd(END());
}

void FT81x::drawBitmap(const uint32_t offset, const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height, const uint8_t scale) {
    cmd(COLOR_RGB(255, 255, 255));
    cmd(BITMAP_SOURCE(FT81x_RAM_G + offset));
    cmd(BITMAP_LAYOUT(FT81x_BITMAP_LAYOUT_RGB565, width * 2, height));  // only supporting one format for now
    cmd(BITMAP_SIZE(FT81x_BITMAP_SIZE_NEAREST, 0, 0, width * scale, height * scale));
    cmd(BEGIN(BITMAPS));
    cmd(LOADIDENTITY());
    cmd(SCALE());
    cmd(scale * 65536);
    cmd(scale * 65536);
    cmd(SETMATRIX());
    cmd(VERTEX2II(x, y, 0, 0));
}

void FT81x::dl(const uint32_t cmd) {
    uint32_t addr = FT81x_RAM_DL + dli;
    write32(addr, cmd);
    dli += 4;
}

void FT81x::cmd(const uint32_t cmd) {
    uint16_t cmdWrite = FT81x::read16(FT81x_REG_CMD_WRITE);
    uint32_t addr = FT81x_RAM_CMD + cmdWrite;
    write32(addr, cmd);
    write16(FT81x_REG_CMD_WRITE, (cmdWrite + 4) % 4096);
}

void FT81x::writeGRAM(const uint32_t offset, const uint32_t size, uint8_t *data) {
    uint32_t sizeRemaining = size;
    uint32_t currentOffset = 0;

    while (sizeRemaining > 0) {
        while (FT81x::read16(FT81x_REG_CMD_WRITE) != FT81x::read16(FT81x_REG_CMD_READ)) {
            __asm__ volatile ("nop");
        }

        uint16_t bulkSpace = read16(FT81x_REG_CMDB_SPACE);
        uint32_t currentSize = sizeRemaining > bulkSpace ? bulkSpace : sizeRemaining;
        uint8_t remainder = currentSize % 4;

        cmd(MEMWRITE());
        cmd(FT81x_RAM_G + currentOffset + offset);
        cmd(currentSize);

        for (uint32_t i = currentOffset; i < currentOffset + currentSize - remainder; i += 4) {
            write32(FT81x_REG_CMDB_WRITE, data[i] | data[i + 1] << 8 | data[i + 2] << 16 | data[i + 3] << 24);
        }

        if (remainder > 0) {
            uint32_t lastBytes = 0;
            for (uint8_t i = currentOffset + currentSize - remainder; i < currentOffset + currentSize; i++) {
                lastBytes |= data[i] << (8 * i);
            }
            write32(FT81x_REG_CMDB_WRITE, lastBytes);
        }

        sizeRemaining -= currentSize;
        currentOffset += currentSize;
    }
}

void FT81x::writeGRAM(const uint32_t offset, const uint32_t size, uint16_t *data) {
    uint32_t sizeRemaining = size;
    uint32_t currentOffset = 0;

    while (sizeRemaining > 0) {
        while (FT81x::read16(FT81x_REG_CMD_WRITE) != FT81x::read16(FT81x_REG_CMD_READ)) {
            __asm__ volatile ("nop");
        }

        uint16_t bulkSpace = read16(FT81x_REG_CMDB_SPACE);
        uint32_t currentSize = sizeRemaining > bulkSpace ? bulkSpace : sizeRemaining;
        uint8_t remainder = currentSize % 2;

        cmd(MEMWRITE());
        cmd(FT81x_RAM_G + currentOffset + offset);
        cmd(currentSize * 2);

        for (uint32_t i = currentOffset; i < currentOffset + currentSize - remainder; i += 2) {
            write32(FT81x_REG_CMDB_WRITE, data[i] | data[i + 1] << 16);
        }

        if (remainder > 0) {
            uint32_t lastBytes = 0;
            for (uint8_t i = currentOffset + currentSize - remainder; i < currentOffset + currentSize; i++) {
                lastBytes |= data[i] << (8 * i);
            }
            write32(FT81x_REG_CMDB_WRITE, lastBytes);
        }

        sizeRemaining -= currentSize;
        currentOffset += currentSize;
    }
}

void FT81x::begin() {
    // Wait for circular buffer to catch up
    while (FT81x::read16(FT81x_REG_CMD_WRITE) != FT81x::read16(FT81x_REG_CMD_READ)) {
        __asm__ volatile ("nop");
    }
    cmd(DLSTART());
    cmd(CLEAR(1, 1, 1));
}

void FT81x::swap() {
    cmd(END_DL());
    cmd(SWAP());
}

void FT81x::sendCommand(const uint32_t cmd) {
    digitalWrite(FT81x_CS1, LOW);
    SPI.beginTransaction(FT81x_SPI_SETTINGS);
    SPI.transfer((cmd >> 16) & 0xFF);
    SPI.transfer((cmd >> 8) & 0xFF);
    SPI.transfer(cmd & 0xFF);
    SPI.endTransaction();
    digitalWrite(FT81x_CS1, HIGH);
}

uint8_t FT81x::read8(const uint32_t address) {
    uint32_t cmd = address;
    digitalWrite(FT81x_CS1, LOW);
    SPI.beginTransaction(FT81x_SPI_SETTINGS);
    SPI.transfer((cmd >> 16) & 0xFF);
    SPI.transfer((cmd >> 8) & 0xFF);
    SPI.transfer(cmd & 0xFF);
    SPI.transfer(0x00);  // dummy byte
    uint8_t result = SPI.transfer(0x00);
    SPI.endTransaction();
    digitalWrite(FT81x_CS1, HIGH);
    return result;
}

uint16_t FT81x::read16(const uint32_t address) {
    uint32_t cmd = address | READ;
    digitalWrite(FT81x_CS1, LOW);
    SPI.beginTransaction(FT81x_SPI_SETTINGS);
    SPI.transfer((cmd >> 16) & 0xFF);
    SPI.transfer((cmd >> 8) & 0xFF);
    SPI.transfer(cmd & 0xFF);
    SPI.transfer(0x00);  // dummy byte
    uint16_t result = SPI.transfer(0x00);
    result |= (SPI.transfer(0x00) << 8);
    SPI.endTransaction();
    digitalWrite(FT81x_CS1, HIGH);
    return result;
}

uint32_t FT81x::read32(const uint32_t address) {
    uint32_t cmd = address | READ;
    digitalWrite(FT81x_CS1, LOW);
    SPI.beginTransaction(FT81x_SPI_SETTINGS);
    SPI.transfer((cmd >> 16) & 0xFF);
    SPI.transfer((cmd >> 8) & 0xFF);
    SPI.transfer(cmd & 0xFF);
    SPI.transfer(0x00);  // dummy byte
    uint16_t result = SPI.transfer(0x00);
    result |= (SPI.transfer(0x00) << 8);
    result |= (SPI.transfer(0x00) << 16);
    result |= (SPI.transfer(0x00) << 24);
    SPI.endTransaction();
    digitalWrite(FT81x_CS1, HIGH);
    return result;
}

void FT81x::write8(const uint32_t address, const uint8_t data) {
    uint32_t cmd = address | WRITE;
    digitalWrite(FT81x_CS1, LOW);
    SPI.beginTransaction(FT81x_SPI_SETTINGS);
    SPI.transfer((cmd >> 16) & 0xFF);
    SPI.transfer((cmd >> 8) & 0xFF);
    SPI.transfer(cmd & 0xFF);
    SPI.transfer(data);
    SPI.endTransaction();
    digitalWrite(FT81x_CS1, HIGH);
}

void FT81x::write16(const uint32_t address, const uint16_t data) {
    uint32_t cmd = address | WRITE;
    digitalWrite(FT81x_CS1, LOW);
    SPI.beginTransaction(FT81x_SPI_SETTINGS);
    SPI.transfer((cmd >> 16) & 0xFF);
    SPI.transfer((cmd >> 8) & 0xFF);
    SPI.transfer(cmd & 0xFF);
    SPI.transfer(data & 0xFF);
    SPI.transfer((data >> 8) & 0xFF);
    SPI.endTransaction();
    digitalWrite(FT81x_CS1, HIGH);
}

void FT81x::write32(const uint32_t address, const uint32_t data) {
    uint32_t cmd = address | WRITE;
    digitalWrite(FT81x_CS1, LOW);
    SPI.beginTransaction(FT81x_SPI_SETTINGS);
    SPI.transfer((cmd >> 16) & 0xFF);
    SPI.transfer((cmd >> 8) & 0xFF);
    SPI.transfer(cmd & 0xFF);
    SPI.transfer(data & 0xFF);
    SPI.transfer((data >> 8) & 0xFF);
    SPI.transfer((data >> 16) & 0xFF);
    SPI.transfer((data >> 24) & 0xFF);
    SPI.endTransaction();
    digitalWrite(FT81x_CS1, HIGH);
}

void FT81x::sendCommandToDisplay(const uint8_t cmd, const unsigned int numParams, uint8_t *params) {
    digitalWrite(FT81x_DC, LOW);
    digitalWrite(FT81x_CS2, LOW);
    SPI.beginTransaction(FT81x_SPI_SETTINGS);
    SPI.transfer(cmd);
    digitalWrite(FT81x_DC, HIGH);
    for (unsigned int i = 0; i < numParams; i++) {
        SPI.transfer(params[i]);
    }
    SPI.endTransaction();
    digitalWrite(FT81x_CS2, HIGH);
    digitalWrite(FT81x_DC, LOW);
}

uint8_t FT81x::queryDisplay(const uint8_t cmd) {
    digitalWrite(FT81x_DC, LOW);
    digitalWrite(FT81x_CS2, LOW);
    SPI.beginTransaction(FT81x_SPI_SETTINGS);
    uint8_t result = SPI.transfer16(cmd << 8) & 0xFF;
    SPI.endTransaction();
    digitalWrite(FT81x_CS2, HIGH);
    return result;
}
