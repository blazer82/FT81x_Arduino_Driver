#include "FT81x.h"

#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 480

#define READ  0x000000
#define WRITE 0x800000

#define DLSTART()                    0xFFFFFF00
#define SWAP()                       0xFFFFFF01
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

    //while(true) {}
}

void FT81x::initFT81x()
{
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

    // pindrive
    /*FT81x::sendCommand(FT81x_CMD_PINDRIVE | 0x01 | (0x09 << 2));
    FT81x::sendCommand(FT81x_CMD_PINDRIVE | 0x01 | (0x0A << 2));
    FT81x::sendCommand(FT81x_CMD_PINDRIVE | 0x01 | (0x0B << 2));
    FT81x::sendCommand(FT81x_CMD_PINDRIVE | 0x01 | (0x0D << 2));
    delay(300);*/

    // configure rgb interface
    FT81x::write16(FT81x_REG_HCYCLE, DISPLAY_WIDTH + 22 + 6 + 18 + 2);
    FT81x::write16(FT81x_REG_HOFFSET, 22 + 6 + 18);
    FT81x::write16(FT81x_REG_HSYNC0, 22);
    FT81x::write16(FT81x_REG_HSYNC1, 22 + 6);
    FT81x::write16(FT81x_REG_HSIZE, DISPLAY_WIDTH);

    FT81x::write16(FT81x_REG_VCYCLE, DISPLAY_HEIGHT + 2 + 2 + 2 + 2);
    FT81x::write16(FT81x_REG_VOFFSET, 2 + 2 + 2);
    FT81x::write16(FT81x_REG_VSYNC0, 2);
    FT81x::write16(FT81x_REG_VSYNC1, 2 + 2);
    FT81x::write16(FT81x_REG_VSIZE, DISPLAY_HEIGHT);

    FT81x::write8(FT81x_REG_SWIZZLE, 0);
    FT81x::write8(FT81x_REG_PCLK_POL, 0);
    FT81x::write8(FT81x_REG_CSPREAD, 1);
    FT81x::write8(FT81x_REG_DITHER, 0);
    FT81x::write8(FT81x_REG_ROTATE, 0);

    // write first display list
    FT81x::begin();
    FT81x::clear(0xFF99FF);
    FT81x::drawCircle(50, 45, 40, FT81x_COLOR_RGB(255, 255, 255));
    FT81x::swap();

    // enable pixel clock
    FT81x::write8(FT81x_REG_PCLK, 10);

    // reset display (somehow this generates a low pulse)
    FT81x::write16(FT81x_REG_GPIOX, 0x8000 | FT81x::read16(FT81x_REG_GPIOX));
    delay(300);
}

void FT81x::initDisplay()
{
    // sleep mode off
    DISPLAY_CMD(ST7701_SLPOUT);
    delay(300);

    // Command2, BK0
    DISPLAY_CMD(ST7701_CND2BKxSEL, 0x77, 0x01, 0x00, 0x00, ST7701_CMD2BK0SEL);
    DISPLAY_CMD(ST7701_BK0_PVGAMCTRL, 0x00, 0x0E, 0x15, 0x0F, 0x11, 0x08, 0x08, 0x08, 0x08, 0x23, 0x04, 0x13, 0x12, 0x2B, 0x34, 0x1F);
    DISPLAY_CMD(ST7701_BK0_NVGAMCTRL, 0x00, 0x0E, 0x95, 0x0F, 0x13, 0x07, 0x09, 0x08, 0x08, 0x22, 0x04, 0x10, 0x0E, 0x2C, 0x34, 0x1F);
    DISPLAY_CMD(ST7701_BK0_LNESET, 0x3B, 0x00);
    DISPLAY_CMD(ST7701_BK0_PORCTRL, 2, 4);
    DISPLAY_CMD(ST7701_BK0_INVSEL, 0x37, 0x03);
    DISPLAY_CMD(ST7701_BK0_RGBCTRL, 0x00, 18, 2);
    
    // Command2, BK1
    DISPLAY_CMD(ST7701_CND2BKxSEL, 0x77, 0x01, 0x00, 0x00, ST7701_CMD2BK1SEL);
    DISPLAY_CMD(ST7701_BK1_VRHS, 0x45);
    DISPLAY_CMD(ST7701_BK1_VCOM, 0x13);
    DISPLAY_CMD(ST7701_BK1_VGHSS, 0x07);
    DISPLAY_CMD(ST7701_BK1_TESTCMD, 0x08);
    DISPLAY_CMD(ST7701_BK1_VGLS, 0x04);
    DISPLAY_CMD(ST7701_BK1_PWCTLR1, 0x85);
    DISPLAY_CMD(ST7701_BK1_PWCTLR2, 0x20);
    DISPLAY_CMD(ST7701_BK1_SPD1, 0x58);
    DISPLAY_CMD(ST7701_BK1_SPD2, 0x58);

    // Copied from Linux driver: https://github.com/torvalds/linux/blob/master/drivers/gpu/drm/panel/panel-sitronix-st7701.c
    /*DISPLAY_CMD(0xE0, 0x00, 0x00, 0x02);
    DISPLAY_CMD(0xE1, 0x0B, 0x00, 0x0D, 0x00, 0x0C, 0x00, 0x0E,0x00, 0x00, 0x44, 0x44);
    DISPLAY_CMD(0xE2, 0x33, 0x33, 0x44, 0x44, 0x64, 0x00, 0x66, 0x00, 0x65, 0x00, 0x67, 0x00, 0x00);
    DISPLAY_CMD(0xE3, 0x00, 0x00, 0x33, 0x33);
    DISPLAY_CMD(0xE4, 0x44, 0x44);
    DISPLAY_CMD(0xE5, 0x0C, 0x78, 0x3C, 0xA0, 0x0E, 0x78, 0x3C, 0xA0, 0x10, 0x78, 0x3C, 0xA0, 0x12, 0x78, 0x3C, 0xA0);
    DISPLAY_CMD(0xE6, 0x00, 0x00, 0x33, 0x33);
    DISPLAY_CMD(0xE7, 0x44, 0x44);
    DISPLAY_CMD(0xE8, 0x0D, 0x78, 0x3C, 0xA0, 0x0F, 0x78, 0x3C, 0xA0, 0x11, 0x78, 0x3C, 0xA0, 0x13, 0x78, 0x3C, 0xA0);
    DISPLAY_CMD(0xEB, 0x02, 0x02, 0x39, 0x39, 0xEE, 0x44, 0x00);
    DISPLAY_CMD(0xEC, 0x00, 0x00);
    DISPLAY_CMD(0xED, 0xFF, 0xF1, 0x04, 0x56, 0x72, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0x27, 0x65, 0x40, 0x1F, 0xFF);*/

    DISPLAY_CMD(ST7701_CND2BKxSEL, 0x77, 0x01, 0x00, 0x00, ST7701_CMD2BKxSEL_NONE);

    // set pixel format
    DISPLAY_CMD(ST7701_COLMOD, 0x70);

    // set data control
    DISPLAY_CMD(ST7701_MADCTL, 0x00);

    // set tearing effect on
    DISPLAY_CMD(ST7701_TEON, 0x00);

    // display on
    DISPLAY_CMD(ST7701_DISPON);

    //DISPLAY_CMD(0x23); // all pixels on

    /*Serial.printf("RDID1: %x\n", queryDisplay(ST7701_RDID1));
    Serial.printf("RDID2: %x\n", queryDisplay(ST7701_RDID2));
    Serial.printf("RDID3: %x\n", queryDisplay(ST7701_RDID3));
    Serial.printf("RDDSDR: %x\n", queryDisplay(ST7701_RDDSDR));
    Serial.printf("RDDPM: %x (must be > 8)\n", queryDisplay(ST7701_RDDPM));*/
}

void FT81x::clear(uint32_t color)
{
    cmd(CLEAR_COLOR(color));
    cmd(CLEAR(1, 1, 1));
}

void FT81x::drawCircle(int16_t x, int16_t y, uint8_t size, uint32_t color)
{
    cmd(COLOR(color));
    cmd(POINT_SIZE(size * 16));
    cmd(BEGIN(POINTS));
    cmd(VERTEX2F(x * 16, y * 16));
    cmd(END());
}

void FT81x::drawRect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint8_t cornerRadius, uint32_t color)
{
    cmd(COLOR(color));
    cmd(LINE_WIDTH(cornerRadius * 16));
    cmd(BEGIN(RECTS));
    cmd(VERTEX2F(x * 16, y * 16));
    cmd(VERTEX2F((x + width) * 16, (y + height) * 16));
    cmd(END());
}

void FT81x::drawLetter(int16_t x, int16_t y, uint8_t size, uint32_t color, uint8_t letter)
{
    cmd(COLOR(color));
    cmd(BEGIN(BITMAPS));
    cmd(VERTEX2II(x, y, size, letter));
    cmd(END());
}

void FT81x::dl(uint32_t cmd)
{
    uint32_t addr = FT81x_RAM_DL + dli;
    //Serial.printf("write32 %x, %x\n", addr, cmd);
    write32(addr, cmd);
    dli += 4;
}

void FT81x::cmd(uint32_t cmd)
{
    uint16_t cmdWrite = FT81x::read16(FT81x_REG_CMD_WRITE);
    uint32_t addr = FT81x_RAM_CMD + cmdWrite;
    //Serial.printf("write32 %x, %x\n", addr, cmd);
    write32(addr, cmd);
    write16(FT81x_REG_CMD_WRITE, (cmdWrite + 4) % 4096);
}

void FT81x::begin()
{
    cmd(DLSTART());
    cmd(CLEAR(1, 1, 1));
}

void FT81x::swap()
{
    cmd(END_DL());
    cmd(SWAP());
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

uint32_t FT81x::read32(uint32_t address)
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
    result |= (SPI.transfer(0x00) << 16);
    result |= (SPI.transfer(0x00) << 24);
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

void FT81x::sendCommandToDisplay(uint8_t cmd, unsigned int numParams, uint8_t *params)
{
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

uint8_t FT81x::queryDisplay(uint8_t cmd)
{
    digitalWrite(FT81x_DC, LOW);
    digitalWrite(FT81x_CS2, LOW);
    SPI.beginTransaction(FT81x_SPI_SETTINGS);
    uint8_t result = SPI.transfer16(cmd << 8) & 0xFF;
    SPI.endTransaction();
    digitalWrite(FT81x_CS2, HIGH);
    return result;
}
