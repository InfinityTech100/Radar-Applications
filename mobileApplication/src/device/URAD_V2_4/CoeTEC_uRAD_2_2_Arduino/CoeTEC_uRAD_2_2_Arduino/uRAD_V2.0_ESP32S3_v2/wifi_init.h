/**
 * @file wifi_init.h  [Wifi initialization]
 * @author Nader Hany (naderhany638@gmail.com)
 * @brief  this will initialize the wifi    
 * @version 0.1
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef WIFI_INIT_h
#define WIFI_INIT_h



#include <Arduino.h>
#include <WiFi.h>


enum class wifi_init_t
{
    WIFI_ACCESS_POINT,
    WIFI_STATION

};


bool wifiInit(wifi_init_t mode,const char* ssid,const char* passwd);


#endif
