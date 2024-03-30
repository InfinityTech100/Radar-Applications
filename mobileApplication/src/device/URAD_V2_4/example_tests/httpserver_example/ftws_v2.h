/**
 * @file ftws.cpp  [File Transfer Web Server]
 * @author Nader Hany (naderhany638@gmail.com)
 * @brief thi will be File Transfer Web Server class for esp32 cam
 *        to sgare files from sd card over any client,
 *        wifi mode will be AP or Station , you will be able to define
 *        the web page name and the whole api .
 * @version 2.1
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */
/*  Download/Upload files to your ESP32 + SD datalogger/sensor using wifi

 Requiered libraries:

 ESP32WebServer - https://github.com/Pedroalbuquerque/ESP32WebServer download and place in your Libraries folder

 Information from David Birds original code - not from My Circuits contribution (with my contribution/simplification to the code do whatever you wish, just mention us!):

 This software, the ideas and concepts is Copyright (c) David Bird 2018. All rights to this software are reserved.

 Any redistribution or reproduction of any part or all of the contents in any form is prohibited other than the following:
 1. You may print or download to a local hard disk extracts for your personal and non-commercial use only.
 2. You may copy the content to individual third parties for their personal use, but only if you acknowledge the author David Bird as the source of the material.
 3. You may not, except with my express written permission, distribute or commercially exploit the content.
 4. You may not transmit it or store it in any other website or other form of electronic retrieval system for commercial purposes.

 The above copyright ('as annotated') notice and this permission notice shall be included in all copies or substantial portions of the Software and where the
 software use is visible to an end-user.

 THE SOFTWARE IS PROVIDED "AS IS" FOR PRIVATE USE ONLY, IT IS NOT FOR COMMERCIAL USE IN WHOLE OR PART OR CONCEPT. FOR PERSONAL USE IT IS SUPPLIED WITHOUT WARRANTY
 OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHOR OR COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 See more at http://www.dsbird.org.uk

*/

#ifndef FTWS2_h
#define FTWS2_h

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


class ftws
{
public:
    ftws();
    ~ftws();
  void start();

private:
static AsyncWebServer server;
static SDCard mysd;
static void req_homePage(AsyncWebServerRequest *request);
static void req_downloadFile(AsyncWebServerRequest *request);
static void req_deleteFile(AsyncWebServerRequest *request);
static void req_listFiles(AsyncWebServerRequest *request);

};




#endif