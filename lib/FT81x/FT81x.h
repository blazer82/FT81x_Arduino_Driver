#pragma once

#include <Arduino.h>
#include <SPI.h>

#define FT81x_COLOR_RGB(r, g, b)       (((r) << 16) | ((g) << 8) | (b))

#ifndef FT81x_SPI_SETTINGS
#define FT81x_SPI_SETTINGS             SPISettings(1, MSBFIRST, SPI_MODE0)
#endif

#ifndef FT81x_CS1
#define FT81x_CS1                      SS
#endif

#ifndef FT81x_CS2
#define FT81x_CS2                      6
#endif

#ifndef FT81x_DC
#define FT81x_DC                       8
#endif

#define FT81x_CMD_ACTIVE               0x000000
#define FT81x_CMD_RST_PULSE            0x680000
#define FT81x_CMD_PINDRIVE             0x700000
#define FT81x_CMD_STANDBY              0x410000
#define FT81x_CMD_SLEEP                0x420000
#define FT81x_CMD_PWRDOWN              0x430000
#define FT81x_CMD_CLKEXT               0x440000
#define FT81x_CMD_CLKINT               0x480000
#define FT81x_CMD_CLKSEL               0x610000

#define FT81x_REG_ID                   0x302000
#define FT81x_REG_FRAMES               0x302004
#define FT81x_REG_CLOCK                0x302008
#define FT81x_REG_FREQUENCY            0x30200C
#define FT81x_REG_RENDERMODE           0x302010
#define FT81x_REG_SNAPY                0x302014
#define FT81x_REG_SNAPSHOT             0x302018
#define FT81x_REG_SNAPFORMAT           0x30201C
#define FT81x_REG_CPURESET             0x302020
#define FT81x_REG_TAP_CRC              0x302024
#define FT81x_REG_TAP_MASK             0x302028
#define FT81x_REG_HCYCLE               0x30202C
#define FT81x_REG_HOFFSET              0x302030
#define FT81x_REG_HSIZE                0x302034
#define FT81x_REG_HSYNC0               0x302038
#define FT81x_REG_HSYNC1               0x30203C
#define FT81x_REG_VCYCLE               0x302040
#define FT81x_REG_VOFFSET              0x302044
#define FT81x_REG_VSIZE                0x302048
#define FT81x_REG_VSYNC0               0x30204C
#define FT81x_REG_VSYNC1               0x302050
#define FT81x_REG_DLSWAP               0x302054
#define FT81x_REG_ROTATE               0x302058
#define FT81x_REG_OUTBITS              0x30205C
#define FT81x_REG_DITHER               0x302060
#define FT81x_REG_SWIZZLE              0x302064
#define FT81x_REG_CSPREAD              0x302068
#define FT81x_REG_PCLK_POL             0x30206C
#define FT81x_REG_PCLK                 0x302070
#define FT81x_REG_TAG_X                0x302074
#define FT81x_REG_TAG_Y                0x302078
#define FT81x_REG_TAG                  0x30207C
#define FT81x_REG_VOL_PB               0x302080
#define FT81x_REG_VOL_SOUND            0x302084
#define FT81x_REG_SOUND                0x302088
#define FT81x_REG_PLAY                 0x30208C
#define FT81x_REG_GPIO_DIR             0x302090
#define FT81x_REG_GPIO                 0x302094
#define FT81x_REG_GPIOX_DIR            0x302098
#define FT81x_REG_GPIOX                0x30209C
#define FT81x_REG_INT_FLAGS            0x3020A8
#define FT81x_REG_INT_EN               0x3020AC
#define FT81x_REG_INT_MASK             0x3020B0
#define FT81x_REG_PLAYBACK_START       0x3020B4
#define FT81x_REG_PLAYBACK_LENGTH      0x3020B8
#define FT81x_REG_PLAYBACK_READPTR     0x3020BC
#define FT81x_REG_PLAYBACK_FREQ        0x3020C0
#define FT81x_REG_PLAYBACK_FORMAT      0x3020C4
#define FT81x_REG_PLAYBACK_LOOP        0x3020C8
#define FT81x_REG_PLAYBACK_PLAY        0x3020CC
#define FT81x_REG_PWM_HZ               0x3020D0
#define FT81x_REG_PWM_DUTY             0x3020D4
#define FT81x_REG_MACRO_0              0x3020D8
#define FT81x_REG_MACRO_1              0x3020DC
#define FT81x_REG_CMD_READ             0x3020F8
#define FT81x_REG_CMD_WRITE            0x3020FC
#define FT81x_REG_CMD_DL               0x302100
#define FT81x_REG_BIST_EN              0x302174
#define FT81x_REG_TRIM                 0x302180
#define FT81x_REG_ANA_COMP             0x302184
#define FT81x_REG_SPI_WIDTH            0x302188
#define FT81x_REG_DATESTAMP            0x302564
#define FT81x_REG_CMDB_SPACE           0x302574
#define FT81x_REG_CMDB_WRITE           0x302578

#define FT81x_RAM_G                    0x000000
#define FT81x_ROM_FONT                 0x1E0000
#define FT81x_ROM_FONT_ADDR            0x2FFFFC
#define FT81x_RAM_DL                   0x300000
#define FT81x_RAM_REG                  0x302000
#define FT81x_RAM_CMD                  0x308000

#define FT81x_DLSWAP_LINE              0x1
#define FT81x_DLSWAP_FRAME             0x2

#define ST7701_SWRESET                 0x01
#define ST7701_RDDPM                   0x0A
#define ST7701_RDDSDR                  0x0F
#define ST7701_SLPIN                   0x10
#define ST7701_SLPOUT                  0x11
#define ST7701_NORON                   0x13
#define ST7701_INVOFF                  0x20
#define ST7701_INVON                   0x21
#define ST7701_DISPOFF                 0x28
#define ST7701_DISPON                  0x29
#define ST7701_TEON                    0x35
#define ST7701_MADCTL                  0x36
#define ST7701_COLMOD                  0x3A
#define ST7701_WRDISBV                 0x51
#define ST7701_SDIR                    0xC7
#define ST7701_RDID1                   0xDA
#define ST7701_RDID2                   0xDB
#define ST7701_RDID3                   0xDC

#define ST7701_CND2BKxSEL              0xFF

#define ST7701_BK0_PVGAMCTRL           0xB0
#define ST7701_BK0_NVGAMCTRL           0xB1
#define ST7701_BK0_LNESET              0xC0
#define ST7701_BK0_PORCTRL             0xC1
#define ST7701_BK0_INVSEL              0xC2
#define ST7701_BK0_RGBCTRL             0xC3
#define ST7701_BK0_SDIR                0xC7

#define ST7701_BK1_VRHS                0xB0
#define ST7701_BK1_VCOM                0xB1
#define ST7701_BK1_VGHSS               0xB2
#define ST7701_BK1_TESTCMD             0xB3
#define ST7701_BK1_VGLS                0xB5
#define ST7701_BK1_PWCTLR1             0xB7
#define ST7701_BK1_PWCTLR2             0xB8
#define ST7701_BK1_SPD1                0xC1
#define ST7701_BK1_SPD2                0xC2

#define ST7701_CMD2BK1SEL              0x11
#define ST7701_CMD2BK0SEL              0x10
#define ST7701_CMD2BKxSEL_NONE         0x00


class FT81x {
    public:
        static void init();
        static void sendCommand(uint32_t cmd);
        
        static uint8_t read8(uint32_t address);
        static uint16_t read16(uint32_t address);
        static uint32_t read32(uint32_t address);

        static void write8(uint32_t address, uint8_t data);
        static void write16(uint32_t address, uint16_t data);
        static void write32(uint32_t address, uint32_t data);

        static void clear(uint32_t color);
        static void drawCircle(int16_t x, int16_t y, uint8_t size, uint32_t color);
        static void drawRect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint8_t cornerRadius, uint32_t color);
        static void drawLetter(int16_t x, int16_t y, uint8_t size, uint32_t color, uint8_t letter);
        
        static void begin();
        static void swap();

    protected:
        static void initFT81x();
        static void initDisplay();

        static void sendCommandToDisplay(uint8_t cmd, unsigned int numParams, uint8_t *params);
        static uint8_t queryDisplay(uint8_t cmd);

        static void dl(uint32_t cmd);
        static void cmd(uint32_t cmd);

    private:
};
