#include <stdio.h> 
#include <math.h> 
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/pwm.h"

#define c1  7
#define c2  6

int no_of_rotation = 0;
int lastEncoded = 0;

char* float_to_string(float f, int precision) {
    static char str[10]; // Static buffer to hold the result
    snprintf(str, 10, "%.*f", precision, f);
    return str;
}

void update_encoder(){
  int MSB = gpio_get(c1); //MSB = most significant bit

  int LSB = gpio_get(c2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) no_of_rotation --;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) no_of_rotation ++;

  lastEncoded = encoded; //store this value for next time

}
int main() {
    stdio_init_all();
    sleep_ms(5000);
    printf("Running .... \n");
    gpio_init(4);
    gpio_set_function(4, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(4);
    pwm_set_wrap(slice_num, 255); // 8-bit resolution
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 255); // Set initial duty cycle to 50%
    pwm_set_enabled(slice_num, true);

    gpio_init(0);
    gpio_set_dir(0, GPIO_OUT);
    gpio_put(0, 1);
    gpio_init(1);
    gpio_set_dir(1, GPIO_OUT);
    gpio_put(1, 0);

    gpio_init(5);
    gpio_set_dir(5, GPIO_OUT);
    gpio_put(5, 1);
    gpio_init(2);
    gpio_set_dir(2, GPIO_OUT);
    gpio_put(2, 1);
    gpio_init(3);
    gpio_set_dir(3, GPIO_OUT);
    gpio_put(3, 0);

    gpio_init(c1);
    gpio_pull_up(c1);
    gpio_init(c2);
    gpio_pull_up(c2);

    gpio_set_dir(c1, GPIO_IN);
    gpio_set_dir(c2, GPIO_IN);


    gpio_set_irq_enabled_with_callback(c1, GPIO_IRQ_EDGE_RISE || GPIO_IRQ_EDGE_FALL,true, &update_encoder);
    gpio_set_irq_enabled_with_callback(c2, GPIO_IRQ_EDGE_RISE || GPIO_IRQ_EDGE_FALL,true, &update_encoder);

    while (true) {
        printf(float_to_string(no_of_rotation/5000, 2));
        printf("\n");
    }
}