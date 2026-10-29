#include <Arduino.h>
#include <Wire.h>
#include "MAX17048.h"

MAX17048::MAX17048() {
    _wire = &Wire;
    _address = 0x36;
}

bool MAX17048::begin(TwoWire &wire, uint8_t address) {
    _wire = &wire;
    _address = address;
    
    // 不要重复初始化I2C总线，因为系统已经初始化过了
    // _wire->begin();
    
    // 检查芯片是否存在
    uint16_t version = 0;
    if (!readRegister(0x08, version)) {
        return false;
    }
    if (version == 0xFFFF || version == 0x0000) {
        return false;
    }
    
    return true;
}

float MAX17048::getSoC() {
    uint16_t soc = 0;
    if (!readRegister(0x04, soc)) {
        return -1.0; // 读取失败，返回无效值，避免把错误数据当成电量
    }
    float percent = (soc >> 8) + ((soc & 0xFF) / 256.0);
    if (percent < 0.0) percent = 0.0;
    if (percent > 100.0) percent = 100.0;
    return percent;
}

float MAX17048::getVoltage() {
    uint16_t voltage = 0;
    if (!readRegister(0x02, voltage)) {
        return -1.0;
    }
    return (voltage >> 4) * 1.25 / 1000.0;
}

float MAX17048::getCrate() {
    uint16_t crate = 0;
    if (!readRegister(0x16, crate)) {
        return 0.0;
    }
    int16_t signed_crate = (int16_t)crate;
    return signed_crate * 0.208 / 1000.0;
}

uint16_t MAX17048::getVersion() {
    uint16_t version = 0;
    readRegister(0x08, version);
    return version;
}

void MAX17048::reset() {
    writeRegister(0xFE, 0x5400);
}

bool MAX17048::readRegister(uint8_t reg, uint16_t &value) {
    if (_wire == nullptr) {
        return false;
    }
    
    _wire->beginTransmission(_address);
    _wire->write(reg);
    if (_wire->endTransmission(true) != 0) {
        return false;
    }
    
    if (_wire->requestFrom(_address, (uint8_t)2) != 2) {
        return false;
    }
    if (_wire->available() < 2) {
        return false;
    }
    
    value = (uint16_t)(_wire->read() & 0xFF) << 8;
    value |= (uint16_t)(_wire->read() & 0xFF);
    
    return true;
}

void MAX17048::writeRegister(uint8_t reg, uint16_t data) {
    _wire->beginTransmission(_address);
    _wire->write(reg);
    _wire->write(data >> 8);
    _wire->write(data & 0xFF);
    _wire->endTransmission();
}