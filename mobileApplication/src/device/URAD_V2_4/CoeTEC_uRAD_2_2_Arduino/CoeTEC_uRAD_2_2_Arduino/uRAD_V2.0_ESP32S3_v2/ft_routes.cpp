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

SDCard *ft_routes::m_sd;
WebServer *ft_routes::m_server;

/*******************************************************
 * @brief Construct a new ft routes::ft routes object
 *
 * @param sd pass a pointer to sdCard device here
 */
ft_routes::ft_routes(SDCard *sd, WebServer *server)
{
  m_sd = sd;
  m_server = server;
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
void ft_routes::homePage_route()
{
    Serial.println("http_task::ft_routes.cpp:: Home Page///");

  const char *html_head = (char *)m_sd->read("/index.html");
  m_server->send(200, "text/html", html_head);
}

/*********************************************************
 * @brief this will handel the download requests
 *        you can use it like this :-
 *        http://192.168.1.1/get?file=/<YourFileName>
 *
 * @param request this is the incomming request
 */
void ft_routes::downlad_route()
{
  Serial.println("http_task::ft_routes.cpp:: Download Started");
  String requestedFile = m_server->arg("file");
  if (requestedFile.length() > 0)
  {
    File file = SD.open(requestedFile);
    if (!file)
    {
      m_server->send(404, "text/plain", "File not found");
      return;
    }
    // m_server->send(200, "application/octet-stream",requestedFile);
    m_server->streamFile(file, "application/octet-stream");
    Serial.printf("Downloaded: %s\n", requestedFile.c_str());
    file.close();
  }
}
/*************************************************************
 * @brief this will handel the delete requests
 *        you can use it like this :-
 *        http://192.168.1.1/delete?file=/<YourFileName>
 *
 * @param request this is the incomming request
 */
void ft_routes::delete_route()
{
  Serial.println("http_task::ft_routes.cpp:: Deleting File");

  String requestedFile = m_server->arg("file");
  if (requestedFile.length() > 0)
  {
    m_sd->deleteFile(requestedFile.c_str());
    Serial.printf("Deleted: %s\n", requestedFile.c_str());
    m_server->send(200, "text/plain", String(m_sd->listDir()));
  }
}

/************************************************************
 * @brief this will handel the delete requests
 *        you can use it like this :-
 *        http://192.168.1.1/delete?file=/<YourFileName>
 *
 * @param request this is the incomming request
 */
void ft_routes::listDir_route()
{
  Serial.println("http_task::ft_routes.cpp:: listing SD directories");
  m_server->send(200, "text/plain", String(m_sd->listDir()));
  Serial.println("http_task::ft_routes.cpp:: Sent Directory List to HTTP");
  
}
