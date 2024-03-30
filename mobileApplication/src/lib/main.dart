///
/// @file      main.dart
/// @breif     this project is CoETEC_uRad_v2 mobile application
/// @author    Nader Hany (naderhany638@gmail.com)  
/// @version   2.72
/// @date      23-7-2023
/// 
/// 
import 'pages/SplashPage.dart';
import 'pages/homePage.dart';
import 'package:flutter/material.dart';
import 'dart:async';
import 'utils/StorageManager.dart';
import 'package:permission_handler/permission_handler.dart';

void main() async{
  WidgetsFlutterBinding.ensureInitialized(); // Initialize the Flutter binding
  PermissionStatus st =await Permission.storage.request();



  runApp(const MyApp());
}

/// this is the initial screen
/// show splash for 4 seconds and move to home page
class MyApp extends StatelessWidget {
  const MyApp({super.key});
  // This widget is the root of your application.

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
      home: const MyHomePage(title: 'Flutter Demo Home Page'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});

  final String title;
  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {

  var splash = SplashScreen('assets/images/logo.png');
  bool showsplash = true;

  @override
  void initState() {
    super.initState();
    startTimer();
  }

  void startTimer() {
    // will Display CoETEC logo for 4 seconds 
    Timer(Duration(seconds: 4), () {

      thread();
    });
  }

  @override
  Widget build(BuildContext context) {
    final Size screenSize = MediaQuery.of(context).size;
    final double screenWidth = screenSize.width;
    final double screenHeight = screenSize.height;

    return Scaffold(
        body:splash.SplashPage(),
    );
  }

  void thread() {
    setState(() {
      showsplash = false;
      Navigator.push(
        context,
        MaterialPageRoute(builder: (context) =>  HomePge()),
      );

    });

  }
}

