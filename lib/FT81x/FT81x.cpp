#include "FT81x.h"

#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 480

#define READ  0x000000
#define WRITE 0x800000

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
    delay(300);

    // configure rgb interface
    FT81x::write16(FT81x_REG_HCYCLE, DISPLAY_WIDTH + 24);
    FT81x::write16(FT81x_REG_HOFFSET, 24);
    FT81x::write16(FT81x_REG_HSYNC0, 4);
    FT81x::write16(FT81x_REG_HSYNC1, 16);
    FT81x::write16(FT81x_REG_HSIZE, DISPLAY_WIDTH);

    FT81x::write16(FT81x_REG_VCYCLE, DISPLAY_HEIGHT + 12);
    FT81x::write16(FT81x_REG_VOFFSET, 12);
    FT81x::write16(FT81x_REG_VSYNC0, 2);
    FT81x::write16(FT81x_REG_VSYNC1, 8);
    FT81x::write16(FT81x_REG_VSIZE, DISPLAY_HEIGHT);

    FT81x::write8(FT81x_REG_SWIZZLE, 0);
    FT81x::write8(FT81x_REG_PCLK_POL, 1);
    FT81x::write8(FT81x_REG_CSPREAD, 0);
    FT81x::write8(FT81x_REG_CSPREAD, 0);

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

    sendCommandToDisplay(0x23); // all pixels on
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