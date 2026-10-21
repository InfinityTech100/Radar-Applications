import 'dart:ffi';

import 'package:flutter/material.dart';
import '../utils/StorageManager.dart';
import '../utils/shared.dart';
import '../widgets/videoPlayer.dart';

class GalleryPage extends StatefulWidget {
  @override
  _SettingsPageState createState() => _SettingsPageState();
}

class _SettingsPageState extends State<GalleryPage> {
  String template =
      "Wi-Fi, short for Wireless Fidelity, is a technology that allows wireless communication between devices using radio waves. It provides a means to connect devices to the internet and to each other without the need for physical cables.";
  // List<String> items = FileManager.listDirectory("CoETEC");
  List<String> items = [];
  String p = '';
  int inside = 0;
/**
 * will just display connect to download files when no files exist
 */
  Widget wifiDisconnected() {
    return Padding(
      padding: EdgeInsets.all(16.0),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Container(
            alignment: Alignment.topCenter,
            child: Container(
              width: 200,
              height: 200,
              child: Image.asset(
                  'assets/images/wifi.png'), // Replace 'assets/device.png' with the path to your image
            ),
          ),
          const SizedBox(height: 26.0),
          Center(
              child: Column(
            children: [
              const SizedBox(height: 5.0),
              Container(
                margin: const EdgeInsets.only(left: 50, right: 50),
                child: Text(
                  template,
                  style: const TextStyle(
                      fontSize: 13.0,
                      fontWeight: FontWeight.bold,
                      color: Colors.white70),
                ),
              )
            ],
          )),
          const SizedBox(height: 16.0),

          const SizedBox(height: 26.0),

          // FooterButtons()
        ],
      ),
    );
  }

  /**
   * will get the already downloaded files
   */
  Widget wifiConnected() {
    return Column(
      children: [
        Expanded(
          child: ListView.builder(
            itemCount: items.length,
            itemBuilder: (BuildContext context, int index) {
              return Column(
                children: [
                  Row(
                    children: [
                      const SizedBox(
                        width: 40,
                      ),
                      const Icon(Icons.filter),
                      const SizedBox(
                        width: 10,
                      ),
                      TextButton(
                          onPressed: () async {
                            if (inside == 0) {
                              if (items[index].contains('accidents')) {
                                items.clear();
                                p = await FileManager.getPath();
                                p = "$p/CoETEC/accidents/";
                                items = FileManager.listDirectory(p);
                                setState(() {});
                                inside++;
                                return;
                              }
                              if (items[index].contains('idle')) {
                                items.clear();
                                p = await FileManager.getPath();
                                p = "$p/CoETEC/idle/";
                                items = FileManager.listDirectory(p);
                                setState(() {});
                                inside++;
                                return;
                              }
                            }
                            Navigator.push(
                              context,
                              MaterialPageRoute(
                                  builder: (context) => VideoPlayerWidget(
                                      videoUrl: items[index])),
                            );
                          },
                          child: Text(
                            items[index].replaceAll(p, ""),
                            style: const TextStyle(
                                color: Colors.white, fontSize: 15),
                          )),
                    ],
                  ),
                ],
              );
            },
          ),
        )
      ],
    );
  }

  Future pathGetter() async {
    p = await FileManager.getPath();
    p = p + "/CoETEC/";
    setState(() {
      items = FileManager.listDirectory(p);
    });
    print(items.toString());
  }

  @override
  void initState() {
    super.initState();
    pathGetter();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.black,
        title: const Text('Gallery'),
        leading: BackButton(
          onPressed: () async {
            if (inside > 0) {
              await pathGetter();
              inside--;
              return;
            }
            radarMonitorEnable = true;
            Navigator.pop(context);
          },
        ),
      ),
      body: (items.isNotEmpty) ? wifiConnected() : wifiDisconnected(),
      // body: wifiConnected(),
    );
  }
}
