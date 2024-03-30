import 'package:flutter/material.dart';

class FWBTN extends StatelessWidget {
  final IconData icon;
  final String text;
  final Function onPressed;
  final double size;

  FWBTN({required this.icon, required this.text,required this.size, required this.onPressed});

  @override
  Widget build(BuildContext context) {
    return Container(
      width: double.infinity,
      height: 30,
      margin:const EdgeInsets.symmetric(horizontal: 20.0),
      child: ElevatedButton(
        onPressed: () {
          //print("hhh");
          onPressed();
        },
        style: ElevatedButton.styleFrom(
          primary: Colors.black, // Set background color
        ),
        child: Row(
          mainAxisAlignment: MainAxisAlignment.start,
          children: [
            Align(
              alignment: Alignment.centerLeft,
              child: Icon(
                icon,
                size: size, // Increase the icon size as desired
              ),
            ),
            SizedBox(width: 5),
            Text(
              text,
              style: TextStyle(
                fontSize: size*0.8, // Increase the text size as desired
              ),
            ),
          ],
        ),
      ),
    );
  }
}
