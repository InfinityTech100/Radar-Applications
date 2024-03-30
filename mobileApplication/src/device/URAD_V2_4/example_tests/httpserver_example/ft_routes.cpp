/**
 * @file ftws.cpp  [File Transfer Web Server]
 * @author Nader Hany (naderhany638@gmail.com)
 * @brief thi will be File Transfer Web Server routes for esp32 cam
 *        to sgare files from sd card over any client,
 *        wifi mode will be AP or Station , you will be able to define
 *        the web page name and the whole api .
 * @version 4.1
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ft_routes.h"

SDCard * ft_routes::m_sd;

/*******************************************************
 * @brief Construct a new ft routes::ft routes object
 * 
 * @param sd pass a pointer to sdCard device here
 */
ft_routes::ft_routes(SDCard* sd)
{
  m_sd=sd;

}

/****************************************************
 * @brief Destroy the ft routes::ft routes object
 * 
 */
ft_routes::~ft_routes()
{

}

/*******************************************************************
 * @brief this will get the default page of our device
 *        will be called when u open this link http://192.168.1.1/
 * 
 * @param request this is the incomming request
 */
void ft_routes::homePage_route(AsyncWebServerRequest *request)
{
    const char *html_head = (char *)m_sd->read("/index.html");
    request->send(200, "text/html", html_head);
}

/*********************************************************
 * @brief this will handel the download requests 
 *        you can use it like this :-
 *        http://192.168.1.1/get?file=/<YourFileName>
 * 
 * @param request this is the incomming request
 */
void ft_routes::downlad_route(AsyncWebServerRequest *request)
{
    if (request->args() > 0) 
    {
      unsigned int i=0;
      String key = request->argName(i);
      String value = request->arg(i);

      if(key=="file")
      {
        File file = SD_MMC.open(value);
        if(!file){
          request->send(404, "text/plain", "File not found");
          return;
        }
        request->send(SD_MMC, value, "application/octet-stream", true);

      }

    }

}
/*************************************************************
 * @brief this will handel the delete requests 
 *        you can use it like this :-
 *        http://192.168.1.1/delete?file=/<YourFileName>
 * 
 * @param request this is the incomming request
 */
void ft_routes::delete_route(AsyncWebServerRequest *request)
{
    if (request->args() > 0) 
    {
      unsigned int i=0;
      String key = request->argName(i);
      String value = request->arg(i);

      if(key=="file")
      {
        m_sd->deleteFile(value.c_str());
        request->send(200, "text/plain", String(m_sd->listDir()));

      }

    }

}

/************************************************************
 * @brief this will handel the delete requests 
 *        you can use it like this :-
 *        http://192.168.1.1/delete?file=/<YourFileName>
 * 
 * @param request this is the incomming request 
 */
void ft_routes::listDir_route(AsyncWebServerRequest *request)
{
  request->send(200, "text/plain", String(m_sd->listDir()));
}

