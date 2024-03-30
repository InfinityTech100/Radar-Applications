/// this file contain shared resourses between all classes

bool radarMonitorEnable = true;

class file {
  String fileName;
  String originalFileName;
  bool download;
  bool saved;
  double d_progress = 0.0;
  file(
      {required this.fileName,
      required this.originalFileName,
      required this.download,
      required this.saved});
}
