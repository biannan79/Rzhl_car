/*
 * ESP8266 Web Server for Rzhl Car Control
 * 连接WiFi后创建Web服务器，接收控制命令并通过串口发送给Arduino
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi配置
const char* ssid = "CET-AL00%6307%CloudClone";
const char* password = "80275924";

// 创建Web服务器，端口80
ESP8266WebServer server(80);

// HTML控制页面
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Rzhl Car Control</title>
  <style>
    body { font-family: Arial; text-align: center; margin: 20px; }
    .btn {
      width: 80px; height: 80px; font-size: 20px; margin: 5px;
      border: 2px solid #333; border-radius: 10px;
      background: #4CAF50; color: white; cursor: pointer;
    }
    .btn:active { background: #45a049; }
    .stop { background: #f44336; }
    .exit { background: #ff9800; width: 200px; height: 50px; margin-top: 20px; }
    .control-pad { display: inline-block; }
  </style>
</head>
<body>
  <h1>Rzhl Smart Car</h1>
  <p>IP: <span id="ip"></span></p>
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
  <script>
    document.getElementById('ip').innerText = window.location.hostname;

    function send(cmd) {
      fetch('/cmd?key=' + cmd)
        .then(r => r.text())
        .then(d => {
          document.getElementById('status').innerText = d;
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

    // 单次点击模式：每次点击只发送一次命令
    document.querySelectorAll('.btn:not(.exit)').forEach(btn => {
      btn.addEventListener('click', function(e) {
        const match = this.getAttribute('onclick').match(/'(\w)'/);
        if (match) {
          send(match[1]);
        }
        e.preventDefault();
      });
    });

    // 键盘控制：每次按键只发送一次
    let pressedKeys = new Set();
    document.addEventListener('keydown', function(e) {
      if (e.repeat) return;  // 忽略长按重复
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
  </script>
</body>
</html>
)rawliteral";

void setup() {
  // 初始化串口（与Arduino通信）
  Serial.begin(115200);

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
      // 发送单个字符命令到Arduino（不带换行符）
      if (cmd.length() > 0) {
        Serial.write(cmd.charAt(0));

        // 等待Arduino回复
        delay(100);
        String response = "Sent: " + cmd;

        // 读取Arduino的回复（Arduino只回传命令字符）
        if (Serial.available()) {
          response += " | OK";
          // 清空缓冲区
          while (Serial.available()) {
            Serial.read();
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

  // 启动服务器
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}
