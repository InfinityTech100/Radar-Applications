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
#include "wifi_init.h"

bool wifiInit(wifi_init_t mode, const char* ssid, const char* passwd)
{

    if (mode == wifi_init_t::WIFI_ACCESS_POINT)
    {
        IPAddress ip(192, 168, 1, 1);
        IPAddress gateway(192, 168, 1, 1);
        IPAddress subnet(255, 255, 255, 0);
        WiFi.softAPConfig(ip, gateway, subnet);
        WiFi.softAP(ssid, passwd);
        return true;
    }
    else
    {
        int timeout=0;
        WiFi.begin(ssid, passwd);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(1000);
            timeout++;
            if (timeout>10)
            {
               return false;
            }
            
        }
        return true;
    }
}
