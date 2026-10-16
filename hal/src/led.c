#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal/led.h"

#define GREEN_LED_TRIGGER_FILE "/sys/class/leds/ACT/trigger"
#define RED_LED_TRIGGER_FILE "/sys/class/leds/PWR/trigger"
#define GREEN_LED_BRIGHTNESS_FILE "/sys/class/leds/ACT/brightness"
#define RED_LED_BRIGHTNESS_FILE "/sys/class/leds/PWR/brightness"


static bool led_init_state = false;
Color current = OFF;

void write_to_file(const char* path, const char* content) {

    FILE *file = fopen(path, "w");

    if (file == NULL) { 

        perror("Error opening file"); 
        return;

    }

    int charWritten = fprintf(file, "%s", content); 

    if (charWritten <= 0) { 

        perror("Error writing data to file"); 
        fclose(file);
        return;
    }

    fclose(file);
}

void led_init(void){
    // Set red and green led trigger file to "none"

    write_to_file(GREEN_LED_TRIGGER_FILE, "none");
    write_to_file(RED_LED_TRIGGER_FILE, "none");

    write_to_file(RED_LED_BRIGHTNESS_FILE, "0");
    write_to_file(GREEN_LED_BRIGHTNESS_FILE, "0");

    current = OFF;

    led_init_state = true;
    printf("Initialized Red and Green LEDs\n");
    fflush(stdout);

    return;
}

void led_set(Color target){
    // turn green and red led on/off depending on target color

    if (target == current){
        return;
    }

    if (target == OFF){

        if (current == GREEN){

            write_to_file(GREEN_LED_BRIGHTNESS_FILE, "0");

        } else {

            write_to_file(RED_LED_BRIGHTNESS_FILE, "0");

        }

        current = OFF;

    } else if (target == GREEN) {

        write_to_file(GREEN_LED_BRIGHTNESS_FILE, "1");

        if (current == RED) {

            write_to_file(RED_LED_BRIGHTNESS_FILE, "0");

        }

        current = GREEN;

    } else if (target == RED) {

        write_to_file(RED_LED_BRIGHTNESS_FILE, "1");

        if (current == GREEN) {

            write_to_file(GREEN_LED_BRIGHTNESS_FILE, "0");

        }

        current = RED;

    }

    return;
}

void led_cleanup(void){
    if (led_init_state == false){
        printf("LEDs has not been initialized\n");
        fflush(stdout);
        return;
    }

    write_to_file(GREEN_LED_TRIGGER_FILE, "heartbeat");
    write_to_file(RED_LED_TRIGGER_FILE, "none");

    led_init_state = false;
    printf("LEDs control have been returned to heartbeat\n");
    fflush(stdout);

    return;
}

bool led_initialized(void) {

    return led_init_state;

}