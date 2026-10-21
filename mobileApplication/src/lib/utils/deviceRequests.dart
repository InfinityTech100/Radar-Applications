import 'package:http/http.dart' as http;
import 'package:urad2/utils/unixTimeStamp.dart';

class DeviceRequests {
  final base_url = 'http://192.168.1.1';

  Future<String> discoverFiles() async {
    try {
      final response = await http.get(Uri.parse('$base_url/list'));
      if (response.statusCode == 200) {
        // print(response.body);
        return response.body;
      } else {
        print("invalid request.....");
      }
    } catch (e) {
      print("failed to make http request");
    }
    return "";
  }

  Future<bool> setAutoRecordMode(bool enable) async {
    try {
      if (enable == true) {
        final response =
            await http.get(Uri.parse('$base_url/capture?status=on'));
        if (response.statusCode == 200) {
          print(response.body);
          return true;
        } else {
          print("invalid request.....");
        }
      } else {
        final response =
            await http.get(Uri.parse('$base_url/capture?status=off'));
        if (response.statusCode == 200) {
          // print(response.body);
          return true;
        } else {
          print("invalid request.....");
        }
      }
    } catch (e) {
      print("failed to make http request");
    }
    return false;
  }

  Future<bool> emmitAccident() async {
    try {
      final response = await http.get(Uri.parse('$base_url/accident'));
      if (response.statusCode == 200) {
        print(response.body);
        return true;
      } else {
        print("invalid request.....");
      }
    } catch (e) {
      print("failed to make http request");
    }
    return false;
  }

  Future<bool> setUnixTimeStamp() async {
    String now=(UTC.now()~/1000).toString();
    print(now);
    try {
      final response = await http.get(Uri.parse('$base_url/timestamp?time=$now'));
      if (response.statusCode == 200) {
        print(response.body);
        return true;
      } else {
        print("invalid request.....");
      }
    } catch (e) {
      print("initUTC()->failed to make http request");
    }
    return false;
  }
}
