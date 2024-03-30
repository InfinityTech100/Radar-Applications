/**
 * @file urad_routes.h  [File Transfer Web Server]
 * @author Nader Hany (naderhany638@gmail.com)
 * @brief this will be the route handler for the urad radar.
 * @version 4.1
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef URAD_ROUTES_h
#define URAD_ROUTES_h

#include <Arduino.h>
#include <WebServer.h>

#include "camera.h"
#include "uRad.h"

class urad_routes
{
private:
    static uRad *m_urad;
    static WebServer *m_server;

public:
    urad_routes(uRad *rad,WebServer *server);
    ~urad_routes();
    static void route();
};

#endif
