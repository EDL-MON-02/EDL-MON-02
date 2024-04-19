#include "pico_pins.h" // Include the header file for Pico pins
#define motor_speed 110 // Define motor speed
#define no_of_rotation_for_turn 1 // Define number of rotations for turn
#define k1 0.4 // Define constant k1
#define k2 0.4 // Define constant k2

// Variables for left encoder
int lastEncoded_left = 0;
long int no_of_pulses_left = 0;
int true_no_of_rotation_left = 0;

// Variables for right encoder
int lastEncoded_right = 0;
long int no_of_pulses_right = 0;
int true_no_of_rotation_right = 0;

int error_forward = 0;

// Variables for distance sensors
float distance_left = 0;
float distance_center = 0;
float distance_center1 = 0;
float distance_center2 = 0;
float distance_right = 0;

// Function prototypes
void turn_left();
void turn_right();
void motor_init(int in1, int in2, int en, int c1, int c2);
void forward();
void backward();
void right();
void left();
void stop();
void update_encoder_left();
void update_encoder_right();
void forward_x(int x);
void backward_x(int x);

// Function to initialize motor pins
void motor_init(int in1, int in2, int en, int c1, int c2){
    gpio_init(in1);
    gpio_set_dir(in1, GPIO_OUT);
    gpio_init(in2);
    gpio_set_dir(in2, GPIO_OUT);

    gpio_init(c1);
    gpio_pull_up(c1);
    gpio_init(c2);
    gpio_pull_up(c2);
    gpio_set_dir(c1, GPIO_IN);
    gpio_set_dir(c2, GPIO_IN);
    gpio_init(en);
    gpio_set_dir(en, GPIO_OUT);
    if(in1 == IN1){
        analogWrite(en, motor_speed);
    }
    else{
        analogWrite(en, motor_speed);
    }
}

// Function to move forward
void forward(){
    // Motor A
    gpio_put(IN1, 1);
    gpio_put(IN2, 0);

    // Motor B
    gpio_put(IN3, 1);
    gpio_put(IN4, 0);
}

// Function to move backward
void backward(){
    // Motor A
    gpio_put(IN1, 0);
    gpio_put(IN2, 1);

    // Motor B
    gpio_put(IN3, 0);
    gpio_put(IN4, 1);
}

// Function to turn right
void right(){
    // Motor A
    gpio_put(IN1, 1);
    gpio_put(IN2, 0);

    // Motor B
    gpio_put(IN3, 0);
    gpio_put(IN4, 1);
}

// Function to turn left
void left(){
    // Motor A
    gpio_put(IN1, 0);
    gpio_put(IN2, 1);

    // Motor B
    gpio_put(IN3, 1);
    gpio_put(IN4, 0);
}

// Function to stop motors
void stop(){
    // Motor A
    gpio_put(IN1, 0);
    gpio_put(IN2, 0);

    // Motor B
    gpio_put(IN3, 0);
    gpio_put(IN4, 0);
}

// Function to update left encoder
void update_encoder_left(){
    int MSB = gpio_get(C1A);
    int LSB = gpio_get(C2A);
    int encoded = (MSB << 1) |LSB; 
    int sum  = (lastEncoded_left << 2) | encoded; 

    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)no_of_pulses_left ++;
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)no_of_pulses_left ++;

    lastEncoded_left = encoded; // Store this value for next time

    if((no_of_pulses_left) > 2800*3)
    {
        no_of_pulses_left = 0;
        true_no_of_rotation_left += 1;
        Serial.print("no_of_rotation_left : ");
        Serial.println(true_no_of_rotation_left);
        Serial.println("\n");
    }
}

// Function to update right encoder
void update_encoder_right(){
    int MSB = gpio_get(C1B);
    int LSB = gpio_get(C2B);
    int encoded = (MSB << 1) |LSB; 
    int sum  = (lastEncoded_right << 2) | encoded; 

    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) no_of_pulses_right ++;
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) no_of_pulses_right ++;

    lastEncoded_right = encoded; // Store this value for next time

    if((no_of_pulses_right) > 2800*3)
    {
        no_of_pulses_right = 0;
        true_no_of_rotation_right += 1;
        Serial.print("no_of_rotation_right : ");
        Serial.println(true_no_of_rotation_right);
        Serial.println("\n");
    }
}

// Function to move forward by x rotations
void forward_x(int x){
    no_of_pulses_left = 0;
    no_of_pulses_right = 0;
    int t0_no_of_rotation_left = true_no_of_rotation_left;
    int t0_no_of_rotation_right = true_no_of_rotation_right;
    while((true_no_of_rotation_left - t0_no_of_rotation_left) < x && (true_no_of_rotation_right - t0_no_of_rotation_right) < x)
    { 
        error_forward = no_of_pulses_left - no_of_pulses_right;
        
        analogWrite(ENA,(motor_speed - k1*error_forward));
        analogWrite(ENB,(motor_speed + k2*error_forward + 10));
        Serial.println(true_no_of_rotation_left - t0_no_of_rotation_left);
        forward();
        
    }
    analogWrite(ENA,motor_speed );
    analogWrite(ENB,motor_speed );
}

// Function to move backward by x rotations
void backward_x(int x){
    int t0_no_of_rotation_left = true_no_of_rotation_left;
    int t0_no_of_rotation_right = true_no_of_rotation_right;
    while((true_no_of_rotation_left - t0_no_of_rotation_left) < x && (true_no_of_rotation_right - t0_no_of_rotation_right) < x)
    {
        backward();
    }
}

// Function to turn right
void turn_right(){
    int t0_no_of_rotation_left = true_no_of_rotation_left;
    int t0_no_of_rotation_right = true_no_of_rotation_right;
    no_of_pulses_right = 0;
    no_of_pulses_left = 0;
    while((true_no_of_rotation_left - t0_no_of_rotation_left) < no_of_rotation_for_turn)
    {
      Serial.print((true_no_of_rotation_left - t0_no_of_rotation_left));
      right();
    }
    while((no_of_pulses_left) < 2800*3*0.35){
      right();
    }
}

// Function to turn left
void turn_left(){
    int t0_no_of_rotation_left = true_no_of_rotation_left;
    int t0_no_of_rotation_right = true_no_of_rotation_right;
    no_of_pulses_left = 0;
    no_of_pulses_right = 0;
    while((true_no_of_rotation_right - t0_no_of_rotation_right) < no_of_rotation_for_turn)
    {
      Serial.print((true_no_of_rotation_right - t0_no_of_rotation_right));
        left();
    }
    while((no_of_pulses_right) < 2800*3*0.35){
        left();
    }
}
