/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 crteensy
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

#ifndef CHIPSELECT_H
#define CHIPSELECT_H

#include <core_pins.h>

/** \brief An abstract base class that provides an interface for chip select classes.
 **/
class AbstractChipSelect {
   public:
    /** \brief Called to select a chip. The implementing class can do other things as well.
     **/
    virtual void select() = 0;

    /** \brief Called to deselect a chip. The implementing class can do other things as well.
     **/
    virtual void deselect() = 0;

    /** \brief the virtual destructor needed to inherit from this class **/
    virtual ~AbstractChipSelect() {}
};

/** \brief "do nothing" chip select class **/
class DummyChipSelect : public AbstractChipSelect {
    void select() override {}

    void deselect() override {}
};

/** \brief "do nothing" chip select class that
 * outputs a message through Serial when something happens
 **/
class DebugChipSelect : public AbstractChipSelect {
    void select() override { Serial.println("Debug CS: select()"); }
    void deselect() override { Serial.println("Debug CS: deselect()"); }
};

/** \brief An active low chip select class. This also configures the given pin.
 * Warning: This class is hardcoded to manage a transaction on SPI (SPI0, that is).
 * If you want to use SPI1: Use AbstractChipSelect1 (see below)
 * If you want to use SPI2: Create AbstractChipSelect2 (adapt the implementation accordingly).
 * Something more flexible is on the way.
 **/
class ActiveLowChipSelect : public AbstractChipSelect {
   public:
    /** Configures a chip select pin for OUTPUT mode,
     * manages the chip selection and a corresponding SPI transaction
     *
     * The chip select pin is asserted \e after the SPI settings are applied
     * and deasserted before the SPI transaction ends.
     * \param pin the CS pin to use
     * \param settings which SPI settings to apply when the chip is selected
     **/
    ActiveLowChipSelect(const unsigned int& pin, const SPISettings& settings) : pin_(pin), settings_(settings) {
        pinMode(pin, OUTPUT);
        digitalWriteFast(pin, 1);
    }

    /** \brief begins an SPI transaction selects the chip (sets the pin to low) and
     **/
    void select() override {
        SPI.beginTransaction(settings_);
        digitalWriteFast(pin_, 0);
    }

    /** \brief deselects the chip (sets the pin to high) and ends the SPI transaction
     **/
    void deselect() override {
        digitalWriteFast(pin_, 1);
        SPI.endTransaction();
    }

   private:
    const unsigned int pin_;
    const SPISettings settings_;
};

class ActiveLowChipSelectStart : public AbstractChipSelect {
   public:
    /** Configures a chip select pin for OUTPUT mode,
     * manages the chip selection and a corresponding SPI transaction
     *
     * The chip select pin is asserted \e after the SPI settings are applied
     * and deasserted before the SPI transaction ends.
     * \param pin the CS pin to use
     * \param settings which SPI settings to apply when the chip is selected
     **/
    ActiveLowChipSelectStart(const unsigned int& pin, const SPISettings& settings) : pin_(pin), settings_(settings) {
        pinMode(pin, OUTPUT);
        digitalWriteFast(pin, 1);
    }

    /** \brief begins an SPI transaction selects the chip (sets the pin to low) and
     **/
    void select() override {
        SPI.beginTransaction(settings_);
        digitalWriteFast(pin_, 0);
    }

    void deselect() override {}

   private:
    const unsigned int pin_;
    const SPISettings settings_;
};

class ActiveLowChipSelectEnd : public AbstractChipSelect {
   public:
    /** Configures a chip select pin for OUTPUT mode,
     * manages the chip selection and a corresponding SPI transaction
     *
     * The chip select pin is asserted \e after the SPI settings are applied
     * and deasserted before the SPI transaction ends.
     * \param pin the CS pin to use
     * \param settings which SPI settings to apply when the chip is selected
     **/
    ActiveLowChipSelectEnd(const unsigned int& pin, const SPISettings& settings) : pin_(pin), settings_(settings) {
        pinMode(pin, OUTPUT);
        digitalWriteFast(pin, 1);
    }

    void select() override {}

    /** \brief deselects the chip (sets the pin to high) and ends the SPI transaction
     **/
    void deselect() override {
        digitalWriteFast(pin_, 1);
        SPI.endTransaction();
    }

   private:
    const unsigned int pin_;
    const SPISettings settings_;
};

#if defined(__MK64FX512__) || defined(__MK66FX1M0__) || defined(__IMXRT1062__) || defined(__MKL26Z64__)
class ActiveLowChipSelect1 : public AbstractChipSelect {
   public:
    /** Equivalent to AbstractChipSelect, but for SPI1.
     **/
    ActiveLowChipSelect1(const unsigned int& pin, const SPISettings& settings) : pin_(pin), settings_(settings) {
        pinMode(pin, OUTPUT);
        digitalWriteFast(pin, 1);
    }

    /** \brief begins an SPI transaction selects the chip (sets the pin to low) and
     **/
    void select() override {
        SPI1.beginTransaction(settings_);
        digitalWriteFast(pin_, 0);
    }

    /** \brief deselects the chip (sets the pin to high) and ends the SPI transaction
     **/
    void deselect() override {
        digitalWriteFast(pin_, 1);
        SPI1.endTransaction();
    }

   private:
    const unsigned int pin_;
    const SPISettings settings_;
};

#endif  // SPI1 only on some hardware
#endif  // CHIPSELECT_H
