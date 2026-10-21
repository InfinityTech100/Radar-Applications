import 'dart:convert';
import 'dart:async';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:location/location.dart';
import 'package:permission_handler/permission_handler.dart';
import 'package:urad2/utils/deviceRequests.dart';
import 'package:urad2/utils/gyroscope.dart';
import 'DevicePage.dart';
import 'SettingPage.dart';
import 'GalleryPage.dart';
import '../widgets/Btn.dart';
import '../widgets/Car.dart';
import '../utils/unixTimeStamp.dart';
import '../utils/shared.dart';
import '../widgets/Battery.dart';
import '../widgets/BikeIndicator.dart';

class HomePge extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false, // Remove debug banner
      theme: ThemeData(
        brightness: Brightness.dark,
        primaryColor: Colors.black,
        // accentColor: Color(0xFF3BB5EA),
        scaffoldBackgroundColor: Colors.black,
        textTheme: const TextTheme(
          bodyText1: TextStyle(color: Colors.white),
          bodyText2: TextStyle(color: Colors.white),
        ),
      ),
      home: CenteredTextPage(),
    );
  }
}

class CenteredTextPage extends StatefulWidget {
  @override
  _CenteredTextPageState createState() => _CenteredTextPageState();
}

class _CenteredTextPageState extends State<CenteredTextPage> {
  String SDevice = "";
  String SDeviceA = "";
  double sw = 0;
  double sh = 0;
  Timer? timer;
  var myBLT;
  List<double> objectsDist = [];
  List<Widget> cars = [];
  String st = "safe";
  bool disconnected = true;
  late LocationData _currentLocation;
  late Location location;
  double dbgMsg = 0.0;
  bool autoRecord = false;
  late Gyro myGyro;
  bool manualStart = false;
  double? mySpeed = 0;
  int modeCtr=0;
  BatteryLevelWidget mybattery = BatteryLevelWidget(
    batteryLevel: 0.80,
    frameWidth: 50,
    frameHeight: 20,
  );

  void initUTC() async {
    DeviceRequests req = DeviceRequests();
    bool x = await req.setUnixTimeStamp();
    ////todo: make this log an internal error
  }

  @override
  void initState() {
    super.initState();
    initPermessions();
    // myGyro = Gyro();
    // myGyro.setTreasholds(80, 120);

    initUTC();
    startTimer();
  }

  @override
  void dispose() {
    timer?.cancel();
    super.dispose();
  }

  void initPermessions() async {
    try {
      WidgetsFlutterBinding.ensureInitialized();
      [
        Permission.location,
        Permission.storage,
      ].request();
      print("step 1");

      location = Location();
      location.enableBackgroundMode(enable: true);
      print("step 2");
    } catch (e) {
      print(e);
    }
    print("step 3");
    bool serviceEnabled;
    try {
      serviceEnabled = await location.serviceEnabled();
      if (!serviceEnabled) {
        serviceEnabled = await location.requestService();
        print("success");
      }
    } catch (e) {
      print("failesd");
    }
    print("step 4");

    _currentLocation = LocationData.fromMap({
      "latitude": (0.0).toDouble(),
      "longitude":(0.0).toDouble(),
      "accuracy":(0.0).toDouble(),
      "altitude": (0.0).toDouble(),
      "speed": (0.0).toDouble(),
      "speedAccuracy": (0.0).toDouble(),
      "heading": (0.0).toDouble(),
      "time":(0.0).toDouble(),
      "isMock": false,
    });
    location.onLocationChanged.listen((LocationData currentLocation) {
      print("inside");
      _currentLocation = currentLocation;
    });
  }

  /// ******************************************************************************
  /// this is a timer callback to make request every 200 ms to get the device radar
  /// status and draw the cars if objects detected.
  void startTimer() async {
    timer =
        Timer.periodic(const Duration(milliseconds: 180), (Timer timer) async {
      // if (myGyro.isAccident() == true) {
      //   DeviceRequests req = DeviceRequests();
      //   bool x = await req.emmitAccident();
      //   ////todo: make this log an internal error
      // }
      /**
        * if device wifi disconnected it will clear all cars
        */
          if(disconnected==false){
            setState(() {
              cars.clear();
              st = "safe";
            });
          }
        modeCtr++;
          if(modeCtr==10){
            setState(() {
              cars.clear();
              st = "safe";
            });
          }
      if (radarMonitorEnable == true) {
        /**
           * steps for getting radar info
           * make http request on http://192.168.1.1/urad
           * get the text message and parse it
           */
        try {
          // Replace 'YOUR_URL' with the actual URL you want to make the request to
          final response = await http.get(Uri.parse('http://192.168.1.1/urad'));
          if(disconnected == false &&modeCtr>10)
            {
               initUTC();
               DeviceRequests mreq=DeviceRequests();
               mreq.setAutoRecordMode(manualStart);
            }

          if (response.statusCode == 200) {
            // Encode the string using UTF-8 encoding
            // List<int> utf8Bytes = utf8.encode(response.body);
            List<String> tmp = response.body.toString().split(',');
            List<int> tmp2 = [];
            // print("step A");
            for (var i in tmp) {
              if (i != "") {
                tmp2.add(int.parse(i));
              }
            }
            // print('parcing done');
            // LocationData x = await location.getLocation();
            // print(x.speed);
            Brecieved_callback(tmp2);

            // Uint8List u8List = Uint8List.fromList(utf8Bytes);
            // List<int> mlist=[0xfd,2,30,1,25,40,1,25];
            // Brecieved_callback(Uint8List.fromList( mlist));
            disconnected = true;
          } else {}
        } catch (e) {
          disconnected = false;
          // print("failed B");
        }
      }
    });
  }

  /// ***************************************************************
  /// this is a callback function when data is recieved
  void Brecieved_callback(List<int> data) {
    modeCtr=0;
    try {
      mySpeed = _currentLocation.speed; //
      // print(mySpeed);
    } catch (e) {
      print("failed to get device speed ,maybe un initialized .! ");
    }
    //double? mySpeed =10; //for dbg purpose
    setState(() {
      dbgMsg = mySpeed!;
    });

    // print(data);
    objectsDist.clear();
    if (data[0] == 0xfd) {
      int objsz = data[1];
      for (int i = 0; i < objsz * 3; i = i + 3) {
        if ((data[i + 3] > 0) && (data[i + 4].toDouble() > mySpeed!)) {
          objectsDist.add(data[i + 2].toDouble() / 2);
        }
        // objectsDist.add(data[i + 2].toDouble());
      }
    }
    // print(objectsDist);
    if ((data[0] == 0xfd) && (data[1] == objectsDist.length)) {
      // print("displaying objects");
      objectsDist.sort();
      // 2-translate distances
      double of = 0;
      if (sh > 600) {
        of = (sh / 24);
      } else {
        of = ((sh / 18) - 30);
      }
      double offset = 176 + of;
      cars.clear();
      double sum = 0;
      for (int i = 0; i < objectsDist.length; i++) {
        if (objectsDist[i] < 50) {
          sum = 0;
          double y = ((objectsDist[i] * (sh - offset) / 60));
          // show the cars
          cars.add(Car(displacement: y, visible: true));
        }
      }
      // show bike status
      if (objectsDist[0] < 10) {
        setState(() {
          st = "danger";
        });
      } else if (objectsDist[0] < 30) {
        setState(() {
          st = "warning";
        });
      } else {
        setState(() {
          st = "safe";
        });
      }
    } else {
      setState(() {
        cars.clear();
        st = "safe";
      });
    }

    //print(data);
    // objectsDist.clear();
    // if (data[0] == 64) {
    //   int objsz = data[1];
    //   for (int i = 0; i < objsz; i++) {
    //     objectsDist.add(data[i + 2].toDouble());
    //   }
    // }
    // // 1-sort distances
    // objectsDist.sort();
    // // 2-translate distances
    // double of = 0;
    // if (sh > 600) {
    //   of = (sh / 24);
    // } else {
    //   of = ((sh / 18) - 30);
    // }
    // double offset = 176 + of;
    // cars.clear();
    // double sum = 0;
    // for (int i = 0; i < objectsDist.length; i++) {
    //   if (objectsDist[i] < 50) {
    //     sum = 0;
    //     double y = ((objectsDist[i] * (sh - offset) / 60));
    //     /**
    //      * every time you add a car it will displayed on screen
    //      */
    //     cars.add(Car(displacement: y, visible: true));
    //   }
    // }
    // // show bike status
    // if (objectsDist[0] < 10) {
    //   setState(() {
    //     st = "danger";
    //   });
    // } else if (objectsDist[0] < 30) {
    //   setState(() {
    //     st = "warning";
    //   });
    // } else {
    //   setState(() {
    //     st = "safe";
    //   });
    // }
    // print("data" + objectsDist.toString());
  }

  /// *********************************************************************
  /// will create the battery widget
  Widget Battery() {
    return Align(
      alignment: Alignment.topLeft,
      child: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            mybattery,

            const SizedBox(height: 16),
            // Other widgets
          ],
        ),
      ),
    );
  }

  /// **********************************************************************
  /// will draw vertical lines on screen
  Widget vLines() {
    double screenHeight = sh;
    double lineStart = (sh > 600) ? (screenHeight / 6) : (screenHeight / 4);
    double end = (sh > 600) ? 130 : 80;
    return Column(
      children: [
        SizedBox(height: lineStart),
        Center(
          child: Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Container(
                width: 2,
                height: sh - lineStart - end,
                color: Colors.white,
              ),
              SizedBox(width: 100),
              Container(
                width: 2,
                height: sh - lineStart - end,
                color: Colors.white,
              ),
            ],
          ),
        )
      ],
    );
  }

  /// ***********************************************************************
  /// will make the device take picture
  void makeCaptureRequest() async {
    if (autoRecord == false) {
      setState(() {
        manualStart = !manualStart;
      });
      if(manualStart==true){
        DeviceRequests myreq=DeviceRequests();
        myreq.setAutoRecordMode(true);
      }
      else{
        DeviceRequests myreq=DeviceRequests();
        myreq.setAutoRecordMode(false);
      }

      // try {
      //   DeviceRequests requester = DeviceRequests();
      //   final status = await requester.setAutoRecordMode(manualStart);
      //   if (status) {
      //     print("record action......");
      //   } else {
      //     print("failed to make record action.....");
      //   }
      // } catch (e) {}
    }
  }

  Widget MyAppBar(double space1, double space2, double space3) {
    return Row(
      children: [
        Battery(),
        // Device Settings
        Container(
          margin: EdgeInsets.only(left: space1),
          height: 50,
          width: 50,
          child: BTN(
            icon: Icons.devices_other,
            size: 50,
            //onPressed:pressed,
            onPressed: () => {
              radarMonitorEnable = false,
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => DevicePage()),
              )
            },
          ),
        ),
        // Gallery
        Container(
          margin: EdgeInsets.only(left: space2),
          height: 30,
          width: 30,
          child: BTN(
            icon: Icons.image,
            size: 30,
            //onPressed:pressed,
            onPressed: () => {
              radarMonitorEnable = false,
              print("aaa"),
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => GalleryPage()),
              )
            },
          ),
        ),
        // General Setting
        Container(
          margin: EdgeInsets.only(left: space3),
          height: 40,
          width: 40,
          child: BTN(
            icon: Icons.settings,
            size: 40,
            //onPressed:pressed,
            onPressed: () => {
              radarMonitorEnable = false,
              print("General setting"),
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => SettingsPage()),
              )
            },
          ),
        )
      ],
    );
  }

  /// *********************************************************************
  /// this is the fotter of the screen contain two buttons
  Widget MyFooter(double ht, double wd) {
    return Stack(
      children: [
        Column(
          children: [
            Row(children: [
              Container(
                margin: EdgeInsets.only(top: ht - 120, left: 20),
                height: 60,
                width: 60,
                child: BTN(
                  icon: Icons.camera_alt,
                  color: (autoRecord == true)
                      ? Colors.cyan
                      : (manualStart == true)
                          ? Colors.green.shade600
                          : Colors.black,
                  accentColor:
                      (autoRecord == true) ? Colors.black : Colors.white,
                  size: 45,
                  onPressed: () => {
                    makeCaptureRequest(),
                  },
                ),
              ),
            ]),
            // Row(
            //   children: [
            //     Switch(
            //         value: autoRecord,
            //         activeColor: Colors.cyan,
            //         onChanged: (e) {
            //           setState(() {
            //             autoRecord = !autoRecord;
            //           });
            //           if (autoRecord == true) {
            //             DeviceRequests myRequests = DeviceRequests();
            //             myRequests.setAutoRecordMode(true);
            //           } else {
            //             DeviceRequests myRequests = DeviceRequests();
            //             myRequests.setAutoRecordMode(false);
            //           }
            //         }),
            //     Text(
            //       (autoRecord) ? "auto record" : "manual",
            //       style: const TextStyle(color: Colors.white),
            //     )
            //   ],
            // )
          ],
        ),
        // Align(
        //   alignment: Alignment.bottomRight,
        //   child: Container(
        //     height: 60,
        //     width: 60,
        //     margin: const EdgeInsets.only(bottom: 50, right: 15),
        //     child: BTN(
        //         icon: Icons.assessment_outlined,
        //         onPressed: () {
        //           print("calibrating");
        //           myGyro.setOrigin();
        //         },
        //         size: 45),
        //   ),
        // ),
        // Align(
        //   alignment: Alignment.bottomRight,
        //   child: Container(
        //       margin: const EdgeInsets.only(bottom: 27, right: 20),
        //       child: const Text("calibrate")),
        // ),
      ],
    );
  }

  @override
  Widget build(BuildContext context) {
    final Size screenSize = MediaQuery.of(context).size;
    final double screenWidth = screenSize.width;
    final double screenHeight = screenSize.height;
    // print(UTC.now());
    sw = screenWidth;
    sh = screenHeight;
    double space1 = (sw / 2) - 110;
    double space2 = (sw > 400)
        ? sw - space1 - 40 - 30 - 40 - 120
        : sw - space1 - 40 - 30 - 10 - 140;
    double space3 =
        (sw > 400) ? sw - space1 - 40 - 15 - 260 : sw - space1 - 40 - 15 - 230;

    return Scaffold(
      body: Container(
        width: sw,
        height: sh,
        child: Stack(
          children: [
            vLines(),
            Column(
              children: [
                const SizedBox(height: 40), // Add spacing
                MyAppBar(space1, space2, space3),
                SizedBox(
                  height: (sh > 600) ? ((sh / 24)) : ((sh / 18) - 30),
                ), // Add spacing
                BikeIndicator(state: st),
                Stack(
                  children: cars,
                )
              ],
            ),
            MyFooter(sh, sw),
          ],
        ),
      ),
    );
  }
}
