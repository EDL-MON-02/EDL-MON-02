// please change the server url

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:edl_p_21_app/isroboton.dart';
import 'package:edl_p_21_app/alarm_page.dart';
import 'package:http/http.dart' as http;

void main() {
  runApp(
    ChangeNotifierProvider(
      create: (context) => RobotState(),
      child: MyApp(),
    ),
  );
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'P-21 MON-02',
      theme: ThemeData(
        primaryColor: Colors.black, // Setting primary color to white
      ),
      home: MyHomePage(),
    );
  }
}

class MyHomePage extends StatefulWidget {
  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  int _selectedIndex = 0;

  final List<Widget> _widgetOptions = <Widget>[
    Text('Home Page', style: TextStyle(color: Colors.white)),
    // Text('Map Page', style: TextStyle(color: Colors.white)),
    Text('Alarm Page', style: TextStyle(color: Colors.white)),
  ];

  void _onItemTapped(int index) {
    setState(() {
      _selectedIndex = index;
    });
    if (index == 1) {
      // Check if Alarm Page is selected
      Navigator.push(
        context,
        MaterialPageRoute(builder: (context) => AlarmPage()),
      );
    }
  }

  Future<void> _showOptionsDialog(BuildContext context) async {
    await showDialog<void>(
      context: context,
      builder: (BuildContext context) {
        return OptionsDialog();
      },
    );
  }

  @override
  Widget build(BuildContext context) {
    bool isRobotOn = Provider.of<RobotState>(context).isRobotOn;
    return Scaffold(
      appBar: AppBar(
        title: Text(
          'P-21 MON-02',
          style: TextStyle(color: Colors.white), // Setting text color to white
        ),
        backgroundColor: Colors.black, // Setting app bar background color to black
        actions: <Widget>[
          IconButton(
            icon: Icon(Icons.help, color: Colors.white), // Help icon
            onPressed: () {
              // Add your help button action here
            },
          ),
        ],
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            CircleAvatar(
              radius: 120,
              backgroundImage: AssetImage(
                  'assets/images/robot.jpeg'), // Replace this with your image path
            ),
            SizedBox(height: 50), // Increased spacing between image and button
            Padding(
              padding: const EdgeInsets.only(bottom: 20), // Added padding only at the bottom
              child: ElevatedButton(
                onPressed: () {
                  _showOptionsDialog(context); // Show options dialog
                },
                style: ElevatedButton.styleFrom(
                  backgroundColor: Colors.white,
                  shadowColor: null,
                  shape: CircleBorder(), // Set button shape to circular
                  padding: EdgeInsets.all(40), // Adjust button size
                ),
                child: Icon(
                  isRobotOn ? Icons.lightbulb : Icons.lightbulb_outline,
                  color: Colors.black,
                  size: 50, // Set icon size
                ),
              ),
            ),
            _widgetOptions.elementAt(_selectedIndex),
          ],
        ),
      ),
      bottomNavigationBar: BottomNavigationBar(
        items: <BottomNavigationBarItem>[
          BottomNavigationBarItem(
            icon: Icon(Icons.home),
            label: 'Home',
          ),
          // BottomNavigationBarItem(
          //   icon: Icon(Icons.map),
          //   label: 'Mapping',
          // ),
          BottomNavigationBarItem(
            icon: Icon(Icons.alarm),
            label: 'Alarm',
          ),
        ],
        currentIndex: _selectedIndex,
        onTap: _onItemTapped,
        backgroundColor: Colors.white, // Setting background color to black
        selectedItemColor: Colors.black, // Setting selected item color to white
        unselectedItemColor: Colors.black.withOpacity(
            0.5), // Setting unselected item color to light grey
      ),
    );
  }
}

class OptionsDialog extends StatefulWidget {
  @override
  _OptionsDialogState createState() => _OptionsDialogState();
}

class _OptionsDialogState extends State<OptionsDialog> {
  bool option1 = false;
  bool option2 = false;
  bool option3 = false;
  bool option4 = false;

  Future<void> sendGetRequest(bool option1, bool option2, bool option3, bool option4) async {
    String url = 'http://192.168.85.180:5000/pico_control';
    Map<String, String> queryParams = {
      'dry': option1.toString(),
      'wet': option2.toString(),
      'dry_wet': option3.toString(),
      'stop': option4.toString(),
    };
    String queryString = Uri(queryParameters: queryParams).query;
    url += '?' + queryString;

    try {
      final response = await http.get(Uri.parse(url));
      if (response.statusCode == 200) {
        // Request successful
        // isRobotOn(true);
        print('GET request successful');
        print('Response body: ${response.body}');
      } else {
        // Request failed
        print('GET request failed with status code: ${response.statusCode}');
      }
    } catch (e) {
      print('Error sending GET request: $e');
    }
  }

  @override
  Widget build(BuildContext context) {
    return AlertDialog(
      title: Text('Select Options'),
      content: Column(
        mainAxisSize: MainAxisSize.min,
        children: <Widget>[
          CheckboxListTile(
            title: Text('Dry cleaning'),
            value: option1,
            onChanged: (value) {
              setState(() {
                option1 = value!;
              });
            },
          ),
          CheckboxListTile(
            title: Text('Wet cleaning'),
            value: option2,
            onChanged: (value) {
              setState(() {
                option2 = value!;
              });
            },
          ),
          CheckboxListTile(
            title: Text('Dry + Wet cleaning'),
            value: option3,
            onChanged: (value) {
              setState(() {
                option3 = value!;
              });
            },
          ),
          CheckboxListTile(
            title: Text('Power Off'),
            value: option4,
            onChanged: (value) {
              setState(() {
                option4 = value!;
              });
            },
          ),
        ],
      ),
      actions: <Widget>[
        TextButton(
          child: Text('OK'),
          onPressed: () async {
            Navigator.of(context).pop();
            // Handle OK button action
            await sendGetRequest(option1, option2, option3, option4);
          },
        ),
        TextButton(
          child: Text('Cancel'),
          onPressed: () {
            Navigator.of(context).pop();
          },
        ),
      ],
    );
  }
}
