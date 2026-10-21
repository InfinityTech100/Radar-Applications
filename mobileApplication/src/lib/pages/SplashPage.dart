import 'package:flutter/cupertino.dart';

/// just a splash screen
class SplashScreen {
  String logo_img = "";

  SplashScreen(this.logo_img) {
    //logo_img = logo_img_path;
  }
  Widget SplashPage()
  {
    return Center(

      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children:  <Widget>[
          Image(
            width: 130,
            image:  AssetImage(this.logo_img),
            fit: BoxFit.cover,
          )
        ],
      ),
    );
  }
}
