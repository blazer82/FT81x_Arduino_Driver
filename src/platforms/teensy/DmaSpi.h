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

#ifndef DMASPI_H
#define DMASPI_H

#if (defined(__arm__) && defined(TEENSYDUINO))

#include <Arduino.h>
#include <SPI.h>
#include <util/atomic.h>

#include "ChipSelect.h"
#include "DMAChannel.h"

// #define DEBUG_DMASPI 1

#if defined(DEBUG_DMASPI)
#define DMASPI_PRINT(x)  \
    do {                 \
        Serial.printf x; \
        Serial.flush();  \
    } while (0);
#else
#define DMASPI_PRINT(x) \
    do {                \
    } while (0);
#endif

namespace DmaSpi {
/** \brief describes an SPI transfer
 *
 * Transfers are kept in a queue (intrusive linked list) until they are processed by the DmaSpi driver.
 *
 **/

class Transfer {
   public:
    /** \brief The Transfer's current state.
     *
     **/
    enum State {
        idle,       /**< The Transfer is idle, the DmaSpi has not seen it yet. **/
        eDone,      /**< The Transfer is done. **/
        pending,    /**< Queued, but not handled yet. **/
        inProgress, /**< The DmaSpi driver is currently busy executing this Transfer. **/
        error       /**< An error occured. **/
    };

    /** \brief Creates a Transfer object.
     * \param pSource pointer to the data source. If this is nullptr, the fill value is used instead.
     * \param transferCount the number of SPI transfers to perform.
     * \param pDest pointer to the data sink. If this is nullptr, data received from the slave will be discarded.
     * \param fill if pSource is nullptr, this value is sent to the slave instead.
     * \param cs pointer to a chip select object.
     *   If not nullptr, cs->select() is called when the Transfer is started and cs->deselect() is called when the Transfer is finished.
     **/
    Transfer(const uint8_t* pSource = nullptr, const uint16_t& transferCount = 0, volatile uint8_t* pDest = nullptr, const uint8_t& fill = 0,
             AbstractChipSelect* cs = nullptr)
        : m_state(State::idle), m_pSource(pSource), m_transferCount(transferCount), m_pDest(pDest), m_fill(fill), m_pNext(nullptr), m_pSelect(cs) {
        DMASPI_PRINT(("Transfer @ %p\n", this));
    };

    /** \brief Check if the Transfer is busy, i.e. may not be modified.
     **/
    bool busy() const { return ((m_state == State::pending) || (m_state == State::inProgress) || (m_state == State::error)); }

    /** \brief Check if the Transfer is done.
     **/
    bool done() const { return (m_state == State::eDone); }

    //      private:
    volatile State m_state;
    const uint8_t* m_pSource;
    uint16_t m_transferCount;
    volatile uint8_t* m_pDest;
    uint8_t m_fill;
    Transfer* m_pNext;
    AbstractChipSelect* m_pSelect;
};
}  // namespace DmaSpi

template <typename DMASPI_INSTANCE, typename SPICLASS, SPICLASS& m_Spi>
class AbstractDmaSpi {
   public:
    using Transfer = DmaSpi::Transfer;

    /** \brief arduino-style initialization.
     *
     * During initialization, two DMA channels are allocated. If that fails, this function returns false.
     * If the channels could be allocated, those DMA channel fields that don't change during DMA SPI operation
     * are initialized to the values they will have at runtime.
     *
     * \return true if initialization was successful; false otherwise.
     * \see end()
     **/
    static bool begin() {
        if (init_count_ > 0) {
            return true;  // this is not particularly bad, so we can return true
        }
        init_count_++;
        DMASPI_PRINT(("DmaSpi::begin() : "));
        // create DMA channels, might fail
        if (!createDmaChannels()) {
            DMASPI_PRINT(("could not create DMA channels\n"));
            return false;
        }
        state_ = eStopped;
        // tx: known destination (SPI), no interrupt, finish silently
        begin_setup_txChannel();
        if (txChannel_()->error()) {
            destroyDmaChannels();
            DMASPI_PRINT(("tx channel error\n"));
            return false;
        }

        // rx: known source (SPI), interrupt on completion
        begin_setup_rxChannel();
        if (rxChannel_()->error()) {
            destroyDmaChannels();
            DMASPI_PRINT(("rx channel error\n"));
            return false;
        }

        return true;
    }

    static void begin_setup_txChannel() { DMASPI_INSTANCE::begin_setup_txChannel_impl(); }
    static void begin_setup_rxChannel() { DMASPI_INSTANCE::begin_setup_rxChannel_impl(); }

    /** \brief Allow the DMA SPI to start handling Transfers. This must be called after begin().
     * \see running()
     * \see busy()
     * \see stop()
     * \see stopping()
     * \see stopped()
     **/
    static void start() {
        DMASPI_PRINT(("DmaSpi::start() : state_ = "));
        switch (state_) {
            case eStopped:
                DMASPI_PRINT(("eStopped\n"));
                state_ = eRunning;
                beginPendingTransfer();
                break;

            case eRunning:
                DMASPI_PRINT(("eRunning\n"));
                break;

            case eStopping:
                DMASPI_PRINT(("eStopping\n"));
                state_ = eRunning;
                break;

            default:
                DMASPI_PRINT(("unknown\n"));
                state_ = eError;
                break;
        }
    }

    /** \brief check if the DMA SPI is in running state.
     * \return true if the DMA SPI is in running state, false otherwise.
     * \see start()
     * \see busy()
     * \see stop()
     * \see stopping()
     * \see stopped()
     **/
    static bool running() { return state_ == eRunning; }

    /** \brief register a Transfer to be handled by the DMA SPI.
     * \return false if the Transfer had an invalid transfer count (zero or greater than 32767), true otherwise.
     * \post the Transfer state is Transfer::State::pending, or Transfer::State::error if the transfer count was invalid.
     **/
    static bool registerTransfer(Transfer& transfer) {
        DMASPI_PRINT(("DmaSpi::registerTransfer(%p)\n", &transfer));
        if ((transfer.busy()) || (transfer.m_transferCount == 0)  // no zero length transfers allowed
            || (transfer.m_transferCount >= 0x8000))              // max CITER/BITER count with ELINK = 0 is 0x7FFF, so reject
        {
            DMASPI_PRINT(("  Transfer is busy or invalid, dropped\n"));
            transfer.m_state = Transfer::State::error;
            return false;
        }
        addTransferToQueue(transfer);
        if ((state_ == eRunning) && (!busy())) {
            DMASPI_PRINT(("  starting transfer\n"));
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { beginPendingTransfer(); }
        }

        return true;
    }

    /** \brief Check if the DMA SPI is busy, which means that it is currently handling a Transfer.
     \return true if a Transfer is being handled.
     * \see start()
     * \see running()
     * \see stop()
     * \see stopping()
     * \see stopped()
    **/
    static bool busy() { return (m_pCurrentTransfer != nullptr); }

    /** \brief Request the DMA SPI to stop handling Transfers.
     *
     * The stopping driver may finish a current Transfer, but it will then not start a new, pending one.
     * \see start()
     * \see running()
     * \see busy()
     * \see stopping()
     * \see stopped()
     **/
    static void stop() {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            switch (state_) {
                case eStopped:
                    break;
                case eRunning:
                    if (busy()) {
                        state_ = eStopping;
                    } else {
                        // this means that the DMA SPI simply has nothing to do
                        state_ = eStopped;
                    }
                    break;
                case eStopping:
                    break;
                default:
                    state_ = eError;
                    break;
            }
        }
    }

    /** \brief See if the DMA SPI is currently switching from running to stopped state
     * \return true if the DMA SPI is switching from running to stopped state
     * \see start()
     * \see running()
     * \see busy()
     * \see stop()
     * \see stopped()
     **/
    static bool stopping() { return (state_ == eStopping); }

    /** \brief See if the DMA SPI is stopped
     * \return true if the DMA SPI is in stopped state, i.e. not handling pending Transfers
     * \see start()
     * \see running()
     * \see busy()
     * \see stop()
     * \see stopping()
     **/
    static bool stopped() { return (state_ == eStopped); }

    /** \brief Shut down the DMA SPI
     *
     * Deallocates DMA channels and sets the internal state to error (this might not be an intelligent name for that)
     * \see begin()
     **/
    static void end() {
        if (init_count_ == 0) {
            state_ = eError;
            return;
        }
        if (init_count_ == 1) {
            init_count_--;
            destroyDmaChannels();
            state_ = eError;
            return;
        } else {
            init_count_--;
            return;
        }
    }

    /** \brief get the last value that was read from a slave, but discarded because the Transfer didn't specify a sink
     **/
    static uint8_t devNull() { return m_devNull; }

   protected:
    enum EState { eStopped,
                  eRunning,
                  eStopping,
                  eError };

    static void addTransferToQueue(Transfer& transfer) {
        transfer.m_state = Transfer::State::pending;
        transfer.m_pNext = nullptr;
        DMASPI_PRINT(("  DmaSpi::addTransferToQueue() : queueing transfer\n"));
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            if (m_pNextTransfer == nullptr) {
                m_pNextTransfer = &transfer;
            } else {
                m_pLastTransfer->m_pNext = &transfer;
            }
            m_pLastTransfer = &transfer;
        }
    }

    static void post_finishCurrentTransfer() { DMASPI_INSTANCE::post_finishCurrentTransfer_impl(); }

    static void finishCurrentTransfer() {
        if (m_pCurrentTransfer->m_pSelect != nullptr) {
            m_pCurrentTransfer->m_pSelect->deselect();
        } else {
            m_Spi.endTransaction();
        }
        m_pCurrentTransfer->m_state = Transfer::State::eDone;
        DMASPI_PRINT(("  finishCurrentTransfer() @ %p\n", m_pCurrentTransfer));
        m_pCurrentTransfer = nullptr;
        post_finishCurrentTransfer();
    }

    static bool createDmaChannels() {
        if (txChannel_() == nullptr) {
            return false;
        }
        if (rxChannel_() == nullptr) {
            delete txChannel_();
            return false;
        }
        return true;
    }

    static void destroyDmaChannels() {
        if (rxChannel_() != nullptr) {
            delete rxChannel_();
        }
        if (txChannel_() != nullptr) {
            delete txChannel_();
        }
    }

    static DMAChannel* rxChannel_() {
        static DMAChannel* pChannel = new DMAChannel();
        return pChannel;
    }

    static DMAChannel* txChannel_() {
        static DMAChannel* pChannel = new DMAChannel();
        return pChannel;
    }

    static void rxIsr_() {
        DMASPI_PRINT(("DmaSpi::rxIsr_()\n"));
        rxChannel_()->clearInterrupt();
        // end current transfer: deselect and mark as done
        finishCurrentTransfer();

        DMASPI_PRINT(("  state = "));
        switch (state_) {
            case eStopped:  // this should not happen!
                DMASPI_PRINT(("eStopped\n"));
                state_ = eError;
                break;
            case eRunning:
                DMASPI_PRINT(("eRunning\n"));
                beginPendingTransfer();
                break;
            case eStopping:
                DMASPI_PRINT(("eStopping\n"));
                state_ = eStopped;
                break;
            case eError:
                DMASPI_PRINT(("eError\n"));
                break;
            default:
                DMASPI_PRINT(("eUnknown\n"));
                state_ = eError;
                break;
        }
    }

    static void pre_cs() { DMASPI_INSTANCE::pre_cs_impl(); }
    static void post_cs() { DMASPI_INSTANCE::post_cs_impl(); }

    static void beginPendingTransfer() {
        if (m_pNextTransfer == nullptr) {
            DMASPI_PRINT(("DmaSpi::beginNextTransfer: no pending transfer\n"));
            return;
        }

        m_pCurrentTransfer = m_pNextTransfer;
        DMASPI_PRINT(("DmaSpi::beginNextTransfer: starting transfer @ %p\n", m_pCurrentTransfer));
        m_pCurrentTransfer->m_state = Transfer::State::inProgress;
        m_pNextTransfer = m_pNextTransfer->m_pNext;
        if (m_pNextTransfer == nullptr) {
            DMASPI_PRINT(("  this was the last in the queue\n"));
            m_pLastTransfer = nullptr;
        }

        // configure Rx DMA
        if (m_pCurrentTransfer->m_pDest != nullptr) {
            // real data sink
            DMASPI_PRINT(("  real sink\n"));
            rxChannel_()->destinationBuffer(m_pCurrentTransfer->m_pDest, m_pCurrentTransfer->m_transferCount);
        } else {
            // dummy data sink
            DMASPI_PRINT(("  dummy sink\n"));
            rxChannel_()->destination(m_devNull);
            rxChannel_()->transferCount(m_pCurrentTransfer->m_transferCount);
        }

        // configure Tx DMA
        if (m_pCurrentTransfer->m_pSource != nullptr) {
            // real data source
            DMASPI_PRINT(("  real source\n"));
            txChannel_()->sourceBuffer(m_pCurrentTransfer->m_pSource, m_pCurrentTransfer->m_transferCount);
        } else {
            // dummy data source
            DMASPI_PRINT(("  dummy source\n"));
            txChannel_()->source(m_pCurrentTransfer->m_fill);
            txChannel_()->transferCount(m_pCurrentTransfer->m_transferCount);
        }

        pre_cs();

        // Select Chip
        if (m_pCurrentTransfer->m_pSelect != nullptr) {
            m_pCurrentTransfer->m_pSelect->select();
        } else {
            m_Spi.beginTransaction(SPISettings());
        }

        post_cs();
    }

    static size_t init_count_;
    static volatile EState state_;
    static Transfer* volatile m_pCurrentTransfer;
    static Transfer* volatile m_pNextTransfer;
    static Transfer* volatile m_pLastTransfer;
    static volatile uint8_t m_devNull;
    // static SPICLASS& m_Spi;
};

template <typename DMASPI_INSTANCE, typename SPICLASS, SPICLASS& m_Spi>
size_t AbstractDmaSpi<DMASPI_INSTANCE, SPICLASS, m_Spi>::init_count_ = 0;

template <typename DMASPI_INSTANCE, typename SPICLASS, SPICLASS& m_Spi>
volatile typename AbstractDmaSpi<DMASPI_INSTANCE, SPICLASS, m_Spi>::EState AbstractDmaSpi<DMASPI_INSTANCE, SPICLASS, m_Spi>::state_ = eError;

template <typename DMASPI_INSTANCE, typename SPICLASS, SPICLASS& m_Spi>
typename AbstractDmaSpi<DMASPI_INSTANCE, SPICLASS, m_Spi>::Transfer* volatile AbstractDmaSpi<DMASPI_INSTANCE, SPICLASS, m_Spi>::m_pNextTransfer = nullptr;

template <typename DMASPI_INSTANCE, typename SPICLASS, SPICLASS& m_Spi>
typename AbstractDmaSpi<DMASPI_INSTANCE, SPICLASS, m_Spi>::Transfer* volatile AbstractDmaSpi<DMASPI_INSTANCE, SPICLASS, m_Spi>::m_pCurrentTransfer = nullptr;

template <typename DMASPI_INSTANCE, typename SPICLASS, SPICLASS& m_Spi>
typename AbstractDmaSpi<DMASPI_INSTANCE, SPICLASS, m_Spi>::Transfer* volatile AbstractDmaSpi<DMASPI_INSTANCE, SPICLASS, m_Spi>::m_pLastTransfer = nullptr;

template <typename DMASPI_INSTANCE, typename SPICLASS, SPICLASS& m_Spi>
volatile uint8_t AbstractDmaSpi<DMASPI_INSTANCE, SPICLASS, m_Spi>::m_devNull = 0;

#if defined(KINETISK)

class DmaSpi0 : public AbstractDmaSpi<DmaSpi0, SPIClass, SPI> {
   public:
    static void begin_setup_txChannel_impl() {
        txChannel_()->disable();
        txChannel_()->destination((volatile uint8_t&)SPI0_PUSHR);
        txChannel_()->disableOnCompletion();
        txChannel_()->triggerAtHardwareEvent(DMAMUX_SOURCE_SPI0_TX);
    }

    static void begin_setup_rxChannel_impl() {
        rxChannel_()->disable();
        rxChannel_()->source((volatile uint8_t&)SPI0_POPR);
        rxChannel_()->disableOnCompletion();
        rxChannel_()->triggerAtHardwareEvent(DMAMUX_SOURCE_SPI0_RX);
        rxChannel_()->attachInterrupt(rxIsr_);
        rxChannel_()->interruptAtCompletion();
    }

    static void pre_cs_impl() {
        SPI0_SR = 0xFF0F0000;
        SPI0_RSER = SPI_RSER_RFDF_RE | SPI_RSER_RFDF_DIRS | SPI_RSER_TFFF_RE | SPI_RSER_TFFF_DIRS;
    }

    static void post_cs_impl() {
        rxChannel_()->enable();
        txChannel_()->enable();
    }

    static void post_finishCurrentTransfer_impl() {
        SPI0_RSER = 0;
        SPI0_SR = 0xFF0F0000;
    }

   private:
};

extern DmaSpi0 DMASPI0;

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)

class DmaSpi1 : public AbstractDmaSpi<DmaSpi1, SPIClass, SPI1> {
   public:
    static void begin_setup_txChannel_impl() {
        txChannel_()->disable();
        txChannel_()->destination((volatile uint8_t&)SPI1_PUSHR);
        txChannel_()->disableOnCompletion();
        txChannel_()->triggerAtHardwareEvent(DMAMUX_SOURCE_SPI1_TX);
    }

    static void begin_setup_rxChannel_impl() {
        rxChannel_()->disable();
        rxChannel_()->source((volatile uint8_t&)SPI1_POPR);
        rxChannel_()->disableOnCompletion();
        rxChannel_()->triggerAtHardwareEvent(DMAMUX_SOURCE_SPI1_RX);
        rxChannel_()->attachInterrupt(rxIsr_);
        rxChannel_()->interruptAtCompletion();
    }

    static void pre_cs_impl() {
        SPI1_SR = 0xFF0F0000;
        SPI1_RSER = SPI_RSER_RFDF_RE | SPI_RSER_RFDF_DIRS | SPI_RSER_TFFF_RE | SPI_RSER_TFFF_DIRS;
    }

    static void post_cs_impl() {
        rxChannel_()->enable();
        txChannel_()->enable();
    }

    static void post_finishCurrentTransfer_impl() {
        SPI1_RSER = 0;
        SPI1_SR = 0xFF0F0000;
    }

   private:
};

extern DmaSpi1 DMASPI1;
// extern DmaSpi2 DMASPI2;
#endif  // defined(__MK64FX512__) || defined(__MK66FX1M0__)

#elif defined(KINETISL)
class DmaSpi0 : public AbstractDmaSpi<DmaSpi0, SPIClass, SPI> {
   public:
    static void begin_setup_txChannel_impl() {
        txChannel_()->disable();
        txChannel_()->destination((volatile uint8_t&)SPI0_DL);
        txChannel_()->disableOnCompletion();
        txChannel_()->triggerAtHardwareEvent(DMAMUX_SOURCE_SPI0_TX);
    }

    static void begin_setup_rxChannel_impl() {
        rxChannel_()->disable();
        rxChannel_()->source((volatile uint8_t&)SPI0_DL);
        rxChannel_()->disableOnCompletion();
        rxChannel_()->triggerAtHardwareEvent(DMAMUX_SOURCE_SPI0_RX);
        rxChannel_()->attachInterrupt(rxIsr_);
        rxChannel_()->interruptAtCompletion();
    }

    static void pre_cs_impl() {
        // disable SPI and enable SPI DMA requests
        SPI0_C1 &= ~(SPI_C1_SPE);
        SPI0_C2 |= SPI_C2_TXDMAE | SPI_C2_RXDMAE;
    }

    static void post_cs_impl() {
        rxChannel_()->enable();
        txChannel_()->enable();
    }

    static void post_finishCurrentTransfer_impl() {
        SPI0_C2 = 0;
        txChannel_()->clearComplete();
        rxChannel_()->clearComplete();
    }

   private:
};

class DmaSpi1 : public AbstractDmaSpi<DmaSpi1, SPIClass, SPI1> {
   public:
    static void begin_setup_txChannel_impl() {
        txChannel_()->disable();
        txChannel_()->destination((volatile uint8_t&)SPI1_DL);
        txChannel_()->disableOnCompletion();
        txChannel_()->triggerAtHardwareEvent(DMAMUX_SOURCE_SPI1_TX);
    }

    static void begin_setup_rxChannel_impl() {
        rxChannel_()->disable();
        rxChannel_()->source((volatile uint8_t&)SPI1_DL);
        rxChannel_()->disableOnCompletion();
        rxChannel_()->triggerAtHardwareEvent(DMAMUX_SOURCE_SPI1_RX);
        rxChannel_()->attachInterrupt(rxIsr_);
        rxChannel_()->interruptAtCompletion();
    }

    static void pre_cs_impl() {
        // disable SPI and enable SPI DMA requests
        SPI1_C1 &= ~(SPI_C1_SPE);
        SPI1_C2 |= SPI_C2_TXDMAE | SPI_C2_RXDMAE;
    }

    //  static void dumpCFG(const char *sz, uint32_t* p)
    //  {
    //    DMASPI_PRINT(("%s: %x %x %x %x \n", sz, p[0], p[1], p[2], p[3]));
    //  }

    static void post_cs_impl() {
        DMASPI_PRINT(("post_cs S C1 C2: %x %x %x\n", SPI1_S, SPI1_C1, SPI1_C2));
        //    dumpCFG("RX", (uint32_t*)(void*)rxChannel_()->CFG);
        //    dumpCFG("TX", (uint32_t*)(void*)txChannel_()->CFG);
        rxChannel_()->enable();
        txChannel_()->enable();
    }

    static void post_finishCurrentTransfer_impl() {
        SPI1_C2 = 0;
        txChannel_()->clearComplete();
        rxChannel_()->clearComplete();
    }

   private:
};

extern DmaSpi0 DMASPI0;
extern DmaSpi1 DMASPI1;

#elif defined(__IMXRT1062__)
class DmaSpi0 : public AbstractDmaSpi<DmaSpi0, SPIClass, SPI> {
   public:
    static void begin_setup_txChannel_impl() {
        txChannel_()->disable();
        txChannel_()->destination((volatile uint8_t&)IMXRT_LPSPI4_S.TDR);
        txChannel_()->disableOnCompletion();
        txChannel_()->triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI4_TX);
    }

    static void begin_setup_rxChannel_impl() {
        rxChannel_()->disable();
        rxChannel_()->source((volatile uint8_t&)IMXRT_LPSPI4_S.RDR);
        rxChannel_()->disableOnCompletion();
        rxChannel_()->triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI4_RX);
        rxChannel_()->attachInterrupt(rxIsr_);
        rxChannel_()->interruptAtCompletion();
    }

    static void pre_cs_impl() {
        IMXRT_LPSPI4_S.TCR = (IMXRT_LPSPI4_S.TCR & ~(LPSPI_TCR_FRAMESZ(31))) | LPSPI_TCR_FRAMESZ(7);
        IMXRT_LPSPI4_S.FCR = 0;

        // Lets try to output the first byte to make sure that we are in 8 bit mode...
        IMXRT_LPSPI4_S.DER = LPSPI_DER_TDDE | LPSPI_DER_RDDE;  // enable DMA on both TX and RX
        IMXRT_LPSPI4_S.SR = 0x3f00;                            // clear out all of the other status...
    }

    static void post_cs_impl() {
        rxChannel_()->enable();
        txChannel_()->enable();

        // dumpDMA_TCD(rxChannel_());
    }

    static void post_finishCurrentTransfer_impl() {
        IMXRT_LPSPI4_S.FCR = LPSPI_FCR_TXWATER(15);  // _spi_fcr_save; // restore the FSR status...
        IMXRT_LPSPI4_S.DER = 0;                      // DMA no longer doing TX (or RX)

        IMXRT_LPSPI4_S.CR = LPSPI_CR_MEN | LPSPI_CR_RRF | LPSPI_CR_RTF;  // actually clear both...
        IMXRT_LPSPI4_S.SR = 0x3f00;                                      // clear out all of the other status...
    }

   private:
};

extern DmaSpi0 DMASPI0;

class DmaSpi1 : public AbstractDmaSpi<DmaSpi1, SPIClass, SPI1> {
   public:
    static void begin_setup_txChannel_impl() {
        txChannel_()->disable();
        txChannel_()->destination((volatile uint8_t&)IMXRT_LPSPI3_S.TDR);
        txChannel_()->disableOnCompletion();
        txChannel_()->triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI3_TX);
    }

    static void begin_setup_rxChannel_impl() {
        rxChannel_()->disable();
        rxChannel_()->source((volatile uint8_t&)IMXRT_LPSPI3_S.RDR);
        rxChannel_()->disableOnCompletion();
        rxChannel_()->triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI3_RX);
        rxChannel_()->attachInterrupt(rxIsr_);
        rxChannel_()->interruptAtCompletion();
    }

    static void pre_cs_impl() {
        IMXRT_LPSPI3_S.TCR = (IMXRT_LPSPI3_S.TCR & ~(LPSPI_TCR_FRAMESZ(31))) | LPSPI_TCR_FRAMESZ(7);
        IMXRT_LPSPI3_S.FCR = 0;

        // Lets try to output the first byte to make sure that we are in 8 bit mode...
        IMXRT_LPSPI3_S.DER = LPSPI_DER_TDDE | LPSPI_DER_RDDE;  // enable DMA on both TX and RX
        IMXRT_LPSPI3_S.SR = 0x3f00;                            // clear out all of the other status...
    }

    static void post_cs_impl() {
        rxChannel_()->enable();
        txChannel_()->enable();
    }

    static void post_finishCurrentTransfer_impl() {
        IMXRT_LPSPI3_S.FCR = LPSPI_FCR_TXWATER(15);  // _spi_fcr_save; // restore the FSR status...
        IMXRT_LPSPI3_S.DER = 0;                      // DMA no longer doing TX (or RX)

        IMXRT_LPSPI3_S.CR = LPSPI_CR_MEN | LPSPI_CR_RRF | LPSPI_CR_RTF;  // actually clear both...
        IMXRT_LPSPI3_S.SR = 0x3f00;                                      // clear out all of the other status...
    }

   private:
};

class DmaSpi2 : public AbstractDmaSpi<DmaSpi2, SPIClass, SPI2> {
   public:
    static void begin_setup_txChannel_impl() {
        txChannel_()->disable();
        txChannel_()->destination((volatile uint8_t&)IMXRT_LPSPI1_S.TDR);
        txChannel_()->disableOnCompletion();
        txChannel_()->triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI1_TX);
    }

    static void begin_setup_rxChannel_impl() {
        rxChannel_()->disable();
        rxChannel_()->source((volatile uint8_t&)IMXRT_LPSPI1_S.RDR);
        rxChannel_()->disableOnCompletion();
        rxChannel_()->triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI1_RX);
        rxChannel_()->attachInterrupt(rxIsr_);
        rxChannel_()->interruptAtCompletion();
    }

    static void pre_cs_impl() {
        IMXRT_LPSPI1_S.TCR = (IMXRT_LPSPI1_S.TCR & ~(LPSPI_TCR_FRAMESZ(31))) | LPSPI_TCR_FRAMESZ(7);
        IMXRT_LPSPI1_S.FCR = 0;

        // Lets try to output the first byte to make sure that we are in 8 bit mode...
        IMXRT_LPSPI1_S.DER = LPSPI_DER_TDDE | LPSPI_DER_RDDE;  // enable DMA on both TX and RX
        IMXRT_LPSPI1_S.SR = 0x3f00;                            // clear out all of the other status...
    }

    static void post_cs_impl() {
        rxChannel_()->enable();
        txChannel_()->enable();
    }

    static void post_finishCurrentTransfer_impl() {
        IMXRT_LPSPI1_S.FCR = LPSPI_FCR_TXWATER(15);  // _spi_fcr_save; // restore the FSR status...
        IMXRT_LPSPI1_S.DER = 0;                      // DMA no longer doing TX (or RX)

        IMXRT_LPSPI1_S.CR = LPSPI_CR_MEN | LPSPI_CR_RRF | LPSPI_CR_RTF;  // actually clear both...
        IMXRT_LPSPI1_S.SR = 0x3f00;                                      // clear out all of the other status...
    }

   private:
};

extern DmaSpi1 DMASPI1;
// extern DmaSpi2 DMASPI2;

#else

#error Unknown chip

#endif  // KINETISK else KINETISL else IMXRT

#endif  // TEENSYDUINO

#endif  // DMASPI_H