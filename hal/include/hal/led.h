// led hal module descriptor
// this interface lets the user set the builtin beagley-ai red and green LEDs

#ifndef _LED_H_
#define _LED_H_

#include <stdbool.h>

// Represents the LED colors that can be displayed.
typedef enum {
    OFF,
    GREEN,
    RED
} Color;

// Initializes the LED hardware.
void led_init(void);

// Sets the LED to the selected color.
void led_set(Color color);

// Cleans up the LED resources.
void led_cleanup(void);

// Returns whether the LED has been initialized.
bool led_initialized(void);

#endif