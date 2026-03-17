# Rzhl智能小车 ESP32-S3-CAM 视频控制使用说明

## 硬件连接

### ESP32-S3-CAM 接线
```
ESP32-S3 VCC  → 5V独立电源（不要与Arduino共用）
ESP32-S3 GND  → Arduino GND（共地）
ESP32-S3 TX (GPIO1)  → Arduino Pin 0 (RX)
ESP32-S3 RX (GPIO2)  → Arduino Pin 1 (TX)
```

**重要注意事项：**
- ESP32-S3-CAM需要独立5V供电（电流至少500mA）
- 不要与Arduino共用电源，避免电流不足导致重启
- 上传Arduino代码时必须断开ESP32的TX和RX连接线
- ESP32-S3只支持2.4GHz WiFi，不支持5GHz

### 硬件型号
本程序适用于：
- HiWonder ESP32-S3-CAM（基于AI-Thinker设计）
- 其他使用ESP32S3_EYE引脚配置的模块

## 软件配置

### 1. Arduino IDE设置（重要！）

**必须使用以下配置，否则会出现内存分配失败：**

```
板型：ESP32S3 Dev Module
USB CDC On Boot: Disabled
CPU Frequency: 240MHz (WiFi)
Core Debug Level: Info
USB DFU On Boot: Disabled
Erase All Flash Before Sketch Upload: Disabled
Events Run On: Core 1
Flash Mode: QIO 80MHz
Flash Size: 8MB (64Mb)
JTAG Adapter: Disabled
Arduino Runs On: Core 1
USB Firmware MSC On Boot: Disabled
Partition Scheme: Huge APP (3MB No OTA/1MB SPIFFS)  ← 关键设置！
PSRAM: OPI PSRAM  ← 必须启用！
Upload Mode: UART0 / Hardware CDC
Upload Speed: 921600
USB Mode: Hardware CDC and JTAG
```

**特别注意：**
- `Partition Scheme` 必须选择 "Huge APP (3MB No OTA/1MB SPIFFS)"
- `PSRAM` 必须选择 "OPI PSRAM"
- 如果选错分区方案，会出现 "frame buffer malloc failed" 错误

### 2. 配置WiFi信息

编辑 `esp32s3_cam.ino` 文件的第10-11行：

```cpp
const char* ssid = "你的WiFi名称";
const char* password = "你的WiFi密码";
```

### 3. 上传代码

**步骤：**
1. 断开ESP32-S3的TX/RX连接线
2. 打开Arduino IDE，按上述配置选择板子和参数
3. 上传 `esp32s3_cam.ino` 到ESP32-S3-CAM
4. 打开串口监视器（115200波特率），查看IP地址
5. 打开Arduino IDE，选择板子 "Arduino UNO"
6. 上传 `Rzhl_car.ino` 到Arduino UNO
7. 重新连接ESP32-S3的TX/RX线

## 使用方法

### 进入WiFi模式

1. 给小车上电（默认进入遥控模式，LED灯熄灭）
2. 按下PS2手柄的 **START** 键
3. 小车灯光闪烁3次，然后LED开始1秒闪烁一次（表示进入WiFi模式）
4. 等待约10秒，ESP32-S3连接到WiFi

### 查找小车IP地址

**方法1：串口监视器（推荐）**
- 在上传ESP32-S3代码后，保持USB连接
- 打开串口监视器（115200波特率）
- 按复位按钮，查看输出的IP地址
- 示例输出：
  ```
  === ESP32-S3-CAM Starting ===
  HiWonder ESP32-S3-CAM (AI-Thinker)
  Serial2 initialized for Arduino communication
  Initializing camera...
  Camera initialized
  Connecting to WiFi...
  ...
  WiFi Connected!
  IP Address: 192.168.1.100
  Web server started
  ```

**方法2：查看路由器**
- 登录你的WiFi路由器管理页面
- 查看已连接设备列表
- 找到ESP32-S3的IP地址

### 控制小车（带视频流）

1. 在手机或电脑浏览器中输入ESP32-S3的IP地址
   - 例如：`http://192.168.1.100`

2. 打开控制页面后，可以看到：
   - **实时视频流**：显示摄像头画面（240x240分辨率）
   - **控制按钮**：点击方向按钮控制
   - **键盘控制**：
     - W 或 ↑ ：前进
     - S 或 ↓ ：后退
     - A 或 ← ：左转
     - D 或 → ：右转
     - X 或 空格：停止
     - Q：退出WiFi模式

3. 每次点击或按键，小车会移动约200毫秒后自动停止

### 退出WiFi模式

**方法1：网页按钮**
- 点击页面上的"退出WiFi模式"按钮

**方法2：键盘**
- 按 Q 键

**方法3：PS2手柄**
- 按 START 键

退出后，小车灯光闪烁2次，LED熄灭，返回遥控模式。

## 控制特性

### 视频流
- 分辨率：240x240像素
- 格式：RGB565转JPEG
- 帧率：约10-15fps（取决于WiFi速度）
- 延迟：< 200ms

### 单次点击模式
- 每次点击按钮或按键，小车只移动约200ms
- 如需持续移动，需要连续点击
- 200ms内无新命令，小车自动停止

### 自动停止保护
- 如果WiFi连接断开或网页关闭
- 小车会在200ms后自动停止
- 防止失控

## 状态指示

### LED灯状态
- **熄灭**：遥控模式
- **1秒闪烁**：WiFi模式
- **常亮**：巡线模式

### 尾灯状态
- **熄灭**：停止
- **两个都亮**：后退
- **左侧闪烁**：左转
- **右侧闪烁**：右转

## 常见问题

### Q1: 编译错误 "frame buffer malloc failed"
**原因：** Arduino IDE分区方案设置错误

**解决方法：**
1. 工具 → Partition Scheme → 选择 "Huge APP (3MB No OTA/1MB SPIFFS)"
2. 工具 → PSRAM → 选择 "OPI PSRAM"
3. 重新编译上传

### Q2: 串口显示乱码
**原因：** USB CDC设置错误或波特率不匹配

**解决方法：**
1. 工具 → USB CDC On Boot → 选择 "Disabled"
2. 串口监视器波特率设置为 115200
3. 重新上传代码

### Q3: 摄像头初始化失败 "Camera init failed"
**原因：** 引脚配置不匹配或硬件问题

**解决方法：**
1. 确认你的模块型号是否为HiWonder ESP32-S3-CAM
2. 检查摄像头排线是否插好
3. 尝试按复位按钮重启

### Q4: 网页打不开
- 检查手机/电脑是否连接到同一个WiFi
- 确认IP地址是否正确
- 等待ESP32-S3完全连接到WiFi（约10秒）
- 检查路由器是否开启了AP隔离

### Q5: 视频流不显示
- 刷新浏览器页面
- 检查摄像头是否正常初始化（查看串口输出）
- 尝试降低分辨率（修改代码中的FRAMESIZE）

### Q6: 小车不动
- 确认已按START键进入WiFi模式（LED闪烁）
- 检查网页是否显示 "Sent: w | OK"
- 如果显示 "No ACK"，说明Arduino没有响应，检查连线
- 确认ESP32-S3的TX/RX已正确连接到Arduino

### Q7: 无法上传代码
- 上传前必须断开ESP32-S3的TX/RX连接线
- 确认选择了正确的板子类型和配置
- 检查USB连接是否正常
- 尝试按住BOOT按钮再上传

### Q8: ESP32-S3频繁重启
- 检查供电是否充足（至少500mA）
- 使用独立电源供电，不要与Arduino共用
- 检查电源线是否过细导致压降

## 技术参数

### 硬件参数
- 芯片：ESP32-S3
- PSRAM：8MB OPI PSRAM
- Flash：8MB
- 摄像头：OV2640或兼容型号
- 工作电压：5V
- 工作电流：300-500mA

### 软件参数
- WiFi频段：2.4GHz
- 通信波特率：115200
- 视频分辨率：240x240
- 视频格式：RGB565 → JPEG
- 控制延迟：< 100ms
- 自动停止时间：200ms
- 电机速度：600 (范围0-1000)
- 网页端口：80

### 引脚定义
```
摄像头引脚：
XCLK:  GPIO15    PCLK:  GPIO13
VSYNC: GPIO6     HREF:  GPIO7
SIOD:  GPIO4     SIOC:  GPIO5
Y9:    GPIO16    Y8:    GPIO17
Y7:    GPIO18    Y6:    GPIO12
Y5:    GPIO10    Y4:    GPIO8
Y3:    GPIO9     Y2:    GPIO11

串口引脚：
TX: GPIO1 (连接Arduino RX)
RX: GPIO2 (连接Arduino TX)
```

## 性能优化建议

### 提高视频帧率
1. 使用更快的WiFi路由器（WiFi 6）
2. 减少WiFi干扰源
3. 缩短ESP32-S3与路由器的距离

### 降低延迟
1. 使用5GHz频段的双频路由器（ESP32连2.4GHz，手机连5GHz）
2. 减少网络中的其他设备
3. 使用有线连接的电脑控制

### 节省内存
如果遇到内存不足，可以：
1. 减小视频分辨率（修改FRAMESIZE_240X240为FRAMESIZE_QQVGA）
2. 降低JPEG质量（增大jpeg_quality值）
3. 减少fb_count（已设为1）

## 安全提示

1. 首次使用建议在空旷场地测试
2. 确保WiFi信号稳定，避免失控
3. 注意小车移动范围，避免碰撞
4. 不要在WiFi模式下长时间不操作（会自动停止）
5. 如遇异常，立即按X键或断电
6. ESP32-S3发热正常，但如果过热请检查供电和散热

## 与ESP8266版本的区别

| 特性 | ESP8266-01S | ESP32-S3-CAM |
|------|-------------|--------------|
| 视频流 | ❌ 无 | ✅ 有（240x240） |
| 处理器 | 单核80MHz | 双核240MHz |
| 内存 | 80KB | 512KB + 8MB PSRAM |
| 供电 | 3.3V | 5V |
| 配置复杂度 | 简单 | 中等 |
| 成本 | 低 | 中 |
| 推荐场景 | 简单遥控 | 视频监控+遥控 |

## 故障排查流程

```
1. 检查Arduino IDE配置
   ├─ Partition Scheme = Huge APP?
   ├─ PSRAM = OPI PSRAM?
   └─ USB CDC On Boot = Disabled?

2. 检查硬件连接
   ├─ ESP32供电是否充足（5V/500mA）?
   ├─ TX/RX是否交叉连接?
   └─ 摄像头排线是否插好?

3. 检查串口输出
   ├─ 是否显示IP地址?
   ├─ 是否有错误信息?
   └─ 摄像头是否初始化成功?

4. 检查网络连接
   ├─ WiFi是否2.4GHz?
   ├─ 手机/电脑是否在同一网络?
   └─ 路由器是否开启AP隔离?

5. 测试功能
   ├─ 网页能否打开?
   ├─ 视频流是否显示?
   ├─ 控制命令是否响应?
   └─ 小车是否移动?
```
