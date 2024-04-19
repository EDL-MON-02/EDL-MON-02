
#include <math.h> 
#include <stdio.h> 
#include "pico_pins.h"
#include "pico/time.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "motor.h"

#define forward_dist 5
#define rotation_dist 5

void setup() {
  Serial.begin(115200);
  // Motors - Left A, Right B
  motor_init(IN1, IN2, ENA, C1A, C2A);
  motor_init(IN3, IN4, ENB, C1B, C2B);

  // Interupts
  attachInterrupt(digitalPinToInterrupt(C1A), update_encoder_left, CHANGE);
  attachInterrupt(digitalPinToInterrupt(C2A), update_encoder_left, CHANGE);
  attachInterrupt(digitalPinToInterrupt(C1B), update_encoder_right, CHANGE);
  attachInterrupt(digitalPinToInterrupt(C2B), update_encoder_right, CHANGE);

}

void loop() {
  Serial.println("forward_x() function called");
  turn_right();
  forward_x(5);
  turn_left();
  Serial.println("stop() function called");
  stop();
  while(1){
  }

}
