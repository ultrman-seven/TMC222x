#include "TMC222x.hpp"
#include "wySys.hpp"

void stepDelay(volatile uint32_t t)
{
    // volatile uint32_t dcnt;
    while (t--)
    {
        // dcnt = 1;
        // while (dcnt--)
        //     ;
    }
}

void TMC222x::step(int32_t s)
{
    if (s < 0)
    {
        s = -s;
        this->dir = 0;
    }
    else
        this->dir = 1;
    while (s--)
    {
        stepPin = 1;
        stepDelay(2);
        stepPin = 0;
        stepDelay(2);
    }
}

const int __speedStep = 0x5000;
// const int __speedStep = 0x500;

void TMC222x::run(bool isForward)
{
    uint32_t cnt;
    int32_t spdStp;
    int32_t spd = 0;
    int32_t uSpd;

    // spdStp = (dir > 0) ? (__speedStep) : (-__speedStep);
    // m->diagEnable = 0;
    isAccelerating = true;
    __motorUartSendData(0x22, 0);
    sys::delayMs(2);
    *this = 1;

    // spdStp = __speedStep;
    spdStp = this->speedStep;
    if (isForward == 0)
        spdStp *= -1;
    if (speed < 0)
    {
        spdStp *= -1;
        uSpd = -speed;
    }
    else
        uSpd = speed;
    cnt = uSpd / this->speedStep;
    while (cnt--)
    {
        spd += spdStp;
        __motorUartSendData(0x22, spd);
        sys::delayMs(2);
    }
    isAccelerating = false;
}

void TMC222x::stop()
{
    *this = 0;
    __motorUartSendData(0x22, 0);
}

TMC222x::TMC222x(
    const char *_en, uint8_t _id, void (*uSend)(uint8_t *, uint8_t),
    const char *_stp, const char *_dir, const char *lim)
    : id(_id), en(GPIO::GpioPin(_en)), uartSend(uSend)
{
    *this = 0;
    if (nullptr != _stp)
        stepPin.reInit(_stp);
    if (nullptr != _dir)
        dir.reInit(_dir);
    if (nullptr != _stp)
        limit.reInit(lim);
    this->speedStep = __speedStep;
}

uint8_t _TMC_CRC8(uint8_t *datagram, uint8_t datagramLength)
{
    int i, j;
    uint8_t crc = 0;
    uint8_t currentByte;
    for (i = 0; i < (datagramLength); i++)
    {
        currentByte = datagram[i];
        // Execute for all bytes of a message
        // Retrieve a byte to be sent from Array
        for (j = 0; j < 8; j++)
        {
            if ((crc >> 7) ^ (currentByte & 0x01)) // update CRC based result of XOR operation
                crc = (crc << 1) ^ 0x07;
            else
                crc = (crc << 1);
            currentByte = currentByte >> 1;
        } // for CRC bit
    } // for message byte
    return crc;
}

void TMC222x::__motorUartReadData(uint8_t reg)
{
    uint8_t tmp[4];
    tmp[0] = 0x05;
    tmp[1] = id;
    tmp[2] = reg & 0x7f;
    tmp[3] = _TMC_CRC8(tmp, 3);
    uartSend(tmp, 4);
}

inline void TMC222x::__motorUartSendData(uint8_t reg, uint32_t dat)
{
    uint8_t writeData[8];

    writeData[0] = 0x05;                    // Sync byte
    writeData[1] = id;                      // Slave address
    writeData[2] = reg | 0x80;              // Register address with write bit set
    writeData[3] = dat >> 24;               // Register Data
    writeData[4] = dat >> 16;               // Register Data
    writeData[5] = dat >> 8;                // Register Data
    writeData[6] = dat & 0xFF;              // Register Data
    writeData[7] = _TMC_CRC8(writeData, 7); // Cyclic redundancy check

    uartSend(writeData, 8);
}

#define TMC_REG_ADD_I_Hold_I_Run 0x10
#define TMC_REG_ADD_T_Step 0x12
#define TMC_REG_ADD_Speed 0x22
#define TMC_REG_ADD_T_Cool_Thrs 0x14
#define TMC_REG_ADD_SG_Thrs 0x40
#define TMC_REG_ADD_G_Cfg 0x00
#define TMC_REG_ADD_Chop_Cfg 0x6c
#define TMC_REG_ADD_PWM_Cfg 0x70
#define TMC_REG_ADD_
// #define TMC_REG_InitData_I_Hold_I_Run
#define TMC_REG_InitData_G_Cfg 0x1c1
// #define TMC_REG_InitData_

const uint32_t __TMC_InitData[] = {
    0x1c0,
    ((0x7 << 16) | (18 << 8) | (15)),
    0,
    // 0x10010053,
    0x10000053,
    0xC10D0024};
// ,
// 0x23,
// 160};

// 0x1c0, ((0x7 << 16) | (18 << 8) | (15)), 0, 0, 0x10000053, 0xC10D0024, 0x253, 21};
// 0x1c0, ((0x7 << 16) | (18 << 8) | (15)), 0, 0, 0x10000053, 0xC10D0024, 0x5000, 110};
// 0x1c0, ((0x7 << 16) | (16 << 8) | (15)), 0, 0, 0x10000053, 0xC10D0024, 0x1E000, 255};
// 0x1c0, ((0x7 << 16) | (16 << 8) | (15)), 0, 0, 0x10000053, 0xC10D0024, 0x1E000, 0x3000};
// 0x1c0, ((0x7 << 16) | (16 << 8) | (15)), 0, 0, 0x10000053, 0xC10D0024, 0xe0000, 155};
const uint8_t __TMC_InitReg[] = {
    TMC_REG_ADD_G_Cfg,
    TMC_REG_ADD_I_Hold_I_Run,
    0x13,
    TMC_REG_ADD_Chop_Cfg,
    TMC_REG_ADD_PWM_Cfg};
// ,
//     TMC_REG_ADD_T_Cool_Thrs,
//     TMC_REG_ADD_SG_Thrs};

void TMC222x::setStalledPara(uint32_t Tcoolthrs, uint8_t Sgthrs)
{
    __motorUartSendData(TMC_REG_ADD_T_Cool_Thrs, Tcoolthrs);
    sys::delayMs(10);
    __motorUartSendData(TMC_REG_ADD_SG_Thrs, Sgthrs);
    sys::delayMs(10);
}

void TMC222x::setRunningCurrent(uint8_t c)
{
    __motorUartSendData(TMC_REG_ADD_I_Hold_I_Run, ((0x7 << 16) | (c << 8) | (15)));
    sys::delayMs(10);
}

void TMC222x::init()
{
    uint8_t cnt = 0;
    *this = 0;

    // while (cnt < 7)
    while (cnt < 5)
    {
        __motorUartSendData(__TMC_InitReg[cnt], __TMC_InitData[cnt]);
        ++cnt;
        // sys::delayMs(100);
        sys::delayMs(10);
    }
}
