#ifndef __MCP3914_H
#define __MCP3914_H

#include <spi.h>

namespace ADC{


#define SPI_SPEED                       18000000    //MCP3914 Capable of up to 20Mhz
#define DEFAULT_SPI_CHANNEL             0


// Delays
#define MCP3914_DELAY_SPI               100

// Memory Addresses
#define	MCP3914_REG_CHANNEL_BASE        0
#define MCP3914_REG_CHANNEL_0           0
#define MCP3914_REG_CHANNEL_1           1
#define MCP3914_REG_CHANNEL_2           2
#define MCP3914_REG_CHANNEL_3           3
#define MCP3914_REG_CHANNEL_4           4
#define MCP3914_REG_CHANNEL_5           5
#define MCP3914_REG_CHANNEL_6           6
#define MCP3914_REG_CHANNEL_7           7
#define MCP3914_REG_MOD                 8
#define MCP3914_REG_PHASE0              9
#define MCP3914_REG_PHASE1              10
#define MCP3914_REG_GAIN                11
#define MCP3914_REG_STATUSCOM           12
#define MCP3914_REG_CONFIG0             13
#define MCP3914_REG_CONFIG1             14
#define MCP3914_REG_OFFCAL_CH0          15
#define MCP3914_REG_GAINCAL_CH0         16 
#define MCP3914_REG_SECURITY            31
#define MCP3914_REG_NUM                 32

// Device Constants
#define MCP3914_CONST_ADDR              0b01000000
#define MCP3914_WRITE                   0
#define MCP3914_READ                    1
#define MCP3914_CONST_DEFAULT_CRC       0xA5

// Device Configurations                
#define MCP3914_CONF_CONFIG1_RESET      0b111111111111111100000000 // 16776960 Resets ADC
#define MCP3914_CONF_CONFIG1_START      0b000000000000000000000000 // Starts ADC
#define MCP3914_CONF_CONT_READ          0b101010010000000000000000 /*
                                                                    11075584 From Stanley 
                                                                    set continuous read by type, 
                                                                    write auto increment, 
                                                                    individual DR pulses, 
                                                                    16-bit CRC, 24-bit adc format, 
                                                                    crc disabled, 
                                                                    crc interrupt disabled.
                                                                    */
#define MCP3914_CONF_OSR256             0b001110000110000001000000 // Sets OSR=256 with temp comp
#define MCP3914_CONF_OSR128             0b001110000100000001000000 // Sets OSR=128 with temp comp


// MASKS
#define MCP3914_MASK_CMD                0b11111111000000000000000000000000
#define MCP3914_MASK_8BIT               0b11111111
#define MCP3914_MASK_24b1               0b00000000111111110000000000000000
#define MCP3914_MASK_24b2               0b00000000000000001111111100000000
#define MCP3914_MASK_24b3               0b00000000000000000000000011111111

class MCP3914{
    // Function Definitions
    private:
        unsigned spiChan_;
        unsigned spi_baud_;
        Spi spi_device_;

        int configure();
        uint8_t get_cmd_byte(uint8_t addr, bool rw);

    public:
        MCP3914(unsigned spiChan = DEFAULT_SPI_CHANNEL, unsigned spi_baud = SPI_SPEED) : spiChan_(spiChan), spi_baud_(spi_baud) 
        {
            spi_device_ = Spi(spi_baud);
            this->init();
        }  
        int init();
        int write_reg(uint8_t write_addr, uint32_t data);
        int read_reg(uint8_t read_addr, uint8_t *rxBuf, int count);
        int read_channels(int count, uint8_t *channel_data);
        int convert_channels(uint8_t *raw_channels, float *converted_channels, int count);
        int sec_read_test();
        int read(float *converted, int count);
};
}

#endif