#include "FT81x.h"

#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 480

#define READ  0x000000
#define WRITE 0x800000

#define DLSTART()                    0xFFFFFF00
#define CLEAR(c, s, t)               ((0x26 << 24) | ((c) << 2) | ((s) << 1) | (t))
#define BEGIN(p)                     ((0x1F << 24) | (p))
#define END()                        (0x21 << 24)
#define END_DL()                     0x00
#define CLEAR_COLOR_RGB(r, g, b)     ((0x02 << 24) | ((r) << 16) | ((g) << 8) | (b))
#define CLEAR_COLOR(rgb)             ((0x02 << 24) | ((rgb) & 0xFFFFFF))
#define COLOR_RGB(r, g, b)           ((0x04 << 24) | ((r) << 16) | ((g) << 8) | (b))
#define COLOR(rgb)                   ((0x04 << 24) | ((rgb) & 0xFFFFFF))
#define POINT_SIZE(s)                ((0x0D << 24) | ((s) & 0xFFFF))
#define VERTEX2II(x, y, h, c)        ((1 << 31) | ((x) << 21) | ((y) << 12) | ((h) << 7) | (c))
#define VERTEX2F(x, y)               ((1 << 30) | ((x) << 15) | (y))

#define POINTS 2
#define RECTS  9

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

void FT81x::initFT81x()
{
    // reset
    FT81x::sendCommand(FT81x_CMD_RST_PULSE);
    delay(300);

    // activate
    FT81x::sendCommand(FT81x_CMD_ACTIVE);
    
    // wait for boot-up to complete
    while (FT81x::read8(FT81x_REG_ID) != 0x7C) {
        __asm__("nop");
    }

    // pindrive
    /*FT81x::sendCommand(FT81x_CMD_PINDRIVE | 0x01 | (0x09 << 2));
    FT81x::sendCommand(FT81x_CMD_PINDRIVE | 0x01 | (0x0A << 2));
    FT81x::sendCommand(FT81x_CMD_PINDRIVE | 0x01 | (0x0B << 2));
    FT81x::sendCommand(FT81x_CMD_PINDRIVE | 0x01 | (0x0D << 2));
    delay(300);*/

    // configure rgb interface
    FT81x::write16(FT81x_REG_HCYCLE, DISPLAY_WIDTH + 68);
    FT81x::write16(FT81x_REG_HOFFSET, 68);
    FT81x::write16(FT81x_REG_HSYNC0, 4);
    FT81x::write16(FT81x_REG_HSYNC1, 62);
    FT81x::write16(FT81x_REG_HSIZE, DISPLAY_WIDTH);

    FT81x::write16(FT81x_REG_VCYCLE, DISPLAY_HEIGHT + 24);
    FT81x::write16(FT81x_REG_VOFFSET, 24);
    FT81x::write16(FT81x_REG_VSYNC0, 2);
    FT81x::write16(FT81x_REG_VSYNC1, 20);
    FT81x::write16(FT81x_REG_VSIZE, DISPLAY_HEIGHT);

    FT81x::write8(FT81x_REG_SWIZZLE, 0);
    FT81x::write8(FT81x_REG_PCLK_POL, 0);
    FT81x::write8(FT81x_REG_CSPREAD, 1);
    FT81x::write8(FT81x_REG_ROTATE, 0);

    // write first display list
    FT81x::clear(0x00FFF8);
    FT81x::swap();

    // enable display
    FT81x::write8(FT81x_REG_GPIO_DIR, 0x80);
    FT81x::write8(FT81x_REG_GPIO, 0x80);
    delay(10);
    FT81x::write8(FT81x_REG_GPIO, 0x00);
    delay(10);
    FT81x::write8(FT81x_REG_GPIO, 0x80);
    delay(10);
    FT81x::write8(FT81x_REG_PCLK, 10);
}

void FT81x::initDisplay()
{
    // display on
    sendCommandToDisplay(ST7701_DISPON);
    delay(100);
    sendCommandToDisplay(ST7701_SLPOUT);
    delay(100);

    // set pixel format
    sendCommandWithParamToDisplay(ST7701_COLMOD, 0x70);

    // normal mode on
    sendCommandToDisplay(ST7701_NORON);

    //sendCommandToDisplay(0x23); // all pixels on
}

void FT81x::clear(uint32_t color)
{
    dl(CLEAR_COLOR(color));
    dl(CLEAR(1, 1, 1));
}

void FT81x::drawCircle(int16_t x, int16_t y, uint8_t size, uint32_t color)
{
    dl(CLEAR_COLOR(0));
    dl(CLEAR(1, 1, 1));
    dl(COLOR(color));
    dl(POINT_SIZE(size * 16));
    dl(BEGIN(POINTS));
    dl(VERTEX2F(x * 16, y * 16));
    dl(END());
}

void FT81x::dl(uint32_t cmd)
{
    Serial.printf("write32 %x, %x\n", FT81x_RAM_DL + dli, cmd);
    write32(FT81x_RAM_DL + dli, cmd);
    dli += 4;
}

void FT81x::swap()
{
    dl(END_DL());
    write8(FT81x_REG_DLSWAP, FT81x_DLSWAP_LINE);
    dli = 0;
}

void FT81x::sendCommand(uint32_t cmd)
{
    digitalWrite(FT81x_CS1, LOW);
    SPI.beginTransaction(FT81x_SPI_SETTINGS);
    SPI.transfer((cmd >> 16) & 0xFF);
    SPI.transfer((cmd >> 8) & 0xFF);
    SPI.transfer(cmd & 0xFF);
    SPI.endTransaction();
    digitalWrite(FT81x_CS1, HIGH);
}

uint8_t FT81x::read8(uint32_t address)
{
    uint32_t cmd = address;
    digitalWrite(FT81x_CS1, LOW);
    SPI.beginTransaction(FT81x_SPI_SETTINGS);
    SPI.transfer((cmd >> 16) & 0xFF);
    SPI.transfer((cmd >> 8) & 0xFF);
    SPI.transfer(cmd & 0xFF);
    SPI.transfer(0x00); // dummy byte
    uint8_t result = SPI.transfer(0x00);
    SPI.endTransaction();
    digitalWrite(FT81x_CS1, HIGH);
    return result;
}

uint16_t FT81x::read16(uint32_t address)
{
    uint32_t cmd = address | READ;
    digitalWrite(FT81x_CS1, LOW);
    SPI.beginTransaction(FT81x_SPI_SETTINGS);
    SPI.transfer((cmd >> 16) & 0xFF);
    SPI.transfer((cmd >> 8) & 0xFF);
    SPI.transfer(cmd & 0xFF);
    SPI.transfer(0x00); // dummy byte
    uint16_t result = SPI.transfer(0x00);
    result |= (SPI.transfer(0x00) << 8);
    SPI.endTransaction();
    digitalWrite(FT81x_CS1, HIGH);
    return result;
}

void FT81x::write8(uint32_t address, uint8_t data)
{
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

void FT81x::write16(uint32_t address, uint16_t data)
{
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

void FT81x::write32(uint32_t address, uint32_t data)
{
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

void FT81x::sendCommandToDisplay(uint8_t cmd)
{
    digitalWrite(FT81x_DC, LOW);
    digitalWrite(FT81x_CS2, LOW);
    SPI.beginTransaction(FT81x_SPI_SETTINGS);
    SPI.transfer(cmd);
    SPI.endTransaction();
    digitalWrite(FT81x_CS2, HIGH);
}

void FT81x::sendCommandWithParamToDisplay(uint8_t cmd, uint8_t param)
{
    digitalWrite(FT81x_DC, LOW);
    digitalWrite(FT81x_CS2, LOW);
    SPI.beginTransaction(FT81x_SPI_SETTINGS);
    SPI.transfer(cmd);
    digitalWrite(FT81x_DC, HIGH);
    SPI.transfer(param);
    SPI.endTransaction();
    digitalWrite(FT81x_CS2, HIGH);
    digitalWrite(FT81x_DC, LOW);
}