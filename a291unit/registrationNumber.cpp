#include "registrationNumber.h"

a291unit::RegistrationNumber::RegistrationNumber(uint8_t registrationNumber[]) {
    for(int i=0; i<RNBR_BYT_SIZE; i++) mRegistrationId[i] = registrationNumber[i];

    if(mRegistrationId[RNBR_FAMILY_BYTE] == 0x28) mFamilyName = "DS18B20";
    else mFamilyName = "unknown";
}

bool a291unit::RegistrationNumber::compareMatch(RegistrationNumber* regNumber) {
    bool match = true;
    for(int i=0; i<RNBR_BYT_SIZE; i++) {
        if(regNumber->mRegistrationId[i] != this->mRegistrationId[i]) match = false;
    }
    
    return match;
}

const std::string a291unit::RegistrationNumber::getRegistrationString() {
    std::string regString = "";

    for(int i=RNBR_BYT_SIZE-1; i>=0; i--) {
        if(i<7) regString += ':';
        char b[5];
        sprintf(b, "%02x", mRegistrationId[i]);
        regString += b;
    }
    
    return regString;
}
