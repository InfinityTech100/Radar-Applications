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
#include "uRad.h"



const char* cmd_table[] ={
                "sensorStop\n",
                "flushCfg\n",
                "dfeDataOutputMode 1\n",
                "channelCfg 15 7 0\n",
                "adcCfg 2 1\n",
                "adcbufCfg -1 0 1 1 1\n",
                "profileCfg 0 60 8 7 13.12 657930 0 30 1 64 12499 0 0 158\n",
                "chirpCfg 0 0 0 0 0 0 0 1\n",
                "chirpCfg 1 1 0 0 0 0 0 2\n",
                "chirpCfg 2 2 0 0 0 0 0 4\n",
                "frameCfg 0 2 128 0 100 1 0\n",
                "lowPower 0 0\n",
                "guiMonitor -1 1 0 0 0 0 0\n",
                "cfarCfg -1 0 2 8 4 3 0 15 1\n",
                "cfarCfg -1 1 0 8 4 4 1 15 1\n",
                "multiObjBeamForming -1 1 0.5\n",
                "clutterRemoval -1 0\n",
                "calibDcRangeSig -1 0 -5 8 256\n",
                "extendedMaxVelocity -1 0\n",
                "lvdsStreamCfg -1 0 0 0\n",
                "compRangeBiasAndRxChanPhase 0.0 1 0 -1 0 1 0 -1 0 1 0 -1 0 1 0 -1 0 1 0 -1 0 1 0 -1 0\n",
                "measureRangeBiasAndRxChanPhase 0 1.5 0.2\n",
                "CQRxSatMonitor 0 3 4 15 0\n",
                "CQSigImgMonitor 0 31 4\n",
                "analogMonitor 0 0\n",
                "aoaFovCfg -1 -90 90 -90 90\n",
                "cfarFovCfg -1 0 0 80\n",
                "cfarFovCfg -1 1 -19.72 19.72\n",
                "calibData 0 0 0\n",
                "sensorStart\n"
               //, NULL
};


// todo add useDebug Flag to debug as old version
//  // //todo

uRad::uRad(/* args */)
{
   
}
uRad::~uRad()
{
}

void uRad::begin()
{
 // init serial 1 in config mode
    uRadSerial.begin(uRAD_Cfg_Baudrate);

    // Start Sending commands to uRAD
    for (uint8_t i = 0; i < 30; i++)
    {

        // send the commands in string followed by \n as mentioned in data sheet
        uRadSerial.write(cmd_table[i]);
        // // //display the command on serial monitor *DEBUGGING ONLY*
//        Serial.print(cmd_table[i]);
        delay(20);
    }

    uRadSerial.flush();
    uRadSerial.begin(uRAD_Data_Baudrate);
    while (!uRadSerial);
}
void uRad::handleTasks()
{
    uRadSerial.readBytes(myBuffer, HEADER_LEN);

    // Serial.print("\n\nFirst 8 Bytes : ");
    for (int i = 0; i < 8; i++) {
      // Serial.print(myBuffer[i], HEX);
      // Serial.print(" ");
    }
    // Serial.println("\n--------------------------------");

    if (checkuRadFrame())
    {
        totalPacketLen = bytes2int32(12, myBuffer); // Get Total packet length including frame header length in Bytes
        numDetectedObj = bytes2int32(28, myBuffer); // Number of detected objects (points) for the frame
        numTLVs = bytes2int32(32, myBuffer);        // Number of TLV items for the frame
        timeCpuCycles = bytes2int32(24, myBuffer);  // Number of cpu cycles
        frameNumber = bytes2int32(20, myBuffer);    // Frame Number

      // Serial.print("Frame Number = ");
      // Serial.println(frameNumber);

      // Serial.print("Time CPU Cycles =  ");
      // Serial.println(timeCpuCycles);

      // Serial.print("Total Packet Length = ");
      // Serial.print(totalPacketLen);
      // Serial.println();

      Serial.print("Number of detected objects=");
      Serial.println(numDetectedObj);

      // Serial.print("Number for Tlvs = ");
      // Serial.println(numTLVs);

      payloadLen = totalPacketLen - HEADER_LEN;
      //Serial.println("\nPaylaod len read successfully");
      // Serial.print("PayloadLength = ");
      // Serial.println(payloadLen);

        if (payloadLen > 10000 || numTLVs > 20 || numDetectedObj > 10)
        {
            Serial.println("\n ########### Wrong Payload..########### ");
            Serial.println("wrong payload length / wrong number of TLVs / More than 10 objects");
            Serial.println("Reseting buffers.....");
            Serial.println("Frame Ignored.....");

            if (uRadSerial.available())
            {
                while (uRadSerial.available())
                {
                    uRadSerial.read();
                }
            }
            memset(myBuffer, 0, HEADER_LEN); // reset header frame to zeros
            return;
        }

        // Allocate new array for payload data
        payloadBuf = new uint8_t[payloadLen];
        // Serial.println("Paylaod Pointer allocated successfully");
        // read payload data from the serial Stream

        // // Serial.print("Size of payloadBuf = ");
        // // Serial.println(sizeof(*payloadBuf) * (payloadLen));
        // // // Serial.print("\n Available bytes in buffer before reading the payload = ");
        // // // Serial.println(Serial0.available());

        uRadSerial.readBytes(payloadBuf, payloadLen);

        // // // Serial.print("\n Available bytes in buffer after reading the payload = ");
        // // // Serial.println(Serial0.available());
        // // Serial.println("PAY LOAD DATA WAS READ .....\n");

        // create dynamic table for detected objects data
        detectedObj_t *detectedObj_info = new detectedObj[numDetectedObj];

        /**
         * TLVs Frame consists of:
         *      tlv header : 8 bytes
         *      tlv payload : 16 byte *  numDetectedObj
         */

        // varaible to help managing multiple TLVS
        uint8_t next_TLV_bytes = 0;

        // reading TLVs Frames
        for (int i = 0; i < numTLVs; i++)
        {

            // Serial.print("\nTLV: ");
            // Serial.println(i + 1);

            // extract first 8 bytes in payload buffer
            tlvType = bytes2int32(0 + next_TLV_bytes, payloadBuf);
            tLvLen = bytes2int32(4 + next_TLV_bytes, payloadBuf);

            // Serial.print("TLV type = ");
            // Serial.println(tlvType);

            // Serial.print("tLvLen = ");
            // Serial.println(tLvLen);

            // incase of wrong payload data
            if (tlvType > 20 || tLvLen > 10000)
            {
                Serial.print("Wrong Payload Reset Buffers \n");
                break; // beak the for loop , maybe return error flag is better ?
            }
            // postion data for detected objects
            if (tlvType == 1)
            {
                // add the total bytes of this TLV to next_TLV_bytes varaible in order to be able to get the next TLV type and length
                next_TLV_bytes = tLvLen + TLV_FRAME_LEN;
                Serial.print("\nTLV Type (1) Detected  \n\n");
                for (uint8_t j = 0; j < numDetectedObj; j++)
                {

                    detectedObj_info[j].obj_x = bytes2float((TLV_FRAME_LEN + (j * 16)), payloadBuf);
                    detectedObj_info[j].obj_y = bytes2float((TLV_FRAME_LEN + (j * 16) + 4), payloadBuf);
                    detectedObj_info[j].obj_z = bytes2float((TLV_FRAME_LEN + (j * 16) + 8), payloadBuf);
                    detectedObj_info[j].obj_v = bytes2float((TLV_FRAME_LEN + (j * 16) + 12), payloadBuf);

                    // Log object data to Serial0
                    // Serial.print("detected Object:");
                    // Serial.print(j + 1);
                    // Serial.println();

                    // Serial.print("X= ");
                    // Serial.print(detectedObj_info[j].obj_x);
                    // Serial.print("m Y= ");

                    // Serial.print(detectedObj_info[j].obj_y);
                    // Serial.print("m Z= ");

                    // Serial.print(detectedObj_info[j].obj_z);
                    // Serial.print("m V= ");

                    // Serial.print(detectedObj_info[j].obj_v);
                    // Serial.println("m/s");
                }
            }
            // NSR / NOISE DETECTED OBJECTS
            else if (tlvType == 7)
            {

                // add the total bytes of this TLV to next_TLV_bytes varaible in order to be able to get the next TLV type and length
                Serial.print("TLV Type (7) Detected \n");

                for (uint8_t j = 0; j < numDetectedObj; j++)
                {

                    detectedObj_info[j].obj_snr = bytes2int16((next_TLV_bytes + TLV_FRAME_LEN + (j * 4)), payloadBuf);
                    detectedObj_info[j].obj_noise = bytes2int16((next_TLV_bytes + TLV_FRAME_LEN + (j * 4) + 2), payloadBuf);

                    // Log object data to Serial0
                    // Serial.print("detected side info for Object: ");
                    // Serial.println(j + 1);
                    // Serial.print("snr[dB]= ");
                    // Serial.print(detectedObj_info[j].obj_snr);

                    // Serial.print("dB noise[dB]= ");
                    // Serial.print(detectedObj_info[j].obj_noise);
                    // Serial.println("dB");
                }
                next_TLV_bytes = tLvLen + TLV_FRAME_LEN;
            }
        }

        // todo here I should raise a flag isObject=true
        if (numDetectedObj)
        {
            m_objs = numDetectedObj;
            m_dataInfo = detectedObj_info;
            m_isObj = true;
            // //sendBT_frame(numDetectedObj, detectedObj_info);
        }

        // free allocated  storage
        memset(myBuffer, 0, HEADER_LEN); // reset header frame to zeros
        // memset(detectedObj_info, 0, sizeof(detectedObj_t) * 16);
        delete[] detectedObj_info;
        delete[] payloadBuf;

        // // // Serial.print("\n Available bytes in buffer at the end of handling the payload = ");
        // // // Serial.println(Serial0.available());
        
    }
    else
    {
        Serial.print("\nWrong Sync Word!\n");

        if (uRadSerial.available())

        {
            while (uRadSerial.available())
            {
                uRadSerial.read();
            }
        }

        memset(myBuffer, 0, HEADER_LEN);
    }
}

bool uRad::isObject()
{
    if (m_isObj == true)
    {
        m_isObj = false;
        return true;
    }
    return m_isObj;
}

String uRad::GetRadarObjects()
{
    // //  Serial.println("\n\n_______________________________");
    // // Serial.println("Sending data to bluetooth ....");

    std ::vector<uint8_t> buffer1;
    uint8_t magicword = '@';
    buffer1.push_back(magicword);
    // // //display on serial monitor the magic word
    Serial.print("Magic word sent to WiFi: ");
    Serial.println(magicword, HEX);

    // add the number of objects to the frame
    buffer1.push_back(m_objs);
    // // Serial.print("Objects Sent to Bluetooth: ");
    // // Serial.println((uint8_t)m_isObj, HEX);

     //add the distance of each object to the frame
  for (int i = 0; i < m_objs; i++) {


    if((m_dataInfo[i].obj_y >0) && (m_dataInfo[i].obj_y<1))
     {
      buffer1.push_back(1);
      Serial.print("Distance  sent: ");
      Serial.println(1, HEX);
    }
    else
    {
      
    buffer1.push_back((uint8_t)m_dataInfo[i].obj_y);
    Serial.print("Distance  sent: ");
    Serial.println((uint8_t)m_dataInfo[i].obj_y, HEX);
    }


      /*************
      @note when object is moving towards the radar the velocity is negative
      @note In app 1 is set for positive and 0 for negative so I reversed them here
      *************/

      //for test
//      buffer1.push_back(1);
//      buffer1.push_back(25);
      //for test end

        //commented for test purposes
        //  if(m_dataInfo[i].obj_v >=0)
        //  {
        //    //send positive sign
        //    //@NOTE when object is moving towards the radar the velocity is negative
        //    buffer1.push_back(0);
        //   //  Serial.println("Velocity sign: +ve ");

        //    //send velocity
        //    buffer1.push_back((uint8_t)m_dataInfo[i].obj_v);
        //   //  Serial.print("Velocity  sent: ");
        //   //  Serial.println((uint8_t)m_dataInfo[i].obj_v, HEX);

        //  }
        //  else
        //  {
          
        //   //send negative sign
        //    buffer1.push_back(1);
        //   //  Serial.println("Velocity sign: -ve ");
        //    //send velocity
        //    buffer1.push_back(abs((uint8_t)m_dataInfo[i].obj_v));
        //   //  Serial.print("Velocity  sent: ");
        //   //  Serial.println(abs((uint8_t)m_dataInfo[i].obj_v), HEX);

        //  }
  }
    // create an array buffer to copy the frame inside it to be able to send it through setVale();
//    uint8_t *buffer2 = new uint8_t[buffer1.size()];
    uint8_t buffer2[50];
    // copy the frame to the array buffer
    for (int i = 0; i < buffer1.size(); i++)
    {
        buffer2[i] = buffer1.at(i);
    }
    
    buffer1.clear();
    m_objs = 0;
    // delete[] m_dataInfo;
    return String((char *)buffer2);

    // delete buffer1 and buffer2 to avoid memory overflow
    // delete[] buffer2;
}


/**
 * @brief
 *
 * @param start
 * @param buff
 * @return uint16_t
 */

uint16_t uRad::bytes2int16(uint8_t start, uint8_t *buff)

{
    uint16_t res = 0;

    res = ((uint16_t)buff[start]) | ((uint16_t)buff[start + 1] << 8);

    return res;
}

/**
 * @brief
 *
 * @param start
 * @param buff
 * @return uint32_t
 */
uint32_t uRad::bytes2int32(uint8_t start, uint8_t *buff)
{
    uint32_t res = 0;
    for (uint8_t i = start; i < start + 4; i++)
    {
        res |= ((uint32_t)buff[i] << ((i - start) * 8));
    }
    return res;
}

/**
 * @brief
 *
 * @param start
 * @param buff
 * @return float
 */

float uRad::bytes2float(uint8_t start, uint8_t *buff)
{
    float res = 0;
    memcpy(&res, &buff[start], 4);
    return res;
}

/**
 * @brief Check the Magic word of received frame
 *
 * @return true
 * @return false
 */
bool uRad::checkuRadFrame()
{
    // assuming little-indian is the default scheme

    // added by Mahdi
    memcpy(sync_word, myBuffer, 8);

    int i; // added by mahdi

    for (i = 0; i < 8; i++)
    {

        // added by Mahdi
        // comapre the magic word
        if (sync_Pattern[i] == sync_word[i])
        {
            continue;
        }
        else
        {
            break;
        }
    }
    // added by Mahdi
    if (i == 8)
    {

        return true;
    }

    else
    {
        Serial.println("Radar Frame Error!!");
        // added by Mahdi
        Serial.print("syncWord IS:  ");
        // display sync word in HEX on serial monitor
        for (int j = 0; j < 8; j++)
        {
          Serial.print(sync_word[j], HEX);
        }
        Serial.println();
        // Serial.print("\nBUT SyncPattern IS: ");

        // display sync pattern in HEX on serial monitor
        for (int j = 0; j < 8; j++)
        {
          // Serial.print(sync_Pattern[j], HEX);
        }
        // Serial.println();
        memset(myBuffer, 0, HEADER_LEN);
        return false;
    }
}

// // /**
// //  * @file   uRAD_Industrial.h
// //  * @author Mohamed Maher (m.maher@infinitytech.ltd)
// //  * @brief  Arduino Library to control uRAD industrial Radar Sensor
// //  *         Product: https://urad.es/en/product/urad-radar-industrial
// //  * @version 0.1
// //  * @date 2023-06-28
// //  *
// //  * @copyright Copyright (c) https://www.infinitytech.ltd 2023
// //  *
// //  */

// // #include "uRad.h"

// // uRad::uRad(/* args */)
// // {
// //     Serial2.begin(uRAD_Baudrate);
// // }

// // void uRad::handleTasks()
// // {
// //     if (Serial2.available() > 0)
// //     {

// //         if (m_objs.size() > 0) // reset the objects
// //         {
// //             m_objs.clear();
// //         }

// //         Serial2.readBytes(myBuffer, HEADER_LEN);
// //         if (checkuRadFrame())
// //         {

// //             // Extract Parameters from Header Frame
// //             totalPacketLen = bytes2int32(12, myBuffer); // Get Total packet length including frame header length in Bytes
// //             numDetectedObj = bytes2int32(28, myBuffer); // Number of detected objects (points) for the frame
// //             numTLVs = bytes2int32(32, myBuffer);        // Number of TLV items for the frame

// //             payloadLen = totalPacketLen - HEADER_LEN;

// //             // delete payloadBuff if not equal to zero
// //             if (payloadBuf != 0)
// //                 delete[] payloadBuf;

// //             // Allocate new array for payload data
// //             payloadBuf = new uint8_t[payloadLen];

// //             // read payload data from the serial Stream
// //             Serial2.readBytes(payloadBuf, payloadLen);

// //             // create dynamic table for detected objects data
// //             detectedObj_t *detectedObj_info = new detectedObj[numDetectedObj];

// //             /**
// //              * TLVs Frame consists of:
// //              *          tlv header : 8 bytes
// //              *          tlv payload : 16 byte *  numDetectedObj
// //              */
// //             // reading TLVs Frames
// //             for (int i = 0; i < numTLVs; i++)
// //             {
// //                 // extract first 8 bytes in payload buffer
// //                 tlvType = bytes2int32(0, payloadBuf);
// //                 tLvLen = bytes2int32(4, payloadBuf);

// //                 // incase of wrong payload data
// //                 if (tlvType > 20 || tLvLen > 10000)
// //                 {
// //                     // Serial.printf("Wrong Payload Reset Buffers \n");
// //                     memset(myBuffer, 0, HEADER_LEN); // reset header frame to zeros
// //                     delete[] payloadBuf;             // free allocated payload storage
// //                     break;                           // beak the for loop , maybe return error flag is better ?
// //                 }
// //                 // postion data for detected objects
// //                 if (tlvType == 1)
// //                 {
// //                     // Serial.printf("TLV Type (1) Detected ) \n");
// //                     for (uint8_t j = 0; j < numDetectedObj; j++)
// //                     {

// //                         detectedObj_info[j].obj_x = bytes2float((TLV_FRAME_LEN + (j * 16)), payloadBuf);
// //                         detectedObj_info[j].obj_y = bytes2float((TLV_FRAME_LEN + (j * 16) + 4), payloadBuf);
// //                         detectedObj_info[j].obj_z = bytes2float((TLV_FRAME_LEN + (j * 16) + 8), payloadBuf);
// //                         detectedObj_info[j].obj_v = bytes2float((TLV_FRAME_LEN + (j * 16) + 12), payloadBuf);
// //                         m_objs.push_back(detectedObj_info[j]);
// //                         // Log object data to serial0
// //                         // Serial.printf("Detected Object[%d] Data:  \n", j);
// //                         // Serial.printf("X= %d, Y= %d, Z=%d, V=%d \n\n", detectedObj_info[j].obj_x, detectedObj_info[j].obj_y, detectedObj_info[j].obj_z, detectedObj_info[j].obj_v);
// //                     }
// //                     memset(myBuffer, 0, HEADER_LEN); // reset header frame to zeros
// //                     delete[] payloadBuf;             // free allocated payload storage
// //                 }
// //                 // NSR / NOISE DETECTED OBJECTS
// //                 else if (tlvType == 7)
// //                 {

// //                     // Serial.printf("TLV Type (2) Detected ) \n");
// //                     /*
// //                      *  Not Implemented Yet
// //                      */
// //                 }
// //             }
// //             // extract Object data from it
// //         }
// //         else
// //         {

// //             // Serial.printf("Wrong Sync Word! \n  ");
// //         }
// //     }
// // }

// // /**
// //  * @brief
// //  *
// //  * @param start
// //  * @param buff
// //  * @return uint32_t
// //  */
// // uint32_t uRad::bytes2int32(uint8_t start, uint8_t *buff)
// // {
// //     uint32_t res = 0;
// //     for (uint8_t i = start; i < start + 4; i++)
// //     {
// //         res |= ((uint32_t)buff[i] << ((i - start) * 8));
// //     }
// //     return res;
// // }

// // /**
// //  * @brief
// //  *
// //  * @param start
// //  * @param buff
// //  * @return float
// //  */
// // float uRad::bytes2float(uint8_t start, uint8_t *buff)
// // {
// //     float res = 0;
// //     memcpy(&res, &buff[start], 4);
// //     return res;
// // }

// // /**
// //  * @brief Check the Magic word of received frame
// //  *
// //  * @return true
// //  * @return false
// //  */
// // bool uRad::checkuRadFrame()
// // {
// //     // assuming little-indian is the default scheme
// //     for (int i = 0; i < 8; i++)
// //     {
// //         sync_word |= ((long long)myBuffer[i] << (i * 8));
// //     }

// //     if (sync_word == syncPattern)
// //         return true;
// //     else
// //         return false;
// // }

// // /**
// //  * @brief check if objects
// //  *
// //  * @return true
// //  * @return false
// //  */
// // bool uRad::isObject()
// // {
// //     if (m_objs.size() > 0)
// //     {
// //         return true;
// //     }
// //     return false;
// // }

// // /**
// //  * @brief get the whole objects
// //  *
// //  * @return String
// //  */
// // String uRad::GetRadarObjects()
// // {
// //     char myframe[40];
// //     myframe[0]='@';
// //     myframe[1]=m_objs.size();

// //     for(int i=0;i<myframe[1];i++)
// //     {
// //         myframe[i+2]=m_objs[i].obj_x;
// //     }
// //     m_objs.clear();
// //     return String(myframe);
// // }

// // /**
// //  * @brief Destroy the u Rad::u Rad object
// //  *
// //  */
// // uRad::~uRad()
// // {

// // }
