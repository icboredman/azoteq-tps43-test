//
//

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

#include "i2c.hpp"

    I2cDriver::I2cDriver(const char* dev)
    {
        fd = -1;
        if ((fd = open(dev, O_RDWR)) < 0)
        {
            //std::cout << "Failed to open the bus." << std::endl;
            /* ERROR HANDLING; you can check errno to see what went wrong */
            return;
        }
    }

    I2cDriver::~I2cDriver()
    {
        close(fd);
        fd = -1;
    }

    bool I2cDriver::Ready()
    {
        return (fd > 0);
    }

    int I2cDriver::Read8(uint8_t &val, uint8_t addr, uint8_t reg)
    {
        if (fd < 0)
            return -1;

        uint8_t outbuf[1], inbuf[1];
        outbuf[0] = reg;
        inbuf[0] = 0;

        struct i2c_msg msgs[2];
        struct i2c_rdwr_ioctl_data msgset[1];

        msgs[0].addr = addr;
        msgs[0].flags = 0;
        msgs[0].len = 1;
        msgs[0].buf = outbuf;

        msgs[1].addr = addr;
        msgs[1].flags = I2C_M_RD; // | I2C_M_NOSTART;
        msgs[1].len = 1;
        msgs[1].buf = inbuf;

        msgset[0].msgs = msgs;
        msgset[0].nmsgs = 2;

        if (ioctl(fd, I2C_RDWR, &msgset) < 0)
            return -2;

        val = inbuf[0];
        return 0;
    }

    int I2cDriver::Write8(uint8_t val, uint8_t addr, uint8_t reg)
    {
        if (fd < 0)
            return -1;

        uint8_t outbuf[2];
        outbuf[0] = reg;
        outbuf[1] = val;

        struct i2c_msg msgs[1];
        struct i2c_rdwr_ioctl_data msgset[1];

        msgs[0].addr = addr;
        msgs[0].flags = 0;
        msgs[0].len = 2;
        msgs[0].buf = outbuf;

        msgset[0].msgs = msgs;
        msgset[0].nmsgs = 1;

        if (ioctl(fd, I2C_RDWR, &msgset) < 0)
            return -2;

        return 0;
    }

    int I2cDriver::Read2(uint16_t &val, uint8_t addr, uint8_t reg)
    {
        uint8_t buf[2];
        int r;
        if ((r = Read8(buf[0], addr, reg)) < 0)
            return r;
        if ((r = Read8(buf[1], addr, reg+1)) < 0)
            return r*10;
        val = (buf[0] << 8) + buf[1];
        return 0;
    }

    int I2cDriver::Read16(uint16_t &val, uint8_t addr, uint8_t reg)
    {
        if (fd < 0)
            return -1;

        uint8_t outbuf[1], inbuf[2];
        outbuf[0] = reg;
        inbuf[0] = inbuf[1] = 0;

        struct i2c_msg msgs[2];
        struct i2c_rdwr_ioctl_data msgset[1];

        msgs[0].addr = addr;
        msgs[0].flags = 0;
        msgs[0].len = 1;
        msgs[0].buf = outbuf;

        msgs[1].addr = addr;
        msgs[1].flags = I2C_M_RD; // | I2C_M_NOSTART;
        msgs[1].len = 2;
        msgs[1].buf = inbuf;

        msgset[0].msgs = msgs;
        msgset[0].nmsgs = 2;

        if (ioctl(fd, I2C_RDWR, &msgset) < 0)
            return -2;

        val = (inbuf[0] << 8) + inbuf[1];
        return 0;
    }

    int I2cDriver::Write16(uint16_t val, uint8_t addr, uint8_t reg)
    {
        if (fd < 0)
            return -1;

        uint8_t outbuf[3];
        outbuf[0] = reg;
        outbuf[1] = val >> 8;
        outbuf[2] = val & 0xFF;

        struct i2c_msg msgs[1];
        struct i2c_rdwr_ioctl_data msgset[1];

        msgs[0].addr = addr;
        msgs[0].flags = 0;
        msgs[0].len = 3;
        msgs[0].buf = outbuf;

        msgset[0].msgs = msgs;
        msgset[0].nmsgs = 1;

        if (ioctl(fd, I2C_RDWR, &msgset) < 0)
            return -2;

        return 0;
    }

    int I2cDriver::Read16reg16(uint16_t &val, uint8_t addr, uint16_t reg)
    {
        if (fd < 0)
            return -1;

        uint8_t outbuf[2], inbuf[2];
        outbuf[0] = reg >> 8;
        outbuf[1] = reg & 0xFF;
        inbuf[0] = inbuf[1] = 0;

        struct i2c_msg msgs[2];
        struct i2c_rdwr_ioctl_data msgset[1];

        msgs[0].addr = addr;
        msgs[0].flags = 0;
        msgs[0].len = 2;
        msgs[0].buf = outbuf;

        msgs[1].addr = addr;
        msgs[1].flags = I2C_M_RD; // | I2C_M_NOSTART;
        msgs[1].len = 2;
        msgs[1].buf = inbuf;

        msgset[0].msgs = msgs;
        msgset[0].nmsgs = 2;

        if (ioctl(fd, I2C_RDWR, &msgset) < 0)
            return -2;

        val = (inbuf[0] << 8) + inbuf[1];
        return 0;
    }

    int I2cDriver::Write8reg16(uint8_t val, uint8_t addr, uint16_t reg)
    {
        if (fd < 0)
            return -1;

        uint8_t outbuf[3];
        outbuf[0] = reg >> 8;
        outbuf[1] = reg & 0xFF;
        outbuf[2] = val;

        struct i2c_msg msgs[1];
        struct i2c_rdwr_ioctl_data msgset[1];

        msgs[0].addr = addr;
        msgs[0].flags = 0;
        msgs[0].len = 3;
        msgs[0].buf = outbuf;

        msgset[0].msgs = msgs;
        msgset[0].nmsgs = 1;

        if (ioctl(fd, I2C_RDWR, &msgset) < 0)
            return -2;

        return 0;
    }

    int I2cDriver::ReadNreg16(uint8_t *inbuf, uint8_t addr, uint16_t reg, uint8_t length)
    {
        if (fd < 0)
            return -1;

        uint8_t outbuf[2];
        outbuf[0] = reg >> 8;
        outbuf[1] = reg & 0xFF;

        struct i2c_msg msgs[2];
        struct i2c_rdwr_ioctl_data msgset[1];

        msgs[0].addr = addr;
        msgs[0].flags = 0;
        msgs[0].len = 2;
        msgs[0].buf = outbuf;

        msgs[1].addr = addr;
        msgs[1].flags = I2C_M_RD; // | I2C_M_NOSTART;
        msgs[1].len = length;
        msgs[1].buf = inbuf;

        msgset[0].msgs = msgs;
        msgset[0].nmsgs = 2;

        if (ioctl(fd, I2C_RDWR, &msgset) < 0)
            return -2;

        return 0;
    }
