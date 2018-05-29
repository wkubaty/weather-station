//
// Created by wojciech on 27.12.2017.
//
#include "Arduino.h"
#include <Wire.h>
#define HTU21D_ADDRESS 0x40  //Unshifted 7-bit I2C address for the sensor

#define ERROR 999

#define TEMPERATURE_REGISTER  0xF3
#define HUMIDITY_REGISTER  0xF5
#define WRITE_USER_REGISTER 0xE6

#define MAX_COUNTER 100
#define DELAY_INTERVAL 10
#define ZERO_OUT_STATUS_BITS 0xFFFC

class my_htu21d{
public:
    my_htu21d();
    void begin(void);
    void setResolution(uint8_t resBits);
    float readHumidity(void);
    float readTemperature(void);
    uint8_t readUserRegister(void);
    void writeUserRegister(uint8_t val);
    uint16_t readValue(uint8_t device_register);

private:
    bool isCRC_OK(uint16_t rawMessage, uint8_t checksum);

};

