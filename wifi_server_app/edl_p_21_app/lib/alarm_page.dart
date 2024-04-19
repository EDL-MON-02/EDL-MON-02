// please change the server url

import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;

class AlarmPage extends StatefulWidget {
  @override
  _AlarmPageState createState() => _AlarmPageState();
}

class _AlarmPageState extends State<AlarmPage> {
  TimeOfDay? _selectedTime;

  Future<void> _selectTime(BuildContext context) async {
    final TimeOfDay? picked = await showTimePicker(
      context: context,
      initialTime: _selectedTime ?? TimeOfDay.now(),
    );
    if (picked != null && picked != _selectedTime) {
      setState(() {
        _selectedTime = picked;
      });
      _sendRequest(picked);
    }
  }

  Future<void> _sendRequest(TimeOfDay? time) async {
    String url = 'http://192.168.85.180:5000/alarm_control?';
    url += "HH="+time!.hour.toString()+"&MM="+time!.minute.toString();
    if (_selectedTime != null) {
      final response = await http.get(Uri.parse(url));
      if (response.statusCode == 200) {
        // Request successful, handle response data if needed
        print('GET request successful');
      } else {
        // Request failed
        print('Failed to send GET request');
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Alarm Page'),
      ),
      body: Center(
        child: _selectedTime != null
            ? Column(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  Text(
                    'Selected Time:',
                    style: TextStyle(fontSize: 20),
                  ),
                  SizedBox(height: 10),
                  Text(
                    '${_selectedTime!.format(context)}',
                    style: TextStyle(fontSize: 24, fontWeight: FontWeight.bold),
                  ),
                ],
              )
            : Text(
                'Select Alarm',
                style: TextStyle(fontSize: 24),
              ),
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: () {
          _selectTime(context);
        },
        child: Icon(Icons.add_alarm, color: Colors.white),
        backgroundColor: Colors.black,
      ),
      floatingActionButtonLocation: FloatingActionButtonLocation.endFloat,
    );
  }
}
