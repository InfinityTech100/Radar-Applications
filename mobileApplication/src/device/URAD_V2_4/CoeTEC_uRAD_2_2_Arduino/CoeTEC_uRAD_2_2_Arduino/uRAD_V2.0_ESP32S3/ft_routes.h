/**
 * @file ft_routes.h  [File Transfer Web Server]
 * @author Nader Hany (naderhany638@gmail.com)
 * @brief thi will be File Transfer Web Server routes for esp32 cam
 *        to share files from sd card over any client,
 *        wifi mode will be AP or Station , you will be able to define
 *        the web page name and the whole api .
 * @version 4.1
 * @date 2023-07-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef FTWS3_h
#define FTWS3_h

#include <Arduino.h>
#include <WebServer.h>

#include "infinity_sd.h"


class ft_routes
{
private:
  static SDCard *m_sd;
  static WebServer *m_server;

public:
    ft_routes(SDCard* sd,WebServer * server);
    ~ft_routes();
    static void homePage_route();
    static void downlad_route ();
    static void delete_route  ();
    static void listDir_route ();

};










#endif
