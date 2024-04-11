#ifndef __SPI_H
#define __SPI_H

#include <cstdint>
#include <cstdio>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>

// Function Definitions
namespace ADC
{
class Spi
{
   private:
      int fd_;
      int baud_;
    public:
        Spi(int baud = 10000000, int fd = -1) : baud_(baud), fd_(fd) {}
        int init(unsigned spiChan, unsigned spiBaud, unsigned spiFlags);
        int close_channel();
        int read(char *buf, unsigned count);
        int write(char *buf, unsigned count);
        int transfer(uint8_t *txBuf, uint8_t *rxBuf, unsigned count);
};
}
#endif