# BeagleY-AI Reaction Timer

A reaction game for the **BeagleY-AI** board. The player uses an analog
joystick to respond to an LED prompt as quickly as possible. The joystick is
connected to an external analog-to-digital converter (ADC), and the ADC is
read through the board's SPI interface.

## How the game works

1. The game flashes a green/red start pattern.
2. The player releases the joystick and waits for the next prompt.
3. After a random delay, the game selects **UP** or **DOWN**:
   - **Green LED**: move the joystick **UP**.
   - **Red LED**: move the joystick **DOWN**.
4. The program measures the response time in milliseconds and reports the
   result in the terminal.
5. A correct response flashes the green LED; an incorrect response flashes
   the red LED.

The best response time is retained while the program is running. Moving the
joystick **LEFT** or **RIGHT** exits the game. Responding before the prompt,
waiting too long, or moving in the wrong direction ends that round or exits
the game as indicated by the terminal output.

## Hardware

The project uses:

- BeagleY-AI board running Linux
- Analog thumb joystick
- External ADC connected to the joystick's X and Y outputs
- SPI connection between the BeagleY-AI and ADC
- The BeagleY-AI's onboard ACT (green) and PWR (red) LEDs

The program expects the ADC to be available as:

```text
/dev/spidev0.0
```

The joystick driver reads two ADC channels over SPI and interprets the
12-bit values around a midpoint of 2048. A tolerance band around the
midpoint prevents small movements and electrical noise from being treated as
input. If both axes are moved at once, the axis with the larger displacement
determines the direction.

> **Wiring note:** Confirm the ADC channel assignment, SPI chip select, power,
> and ground connections against your ADC and BeagleY-AI pinout before
> powering the circuit. Do not connect an ADC or joystick signal above the
> board's supported voltage.

## Project structure

```text
.
├── app/
│   ├── include/             # Application headers
│   └── src/main.c           # Reaction game and timing logic
├── hal/
│   ├── include/hal/
│   │   ├── joystick.h       # Joystick interface
│   │   └── led.h            # LED interface
│   └── src/
│       ├── joystick.c       # SPI ADC and joystick state handling
│       └── led.c            # Linux LED control
├── CMakeLists.txt
└── README.md
```

The hardware abstraction layer (`hal`) keeps SPI and Linux LED details
separate from the game logic in `app`.

## Building

### On the BeagleY-AI

Install the build tools if necessary:

```sh
sudo apt update
sudo apt install build-essential cmake
```

Configure and build from the project directory:

```sh
cmake -S . -B build
cmake --build build
```

The executable is:

```text
build/app/reaction_timer
```

The project enables AddressSanitizer in `CMakeLists.txt`. If the target
does not already have the runtime installed, install it with the package
appropriate for the board's compiler (for example, `libasan6` on Debian
systems), or disable the sanitizer options for a production build.

### Cross-compiling from a development machine

Use the CMake Tools extension in VS Code and select an ARM Linux toolchain
configured for the BeagleY-AI. The host compiler can build the project, but
the resulting executable will only run on the host if it was not cross-
compiled for ARM.

## Running

Before starting the game, verify that:

- the SPI device `/dev/spidev0.0` exists;
- the user running the program can open the SPI device;
- the ADC and joystick are powered and wired correctly; and
- the BeagleY-AI LED sysfs interfaces are available.

Run the game on the board with:

```sh
./build/app/reaction_timer
```

The game prints prompts, response times, records, and error messages to the
terminal. When the normal game loop exits, the LED driver restores the ACT
LED's heartbeat behavior. If the process is stopped with `Ctrl+C`, verify the
LED state afterward and restore the heartbeat trigger if necessary.

## Configuration

The main hardware constants are defined in
[`hal/src/joystick.c`](hal/src/joystick.c):

- SPI device: `/dev/spidev0.0`
- SPI mode: 0
- SPI clock: 100 kHz
- ADC resolution: 12 bits
- Joystick midpoint: 2048
- Centre tolerance: 200 ADC counts

Adjust these values only after checking the ADC datasheet and the actual
electrical setup.

## Troubleshooting

### `Failed to open SPI device`

Make sure SPI is enabled in the BeagleY-AI device configuration and that
`/dev/spidev0.0` exists. Check the device permissions or run the program
with an account that belongs to the appropriate hardware-access group.

### The joystick is always centred or moves in the wrong direction

Check the ADC power and ground first, then verify the X/Y channel wiring.
Confirm that the ADC command/channel configuration in `hal/src/joystick.c`
matches the converter used in the circuit.

### LEDs do not change

The program controls the onboard `ACT` and `PWR` LED sysfs interfaces and
requires permission to write to them. Check that those LED paths exist on the
running image and that no other service is immediately restoring their
triggers.
