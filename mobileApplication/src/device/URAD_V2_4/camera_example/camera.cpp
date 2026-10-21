// /**
//  * @file camera.h
//  * @author Nader Hany (naderhany638@gmail.com)
//  * @brief this is a camera interface driver 
//  * @version 12.1
//  * @date 2023-07-19
//  * 
//  * @copyright Copyright (c) 2023
//  * 
//  */
// /*

//   TimeLapseAvi
//   ESP32-CAM Video Recorder
//   This program records an AVI video on the SD Card of an ESP32-CAM.

//   Acknowlegements:
//   1.  https://robotzero.one/time-lapse-esp32-cameras/
//       Timelapse programs for ESP32-CAM version that sends snapshots of screen.
//   2.  https://github.com/nailbuster/esp8266FTPServer
//       ftp server (slightly modifed to get the directory function working)
//   3.  https://github.com/ArduCAM/Arduino/tree/master/ArduCAM/examples/mini
//       ArduCAM Mini demo (C)2017 LeeWeb: http://www.ArduCAM.com
//       I copied the structure of the avi file, some calculations.

//   3.  https://github.com/jameszah/ESP32-CAM-Video-Recorder
//       jameszah/ESP32-CAM-Video-Recorder is licensed under the
//       GNU General Public License v3.0

//   Thanks For:
//     James Zahary jamzah.plc@gmail.com

//   The is Arduino code, with standard setup for ESP32-CAM
//     - Board ESP32 Wrover Module
//     - Partition Scheme Huge APP (3MB No OTA)
    
//   This program records an AVI video on the SD Card of an ESP32-CAM.
  
//   It will record realtime video at limited framerates, or timelapses with the full resolution of the ESP32-CAM.
//   It is controlled by a web page it serves to stop and start recordings with many parameters, and look through the viewfinder.
  
//   You can control framesize (UXGA, VGA, ...), quality, length, and fps to record, and fps to playback later, etc.

//   There is also an ftp server to download the recordings to a PC.

//   Instructions:
//     framesize can be UXGA, SVGA, VGA, CIF (default VGA)
//     length is length in seconds of the recording 0..3600 (default 1800)
//     interval is the milli-seconds between frames (default 200)
//     quality is a number 5..50 for the jpeg  - smaller number is higher quality with bigger and more detailed jpeg (default 10)
//     repeat is a number of who many of the same recordings should be made (default 100)
//     speed is a factor to speed up realtime for a timelapse recording - 1 is realtime (default 1)
//     gray is 1 for a grayscale video (default 0 - color)
  
//     These factors have to be within the limit of the SD chip to receive the data.
//     For example, using a LEXAR 300x 32GB microSDHC UHS-I, the following works for me:
  
//     UXGA quality 10,  2 fps (or interval of 500ms)
//     SVGA quality 10,  5 fps (200ms)
//     VGA  quality 10, 10 fps (100ms)
//     CIG  quality 10, 20 fps (50ms)
  
//     If you increase fps, you might have to reduce quality or framesize to keep it from dropping frames as it writes all the data to the SD chip.
  
//     Also, other SD chips will be faster or slower.  I was using a SanDisk 16GB microSDHC "Up to 653X" - which was slower and more unpredictable than the LEXAR ???
 
// */



// //#include "camera.h"


// /**
//  * @brief Construct a new camera::camera object
//  * 
//  */
// camera::camera() {
//   // pinMode(33, OUTPUT);   // little red led on back of chip
//   // digitalWrite(33, LOW); // turn on the red LED on the back of chip

//   String msg = (_mysd.mount() == true) ? "sd card mounted succesfully..." : "failed to mount sd card....";
//   Serial.println(msg);

//   camera_config_t config;
//   config.ledc_channel = LEDC_CHANNEL_0;
//   config.ledc_timer = LEDC_TIMER_0;
//   config.pin_d0 = Y2_GPIO_NUM;
//   config.pin_d1 = Y3_GPIO_NUM;
//   config.pin_d2 = Y4_GPIO_NUM;
//   config.pin_d3 = Y5_GPIO_NUM;
//   config.pin_d4 = Y6_GPIO_NUM;
//   config.pin_d5 = Y7_GPIO_NUM;
//   config.pin_d6 = Y8_GPIO_NUM;
//   config.pin_d7 = Y9_GPIO_NUM;
//   config.pin_xclk = XCLK_GPIO_NUM;
//   config.pin_pclk = PCLK_GPIO_NUM;
//   config.pin_vsync = VSYNC_GPIO_NUM;
//   config.pin_href = HREF_GPIO_NUM;
//   config.pin_sscb_sda = SIOD_GPIO_NUM;
//   config.pin_sscb_scl = SIOC_GPIO_NUM;
//   config.pin_pwdn = PWDN_GPIO_NUM;
//   config.pin_reset = RESET_GPIO_NUM;
//   config.xclk_freq_hz = 20000000;
//   config.pixel_format = PIXFORMAT_JPEG;

//   // init with high specs to pre-allocate larger buffers
//   if (psramFound()) {
//     Serial.println("PSRAM FOUND");
//     config.frame_size = FRAMESIZE_QSXGA;
//     config.jpeg_quality = 1;
//     config.fb_count = 2;
//   } else {
//     Serial.println("PSRAM NOT FOUND");
//     config.frame_size = FRAMESIZE_SVGA;  // svga 12 fails due to jpg 60000
//     config.jpeg_quality = 12;
//     config.fb_count = 1;
//   }

//   // camera init
//   cam_err = esp_camera_init(&config);
//   if (cam_err != ESP_OK) {
//     major_fail();
//     return;
//   }

//   // digitalWrite(33, HIGH);

//   //
//   //  startup defaults  -- EDIT HERE
//   //  zzz
//   Serial.println("Before sensor get fucntion");
//   sensor_t *s = esp_camera_sensor_get();
//   s->set_framesize(s, FRAMESIZE_VGA);
//   s->set_quality(s, 10);
//   do_fb();  // do a couple captures to make sure camera has new config
//   Serial.println("After sensor get fucntion");
//   do_fb();

//   framesize = 6;  // vga
//   repeat = 100;   // 100 videos
//   xspeed = 1;     // playback at 1 x realtime
//   gray = 0;       // not gray

//   quality = 10;            // quality 10 - pretty good.  Goes from 0..63, but 0-5 sometimes fails on bright scenery (jpg too big for ESP32CAM system)
//   capture_interval = 200;  // 200 milli-secconds per frame
//   total_frames = 9000;     // 9000 frames x 200 ms = 1800 sec = 30 min

//   xlength = total_frames * capture_interval / 1000;

//   newfile = 0;    // no file is open  // don't fiddle with this!
//   recording = 0;  // start recording on reboot without sending a command
// }

// /**
//  * @brief this should be inside a while(1) function
//  *          
//  */
// void camera::handleRequests() {

//   if (recording == 1) {
//     unsigned long currentMillis = millis();  // grab the current time
//     if (currentMillis - previousMillis >= fps) {
//       // save the last time we blinked the LED
//       previousMillis = currentMillis;
//       camera_fb_t *fb = esp_camera_fb_get();
//       if (!fb) {
//         Serial.println("Error getting framebuffer!");
//         return;
//       }
//       videoFile.write(fb->buf, fb->len);
//       esp_camera_fb_return(fb);
//     }
//   } else {
//     previousMillis = 0;
//   }
// }

// /**
//  * @brief this function will start creating
//  *        an avi file 
//  * 
//  */
// void camera::make_avi() {
//   // we are recording, but no file is open
//   if (newfile == 0 && recording == 1) {  // open the file
//     start_avi();
//     // digitalWrite(33, HIGH);
//     newfile = 1;
//     totalp = 0;
//     totalw = 0;
//     frame_cnt = 0;
//     frames_so_far = 0;
//   } else {
//     // we have a file open, but not recording
//     if (newfile == 1 && recording == 0) {  // got command to close file
//       // digitalWrite(33, LOW);
//       end_avi();
//       frames_so_far = total_frames;
//       newfile = 0;    // file is closed
//       recording = 0;  // DO NOT start another recording
//     } else {
//       if (newfile == 1 && recording == 1) {   // regular recording
//         if (frames_so_far == total_frames) {  // we are done the recording
//           // digitalWrite(33, LOW); // close the file
//           end_avi();
//           frames_so_far = 0;
//           newfile = 0;  // file is closed
//           if (repeat > 0) {
//             recording = 1;  // start another recording
//             repeat = repeat - 1;
//           } else {
//             recording = 0;
//           }
//         } else if ((millis() - startms) > (total_frames * capture_interval)) {
//           // digitalWrite(33, LOW); // close the file
//           end_avi();
//           frames_so_far = 0;
//           newfile = 0;  // file is closed
//           if (repeat > 0) {
//             recording = 1;  // start another recording
//             repeat = repeat - 1;
//           } else {
//             recording = 0;
//           }
//         } else {  // regular
//           current_millis = millis();
//           if (current_millis - last_capture_millis > capture_interval) {  // Take another picture - fixed interval
//             last_capture_millis = millis();
//             frames_so_far = frames_so_far + 1;
//             frame_cnt++;
//             another_pic_avi();
//           }
//         }
//       }
//     }
//   }
// }
// /**
//  * @brief will create and write the avi header
//  * 
//  * @return esp_err_t 
//  */
// esp_err_t camera::start_avi() {
//   do_fb();  // start the camera ... warm it up
//   delay(1000);
//   do_fb();
//   delay(1000);
//   do_fb();
//   Serial.println("start Recording step 0");
//   time(&now);
//   localtime_r(&now, &timeinfo);
//   strftime(strftime_buf, sizeof(strftime_buf), "%F__%H-%M-%S", &timeinfo);
//   char fname[100];
//   Serial.println("start Recording step 1");
//   if (m_cameraSetting == CameraSetting::VGA) {
//     //sprintf(fname, "/sdcard/%s_vga_Q%d_I%d_L%d_S%d.avi", strftime_buf, quality, capture_interval, xlength, xspeed);
//     //sprintf(fname, "/sdcard/%s_vga.avi", m_fileName);
//     _mysd.write("/" + m_fileName + "_vga.avi", (uint8_t *)"");

//   } else if (m_cameraSetting == CameraSetting::UXGA) {
//     // sprintf(fname, "/sdcard/%s_svga_Q%d_I%d_L%d_S%d.avi", strftime_buf, quality, capture_interval, xlength, xspeed);
//     sprintf(fname, "/sdcard/%s_uxga.avi", m_fileName);

//   } else if (m_cameraSetting == CameraSetting::SVGA) {
//     //sprintf(fname, "/sdcard/%s_uxga_Q%d_I%d_L%d_S%d.avi", strftime_buf, quality, capture_interval, xlength, xspeed);
//     sprintf(fname, "/sdcard/%s_svga.avi", m_fileName);

//   } else if (m_cameraSetting == CameraSetting::CIF) {
//     //sprintf(fname, "/sdcard/%s_cif_Q%d_I%d_L%d_S%d.avi", strftime_buf, quality, capture_interval, xlength, xspeed);
//     sprintf(fname, "/sdcard/%s_cif.avi", m_fileName);

//   } else {
//     sprintf(fname, "/sdcard/%s_xxx_Q%d_I%d_L%d_S%d.avi", strftime_buf, quality, capture_interval, xlength, xspeed);
//   }

//   Serial.print("\nFile name will be >");
//   Serial.print(fname);
//   Serial.println("<");

//   avifile = fopen(fname, "w");
//   idxfile = fopen("/idx.tmp", "w");
//   Serial.println("start Recording step2");

//   if (avifile != NULL) {
//     // no error
//   } else {
//     // error
//     major_fail();
//   }
//   Serial.println("start Recording step2 avi file not created");
//   if (idxfile != NULL) {
//   } else {
//     // error
//     major_fail();
//   }
//   Serial.println("start Recording step2 idx not created");
//   for (i = 0; i < AVIOFFSET; i++) {
//     char ch = pgm_read_byte(&avi_header[i]);
//     buf[i] = ch;
//   }

//   size_t err = fwrite(buf, 1, AVIOFFSET, avifile);

//   if (framesize == 6) {

//     fseek(avifile, 0x40, SEEK_SET);
//     err = fwrite(vga_w, 1, 2, avifile);
//     fseek(avifile, 0xA8, SEEK_SET);
//     err = fwrite(vga_w, 1, 2, avifile);
//     fseek(avifile, 0x44, SEEK_SET);
//     err = fwrite(vga_h, 1, 2, avifile);
//     fseek(avifile, 0xAC, SEEK_SET);
//     err = fwrite(vga_h, 1, 2, avifile);
//   } else if (framesize == 10) {

//     fseek(avifile, 0x40, SEEK_SET);
//     err = fwrite(uxga_w, 1, 2, avifile);
//     fseek(avifile, 0xA8, SEEK_SET);
//     err = fwrite(uxga_w, 1, 2, avifile);
//     fseek(avifile, 0x44, SEEK_SET);
//     err = fwrite(uxga_h, 1, 2, avifile);
//     fseek(avifile, 0xAC, SEEK_SET);
//     err = fwrite(uxga_h, 1, 2, avifile);
//   } else if (framesize == 7) {

//     fseek(avifile, 0x40, SEEK_SET);
//     err = fwrite(svga_w, 1, 2, avifile);
//     fseek(avifile, 0xA8, SEEK_SET);
//     err = fwrite(svga_w, 1, 2, avifile);
//     fseek(avifile, 0x44, SEEK_SET);
//     err = fwrite(svga_h, 1, 2, avifile);
//     fseek(avifile, 0xAC, SEEK_SET);
//     err = fwrite(svga_h, 1, 2, avifile);
//   } else if (framesize == 5) {

//     fseek(avifile, 0x40, SEEK_SET);
//     err = fwrite(cif_w, 1, 2, avifile);
//     fseek(avifile, 0xA8, SEEK_SET);
//     err = fwrite(cif_w, 1, 2, avifile);
//     fseek(avifile, 0x44, SEEK_SET);
//     err = fwrite(cif_h, 1, 2, avifile);
//     fseek(avifile, 0xAC, SEEK_SET);
//     err = fwrite(cif_h, 1, 2, avifile);
//   }
//   fseek(avifile, AVIOFFSET, SEEK_SET);
//   Serial.println("start Recording step 3");
//   startms = millis();
//   bigdelta = millis();
//   totalp = 0;
//   totalw = 0;
//   overtime_count = 0;
//   jpeg_size = 0;
//   movi_size = 0;
//   uVideoLen = 0;
//   idx_offset = 4;
//   Serial.println("start Recording step 5");
// }

// /**
//  * @brief will end recording on the file
//  * 
//  * @return esp_err_t 
//  */
// esp_err_t camera::end_avi() {
//   unsigned long current_end = 0;
//   current_end = ftell(avifile);
//   elapsedms = millis() - startms;
//   float fRealFPS = (1000.0f * (float)frame_cnt) / ((float)elapsedms) * xspeed;
//   float fmicroseconds_per_frame = 1000000.0f / fRealFPS;
//   uint8_t iAttainedFPS = round(fRealFPS);
//   uint32_t us_per_frame = round(fmicroseconds_per_frame);

//   // Modify the MJPEG header from the beginning of the file, overwriting various placeholders

//   fseek(avifile, 4, SEEK_SET);
//   print_quartet(movi_size + 240 + 16 * frame_cnt + 8 * frame_cnt, avifile);
//   fseek(avifile, 0x20, SEEK_SET);
//   print_quartet(us_per_frame, avifile);
//   unsigned long max_bytes_per_sec = movi_size * iAttainedFPS / frame_cnt;
//   fseek(avifile, 0x24, SEEK_SET);
//   print_quartet(max_bytes_per_sec, avifile);
//   fseek(avifile, 0x30, SEEK_SET);
//   print_quartet(frame_cnt, avifile);
//   fseek(avifile, 0x84, SEEK_SET);
//   print_quartet((int)iAttainedFPS, avifile);
//   fseek(avifile, 0xe8, SEEK_SET);
//   print_quartet(movi_size + frame_cnt * 8 + 4, avifile);
//   fseek(avifile, current_end, SEEK_SET);
//   fclose(idxfile);
//   size_t i1_err = fwrite(idx1_buf, 1, 4, avifile);
//   print_quartet(frame_cnt * 16, avifile);
//   idxfile = fopen("/sdcard/idx.tmp", "r");

//   if (idxfile != NULL) {
//   } else {
//     // major_fail();
//   }
//   char *AteBytes;
//   AteBytes = (char *)malloc(8);
//   for (int i = 0; i < frame_cnt; i++) {
//     size_t res = fread(AteBytes, 1, 8, idxfile);
//     size_t i1_err = fwrite(dc_buf, 1, 4, avifile);
//     size_t i2_err = fwrite(zero_buf, 1, 4, avifile);
//     size_t i3_err = fwrite(AteBytes, 1, 8, avifile);
//   }

//   free(AteBytes);
//   fclose(idxfile);
//   fclose(avifile);
// }
// /**
//  * @brief will be used to heat the camera
//  * 
//  * @return esp_err_t 
//  */
// void camera::do_fb() {
//   camera_fb_t *fb = esp_camera_fb_get();
//   if (!fb) {
//     Serial.println("Error getting framebuffer!");
//     return;
//   }
//   esp_camera_fb_return(fb);
// }

// /**
//  * @brief will be used to take another frame on the avi file
//  * 
//  * @return esp_err_t 
//  */
// esp_err_t camera::another_pic_avi() {
//   // digitalWrite(33, LOW);
//   bp = millis();
//   fb = esp_camera_fb_get();
//   totalp = totalp + millis() - bp;
//   jpeg_size = fb->len;
//   movi_size += jpeg_size;  // Update totals
//   uVideoLen += jpeg_size;
//   size_t dc_err = fwrite(dc_buf, 1, 4, avifile);
//   size_t ze_err = fwrite(zero_buf, 1, 4, avifile);
//   bw = millis();
//   size_t err = fwrite(fb->buf, 1, fb->len, avifile);
//   totalw = totalw + millis() - bw;
//   // digitalWrite(33, HIGH); // red led is on durng the photo and the write

//   if (millis() - bigdelta > capture_interval * 1.25) {  // how many are 25 % overtime
//     overtime_count = overtime_count + 1;
//   }

//   bigdelta = millis();
//   remnant = (4 - (jpeg_size & 0x00000003)) & 0x00000003;
//   print_quartet(idx_offset, idxfile);
//   print_quartet(jpeg_size, idxfile);
//   idx_offset = idx_offset + jpeg_size + remnant + 8;
//   jpeg_size = jpeg_size + remnant;
//   movi_size = movi_size + remnant;

//   if (remnant > 0) {
//     size_t rem_err = fwrite(zero_buf, 1, remnant, avifile);
//   }
//   esp_camera_fb_return(fb);
//   fileposition = ftell(avifile);                           // Here, we are at end of chunk (after padding)
//   fseek(avifile, fileposition - jpeg_size - 4, SEEK_SET);  // Here we are the the 4-bytes blank placeholder
//   print_quartet(jpeg_size, avifile);                       // Overwrite placeholder with actual frame size (without padding)
//   fileposition = ftell(avifile);
//   fseek(avifile, fileposition + 6, SEEK_SET);  // Here is the FOURCC "JFIF" (JPEG header)

//   // Overwrite "JFIF" (still images) with more appropriate "AVI1"
//   size_t av_err = fwrite(avi1_buf, 1, 4, avifile);
//   fileposition = ftell(avifile);
//   fseek(avifile, fileposition + jpeg_size - 10, SEEK_SET);
// }

// /**
//  * @brief ?? i barely understand it 
//  *          it just do some data writing on a file
//  * 
//  * @param i ????????????????????????
//  * @param fd this is the file that will be written 
//  */
// inline void camera::print_quartet(unsigned long i, FILE *fd) {
//   uint8_t x[1];
//   x[0] = i % 0x100;
//   size_t i1_err = fwrite(x, 1, 1, fd);
//   i = i >> 8;
//   x[0] = i % 0x100;
//   size_t i2_err = fwrite(x, 1, 1, fd);
//   i = i >> 8;
//   x[0] = i % 0x100;
//   size_t i3_err = fwrite(x, 1, 1, fd);
//   i = i >> 8;
//   x[0] = i % 0x100;
//   size_t i4_err = fwrite(x, 1, 1, fd);
// }

// /**
//  * @brief will physically start recording 
//  * 
//  * @param fileName will be the file name of the video ex: abc,test,video
//  */
// void camera::StartRecording(String fileName) {
//   m_fileName = fileName;
//   if (recording == 1) {
//     // const char* resp = "You must Stop recording, before starting a new one.  Start over ...";
//   } else {
//     recording = 1;
//     char filename[100];
//     sprintf(filename, "/video%s.avi", m_fileName);
//     videoFile = SD.open(filename, FILE_WRITE);
//     if (!videoFile) {
//       Serial.println("Error opening video file!");
//       return;
//     }

//     for (i = 0; i < AVIOFFSET; i++) {
//       char ch = pgm_read_byte(&avi_header[i]);
//       buf[i] = ch;
//     }


//     size_t err = _mysd.write(filename ,buf);


//   }
// }

// /**
//  * @brief will stop recording
//  * 
//  */
// void camera::StopRecording() {
//   recording = 0;
//   videoFile.close();
// }

// /**
//  * @brief will take a picture
//  * 
//  * @param fileName will be the file name of the video ex: abc,test,video
//  */
// void camera::takePicture(String fileName) {

//   camera_fb_t *_fb = NULL;
//   char fname[100];
//   _fb = esp_camera_fb_get();
//   if (!_fb) {
//     // failed to capture
//   }
//   sprintf(fname, "/%s.jpg", fileName);

//   // Save picture to microSD card
//   fs::FS &fs = SD;
//   File file = fs.open(fname, FILE_WRITE);
//   if (!file) {
//     // failed to open file
//   } else {
//     file.write(_fb->buf, _fb->len);  // payload (image), payload length
//   }
//   // Close the file
//   file.close();
//   esp_camera_fb_return(fb);
// }
// void camera::major_fail() {

//   Serial.println("MAJOR FAILURE");
//   // digitalWrite(33, LOW);
//   // delay(100);
//   // digitalWrite(33, HIGH);
//   // delay(100);

//   // digitalWrite(33, LOW);
//   // delay(100);
//   // digitalWrite(33, HIGH);
//   // delay(100);

//   // digitalWrite(33, LOW);
//   // delay(100);
//   // digitalWrite(33, HIGH);
//   // // delay(100);

//   // delay(1000);

//   // digitalWrite(33, LOW);
//   // delay(500);
//   // digitalWrite(33, HIGH);
//   // delay(500);

//   // digitalWrite(33, LOW);
//   // delay(500);
//   // digitalWrite(33, HIGH);
//   // delay(500);

//   // digitalWrite(33, LOW);
//   // delay(500);
//   // digitalWrite(33, HIGH);
//   // // delay(500);

//   delay(3000);
// }
// /**
//  * @brief use this function to change the camera recording quality
//  *        qualities are {VGA,UXGA,SVGA,CIF}
//  * 
//  * @param newSttng 
//  */
// void camera::config(CameraSetting newSttng) {
//   m_cameraSetting = newSttng;
// }

// void camera::changeSetting(CameraSetting newSetting) {
//   if (newSetting == CameraSetting::VGA) {
//     /* from the setting file vga
//         new_interval:200
//         new_lenght:1800
//         new_frame_size:6
//         new_quality:10
//         new_repeat:100
//         new_xspeed:1
//         new_xlenght:3
//         new_gray:0
//         */
//     framesize = 6;
//     capture_interval = 200;
//     xlength = 1800;
//     total_frames = 1800 * 1000 / capture_interval;
//     repeat = 100;
//     quality = 5;
//     xspeed = 1;
//     gray = 0;
//   } else if (newSetting == CameraSetting::UXGA) {
//     /* from the setting file uxga2
//         new_interval:2000
//         new_lenght:1800
//         new_frame_size:10
//         new_quality:10
//         new_repeat:100
//         new_xspeed:30
//         new_xlenght:3
//         new_gray:0
//         */
//     framesize = 10;
//     capture_interval = 2000;
//     xlength = 1800;
//     total_frames = 1800 * 1000 / capture_interval;
//     repeat = 100;
//     quality = 10;
//     xspeed = 2;
//     gray = 0;
//   } else if (newSetting == CameraSetting::SVGA) {
//     /* from the setting file uxga5
//         new_interval:200
//         new_lenght:1800
//         new_frame_size:10
//         new_quality:10
//         new_repeat:100
//         new_xspeed:1
//         new_xlenght:3
//         new_gray:0
//         */
//     framesize = 10;
//     capture_interval = 200;
//     xlength = 1800;
//     total_frames = 1800 * 1000 / capture_interval;
//     repeat = 100;
//     quality = 10;
//     xspeed = 1;
//     gray = 0;
//   } else if (newSetting == CameraSetting::CIF) {
//     /* from the setting file cif
//         new_interval:50
//         new_lenght:1800
//         new_frame_size:5
//         new_quality:10
//         new_repeat:100
//         new_xspeed:1
//         new_xlenght:3
//         new_gray:0
//         */
//     framesize = 10;
//     capture_interval = 140;
//     xlength = 1800;
//     total_frames = 1800 * 1000 / capture_interval;
//     repeat = 100;
//     quality = 5;
//     xspeed = 1;
//     gray = 0;
//   } else {
//     // do nothing
//   }
// }

// /**
//  * @brief Destroy the camera::camera object
//  * 
//  */
// camera::~camera() {
// }
