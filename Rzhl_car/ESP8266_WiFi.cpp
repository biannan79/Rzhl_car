#include "ESP8266_WiFi.h"
#include "WiFi_Config.h"
#include <Arduino.h>

// ESP8266-01S连接到Arduino的硬件串口
// Arduino UNO Pin 0 (RX) → ESP8266 TX
// Arduino UNO Pin 1 (TX) → ESP8266 RX
// 使用硬件串口Serial1，如果是UNO则使用Serial

// 注意：使用硬件串口时，上传代码前需要断开ESP8266，上传后再连接
// 或者在setup()中延迟初始化ESP8266

uint8_t wifi_connected = 0;
uint8_t client_connected = 0;
char last_command = 0;

// 使用硬件串口Serial（Arduino UNO只有一个硬件串口）
// 注意：这会占用USB串口调试功能
#define espSerial Serial

/**
 * @brief  发送AT命令到ESP8266
 * @param  cmd: AT命令字符串
 * @param  timeout: 超时时间(ms)
 * @retval 1=成功, 0=失败
 */
uint8_t ESP8266_SendCommand(const char* cmd, uint16_t timeout) {
  espSerial.println(cmd);

  unsigned long start = millis();
  String response = "";

  while (millis() - start < timeout) {
    while (espSerial.available()) {
      char c = espSerial.read();
      response += c;

      if (response.indexOf("OK") != -1) {
        return 1;
      }
      if (response.indexOf("ERROR") != -1) {
        return 0;
      }
    }
  }
  return 0;
}

/**
 * @brief  ESP8266初始化
 * @param  无
 * @retval 无
 */
void ESP8266_Init(void) {
  // 尝试不同的波特率
  uint32_t baudRates[] = {115200, 9600, 57600, 74880};
  bool espFound = false;

  for (int i = 0; i < 4; i++) {
    espSerial.begin(baudRates[i]);
    delay(100);

    // 测试AT命令
    if (ESP8266_SendCommand("AT", 1000)) {
      espFound = true;
      break;
    }
  }

  if (!espFound) {
    // ESP8266 not responding at any baud rate
    // 闪烁LED 5次表示ESP8266无响应
    for (int i = 0; i < 5; i++) {
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
    }
    return;
  }

  // 设置WiFi模式为Station模式
  ESP8266_SendCommand("AT+CWMODE=1", 2000);
  delay(100);

  // 连接WiFi
  String connectCmd = "AT+CWJAP=\"";
  connectCmd += WIFI_SSID;
  connectCmd += "\",\"";
  connectCmd += WIFI_PASSWORD;
  connectCmd += "\"";

  if (ESP8266_SendCommand(connectCmd.c_str(), 15000)) {
    wifi_connected = 1;
    // 闪烁LED 2次表示WiFi连接成功
    for (int i = 0; i < 2; i++) {
      digitalWrite(13, HIGH);
      delay(200);
      digitalWrite(13, LOW);
      delay(200);
    }
  } else {
    // 闪烁LED 4次表示WiFi连接失败
    for (int i = 0; i < 4; i++) {
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
    }
    return;
  }

  // 启用多连接
  ESP8266_SendCommand("AT+CIPMUX=1", 2000);
  delay(100);

  // 启动TCP服务器
  String serverCmd = "AT+CIPSERVER=1,";
  serverCmd += String(SERVER_PORT);

  if (ESP8266_SendCommand(serverCmd.c_str(), 2000)) {
    // 获取IP地址
    espSerial.println("AT+CIFSR");
    delay(1000);
    while (espSerial.available()) {
      espSerial.read(); // 清空缓冲区
    }
  }
}

/**
 * @brief  处理ESP8266接收的数据
 * @param  无
 * @retval 无
 */
void ESP8266_Process(void) {
  static String buffer = "";

  while (espSerial.available()) {
    char c = espSerial.read();
    buffer += c;

    // 检测到新连接
    if (buffer.indexOf("CONNECT") != -1) {
      client_connected = 1;
      buffer = "";
    }

    // 检测到断开连接
    if (buffer.indexOf("CLOSED") != -1) {
      client_connected = 0;
      buffer = "";
    }

    // 处理HTTP请求
    if (buffer.indexOf("+IPD") != -1) {
      // 查找GET请求
      int cmdPos = buffer.indexOf("GET /cmd?key=");
      if (cmdPos != -1) {
        // 提取命令字符
        char cmd = buffer.charAt(cmdPos + 13);
        last_command = cmd;

        // 发送HTTP响应
        String response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/plain\r\n";
        response += "Access-Control-Allow-Origin: *\r\n";
        response += "Connection: close\r\n\r\n";
        response += "OK";

        String sendCmd = "AT+CIPSEND=0,";
        sendCmd += String(response.length());
        espSerial.println(sendCmd);
        delay(50);
        espSerial.print(response);
        delay(100);
        espSerial.println("AT+CIPCLOSE=0");

        buffer = "";
      }
      // 处理主页请求
      else if (buffer.indexOf("GET / ") != -1 || buffer.indexOf("GET /index") != -1) {
        // 发送HTML控制页面
        String html = "HTTP/1.1 200 OK\r\n";
        html += "Content-Type: text/html\r\n";
        html += "Connection: close\r\n\r\n";
        html += "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
        html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
        html += "<title>Car Control</title>";
        html += "<style>body{font-family:Arial;text-align:center;margin:20px;}";
        html += ".btn{width:80px;height:80px;font-size:24px;margin:5px;border:2px solid #333;";
        html += "border-radius:10px;background:#4CAF50;color:white;cursor:pointer;}";
        html += ".btn:active{background:#45a049;}</style></head><body>";
        html += "<h1>NeZha Car Control</h1>";
        html += "<div><button class='btn' onclick='send(\"w\")'>↑</button></div>";
        html += "<div><button class='btn' onclick='send(\"a\")'>←</button>";
        html += "<button class='btn' onclick='send(\"x\")'>■</button>";
        html += "<button class='btn' onclick='send(\"d\")'>→</button></div>";
        html += "<div><button class='btn' onclick='send(\"s\")'>↓</button></div>";
        html += "<p id='status'>Ready</p>";
        html += "<script>function send(k){fetch('/cmd?key='+k).then(r=>r.text())";
        html += ".then(d=>{document.getElementById('status').innerText='Command: '+k;})";
        html += ".catch(e=>console.error(e));}";
        html += "document.addEventListener('keydown',function(e){";
        html += "if(e.key=='ArrowUp'||e.key=='w')send('w');";
        html += "else if(e.key=='ArrowDown'||e.key=='s')send('s');";
        html += "else if(e.key=='ArrowLeft'||e.key=='a')send('a');";
        html += "else if(e.key=='ArrowRight'||e.key=='d')send('d');";
        html += "else if(e.key==' ')send('x');});</script></body></html>";

        String sendCmd = "AT+CIPSEND=0,";
        sendCmd += String(html.length());
        espSerial.println(sendCmd);
        delay(100);
        espSerial.print(html);
        delay(200);
        espSerial.println("AT+CIPCLOSE=0");

        buffer = "";
      }
    }

    // 限制缓冲区大小
    if (buffer.length() > 500) {
      buffer = buffer.substring(buffer.length() - 200);
    }
  }
}

/**
 * @brief  获取WiFi接收到的命令
 * @param  无
 * @retval 命令字符，0表示无命令
 */
uint8_t ESP8266_GetCommand(void) {
  uint8_t cmd = last_command;
  last_command = 0;  // 清除命令
  return cmd;
}

/**
 * @brief  发送状态信息到客户端
 * @param  status: 状态字符串
 * @retval 无
 */
void ESP8266_SendStatus(const char* status) {
  if (!client_connected) return;

  String response = "STATUS:";
  response += status;

  String sendCmd = "AT+CIPSEND=0,";
  sendCmd += String(response.length());
  espSerial.println(sendCmd);
  delay(50);
  espSerial.print(response);
}

/**
 * @brief  检查WiFi是否连接
 * @param  无
 * @retval 1=已连接, 0=未连接
 */
uint8_t ESP8266_IsConnected(void) {
  return wifi_connected;
}
