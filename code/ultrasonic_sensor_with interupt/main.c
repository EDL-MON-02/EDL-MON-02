#include <stdio.h> 
#include <math.h> 
#include "pico/stdlib.h"
#include "pico/time.h"

#define echo_pin    18
#define triger_pin  19

float distance = 0;

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

void measure(){
    absolute_time_t start_time = get_absolute_time();
    while (gpio_get(echo_pin));
    absolute_time_t end_time = get_absolute_time();

    printf("Pulse ended \n");
    uint32_t duration = absolute_time_diff_us(start_time, end_time);

    distance = (duration / 2.0) / 29.1; 
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    sensor_init();
    printf("Initilizing sensor .... \n");
    gpio_set_irq_enabled_with_callback(echo_pin, GPIO_IRQ_EDGE_RISE, true, &measure);

    while (true) {
        triger_sensor();
        printf(float_to_string(distance, 2));
        printf("\n");
        sleep_ms(2000);
    }
}