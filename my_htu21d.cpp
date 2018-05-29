
#include "my_htu21d.h"
#include <Wire.h>

my_htu21d::my_htu21d(){
}

void my_htu21d::begin(void){
    uint8_t val = 0x81; //RH11/TEMP11
    Wire.begin();
    delay(200);
    Wire.beginTransmission(HTU21D_ADDRESS);
    Wire.write(WRITE_USER_REGISTER); //Write to the user register
    Wire.write(val); //Write the resolution bits
    Wire.endTransmission();
}

uint16_t my_htu21d::readValue(uint8_t deviceRegister){
    //Request a humidity reading
    Wire.beginTransmission(HTU21D_ADDRESS);
    Wire.write(deviceRegister);
    Wire.endTransmission();
    
    uint8_t toRead=0;
    uint8_t counter=0;
    for (counter = 0, toRead = 0 ; counter < MAX_COUNTER && toRead != 3 ; counter++)
    {
        delay(DELAY_INTERVAL);

        //three bytes, data(MSB) / data(LSB) / Checksum
        toRead = Wire.requestFrom(HTU21D_ADDRESS, 3);
    }
    
    if (counter == MAX_COUNTER){
        return ERROR;
    }
    
    uint8_t msb, lsb, checksum;
    msb = Wire.read();
    lsb = Wire.read();
    checksum = Wire.read();
    
    uint16_t rawValue = ((uint16_t) msb << 8) | (uint16_t) lsb;


    if (isCRC_OK(rawValue, checksum)) {
        return (uint16_t) (rawValue & ZERO_OUT_STATUS_BITS); // Zero out the status bits
    }
    else {
        return ERROR;
    }
    
    
}

float my_htu21d::readHumidity(void){
    uint16_t rawHumidity = readValue(HUMIDITY_REGISTER);
    
    if(rawHumidity == ERROR){
        return rawHumidity;
    }
    
    //Convert raw humidity value to the real one
       float realHumidity = rawHumidity * (125.0 / 65536.0) - 6.0; //2^16 = 65536
    
    if(realHumidity>100.0){
        realHumidity = 100.0;
    } else if(realHumidity<0.0){
        realHumidity = 0.0;
    }
    
    return realHumidity;
}

float my_htu21d::readTemperature(void){
    uint16_t rawTemperature = readValue(TEMPERATURE_REGISTER);

    if(rawTemperature == ERROR){
        return rawTemperature;
    }
    
    //Convert raw humidity value to the real one
    float realTemperature = rawTemperature * (175.72 / 65536.0) - 46.85;//2^16 = 65536
    
    return realTemperature;
}

bool my_htu21d::isCRC_OK(uint16_t rawMessage, uint8_t checksum) {
    
    uint32_t divisor = 305;// 305=100110001
    divisor <<= 15;
    uint32_t remainder = (uint32_t)rawMessage << 8; //Pad with 8 bits because we have to add in the check value
    remainder |= checksum;
    
    for (int i = 0 ; i < 16 ; i++){
        
        if ( remainder & (uint32_t)1 << (23 - i) ) //Check if there is a one in the left position
            remainder ^= divisor;
        
        divisor >>= 1; //Rotate the divisor max 16 times so that we have 8 bits left of a remainder
    }
    
    return remainder == 0;
}

