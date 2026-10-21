/**
 * @file   uRAD_Industrial.h
 * @author Mohamed Maher (m.maher@infinitytech.ltd)
 * @brief  Arduino Library to control uRAD industrial Radar Sensor
 *         Product: https://urad.es/en/product/urad-radar-industrial
 * @version 0.1
 * @date 2023-06-28
 *
 * @copyright Copyright (c) https://www.infinitytech.ltd 2023
 *
 */

#ifndef URADAR_h
#define URADAR_h

#include <Arduino.h>
#include <stdint.h>
#include "BluetoothSerial.h"
#include <vector>
#include <iostream>

/*************** configs *************************/

#define HEADER_LEN 40             // header frame length in bytes
#define TLV_FRAME_LEN 8           // TLVs header frame length in bytes
#define uRAD_Cfg_Baudrate 115200  // data stream buadrate for uRad industrial shield
#define uRAD_Data_Baudrate 921600 // 921600            // data stream buadrate for uRad industrial shield
#define CMD_CNT 30
#define uRAD_RST 6
#define uRAD_ON 7
#define uRAD_RESET true
#define uRadSerial Serial2




typedef struct detectedObj
{
    float obj_x;        // object x position in meters
    float obj_y;        // object y position in meters
    float obj_z;        // object z position in meters
    float obj_v;        // object radial Doppler velocity in meters/sec
    uint16_t obj_snr;   // object SNR [dB]
    uint16_t obj_noise; // Noise [dB]

} detectedObj_t;

class uRad
{
private:
    // Variables for uRAD
    uint8_t myBuffer[HEADER_LEN];
    uint8_t sync_word[8];
    // const long long syncPattern = 0x708050603040102  ; // Sync pattern value for uRad industrial shield // commented by Mahdi
    const uint8_t sync_Pattern[8] = {0x02, 0x01, 0x04, 0x03, 0x06, 0x05, 0x08, 0x07}; // added by Mahdi
    uint32_t uRad_version, totalPacketLen, platform, frameNumber, timeCpuCycles, numDetectedObj, numTLVs, subFrameNumber;
    uint32_t tlvType, tLvLen;

    uint8_t *payloadBuf = 0;
    uint32_t payloadLen = 0;
    uint8_t paddingBytes = 0;


    bool m_isObj=false;
    uint8_t m_objs=0;
    detectedObj_t* m_dataInfo;

    // Functions Protoyping
    float bytes2float(uint8_t start, uint8_t *buff);
    uint32_t bytes2int32(uint8_t start, uint8_t *buff);
    uint16_t bytes2int16(uint8_t start, uint8_t *buff);
    bool checkuRadFrame();
    // void sendBT_frame(uint8_t objs, detectedObj_t* data);


public:
    uRad(/* args */);
    ~uRad();
    void begin();
    void handleTasks();
    bool isObject();
    String GetRadarObjects();
};

#endif