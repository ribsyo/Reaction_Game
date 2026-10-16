// Main program to build the application
// Reaction game using the analog thumb joystick

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include "hal/led.h"
#include "hal/joystick.h"

// note: 1000000000 nanoseconds = 1 second
void sleep(long seconds, long nanoseconds){
    struct timespec reqDelay = {seconds, nanoseconds}; 
    nanosleep(&reqDelay, (struct timespec *) NULL); 
    return;
}

void game_start_pattern(){
    led_set(GREEN);
    sleep(0, 250000000);
    led_set(OFF);
    sleep(0, 250000000);
    led_set(RED);
    sleep(0, 250000000);
    led_set(OFF);
    sleep(0, 250000000);

    led_set(GREEN);
    sleep(0, 250000000);
    led_set(OFF);
    sleep(0, 250000000);
    led_set(RED);
    sleep(0, 250000000);
    led_set(OFF);
    sleep(0, 250000000);

    led_set(GREEN);
    sleep(0, 250000000);
    led_set(OFF);
    sleep(0, 250000000);
    led_set(RED);
    sleep(0, 250000000);
    led_set(OFF);
    sleep(0, 250000000);

    led_set(GREEN);
    sleep(0, 250000000);
    led_set(OFF);
    sleep(0, 250000000);
    led_set(RED);
    sleep(0, 250000000);
    led_set(OFF);
    sleep(0, 250000000);
}

void correct_answer_pattern(){
    led_set(GREEN);
    sleep(0, 100000000);
    led_set(OFF);
    sleep(0, 100000000);
    led_set(GREEN);
    sleep(0, 100000000);
    led_set(OFF);
    sleep(0, 100000000);
    led_set(GREEN);
    sleep(0, 100000000);
    led_set(OFF);
    sleep(0, 100000000);
    led_set(GREEN);
    sleep(0, 100000000);
    led_set(OFF);
    sleep(0, 100000000);
    led_set(GREEN);
    sleep(0, 100000000);
    led_set(OFF);
    sleep(0, 100000000);
    return;
}

void incorrect_answer_pattern(){
    led_set(RED);
    sleep(0, 100000000);
    led_set(OFF);
    sleep(0, 100000000);
    led_set(RED);
    sleep(0, 100000000);
    led_set(OFF);
    sleep(0, 100000000);
    led_set(RED);
    sleep(0, 100000000);
    led_set(OFF);
    sleep(0, 100000000);
    led_set(RED);
    sleep(0, 100000000);
    led_set(OFF);
    sleep(0, 100000000);
    led_set(RED);
    sleep(0, 100000000);
    led_set(OFF);
    sleep(0, 100000000);
    return;
}

// main game loop
void play_game(){

    int best_time = 5000;
    State state;
    while (true) {

        printf("Get READY!\n");
        fflush(stdout);

        game_start_pattern();
        state = joystick_get_state();
        if (state == UP || state == DOWN) {
            printf("Please let go of the joystick...\n");
            fflush(stdout);

            while(joystick_get_state() != CENTRE) {
                sleep(0, 100000000);
            }

        }

        int num = (rand() % (251)) + 50; // gives a number between 50 and 300

        // seperate the random number into seconds and nanoseconds
        sleep((long)(num / 100), (long)(num % 100) * 10000000L);

        state = joystick_get_state();
        if (state == UP || state == DOWN) {

            printf("too soon!\n");
            fflush(stdout);
            continue;

        }

        // select a direction

        State target;
        if (rand() % 2 == 0) {

            printf("UP\n");
            led_set(GREEN);
            target = UP;

        } else {

            printf("DOWN\n");
            led_set(RED);
            target = DOWN;

        }

        fflush(stdout);

        bool quit = false;
        bool timeout = true;

        // 1000000 nanoseconds = 1 millisecond
        // delay from overhead of other code is negligible
        for (int i = 0; i < 5000; i++){
            sleep(0, 1000000);
            state = joystick_get_state();

            if (state == CENTRE) {
                continue;
            } else if (state == LEFT || state == RIGHT) {
                quit = true;
                break;
            } else {
                if (state == target) {

                    printf("You got it right!\n");
                    printf("The best time is: %dms\n", best_time);
                    printf("Your time is: %dms!\n", i);

                    if (i < best_time) {

                        printf("You achived a new record!\n");
                        best_time = i;

                    }

                    fflush(stdout);
                    correct_answer_pattern();
                    timeout = false;
                    break;

                } else {
                    
                    printf("you got it wrong... try again\n");
                    incorrect_answer_pattern();
                    timeout = false;
                    break;

                }
            }
        }

        if (quit == true) {
            printf("quitting the program.\n");
            break;
        }

        if (timeout == true) {
            printf("you have times out... quitting the program.\n");
            break;
        }

    }

    return;

}



int main()
{

    printf("initializing joystick\n");
    fflush(stdout);
    joystick_init();
    if (joystick_initialized() == false){
        return 1;
    }

    printf("initializing led\n");
    fflush(stdout);
    led_init();
    if (led_initialized() == false){
        joystick_cleanup();
        return 1;
    }

    printf("hardware initialization is complete\n");

    srand(time(NULL));
    play_game();

    led_cleanup();
    joystick_cleanup();


    return 0;
}