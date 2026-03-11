# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Arduino-based remote control system for a NeZha smart car with mecanum wheels and robot arm. Supports three control modes:
1. PS2 wireless controller (default)
2. Line tracking with IR sensors
3. WiFi control via ESP8266-01S module (web browser/keyboard)

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

**ESP8266-01S WiFi Module (Optional)**
- Pins: RX=2, TX=3 (SoftwareSerial)
- Voltage: 3.3V (use voltage divider on RX pin)
- Baud rate: 115200
- Creates TCP server on port 80
- Serves HTML control page with keyboard support

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

- **Rzhl_car.ino**: Main control logic with mode switching
- **Rzhl.cpp/h**: Driver board API (motors, servos, encoders, LEDs)
- **Rzhl_I2C.cpp/h**: Software I2C implementation
- **ps2.cpp/h**: PS2 controller SPI-like protocol
- **RobotArm.cpp/h**: Robot arm servo abstraction with limit checking
- **Board_Timer.cpp/h**: TimerOne wrapper for 20ms interrupt
- **LED.cpp/h**: Status LED on pin 13
- **ESP8266_WiFi.cpp/h**: WiFi module communication and web server
- **Vehicle_Chassis.cpp/h**: Mecanum wheel movement functions
- **Sensor.cpp/h**: IR sensor interface for line tracking

## Control Modes

The system supports three operating modes:

**1. Remote Control Mode (MODE_REMOTE = 0)**
- Default mode on startup
- Controlled by PS2 wireless controller
- Status LED blinks every 1 second
- Supports both analog (red light) and digital (green light) modes

**2. Line Tracking Mode (MODE_LINE_TRACK = 1)**
- Autonomous line following using IR sensors
- Status LED stays on constantly
- Uses 4 IR sensors to detect line position
- Switch via SELECT button or serial command '1'

**3. WiFi Control Mode (MODE_WIFI = 2)**
- Remote control via web browser
- Keyboard control: W/A/S/D or arrow keys
- Status LED blinks rapidly
- Switch via START button or serial command 'w'
- Requires ESP8266-01S module

**Mode Switching:**
- **START button**: Toggle between Remote/Line Track ↔ WiFi mode
- **SELECT button**: Toggle between Remote ↔ Line Track mode (not available in WiFi mode)
- **Serial commands**: '0'=Remote, '1'=Line Track, 'w'=WiFi

**Red Light Mode (Analog - 0x73)**
- Normal: Left stick = movement, right stick X = rotation
- L1 held: Left stick Y = raise/drop, right stick Y = stretch/shrink, right stick X = shake/let

**Green Light Mode (Digital - 0x41)**
- Normal: D-pad = movement, L2/R2 = rotation
- L1 held: D-pad up/down = raise/drop, triangle/cross = stretch/shrink, square/circle = shake/let

**WiFi Control Commands:**
- W or ↑: Forward (speed 600)
- S or ↓: Backward (speed 600)
- A or ←: Left turn (speed 600)
- D or →: Right turn (speed 600)
- X or Space: Stop

## Tail LED States

Controlled by Led_State variable (project.ino:19):
- 0: Off (stopped)
- 1: Both on (reversing, ly_target > 0)
- 2: Left blinking (left turn, lx_target < 0 or rx_target < 0)
- 3: Right blinking (right turn, lx_target > 0 or rx_target > 0)

## Porting to Different Arduino

Modify pin assignments in:
1. **Rzhl_I2C.cpp:33-34**: Change SCL/SDA pins (currently 6/7)
2. **ps2.cpp:5-8**: Change PS2 pins (currently DI=11, DO=9, CS=8, SCK=10)
3. **LED.cpp:3**: Change status LED pin (currently 13)
4. **ESP8266_WiFi.cpp:8-9**: Change ESP8266 pins (currently RX=2, TX=3)

Update corresponding pinMode() calls in Init functions.

## WiFi Module Setup

**Hardware Connection:**
- ESP8266 VCC → 3.3V (NOT 5V!)
- ESP8266 GND → GND
- ESP8266 TX → Arduino Pin 3
- ESP8266 RX → Arduino Pin 2 (use 1K-2K resistor divider to 3.3V)
- ESP8266 CH_PD → 3.3V

**Software Configuration:**
Edit `ESP8266_WiFi.cpp` lines 13-14:
```cpp
const char* WIFI_SSID = "YourWiFiSSID";
const char* WIFI_PASSWORD = "YourPassword";
```

**Usage:**
1. Press START button or send 'w' via serial
2. Wait for WiFi initialization (check serial monitor for IP)
3. Open browser to ESP8266 IP address (e.g., http://192.168.1.100)
4. Control via web page buttons or keyboard

See `ESP8266使用说明.md` for detailed instructions.

## Important Constants

- `UNIT_PWM`: Servo increment per cycle (Rzhl_car.ino:10, default: 2)
- `PS2_LSPEED`: Max motor speed for left stick (Rzhl_car.ino:11, value: 1000)
- `PS2_RSPEED`: Max motor speed for right stick (Rzhl_car.ino:12, value: 1000)
- `RECIPROCAL`: 0.0078f (1/128 for joystick mapping, Rzhl_car.ino:13)
- `HIGH_TIM`: PS2 SPI timing delay (ps2.cpp:10, value: 50)
- `wifi_speed`: WiFi control speed (Rzhl_car.ino ProcessWiFiControl(), value: 600)

## Serial Debug Commands

Open serial monitor at 9600 baud:
- `0` - Switch to Remote mode
- `1` - Switch to Line Track mode
- `w` - Switch to WiFi mode (initializes ESP8266)
- `m` - Show current mode
- `s` - Show sensor status
- `d` - Toggle debug mode (shows all PS2 button presses)

## I2C Communication Protocol

All NeZha commands follow this pattern:
1. Send command byte via NeZha_WriteCommand()
2. For data commands: Start → Address → Command → Data bytes → Stop
3. Motor PWM: 4 bytes (motor_a high/low, motor_b high/low)
4. Servo PWM: 2 bytes (pwm high/low)

Motor direction: motor_a = reverse, motor_b = forward (0-1000 range)
