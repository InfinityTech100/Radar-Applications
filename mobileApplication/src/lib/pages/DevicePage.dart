///  this file is an interface
/// between the mobile app and uRad Device

import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import '../utils/shared.dart';
import '../widgets/FullWidthBTN.dart';
import '../widgets/StorageWidget.dart';


String selectedResolution = "SVGA";

/**
 * this will handle change in reseluotion
 */
void _onResolutionChanged(String value) {
  selectedResolution = value;

  //print("Selected resolution: $value");
}

/**
 * this will make device request to change reselouion
 */
void setQuality(String q) async {
  String ql = "";
  if (q == "SVGA") {
    ql = "svga";
  }
  if (q == "UXGA") {
    ql = "uxga";
  }
  if (q == "CIF") {
    ql = "cif";
  }
  if (q == "VGA") {
    ql = "vga";
  }
  try {
    // Replace 'YOUR_URL' with the actual URL you want to make the request to
    print("http://192.168.1.1/config?quality=$ql");
    final response =
        await http.get(Uri.parse('http://192.168.1.1/config?quality=$ql'));

    if (response.statusCode == 200) {
    } else {}
  } catch (e) {}
}

/**
 * this will show dialog to select reseloution
 */
void showDialogOnCenter(BuildContext context, Widget dialog) {
  showDialog(
    context: context,
    builder: (BuildContext context) {
      return AlertDialog(
          alignment: Alignment.center,
          backgroundColor: Colors.black87,
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(10.0),
          ),
          content: Column(
            children: [
              SizedBox(
                height: 50,
              ),
              Container(
                alignment: Alignment.center,
                child: Center(
                  child: Row(
                    children: const [
                      Icon(Icons.camera_alt_outlined),
                      SizedBox(width: 10),
                      Text("Select Camera Quality")
                    ],
                  ),
                ),
              ),
              SizedBox(
                height: 30,
              ),
              RadioListTile(
                title: Text('VGA'),
                value: 'VGA',
                groupValue: selectedResolution,
                onChanged: (a) => {
                  setQuality(a!),
                  selectedResolution = a.toString(),
                  Navigator.pop(context)
                },
              ),
              RadioListTile(
                title: Text('SVGA'),
                value: 'SVGA',
                groupValue: selectedResolution,
                onChanged: (a) => {
                  setQuality(a!),
                  selectedResolution = a.toString(),
                  Navigator.pop(context)
                },
              ),
              RadioListTile(
                title: Text('CIF'),
                value: 'CIF',
                groupValue: selectedResolution,
                onChanged: (a) => {
                  setQuality(a!),
                  selectedResolution = a.toString(),
                  Navigator.pop(context)
                },
              ),
              RadioListTile(
                title: Text('UXGA'),
                value: 'UXGA',
                groupValue: selectedResolution,
                onChanged: (a) => {
                  setQuality(a!),
                  selectedResolution = a.toString(),
                  Navigator.pop(context)
                },
              ),
            ],
          ));
    },
  );
}

class DevicePage extends StatefulWidget {
  @override
  _SettingsPageState createState() => _SettingsPageState();
}

class _SettingsPageState extends State<DevicePage> {
  String version = " 1.0.1";
  String devName = "";
  String devModelNumber = "";
  late BuildContext cntxt;


  void _showDialog() {
    showDialogOnCenter(context, MyDialog());
  }



 /**
  * this will generate footer widget buttons
  */
  Widget FooterButtons() {
    return Column(
      children: [
        FWBTN(
          icon: Icons.camera_alt,
          text: "camera",
          size: 24,
          onPressed: _showDialog,
        ),
        const SizedBox(height: 20.0),
        FWBTN(
          icon: Icons.sd_card,
          text: "Storage",
          size: 24,
          onPressed: () => {
            Navigator.push(context,
                MaterialPageRoute(builder: (context) => StorageWidget()))
          },
        ),
        const SizedBox(height: 20.0),
        FWBTN(
          icon: Icons.update,
          text: "Software Updates",
          size: 24,
          onPressed: () => {print("Software Updates")},
        ),
      ],
    );
  }

/**
 * will make device request to get info
 */
  void getDeviceInfo() async
  {
    try {
      // Replace 'YOUR_URL' with the actual URL you want to make the request to
      final response =
      await http.get(Uri.parse('http://192.168.1.1/info'));

      if (response.statusCode == 200) {
        String info=response.body;
        List<String> inf=info.split(", ,");
        setState(() {
          devName=inf.first;
          devModelNumber=inf.last;
        });

      } else {}
    } catch (e) {}
  }

  @override
  void initState() {
    super.initState();
    getDeviceInfo();

  }
  
  
  @override
  Widget build(BuildContext context) {
    cntxt = context;
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.black,
        title: Text('Device Settings'),
        leading: BackButton(
          onPressed: () {
            radarMonitorEnable = true;
            Navigator.pop(context);
          },
        ),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Container(
              alignment: Alignment.topCenter,
              child: Container(
                width: 200,
                height: 200,
                child: Image.asset(
                    'assets/images/device.png'), // Replace 'assets/device.png' with the path to your image
              ),
            ),
            const SizedBox(height: 26.0),
            Center(
                child: Column(
              children: [
                Text(
                  devName,
                  style: const TextStyle(
                      fontSize: 18.0,
                      fontWeight: FontWeight.bold,
                      color: Colors.white),
                ),
                const SizedBox(height: 5.0),
                Text(
                  devModelNumber,
                  style:const TextStyle(
                      fontSize: 10.0,
                      fontWeight: FontWeight.bold,
                      color: Colors.white70),
                ),
              ],
            )),
            const SizedBox(height: 16.0),
            const SizedBox(height: 26.0),
            FooterButtons()
          ],
        ),
      ),
    );
  }
}

class MyDialog extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return const AlertDialog(
      title: Text('Camer Setting'),
      backgroundColor: Colors.white70,
      content: Center(
        child: Text('This is my dialog.'),
      ),
    );
  }
}
