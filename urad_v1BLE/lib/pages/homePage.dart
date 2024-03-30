import 'dart:async';
import 'dart:typed_data';

import 'package:coetec_radar/pages/DevicePage.dart';
import 'package:coetec_radar/widgets/Btn.dart';
import 'package:coetec_radar/widgets/Car.dart';
import 'package:flutter/material.dart';

import '../widgets/Battery.dart';
import '../widgets/BikeIndicator.dart';
import 'package:coetec_radar/pages/SettingPage.dart';
import 'package:coetec_radar/pages/GalleryPage.dart';
import 'package:coetec_radar/utils/BluetoothController.dart';
import 'package:coetec_radar/widgets/devicesDialog.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:flutter/foundation.dart'; // Import ChangeNotifier class
import 'package:permission_handler/permission_handler.dart';
import 'package:get/get.dart';

class BlueCam extends GetxController {
  // FlutterBluePlus fble=FlutterBluePlus.instance;

}

class HomePge extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    // Start scanning
    return MaterialApp(
      debugShowCheckedModeBanner: false, // Remove debug banner
      theme: ThemeData(
        brightness: Brightness.dark,
        primaryColor: Colors.black,
        //accentColor: Color(0xFF3BB5EA),
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
  List<String> drawerItems = [];
  List<String> drawerItems2 = [];
  Timer? timer;
  Timer? timer2;
  var myBLT;
  List<double> objectsDist = [];
  List<Widget> cars = [];
  String st = "safe";

  BatteryLevelWidget mybattery = BatteryLevelWidget(
    batteryLevel: 0.80,
    frameWidth: 50,
    frameHeight: 20,
  );

  void initPermessions() async {
    WidgetsFlutterBinding.ensureInitialized();
    [
      Permission.location,
      Permission.storage,
      Permission.bluetooth,
      Permission.bluetoothConnect,
      Permission.bluetoothScan
    ].request();
    await FlutterBluePlus.turnOn();

    // WidgetsFlutterBinding.ensureInitialized(); // Initialize the Flutter binding
    // PermissionStatus st3 = await Permission.bluetooth.request();
    // PermissionStatus st2 = await Permission.location.request();
    // PermissionStatus st4 = await Permission.bluetoothConnect.request();
    // PermissionStatus st5 = await Permission.bluetoothScan.request();
    // PermissionStatus st = await Permission.storage.request();
  }

  @override
  void initState() {
    super.initState();
    initPermessions();
    // myBLT=AbstractBLT(
    //   onDataRecieved:Brecieved_callback,
    // );

    startTimer();
  }

  @override
  void dispose() {
    timer?.cancel(); // Cancel the timer when the state is disposed
    // timer2?.cancel();
    super.dispose();
  }

  void startTimer() {
    timer = Timer.periodic(Duration(milliseconds: 100), (Timer timer) {
      setState(() {
        //drawerItems = myBLT.getDevice_Names();
      });
    });
    timer2 = Timer.periodic(Duration(milliseconds: 10000), (Timer timer) {});
  }

  void Brecieved_callback(List<int> data) {
    objectsDist.clear();
    if (data[0] == 0xfd) {
      int objsz = data[1];
      for (int i = 0; i < objsz; i++) {
        objectsDist.add(data[i + 2].toDouble());
      }
    }
    // 1-sort distances
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
    print("data" + objectsDist.toString());
  }

  void setDrawerItems(List<String> newItems, List<String> newItems2) {
    drawerItems.clear();
    drawerItems.addAll(newItems);
    drawerItems2.clear();
    drawerItems2.addAll(newItems2);
  }

  /**
   *  serviceUuids: [4fafc201-1fb5-459e-8fcc-c5c9c331914b]}, rssi: -63, timeStamp: 2023-08-16 20:20:11.098165}
   */
  void SelectDevice(String sd, String sda) async{
    BluetoothDevice selectedBLE = BluetoothDevice(
        remoteId: DeviceIdentifier(sda),
        localName: sd,
        type: BluetoothDeviceType.le);
    await selectedBLE.connect(timeout: const Duration(seconds: 3), autoConnect: true);

    print("characterestics=");
    var x2=await selectedBLE.discoverServices(timeout:15 );
    BluetoothCharacteristic x3;

   for(var i in x2)
     {

       if(i.serviceUuid.toString()=='4fafc201-1fb5-459e-8fcc-c5c9c331914b')
         {
           print("object");
           List<BluetoothCharacteristic> mc=i.characteristics;
           for(var j in mc)
             {
               if(j.characteristicUuid.toString()=='beb5483e-36e1-4688-b7f5-ea07361b26a8')
                 {
                   x3=j;
                   x3.setNotifyValue(true,timeout: 15);
                   // List<int> ss=await x3.read(timeout: 15);
                   // x3.onValueReceived.listen((event) { print("ss=");print(event);});
                   x3.onValueReceived.listen(Brecieved_callback);
                 }
             }
         }
       print(i.serviceUuid);
       print("****************************************************************");
       print("****************************************************************");

     }


    // Get the characteristic that will be used to receive data
    // BluetoothCharacteristic characteristic =

// Enable notifications for the characteristic
    //selectedBLE.
    // Get the BluetoothConnection object for the device.

    // SDevice=sd;
    // SDeviceA=sda;
    // print(" Selected dev "+ SDevice+"  Selected dev address "+ SDeviceA);
    // myBLT.connect(sda);
  }

  void RefreshBluetooth() async {
    print("hello2");
    setState(() {
      drawerItems.clear();
      drawerItems2.clear();
    });
    var x =
        await FlutterBluePlus.startScan(timeout: const Duration(seconds: 5));

    List<BluetoothDevice> mdevices = [];
    for (var i in x) {
      mdevices.add(i.device);
    }
    print("devices=");
    print(x);

    setState(() {
      drawerItems.clear();
      drawerItems2.clear();

      for (var i in mdevices) {
        drawerItems.add(i.localName);
        drawerItems2.add(i.remoteId.toString());
      }
      // myBLT.search();
      // drawerItems = myBLT.getDevice_Names();
      // drawerItems2 = myBLT.getDevice_Adresses();
    });
  }

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

  Widget vLines() {
    double screenHeight = sh;
    double lineStart = (sh > 600) ? (screenHeight / 6) : (screenHeight / 4);
    double end = (sh > 600) ? 130 : 80;
    return Column(
      children: [
        SizedBox(height: lineStart+40),
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

  void pressed() {
    print("pressed...");
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
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => DevicePage()),
              )
            },
          ),
        ),
        // Gallery
        Container(
          margin: EdgeInsets.only(left: 20),
          height: 30,
          width: 30,
          child: BTN(
            icon: Icons.image,
            size: 30,
            //onPressed:pressed,
            onPressed: () => {
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

  Widget MyFooter(double ht, double wd) {
    return Row(children: [
      Container(
        margin: EdgeInsets.only(top: ht - 80, left: 20),
        height: 60,
        width: 60,
        // color: Colors.lightBlue,
        child: BTN(
          icon: Icons.camera_alt,
          size: 60,
          //onPressed:pressed,
          onPressed: () => {print("aaa2")},
        ),
      ),
      Container(
        margin: EdgeInsets.only(top: ht - 80, left: wd - 20 - 60 - 80),
        height: 60,
        width: 60,
        // color: Colors.lightBlue,
        child: BTN(
          icon: Icons.video_call,
          size: 60,
          //onPressed:pressed,
          onPressed: () => {print("aaa3")},
        ),
      )
    ]);
  }

  @override
  Widget build(BuildContext context) {
    final Size screenSize = MediaQuery.of(context).size;
    final double screenWidth = screenSize.width;
    final double screenHeight = screenSize.height;
    sw = screenWidth;
    sh = screenHeight;
    double space1 = (sw / 2) - 110;
    double space2 = (sw > 400)
        ? sw - space1 - 40 - 30 - 40 - 120
        : sw - space1 - 40 - 30 - 10 - 140;
    double space3 =
        (sw > 400) ? sw - space1 - 40 - 15 - 260 : sw - space1 - 40 - 15 - 230;

    CustomDrawer DevicesDrawer = CustomDrawer(
      items: drawerItems,
      items_description: drawerItems2,
      selectedSetter: SelectDevice,
      rfrsh: RefreshBluetooth,
      setItems: setDrawerItems,
    );

    // cars.add(Car(displacement: 50, visible: true));
    // cars.add(Car(displacement: 50, visible: true));
    // cars.add(Car(displacement: 50, visible: true));
    return Scaffold(
      body: Container(
        width: sw,
        height: sh,
        //  color:  Color(0xFFF41624),
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
      drawer: DevicesDrawer,
    );
  }
}
