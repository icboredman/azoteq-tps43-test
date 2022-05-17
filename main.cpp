#include <iostream> // cin, cout
#include <thread>
#include <chrono>

#include "i2c.hpp"
#include "tps43.hpp"

#define HEART_RATE_MS  50

int main (void)
{
    int err;

    I2cDriver i2c("/dev/i2c-2");
    if (!i2c.Ready())
    {
        std::cout << "Failed to init I2C communication.\n";
        return -1;
    }
    Azoteq::TPS43 tpad(i2c);
/*
    if ((err = tpad.GetVersion()) != 0)
        return err;
    std::cout << "Product:   " << tpad.version.product << std::endl;
    std::cout << "  Project: " << tpad.version.project << std::endl;
    std::cout << "  Version: " << int(tpad.version.v_major) << "." << int(tpad.version.v_minor) << std::endl << std::endl;
*/

    Azoteq::tGesture gesture;

    while (1)
    {
        while (tpad.GetGesture(gesture))
        {
            if (gesture.type != 0)
            {
                std::cout << "Type: " << std::hex << gesture.type << std::endl;
                std::cout << "   X: " << std::dec << gesture.relX << std::endl;
                std::cout << "   Y: " << std::dec << gesture.relY << std::endl;
                std::cout << "time: " << gesture.timestamp.count() << std::endl << std::endl;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(HEART_RATE_MS));
    }

    return 0;
}
