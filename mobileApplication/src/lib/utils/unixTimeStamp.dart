

DateTime StartUpTime =DateTime.now();

class UTC {
  static String _twoDigits(int n) => n.toString().padLeft(2, '0');

  static String ftomUTC(int timestamp) {
    DateTime aaa = DateTime.fromMillisecondsSinceEpoch(timestamp);

    String formattedTimestamp =
        '${aaa.year}_${_twoDigits(aaa.month)}_${_twoDigits(aaa.day)}__${_twoDigits(aaa.hour)}:${_twoDigits(aaa.minute)}:${_twoDigits(aaa.second)}';
    return formattedTimestamp;
  }

  static int now() {
    DateTime dateTimeUTC = DateTime.now().toUtc();
    DateTime timeZoneOffset = DateTime.now();
    double toffset=timeZoneOffset.timeZoneOffset.inHours.toDouble();
//      print(timeZoneOffset.timeZoneOffset.inHours.toDouble());
    return dateTimeUTC.millisecondsSinceEpoch+(3600*1000*toffset).toInt();
  }
}
