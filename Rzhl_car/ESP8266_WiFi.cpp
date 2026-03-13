#include "ESP8266_WiFi.h"
#include <Arduino.h>

// ESP8266-01S连接到Arduino的硬件串口
// ESP8266 TX → Arduino Pin 0 (RX)
// ESP8266 RX → Arduino Pin 1 (TX)

uint8_t wifi_connected = 1;  // 假设ESP8266已经连接WiFi
uint8_t client_connected = 0;
char last_command = 0;

// 使用硬件串口Serial
#define espSerial Serial

/**
 * @brief  ESP8266初始化（简化版，ESP8266已运行Web服务器）
 * @param  无
 * @retval 无
 */
void ESP8266_Init(void) {
  // 初始化串口，波特率115200（与ESP8266 Web服务器匹配）
  espSerial.begin(115200);
  delay(100);

  // ESP8266已经运行Web服务器，无需AT命令初始化
  wifi_connected = 1;
}

/**
 * @brief  处理ESP8266接收的数据
 * @param  无
 * @retval 无
 */
void ESP8266_Process(void) {
  // 从ESP8266读取命令（ESP8266 Web服务器会发送单个字符命令）
  while (espSerial.available()) {
    char c = espSerial.read();

    // 检查是否是有效命令
    if (c == 'w' || c == 'a' || c == 's' || c == 'd' || c == 'x' || c == 'q') {
      last_command = c;
      // 回复确认信息给ESP8266（简短响应）
      espSerial.write(c);  // 只回传命令字符
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
