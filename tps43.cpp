// Handler for Azoteq TPS43 touch pad module, which is based on IQS572 device
//
#include <iostream>
#include <chrono>
#include "tps43.hpp"
#include "IQS5xx.h"

namespace Azoteq
{
    #define DEV_ADDR       0x74

    TPS43::TPS43(I2cDriver &drv)
    {
        i2c = &drv;
        AckReset();
        // fetch std::future object associated with promise
        std::future<void> futureObj = exitSignal.get_future();
        // start worker thread passing future object to it
        proc_thread = std::thread(&TPS43::ProcessingThread, this, std::move(futureObj));
    }

    TPS43::~TPS43()
    {
        // signal thread to stop
        exitSignal.set_value();
        // wait for thread to join
        proc_thread.join();
    }

    bool TPS43::GetGesture(Azoteq::tGesture &gest)
    {
        std::lock_guard<std::mutex> lock(fifomutex);
        if (fifo.empty())
            return false;
        gest = fifo.front();
        fifo.pop();
        return true;
    }

    // Communication is based on polling without using RDY pin.
    // Clock stretching must be supported by I2C hardware.
    // This function should be called at least as often as the Reporting Rates
    // configured for various modes of the device.
    void TPS43::ProcessingThread(std::future<void> futureObj)
    {
        std::cout << "TPS43 Thread Started" << std::endl;
        auto start_time = std::chrono::system_clock::now();

        while (futureObj.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout)
        {
            uint8_t buf[9];

            int err = i2c->ReadNreg16(buf, DEV_ADDR, GestureEvents0_adr, 9);
            // if not successful, retry a few more times:
            int retry = 3;
            while( err && retry!=0 )
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                err = i2c->ReadNreg16(buf, DEV_ADDR, GestureEvents0_adr, 9);
                retry--;
            }
            if (err == 0)
            {
                tGesture gesture;
                gesture.type = (buf[0] << 8) + buf[1];
                gesture.relX = (buf[5] << 8) + buf[6];
                gesture.relY = (buf[7] << 8) + buf[8];
                if (gesture.type != 0)
                {
                    gesture.timestamp =
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now() - start_time
                        );
                    std::lock_guard<std::mutex> lock(fifomutex);
                    fifo.push(gesture);
                    if (fifo.size() > 50)
                        fifo.pop();
                }
            }
            else
                std::cout << "TPS43 error: " << err << std::endl;

            if ((err = CloseComms()) != 0)
                std::cout << "CloseComm error: " << err << std::endl;

            // period of this thread
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::cout << "TPS43 Thread End" << std::endl;
    }

/*
    // Communication is based on polling without using RDY pin.
    // Clock stretching must be supported by I2C hardware.
    // This function should be called at least as often as the Reporting Rates
    // configured for various modes of the device.
    int TPS43::Process()
    {
        uint8_t buf[9];
        int err, retry=3;

        err = i2c->ReadNreg16(buf, DEV_ADDR, GestureEvents0_adr, 9);
        // if not successful, retry a few more times:
        while( err && retry!=0 )
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            err = i2c->ReadNreg16(buf, DEV_ADDR, GestureEvents0_adr, 9);
            retry--;
        }
        if ( err )
            return err;

        gesture.type = (buf[0] << 8) + buf[1];
        gesture.relX = (buf[5] << 8) + buf[6];
        gesture.relY = (buf[7] << 8) + buf[8];

        return CloseComms();
    }
*/

    //
    // Get Version
    //
    int TPS43::GetVersion()
    {
        uint8_t buf[6];
        int err, retry=3;

        err = i2c->ReadNreg16(buf, DEV_ADDR, ProductNumber_adr, 6);
        // if not successful, retry a few more times:
        while( err && retry!=0 )
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            err = i2c->ReadNreg16(buf, DEV_ADDR, ProductNumber_adr, 6);
            retry--;
        }
        if ( err )
            return err;

        version.product = (buf[0] << 8) + buf[1];
        version.project = (buf[2] << 8) + buf[3];
        version.v_major = buf[4];
        version.v_minor = buf[5];

        return CloseComms();
    }


    // Acknowledge the reset flag
    // This function simply sets the ACK_RESET bit found in the SYSTEM_CONTROL_0 register.
    // By setting this bit the SHOW_RESET flag is cleared in the SYSTEM_INFO_0 register.
    // During normal operation, the SHOW_RESET bit can be monitored and if it becomes set,
    // then an unexpected reset has occurred.
    // If any device configuration is needed, it must then be repeated.
    int TPS43::AckReset()
    {
        return i2c->Write8reg16(ACK_RESET, DEV_ADDR, SystemControl0_adr);
    }


    // Terminate communication window
    // The IQS5xx B000 does not close the communication window on the reception of the STOP bit.
    // Therefore this function sends the END COMMUNICATION WINDOW COMMAND (Please see datasheet).
    // RDY will go low after receiving any write to 0xEEEE followed by a STOP.
    int TPS43::CloseComms()
    {
        return i2c->Write8reg16(0x00, DEV_ADDR, 0xEEEE);
    }

}
