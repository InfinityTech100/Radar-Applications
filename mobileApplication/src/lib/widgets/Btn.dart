import 'package:flutter/material.dart';

class BTN extends StatelessWidget {
  final IconData icon;
  final Function onPressed;
  final double size;
  late Color color;
  late Color accentColor;

  BTN(
      {required this.icon,
      required this.onPressed,
      this.color = Colors.black,
      this.accentColor=Colors.white,
      required this.size});

  @override
  Widget build(BuildContext context) {
    return Material(
      color: Colors.transparent,
      child: InkWell(
        onTap: () {
          onPressed();
        },
        child: Container(
          width: size * 2.5,
          height: size * 2.5,
          decoration: BoxDecoration(
            shape: BoxShape.circle,
            color: color,
            border: Border.all(
              color: accentColor,
              width: 1,
            ),
          ),
          child: Icon(
            icon,
            color: accentColor,
            size: size * 0.7,
          ),
        ),
      ),
    );
  }
}
