/**
 * @file camera.h
 * @author Nader Hany (naderhany638@gmail.com)
 * @brief this is a camera interface driver 
 * @version 12.1
 * @date 2023-07-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */
/*

  TimeLapseAvi
  ESP32-CAM Video Recorder
  This program records an AVI video on the SD Card of an ESP32-CAM.

  Acknowlegements:
  1.  https://robotzero.one/time-lapse-esp32-cameras/
      Timelapse programs for ESP32-CAM version that sends snapshots of screen.
  2.  https://github.com/nailbuster/esp8266FTPServer
      ftp server (slightly modifed to get the directory function working)
  3.  https://github.com/ArduCAM/Arduino/tree/master/ArduCAM/examples/mini
      ArduCAM Mini demo (C)2017 LeeWeb: http://www.ArduCAM.com
      I copied the structure of the avi file, some calculations.

  3.  https://github.com/jameszah/ESP32-CAM-Video-Recorder
      jameszah/ESP32-CAM-Video-Recorder is licensed under the
      GNU General Public License v3.0

  Thanks For:
    James Zahary jamzah.plc@gmail.com

  The is Arduino code, with standard setup for ESP32-CAM
    - Board ESP32 Wrover Module
    - Partition Scheme Huge APP (3MB No OTA)
    
  This program records an AVI video on the SD Card of an ESP32-CAM.
  
  It will record realtime video at limited framerates, or timelapses with the full resolution of the ESP32-CAM.
  It is controlled by a web page it serves to stop and start recordings with many parameters, and look through the viewfinder.
  
  You can control framesize (UXGA, VGA, ...), quality, length, and fps to record, and fps to playback later, etc.

  There is also an ftp server to download the recordings to a PC.

  Instructions:
    framesize can be UXGA, SVGA, VGA, CIF (default VGA)
    length is length in seconds of the recording 0..3600 (default 1800)
    interval is the milli-seconds between frames (default 200)
    quality is a number 5..50 for the jpeg  - smaller number is higher quality with bigger and more detailed jpeg (default 10)
    repeat is a number of who many of the same recordings should be made (default 100)
    speed is a factor to speed up realtime for a timelapse recording - 1 is realtime (default 1)
    gray is 1 for a grayscale video (default 0 - color)
  
    These factors have to be within the limit of the SD chip to receive the data.
    For example, using a LEXAR 300x 32GB microSDHC UHS-I, the following works for me:
  
    UXGA quality 10,  2 fps (or interval of 500ms)
    SVGA quality 10,  5 fps (200ms)
    VGA  quality 10, 10 fps (100ms)
    CIG  quality 10, 20 fps (50ms)
  
    If you increase fps, you might have to reduce quality or framesize to keep it from dropping frames as it writes all the data to the SD chip.
  
    Also, other SD chips will be faster or slower.  I was using a SanDisk 16GB microSDHC "Up to 653X" - which was slower and more unpredictable than the LEXAR ???
 
*/


#ifndef CAM_h
#define CAM_h


#define USE_ESP32_CAM 0

#include <Arduino.h>
#include "esp_log.h"
#include "esp_camera.h"

// Time
#include "time.h"
#include "lwip/err.h"
#include "lwip/apps/sntp.h"

// MicroSD
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include "FS.h"



#if (USE_ESP32_CAM == 1)

#include "SD_MMC.h"

#else

#include "FS.h"            // https://github.com/espressif/arduino-esp32/blob/master/libraries/FS/
#include "SD.h"            // https://github.com/espressif/arduino-esp32/tree/master/libraries/SD
#include "SPI.h"

#endif
#include "infinity_sd.h"

// CAMERA_MODEL_AI_THINKER
// #define PWDN_GPIO_NUM     32
// #define RESET_GPIO_NUM    -1
// #define XCLK_GPIO_NUM      0
// #define SIOD_GPIO_NUM     26
// #define SIOC_GPIO_NUM     27
// #define Y9_GPIO_NUM       35
// #define Y8_GPIO_NUM       34
// #define Y7_GPIO_NUM       39
// #define Y6_GPIO_NUM       36
// #define Y5_GPIO_NUM       21
// #define Y4_GPIO_NUM       19
// #define Y3_GPIO_NUM       18
// #define Y2_GPIO_NUM        5
// #define VSYNC_GPIO_NUM    25
// #define HREF_GPIO_NUM     23
// #define PCLK_GPIO_NUM     22
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40
#define SIOC_GPIO_NUM     39

#define Y9_GPIO_NUM       48
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15
#define VSYNC_GPIO_NUM    38
#define HREF_GPIO_NUM     47
#define PCLK_GPIO_NUM     13

#define LED_GPIO_NUM      21

// GLOBALS
#define BUFFSIZE 512
#define AVIOFFSET 240 // AVI main header length



const int avi_header[AVIOFFSET] PROGMEM = {
  0x52, 0x49, 0x46, 0x46, 0xD8, 0x01, 0x0E, 0x00, 0x41, 0x56, 0x49, 0x20, 0x4C, 0x49, 0x53, 0x54,
  0xD0, 0x00, 0x00, 0x00, 0x68, 0x64, 0x72, 0x6C, 0x61, 0x76, 0x69, 0x68, 0x38, 0x00, 0x00, 0x00,
  0xA0, 0x86, 0x01, 0x00, 0x80, 0x66, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
  0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x02, 0x00, 0x00, 0xe0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4C, 0x49, 0x53, 0x54, 0x84, 0x00, 0x00, 0x00,
  0x73, 0x74, 0x72, 0x6C, 0x73, 0x74, 0x72, 0x68, 0x30, 0x00, 0x00, 0x00, 0x76, 0x69, 0x64, 0x73,
  0x4D, 0x4A, 0x50, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x74, 0x72, 0x66,
  0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x80, 0x02, 0x00, 0x00, 0xe0, 0x01, 0x00, 0x00,
  0x01, 0x00, 0x18, 0x00, 0x4D, 0x4A, 0x50, 0x47, 0x00, 0x84, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x4E, 0x46, 0x4F,
  0x10, 0x00, 0x00, 0x00, 0x6A, 0x61, 0x6D, 0x65, 0x73, 0x7A, 0x61, 0x68, 0x61, 0x72, 0x79, 0x67,
  0x6D, 0x61, 0x69, 0x6C, 0x4C, 0x49, 0x53, 0x54, 0x00, 0x01, 0x0E, 0x00, 0x6D, 0x6F, 0x76, 0x69,
};



enum class CameraSetting{VGA,UXGA,SVGA,CIF};

class camera
{
private:
  uint8_t  vga_w[2] = {0x80, 0x02}; // 640
  uint8_t  vga_h[2] = {0xE0, 0x01}; // 480
  uint8_t  cif_w[2] = {0x90, 0x01}; // 400
  uint8_t  cif_h[2] = {0x28, 0x01}; // 296
  uint8_t svga_w[2] = {0x20, 0x03}; //
  uint8_t svga_h[2] = {0x58, 0x02}; //
  uint8_t uxga_w[2] = {0x40, 0x06}; // 1600
  uint8_t uxga_h[2] = {0xB0, 0x04}; // 1200

  uint8_t zero_buf[4] = {0x00, 0x00, 0x00, 0x00};
  uint8_t   dc_buf[4] = {0x30, 0x30, 0x64, 0x63};    // "00dc"
  uint8_t avi1_buf[4] = {0x41, 0x56, 0x49, 0x31};    // "AVI1"
  uint8_t idx1_buf[4] = {0x69, 0x64, 0x78, 0x31};    // "idx1"

  // 1 init
  esp_err_t cam_err;
  int capture_interval = 200;  // microseconds between captures
  int total_frames = 300;      // default updated below
  int recording = 0;           // default start recording on reboot
  int framesize = 10;           // vga
  int repeat = 100;            // capture 100 videos
  int quality = 5;
  int xspeed = 1;
  int xlength = 3;
  int gray = 0;
  long wakeup;
  long last_wakeup = 0;
  int newfile = 0;
  int frames_so_far = 0;
  long bp;
  long bw;
  long totalp;
  long totalw;
  int overtime_count = 0;
  uint16_t frame_cnt = 0;
  uint32_t startms;
  long last_capture_millis = 0;
  long current_millis;

  // 3 make_avi
  time_t now;
  struct tm timeinfo;
  char strftime_buf[64];

  // 4 start_avi()
  FILE *avifile = NULL;
  FILE *idxfile = NULL;
  int i = 0;
  uint8_t buf[BUFFSIZE];
  long bigdelta = 0;
  unsigned long movi_size = 0;
  unsigned long jpeg_size = 0;
  unsigned long idx_offset = 0;
  uint32_t uVideoLen = 0;
  
  // 5 end_avi()
  uint32_t elapsedms;

  // 6 another_pic()
  camera_fb_t * fb = NULL;
  uint16_t remnant = 0;
  unsigned long fileposition = 0;


  unsigned long previousMillis = 0; 
const long fps = 200; 

  // setting
  CameraSetting m_cameraSetting=CameraSetting::SVGA;
  String m_fileName;
  File videoFile;

  SDCard _mysd;

  void        make_avi(); 
  void        major_fail();   
  void        print_quartet(unsigned long i, FILE * fd);
  void        changeSetting(CameraSetting newSetting);
  esp_err_t   start_avi(); 
  esp_err_t   end_avi();
  void   do_fb();
  esp_err_t   another_pic_avi(); 
public:



  camera();
  ~camera();
  void config(CameraSetting newSttng);
  void handleRequests();
  void takePicture(String fileName);
  void StartRecording(String fileName);
  void StopRecording();
};


#endif
