#ifndef B2288B5E_43D4_44AF_8D3B_B3C8D1D8D6BD
#define B2288B5E_43D4_44AF_8D3B_B3C8D1D8D6BD

#include "stdint.h"
#include "wyGpio.hpp"

class TMC222x
{
private:
    uint8_t id;
    GPIO::GpioPin stepPin;
    GPIO::GpioPin dir;
    GPIO::GpioPin en;
    GPIO::GpioPin limit;
    void (*uartSend)(uint8_t *dat, uint8_t len);
    inline void __motorUartSendData(uint8_t reg, uint32_t dat);

public:
    int32_t speed;
    // uint8_t isForward;
    TMC222x(const char *_en, uint8_t _id, void (*uSend)(uint8_t *, uint8_t),
            const char *_stp = nullptr, const char *_dir = nullptr, const char *lim = nullptr);
    void __motorUartReadData(uint8_t reg);
    // void setSpeed(int32_t speed);
    void operator=(bool s) { en = !s; }
    void init();
    void run(bool d = true);
    // void stop();
    void step(uint32_t s);
    // ~TMC222x();
};
#endif /* B2288B5E_43D4_44AF_8D3B_B3C8D1D8D6BD */
