import 'dart:io';
import 'package:flutter/material.dart';
import 'package:dio/dio.dart';
import 'package:flutter/widgets.dart';
import 'package:permission_handler/permission_handler.dart';
import 'package:http/http.dart' as http;
import 'package:urad2/utils/StorageManager.dart';
import 'package:urad2/utils/unixTimeStamp.dart';
import 'package:urad2/utils/videoConverter.dart';

////todo: 1- on downloading get the progress (done)
///       2- on download finished convert the file if the extention is .itechVid
///       3- log the downloaded file on the downloaded files

/// use it to download files
class FileDownloader {
  double progress = 0.0;
  bool status = false;
  String error = "";
  final String WorkSpaceDirectory;

  FileDownloader({required this.WorkSpaceDirectory});

  Future logSavedFile(String file) async {
    try {
      String p = "${await FileManager.getPath()}/$WorkSpaceDirectory/saved.txt";
      File savedLogFile = File(p);
      RandomAccessFile rrr = await savedLogFile.open(mode: FileMode.read);
      String t = await savedLogFile.readAsString();
      rrr.close();
      File savedLogFile2 = File(p);
      RandomAccessFile rrrr = await savedLogFile2.open(mode: FileMode.write);
      await savedLogFile2.writeAsString("$t$file,");
      rrrr.close();
    } catch (e) {
      print("file not exist..........");
      String p = "${await FileManager.getPath()}/$WorkSpaceDirectory/saved.txt";
      File savedLogFile2 = File(p);
      RandomAccessFile rrrr = await savedLogFile2.open(mode: FileMode.write);
      await savedLogFile2.writeAsString("$file,");
      rrrr.close();
    }
  }

  Future logMapFile(String file) async {
    try {
      String p = "${await FileManager.getPath()}/$WorkSpaceDirectory/map.txt";
      File savedLogFile = File(p);
      RandomAccessFile rrr = await savedLogFile.open(mode: FileMode.read);
      String t = await savedLogFile.readAsString();
      rrr.close();
      File savedLogFile2 = File(p);
      RandomAccessFile rrrr = await savedLogFile2.open(mode: FileMode.write);
      await savedLogFile2.writeAsString("$t$file,");
      rrrr.close();
    } catch (e) {
      print("file not exist..........");
      String p = "${await FileManager.getPath()}/$WorkSpaceDirectory/map.txt";
      File savedLogFile2 = File(p);
      RandomAccessFile rrrr = await savedLogFile2.open(mode: FileMode.write);
      await savedLogFile2.writeAsString("$file,");
      rrrr.close();
    }
  }

  Future<String> getMappedFile(String file) async {
    try {
      String p = "${await FileManager.getPath()}/$WorkSpaceDirectory/map.txt";
      File savedLogFile = File(p);
      RandomAccessFile rrr = await savedLogFile.open(mode: FileMode.read);
      String aa = await savedLogFile.readAsString();
      rrr.close();
      List<String> aaa = aa.split(',');
      for (String i in aaa) {
        if (i.contains(file) == true) {
          List<String> ii = i.split("##");
          print("mapped file is ");
          print(ii);
          return ii[1];
        }
      }
    } catch (e) {
      print("file not exist..........");
    }

    return "";
  }

  double getProgress() {
    return progress;
  }

  Future<void> downloadFile(
      String url, String fileName, Function callback) async {
    Dio dio = Dio();
    Uri uri = Uri.parse(url);
    String filePath = "";
    try {
      String p = await FileManager.getPath();
      String dir = '$p/$WorkSpaceDirectory/.downloads';
      String dir2 = '$p/$WorkSpaceDirectory/accidents';
      String dir3 = '$p/$WorkSpaceDirectory/idle';

      Directory appDocDir = Directory(dir);
      Directory appDocDir2 = Directory(dir2);
      Directory appDocDir3 = Directory(dir3);
      await appDocDir.create(recursive: true);
      await appDocDir2.create(recursive: true);
      await appDocDir3.create(recursive: true);
      filePath = '${appDocDir.path}/$fileName';

      await dio.download(url, filePath,
          onReceiveProgress: (receivedBytes, totalBytes) {
        if (totalBytes != -1) {
          double mProgress = (receivedBytes / totalBytes) * 100;
          progress = mProgress / 100;
          callback();
          print('Download progress: ${progress}%');
        }
      });
      await logSavedFile(fileName);
      status = true;
      progress = 0;
      print('File downloaded to: $filePath');
    } catch (e) {
      print('Error occurred during downloading: $e');
      error = 'Error occurred during downloading: $e';
      status = false;
    }
    if (status == true) {
      // convert the downloaded file on filepath if itechVid or copy it if .jpg
      // store ti to the working dir
      VideoConverter converter = VideoConverter(filePath: filePath);
       String outputFile = await converter.convertMP4();

      // String outputFile = await converter.convertMP4_2(fileName);
      await logMapFile('$fileName##$outputFile');
      print("file Converted to mp4 at :: $outputFile ");
    }
  }

  //   Future<void> downloadFile(
  //     String url, String fileName, Function callback) async {
  //   Dio dio = Dio();
  //   Uri uri = Uri.parse(url);

  //   try {
  //     String p = await FileManager.getPath();
  //     String dir = '$p/$WorkSpaceDirectory';
  //     Directory appDocDir = Directory(dir);
  //     String filePath = '${appDocDir.path}/$fileName';

  //     await dio.download(url, filePath,
  //         onReceiveProgress: (receivedBytes, totalBytes) {
  //       if (totalBytes != -1) {
  //         double mProgress = (receivedBytes / totalBytes) * 100;
  //         progress = mProgress / 100;
  //         callback();
  //         print('Download progress: ${progress}%');
  //       }
  //     });
  //     status = true;
  //     progress = 0;
  //     print('File downloaded to: $filePath');
  //   } catch (e) {
  //     print('Error occurred during downloading: $e');
  //     error = 'Error occurred during downloading: $e';
  //     status = false;
  //   }
  // }
}
