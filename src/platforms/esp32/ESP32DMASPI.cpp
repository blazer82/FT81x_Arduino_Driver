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

#include "ESP32DMASPI.h"

ARDUINO_ESP32_DMA_SPI_NAMESPACE_BEGIN

bool ESP32DMASPI::begin(uint8_t spi_bus, int8_t sck, int8_t miso, int8_t mosi, int8_t ss) {
    if ((sck == -1) && (miso == -1) && (mosi == -1) && (ss == -1)) {
        bus_cfg.sclk_io_num = (spi_bus == VSPI) ? SCK : 14;
        bus_cfg.miso_io_num = (spi_bus == VSPI) ? MISO : 12;
        bus_cfg.mosi_io_num = (spi_bus == VSPI) ? MOSI : 13;
        if_cfg.spics_io_num = (spi_bus == VSPI) ? SS : 15;
    } else {
        bus_cfg.sclk_io_num = sck;
        bus_cfg.miso_io_num = miso;
        bus_cfg.mosi_io_num = mosi;
        if_cfg.spics_io_num = ss;
    }

    if_cfg.mode = mode;
    if_cfg.clock_speed_hz = frequency;
    if_cfg.queue_size = queue_size;
    if_cfg.flags = SPI_DEVICE_NO_DUMMY;
    // TODO: callback??
    if_cfg.pre_cb = NULL;
    if_cfg.post_cb = NULL;

    bus_cfg.max_transfer_sz = max_size;

    // make sure to use DMA buffer
    if ((dma_chan != 1) && (dma_chan != 2)) {
        printf("[WARNING] invalid DMA channel %d, force to set channel 1. make sure to select 1 or 2\n", dma_chan);
        dma_chan = 1;
    }
    if ((mode != SPI_MODE1) && (mode != SPI_MODE3)) {
        printf("[WARNING] invalid SPI channel %d, force to set SPI_MODE3. make sure to select MODE1 or MODE3 (because of DMA)\n", mode);
        mode = SPI_MODE3;
    }

    host = (spi_bus == HSPI) ? HSPI_HOST : VSPI_HOST;

    esp_err_t e = spi_bus_initialize(host, &bus_cfg, dma_chan);
    if (e != ESP_OK) {
        printf("[ERROR] SPI bus initialize failed : %d\n", e);
        return false;
    }

    e = spi_bus_add_device(host, &if_cfg, &handle);
    if (e != ESP_OK) {
        printf("[ERROR] SPI bus add device failed : %d\n", e);
        return false;
    }

    return true;
}

bool ESP32DMASPI::end() {
    esp_err_t e = spi_bus_remove_device(handle);
    if (e != ESP_OK) {
        printf("[ERROR] SPI bus remove device failed : %d\n", e);
        return false;
    }

    e = spi_bus_free(host);
    if (e != ESP_OK) {
        printf("[ERROR] SPI bus free failed : %d\n", e);
        return false;
    }

    return true;
}

uint8_t* ESP32DMASPI::allocDMABuffer(size_t s) {
    return (uint8_t*)heap_caps_malloc(s, MALLOC_CAP_DMA);
}

size_t ESP32DMASPI::transfer(uint8_t* tx_buf, size_t size) {
    return transfer(tx_buf, NULL, size);
}

size_t ESP32DMASPI::transfer(uint8_t* tx_buf, uint8_t* rx_buf, size_t size) {
    if (!transactions.empty()) {
        printf("[ERROR] can not execute transfer if queued transaction exits. queueed size = %d\n", transactions.size());
        return 0;
    }

    addTransaction(tx_buf, rx_buf, size);

    // send a spi transaction, wait for it to complete, and return the result
    esp_err_t e = spi_device_transmit(handle, &transactions.back());
    if (e != ESP_OK) {
        printf("[ERROR] SPI device transmit failed : %d\n", e);
        transactions.pop_back();
        return 0;
    }

    size_t len = transactions.back().rxlength / 8;
    transactions.pop_back();
    return len;
}

bool ESP32DMASPI::queue(uint8_t* tx_buf, size_t size) {
    return queue(tx_buf, NULL, size);
}

bool ESP32DMASPI::queue(uint8_t* tx_buf, uint8_t* rx_buf, size_t size) {
    if (transactions.size() >= queue_size) {
        printf("[WARNING] queue is full with transactions. discard new transaction request\n");
        return false;
    }

    addTransaction(tx_buf, rx_buf, size);
    esp_err_t e = spi_device_queue_trans(handle, &transactions.back(), portMAX_DELAY);

    return (e == ESP_OK);
}

void ESP32DMASPI::yield() {
    size_t n = transactions.size();
    for (uint8_t i = 0; i < n; ++i) {
        spi_transaction_t* r_trans;
        esp_err_t e = spi_device_get_trans_result(handle, &r_trans, portMAX_DELAY);
        if (e != ESP_OK)
            printf("[ERROR] SPI device get trans result failed %d / %d : %d\n", i, n, e);
        transactions.pop_front();
    }
}

void ESP32DMASPI::setFrequency(uint32_t f) {
    frequency = f;
}

void ESP32DMASPI::setDataMode(uint8_t m) {
    mode = m;
}

void ESP32DMASPI::setMaxTransferSize(int s) {
    max_size = s;
}

void ESP32DMASPI::setDMAChannel(int c) {
    dma_chan = c;  // 1 or 2 only
}

void ESP32DMASPI::setQueueSize(int s) {
    queue_size = s;
}

void ESP32DMASPI::addTransaction(uint8_t* tx_buf, uint8_t* rx_buf, size_t size) {
    transactions.emplace_back(spi_transaction_t());
    transactions.back().flags = 0;
    // transactions.back().cmd = ;
    // transactions.back().addr = ;
    transactions.back().length = 8 * size;  // in bit size
    // transactions.back().user = ;
    transactions.back().tx_buffer = tx_buf;
    transactions.back().rx_buffer = rx_buf;
}

ARDUINO_ESP32_DMA_SPI_NAMESPACE_END