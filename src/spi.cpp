#include "spi.h"


/// @brief Namespace for all the libraries used reading analog values
namespace ADC
{
   int Spi::close_channel(){
      return close(this->fd_);
   }

   /// @brief Init SpiDev on relevent /dev/spiA.B
   /// @param spiChan SPI chip select
   /// @param spiBaud SPI Baud rate
   /// @param spiFlags SPI Flags
   /// @return File Descriptor
   int  Spi::init(unsigned spiChan, unsigned spiBaud, unsigned spiFlags)
   {
      /**
       * @brief 
       * 
       */
      char  spiMode;
      char  spiBits  = 8;
      char dev[32];

      spiMode  = spiFlags & 3;
      spiBits  = 8;

      sprintf(dev, "/dev/spidev0.1", spiChan);

      if ((this->fd_ = open(dev, O_RDWR)) < 0)
      {
         return 0;
      }

      if (ioctl(this->fd_, SPI_IOC_WR_MODE, &spiMode) < 0)
      {
         close_channel();
         return 0;
      }

      if (ioctl(this->fd_, SPI_IOC_WR_BITS_PER_WORD, &spiBits) < 0)
      {
         close_channel();
         return 0;
      }

      if (ioctl(this->fd_, SPI_IOC_WR_MAX_SPEED_HZ, &spiBaud) < 0)
      {
         close_channel();
         return 0;
      }

      return 1;
   }

   /// @brief SPI Read using ioctl
   /// @param buf Recieve buffer
   /// @param count size of recieve buffer
   /// @return Error
   int  Spi::read(char *buf, unsigned count)
   {
      int err;
      struct spi_ioc_transfer spi;

      memset(&spi, 0, sizeof(spi));

      spi.tx_buf        = (unsigned long) NULL;
      spi.rx_buf        = (unsigned long) buf;
      spi.len           = count;
      spi.speed_hz      = baud_;
      spi.delay_usecs   = 0;
      spi.bits_per_word = 8;
      spi.cs_change     = 0;

      err = ioctl(this->fd_, SPI_IOC_MESSAGE(1), &spi);

      return err;
   }

   /// @brief SPI write using ioctl
   /// @param buf Recieve buffer
   /// @param count size of transfer buffer
   /// @return Error
   int  Spi::write(char *buf, unsigned count)
   {
      int err;
      struct spi_ioc_transfer spi;

      memset(&spi, 0, sizeof(spi));

      spi.tx_buf        = (unsigned long) buf;
      spi.rx_buf        = (unsigned long) NULL;
      spi.len           = count;
      spi.speed_hz      = this->baud_;
      spi.delay_usecs   = 0;
      spi.bits_per_word = 8;
      spi.cs_change     = 0;

      err = ioctl(this->fd_, SPI_IOC_MESSAGE(1), &spi);

      return err;
   }

   /// @brief Full duplex SPI transfer
   /// @param txBuf Transfer buffer
   /// @param rxBuf Recieve buffer
   /// @param count size of buffers
   /// @return Error
   int  Spi::transfer(uint8_t *txBuf, uint8_t *rxBuf, unsigned count)
   {
      /**
       * @brief Full duplex tranfer with given speed and buffers on file descriptor
       * 
       */
      int err;
      struct spi_ioc_transfer spi;

      memset(&spi, 0, sizeof(spi));

      spi.tx_buf        = (unsigned long) txBuf;
      spi.rx_buf        = (unsigned long) rxBuf;
      spi.len           = count;
      spi.speed_hz      = this->baud_;
      spi.delay_usecs   = 0;
      spi.bits_per_word = 8;
      spi.cs_change     = 0;

      if (this->fd_ > 0) {
         err = ioctl(this->fd_, SPI_IOC_MESSAGE(1), &spi);
      } else {
         return -1;
      }

      return err;
   }
}