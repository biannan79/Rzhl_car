#ifndef ESP8266_WIFI_H
#define ESP8266_WIFI_H

#include <stdint.h>

// WiFi模式定义
#define WIFI_MODE_OFF       0  // WiFi关闭
#define WIFI_MODE_ACTIVE    1  // WiFi激活

// 键盘命令定义
#define CMD_FORWARD     'w'
#define CMD_BACKWARD    's'
#define CMD_LEFT        'a'
#define CMD_RIGHT       'd'
#define CMD_STOP        'x'

void ESP8266_Init(void);
void ESP8266_Process(void);
uint8_t ESP8266_GetCommand(void);
void ESP8266_SendStatus(const char* status);
uint8_t ESP8266_IsConnected(void);

#endif
