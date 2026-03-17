/*
 * ESP32-S3-CAM Web Server for Rzhl Car Control
 * 提供视频流和控制命令发送功能
 * 通过串口与Arduino通信
 */

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// WiFi配置
const char* ssid = "CET-AL00%6307%CloudClone";
const char* password = "80275924";

// 创建Web服务器，端口80
WebServer server(80);

// HiWonder ESP32-S3-CAM引脚定义（ESP32S3_EYE配置）
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     15
#define SIOD_GPIO_NUM     4
#define SIOC_GPIO_NUM     5

#define Y9_GPIO_NUM       16
#define Y8_GPIO_NUM       17
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       12
#define Y5_GPIO_NUM       10
#define Y4_GPIO_NUM       8
#define Y3_GPIO_NUM       9
#define Y2_GPIO_NUM       11
#define VSYNC_GPIO_NUM    6
#define HREF_GPIO_NUM     7
#define PCLK_GPIO_NUM     13

#define XCLK_FREQ_HZ      20000000

// 串口配置（与Arduino通信）
// 使用GPIO1和GPIO2（避免与USB冲突）
#define RXD2 2   // ESP32 RX <- Arduino TX
#define TXD2 1   // ESP32 TX -> Arduino RX

// HTML控制页面（带视频流）
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Rzhl Car Control with Camera</title>
  <style>
    body {
      font-family: Arial;
      text-align: center;
      margin: 0;
      padding: 10px;
      background: #1a1a1a;
      color: white;
    }
    .container {
      max-width: 800px;
      margin: 0 auto;
    }
    h1 { margin: 10px 0; }
    .video-container {
      width: 100%;
      max-width: 640px;
      margin: 10px auto;
      border: 2px solid #333;
      border-radius: 10px;
      overflow: hidden;
      background: #000;
    }
    #stream {
      width: 100%;
      height: auto;
      display: block;
    }
    .info {
      margin: 10px 0;
      font-size: 14px;
      color: #888;
    }
    .btn {
      width: 80px;
      height: 80px;
      font-size: 20px;
      margin: 5px;
      border: 2px solid #333;
      border-radius: 10px;
      background: #4CAF50;
      color: white;
      cursor: pointer;
      user-select: none;
    }
    .btn:active { background: #45a049; }
    .stop { background: #f44336; }
    .exit {
      background: #ff9800;
      width: 200px;
      height: 50px;
      margin-top: 20px;
    }
    .control-pad {
      display: inline-block;
      margin: 10px 0;
    }
    #status {
      margin: 10px 0;
      padding: 5px;
      background: #333;
      border-radius: 5px;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Rzhl Smart Car</h1>
    <div class="info">IP: <span id="ip"></span></div>

    <div class="video-container">
      <img id="stream" src="/stream">
    </div>

    <p id="status">Ready</p>

    <div class="control-pad">
      <div><button class="btn" onclick="send('w')">↑<br>W</button></div>
      <div>
        <button class="btn" onclick="send('a')">←<br>A</button>
        <button class="btn stop" onclick="send('x')">■<br>X</button>
        <button class="btn" onclick="send('d')">→<br>D</button>
      </div>
      <div><button class="btn" onclick="send('s')">↓<br>S</button></div>
    </div>

    <div>
      <button class="btn exit" onclick="exitWiFi()">退出WiFi模式 (Q)</button>
    </div>
  </div>

  <script>
    document.getElementById('ip').innerText = window.location.hostname;

    function send(cmd) {
      fetch('/cmd?key=' + cmd)
        .then(r => r.text())
        .then(d => {
          document.getElementById('status').innerText = d;
        })
        .catch(e => {
          document.getElementById('status').innerText = 'Error: ' + e;
        });
    }

    function exitWiFi() {
      if (confirm('确定要退出WiFi模式吗？小车将返回遥控模式。')) {
        send('q');
        setTimeout(() => {
          document.getElementById('status').innerText = '已退出WiFi模式，请使用PS2手柄控制';
        }, 500);
      }
    }

    // 单次点击模式
    document.querySelectorAll('.btn:not(.exit)').forEach(btn => {
      btn.addEventListener('click', function(e) {
        const match = this.getAttribute('onclick').match(/'(\w)'/);
        if (match) {
          send(match[1]);
        }
        e.preventDefault();
      });
    });

    // 键盘控制
    let pressedKeys = new Set();
    document.addEventListener('keydown', function(e) {
      if (e.repeat) return;
      const key = e.key.toLowerCase();
      let cmd = null;
      if(key === 'w' || e.key === 'ArrowUp') cmd = 'w';
      else if(key === 'a' || e.key === 'ArrowLeft') cmd = 'a';
      else if(key === 's' || e.key === 'ArrowDown') cmd = 's';
      else if(key === 'd' || e.key === 'ArrowRight') cmd = 'd';
      else if(key === 'x' || e.key === ' ') cmd = 'x';
      else if(key === 'q') {
        exitWiFi();
        return;
      }

      if (cmd && !pressedKeys.has(key)) {
        pressedKeys.add(key);
        send(cmd);
        e.preventDefault();
      }
    });

    document.addEventListener('keyup', function(e) {
      const key = e.key.toLowerCase();
      pressedKeys.delete(key);
    });

    // 视频流错误处理
    document.getElementById('stream').onerror = function() {
      this.src = '/stream?' + new Date().getTime();
    };
  </script>
</body>
</html>
)rawliteral";

void setup() {
  // 初始化串口（调试用）
  Serial.begin(115200);
  delay(1000);  // 等待串口稳定

  Serial.println("\n\n=== ESP32-S3-CAM Starting ===");
  Serial.println("HiWonder ESP32-S3-CAM (AI-Thinker)");

  // 初始化串口2（与Arduino通信）
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial2 initialized for Arduino communication");

  // 初始化摄像头
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = XCLK_FREQ_HZ;
  config.pixel_format = PIXFORMAT_RGB565;  // 使用RGB565格式（与ColorDetection一致）
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.frame_size = FRAMESIZE_240X240;  // 240x240（与ColorDetection一致）
  config.jpeg_quality = 12;
  config.fb_count = 1;  // 只用1个缓冲区，节省内存

  Serial.println("Initializing camera...");

  // 初始化摄像头
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }
  Serial.println("Camera initialized");

  // 连接WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // 配置Web服务器路由
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", htmlPage);
  });

  server.on("/cmd", HTTP_GET, []() {
    if (server.hasArg("key")) {
      String cmd = server.arg("key");
      if (cmd.length() > 0) {
        char c = cmd.charAt(0);
        // 发送命令到Arduino
        Serial2.write(c);

        delay(100);
        String response = "Sent: " + cmd;

        // 读取Arduino回复
        if (Serial2.available()) {
          response += " | OK";
          while (Serial2.available()) {
            Serial2.read();
          }
        } else {
          response += " | No ACK";
        }

        server.send(200, "text/plain", response);
      } else {
        server.send(400, "text/plain", "Empty command");
      }
    } else {
      server.send(400, "text/plain", "Missing key parameter");
    }
  });

  // 视频流路由
  server.on("/stream", HTTP_GET, []() {
    WiFiClient client = server.client();

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: multipart/x-mixed-replace; boundary=frame");
    client.println();

    while (client.connected()) {
      camera_fb_t * fb = esp_camera_fb_get();
      if (!fb) {
        Serial.println("Camera capture failed");
        break;
      }

      // RGB565需要转换为JPEG
      uint8_t * jpg_buf = NULL;
      size_t jpg_len = 0;
      bool converted = false;

      if(fb->format != PIXFORMAT_JPEG) {
        converted = frame2jpg(fb, 80, &jpg_buf, &jpg_len);
        if(!converted) {
          Serial.println("JPEG compression failed");
          esp_camera_fb_return(fb);
          break;
        }
      } else {
        jpg_buf = fb->buf;
        jpg_len = fb->len;
      }

      client.print("--frame\r\n");
      client.print("Content-Type: image/jpeg\r\n");
      client.print("Content-Length: ");
      client.print(jpg_len);
      client.print("\r\n\r\n");
      client.write(jpg_buf, jpg_len);
      client.print("\r\n");

      if(converted) {
        free(jpg_buf);
      }
      esp_camera_fb_return(fb);

      if (!client.connected()) break;
    }
  });

  // 启动服务器
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();

  // 转发Arduino发来的数据到调试串口
  if (Serial2.available()) {
    Serial.write(Serial2.read());
  }
}
