import 'dart:async';
// import 'dart:io';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:urad2/utils/unixTimeStamp.dart';
import 'package:urad2/widgets/videoPlayer.dart';
import '../utils/FileDownloader.dart';
import '../utils/StorageManager.dart';
import '../utils/shared.dart';

class StorageWidget extends StatefulWidget {
  @override
  _StatefulPageState createState() => _StatefulPageState();
}

class _StatefulPageState extends State<StorageWidget> {
  bool showProgressBar = false;
  double progrss = 0;
  List<file> myList = [];
  List<String> downloaded = [];
  Timer? timer;
  bool update = true;
  FileDownloader myDownloader = FileDownloader(WorkSpaceDirectory: "CoETEC");
  String aa = '';

  /// this will parse the names of files recived from device
  List<String> parseFilenames(String inputString) {
    List<String> filenames = inputString.split(',');
    List<String> outputList = [];
    for (String filename in filenames) {
      if (filename.contains('.itechVid') || filename.contains('.jpg')) {
        outputList.add(filename.trim());
      }
    }
    return outputList;
  }

  /// will request the device files
  Future<String> getFiles() async {
    try {
      final response = await http.get(Uri.parse('http://192.168.1.1/list'));
      if (response.statusCode == 200) {
        // print(response.body);
        return response.body;
      } else {
        print("getFiles():: bad request.....");
      }
    } catch (e) {
      print("failed to make http request");
    }
    return "";
  }

  /// this will remove file on the device
  Future<void> removeFile(String file) async {
    try {
      // Replace 'YOUR_URL' with the actual URL you want to make the request to
      final response =
          await http.get(Uri.parse('http://192.168.1.1/delete?file=/$file'));

      if (response.statusCode == 200) {
        // print(response.body);
        print("getFiles():: success .....");
      } else {
        print("getFiles():: bad request.....");
      }
    } catch (e) {
      print("getFiles():: failed to make http request");
    }
  }

  /// this will get the already downloaded files
  List<String> getSavedFiles() //implement later
  {
    return [];
  }

  /// this is a timer callback act as thread that run every
  /// file download will update its saved files
  /// steps:
  /// 1- request device files
  /// 2- parse files
  /// 3- add the files to myList so that it can be shawn in listView
  /// 4- get already downloaded files
  /// 5- check if file is downloaded mark as saved
  void startTimer() async {
    timer =
        Timer.periodic(const Duration(milliseconds: 1000), (Timer timer) async {
      if (update == true) {
        update = false;
        //  1- request device files
        String tmpp = await getFiles();
        //  2- parse files
        List<String> tmpp2 = parseFilenames(tmpp);
        //  3- add the files to myList so that it can be shawn in listView
        myList.clear();
        for (int i = 0; i < tmpp2.length; i++) {
          file f = file(
              fileName: tmpp2[i],
              originalFileName: "",
              download: false,
              saved: false);
          setState(() {
            myList.add(f);
          });
        }
        //  4- get already downloaded files
        String p = await FileManager.getPath();
        // List<String> savedFiles = await FileManager.listDirectory("$p/CoETEC");
        List<String> savedFiles = await FileManager.getAlreadyDownloaded();
        print("saved files are");
        print(savedFiles);
        // Iterate over the list of files and check if they are saved.
        for (int i = 0; i < myList.length; i++) {
          for (int j = 0; j < savedFiles.length; j++) {
            // String tmpy = "$p/CoETEC/${myList[i].fileName}";

            //  5- check if file is downloaded mark as saved
            if (myList[i].fileName == savedFiles[j]) {
              print("inside..${myList[i].fileName}");
              String x = await myDownloader.getMappedFile(myList[i].fileName);
              setState(() {
                myList[i].fileName =
                    x.replaceAll("/storage/emulated/0/CoETEC/", "");
                myList[i].saved = true;
                myList[i].originalFileName = x;
              });
            }
          }
        }
      }
    });
  }

  // Future<String> getpp() async {
  //   aa = await FileManager.getPath();
  //   return "path= $aa";
  // }

  @override
  void initState() {
    super.initState();
    // getpp();

    startTimer();
  }

  void baisicPopUp(Widget body, String title) {
    showDialog(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: Row(
              mainAxisAlignment: MainAxisAlignment.end,
              crossAxisAlignment: CrossAxisAlignment.end,
              children: [
                Text(
                  title,
                  style: const TextStyle(color: Colors.amberAccent),
                )
              ]),
          content: SingleChildScrollView(child: body),
          actions: <Widget>[
            ElevatedButton(
              onPressed: () {
                Navigator.of(context).pop(); // Close the dialog
              },
              child: const Text('الغاء'),
            ),
          ],
        );
      },
    );
  }

  /// this is just a list view has all files
  /// if file already saved it wont be downloaded again
  /// ,it has a delet button will perfom delete on the device
  Widget _storage() {
    return Center(
      child: ListView.builder(
        itemCount: myList.length,
        itemBuilder: (context, index) {
          return Column(
            children: [
              ListTile(
                title: Text(myList[index].fileName),
                trailing: Row(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    (myList[index].saved == true)
                        ? IconButton(
                            icon: const Icon(Icons.play_arrow_outlined),
                            onPressed: () async {
                              // open the video player
                              // String p = await FileManager.getPath();

                              // // String path =
                              // //     "$p/CoETEC/${myList[index].fileName}";
                              // String path = await myDownloader
                              //     .getMappedFile(myList[index].fileName);
                              // print(path);
                              try {
                                //                       baisicPopUp(
                                // Text("$p/CoETEC/${myList[index].fileName}"),
                                // "success");
                                // ignore: use_build_context_synchronously
                                Navigator.push(
                                  context,
                                  MaterialPageRoute(
                                      builder: (context) =>
                                          VideoPlayerWidget(videoUrl: myList[index].originalFileName)),
                                );
                              } catch (e) {
                                // baisicPopUp(Text(e.toString()), "failed");
                                // String path =
                                //     "$p/CoETEC/${myList[index].fileName}";
                              }
                            },
                          )
                        : IconButton(
                            icon: const Icon(Icons.download),
                            onPressed: () async {
                              myList[index].download = true;
                              String ff =
                                  myList[index].fileName.replaceAll(" ", "");
                              String url = "http://192.168.1.1/get?file=/$ff";
                              await myDownloader.downloadFile(
                                  url, myList[index].fileName, () {
                                setState(() {
                                  progrss = myDownloader.getProgress();
                                });
                              });
                              setState(() {
                                update = true;
                              });
                            },
                          ),
                    IconButton(
                      icon: const Icon(Icons.delete),
                      onPressed: () {
                        removeFile(myList[index].fileName);
                        removeFile(myList[index].fileName);
                        setState(() {
                          myList.removeAt(index);
                        });
                      },
                    ),
                  ],
                ),
              ),
              if (myList[index].download)
                LinearProgressIndicator(
                  value: progrss, // Set a value if you want to show progress
                ),
            ],
          );
        },
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.black,
        title: const Text('Storage '),
      ),
      body: (myList.isEmpty)
          ? const Center(
              child: CircularProgressIndicator(color: Colors.white),
            )
          : _storage(),
    );
  }
}
