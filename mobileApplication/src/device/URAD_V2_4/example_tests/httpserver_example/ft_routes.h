/**
 * @file ftws.cpp  [File Transfer Web Server]
 * @author Nader Hany (naderhany638@gmail.com)
 * @brief thi will be File Transfer Web Server routes for esp32 cam
 *        to share files from sd card over any client,
 *        wifi mode will be AP or Station , you will be able to define
 *        the web page name and the whole api .
 * @version 4.1
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef FTWS3_h
#define FTWS3_h

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h> 
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h> //https://github.com/me-no-dev/ESPAsyncWebServer.git

#include "infinity_sd.h"


class ft_routes
{
private:
  static SDCard *m_sd;
public:
    ft_routes(SDCard* sd);
    ~ft_routes();
    static void homePage_route(AsyncWebServerRequest *request);
    static void downlad_route(AsyncWebServerRequest *request);
    static void delete_route(AsyncWebServerRequest *request);
    static void listDir_route(AsyncWebServerRequest *request);

};










#endif
