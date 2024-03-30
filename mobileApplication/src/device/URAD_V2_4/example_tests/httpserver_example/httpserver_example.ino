/**
 * @file httpserver_example.ino
 * @author Nader Hany (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h> 
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h> //https://github.com/me-no-dev/ESPAsyncWebServer.git

#include "ft_routes.h"
#include "wifi_init.h"
//#include "infinity_sd.h"
#include <FS.h>


AsyncWebServer server(80);
SDCard mysd;
ft_routes FileTransferRoutes(&mysd);

const char* ssid = "CoETEC";
const char* password = "00000000";


//http://192.168.4.1/hellaow?arg1=value1&arg2=value2


void setup() {

  Serial.begin(115200);


  wifi_init_t mode=wifi_init_t::WIFI_ACCESS_POINT;
  if(wifiInit(mode,ssid,password)==true)
  {
    Serial.println("wifi initialized successfully.....");
  }

  String msg=(mysd.mount()==true)?"sd card mounted succesfully...":"failed to mount sd card....";
  Serial.println(msg);  

  server.on("/"       , HTTP_GET,FileTransferRoutes.homePage_route);
  server.on("/get"    , HTTP_GET,FileTransferRoutes.downlad_route);
  server.on("/list"   , HTTP_GET,FileTransferRoutes.listDir_route);
  server.on("/delete" , HTTP_GET,FileTransferRoutes.delete_route);


  server.begin();
  Serial.println("hello world ....");
  
   
}

void loop()
{
  Serial.println("hello world ....");
  delay(1000);
}

  // IPAddress ip(192, 168, 1, 1);
  // IPAddress gateway(192, 168, 1, 1);
  // IPAddress subnet(255, 255, 255, 0);
  // WiFi.softAPConfig(ip, gateway, subnet);
  // WiFi.softAP(ssid, password);
  // Serial.print("IP Address: ");
  // Serial.println(WiFi.localIP());

// void handleHelloRequest(AsyncWebServerRequest *request) {
//   // Check if there are any arguments
//   if (request->args() > 0) {
//     // Iterate over each argument
//     for (unsigned int i = 0; i < request->args(); i++) {
//       // Get the argument key and value
//       String key = request->argName(i);
//       String value = request->arg(i);
      
//       // Print the argument key and value
//       Serial.print("Argument ");
//       Serial.print(i);
//       Serial.print(": ");
//       Serial.print(key);
//       Serial.print(" = ");
//       Serial.println(value);
//     }
//   }

//   // Send a response back to the client
//   request->send(200, "text/plain", "Hello from ESP32!");
// }

    // Send a GET request to <IP>/get?message=<message>
//    server.on("/", HTTP_GET, [] (AsyncWebServerRequest *request) {
//         const char* html_head=(char*)mysd.read("/index.html");
//         request->send(200, "text/html", html_head);
//    });
 
//    server.on("/hellaow", HTTP_GET, handleHelloRequest);


//   server.on("/file.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
//     Serial.
//    request->send(200, "application/octet-stream", String(file_content));
//  });

  //  server.on("/list", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(200, "text/plain", String(mysd.listDir()));
  // });

//  server.on("/a.jpg", HTTP_GET, [](AsyncWebServerRequest *request) {
//  //  request->send_P(200, "application/octet-stream", mysd.read("a.jpg"),mysd.getFileSize("a.jpg"));
//    request->send_P(200, "image/jpeg", mysd.read("a.jpg"),mysd.getFileSize("a.jpg"));
//   // File file = SD_MMC.open("/a.jpg", "r");
//      // request->send(200, "image/jpeg", file);
// //   AsyncWebServerResponse *response = request->beginResponse(mysd.getFileSize("a.jpg"), "application/octet-stream",mysd.read("a.jpg") );
////    request->send(response);
//  });
  //   server.on("/file.jpg", HTTP_GET, [](AsyncWebServerRequest *request){
  //   File file = SD_MMC.open("/a.jpg");
  //   if(!file){
  //     request->send(404, "text/plain", "File not found");
  //     return;
  //   }
  //   request->send(SD_MMC, "/a.jpg", String(), true);
  // });
  
//  server.on("/file2.jpg", HTTP_GET, [](AsyncWebServerRequest *request){
//    File file = SD_MMC.open("/a.jpg");
//    if(!file){
//      request->send(404, "text/plain", "File not found");
//      return;
//    }
//    request->send(SD_MMC, "/a.jpg", "application/octet-stream", true);
//  });

  //   server.on("/file2.jpg", HTTP_GET, [](AsyncWebServerRequest *request){
  //   File file = SD_MMC.open("/test.avi");
  //   if(!file){
  //     request->send(404, "text/plain", "File not found");
  //     return;
  //   }
  //   request->send(SD_MMC, "/test.avi", "application/octet-stream", true);
  // });
 

