import 'dart:io';

import 'package:path_provider/path_provider.dart';

/**
 * will read directory files
 */
class FileManager {
  static String m_pth = '';
  static Future<String> getPath() async {
    //return /android/packagename/files
    // List<Directory>? externalStorageDirectories =
    //     await getExternalStorageDirectories();
    // for (Directory directory in externalStorageDirectories!) {
    //   m_pth = directory.path.toString();
    //   return directory.path.toString();
    // }

    return "/storage/emulated/0";
  }

  static List<String> listDirectory(String pat) {
    try {
      List<FileSystemEntity> files = Directory(pat).listSync();
      List<String> fileNames = [];
      for (FileSystemEntity file in files) {
        if ((file.path.contains('.downloads') == true )||
            (file.path.contains('itechVid') == true) ||
            (file.path.contains('.txt') == true)) {
          print("elemenated::${file.path}");
          continue;
        }
       print("passed::${file.path}");

        fileNames.add(file.path);
      }

      return fileNames;
    } catch (e) {
      // directory doesnt exist

      Directory dir = Directory(pat);
    }
    return [];
  }

  static Future<List<String>> getAlreadyDownloaded() async {
    try {
      String p = "${await FileManager.getPath()}/CoETEC/saved.txt";
      File savedLogFile = File(p);
      RandomAccessFile rrr = await savedLogFile.open(mode: FileMode.read);
      String files = await savedLogFile.readAsString();
      rrr.close();
      return files.split(',');
    } catch (e) {
      print("file not exist..........");
    }
    return [];
  }
}
