#ifndef ONEWIRESEARCH_H
#define ONEWIRESEARCH_H

#include "controler.h"
#include "ds18b20Sensor.h"
#include "ds18b20SensorGroup.h"

#include <stdint.h>

#define CFG_WBUS_CTRL_USR_NAME "wire bus controler"
#define CFG_WBUS_CTRL_OBJ_NAME "WIREBUSCONTROLER"
#define CFG_WBUS_CTRL_MAX_GROUP 4
#define CFG_WBUS_CTRL_MAX_SENSOR 4


#define OWC_ADRESS_BIT_SIZE 64
#define OWC_ADRESS_BYTE_SIZE OWC_ADRESS_BIT_SIZE / 8

// reset time high and low are both 480uS
#define OWC_RESET_TIME_HIGH 480
#define OWC_RESET_TIME_LOW 480

// read write bit timeslot 60uS ... 120uS
#define OWC_TIME_SLOT 100

// recovery time 1uS
#define OWC_RECOVERY_TIME 5

#define OWC_BIT_WRITE_0_LOW_TIME 90
#define OWC_BIT_WRITE_1_LOW_TIME 2
#define OWC_BIT_READ_REQ_LOW_TIME 1
#define OWC_BIT_READ_DATA_VALID_TIME 10

#define OWC_ROM_CMD_SEARCH 0xf0
#define OWC_ROM_CMD_READ 0x33
#define OWC_ROM_CMD_MATCH 0x55
#define OWC_ROM_CMD_SKIP 0xcc
#define OWC_ROM_CMD_ALARM 0xec

#define OWC_FNC_CMD_CONVERT 0x44
#define OWC_FNC_CMD_WRITE_SCRATCHPAD 0x4e
#define OWC_FNC_CMD_READ_SCRATCHPAD 0xbe
#define OWC_FNC_CMD_COPY_SCRATCHPAD 0x48
#define OWC_FNC_CMD_RECALL_E2 0xb8
#define OWC_FNC_CMD_READ_POWER_SUPPLY 0xb4

// the maximum converting time is specified as 750 mSeconds
// convert timing is checked every 50 mSec untill done but a maiximun of 40 times
// the resulting timeout for a conversion is therfore 2 seconds 
#define DS18B_CONVERT_CHECK_mS 50
#define DS18B_CONVERT_LOOP 40

namespace a291unit {
    
    static uint8_t DS_CRC_TABLE[] {
          0,  94, 188, 226,  97,  63, 221, 131, 194, 156, 126,  32, 163, 253,  31,  65,
        157, 195,  33, 127, 252, 162,  64,  30,  95,   1, 227, 189,  62,  96, 130, 220,
         35, 125, 159, 193,  66,  28, 254, 160, 225, 191,  93,   3, 128, 222,  60,  98,
        190, 224,   2,  92, 223, 129,  99,  61, 124,  34, 192, 158,  29,  67, 161, 255,
         70,  24, 250, 164,  39, 121, 155, 197, 132, 218,  56, 102, 229, 187,  89,   7,
        219, 133, 103,  57, 186, 228,   6,  88,  25,  71, 165, 251, 120,  38, 196, 154,
        101,  59, 217, 135,   4,  90, 184, 230, 167, 249,  27,  69, 198, 152, 122,  36,
        248, 166,  68,  26, 153, 199,  37, 123,  58, 100, 134, 216,  91,   5, 231, 185,
        140, 210,  48, 110, 237, 179,  81,  15,  78,  16, 242, 172,  47, 113, 147, 205,
         17,  79, 173, 243, 112,  46, 204, 146, 211, 141, 111,  49, 178, 236,  14,  80,
        175, 241,  19,  77, 206, 144, 114,  44, 109,  51, 209, 143,  12,  82, 176, 238,
         50, 108, 142, 208,  83,  13, 239, 177, 240, 174,  76,  18, 145, 207,  45, 115,
        202, 148, 118,  40, 171, 245,  23,  73,   8,  86, 180, 234, 105,  55, 213, 139,
         87,   9, 235, 181,  54, 104, 138, 212, 149, 203,  41, 119, 244, 170,  72,  22,
        233, 183,  85,  11, 136, 214,  52, 106,  43, 117, 151, 201,  74,  20, 246, 168,
        116,  42, 200, 150,  21,  75, 169, 247, 182, 232,  10,  84, 215, 137, 107,  53 };
    
    static uint8_t ds_crc_add(uint8_t crc, uint8_t value) {
        return a291unit::DS_CRC_TABLE[crc ^ value];
    }
    
    
    class WireBusControler : public Controler {
    private:
        
        // family based sensor groups
        int mSensorGroupIterationIndex;
		bool newGroup;
        Ds18b20SensorGroup* mSensorGroupList[CFG_WBUS_CTRL_MAX_GROUP];
        
        uint8_t mCrc;
    
    protected:
        // common one wire functions
        uint8_t readBit(uint8_t pinNumber);
        uint8_t readBit(uint8_t pinNumber, int nbrOfBits);
        uint8_t readByte(uint8_t pinNumber);
 
        void writeBit(uint8_t pinNumber, uint8_t bitValue);
        void writeByte(uint8_t pinNumber, uint8_t databyte);
 
        bool initPuls(uint8_t gpioNbr);
        
        // sensor group (family)
        Ds18b20SensorGroup * getSensorGroup(uint8_t gpioPin);
        
        int pullScratchpad(uint8_t pinNumber, uint8_t * data, uint8_t len);
        int selectDevice(Ds18b20Sensor* sensor);
        bool isExternalPowerd(Ds18b20Sensor* sensor);
        SensorOperationResult readTemperature(Ds18b20Sensor* sensor);
        
		void set_sensor_data(Ds18b20Sensor* sensor, const uint8_t rawData[9], const int rawDataLen);
		
		int initializeGroup(Ds18b20SensorGroup* sensorGroup);

		// wire bus scan attributes and functions
		#ifdef DEBUG
		int scanCount;
		#endif
		bool mScanIsActive;
		int mScanBranch;
		int mScanDirection;
		int scaneForDevices(Ds18b20SensorGroup* sensorGroup);
        RegistrationNumber* scanWireBus(uint8_t gpioNbr);
		
    public:
        WireBusControler();
        
        // virtual controler functions to be implemented
        virtual bool addConfigurabel(Configurabel* configurable);
        virtual ActionResult onInitialize();
        virtual ActionResult onAquire();

		// overwrite functions
		virtual Configurabel* getFirstConfigurabel();
		virtual Configurabel* getNextConfigurabel();
	
    };
    
}

#endif