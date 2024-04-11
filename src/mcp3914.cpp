#include <iostream>
#include <cstdint>
#include <string>
#include <chrono>
#include <thread>
#include <limits>
#include <cstring>

#include "mcp3914.h"

namespace ADC
{
    #define VOLTAGE_CONV    0.055748 //0.058036347 //OLD 0.060155
    #define CURRENT_CONV    0.009984
    
    /// @brief Configure the MCP3914 for standard operatrion for the PMU
    /// @return 
    int MCP3914::configure() {
        this->write_reg(MCP3914_REG_CONFIG1, MCP3914_CONF_CONFIG1_RESET);  // Reset ADC
        std::this_thread::sleep_for(std::chrono::milliseconds(MCP3914_DELAY_SPI));

        this->write_reg(MCP3914_REG_STATUSCOM, MCP3914_CONF_CONT_READ);      // Set Device to continuous read
        std::this_thread::sleep_for(std::chrono::milliseconds(MCP3914_DELAY_SPI));

        this->write_reg(MCP3914_REG_CONFIG0, MCP3914_CONF_OSR256);         // Set OSR=256
        std::this_thread::sleep_for(std::chrono::milliseconds(MCP3914_DELAY_SPI));

        this->write_reg(MCP3914_REG_CONFIG1, MCP3914_CONF_CONFIG1_START);  // Start ADC
        std::this_thread::sleep_for(std::chrono::milliseconds(MCP3914_DELAY_SPI));

        std::this_thread::sleep_for(std::chrono::seconds(1));

        return 0;
    }

    /// @brief Generates the command byte required for the MCP3914
    /// @param addr Device Address
    /// @param rw Read or Write
    /// @return Command byte
    uint8_t MCP3914::get_cmd_byte(uint8_t addr, bool rw) {
        uint8_t dev_addr = MCP3914_CONST_ADDR;
        uint8_t reg_addr = (addr << 1) & 0b00111110;
        
        return (dev_addr ^ reg_addr ^ rw);
    }

        
    /// @brief Initialises the spi inteerface and configures the device registers
    int MCP3914::init() {
        uint8_t flags = 0;

        this->spi_device_.init(spiChan_, (uint8_t) spi_baud_, flags);

        // Initialize Registers
        this->configure();

        return 0;
    }

    /// @brief Read channel values from the ADC
    /// @param converted Converted values
    /// @param count Number of values
    /// @return Error
    int MCP3914::read(float *converted, int count) {
        uint8_t readBuf[3*8];
        int countReg;
        int res;
        
        if (count != 8) {
            return 0;
        }

        countReg = sizeof(readBuf)/sizeof(readBuf[0]);

        res = read_channels(countReg, readBuf);
        
        MCP3914::convert_channels(readBuf, converted, countReg);

        return res;
    }



    /// @brief Write data to a MCP3914 register
    /// @param write_addr Device Address
    /// @param data Data to be written
    /// @return Error handling
    int MCP3914::write_reg(uint8_t write_addr, uint32_t data) {
        uint8_t cmd_byte = this->get_cmd_byte(write_addr, MCP3914_WRITE);
        uint32_t msg = ((cmd_byte << 24) & MCP3914_MASK_CMD) ^ (data & ~MCP3914_MASK_CMD);

        uint8_t txBuf[4] = {0};
        uint8_t rxBuf[4] = {0};

        txBuf[0] = cmd_byte;

        txBuf[1] = ((data >> 16) & MCP3914_MASK_8BIT); 
        txBuf[2] = ((data >> 8) & MCP3914_MASK_8BIT); 
        txBuf[3] = ((data >> 0) & MCP3914_MASK_8BIT); 

        unsigned count = sizeof(txBuf)/sizeof(txBuf[0]);

        int err = this->spi_device_.transfer(txBuf, rxBuf, count);
        
        return err;
    }

    /// @brief Read data from an MCP3914 register
    /// @param read_addr Device address
    /// @param rxBuf Recieve buffer
    /// @param count size of buffer
    /// @note First byte of rx_buff will always be 0 due to response during cmd byte
    /// @return 
    int MCP3914::read_reg(uint8_t read_addr, uint8_t *rxBuf, int count) {
        uint8_t cmd_byte = this->get_cmd_byte(read_addr, MCP3914_READ);

        uint8_t txBuf[count];
        txBuf[0] = cmd_byte;
        for (int i=1; i<count; i++) {
            txBuf[i] = 0;
        }

        int err = this->spi_device_.transfer(txBuf, rxBuf, count);

        return err;
    }

    /// @brief Read and return readings for each channel. Raw results returned. Note that channel 7 is not connected.
    /// @param count size of channel sata
    /// @param channel_data data to be recieved
    /// @return error
    int MCP3914::read_channels(int count, uint8_t *channel_data){
        if (count != (8*3) ) {
            return 0;
        }

        uint8_t cmd_byte = this->get_cmd_byte(MCP3914_REG_CHANNEL_0, MCP3914_READ);

        uint8_t txBuf[count+1];
        uint8_t rxBuf[count+1];
        txBuf[0] = cmd_byte;

        int err = this->spi_device_.transfer(txBuf, rxBuf, count);

        std::memcpy(channel_data, rxBuf + sizeof(rxBuf[0]), (3*8));      

        return err;
    }

    /// @brief Calculate ADC measured values based on spi data. Expects data from u8 array with 24bit for each channel
    /// @param raw_channels 
    /// @param converted_channels 
    /// @param count 
    /// @return 
    int MCP3914::convert_channels(uint8_t *raw_channels, float *converted_channels, int count) {

        if (count != (3*8)) {
            return -1;
        }

        uint32_t data[8];

        memset(&data[0], 0, sizeof(data[0])*8);

        for (int i=0; i<8; i++) {
            for (int j=0; j<3; j++) { 
                data[i] = data[i] | (raw_channels[(j+(i*3))] << (8*(2-j)));
            }
        }
        

        for (int i=0; i<8; i++) {
            if ((int)data[i] > 8388607) {
                converted_channels[i] = (((int)data[i] - 16777216));
            } else {
                converted_channels[i] = data[i];
            }

            if (i < 3){
                converted_channels[i] = (converted_channels[i] * VOLTAGE_CONV) * 0.001;
            } else {
                converted_channels[i] = (converted_channels[i] * CURRENT_CONV) * 0.001;
            }
        }
        return 0;
    }

    /// @brief Test the value of the security register
    /// @return 
    int MCP3914::sec_read_test() {
        uint8_t readBuf_secTest[2];
        int count = sizeof(readBuf_secTest)/sizeof(readBuf_secTest[0]);
        
        int res_secTest = read_reg(MCP3914_REG_SECURITY, readBuf_secTest, count);

        if (readBuf_secTest[1] == 165) {
            return 1;
        } else {
            return 0;
        }
    }
}