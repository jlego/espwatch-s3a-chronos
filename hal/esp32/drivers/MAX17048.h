/*
 * MAX17048 Arduino Library Header
 */

#ifndef MAX17048_h
#define MAX17048_h

#include "Arduino.h"
#include "Wire.h"

class MAX17048
{
public:
    MAX17048();
    bool begin(TwoWire &wire = Wire, uint8_t address = 0x36);
    
    float getSoC();           // 获取电池电量百分比
    float getVoltage();       // 获取电池电压
    float getCrate();         // 获取充放电倍率
    uint16_t getVersion();    // 获取芯片版本
    void reset();             // 复位芯片
    
private:
    TwoWire *_wire;
    uint8_t _address;
    
    bool readRegister(uint8_t reg, uint16_t &value);
    void writeRegister(uint8_t reg, uint16_t data);
};

#endif