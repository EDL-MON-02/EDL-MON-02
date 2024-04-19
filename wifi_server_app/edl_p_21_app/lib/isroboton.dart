import 'package:flutter/material.dart';

class RobotState extends ChangeNotifier {
  bool _isRobotOn = false;

  bool get isRobotOn => _isRobotOn;

  set isRobotOn(bool value) {
    _isRobotOn = value;
    notifyListeners(); // Notify listeners when the state changes
  }
}
