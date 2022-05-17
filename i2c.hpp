//
//
#ifndef I2CDRIVER_H
#define I2CDRIVER_H

#include <stdint.h>

class I2cDriver
{
    public:
        I2cDriver(const char* dev);
        ~I2cDriver();
        bool Ready();
        int Read8(uint8_t &val, uint8_t addr, uint8_t reg);
        int Write8(uint8_t val, uint8_t addr, uint8_t reg);
        int Read2(uint16_t &val, uint8_t addr, uint8_t reg);
        int Read16(uint16_t &val, uint8_t addr, uint8_t reg);
        int Write16(uint16_t val, uint8_t addr, uint8_t reg);
        int Read16reg16(uint16_t &val, uint8_t addr, uint16_t reg);
        int Write8reg16(uint8_t val, uint8_t addr, uint16_t reg);
        int ReadNreg16(uint8_t *inbuf, uint8_t addr, uint16_t reg, uint8_t length);

    private:
        int fd;
};

#endif
