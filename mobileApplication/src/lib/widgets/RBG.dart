import 'package:flutter/material.dart';


class RadioButtonGroup extends StatefulWidget {
  final List<String> labels;
  final Function(String) onChanged;
  final String selected;

  const RadioButtonGroup({
    required this.labels,
    required this.onChanged,
    required this.selected,
  });

  @override
  State<RadioButtonGroup> createState() => _RadioButtonGroupState();
}

class _RadioButtonGroupState extends State<RadioButtonGroup> {
  int _selectedRadioButtonIndex = -1;

  @override
  void initState() {
    super.initState();
    _selectedRadioButtonIndex = widget.labels.indexOf(widget.selected);
  }

  @override
  Widget build(BuildContext context) {
    return Row(
      children: widget.labels
          .map((label) => Radio<String>(
        value: label,
        groupValue: _selectedRadioButtonIndex.toString(),
        onChanged: (value) {
          setState(() {
            _selectedRadioButtonIndex = widget.labels.indexOf(value!);
          });
          widget.onChanged(value!);
        },
      ))
          .toList(),
    );
  }
}