/**
 * @file infinity_sd.h
 * @author Nader Hany (naderhany638@gmail.com)
 * @brief sd card example
 * @version 0.1
 * @date 2023-07-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#include "infinity_sd.h"


SDCard mysd;

void setup() {

  Serial.begin(115200);


  String mount_msg =(mysd.mount()==true)?"sd card mounted successfully":"failed to mount sd card";

  Serial.println(mount_msg);
  mysd.write("/hellow.txt",(uint8_t*)"hello from c++");

 
  std::vector<String> dirfiles=mysd.listFiles("/");
  for (auto i : dirfiles)
  {
    Serial.print("file:");
    Serial.println(i);
  }
  String msgg=(char*)mysd.read("/hellow.txt");
  Serial.println(msgg);
  


  

}

void loop() {
  // put your main code here, to run repeatedly:

}
















