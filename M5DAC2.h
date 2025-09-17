#pragma once
#include <DFRobot_GP8XXX.h>

class M5DAC2 {
public:
    M5DAC2(DFRobot_GP8XXX_IIC* dac);
    void begin();
    void setVoltageRaw(uint16_t ch0_mv, uint16_t ch1_mv);
    void softRamp(uint16_t target_mv, uint16_t rampStep = 5);

private:
    DFRobot_GP8XXX_IIC* dac;
    uint16_t ch1_mv = 360, ch0_mv = 720;
};
