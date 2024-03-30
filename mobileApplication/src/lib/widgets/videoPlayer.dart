// import 'dart:io';
// import 'package:flutter/material.dart';
// import 'package:flutter_vlc_player/flutter_vlc_player.dart';

// class VideoPlayerWidget extends StatefulWidget {
//   final String videoUrl;

//   VideoPlayerWidget({required this.videoUrl});

//   @override
//   _VideoPlayerWidgetState createState() => _VideoPlayerWidgetState();
// }
// class _VideoPlayerWidgetState extends State<VideoPlayerWidget> {
//   late VlcPlayerController _vlcPlayerController;

//   @override
//   void initState() {
//     super.initState();
//     _initializePlayer();
//   }

//   @override
//   void dispose() {
//     _vlcPlayerController.dispose();
//     super.dispose();
//   }

//   /**
//    * will play video from internal storage
//    */
// Future<void> _initializePlayer() async {
//   try {
//     _vlcPlayerController = VlcPlayerController.file(File(widget.videoUrl));
//   } catch (e) {
//     print('Error initializing player: $e');
//   }
// }

//   @override
//   Widget build(BuildContext context) {
//     return Scaffold(
//       appBar: AppBar(
//         backgroundColor: Colors.black,
//         title: Text('Video Player'),
//       ),
//       body: Center(
//         child: VlcPlayer(
//           controller: _vlcPlayerController,
//           aspectRatio: 4 / 3, // Set the aspect ratio according to your video
//           placeholder:const Center(child: CircularProgressIndicator(color: Colors.white,)),
//         ),
//       ),
//     );
//   }
// }

/********************************************
 *  works fine but cant start avi files
 * ******************************************/
// import 'dart:async';
// import 'dart:io';

// import 'package:flutter/material.dart';
// import 'package:video_player/video_player.dart';

// class VideoPlayerApp extends StatelessWidget {
//   const VideoPlayerApp({super.key});

//   @override
//   Widget build(BuildContext context) {
//     return const MaterialApp(
//       title: 'Video Player Demo',
//       home: VideoPlayerScreen(),
//     );
//   }
// }

// class VideoPlayerScreen extends StatefulWidget {
//   const VideoPlayerScreen({super.key});

//   @override
//   State<VideoPlayerScreen> createState() => _VideoPlayerScreenState();
// }

import 'dart:async';
import 'dart:io';
import 'dart:typed_data';
import 'package:video_player/video_player.dart';
import 'package:flutter/material.dart';
import 'package:flutter_ffmpeg/flutter_ffmpeg.dart';
import '../utils/StorageManager.dart';

// Make sure to add following packages to pubspec.yaml:
// * media_kit
// * media_kit_video
// * media_kit_libs_video
// import 'package:media_kit/media_kit.dart'; // Provides [Player], [Media], [Playlist] etc.
// import 'package:media_kit_video/media_kit_video.dart'; // Provides [VideoController] & [Video] etc.

class VideoPlayerWidget extends StatefulWidget {
  final String videoUrl;
  VideoPlayerWidget({required this.videoUrl}) {
    WidgetsFlutterBinding.ensureInitialized();
  }

  @override
  _VideoPlayerScreenState createState() => _VideoPlayerScreenState();
}

class _VideoPlayerScreenState extends State<VideoPlayerWidget> {
  late VideoPlayerController _controller;
  List<String> video = [];
  late Timer timer;
  late Image currentFrame = Image.file(File(widget.videoUrl));
  int videoIdx = 0;

  String convertToFourDigitString(int number) {
    return number.toString().padLeft(4, '0');
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
      File vidfle = File(widget.videoUrl);
      Uint8List buffer = await vidfle.readAsBytes();
      await splitUint8List(buffer, "NextFrame");
    } catch (e) {}
  }

  Future<void> saveVideo() async {
    String dir = "${await FileManager.getPath()}/ext";
    final FlutterFFmpeg _flutterFFmpeg = FlutterFFmpeg();
    String cmd =
        "-framerate 10 -i '$dir/extracted_%04d.jpg' ${widget.videoUrl.replaceAll("itechVid", "mp4")}";
    int res = await _flutterFFmpeg.execute(cmd);
    
    if (res == 0) {
      print("Video conversion successful!");
      File myfile = File(widget.videoUrl);
      await myfile.delete(recursive: true);

      _controller = VideoPlayerController.file(
          File(widget.videoUrl.replaceAll("itechVid", "mp4")))
        ..initialize().then((_) {
          setState(() {});
        });
    } else {
      print("Video conversion failed with result code: $res");
    }
  }

  int stat = 0;
  @override
  void initState() {
    super.initState();

    if (widget.videoUrl.contains("jpg") == true) {
      setState(() {
        stat = 2;
      });
      return;
    }
    if (widget.videoUrl.contains("png") == true) {
      setState(() {
        stat = 2;
      });
      return;
    }
    if (widget.videoUrl.contains("JPG") == true) {
      setState(() {
        stat = 2;
      });
      return;
    }
    if (widget.videoUrl.contains("JPEG") == true) {
      setState(() {
        stat = 2;
      });
      return;
    }
    if (widget.videoUrl.contains("jpeg") == true) {
      setState(() {
        stat = 2;
      });
      return;
    }
    if (widget.videoUrl.contains("itechVid")) {
      print("object");
      getFrames();
      saveVideo();

      _controller = VideoPlayerController.file(
          File(widget.videoUrl.replaceAll("itechVid", "mp4")))
        ..initialize().then((_) {
          setState(() {
            Navigator.popUntil(context, ModalRoute.withName('/'));
          });
        });

      return;
    }

    _controller = VideoPlayerController.file(File(widget.videoUrl))
      ..initialize().then((_) {
        setState(() {});
      });
  }

  void startTimer() async {}

  @override
  void dispose() {
    super.dispose();
    _controller.dispose();
    // timer.cancel();
  }

  @override
  Widget build(BuildContext context) {
    return (stat == 0)
        ? Scaffold(
            appBar: AppBar(
              title: const Text(
                "media player",
                style: TextStyle(color: Colors.white),
              ),
            ),
            body: Center(
              child: _controller.value.isInitialized
                  ? AspectRatio(
                      aspectRatio: _controller.value.aspectRatio,
                      child: VideoPlayer(_controller),
                    )
                  : Container(),
            ),
            floatingActionButton: FloatingActionButton(
              backgroundColor: Colors.cyan,
              onPressed: () {
                setState(() {
                  _controller.value.isPlaying
                      ? _controller.pause()
                      : _controller.play();
                });
              },
              child: Icon(
                _controller.value.isPlaying ? Icons.pause : Icons.play_arrow,
              ),
            ),
          )
        : Scaffold(
            appBar: AppBar(
              title: const Text(
                "media player",
                style: TextStyle(color: Colors.white),
              ),
            ),
            body: Center(
              child: (stat == 2)
                  ? Image.file(File(widget.videoUrl))
                  : currentFrame,
            ),
            floatingActionButton: FloatingActionButton(
              backgroundColor: Colors.cyan,
              onPressed: () {
                setState(() {
                  setState(() {
                    stat = 3;
                  });
                });
              },
              child: Icon(Icons.abc),
            ),
          );
  }
}
