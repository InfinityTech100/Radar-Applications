/**
 * @file main.cpp
 * @author Nader Hany (naderhany638@gmail.com)
 * @brief this is the third version of CoETEC_urad app
 *        the problem with the previous version was
 *        the asyncronus web server made esp restart
 *        so here we will use syncronus web server .
 * @version 27.1
 * @date 2023-07-21
 *
 * @copyright Copyright (c) 2023
 *
 */

/**
 * libraries
 * 
 */
#include <Arduino.h>
#include <WebServer.h>

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "ft_routes.h"
#include "urad_routes.h"
#include "uRad.h"
#include "wifi_init.h"
#include "camera.h"


#define OPNO_FILE_PATH "/opereationNoFile.txt"

/****************************************************************
 * @brief this is the wifi paramters
 * @param ssid            will be the ssid of our AP wifi
 * @param password        will be the password of our AP wifi
 * @param DeviceName      will be the device name on the app
 * @param DeviceModel     will be the device model
 *
 */
const char *ssid = "CoETEC";
const char *password = "12345678";
const char *DeviceName = "CoETEC uRAD Radar";
const char *DeviceModel = "2572003";

/****************************************************************
 * @param op_fileCTR       will be used incremented every reset event 
 * @param msg              will be used for Debugging
 * @param rec              will be used for video recording [Radar,Camera]
 * @param server           will be used as the main web server
 * @param mysd             will be used as sd card manager
 * @param uradRadar        will be used as uRAD redar
 * @param q_utcCTR         will be used as utcCTR [Camera ,UTC]
 * @param q_camerQuality   will be used as [Camera]
 * @param q_camerCapture   will be used as uRAD redar [Camera]
 */
int op_fileCTR = 0;
String msg;
SDCard mysd;
WebServer server(80);
uRad uradRadar;
QueueHandle_t rec, q_utcCTR, q_camerQuality, q_camerCapture;



/****************************************************
 * @brief will be used to update or initialize
 *        OpereationNumberFile.txt
 *
 */
void init_opnf();

/***************************************************
 * @brief task for the radar responsible for detecting
 *        objects and do auto record action when object
 *        detected.
 *
 * @param pvParameters
 */
void radar_task(void *pvParameters);

/***************************************************
 * @brief task for the camera responsible for video 
 *        recording and image capturing.
 *
 * @param pvParameters
 */
void camera_task(void *pvParameters);

/***************************************************
 * @brief task for the unix time stamp responsible
 *        for naming the files regardless of image or
 *        video files.
 *
 * @param pvParameters
 */
void unixTimeStamp_task(void *pvParameters);

/*****************************************************
 * @brief task for handling http requests responsible
 *        for handling file download,delete,list requests
 *        and also for sendin urad objects , device info
 *        also handling control requests like capture,quality.
 * 
 * @callergraph 
 *      1) home page       @http://192.168.1.1/            
 *      2) download file   @http://192.168.1.1/get?file=/<FileName>
 *      3) delete file     @http:192.168.1.1/delete?file=/<FileName>
 *      4) list files      @http://192.168.1.1/list
 *      5) device info     @http://192.168.1.1/info
 *      6) radar status    @http://192.168.1.1/urad
 *      7) camera quality  @http://192.168.1.1/config?quality=<QUALITY>
 *      8) camera capture  @http://192.168.1.1/capture
 * 
 * @param pvParameters 
 */
void httpServer_task(void *pvParameters);

/***************************************************
 * @brief act as a rout handler for setting camera
 *        quality.
 * 
 */
void cameraQualityControl_route();

/**
 * @brief act as a rout handler for camera capture.
 * 
 */
void cameraManuaCapture_route();

/**
 * @brief act as a rout handler for device info.
 * 
 */
void deviceInfo_route();

/*****************************************************
 * @brief act as int main(void){}
 * 
 */
void setup() {

  // initialize the serial
  Serial.begin(115200);
  delay(5000);



  // initialize wifi in Access Point Mode
  wifi_init_t mode = wifi_init_t::WIFI_ACCESS_POINT;
  msg = (wifiInit(mode, ssid, password) == true) ? "wifi initialized successfully....." : "failed to initalize wifi";
  Serial.println(msg);

  // mounting the sd card
  msg = (mysd.mount() == true) ? "sd card mounted succesfully..." : "failed to mount sd card....";
  Serial.println(msg);




  // get and update the opereation File
  init_opnf();


  // create task handlers
  TaskHandle_t t1;
  TaskHandle_t t2;
  TaskHandle_t t3;
  TaskHandle_t t4;

  // create task queues
  rec = xQueueCreate(100, sizeof(bool));
  q_utcCTR = xQueueCreate(50, sizeof(uint32_t));
  q_camerQuality = xQueueCreate(30, sizeof(CameraSetting));
  q_camerCapture = xQueueCreate(30, sizeof(bool));

  // create tasks
  // xTaskCreate(radar_task, "Radar", 4096, NULL, 3, &t1);
  xTaskCreate(camera_task, "Camera", 4096, NULL, 2, &t2);
  xTaskCreate(httpServer_task, "HTTP", 4096, NULL, 2, &t4);
  xTaskCreate(unixTimeStamp_task, "UTC", 1024, NULL, 1, &t3);

  // Disable brownout detector
  /**
   * @brief Error: “Brownout detector was triggered”
   * It's often related to one of the following issues: Poor quality USB cable;
   * USB cable is too long; Board with some defect (bad solder joints);
   *
   * note : fuck you this will get cpu panic !!!!!!!!!!
   *
   */
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable   detector

  // start freeRTOS Scheduler
  // vTaskStartScheduler();
}

void loop() {
}

/**********************************************************
 * @brief basically every time the device reset , this 
 *        will increment the value on file OPNO_FILE_PATH.
 * steps:
 *      1) read the whole sd files and get OPNO_FILE_PATH.
 *      2) if not found ,then will create one else will 
 *         update the file with new value.
 *      3) increment the counter
 * 
 */
void init_opnf() {
  std::vector<String> files = mysd.listFiles("/");
  bool exist = false;
  for (auto i : files) {
    String tmpp = "/" + i;
    if (tmpp == String(OPNO_FILE_PATH)) {
      msg = "opereation File Detected ....";
      exist = true;
      break;
    }
  }
  if (exist == true) {
    Serial.println(msg);
  } else {
    mysd.write(OPNO_FILE_PATH, (uint8_t *)"0 ,");
    msg = "opereation File Created ....";
    Serial.println(msg);
  }

  char *t = (char *)mysd.read(OPNO_FILE_PATH);
  const char delimiter[] = " ,";
  char *token;
  token = strtok(t, delimiter);

  int opfile = String(token).toInt();
  Serial.print("Opereation Number Is:");
  Serial.println(opfile);
  bool st = mysd.deleteFile(OPNO_FILE_PATH);
  opfile++;
  String out = String(opfile) + " ,";
  const char *tmp = out.c_str();
  msg = (mysd.write(OPNO_FILE_PATH, (uint8_t *)tmp) == true) ? "opereation File Updated Successfully ...." : "Failed to update Operation File ....";
  Serial.println(msg);
  op_fileCTR = opfile;
}

/****************************************************
 * @brief the main radar task will auto record video
 *        and save it on sd card with the utc value.
 * 
 * steps:
 *    1) check the radar sensor if objects detected
 *    2) if object detected send true to rec queue
 *       else send false to rec.
 * 
 * @param pvParameters 
 */
void radar_task(void *pvParameters) {

  bool record = false;
  Serial.println("radar_task::starting");
  uradRadar.begin();
  while (1) {
    // Serial.println("radar_task::running");
    uradRadar.handleTasks();
    if (uradRadar.isObject() == true) {
      Serial.println("radar_task::object Detected ");
      record = true;
      xQueueSend(rec, (void *)&record, (TickType_t)300);
    } else {
      // record = false;
      // xQueueSend(rec, (void *)&record, (TickType_t)300);
    }
  }
}

/*****************************************************
 * @brief this is camera task act as server to respond 
 *        client requests.
 * steps:
 *     1) get the clients requests by recieving 
 *        the following queues:
 *            rec for video recording action.
 *            q_utcCTR to read the current unix time stamp .
 *            q_camerQuality to change img/video quality.
 *            q_camerCapture for taking picture request.
 *     2) if record request recieved , it will go inside a FSM of
 *        two states to ensure no double start record to occure or it will
 *        get a bad error .
 *     3) if capture or change quality request recieved , it will make sure it is
 *        not recording and then take the photo or change quality setting.
 * 
 * @param pvParameters 
 */
void camera_task(void *pvParameters) {
  Serial.println("camera_task::Initializing...");
  // initalize camera
  camera *OV2640Cam = new camera();
  CameraSetting m_config = CameraSetting::SVGA;
  OV2640Cam->config(m_config);

  bool fsm = false;
  bool rcrd = false;
  bool manualCapture = false;
  uint32_t utcCTr = 0;

  Serial.println("camera_task::starting");

  while (1) {
    //    Serial.println("camera_task::running");
    xQueueReceive(rec, &(rcrd), (TickType_t)20);
    xQueueReceive(q_utcCTR, &(utcCTr), (TickType_t)10);
    xQueueReceive(q_camerQuality, &(m_config), (TickType_t)10);
    xQueueReceive(q_camerCapture, &(manualCapture), (TickType_t)10);

    if (fsm == false) {
      OV2640Cam->config(m_config);
      if (manualCapture == true) {
        // String newImagePath = "IMG_" + String(op_fileCTR) + "_" + String(utcCTr);
        // Serial.println("camera_task::Camera capture ");
        // OV2640Cam->takePicture(newImagePath);
         String newVideoPath = "VID_" + String(op_fileCTR) + "_" + String(utcCTr);
        Serial.println("camera_task::Camera start Recording ");
        OV2640Cam->StartRecording(newVideoPath);  // Start recording video when object is detected. 
        manualCapture = false;
      }
    }
    if (fsm == false) {

      if (rcrd == true) {
        fsm = true;
        String newVideoPath = "VID_" + String(op_fileCTR) + "_" + String(utcCTr);
        OV2640Cam->StartRecording(newVideoPath);  // Start recording video when object is detected.
      }
    } else {
      if (rcrd == false) {
        fsm = false;

        OV2640Cam->StopRecording();  // Stop recording video when no object
      }
    }
    OV2640Cam->handleRequests();
   
  }
}

/**************************************************************
 * @brief will increment UTC_CTR every 1 second ,very simple
 *  
 * 
 * @param pvParameters 
 */
void unixTimeStamp_task(void *pvParameters) {
  delay(1500);
  uint32_t UTC_CTR = 0;
  Serial.println("unixTimeStamp_task::starting");
  while (1) {
    // Serial.println("unixTimeStamp_task::running");
    UTC_CTR++;
    Serial.print("unixTimeStamp::opereation time = ");
    Serial.println(String(UTC_CTR));
    xQueueSend(q_utcCTR, (void *)&UTC_CTR, (TickType_t)5);
    delay(1000);
  }
}


/***********************************************************
 * @brief will respond to the whole http requests
 * 
 * @param pvParameters 
 */
void httpServer_task(void *pvParameters) {
  delay(1000);
  Serial.println("http server task starting........");
  ft_routes FileTransferRoutes(&mysd, &server);
  urad_routes radarRoutes(&uradRadar, &server);
  server.on("/", FileTransferRoutes.homePage_route);
  server.on("/get", FileTransferRoutes.downlad_route);
  server.on("/list", FileTransferRoutes.listDir_route);
  server.on("/delete", FileTransferRoutes.delete_route);
  server.on("/urad", radarRoutes.route);
  server.on("/config", cameraQualityControl_route);
  server.on("/capture", cameraManuaCapture_route);
  server.on("/info", deviceInfo_route);

  server.begin();

  while (1) {
    // Serial.println("http server task:: runing");
    // bool tmp = false;
    // xQueueSend(q_camerCapture, (void *)&tmp, (TickType_t)1);
    server.handleClient();
  }
}

/******************************************************
 * @brief will change the camera quality , very simple
 * 
 *   steps: 
 *      1) read the requested quality 
 *      2) send the value to q_camerQuality queue to Camera Task
 * 
 */
void cameraQualityControl_route() {
  String qulity = server.arg("quality");
  CameraSetting stng;
  if (qulity == "svga") {
    stng = CameraSetting::SVGA;
    xQueueSend(q_camerQuality, (void *)&stng, (TickType_t)300);
    Serial.println("camera quality changed to SVGA");
  } else if (qulity == "uxga") {
    stng = CameraSetting::UXGA;
    xQueueSend(q_camerQuality, (void *)&stng, (TickType_t)300);
    Serial.println("camera quality changed to uxga");

  } else if (qulity == "vga") {
    stng = CameraSetting::VGA;
    xQueueSend(q_camerQuality, (void *)&stng, (TickType_t)300);
    Serial.println("camera quality changed to VGA");
  } else {
    stng = CameraSetting::CIF;
    xQueueSend(q_camerQuality, (void *)&stng, (TickType_t)300);
    Serial.println("camera quality changed to CIF");
  }
}

/*********************************************************************
 * @brief will take a picture 
 * 
 *  steps: 
 *      1) send true on q_camerCapture queue to be read by Camera Task
 * 
 */
void cameraManuaCapture_route() {
  bool tmp = true;
  Serial.println("http_task:main.cpp:: Received Captured a picture cmd");
  xQueueSend(q_camerCapture, (void *)&tmp, (TickType_t)300);
}

/***********************************************************************
 * @brief will return the device info 
 * 
 */
void deviceInfo_route() {
  String info = String(DeviceName) + ", ," + String(DeviceModel);
  server.send(200, "text/plain", info);
}
/*************************** END OF FILE ********************************/
