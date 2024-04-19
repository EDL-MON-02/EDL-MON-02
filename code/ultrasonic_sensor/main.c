#include <stdio.h> 
#include <math.h> 
#include "pico/stdlib.h"
#include "pico/time.h"

#define echo_pin    18
#define triger_pin  19

char* float_to_string(float f, int precision) {
    static char str[10]; // Static buffer to hold the result
    snprintf(str, 10, "%.*f", precision, f);
    return str;
}
void sensor_init(){
    gpio_init(echo_pin);
    gpio_set_dir(echo_pin, GPIO_IN);
    gpio_init(triger_pin);
    gpio_set_dir(triger_pin, GPIO_OUT);
}

void triger_sensor(){
    gpio_put(triger_pin, 0);
    sleep_ms(2);
    gpio_put(triger_pin, 1);
    sleep_ms(10);
    gpio_put(triger_pin, 0);
}

float measure(){
    printf("Trigered sensor \n");
    triger_sensor();
    while (!gpio_get(echo_pin));
    printf("Pulse started \n");

    absolute_time_t start_time = get_absolute_time();
    while (gpio_get(echo_pin));
    absolute_time_t end_time = get_absolute_time();

    printf("Pulse ended \n");
    uint32_t duration = absolute_time_diff_us(start_time, end_time);

    float distance = (duration / 2.0) / 29.1; 

    return distance;

}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("Initilizing sensor .... \n");
    
    sensor_init();
    float distance = 0;

    while (true) {
        distance = measure();
        printf(float_to_string(distance, 2));
        printf("\n");
    }
}