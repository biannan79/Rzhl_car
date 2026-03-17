# Rzhl智能小车 - 遥控与巡线整合版 V2.0

## 项目简介

本项目整合了遥控控制、自动巡线、WiFi远程控制三大功能，通过PS2手柄按键可以在三种模式之间自由切换。支持ESP32-S3-CAM实时视频流控制。

## 功能特性

### 1. 遥控模式（默认模式）
- 支持PS2手柄红灯模式（摇杆控制）和绿灯模式（按键控制）
- 麦轮全向移动控制
- 机械臂控制（按住L1键）
- 智能尾灯提示（倒车灯、转向灯）

### 2. 巡线模式
- 基于4个光电传感器的自动循迹
- 7种路况识别（直线、左转、右转、大弯、小弯等）
- 智能转向灯提示

### 3. WiFi控制模式（新增）
- 支持ESP8266-01S（简单控制）和ESP32-S3-CAM（带视频流）
- 网页控制界面，支持键盘操作（W/A/S/D/X/Q）
- ESP32-S3-CAM提供240x240实时视频流
- 单次点击模式，200ms自动停止保护

### 4. 模式切换
- **按下SELECT键**：在遥控模式和巡线模式之间切换
  - 进入巡线：灯光闪烁1次，LED常亮
  - 退出巡线：灯光闪烁2次，LED熄灭
- **按下START键**：在遥控/巡线模式与WiFi模式之间切换
  - 进入WiFi：灯光闪烁3次，LED每秒闪烁
  - 退出WiFi：灯光闪烁2次，LED熄灭
- **LED指示**：
  - 遥控模式：LED熄灭
  - 巡线模式：LED常亮不闪
  - WiFi模式：LED每秒闪烁1次

## 硬件配置

### 传感器布局
- SENSOR0: 引脚2 (最左侧)
- SENSOR1: 引脚3 (左中)
- SENSOR2: 引脚4 (右中)
- SENSOR3: 引脚5 (最右侧)

### PS2手柄引脚
- PS2_DI: 引脚11
- PS2_DO: 引脚9
- PS2_CS: 引脚8
- PS2_SCK: 引脚10

### WiFi模块引脚（可选）

#### ESP8266-01S
- ESP8266 TX → Arduino Pin 3
- ESP8266 RX → Arduino Pin 2（需要电压分压到3.3V）
- ESP8266 VCC → 3.3V（不能接5V！）
- ESP8266 GND → Arduino GND
- ESP8266 CH_PD → 3.3V

#### ESP32-S3-CAM（推荐）
- ESP32-S3 TX (GPIO1) → Arduino Pin 0 (RX)
- ESP32-S3 RX (GPIO2) → Arduino Pin 1 (TX)
- ESP32-S3 VCC → 5V独立电源（至少500mA）
- ESP32-S3 GND → Arduino GND

**重要提示**：
- 使用WiFi模式时必须断开USB线（硬件串口冲突）
- ESP32-S3需要独立5V供电，不要与Arduino共用电源

## 使用说明

### 遥控模式操作

#### 红灯模式（摇杆控制）
- **左摇杆**：控制小车前后左右移动
- **右摇杆**：控制小车原地旋转
- **L1 + 摇杆**：控制机械臂
  - 左摇杆上下：机械臂抬起/放下
  - 右摇杆上下：机械臂伸展/收缩
  - 右摇杆左右：机械臂握手/松手

#### 绿灯模式（按键控制）
- **方向键**：控制小车移动
  - 上：前进
  - 下：后退
  - 左：左平移
  - 右：右平移
- **粉色/红色键**：控制旋转
  - 粉色：逆时针旋转
  - 红色：顺时针旋转
- **L1 + 按键**：控制机械臂
  - 上/下：机械臂抬起/放下
  - 绿色/蓝色：机械臂伸展/收缩
  - 粉色/红色：机械臂握手/松手

### 巡线模式操作

1. 将小车放置在黑色轨道线上
2. 按下SELECT键进入巡线模式（灯光闪烁1次，LED变为常亮）
3. 小车将自动沿黑线行驶
4. 尾灯会根据转向方向闪烁提示
5. 再次按下SELECT键退出巡线模式（灯光闪烁2次，小车停止，LED熄灭）

### WiFi控制模式操作

#### 硬件准备
选择以下任一WiFi模块：
- **ESP8266-01S**：简单控制（无视频）
- **ESP32-S3-CAM**：视频流+控制（推荐）

#### 软件配置
1. 打开对应的WiFi模块程序：
   - ESP8266: `Rzhl_connect/esp8266_webserver/esp8266_webserver.ino`
   - ESP32-S3: `Rzhl_connect/esp32s3_cam/esp32s3_cam.ino`
2. 修改WiFi配置（ssid和password）
3. 上传到WiFi模块

**ESP32-S3-CAM特别注意**：
- Arduino IDE必须配置正确的分区方案：
  - Partition Scheme: "Huge APP (3MB No OTA/1MB SPIFFS)"
  - PSRAM: "OPI PSRAM"
  - USB CDC On Boot: "Disabled"
- 详见：`Rzhl_connect/esp32s3_cam/ESP32-S3-CAM使用说明.md`

#### 使用步骤
1. 断开USB线，使用电池供电
2. 按下START键进入WiFi模式（灯光闪烁3次，LED开始每秒闪烁）
3. 等待5-10秒让WiFi模块连接WiFi
4. 在浏览器输入WiFi模块的IP地址
5. 使用网页按钮或键盘控制：
   - W或↑：前进
   - S或↓：后退
   - A或←：左转
   - D或→：右转
   - X或空格：停止
   - Q：退出WiFi模式
6. ESP32-S3-CAM可看到实时视频流（240x240）

#### 退出WiFi模式
- 网页按钮：点击"退出WiFi模式(Q)"
- 键盘：按Q键
- 手柄：按START键

退出后，灯光闪烁2次，小车停止，LED熄灭，返回遥控模式。

### 串口调试命令

连接串口监视器（9600波特率）后，可以使用以下命令：
- **0**：进入遥控模式
- **1**：进入巡线模式
- **w**：进入WiFi模式（需要WiFi模块）
- **q**：退出WiFi模式
- **s**：显示传感器状态
- **m**：显示当前模式
- **d**：开启/关闭调试模式（显示所有按键）

## 控制周期

- 定时器周期：20ms
- 遥控/巡线控制周期：20ms
- 电机控制周期：40ms
- 尾灯控制周期：200ms
- 指示灯闪烁周期：1000ms

## 文件说明

### 主程序
- `Rzhl_car.ino`: 主程序，包含三种模式切换和控制逻辑

### 硬件驱动
- `Rzhl.cpp/h`: Rzhl驱动板底层驱动
- `Rzhl_I2C.cpp/h`: I2C通信接口
- `Board_Timer.cpp/h`: 定时器配置

### 功能模块
- `ps2.cpp/h`: PS2手柄通信协议
- `RobotArm.cpp/h`: 机械臂控制
- `Sensor.cpp/h`: 光电传感器读取
- `Vehicle_Chassis.cpp/h`: 底盘运动控制
- `LED.cpp/h`: LED指示灯控制
- `ESP8266_WiFi.cpp/h`: WiFi模块驱动（可选）
- `type_servo.h`: 舵机参数结构体定义

### WiFi模块程序
- `Rzhl_connect/esp8266_webserver/`: ESP8266-01S程序和说明
  - `esp8266_webserver.ino`: ESP8266主程序
  - `WiFi控制说明.md`: 详细使用说明
- `Rzhl_connect/esp32s3_cam/`: ESP32-S3-CAM程序和说明
  - `esp32s3_cam.ino`: ESP32-S3-CAM主程序
  - `ESP32-S3-CAM使用说明.md`: 详细配置和使用说明
- `Rzhl_connect/ColorDetection/`: ESP32-S3-CAM颜色检测示例

### 文档
- `README.md`: 本文档，详细说明
- `更新说明.md`: 版本更新历史
- `测试指南.md`: 完整测试流程和故障排查
- `使用说明.txt`: 快速使用指南
- `CLAUDE.md`: 项目技术文档（供AI助手参考）

## 重要提示

### 关于Vehicle_Chassis函数
根据原inspect项目的README说明，`Vehicle_Chassis.cpp`中的函数命名与实际动作可能相反：
- `Vehicle_Chassis_Forward()` 实际可能是后退
- `Vehicle_Chassis_TurnLeft()` 实际可能是右转
- `Vehicle_Chassis_TurnRight()` 实际可能是左转

这可能是由于：
1. 函数命名错误
2. 电机接线接反

如果巡线时小车运动方向不正确，请检查电机接线或修改函数实现。

## 编译与上传

### Arduino主程序
1. 使用Arduino IDE打开`Rzhl_car.ino`
2. 选择开发板：Arduino UNO
3. 选择正确的端口
4. 点击上传按钮
5. 等待上传完成

### WiFi模块程序（可选）

#### ESP8266-01S
1. 打开`Rzhl_connect/esp8266_webserver/esp8266_webserver.ino`
2. 修改WiFi配置（第10-11行）
3. 选择开发板：Generic ESP8266 Module
4. 上传到ESP8266模块

#### ESP32-S3-CAM
1. 打开`Rzhl_connect/esp32s3_cam/esp32s3_cam.ino`
2. 修改WiFi配置（第12-13行）
3. 配置Arduino IDE（关键！）：
   - 板型：ESP32S3 Dev Module
   - Partition Scheme: **Huge APP (3MB No OTA/1MB SPIFFS)**
   - PSRAM: **OPI PSRAM**
   - USB CDC On Boot: Disabled
   - CPU Frequency: 240MHz (WiFi)
4. 上传到ESP32-S3-CAM模块
5. 打开串口监视器（115200波特率）查看IP地址

详细配置说明请查看对应的使用说明文档。

## 故障排除

### 巡线方向错误
- 检查传感器是否正确连接到引脚2-5
- 检查电机接线是否正确
- 可能需要修改`Vehicle_Chassis.cpp`中的函数实现

### 遥控无响应
- 检查PS2手柄是否正确连接
- 检查手柄电池是否有电
- 检查PS2引脚连接是否正确

### START/SELECT键无效
- **已修复**：原代码中`ps2.cpp`的`ps2_key_serch()`函数循环从索引4开始，导致SELECT、L3、R3、START这4个按键未被检测
- 修复方法：将`for(index=4; index<16; index++)`改为`for(index=0; index<16; index++)`
- 如果仍无效，使用串口命令`d`开启调试模式，查看实际按键编号

### USB供电时遥控器无法连接
- USB供电不足以驱动PS2接收器
- 使用电池供电即可正常连接
- 如需调试，可使用串口命令控制

### WiFi模式无法连接
- 确认已断开USB线（必须使用电池供电）
- 检查WiFi模块是否正确连接到Pin 0和Pin 1（ESP32-S3）或Pin 2和Pin 3（ESP8266）
- ESP8266必须使用3.3V供电，ESP32-S3必须使用5V独立供电
- 确认WiFi路由器是2.4GHz频段（不支持5GHz）
- ESP32-S3-CAM：检查Arduino IDE分区方案是否设置为"Huge APP"
- 详细故障排查请查看`测试指南.md`

## 版本信息

- 版本：V2.0
- 更新日期：2026-03-11
- 基于：remote项目 + inspect项目 + WiFi控制

### 更新历史
- **V2.0 (2026-03-11)**：新增WiFi控制模式，支持ESP32-S3-CAM视频流
- **V1.4 (2026-03-09)**：修复ps2.cpp中按键检测bug，START/SELECT键现已正常工作
- **V1.3 (2026-03-09)**：添加串口调试命令，支持无遥控器测试
- **V1.2 (2026-03-09)**：添加SELECT键备用切换，LED模式指示
- **V1.1 (2026-03-09)**：添加灯光提示和串口调试输出
- **V1.0 (2026-03-09)**：初始整合版本

## 作者

@zhl
