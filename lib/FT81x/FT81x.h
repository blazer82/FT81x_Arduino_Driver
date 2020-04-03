#pragma once

#include <Arduino.h>
#include <SPI.h>

#define SPI_SETTINGS             SPISettings(1, MSBFIRST, SPI_MODE0)

#define CMD_ACTIVE               0x000000
#define CMD_RST_PULSE            0x680000
#define CMD_STANDBY              0x410000
#define CMD_SLEEP                0x420000
#define CMD_PWRDOWN              0x430000
#define CMD_CLKEXT               0x440000
#define CMD_CLKINT               0x480000
#define CMD_CLKSEL               0x610000

#define REG_ID                   0x302000
#define REG_FRAMES               0x302004
#define REG_CLOCK                0x302008
#define REG_FREQUENCY            0x30200C
#define REG_RENDERMODE           0x302010
#define REG_SNAPY                0x302014
#define REG_SNAPSHOT             0x302018
#define REG_SNAPFORMAT           0x30201C
#define REG_CPURESET             0x302020
#define REG_TAP_CRC              0x302024
#define REG_TAP_MASK             0x302028
#define REG_HCYCLE               0x30202C
#define REG_HOFFSET              0x302030
#define REG_HSIZE                0x302034
#define REG_HSYNC0               0x302038
#define REG_HSYNC1               0x30203C
#define REG_VCYCLE               0x302040
#define REG_VOFFSET              0x302044
#define REG_VSIZE                0x302048
#define REG_VSYNC0               0x30204C
#define REG_VSYNC1               0x302050
#define REG_DLSWAP               0x302054
#define REG_ROTATE               0x302058
#define REG_OUTBITS              0x30205C
#define REG_DITHER               0x302060
#define REG_SWIZZLE              0x302064
#define REG_CSPREAD              0x302068
#define REG_PCLK_POL             0x30206C
#define REG_PCLK                 0x302070
#define REG_TAG_X                0x302074
#define REG_TAG_Y                0x302078
#define REG_TAG                  0x30207C
#define REG_VOL_PB               0x302080
#define REG_VOL_SOUND            0x302084
#define REG_SOUND                0x302088
#define REG_PLAY                 0x30208C
#define REG_GPIO_DIR             0x302090
#define REG_GPIO                 0x302094
#define REG_GPIOX_DIR            0x302098
#define REG_GPIOX                0x30209C
#define REG_INT_FLAGS            0x3020A8
#define REG_INT_EN               0x3020AC
#define REG_INT_MASK             0x3020B0
#define REG_PLAYBACK_START       0x3020B4
#define REG_PLAYBACK_LENGTH      0x3020B8
#define REG_PLAYBACK_READPTR     0x3020BC
#define REG_PLAYBACK_FREQ        0x3020C0
#define REG_PLAYBACK_FORMAT      0x3020C4
#define REG_PLAYBACK_LOOP        0x3020C8
#define REG_PLAYBACK_PLAY        0x3020CC
#define REG_PWM_HZ               0x3020D0
#define REG_PWM_DUTY             0x3020D4
#define REG_MACRO_0              0x3020D8
#define REG_MACRO_1              0x3020DC
#define REG_CMD_READ             0x3020F8
#define REG_CMD_WRITE            0x3020FC
#define REG_CMD_DL               0x302100
#define REG_BIST_EN              0x302174
#define REG_TRIM                 0x302180
#define REG_ANA_COMP             0x302184
#define REG_SPI_WIDTH            0x302188
#define REG_DATESTAMP            0x302564
#define REG_CMDB_SPACE           0x302574
#define REG_CMDB_WRITE           0x302578


class FT81x {
    public:
        static void sendCommand(uint32_t cmd);
        
        static uint8_t read8(uint32_t address);
        static uint16_t read16(uint32_t address);
        static uint32_t read32(uint32_t address);

        static void write8(uint32_t address, uint8_t data);
        static void write16(uint32_t address, uint16_t data);
        static void write32(uint32_t address, uint32_t data);
    protected:
    private:
};
