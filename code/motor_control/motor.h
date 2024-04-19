#define motor_speed 100
#define no_of_rotation_for_turn 1

int lastEncoded_left = 0;
int no_of_pulses_left = 0;
int true_no_of_rotation_left = 0;

int lastEncoded_right = 0;
int no_of_pulses_right = 0;
int true_no_of_rotation_right = 0;

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
    analogWrite(en, motor_speed);
}

void forward(){
    // Motor A
    gpio_put(IN1, 0);
    gpio_put(IN2, 1);

    // Motor B
    gpio_put(IN3, 0);
    gpio_put(IN4, 1);
}

void backward(){
    // Motor A
    gpio_put(IN1, 1);
    gpio_put(IN2, 0);

    // Motor B
    gpio_put(IN3, 1);
    gpio_put(IN4, 0);
}

void right(){
    // Motor A
    gpio_put(IN1, 0);
    gpio_put(IN2, 1);

    // Motor B
    gpio_put(IN3, 1);
    gpio_put(IN4, 0);
}

void left(){
    // Motor A
    gpio_put(IN1, 1);
    gpio_put(IN2, 0);

    // Motor B
    gpio_put(IN3, 0);
    gpio_put(IN4, 1);
}

void stop(){
    // Motor A
    gpio_put(IN1, 0);
    gpio_put(IN2, 0);

    // Motor B
    gpio_put(IN3, 0);
    gpio_put(IN4, 0);
}

void update_encoder_left(){
    int MSB = gpio_get(C1A);

    int LSB = gpio_get(C2A);

    int encoded = (MSB << 1) |LSB; 
    int sum  = (lastEncoded_left << 2) | encoded; 

    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)no_of_pulses_left --;
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)no_of_pulses_left ++;

    lastEncoded_left = encoded; //store this value for next time

    if(abs(no_of_pulses_left) > 2800*3)
    {
        no_of_pulses_left = 0;
        true_no_of_rotation_left += 1;
        Serial.print("no_of_rotation_left : ");
        Serial.println(true_no_of_rotation_left);
        Serial.println("\n");
    }

}

void update_encoder_right(){
    int MSB = gpio_get(C1B);

    int LSB = gpio_get(C2B);

    int encoded = (MSB << 1) |LSB; 
    int sum  = (lastEncoded_right << 2) | encoded; 

    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) no_of_pulses_right --;
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) no_of_pulses_right ++;

    lastEncoded_right = encoded; //store this value for next time
    if(abs(no_of_pulses_right) > 2800*3)
    {
        no_of_pulses_right = 0;
        true_no_of_rotation_right += 1;
        Serial.print("no_of_rotation_right : ");
        Serial.println(true_no_of_rotation_right);
        Serial.println("\n");
    }
}

void forward_x(int x){
    int t0_no_of_rotation_left = true_no_of_rotation_left;
    int t0_no_of_rotation_right = true_no_of_rotation_right;
    Serial.print("to : left");
    Serial.println(t0_no_of_rotation_left);
    Serial.print("to : right");
    Serial.println(t0_no_of_rotation_right);
    while(abs(true_no_of_rotation_left - t0_no_of_rotation_left) < x && abs(true_no_of_rotation_right - t0_no_of_rotation_right) < x)
    {
        forward();
    }
    stop();
}
void backward_x(int x){
    int t0_no_of_rotation_left = true_no_of_rotation_left;
    int t0_no_of_rotation_right = true_no_of_rotation_right;
    while(abs(true_no_of_rotation_left - t0_no_of_rotation_left) < x && abs(true_no_of_rotation_right - t0_no_of_rotation_right) < x)
    {
        backward();
    }
    stop();
}

void turn_right(){
    int t0_no_of_rotation_left = true_no_of_rotation_left;
    int t0_no_of_rotation_right = true_no_of_rotation_right;
    while((true_no_of_rotation_left - t0_no_of_rotation_left) < no_of_rotation_for_turn)
    {
      Serial.print((true_no_of_rotation_left - t0_no_of_rotation_left));
      right();
    }
    while(abs(no_of_pulses_left) < 2800*3*0.108){
      right();
    }
    stop();
}
void turn_left(){
    // Serial.Print("TURN_LEFT()")
    // int t0_no_of_rotation_left = true_no_of_rotation_left;
    // int t0_no_of_rotation_right = true_no_of_rotation_right;
    // while((true_no_of_rotation_right - t0_no_of_rotation_right) < no_of_rotation_for_turn)
    // {
    //     left();
    // }
    // while(abs(no_of_pulses_right) < 2800*3*0.2){
    //     left();
    // }
    // stop();
    turn_right();
    turn_right();
    turn_right();
    while(abs(no_of_pulses_left) < 2800*3*0.22){
      right();
    }
}


