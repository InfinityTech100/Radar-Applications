/**
 * @file infinity_sd.h
 * @author Nader Hany (naderhany638@gmail.com)
 * @brief this file used to read/write files inside sd card
 *        for esp32-cam
 * @version 0.1
 * @date 2023-07-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SD_H
#define SD_H

// MicroSD
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include "FS.h"              // https://github.com/espressif/arduino-esp32/blob/master/libraries/FS/
#include "SD_MMC.h"          // https://github.com/espressif/arduino-esp32/blob/master/libraries/SD_MMC/src/SD_MMC.h

#include <vector>
#include <iostream>


/**
 * @brief the sd card class
 * 
 */
class SDCard
{    
public:
    SDCard();
    bool mount();
    bool write(String path,const uint8_t *buf);
    uint8_t* read(String path);
    size_t getFileSize(String path);
    bool deleteFile(const char* path);
    std::vector<String> listFiles(const String& path);
    String listDir();

    ~SDCard();
private:
    int szof(const uint8_t *buf);

    
};





#endif // SD_H