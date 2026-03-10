# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Arduino-based remote control system for a NeZha smart car with mecanum wheels and robot arm. Uses PS2 wireless controller for input and communicates with NeZha motor driver board via I2C.

## Build and Upload

This is an Arduino sketch. To compile and upload:
```
Arduino IDE: Open project.ino, select board type, click Upload
Arduino CLI: arduino-cli compile --fqbn <board> . && arduino-cli upload -p <port> --fqbn <board> .
```

## Hardware Architecture

**NeZha Driver Board (I2C Address: 0x80)**
- 4 DC motors (M1-M4) for mecanum wheel chassis
- 4 servo channels (Servo1-Servo4) for robot arm
- Onboard LEDs (head, tail left/right, ambient)
- Software I2C on pins 6 (SCL) and 7 (SDA)
- Max I2C speed: 200KHz (UNIT_T >= 2us in NeZha_I2C.cpp:31)

**PS2 Controller (SPI-like protocol)**
- Pins: DI=11, DO=9, CS=8, SCK=10 (ps2.cpp:5-8)
- Two modes: 0x73=red light (analog), 0x41=green light (digital)
- Initialization sequence: ShortPoll → EnterConfig → TurnOnAnalogMode → ExitConfig

**Robot Arm Servos**
- Servo2 → Left servo (raise/drop hand): PWM 186-250
- Servo3 → Front servo (shake/let hand - gripper): PWM 100-250
- Servo4 → Right servo (stretch/shrink hand): PWM 163-250

**Status LED**
- Pin 13, toggles every 1 second

## Control Loop Timing

Uses TimerOne library with 20ms interrupt (Board_Timer.cpp):
- PS2 input reading: every 20ms (1x cycle)
- Motor control updates: every 40ms (2x cycles)
- Tail LED updates: every 200ms (10x cycles)
- Status LED toggle: every 1000ms (50x cycles)

## Mecanum Wheel Kinematics

Motor speed calculation (project.ino:86-90):
```
M1 =  ly + lx - rx  (front-left)
M2 = -ly + lx - rx  (front-right)
M3 = -ly - lx - rx  (rear-right)
M4 =  ly - lx - rx  (rear-left)
```

Where:
- ly: forward/backward (left stick Y)
- lx: strafe left/right (left stick X)
- rx: rotation (right stick X)

**Joystick Mapping (Analog Mode)**
- ly_target = 7.8f * (ps2_ly - 127)
- lx_target = 7.8f * (ps2_lx - 128)
- rx_target = 6.24f * (ps2_rx - 128)
- Joystick range: 0-255, center at 128
- Motor PWM range: 0-1000

## Key Files

- **project.ino**: Main control logic, 212 lines
- **NeZha.cpp/h**: Driver board API (motors, servos, encoders, LEDs)
- **NeZha_I2C.cpp/h**: Software I2C implementation
- **ps2.cpp/h**: PS2 controller SPI-like protocol
- **RobotArm.cpp/h**: Robot arm servo abstraction with limit checking
- **Board_Timer.cpp/h**: TimerOne wrapper for 20ms interrupt
- **LED.cpp/h**: Status LED on pin 13

## Control Modes

**Red Light Mode (Analog - 0x73)**
- Normal: Left stick = movement, right stick X = rotation
- L1 held: Left stick Y = raise/drop, right stick Y = stretch/shrink, right stick X = shake/let

**Green Light Mode (Digital - 0x41)**
- Normal: D-pad = movement, L2/R2 = rotation
- L1 held: D-pad up/down = raise/drop, triangle/cross = stretch/shrink, square/circle = shake/let

## Tail LED States

Controlled by Led_State variable (project.ino:19):
- 0: Off (stopped)
- 1: Both on (reversing, ly_target > 0)
- 2: Left blinking (left turn, lx_target < 0 or rx_target < 0)
- 3: Right blinking (right turn, lx_target > 0 or rx_target > 0)

## Porting to Different Arduino

Modify pin assignments in:
1. **NeZha_I2C.cpp:33-34**: Change SCL/SDA pins (currently 6/7)
2. **ps2.cpp:5-8**: Change PS2 pins (currently DI=11, DO=9, CS=8, SCK=10)
3. **LED.cpp:3**: Change status LED pin (currently 13)

Update corresponding pinMode() calls in Init functions.

## Important Constants

- `UNIT_PWM`: Servo increment per cycle (project.ino:9, default: 2)
- `PS2_LSPEED`: Max motor speed for left stick (project.ino:10, value: 1000)
- `PS2_RSPEED`: Max motor speed for right stick (project.ino:11, value: 1000)
- `RECIPROCAL`: 0.0078f (1/128 for joystick mapping, project.ino:12)
- `HIGH_TIM`: PS2 SPI timing delay (ps2.cpp:10, value: 50)

## I2C Communication Protocol

All NeZha commands follow this pattern:
1. Send command byte via NeZha_WriteCommand()
2. For data commands: Start → Address → Command → Data bytes → Stop
3. Motor PWM: 4 bytes (motor_a high/low, motor_b high/low)
4. Servo PWM: 2 bytes (pwm high/low)

Motor direction: motor_a = reverse, motor_b = forward (0-1000 range)
