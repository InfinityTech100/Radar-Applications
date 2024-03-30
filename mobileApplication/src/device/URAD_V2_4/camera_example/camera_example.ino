#include "esp_camera.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "esp_timer.h"

#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM

#include "camera.h"

const int SD_PIN_CS = 21;

File videoFile;
bool camera_sign = false;
bool sd_sign = false;
unsigned long lastCaptureTime = 0;
unsigned long captureDuration = 10000; // 10 seconds
int imageCount = 0;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  
  // Initialize the camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  
  camera_sign = true;
  
  // Initialize the SD card
  if (!SD.begin(SD_PIN_CS)) {
    Serial.println("SD card initialization failed!");
    return;
  }

  uint8_t cardType = SD.cardType();

  // Determine if the type of SD card is available
  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  
  sd_sign = true;

  Serial.println("Video will begin in one minute, please be ready.");
}

void loop() {
  // Camera & SD available, start taking video
  if (camera_sign && sd_sign) {
    // Get the current time
    unsigned long now = millis();

    //If it has been more than 1 minute since the last video capture, start capturing a new video
    if ((now - lastCaptureTime) >= 60000) {
      char filename[32];
      sprintf(filename, "/video%d.avi", imageCount);
      videoFile = SD.open(filename, FILE_WRITE);
      if (!videoFile) {
        Serial.println("Error opening video file!");
        return;
      }
      Serial.printf("Recording video：%s\n", filename);
      lastCaptureTime = now;
      
      // Start capturing video frames
      while ((millis() - lastCaptureTime) < captureDuration) {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
          Serial.println("Error getting framebuffer!");
          break;
        }
        videoFile.write(fb->buf, fb->len);
        esp_camera_fb_return(fb);
      }
      
      // Close the video file
      videoFile.close();
      Serial.printf("Video saved: %s\n", filename);
      imageCount++;

      Serial.println("Video will begin in one minute, please be ready.");

      // Wait for the remaining time of the minute
      delay(60000 - (millis() - lastCaptureTime));
    }
  }
}




























/*


#include "infinity_sd.h"
#include "camera.h"




SDCard mysd;
camera *OV2640Cam;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  String msg = (mysd.mount() == true) ? "sd card mounted succesfully..." : "failed to mount sd card....";
  Serial.println(msg);
  OV2640Cam = new camera();
  OV2640Cam->config(CameraSetting::VGA);
  //mysd.write("/fileCTR.txt", (uint8_t *)"0000"); // working perfect
  
  int fileCTR=0;
  Serial.println("Camera initilaized..");
  while (true) {

    if (Serial.available() > 0) {
      char c = Serial.read();
      if (c == 's') {
        String file="xyz"+String(fileCTR);
        fileCTR++;
        OV2640Cam->StartRecording(file);  // Start recording video when 's' is received on Serial UART.
        Serial.println("Recording started");
      }
      if (c == 'p') {
        OV2640Cam->StopRecording();  // Stop recording video when 'p' is received on Serial UART.
        Serial.println("Recording stopped");
      }
      if (c == 'a') {
        OV2640Cam->config(CameraSetting::VGA);
        Serial.println("Camera settings VGA");
      }
      if (c == 'b') {
        OV2640Cam->config(CameraSetting::UXGA);
        Serial.println("Camera settings UXGA");
      }
      if (c == 'c') {
        OV2640Cam->config(CameraSetting::SVGA);
        Serial.println("Camera settings SVGA");
      }
      if (c == 'd') {
        OV2640Cam->config(CameraSetting::CIF);
        Serial.println("Camera settings CIF");
      }
      if (c == 'f') {
        String file="IMG"+String(fileCTR++);
        OV2640Cam->takePicture(file);
        Serial.println("Captured a picture");
      }
    }


    OV2640Cam->handleRequests();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}

*/