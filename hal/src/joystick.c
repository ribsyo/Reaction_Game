#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include <hal/joystick.h>

#define SPI_DEVICE "/dev/spidev0.0"
#define CLOCK_SPEED 100000      // 2MHz at 5v, 1MHz at 2.7v, currently using 3.3
#define JOYSTICK_MAX 4095       // 2^12 - 1, maximum response for 12 bit response, maximum value the joystick can output
#define JOYSTICK_MID 2048       // 2^11 for the approximate centre value of joystick
#define CENTRE_TOLERANCE 200    // tolerance for what counts as the joystick being at centre


int spi;
static bool joystick_init_state = false;

void joystick_init(void) {

    if (joystick_init_state == true) {
        printf("joystick has already been initialized.\n");
        return;
    }
    
    spi = open(SPI_DEVICE, O_RDWR);

    if (spi < 0) {
        perror("Failed to open SPI device");
        return;
    }

    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;
    uint32_t speed = CLOCK_SPEED;

    if (ioctl(spi, SPI_IOC_WR_MODE, &mode) < 0) {
        perror("Failed to set bite per word");
        return;
    }

    if (ioctl(spi, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
        perror("Failed to set bite per word");
        return;
    }

    if (ioctl(spi, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        perror("Failed to set bite per word");
        return;
    }

    joystick_init_state = true;
    return;

}

// internal function for getting joystick x via spi
int getx() {

    uint8_t tx_x[3] = {0x06, 0x00, 0x00};
    uint8_t rx[3] = {0};

    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long)tx_x,
        .rx_buf = (unsigned long)rx,
        .len = 3,
        .speed_hz = CLOCK_SPEED,
        .bits_per_word = 8
    };

    if (ioctl(spi, SPI_IOC_MESSAGE(1), &transfer) < 0) {
        perror("SPI transfer failed");
        return -1;
    }

    return (int)(((rx[1] & 0x0F) << 8) | rx[2]); // 12 bit response -> first 4 bits of rx[1] and all of rx[2]

}

// internal function for getting joystick y via spi
int gety() {

    uint8_t tx_y[3] = {0x06, 0x40, 0x00};
    
    uint8_t rx[3] = {0};

    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long)tx_y,
        .rx_buf = (unsigned long)rx,
        .len = 3,
        .speed_hz = CLOCK_SPEED,
        .bits_per_word = 8
    };

    if (ioctl(spi, SPI_IOC_MESSAGE(1), &transfer) < 0) {
        perror("SPI transfer failed");
        return -1;
    }

    return (int)(((rx[1] & 0x0F) << 8) | rx[2]); // 12 bit response -> first 4 bits of rx[1] and all of rx[2]

}

// note: this is designed in a way where diagonal joystick movement is not returned
State joystick_get_state(void){

    if (joystick_init_state == false) {
        perror("Joystick has not been initialized");
        return CENTRE;
    }

    int x = getx();
    int y = gety();

    int lower = JOYSTICK_MID - CENTRE_TOLERANCE;
    int higher = JOYSTICK_MID + CENTRE_TOLERANCE;

    // if x is not in the center
    if (x < lower || x > higher) { 

        // if y is not in the centre
        if (y < lower || y > higher) {
            //compare x and y to see which dominates
            int mx = abs(JOYSTICK_MID - x);
            int my = abs(JOYSTICK_MID - y);

            if (mx > my){

                if (x < lower) {
                    return LEFT;
                } else {
                    return RIGHT;
                }

            } else {

                if (y < lower) {
                    return DOWN;
                } else {
                    return UP;
                }

            }
        
        }

        if (x < lower) {
            return LEFT;
        } else {
            return RIGHT;
        }

    // if y is not in the centre
    } else if (y < lower || y > higher) { 

        if (y < lower) {
            return DOWN;
        } else {
            return UP;
        }

    } else {

        return CENTRE;

    }



}

bool joystick_initialized(void) {

    return joystick_init_state;

}

void joystick_cleanup(void){

    if (joystick_init_state == false){
        printf("joystick is not initialized\n");
        return;
    }

    close(spi);
    joystick_init_state = false;

}