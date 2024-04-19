// This code is for controlling a robot with an [R pi pico W] module, which maps its environment and controls a blower system.
// The robot uses ultrasonic sensors for mapping and a web server for remote control.

// Include necessary libraries
#include <math.h> 
#include <stdio.h> 
#include "pico/time.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "motor.h"
#include "pico/multicore.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Define constants
#define map_dist 30         // Threshold distance for mapping
#define obst_dist 28        // Threshold distance for obstacle detection
#define obst_dist1 12       // Threshold distance for obstacle detection
const float speed_of_sound = 0.0343; // Speed of sound in cm/us

// WIFI 
bool is_blower_on = false;      // Flag to indicate if the blower is on
bool is_mopping_on = 0;         // Flag to indicate if the mopping system is on
const char* ssid = "pratik_yabaji";    // WiFi SSID
const char* password = "12345678";      // WiFi password
ESP8266WebServer server(80);            // Create a web server object

float duration_left = 0;        // Duration of signal received from left sensor
float duration_center = 0;      // Duration of signal received from center sensor
float duration_center1 = 0;     // Duration of signal received from center1 sensor
float duration_center2 = 0;     // Duration of signal received from center2 sensor
float duration_right = 0;       // Duration of signal received from right sensor


// Room Size
const int N=15, M=15;           // Size of the room
char room[N][M];                // Matrix representing the room

// Unit of room can be: unkwown, path or obstacle
const char u = 'u'; // unknown: not explored yet
const char p = 'p'; // path: free to explore
const char o = 'o'; // obstacle: robot cannot go there

// Direction for robot backracing 
const char n='n', e='e', s='s', w='w'; //north, south, east, west: tells the direction from which Robot visits a new unit of surface
char orientation[4] = {'f','r','d','l'};
// Robot initial position
int x = 13; 
int y = 2; 


// Sensors
void sensor_init(int echo_pin, int triger_pin);
void triger();
void measure_left();
void measure_right();
void measure_center();
void measure_center1();
void measure_center2();

char detect_obstacle(char select_sensor);

// Blower
void pump_init();
void blower_init();
void on_blower();
void off_blower();

void start_mapping();
void map_surrounding();
int is_room_completly_mapped();
bool is_surrounding_mapped = false;

void PrintMap();

// Handle Server Request
void dc() {
  String url = server.uri(); // Get the requested URL
  Serial.print("Requested URL: ");
  Serial.println(url);
  server.send(200, "text/html", "<html><body><h1>Hello from ESP8266!</h1></body></html>");
  is_blower_on = true;
  gpio_put(12, 1);
  gpio_put(22, 1);
  gpio_put(15, 0);
  analogWrite(ENA, motor_speed);
  analogWrite(ENB, motor_speed);
}

void wc() {
  String url = server.uri(); // Get the requested URL
  Serial.print("Requested URL: ");
  Serial.println(url);
  server.send(200, "text/html", "<html><body><h1>Hello from ESP8266!</h1></body></html>");
  is_blower_on = true;
  gpio_put(12, 0);
  gpio_put(22, 0);
  gpio_put(15, 1);
  analogWrite(ENA, motor_speed);
  analogWrite(ENB, motor_speed);
}
void dwc() {
  String url = server.uri(); // Get the requested URL
  Serial.print("Requested URL: ");
  Serial.println(url);
  server.send(200, "text/html", "<html><body><h1>Hello from ESP8266!</h1></body></html>");
  is_blower_on = true;
  gpio_put(12, 1);
  gpio_put(22, 1);
  gpio_put(15, 1);
  analogWrite(ENA, motor_speed);
  analogWrite(ENB, motor_speed);
}
void stop_completly() {
  String url = server.uri(); // Get the requested URL
  Serial.print("Requested URL: ");
  Serial.println(url);
  server.send(200, "text/html", "<html><body><h1>Hello from ESP8266!</h1></body></html>");
  is_blower_on = true;
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  gpio_put(12, 0);
  gpio_put(22, 0);
  gpio_put(15, 0);
}

// Make use of 2nd core to handle the wifi server
void core1_entry(){

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/dc", HTTP_GET, dc);
  server.on("/wc", HTTP_GET, wc);
  server.on("/dwc", HTTP_GET, dwc);
  server.on("/stop", HTTP_GET, stop_completly);

  server.begin();

  while(1){
    server.handleClient();
  }
}


void setup(){

  // Setup the WiFi server
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/dc", HTTP_GET, dc);
  server.on("/wc", HTTP_GET, wc);
  server.on("/dwc", HTTP_GET, dwc);
  server.on("/stop", HTTP_GET, stop_completly);
  server.begin();

  // Initialize Room matrix: initially all unit of surfaces are unknown
  Serial.begin(115200);
  for (int i=0; i<N; i++)
  {
    for (int j=0; j<M; j++)
    {
    room[i][j]=u;
    }
  }

  // while robot initial position is path
  room[x][y] = p;
  //Print map once before starting of exploration
  PrintMap();
  
  // Ultrasonic Sensors - Left, Center, Right
  sensor_init(echo_pin_left, trig_pin_left);
  sensor_init(echo_pin_center, trig_pin_center);
  sensor_init(echo_pin_center1, trig_pin_center1);
  sensor_init(echo_pin_center2, trig_pin_center2);
  sensor_init(echo_pin_right, trig_pin_right);

  // Motors - Left A, Right B
  motor_init(IN1, IN2, ENA, C1A, C2A);
  motor_init(IN3, IN4, ENB, C1B, C2B);

  // Blower
  blower_init();
  pump_init();

  // Attach interrupts for motor encoders
  attachInterrupt(digitalPinToInterrupt(C1A), update_encoder_left, CHANGE);
  attachInterrupt(digitalPinToInterrupt(C2A), update_encoder_left, CHANGE);
  attachInterrupt(digitalPinToInterrupt(C1B), update_encoder_right, CHANGE);
  attachInterrupt(digitalPinToInterrupt(C2B), update_encoder_right, CHANGE);


  // multicore_launch_core1(core1_entry);

  // Trigger ultrasonic sensors and measure distances
  triger_center();
  distance_center = pulseIn(echo_pin_center, HIGH)*0.034/2;
  triger_left();
  distance_left = pulseIn(echo_pin_left, HIGH)*0.034/2;
  triger_right();
  distance_right = pulseIn(echo_pin_right, HIGH)*0.034/2;
  triger_center1();
  distance_center1 = pulseIn(echo_pin_center1, HIGH)*0.034/2;
  triger_center2();
  distance_center2 = pulseIn(echo_pin_center2, HIGH)*0.034/2;

}

void loop(){
  server.handleClient();
  
  if(is_blower_on){
    // if ronot is out of grid
    if(x == N  || y == M || x == -1 || y == -1 ){
      gpio_put(12, 0);
      gpio_put(22, 0);
      gpio_put(15, 0);
      Serial.print("Robot out of matrix!");
      PrintMap();
      stop();
    }
    else{
      if(is_room_completly_mapped() != 1){
        // if room is not completly mapped start mapping
          is_surrounding_mapped = 0;
          map_surrounding();
          if(is_surrounding_mapped == 1){
            PrintMap();
            start_mapping();
          }
      }
      else{
        // stop if room is completly mapped
        stop();
        while(1){
          PrintMap();
        }
      }
    }
  }
  
}

// Blower
void blower_init(){
    gpio_init(blower_signal);
    gpio_set_dir(blower_signal, GPIO_OUT);
    gpio_put(blower_signal, 0);
    gpio_init(22);
    gpio_set_dir(22, GPIO_OUT);
    gpio_put(22, 0);
}
void on_blower(){
    gpio_put(blower_signal, 1);
}
void off_blower(){
    gpio_put(blower_signal, 0);
}

void pump_init(){
    gpio_init(pump_signal);
    gpio_set_dir(pump_signal, GPIO_OUT);
    gpio_put(pump_signal, 0);
}

int is_room_completly_mapped(){
  // check the room is completly mapped
    for(int i=1; i<N; i++)
    {
      for(int j=1; j<M; j++)
      {
        if (room[i][j] == u) 
        {
          if ( (room[i-1][j] == p) || (room[i-1][j] == n) || (room[i-1][j] == e) || (room[i-1][j] == s) || (room[i-1][j] == w ) )//North
          {return 0;}
          
          else if ( (room[i][j+1] == p) || (room[i][j+1] == n) || (room[i][j+1] == e) || (room[i][j+1] == s) || (room[i][j+1] == w) ) //East
          {return 0;}
          
          else if ( (room[i+1][j] == p) || (room[i+1][j] == n) || (room[i+1][j] == e) || (room[i+1][j] == s) || (room[i+1][j] == w) ) //South
          {return 0;}

          else if ( (room[i][j-1] == p) || (room[i][j-1] == n) || (room[i][j-1] == e) || (room[i][j-1] == s) || (room[i][j-1] == w) ) //West
          {return 0;}
        }
        else if(room[i][j] == p){
          return 0;
        }
      }
    }
    return 1;
  }

void map_surrounding(){
  // start mapping by detecting the obstacles
  Serial.println("MAP Surrounding started ......");
  room[x-1][y] = detect_obstacle(orientation[0], x-1,y);
  room[x][y+1] = detect_obstacle(orientation[1], x, y+1);
  room[x+1][y] = detect_obstacle(orientation[2], x+1,y);                   
  room[x][y-1] = detect_obstacle(orientation[3], x, y-1);                  
  is_surrounding_mapped = 1;
  Serial.println("MAP Surrounding done ......");

}

char detect_obstacle(char select_sensor, int x0, int y0){
  // triger the ultrasonic sensors and measure the distance
  triger_center();
  distance_center = pulseIn(echo_pin_center, HIGH)*0.034/2;
  triger_left();
  distance_left = pulseIn(echo_pin_left, HIGH)*0.034/2;
  triger_right();
  distance_right = pulseIn(echo_pin_right, HIGH)*0.034/2;
  triger_center1();
  distance_center1 = pulseIn(echo_pin_center1, HIGH)*0.034/2;
  triger_center2();
  distance_center2 = pulseIn(echo_pin_center2, HIGH)*0.034/2;
  if(x0 == N - 1 || y0 == M-1 || x0 == 0 || y0 == 0){
    return 'o';
  }
  else if (room[x0][y0] == u){
    if(select_sensor == 'f')
    {
      if((distance_center < obst_dist || distance_center1 < obst_dist1 || distance_center2 < obst_dist1)){
        return 'o';
      }
      else{
        return 'p';
      }

    }
    else if(select_sensor == 'r'){
      if(distance_right < obst_dist || distance_center2 < obst_dist1){
        return 'o';
      }
      else{
        return 'p';
      }
    }
    else if(select_sensor == 'l'){
      if(distance_left < obst_dist || distance_center1 < obst_dist1){
        return 'o';
      }
      else{
        return 'p';
      }
    }
    return room[x0][y0];
  }
  else{
    if(select_sensor == 'f')
    {
      if((distance_center < obst_dist || distance_center1 < obst_dist1 || distance_center2 < obst_dist1)){
        return 'o';
      }
    }
    else if(select_sensor == 'r'){
      if(distance_right < obst_dist || distance_center2 < obst_dist1){
        return 'o';
      }
    }
    else if(select_sensor == 'l'){
      if(distance_left < obst_dist || distance_center1 < obst_dist1){
        return 'o';
      }
    }
    return room[x0][y0];
  }
}

void start_mapping(){
  Serial.println("Mapping Started .....");
  // Move the robot according to the obstacle and update the robot coordinates
  if(room[x-1][y] == p){
    room[x-1][y] = n;
    x--;
    Serial.println("North");
    MoveNorth();
  }
  else if (room[x][y+1] == p)
  {
    room[x][y+1] = e;
    
    y++;
    Serial.println("East");
    MoveEast();
  }
  else if (room[x+1][y] == p)
  {
    room[x+1][y] = s;
    
    x++;
    Serial.println("South");
    MoveSouth();
  }
  else if (room[x][y-1] == p)
  {
    room[x][y-1] = w;

    y--;
    Serial.println("West");
    MoveWest();
  }
  else if((room[x-1][y] != p) && (room[x][y+1] != p) && (room[x+1][y] != p) && (room[x][y-1] != p)){
    if (room[x][y] == n)
    {
      x++; 
      MoveSouth(); 
    }
    
    else if (room[x][y] == e)
    {
      y--;
      MoveWest(); 
    }
    
    else if (room[x][y] == s)
    {
      x--; 
      MoveNorth(); 
    }
    
    else if (room[x][y] == w)
    {
      y++; 
      MoveEast(); 
    }
  }
}

void MoveNorth()
{
  turn(orientation[0]);
}

void MoveEast()
{
  turn(orientation[1]);
}

void MoveSouth()
{
  turn(orientation[2]);
}

void MoveWest()
{
  turn(orientation[3]);
}

void GO(){
  Serial.print("GO()");
  forward_x(1);
}
void STOP(){
  Serial.print("STOP()");
  stop();
}
void TURN_RIGHT(){
  Serial.print("TURN_RIGHT()");
  stop();
  turn_right();
  stop();
}
void TURN_LEFT(){
  Serial.print("TURN_LEFT()");
  stop();
  turn_left();
  stop();
  
}
void rotate_to_left() //left rotation 
{
  char temp = orientation[0];
  orientation[0] = orientation[1];
  orientation[1] = orientation[2];
  orientation[2] = orientation[3];
  orientation[3] = temp;
}

void rotate_to_right() //right rotation
{
  char temp = orientation[3];
  orientation[3] = orientation[2]; 
  orientation[2] = orientation[1]; 
  orientation[1] = orientation[0]; 
  orientation[0] = temp;
}

void turn(char rotation_value)
{
  if (rotation_value == 'f')  // go forward
  {
    GO();
  }
  
  else if (rotation_value == 'r') //turn right
  {
    TURN_RIGHT();
    GO();
    rotate_to_right(); 
  }
  
  else if (rotation_value == 'd') //turn backwards 180 degree
  {
    TURN_RIGHT();
    //STOP();
    TURN_RIGHT();
    while(abs(no_of_pulses_left) < 2800*3*0.40){
      right();
    }
    GO();
    rotate_to_right(); 
    rotate_to_right(); 
  }
  
  else if (rotation_value == 'l')  //turn left
  {
    TURN_LEFT();
    GO();
    rotate_to_left(); 
  }
}

// Sensor
void sensor_init(int echo_pin, int triger_pin){
    gpio_init(echo_pin);
    gpio_set_dir(echo_pin, GPIO_IN);
    gpio_init(triger_pin);
    gpio_set_dir(triger_pin, GPIO_OUT);
    gpio_put(triger_pin, 0);
}

// triger function for tigering the sensors
void triger_center(){
    gpio_put(trig_pin_center, 0);
    sleep_us(2);
    gpio_put(trig_pin_center, 1);
    sleep_us(10);
    gpio_put(trig_pin_center, 0);
}
void triger_center1(){
    gpio_put(trig_pin_center1, 0);
    sleep_us(2);
    gpio_put(trig_pin_center1, 1);
    sleep_us(10);
    gpio_put(trig_pin_center1, 0);
}
void triger_center2(){
    gpio_put(trig_pin_center2, 0);
    sleep_us(2);
    gpio_put(trig_pin_center2, 1);
    sleep_us(10);
    gpio_put(trig_pin_center2, 0);
}

void triger_right(){
    gpio_put(trig_pin_right, 0);
    sleep_us(2);
    gpio_put(trig_pin_right, 1);
    sleep_us(10);
    gpio_put(trig_pin_right, 0);
}

void triger_left(){
    gpio_put(trig_pin_left, 0);
    sleep_us(2);
    gpio_put(trig_pin_left, 1);
    sleep_us(10);
    gpio_put(trig_pin_left, 0);
}

// Functions to measure the distance and update 
void measure_left() {
  static unsigned long startTime = 0;
  if (digitalRead(echo_pin_left) == HIGH) {
    startTime = micros(); // Record the start time of the pulse
  } else {
    duration_left = micros() - startTime; // Calculate the pulse duration
    distance_left = (duration_left * speed_of_sound)/2;
  }
  Serial.print("distance_left:");
  Serial.println(distance_left);
}

void measure_center() {
  static unsigned long startTime = 0;
  if (digitalRead(echo_pin_center) == HIGH) {
    startTime = micros(); // Record the start time of the pulse
  } else {
    duration_center = micros() - startTime; // Calculate the pulse duration
    distance_center = (duration_center * speed_of_sound) /2;
  }
  Serial.print("distance_center:");
  Serial.println(distance_center);
}
void measure_center1() {
  static unsigned long startTime = 0;
  if (digitalRead(echo_pin_center1) == HIGH) {
    startTime = micros(); // Record the start time of the pulse
  } else {
    duration_center1 = micros() - startTime; // Calculate the pulse duration
    distance_center1 = (duration_center1 * speed_of_sound) ;
  }
}
void measure_center2() {
  static unsigned long startTime = 0;
  if (digitalRead(echo_pin_center2) == HIGH) {
    startTime = micros(); // Record the start time of the pulse
  } else {
    duration_center2 = micros() - startTime; // Calculate the pulse duration
    distance_center2 = (duration_center2 * speed_of_sound) ;
  }
}

void measure_right() {
  static unsigned long startTime = 0;
  if (digitalRead(echo_pin_right) == HIGH) {
    startTime = micros(); // Record the start time of the pulse
  } else {
    duration_right = micros() - startTime; // Calculate the pulse duration
    distance_right = (duration_right * speed_of_sound)/2;
  }
  Serial.print("distance_right:");
  Serial.println(distance_right);
}

// function to print the complete map
void PrintMap()
{   
  Serial.println("Map of explored room:");
  //Serial.println("");
  for (int i=0; i<N; i++)
  {
    Serial.println("");
    for (int j=0; j<M; j++)
    {
      Serial.print(room[i][j]);
      Serial.print(" ");
    }
  }
   Serial.println("");
}

void move()
{
  if (room[x-1][y] == p)
  {
    room[x-1][y] = n;
    
    x--; 
    MoveNorth();
  }
  
  else if (room[x][y+1] == p)
  {
    room[x][y+1] = e;
    
    y++;
    MoveEast();
  }

  else if (room[x+1][y] == p)
  {
    room[x+1][y] = s;
    
    x++;
    MoveSouth();
  }
  
  else if (room[x][y-1] == p)
  {
    room[x][y-1] = w;

    y--;
    MoveWest();
  }
}