/**
 * @file urad_routes.cpp
 * @author Nader Hany (naderhany638@gmail.com)
 * @brief this will be the route handler for the urad radar.
 * @version 4.1
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "urad_routes.h"

/**
 * to do 
 * 1- replace urad_routes constructor with the original urad file of MMaher
 * 2- modify it on main.cpp
 * 
 */

uRad *urad_routes::m_urad;
WebServer *urad_routes::m_server;

urad_routes::urad_routes(uRad *rad,WebServer *server)
{
    m_urad = rad; 
    m_server=server;
}

urad_routes::~urad_routes()
{
    
}

void urad_routes::route()
{
    m_server->send(200, "text/plain", m_urad->GetRadarObjects());
}
