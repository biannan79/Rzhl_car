# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Arduino-based remote control system for a Rzhl smart car with mecanum wheels and robot arm. Supports three control modes:
1. PS2 wireless controller (default)
2. Line tracking with IR sensors
3. WiFi control via ESP8266-01S module (web browser/keyboard)

## Build and Upload

This is an Arduino sketch. To compile and upload:
```
Arduino IDE: Open Rzhl_car.ino, select board type, click Upload
Arduino CLI: arduino-cli compile --fqbn <board> . && arduino-cli upload -p <port> --fqbn <board> .
```

## Hardware Architecture

**Rzhl Driver Board (I2C Address: 0x80)**
- 4 DC motors (M1-M4) for mecanum wheel chassis
- 4 servo channels (Servo1-Servo4) for robot arm
- Onboard LEDs (head, tail left/right, ambient)
- Software I2C on pins 6 (SCL) and 7 (SDA)
- Max I2C speed: 200KHz (UNIT_T >= 2us in Rzhl_I2C.cpp:31)

**PS2 Controller (SPI-like protocol)**
- Pins: DI=11, DO=9, CS=8, SCK=10 (ps2.cpp:5-8)
- Two modes: 0x73=red light (analog), 0x41=green light (digital)
- Initialization sequence: ShortPoll → EnterConfig → TurnOnAnalogMode → ExitConfig

**Robot Arm Servos**
- Servo2 → Left servo (raise/drop hand): PWM 186-250
- Servo3 → Front servo (shake/let hand - gripper): PWM 100-250
- Servo4 → Right servo (stretch/shrink hand): PWM 163-250

**Status LED (Pin 13)**
- Remote mode: OFF
- WiFi mode: Blinking (1 second interval)
- Line tracking mode: ON constantly

**ESP8266-01S WiFi Module**
- Connection: ESP8266 TX → Arduino Pin 0 (RX), ESP8266 RX → Arduino Pin 1 (TX)
- Voltage: 3.3V (IMPORTANT: NOT 5V!)
- Baud rate: 115200
- Runs standalone web server with HTML control interface
- Single-click control mode: each button press moves car for ~200ms

## Control Loop Timing

Uses TimerOne library with 20ms interrupt (Board_Timer.cpp):
- PS2 input reading: every 20ms (1x cycle)
- WiFi timeout check: every 20ms (1x cycle)
- Motor control updates: every 40ms (2x cycles)
- Tail LED updates: every 200ms (10x cycles)
- Status LED toggle: every 1000ms (50x cycles)

## Mecanum Wheel Kinematics

**PS2 Remote Control:**
Motor speed calculation (Rzhl_car.ino:366-369):
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

**WiFi Control:**
Direct motor control (Rzhl_car.ino:73-103):
- Forward (W): M1=-600, M2=600, M3=600, M4=-600
- Backward (S): M1=600, M2=-600, M3=-600, M4=600
- Left (A): M1=600, M2=600, M3=600, M4=600
- Right (D): M1=-600, M2=-600, M3=-600, M4=-600
- Auto-stop after 200ms without new command

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
- Status LED: OFF
- Supports both analog (red light) and digital (green light) modes
- Flash pattern on entry: 2 blinks

**2. Line Tracking Mode (MODE_LINE_TRACK = 1)**
- Autonomous line following using IR sensors
- Status LED: ON constantly
- Uses 4 IR sensors to detect line position
- Switch via SELECT button (from remote mode only)
- Flash pattern on entry: 1 blink

**3. WiFi Control Mode (MODE_WIFI = 2)**
- Remote control via web browser
- Status LED: Blinking (1 second interval)
- Single-click control: each command moves car for ~200ms
- Switch via START button (from any mode)
- Flash pattern on entry: 3 blinks
- Flash pattern on exit: 2 blinks
- Requires ESP8266-01S module

**Mode Switching:**
- **START button**: Toggle between (Remote/Line Track) ↔ WiFi mode
- **SELECT button**: Toggle between Remote ↔ Line Track mode (only when NOT in WiFi mode)
- **Web button/Q key**: Exit WiFi mode → return to Remote mode

**Red Light Mode (Analog - 0x73)**
- Normal: Left stick = movement, right stick X = rotation
- L1 held: Left stick Y = raise/drop, right stick Y = stretch/shrink, right stick X = shake/let

**Green Light Mode (Digital - 0x41)**
- Normal: D-pad = movement, L2/R2 = rotation
- L1 held: D-pad up/down = raise/drop, triangle/cross = stretch/shrink, square/circle = shake/let

**WiFi Control Commands:**
- W or ↑: Forward (speed 600, ~200ms)
- S or ↓: Backward (speed 600, ~200ms)
- A or ←: Left turn (speed 600, ~200ms)
- D or →: Right turn (speed 600, ~200ms)
- X or Space: Stop immediately
- Q: Exit WiFi mode

## Tail LED States

Controlled by Led_State variable (Rzhl_car.ino:25):
- 0: Off (stopped)
- 1: Both on (reversing)
- 2: Left blinking (left turn)
- 3: Right blinking (right turn)

## Porting to Different Arduino

Modify pin assignments in:
1. **Rzhl_I2C.cpp:33-34**: Change SCL/SDA pins (currently 6/7)
2. **ps2.cpp:5-8**: Change PS2 pins (currently DI=11, DO=9, CS=8, SCK=10)
3. **LED.cpp:3**: Change status LED pin (currently 13)
4. **ESP8266_WiFi.cpp:13**: Change to hardware Serial (currently using Serial on pins 0/1)

Update corresponding pinMode() calls in Init functions.

## WiFi Module Setup

**Hardware Connection:**
- ESP8266 VCC → 3.3V (NOT 5V!)
- ESP8266 GND → GND
- ESP8266 TX → Arduino Pin 0 (RX)
- ESP8266 RX → Arduino Pin 1 (TX)
- ESP8266 CH_PD → 3.3V

**IMPORTANT:** When uploading code to Arduino, disconnect ESP8266 TX/RX wires first!

**Software Configuration:**
Edit `esp8266_webserver.ino` lines 10-11:
```cpp
const char* ssid = "YourWiFiSSID";
const char* password = "YourPassword";
```

**Note:** ESP8266 only supports 2.4GHz WiFi networks, NOT 5GHz!

**Usage:**
1. Power on Arduino (starts in Remote mode, LED off)
2. Press START button on PS2 controller
3. LED blinks 3 times, then starts blinking every 1 second (WiFi mode active)
4. Wait ~10 seconds for ESP8266 to connect to WiFi
5. Check your router for ESP8266 IP address (or use serial monitor at 115200 baud)
6. Open browser to ESP8266 IP address (e.g., http://192.168.43.40)
7. Control via web page buttons or keyboard (W/A/S/D)
8. Click "退出WiFi模式" button or press Q to exit WiFi mode

## Important Constants

- `UNIT_PWM`: Servo increment per cycle (Rzhl_car.ino:11, default: 2)
- `PS2_LSPEED`: Max motor speed for left stick (Rzhl_car.ino:12, value: 1000)
- `PS2_RSPEED`: Max motor speed for right stick (Rzhl_car.ino:13, value: 1000)
- `RECIPROCAL`: 0.0078f (1/128 for joystick mapping, Rzhl_car.ino:14)
- `HIGH_TIM`: PS2 SPI timing delay (ps2.cpp:10, value: 50)
- `wifi_speed`: WiFi control speed (Rzhl_car.ino:71, value: 600)
- `WIFI_TIMEOUT_TICKS`: Auto-stop timeout (Rzhl_car.ino:31, value: 10 = 200ms)

## Serial Communication Notes

**IMPORTANT:** When using hardware Serial (pins 0/1) for ESP8266:
- Serial debugging is NOT available
- Must disconnect ESP8266 TX/RX before uploading code
- Serial monitor cannot be used simultaneously with WiFi control
- All debug Serial.print() statements are removed from WiFi mode code

## I2C Communication Protocol

All Rzhl commands follow this pattern:
1. Send command byte via Rzhl_WriteCommand()
2. For data commands: Start → Address → Command → Data bytes → Stop
3. Motor PWM: 4 bytes (motor_a high/low, motor_b high/low)
4. Servo PWM: 2 bytes (pwm high/low)

Motor direction: motor_a = reverse, motor_b = forward (0-1000 range)

## WiFi Control Implementation Details

**Communication Protocol:**
- ESP8266 runs standalone web server (esp8266_webserver.ino)
- Web page sends single character commands via HTTP GET: `/cmd?key=w`
- ESP8266 forwards command to Arduino via Serial.write()
- Arduino responds with echo character
- ESP8266 displays "Sent: w | OK" or "Sent: w | No ACK"

**Auto-Stop Mechanism:**
- Each command resets WiFi_Timeout_Counter to 0
- Counter increments every 20ms in timer interrupt
- After 10 ticks (200ms) without new command, motors stop automatically
- Prevents runaway if WiFi connection drops

**Command Processing:**
- Commands processed in main loop: ESP8266_Process() reads serial
- Motor targets set in ProcessWiFiControl()
- Actual motor control happens in timer interrupt (every 40ms)
- Serial buffer cleared when entering WiFi mode to prevent stale commands
