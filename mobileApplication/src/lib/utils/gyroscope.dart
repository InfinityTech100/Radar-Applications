import 'dart:math';

import 'package:sensors_plus/sensors_plus.dart';

class Gyro {
  double pitch = 0, roll = 0;
  DateTime prevTimestamp = DateTime.now();
  double roll_treashold = 0, pitch_treashold = 0;
  double origin_pitch = 0, origin_roll = 0;
  Gyro() {
    accelerometerEvents.listen((AccelerometerEvent event) {
      double ax = event.x;
      double ay = event.y;
      double az = event.z;

      // Calculate pitch and roll angles in radians
      // this formula is derieved by this url
      // https://www.nxp.com/docs/en/application-note/AN3461.pdf
      pitch = atan2(-ax, sqrt(ay * ay + az * az));
      roll = atan2(ay, az);

      // Convert angles to degrees
      pitch = pitch * 180 / pi;
      roll = roll * 180 / pi;

      // print('Pitch: $pitch degrees, Roll: $roll degrees');
    });
  }

  void setOrigin() {
    origin_pitch = pitch;
    origin_roll = roll;
  }

  void setTreasholds(double pitch_t, double roll_t) {
    roll_treashold = roll_t;
    pitch_treashold = pitch_t;
  }

  bool isAccident() {
    bool pitch_condition1 = (pitch >= (origin_pitch + pitch_treashold));
    bool pitch_condition2 = (pitch <= (origin_pitch - pitch_treashold));
    bool roll_condition1 = (roll >= (origin_roll + roll_treashold));
    bool roll_condition2 = (roll <= (origin_roll - roll_treashold));

    if (pitch_condition1 ||
        pitch_condition2 ||
        roll_condition1 ||
        roll_condition2) {
      print("accident occure...........");
      return true;
    }

    return false;
  }
}
