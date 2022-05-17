//
//
#ifndef T_AZOTEQTPS43_HPP
#define T_AZOTEQTPS43_HPP

#include <stdint.h>
#include "i2c.hpp"
#include <queue>
#include <mutex>
#include <thread>
#include <future>
#include <chrono>

namespace Azoteq
{

    #define TPS43_GTYPE_NONE         0
    #define TPS43_GTYPE_SWIPE_Y_NEG  0x2000
    #define TPS43_GTYPE_SWIPE_Y_POS  0x1000
    #define TPS43_GTYPE_SWIPE_X_POS  0x0800
    #define TPS43_GTYPE_SWIPE_X_NEG  0x0400
    #define TPS43_GTYPE_TAP_N_HOLD   0x0200
    #define TPS43_GTYPE_SINGLE_TAP   0x0100
    #define TPS43_GTYPE_ZOOM         0x0004
    #define TPS43_GTYPE_SCROLL       0x0002
    #define TPS43_GTYPE_2F_TAP       0x0001

    typedef struct {
        uint16_t type;
        int16_t  relX;
        int16_t  relY;
        std::chrono::milliseconds timestamp;
    } tGesture;

    class TPS43
    {
        public:
            TPS43(I2cDriver &drv);
            ~TPS43();
            int GetVersion();
            bool GetGesture(Azoteq::tGesture &gest);

            struct {
                uint16_t product;
                uint16_t project;
                uint8_t  v_major;
                uint8_t  v_minor;
            } version;

        private:
            I2cDriver *i2c;
            int AckReset();
            int CloseComms();
            void ProcessingThread(std::future<void> futureObj);
            std::queue<tGesture> fifo;
            std::mutex fifomutex;
            std::thread proc_thread;
            std::promise<void> exitSignal;
    };

}

#endif
