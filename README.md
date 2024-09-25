# Tetris Game on DE1-SoC

## Description
This project involves developing a Tetris game for the DE1-SoC development board. The game is controlled using the accelerometer for movement, DE1-SoC buttons for menu navigation, and utilizes the VGA interface for game visualization. The system is designed to be a comprehensive demonstration of hardware-software interaction using the DE1-SoC's capabilities.

## Table of Contents
1. [Description](#description)
2. [Objectives](#objectives)
3. [Requirements](#requirements)
4. [Hardware Setup](#hardware-setup)
5. [How to Compile and Run](#how-to-compile-and-run)
6. [Images](#images)
7. [Testing](#testing)
8. [Final Analysis](#final-analysis)
9. [Additional Research Material](#additional-research-material)

## Objectives
- Apply hardware-software interaction knowledge to develop a functional game.
- Program in C for ARM architecture.
- Understand Linux OS management on ARM architecture.
- Utilize the DE1-SoC board's interfaces effectively.

## Requirements
- DE1-SoC Development Board
- Accelerometer (ADXL345)
- VGA Monitor
- Push Buttons and LEDs
- Linux OS (preferably running on ARM architecture)
- Basic knowledge of C programming and hardware interfacing

## Hardware Setup
### DE1-SoC Board
  - **CPU**: ARM Cortex-A9
  - **Memory**: 1GB DDR3
  - **Video Output**: VGA
  - **Input**: Accelerometer, push buttons, LEDs, 7-segment display

### Connections
1. **VGA Monitor**: Connect to the VGA output of the DE1-SoC.
2. **Accelerometer**: Connect to I2C interface.
3. **7-Segment Display**: Connect to the HEX display interface.
4. **Push Buttons**: Connect to GPIO for user input.
5. **LEDs**: Connect to GPIO for status indication.

## How to Compile and Run
1. **Clone the Repository**:
    ```bash
    git clone https://github.com/yourusername/tetris-de1-soc.git
    cd tetris-de1-soc
    ```

2. **Compile the Code**:
    ```bash
    make
    ```

3. **Run the Game**:
    ```bash
    sudo ./tetris
    ```

   Ensure that you have the necessary permissions to access `/dev/mem`.

## Images
### DE1-SoC Board and Hardware Setup
![DE1-SoC Board](images/de1-soc-board.jpg)

### VGA Display Showing the Game
![Game on VGA](images/vga-display.jpg)

### Accelerometer and Other Components
![Accelerometer](images/accelerometer.jpg)

## Testing
During the testing phase, the following steps were taken:
1. Verified connections between the DE1-SoC and peripherals (VGA).
2. Ensured the game correctly responds to accelerometer inputs for piece movement.
3. Tested push buttons for game control functions (pause, resume, reset and exit).
4. Verified the game's visual output on the VGA monitor and ensured no graphical issues.

## Final Analysis
The project successfully demonstrated the integration of hardware and software on the DE1-SoC board. Key learning outcomes include handling hardware interfaces, managing real-time game logic, and developing a complete game system using C programming. Future improvements could include refining the game's controls and adding more features.

## Additional Research Material
- [DE1-SoC User Manual](datasheets/user_manual.pdf)
- [ADXL345 Accelerometer Datasheet](datasheets/Accelerometer.pdf)

- [Return to Table of Contents](#table-of-contents)
