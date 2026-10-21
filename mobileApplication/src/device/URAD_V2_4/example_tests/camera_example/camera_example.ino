/**
 * @file    working_v2.ino
 * @author Nader Hany (naderhany638@gmail.com)
 * @brief  this is a camera example describe how to use
 *         my driver on esp32-camera board.
 * @note   you need firstly to mount the sd card before you initialize your camera
 *         or you will get an error.
 * @version 7.1
 * @date 2023-07-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "infinity_sd.h"
#include "camera.h"




SDCard mysd;
camera *OV2640Cam;

/**
 * @brief don't initialize your camera before you mount your sd card 
 * 
 */
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  String msg = (mysd.mount() == true) ? "sd card mounted succesfully..." : "failed to mount sd card....";
  Serial.println(msg);
  OV2640Cam = new camera();
  OV2640Cam->config(CameraSetting::SVGA);
  mysd.write("/fileCTR.txt", (uint8_t *)"0000"); // working perfect
  
  int fileCTR=0;
  while (true) {
    // Serial.println("waiting for command..");

    if (Serial.available() > 0) {
      char c = Serial.read();
      if (c == 's') {
        String file="videofile"+String(fileCTR);
        fileCTR++;
        OV2640Cam->StartRecording(file); 
         // Start recording video when 's' is received on Serial UART.
         Serial.println("Recording started");
      }
      if (c == 'p') {
        OV2640Cam->StopRecording();  // Stop recording video when 'p' is received on Serial UART.
        Serial.println("Recording Stopped");
      }
      if (c == 'a') {
        OV2640Cam->config(CameraSetting::VGA);
        Serial.println("Camera setting changed to VGA");
      }
      if (c == 'b') {
        OV2640Cam->config(CameraSetting::UXGA);
        Serial.println("Camera setting changed to UXGA");
      }
      if (c == 'c') {
        OV2640Cam->config(CameraSetting::SVGA);
        Serial.println("Camera setting changed to SVGA");
      }
      if (c == 'd') {
        OV2640Cam->config(CameraSetting::CIF);
        Serial.println("Camera setting changed to CIF");
      }
      if (c == 'f') {
        String file="/IMG"+String(fileCTR++);
        OV2640Cam->takePicture(file);
        Serial.println("took picture.. ");
      }
    }
    OV2640Cam->handleRequests();

    // delay(200);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
