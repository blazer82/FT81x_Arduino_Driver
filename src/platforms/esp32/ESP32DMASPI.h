/**
 * MIT License
 * 
 * Copyright (c) 2019 Hideaki Tai
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

#pragma once
#ifndef ESP32DMASPI_H
#define ESP32DMASPI_H

#include <Arduino.h>
#include <SPI.h>
#include <driver/spi_master.h>

#include <deque>

namespace arduino {
namespace esp32 {
namespace spi {
namespace dma {

class ESP32DMASPI {
    spi_device_interface_config_t if_cfg;
    spi_bus_config_t bus_cfg;
    spi_device_handle_t handle;

    spi_host_device_t host{HSPI_HOST};
    uint8_t mode{SPI_MODE3};  // must be 1 or 3
    int dma_chan{1};          // must be 1 or 2
    int max_size{4094};       // default size
    uint32_t frequency{SPI_MASTER_FREQ_8M};

    std::deque<spi_transaction_t> transactions;
    int queue_size{1};

   public:
    bool begin(uint8_t spi_bus = HSPI, int8_t sck = -1, int8_t miso = -1, int8_t mosi = -1, int8_t ss = -1);
    bool end();

    uint8_t* allocDMABuffer(size_t s);

    // execute transaction and wait for transmission one by one
    size_t transfer(uint8_t* tx_buf, size_t size);
    size_t transfer(uint8_t* tx_buf, uint8_t* rx_buf, size_t size);

    // queueing transaction and execute simultaneously
    // wait (blocking) and timeout occurs if queue is full with transaction
    // (but designed not to queue transaction more than queue_size, so there is no timeout argument)
    bool queue(uint8_t* tx_buf, size_t size);
    bool queue(uint8_t* tx_buf, uint8_t* rx_buf, size_t size);
    void yield();

    // set these optional parameters before begin() if you want
    void setDataMode(uint8_t m);
    void setFrequency(uint32_t f);
    void setMaxTransferSize(int s);
    void setDMAChannel(int c);
    void setQueueSize(int s);

   private:
    void addTransaction(uint8_t* tx_buf, uint8_t* rx_buf, size_t size);
};

}  // namespace dma
}  // namespace spi
}  // namespace esp32
}  // namespace arduino

#ifndef ARDUINO_ESP32_DMA_SPI_NAMESPACE_BEGIN
#define ARDUINO_ESP32_DMA_SPI_NAMESPACE_BEGIN \
    namespace arduino {                       \
    namespace esp32 {                         \
    namespace spi {                           \
    namespace dma {
#endif
#ifndef ARDUINO_ESP32_DMA_SPI_NAMESPACE_END
#define ARDUINO_ESP32_DMA_SPI_NAMESPACE_END \
    }                                       \
    }                                       \
    }                                       \
    }
#endif

namespace ESP32DMASPI = arduino::esp32::spi::dma;

#endif  // ESP32DMASPI_H