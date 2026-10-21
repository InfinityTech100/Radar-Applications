/**
 * @file camera.cpp
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

#include "camera.h"

/**
 * @brief Construct a new camera::camera object
 *
 */
camera::camera()
{
#if (USE_ESP_CAM == 1)
    pinMode(33, OUTPUT);   // little red led on back of chip
    digitalWrite(33, LOW); // turn on the red LED on the back of chip

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

    Serial.println("before psramFound........");
    // init with high specs to pre-allocate larger buffers
    if (psramFound())
    {
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 1;
        config.fb_count = 2;
    }
    else
    {
        config.frame_size = FRAMESIZE_SVGA; // svga 12 fails due to jpg 60000
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }
    Serial.println("after psramFound........");

    // camera init
    cam_err = esp_camera_init(&config);
    if (cam_err != ESP_OK)
    {
        major_fail();
        return;
    }

    digitalWrite(33, HIGH);

    //
    //  startup defaults  -- EDIT HERE
    //  zzz

    sensor_t *s = esp_camera_sensor_get();
    s->set_framesize(s, FRAMESIZE_VGA);
    s->set_quality(s, 10);
    do_fb(); // do a couple captures to make sure camera has new config
    do_fb();
#else

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
    config.frame_size = FRAMESIZE_UXGA;
    config.pixel_format = PIXFORMAT_JPEG;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 12;
    config.fb_count = 1;

    // init with high specs to pre-allocate larger buffers
    if (config.pixel_format == PIXFORMAT_JPEG)
    {
        if (psramFound())
        {
            config.jpeg_quality = 10;
            config.fb_count = 2;
            config.grab_mode = CAMERA_GRAB_LATEST;
        }
        else
        {
            // Limit the frame size when PSRAM is not available
            config.frame_size = FRAMESIZE_SVGA;
            config.fb_location = CAMERA_FB_IN_DRAM;
        }
    }
    else
    {
        // Best option for face detection/recognition
        config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
        config.fb_count = 2;
#endif
    }

    // camera init
    cam_err = esp_camera_init(&config);
    if (cam_err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", cam_err);
        // major_fail();
        return;
    }
    sensor_t *s = esp_camera_sensor_get();
    s->set_framesize(s, FRAMESIZE_VGA);
    s->set_quality(s, 10);
    do_fb(); // do a couple captures to make sure camera has new config
    do_fb();
#endif

    framesize = 6; // vga
    repeat = 100;  // 100 videos
    xspeed = 1;    // playback at 1 x realtime
    gray = 0;      // not gray

    quality = 10;           // quality 10 - pretty good.  Goes from 0..63, but 0-5 sometimes fails on bright scenery (jpg too big for ESP32CAM system)
    capture_interval = 200; // 200 milli-secconds per frame
    total_frames = 9000;    // 9000 frames x 200 ms = 1800 sec = 30 min

    xlength = total_frames * capture_interval / 1000;

    newfile = 0;   // no file is open  // don't fiddle with this!
    recording = 0; // start recording on reboot without sending a command
}

/**
 * @brief this should be inside a while(1) function
 *
 */
void camera::handleRequests()
{
    wakeup = millis();
    if (wakeup - last_wakeup > (10 * 60 * 1000))
    { // 10 minutes
        last_wakeup = millis();
    }
    make_avi();
}

/**
 * @brief this function will start creating
 *        an avi file
 *
 */
void camera::make_avi()
{
    // we are recording, but no file is open
    if (newfile == 0 && recording == 1)
    { // open the file
        start_avi();
        digitalWrite(33, HIGH);
        newfile = 1;
        totalp = 0;
        totalw = 0;
        frame_cnt = 0;
        frames_so_far = 0;
    }
    else
    {
        // we have a file open, but not recording
        if (newfile == 1 && recording == 0)
        { // got command to close file
            digitalWrite(33, LOW);
            end_avi();
            frames_so_far = total_frames;
            newfile = 0;   // file is closed
            recording = 0; // DO NOT start another recording
        }
        else
        {
            if (newfile == 1 && recording == 1)
            { // regular recording
                if (frames_so_far == total_frames)
                {                          // we are done the recording
                    digitalWrite(33, LOW); // close the file
                    end_avi();
                    frames_so_far = 0;
                    newfile = 0; // file is closed
                    if (repeat > 0)
                    {
                        recording = 1; // start another recording
                        repeat = repeat - 1;
                    }
                    else
                    {
                        recording = 0;
                    }
                }
                else if ((millis() - startms) > (total_frames * capture_interval))
                {
                    digitalWrite(33, LOW); // close the file
                    end_avi();
                    frames_so_far = 0;
                    newfile = 0; // file is closed
                    if (repeat > 0)
                    {
                        recording = 1; // start another recording
                        repeat = repeat - 1;
                    }
                    else
                    {
                        recording = 0;
                    }
                }
                else
                { // regular
                    current_millis = millis();
                    if (current_millis - last_capture_millis > capture_interval)
                    { // Take another picture - fixed interval
                        last_capture_millis = millis();
                        frames_so_far = frames_so_far + 1;
                        frame_cnt++;
                        another_pic_avi();
                    }
                }
            }
        }
    }
}
/**
 * @brief will create and write the avi header
 *
 *
 */
void camera::start_avi()
{
    do_fb(); // start the camera ... warm it up
    delay(1000);
    do_fb();
    delay(1000);
    do_fb();
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%F__%H-%M-%S", &timeinfo);
    char fname[100];

    if (m_cameraSetting == CameraSetting::VGA)
    {
        sprintf(fname, "/%s_vga.avi", m_fileName);
    }
    else if (m_cameraSetting == CameraSetting::UXGA)
    {
        sprintf(fname, "/%s_uxga.avi", m_fileName);
    }
    else if (m_cameraSetting == CameraSetting::SVGA)
    {
        sprintf(fname, "/%s_svga.avi", m_fileName);
    }
    else if (m_cameraSetting == CameraSetting::CIF)
    {
        sprintf(fname, "/%s_cif.avi", m_fileName);
    }
    else
    {
        sprintf(fname, "/%s_xxx_Q%d_I%d_L%d_S%d.avi", strftime_buf, quality, capture_interval, xlength, xspeed);
    }

    Serial.print("\nFile name will be >");
    Serial.print(fname);
    Serial.println("<");

    avifile = SD_CARD.open(fname, "w", true);
    idxfile = SD_CARD.open("/idx.tmp", "w", true);

    if (avifile != NULL)
    {
        // no error
    }
    else
    {
        // error
        major_fail();
    }

    if (idxfile != NULL)
    {
    }
    else
    {
        // error
        major_fail();
    }
    for (i = 0; i < AVIOFFSET; i++)
    {
        char ch = pgm_read_byte(&avi_header[i]);
        buf[i] = ch;
    }

    avifile.write(buf, AVIOFFSET);

    uint16_t frameWidth, frameHeight;

    if (framesize == 6)
    {
        frameWidth = vga_w;
        frameHeight = vga_h;
    }
    else if (framesize == 10)
    {
        frameWidth = uxga_w;
        frameHeight = uxga_h;
    }
    else if (framesize == 7)
    {
        frameWidth = svga_w;
        frameHeight = svga_h;
    }
    else if (framesize == 5)
    {
        frameWidth = cif_w;
        frameHeight = cif_h;
    }
    else
    {
        // Handle unsupported framesize value
        avifile.close();
        idxfile.close();
        return;
    }

    // Set parameters at different positions in the avifile
    avifile.seek(0x40, SeekSet);
    avifile.write(reinterpret_cast<uint8_t *>(&frameWidth), sizeof(frameWidth));
    avifile.seek(0xA8, SeekSet);
    avifile.write(reinterpret_cast<uint8_t *>(&frameWidth), sizeof(frameWidth));
    avifile.seek(0x44, SeekSet);
    avifile.write(reinterpret_cast<uint8_t *>(&frameHeight), sizeof(frameHeight));
    avifile.seek(0xAC, SeekSet);
    avifile.write(reinterpret_cast<uint8_t *>(&frameHeight), sizeof(frameHeight));

    // Move the file pointer to AVIOFFSET
    avifile.seek(AVIOFFSET, SeekSet);

    startms = millis();
    bigdelta = millis();
    totalp = 0;
    totalw = 0;
    overtime_count = 0;
    jpeg_size = 0;
    movi_size = 0;
    uVideoLen = 0;
    idx_offset = 4;
}

/**
 * @brief will end recording on the file
 *
 * @return esp_err_t
 */
void camera::end_avi()
{
    unsigned long current_end = 0;
    // current_end = ftell(avifile);
    current_end = avifile.position();
    elapsedms = millis() - startms;
    float fRealFPS = (1000.0f * (float)frame_cnt) / ((float)elapsedms) * xspeed;
    float fmicroseconds_per_frame = 1000000.0f / fRealFPS;
    uint8_t iAttainedFPS = round(fRealFPS);
    uint32_t us_per_frame = round(fmicroseconds_per_frame);

    avifile.seek(4, SeekSet);
    print_quartet(movi_size + 240 + 16 * frame_cnt + 8 * frame_cnt, avifile);
    avifile.seek(0x20, SeekSet);
    print_quartet(us_per_frame, avifile);
    unsigned long max_bytes_per_sec = movi_size * iAttainedFPS / frame_cnt;
    avifile.seek(0x24, SeekSet);
    print_quartet(max_bytes_per_sec, avifile);
    avifile.seek(0x30, SeekSet);
    print_quartet(frame_cnt, avifile);

    avifile.seek(0x84, SeekSet);
    print_quartet(static_cast<int>(iAttainedFPS), avifile);

    avifile.seek(0xe8, SeekSet);
    print_quartet(movi_size + frame_cnt * 8 + 4, avifile);

    avifile.seek(current_end, SeekSet);

    // Close the idxfile
    idxfile.close();

    size_t i1_err = avifile.write(idx1_buf, 4);
    avifile.seek(0, SeekEnd);
    print_quartet(frame_cnt * 16, avifile);
    idxfile = SD_CARD.open("/idx.tmp", "r");

    if (idxfile != NULL)
    {
    }
    else
    {
        // major_fail();
    }
    char *AteBytes;
    AteBytes = (char *)malloc(8);
    for (int i = 0; i < frame_cnt; i++)
    {

        size_t res = idxfile.read((uint8_t *)AteBytes, 8);
        size_t i1_err = avifile.write(dc_buf, 4);
        size_t i2_err = avifile.write(zero_buf, 4);
        size_t i3_err = avifile.write((uint8_t *)AteBytes, 8);
    }

    idxfile.close();
    avifile.close();
}
/**
 * @brief will be used to heat the camera
 *
 * @return esp_err_t
 */
void camera::do_fb()
{
    camera_fb_t *fb = esp_camera_fb_get();
    esp_camera_fb_return(fb);
}

/**
 * @brief will be used to take another frame on the avi file
 *
 * @return esp_err_t
 */
void camera::another_pic_avi()
{
    digitalWrite(33, LOW);
    bp = millis();
    fb = esp_camera_fb_get();
    totalp = totalp + millis() - bp;
    jpeg_size = fb->len;
    movi_size += jpeg_size; // Update totals
    uVideoLen += jpeg_size;
    size_t dc_err = avifile.write(dc_buf, 4);
    size_t ze_err = avifile.write(zero_buf, 4);
    bw = millis();
    size_t err = avifile.write(fb->buf, fb->len);
    totalw = totalw + millis() - bw;
    digitalWrite(33, HIGH); // red led is on durng the photo and the write

    if (millis() - bigdelta > capture_interval * 1.25)
    { // how many are 25 % overtime
        overtime_count = overtime_count + 1;
    }

    bigdelta = millis();
    remnant = (4 - (jpeg_size & 0x00000003)) & 0x00000003;
    print_quartet(idx_offset, idxfile);
    print_quartet(jpeg_size, idxfile);
    idx_offset = idx_offset + jpeg_size + remnant + 8;
    jpeg_size = jpeg_size + remnant;
    movi_size = movi_size + remnant;

    if (remnant > 0)
    {
        // size_t rem_err = fwrite(zero_buf, 1, remnant, avifile);
        size_t rem_err = avifile.write(zero_buf, remnant);
    }
    esp_camera_fb_return(fb);
    fileposition = avifile.position(); // Get the current position (end of chunk after padding)
    avifile.seek(fileposition - jpeg_size - 4, SeekSet);
    print_quartet(jpeg_size, avifile); // Overwrite placeholder with actual frame size (without padding)

    fileposition = avifile.position();
    avifile.seek(fileposition + 6, SeekSet);
    size_t av_err = avifile.write(avi1_buf, 4);
    fileposition = avifile.position();
    avifile.seek(fileposition + jpeg_size - 10, SeekSet);
}

/**
 * @brief ?? i barely understand it
 *          it just do some data writing on a file
 *
 * @param i ????????????????????????
 * @param fd this is the file that will be written
 */
inline void camera::print_quartet(unsigned long i, fs::File fd)
{

    uint8_t x[1];

    x[0] = i % 0x100;
    size_t i1_err = fd.write(x, 1);
    i = i >> 8;

    x[0] = i % 0x100;
    size_t i2_err = fd.write(x, 1);
    i = i >> 8;

    x[0] = i % 0x100;
    size_t i3_err = fd.write(x, 1);
    i = i >> 8;

    x[0] = i % 0x100;
    size_t i4_err = fd.write(x, 1);
}

/**
 * @brief will physically start recording
 *
 * @param fileName will be the file name of the video ex: abc,test,video
 */
void camera::StartRecording(String fileName)
{
    m_fileName = fileName;
    esp_err_t res = ESP_OK;
    if (recording == 1)
    {
        // const char* resp = "You must Stop recording, before starting a new one.  Start over ...";
    }
    else
    {
        recording = 1;
        sensor_t *s = esp_camera_sensor_get();
        int new_framesize = s->status.framesize;
        int new_quality = s->status.quality;
        int new_gray = 0;
        s->set_quality(s, new_quality);
        s->set_framesize(s, (framesize_t)new_framesize);
        if (new_gray == 1)
        {
            s->set_special_effect(s, 2); // 0 regular, 2 grayscale
        }
        else
        {
            s->set_special_effect(s, 0); // 0 regular, 2 grayscale
        }
        changeSetting(m_cameraSetting);
        do_fb();
        do_fb(); // let camera warm up
    }
}

/**
 * @brief will stop recording
 *
 */
void camera::StopRecording()
{
    recording = 0;
}

/**
 * @brief will take a picture
 *
 * @param fileName will be the file name of the video ex: abc,test,video
 */
void camera::takePicture(String fileName)
{

    fb = esp_camera_fb_get();
    char fname[100];
    if (!fb)
    {
        // failed to capture
        Serial.println("camera_task::camera.cpp:: Failed to capture (Failed to get Camera frame buffer!!)");
        return;
    }
    sprintf(fname, "/%s.jpg", fileName);

    // Save picture to microSD card
    fs::FS &fs = SD_CARD;
    File file = fs.open(fname, FILE_WRITE);
    if (!file)
    {
        // failed to open file
        Serial.println("camera_task::camera.cpp:: Failed to open file");
        return;
    }
    else
    {
        file.write(fb->buf, fb->len); // payload (image), payload length
    }
    // Close the file
    file.close();
    esp_camera_fb_return(fb);
    Serial.println("camera_task::camera.cpp:: Captured a picture successfully");

    // camera_fb_t *_fb = NULL;
    // char fname[100];
    // _fb = esp_camera_fb_get();
    // if (!_fb)
    // {
    //     // failed to capture
    // }
    // sprintf(fname, "/%s.jpg", fileName);
    // // Save picture to microSD card
    // fs::FS &fs = SD_CARD;
    // Serial.println("pass A");
    // File file = fs.open(fname, FILE_WRITE);
    // if (!file)
    // {
    //     // failed to open file
    // }
    // else
    // {
    //     file.write(_fb->buf, _fb->len); // payload (image), payload length
    // }
    // // Close the file
    // Serial.println("pass B");

    // file.close();
    // esp_camera_fb_return(fb);
    // Serial.println("pass C");
}
void camera::major_fail()
{
    while (1)
    {
        digitalWrite(33, LOW);
        delay(100);
        digitalWrite(33, HIGH);
        delay(100);

        digitalWrite(33, LOW);
        delay(100);
        digitalWrite(33, HIGH);
        delay(100);

        digitalWrite(33, LOW);
        delay(100);
        digitalWrite(33, HIGH);
        // delay(100);

        delay(1000);

        digitalWrite(33, LOW);
        delay(500);
        digitalWrite(33, HIGH);
        delay(500);

        digitalWrite(33, LOW);
        delay(500);
        digitalWrite(33, HIGH);
        delay(500);

        digitalWrite(33, LOW);
        delay(500);
        digitalWrite(33, HIGH);
        // delay(500);

        delay(1000);
    }
}
/**
 * @brief use this function to change the camera recording quality
 *        qualities are {VGA,UXGA,SVGA,CIF}
 *
 * @param newSttng
 */
void camera::config(CameraSetting newSttng)
{
    m_cameraSetting = newSttng;
}

void camera::changeSetting(CameraSetting newSetting)
{
    if (newSetting == CameraSetting::VGA)
    {

        framesize = 6;
        capture_interval = 200;
        xlength = 1800;
        total_frames = 1800 * 1000 / capture_interval;
        repeat = 100;
        quality = 5;
        xspeed = 1;
        gray = 0;
    }
    else if (newSetting == CameraSetting::UXGA)
    {

        framesize = 10;
        capture_interval = 2000;
        xlength = 1800;
        total_frames = 1800 * 1000 / capture_interval;
        repeat = 100;
        quality = 10;
        xspeed = 2;
        gray = 0;
    }
    else if (newSetting == CameraSetting::SVGA)
    {

        framesize = 10;
        capture_interval = 200;
        xlength = 1800;
        total_frames = 1800 * 1000 / capture_interval;
        repeat = 100;
        quality = 10;
        xspeed = 1;
        gray = 0;
    }
    else if (newSetting == CameraSetting::CIF)
    {

        framesize = 10;
        capture_interval = 140;
        xlength = 1800;
        total_frames = 1800 * 1000 / capture_interval;
        repeat = 100;
        quality = 5;
        xspeed = 1;
        gray = 0;
    }
    else
    {
        // do nothing
    }
}

/**
 * @brief Destroy the camera::camera object
 *
 */
camera::~camera()
{
}
