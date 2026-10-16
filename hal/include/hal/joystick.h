// joystick hal module descriptor
// a joystick that detects the direction of the highest magnitude

#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include <stdbool.h>

// Represents the current joystick position or action.
typedef enum {
    CENTRE,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SELECT
} State;

// Initializes the joystick hardware.
void joystick_init(void);

// Returns the current joystick state.
State joystick_get_state(void);

// Cleans up the joystick resources.
void joystick_cleanup(void);

// Returns whether the joystick has been initialized.
bool joystick_initialized(void);

#endif