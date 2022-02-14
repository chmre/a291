#ifndef REGISTRATIONMUMBER_H
#define REGISTRATIONMUMBER_H

#include <stdint.h>
#include <fstream>

#define RNBR_BIT_SIZE 64
#define RNBR_BYT_SIZE 8
#define RNBR_FAMILY_BYTE 0
#define RNBR_CRC_BYTE 7
#define RNBR_SERIAL_BYTE_LSB 1
#define RNBR_SERIAL_BYTE_SIZE 6

namespace a291unit {
    
    class RegistrationNumber {
    private:
        uint8_t mRegistrationId[RNBR_BYT_SIZE];
        std::string mFamilyName;
        
    public:
        RegistrationNumber(uint8_t registrationNumber[RNBR_BYT_SIZE]);
        
        bool compareMatch(RegistrationNumber* regNumber);
        
        const uint8_t * getRegistrationId() const {return mRegistrationId;}
        const std::string getRegistrationString();
        const uint8_t getFamilyCode() const {return mRegistrationId[RNBR_FAMILY_BYTE];}
        const std::string getFamilyName() const {return mFamilyName;}
        const uint8_t getCrc() const {return mRegistrationId[RNBR_CRC_BYTE];}
    };

    
}

#endif