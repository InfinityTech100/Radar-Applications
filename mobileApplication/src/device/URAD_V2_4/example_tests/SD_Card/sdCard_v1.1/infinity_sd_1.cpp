#include "infinity_sd.h"
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
#if USE_BUILTIN_SD 0

/**
 * @brief Construct a new SDCard::SDCard object
 *
 */

SDCard::SDCard()
{
}
/**
 * @brief use this function to write/create a file
 *
 * @param path
 * @param buf the content that should be written inside this file
 * @return true if success
 * @return false failed
 */
bool SDCard::write(String path, const uint8_t *buf)
{
    // Save picture to microSD card
    File file = SD.open(path.c_str(), FILE_WRITE);
    int sz = szof(buf);
    if (!file)
    {
        return false;
    }
    else
    {
        file.write(buf, sz); // payload (image), payload length
        return true;
    }
    // Close the file
    file.close();
}
/**
 * @brief will return the size of a buffer
 *
 * @param buf
 * @return int
 */
int SDCard::szof(const uint8_t *buf)
{
    int size2 = 0;
    for (const uint8_t *ptr = buf; *ptr != '\0'; ++ptr)
    {
        ++size2;
    }
    return size2;
}
/**
 * @brief will list the content of a directory
 *
 * @return String the results in one string
 */
String SDCard::listDir()
{
    File root = SD.open("/");
    String output;
    while (true)
    {
        File file = root.openNextFile();
        if (!file)
            break;
        output = output + file.name() + "  #  ";
        // Serial.print("FILE NAME: ");
        // Serial.println(file.name());
        file.close();
    }
    return output;
}
/**
 * @brief read any file at any path
 *
 * @param path
 * @return uint8_t* the content of file in raw format
 */
uint8_t *SDCard::read(String path)
{
    File file = SD.open(path);
    size_t fileSize = file.size();
    uint8_t *fileContent = (uint8_t *)malloc(fileSize);
    file.read(fileContent, fileSize);
    file.close();
    return fileContent;
}
/**
 * @brief act as pwd command
 *
 * @param path
 * @return std::vector<String>
 */
std::vector<String> SDCard::listFiles(const String &path)
{
    // Create a vector to store the file names.
    std::vector<String> files;
    File root = SD.open(path);
    while (true)
    {
        File file = root.openNextFile();
        if (!file)
            break;
        files.push_back(file.name());

        file.close();
    }
    return files;
}

/**
 * @brief use it for mounting your sd card
 *
 * @return true if success
 * @return false failed
 */
bool SDCard::mount()
{

#ifdef REASSIGN_PINS
    SPI.begin(sck, miso, mosi, cs);
#endif
    // if(!SD.begin(cs)){ //Change to this function to manually change CS pin
    if (!SD.begin())
    {
        Serial.println("Card Mount Failed");
        return false;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
        return false;
    }
    return true;
}

/**
 * @brief Destroy the SDCard::SDCard object
 *
 */
SDCard::~SDCard()
{
}

#endif