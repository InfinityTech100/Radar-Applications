import 'dart:async';
import 'dart:io';
import 'dart:typed_data';
import 'package:flutter_ffmpeg/flutter_ffmpeg.dart';
import 'package:urad2/utils/unixTimeStamp.dart';
import '../utils/StorageManager.dart';

class VideoConverter {
  String filePath;
  // List<String> video = [];
  // late Timer timer;
  // late Image currentFrame ;
  // int videoIdx = 0;
  VideoConverter({required this.filePath});

  Future<String> convertMP4() async {
    await getFrames();
    await saveVideo();
    await deleteExtractedDir();
    if ((filePath.contains('ACVID_') == true)) {
      return filePath
          .replaceAll("itechVid", "mp4")
          .replaceAll("/.downloads", "/accidents");
    }
    return filePath
        .replaceAll("itechVid", "mp4")
        .replaceAll("/.downloads", "/idle");

    // return filePath.replaceAll("itechVid", "mp4").replaceAll("/.downloads", "");
  }

  Future<String> convertMP4_2(String newfile) async {
    print("############### Video Converter ###############");
    print("source: " + newfile);
    await getFrames();
    String fff =
        "video_${UTC.ftomUTC(int.parse(newfile.replaceAll(".itechVid", "").replaceAll("ACVID_", "").replaceAll("VID_", "")))}.mp4";
    print("dist:" + fff);
    await saveVideo_2(newfile, fff);
    await deleteExtractedDir();
    if ((filePath.contains('ACVID_') == true)) {
      return filePath
          .replaceAll("/.downloads", "/accidents")
          .replaceAll(newfile, fff);
    }
    return filePath.replaceAll("/.downloads", "/idle").replaceAll(newfile, fff);
  }

  String convertToFourDigitString(int number) {
    return number.toString().padLeft(4, '0');
  }

//VID_1700911698298
  Future deleteExtractedDir() async {
    late Directory appDocDir;
    List<int> t = [];
    try {
      String dir = "${await FileManager.getPath()}/ext";
      appDocDir = Directory(dir);
      try {
        await appDocDir.delete(recursive: true);
        print("deleted ext/ directory succesfuly.....");
      } catch (e) {}
      await appDocDir.create(recursive: true);
      print("directory created at ${appDocDir.path}");
    } catch (e) {
      print("failed to create directory");
      return;
    }
  }

  /// extract the video images
  Future<void> splitUint8List(Uint8List data, String delimiter) async {
    late Directory appDocDir;
    List<int> t = [];
    try {
      String dir = "${await FileManager.getPath()}/ext";
      appDocDir = Directory(dir);
      try {
        await appDocDir.delete(recursive: true);
      } catch (e) {}
      await appDocDir.create(recursive: true);
      print("directory created at ${appDocDir.path}");
    } catch (e) {
      print("failed to create directory");
      return;
    }
    int ctr = 0;
    for (int i = 0; i < (data.length); i++) {
      if (i <= (data.length - 9)) {
        if ((data[i] == delimiter.codeUnitAt(0)) &&
            (data[i + 1] == delimiter.codeUnitAt(1)) &&
            (data[i + 2] == delimiter.codeUnitAt(2)) &&
            (data[i + 3] == delimiter.codeUnitAt(3)) &&
            (data[i + 4] == delimiter.codeUnitAt(4)) &&
            (data[i + 5] == delimiter.codeUnitAt(5)) &&
            (data[i + 6] == delimiter.codeUnitAt(6)) &&
            (data[i + 7] == delimiter.codeUnitAt(7)) &&
            (data[i + 8] == delimiter.codeUnitAt(8))) {
          // print("match NextFrame");
          i = i + 8;
          String ff =
              "${appDocDir.path}/extracted_${convertToFourDigitString(ctr)}.jpg";
          ctr++;
          // video.add(ff);
          // print(ff);
          try {
            File mf = File(ff);
            RandomAccessFile raf = await mf.open(mode: FileMode.write);
            mf.writeAsBytes(t);
            await raf.close();
            // print(ff);
          } catch (e) {
            print("failed to create file ${e.toString()}");
          }
          t.clear();
        } else {
          t.add(data[i]);
        }
      }
    }
  }

  /// this will return the video frames
  Future<void> getFrames() async {
    try {
      File vidfle = File(filePath);
      Uint8List buffer = await vidfle.readAsBytes();
      await splitUint8List(buffer, "NextFrame");
    } catch (e) {}
  }

  Future<void> saveVideo() async {
    String dir = "${await FileManager.getPath()}/ext";
    final FlutterFFmpeg flutterFFmpeg = FlutterFFmpeg();
    String cmd;
    if ((filePath.contains('ACVID_') == true)) {
      cmd =
      "-framerate 10 -i '$dir/extracted_%04d.jpg' ${filePath.replaceAll(
          "itechVid", "mp4").replaceAll("/.downloads", "/accidents")}";
    }else{
      cmd =
      "-framerate 10 -i '$dir/extracted_%04d.jpg' ${filePath.replaceAll(
          "itechVid", "mp4").replaceAll("/.downloads", "/idle")}";
    }

    int res = await flutterFFmpeg.execute(cmd);

    if (res == 0) {
      print("Video conversion successful!");
      File myfile = File(filePath);
      await myfile.delete(recursive: true);
    } else {
      print("Video conversion failed with result code: $res");
    }
  }

  Future<void> saveVideo_2(String fname, String fnameUtc) async {
    String dir = "${await FileManager.getPath()}/ext";
    final FlutterFFmpeg flutterFFmpeg = FlutterFFmpeg();
    String cmd = '';
    if ((fname.contains('ACVID_') == true)) {
      cmd =
          "-framerate 10 -i '$dir/extracted_%04d.jpg' ${filePath.replaceAll("/.downloads", "/accidents").replaceAll(fname, fnameUtc)}";
    } else {
      cmd =
          "-framerate 10 -i '$dir/extracted_%04d.jpg' ${filePath.replaceAll("/.downloads", "/idle").replaceAll(fname, fnameUtc)}";
    }
    int res = await flutterFFmpeg.execute(cmd);

    if (res == 0) {
      print("Video conversion successful!");
      File myfile = File(filePath);
      await myfile.delete(recursive: true);
    } else {
      print("Video conversion failed with result code: $res");
    }
  }
}
