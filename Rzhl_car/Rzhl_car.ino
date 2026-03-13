#include "Rzhl.h"
#include <stdint.h>
#include "RobotArm.h"
#include "Board_Timer.h"
#include "ps2.h"
#include "LED.h"
#include "Sensor.h"
#include "Vehicle_Chassis.h"
#include "ESP8266_WiFi.h"

#define   UNIT_PWM	2				//舵机单位转动值
#define   PS2_LSPEED    1000		//左遥杆拨到底对应电机最大目标值（电机最大速度为1000）
#define   PS2_RSPEED    1000		//右遥杆拨到底对应电机最大目标值（电机最大速度为1000）
#define   RECIPROCAL	0.0078f     //128的倒数，此处为方便摇杆值映射目标值的计算。不可更改

// 模式定义
#define MODE_REMOTE     0  // 遥控模式
#define MODE_LINE_TRACK 1  // 巡线模式
#define MODE_WIFI       2  // WiFi控制模式

uint8_t KeyNum;
uint8_t PS2_Mode, Last_PS2_Mode;
int16_t M1_Target, M2_Target, M3_Target, M4_Target;
uint8_t Time;
uint8_t Led_State;  //车尾灯灯状态标志位； 0：熄灭， 1：后退， 2：左转弯灯， 3：右转弯灯

// 巡线相关变量
uint8_t SensorState[4];
uint8_t Area, Last_Area;

// WiFi控制超时变量
uint8_t WiFi_Timeout_Counter = 0;   // WiFi命令超时计数器（20ms为单位）
#define WIFI_TIMEOUT_TICKS 10       // 10个tick = 200ms无命令则停止

// 模式控制变量
uint8_t Current_Mode = MODE_REMOTE;      // 默认启动进入遥控模式
uint8_t Last_Start_State = 0;            // 上次START键状态，用于检测按键按下
uint8_t Last_Select_State = 0;           // 上次SELECT键状态
uint8_t Debug_Mode = 0;                  // 调试模式：0=正常，1=显示所有按键

// 灯光闪烁函数
void FlashAllLights(uint8_t times) {
  for (uint8_t i = 0; i < times; i++) {
    // 打开所有灯
    Rzhl_TailLeftLed_TurnOn();
    Rzhl_TailRightLed_TurnOn();
    Led_On();
    delay(200);

    // 关闭所有灯
    Rzhl_TailLeftLed_TurnOff();
    Rzhl_TailRightLed_TurnOff();
    Led_Off();
    delay(200);
  }
}

// LED模式指示（巡线模式时LED常亮）
void UpdateModeIndicator() {
  if (Current_Mode == MODE_LINE_TRACK) {
    Led_On();  // 巡线模式：LED常亮
  } else if (Current_Mode == MODE_WIFI) {
    // WiFi模式：LED快速闪烁（在主循环中处理）
  } else {
    // 遥控模式：LED按原来的1秒闪烁
  }
}

// WiFi控制处理函数
void ProcessWiFiControl() {
  uint8_t cmd = ESP8266_GetCommand();

  if (cmd != 0) {
    const int wifi_speed = 600;  // WiFi控制速度
    WiFi_Timeout_Counter = 0;    // 收到命令，重置超时计数器

    switch(cmd) {
      case CMD_FORWARD:  // 'w' - 前进
        M1_Target = -wifi_speed;
        M2_Target = wifi_speed;
        M3_Target = wifi_speed;
        M4_Target = -wifi_speed;
        Led_State = 0;
        break;

      case CMD_BACKWARD:  // 's' - 后退
        M1_Target = wifi_speed;
        M2_Target = -wifi_speed;
        M3_Target = -wifi_speed;
        M4_Target = wifi_speed;
        Led_State = 1;
        break;

      case CMD_LEFT:  // 'a' - 左转
        M1_Target = wifi_speed;
        M2_Target = wifi_speed;
        M3_Target = wifi_speed;
        M4_Target = wifi_speed;
        Led_State = 2;
        break;

      case CMD_RIGHT:  // 'd' - 右转
        M1_Target = -wifi_speed;
        M2_Target = -wifi_speed;
        M3_Target = -wifi_speed;
        M4_Target = -wifi_speed;
        Led_State = 3;
        break;

      case CMD_STOP:  // 'x' - 停止
        M1_Target = 0;
        M2_Target = 0;
        M3_Target = 0;
        M4_Target = 0;
        Led_State = 0;
        WiFi_Timeout_Counter = 0;
        break;

      case CMD_EXIT_WIFI:  // 'q' - 退出WiFi模式
        Current_Mode = MODE_REMOTE;
        M1_Target = 0;
        M2_Target = 0;
        M3_Target = 0;
        M4_Target = 0;
        Rzhl_Motor1_SetPwm(0, 0);
        Rzhl_Motor2_SetPwm(0, 0);
        Rzhl_Motor3_SetPwm(0, 0);
        Rzhl_Motor4_SetPwm(0, 0);
        Led_State = 0;
        WiFi_Timeout_Counter = 0;
        FlashAllLights(2);  // 闪烁2次表示退出WiFi模式
        break;

      default:
        break;
    }
  }
}

void setup() {
  // 注意：使用硬件串口连接ESP8266时，无法同时使用串口调试
  // 如果需要调试，请在setup()开始时延迟5秒，用于查看串口输出
  // Serial.begin(9600);
  // Serial.println("System Initialized!");
  // delay(5000);  // 延迟5秒后再初始化ESP8266

  PS2_Init();
  Rzhl_Init();
  Rzhl_Motor_Init();
  RobotArm_Init();
  Sensor_Init();           // 初始化传感器
  Vehicle_Chassis_Init();  // 初始化底盘
  Board_Timer_Init();
  Led_Init();

  // 提前初始化ESP8266串口（即使不使用WiFi模式也初始化）
  ESP8266_Init();

  // 启动时闪灯2次表示系统就绪，进入遥控模式
  FlashAllLights(2);
}

void loop() {
  // 注意：使用硬件串口连接ESP8266时，串口命令功能不可用
  // 所有模式切换通过PS2手柄完成：
  // - START键：切换WiFi模式
  // - SELECT键：切换巡线模式

  // WiFi模式下处理ESP8266通信
  if (Current_Mode == MODE_WIFI) {
    ESP8266_Process();
    ProcessWiFiControl();
  }

  if (Board_Timer_Flag_Get())    //定时器定时20ms周期
  {
    Time++;

    if ((Time + 1) % 1 == 0)	//遥控控制周期20ms
    {
      KeyNum = ps2_key_serch();
      PS2_Mode = ps2_mode_get();

      // WiFi模式下的超时检查（每20ms检查一次）
      if (Current_Mode == MODE_WIFI) {
        WiFi_Timeout_Counter++;
        if (WiFi_Timeout_Counter > WIFI_TIMEOUT_TICKS) {
          // 超时，停止电机
          M1_Target = 0;
          M2_Target = 0;
          M3_Target = 0;
          M4_Target = 0;
          Led_State = 0;
          WiFi_Timeout_Counter = WIFI_TIMEOUT_TICKS;  // 防止溢出
        }
      }

      // 检测START键切换WiFi模式
      uint8_t start_state = ps2_get_key_state(PSB_START);
      uint8_t select_state = ps2_get_key_state(PSB_SELECT);

      // START键切换WiFi模式
      if (start_state && !Last_Start_State)
      {
        // 切换到WiFi模式或返回遥控模式
        if (Current_Mode == MODE_REMOTE || Current_Mode == MODE_LINE_TRACK)
        {
          Current_Mode = MODE_WIFI;  // 切换到WiFi模式
          // 停止电机
          M1_Target = 0;
          M2_Target = 0;
          M3_Target = 0;
          M4_Target = 0;
          // 清空串口缓冲区，避免执行之前的命令
          while (Serial.available()) {
            Serial.read();
          }
          FlashAllLights(3);  // 闪烁3次表示进入WiFi模式
          // ESP8266串口已在setup中初始化，这里不需要再初始化
        }
        else if (Current_Mode == MODE_WIFI)
        {
          Current_Mode = MODE_REMOTE;  // 返回遥控模式
          // 停止电机
          Rzhl_Motor1_SetPwm(0, 0);
          Rzhl_Motor2_SetPwm(0, 0);
          Rzhl_Motor3_SetPwm(0, 0);
          Rzhl_Motor4_SetPwm(0, 0);
          M1_Target = 0;
          M2_Target = 0;
          M3_Target = 0;
          M4_Target = 0;
          FlashAllLights(2);  // 闪烁2次表示返回遥控模式
        }
      }

      // SELECT键在遥控模式和巡线模式之间切换
      if (select_state && !Last_Select_State && Current_Mode != MODE_WIFI)
      {
        // 切换模式
        if (Current_Mode == MODE_REMOTE)
        {
          Current_Mode = MODE_LINE_TRACK;  // 切换到巡线模式
          FlashAllLights(1);  // 闪烁1次表示进入巡线模式
        }
        else
        {
          Current_Mode = MODE_REMOTE;      // 切换到遥控模式
          // 立即停止电机
          Rzhl_Motor1_SetPwm(0, 0);
          Rzhl_Motor2_SetPwm(0, 0);
          Rzhl_Motor3_SetPwm(0, 0);
          Rzhl_Motor4_SetPwm(0, 0);
          M1_Target = 0;
          M2_Target = 0;
          M3_Target = 0;
          M4_Target = 0;
          FlashAllLights(2);  // 闪烁2次表示返回遥控模式
        }
      }

      Last_Start_State = start_state;
      Last_Select_State = select_state;

      // 根据当前模式执行不同的控制逻辑
      if (Current_Mode == MODE_LINE_TRACK)
      {
        // ========== 巡线模式 ==========
        // 保持LED常亮表示巡线模式
        Led_On();

        SensorState[0] = Sensor0_Get_State();
        SensorState[1] = Sensor1_Get_State();
        SensorState[2] = Sensor2_Get_State();
        SensorState[3] = Sensor3_Get_State();

        // 调试输出传感器状态已禁用（串口被ESP8266占用）
        // if ((Time + 1) % 25 == 0) { ... }

        // 标记是否找到有效路径
        bool path_found = false;

        if (SensorState[0] && !SensorState[1] && !SensorState[2] && SensorState[3])     //Area 3，当前处于直线
        {
          Area = 3;
          Vehicle_Chassis_Forward(500);     //小车以500电机转直行
          Led_State = 0;
          path_found = true;
        }
        else if (SensorState[0] && SensorState[1] && !SensorState[2] && SensorState[3]) //Area4, 右小弯道
        {
          Area = 4;
          Vehicle_Chassis_TurnRight(500);   //小车以500速度右转
          Led_State = 3;
          path_found = true;
        }
        else if (SensorState[0] && SensorState[1] && SensorState[2] && !SensorState[3]) //Area6 ,右大弯
        {
          Area = 6;
          Vehicle_Chassis_TurnRight(600);   //小车以600速度右转
          Led_State = 3;
          path_found = true;
        }
        else if (SensorState[0] && !SensorState[1] && SensorState[2] && SensorState[3]) //Area2 ,左小弯
        {
          Area = 2;
          Vehicle_Chassis_TurnLeft(500);    //小车以500速度左转
          Led_State = 2;
          path_found = true;
        }
        else if (!SensorState[0] && SensorState[1] && SensorState[2] && SensorState[3]) //Area0 ,左大弯
        {
          Area = 0;
          Vehicle_Chassis_TurnLeft(700);    //小车以700速度左转
          Led_State = 2;
          path_found = true;
        }
        else if (SensorState[0] && SensorState[1] && SensorState[2] && SensorState[3])
        {
          if (Last_Area == 4 || Last_Area == 6)  //Area5, 右中等弯
          {
            Area = 5;
            Vehicle_Chassis_TurnRight(650);  //小车以650速度右转
            Led_State = 3;
            path_found = true;
          }
          else if (Last_Area == 2 || Last_Area == 0)  //Area1, 左中等弯
          {
            Area = 1;
            Vehicle_Chassis_TurnLeft(650);   //小车以650速度左转
            Led_State = 2;
            path_found = true;
          }
        }

        // 如果没有找到有效路径，停止小车
        if (!path_found)
        {
          Rzhl_Motor1_SetPwm(0, 0);
          Rzhl_Motor2_SetPwm(0, 0);
          Rzhl_Motor3_SetPwm(0, 0);
          Rzhl_Motor4_SetPwm(0, 0);
          Led_State = 0;
        }
        else
        {
          Last_Area = Area;
        }
      }
      else if (Current_Mode == MODE_REMOTE)
      {
        // ========== 遥控模式 ==========
        if (PS2_Mode == PSB_REDLIGHT_MODE)
        {
          static unsigned char ps2_lx, ps2_ly, ps2_rx, ps2_ry;
          ps2_lx = ps2_get_anolog_data(PSS_LX);
          ps2_ly = ps2_get_anolog_data(PSS_LY);
          ps2_rx = ps2_get_anolog_data(PSS_RX);
          ps2_ry = ps2_get_anolog_data(PSS_RY);

          if (ps2_get_key_state(PSB_L1))  //机械臂控制
          {
            if (ps2_ly == 0x00)	RobotArm_DropHand(UNIT_PWM);
            if (ps2_ly == 0xff) RobotArm_RaiseHand(UNIT_PWM);
            if (ps2_ry == 0x00)	RobotArm_StretchHand(UNIT_PWM);
            if (ps2_ry == 0xff) RobotArm_ShrinkHand(UNIT_PWM);
            if (ps2_rx == 0x00)	RobotArm_ShakeHand(UNIT_PWM);
            if (ps2_rx == 0xff) RobotArm_LetHand(UNIT_PWM);

            M1_Target = 0;
            M4_Target = 0;
            M2_Target = 0;
            M3_Target = 0;
            Led_State = 0;
          }
          else   //小车底盘控制控制
          {
            //遥感数据处理
            int lx_adjust, rx_adjust, ly_adjust;
            long int lx_target = 0, ly_target = 0, rx_target = 0;

            lx_adjust = ps2_lx - 128;
            rx_adjust = ps2_rx - 128;
            ly_adjust = ps2_ly - 127;

            ly_target = 7.8f * ly_adjust;
            lx_target = 7.8f * lx_adjust;
            rx_target = 6.24f * rx_adjust;

            //各电机转速计算
            M1_Target =  ly_target + lx_target - rx_target;
            M2_Target = -ly_target + lx_target - rx_target;
            M3_Target = -ly_target - lx_target - rx_target;
            M4_Target =  ly_target - lx_target - rx_target;

            //尾灯状态判断； 0：熄灭， 1：后退， 2：左转弯灯， 3：右转弯灯
            if (ly_target > 0)    //倒车
            {
              Led_State = 1;
            }
            else
            {
              if (lx_target < 0 || rx_target < 0)  //左转弯灯
              {
                Led_State = 2;
              }
              else if (lx_target > 0  || rx_target > 0) //右转弯灯
              {
                Led_State = 3;
              }
              else
              {
                Led_State = 0;
              }
            }
          }
        }
        else if (PS2_Mode == PSB_GREENLIGHT_MODE)
        {
          if (ps2_get_key_state(PSB_L1))  //机械臂控制
          {
            if (ps2_get_key_state(PSB_PAD_UP))	   RobotArm_DropHand(UNIT_PWM);
            if (ps2_get_key_state(PSB_PAD_DOWN))   RobotArm_RaiseHand(UNIT_PWM);
            if (ps2_get_key_state(PSB_GREEN))	   RobotArm_StretchHand(UNIT_PWM);
            if (ps2_get_key_state(PSB_BLUE))       RobotArm_ShrinkHand(UNIT_PWM);
            if (ps2_get_key_state(PSB_PINK))	   RobotArm_ShakeHand(UNIT_PWM);
            if (ps2_get_key_state(PSB_RED))		   RobotArm_LetHand(UNIT_PWM);
            //速度清零
            M1_Target = 0;
            M4_Target = 0;
            M2_Target = 0;
            M3_Target = 0;

            //关闭转弯灯
            Led_State = 0;
          }
          else   //小车底盘控制控制
          {
            unsigned char up_state, down_state;
            unsigned char left_state, right_state;
            unsigned char pink_state, red_state;
            int lx_target = 0, ly_target = 0, rx_target = 0;

            up_state = ps2_get_key_state(PSB_PAD_UP);
            down_state = ps2_get_key_state(PSB_PAD_DOWN);
            left_state = ps2_get_key_state(PSB_PAD_LEFT);
            right_state = ps2_get_key_state(PSB_PAD_RIGHT);
            pink_state = ps2_get_key_state(PSB_PINK);
            red_state = ps2_get_key_state(PSB_RED);

            if (up_state)    ly_target = -800;
            if (down_state)  ly_target =  800;
            if (left_state)  lx_target = -800;
            if (right_state) lx_target =  800;
            if (pink_state)  rx_target = -800;
            if (red_state)   rx_target =  800;

            M1_Target =  ly_target + lx_target - rx_target;
            M2_Target = -ly_target + lx_target - rx_target;
            M3_Target = -ly_target - lx_target - rx_target;
            M4_Target =  ly_target - lx_target - rx_target;

            //尾灯状态判断； 0：熄灭， 1：后退， 2：左转弯灯， 3：右转弯灯
            if (ly_target > 0)    //倒车
            {
              Led_State = 1;
            }
            else
            {
              if (lx_target < 0 || rx_target < 0)  //左转弯灯
              {
                Led_State = 2;
              }
              else if (lx_target > 0  || rx_target > 0) //右转弯灯
              {
                Led_State = 3;
              }
              else
              {
                Led_State = 0;
              }
            }
          }
        }
      }
    }

    if ((Time + 1) % 2 == 0)        //电机控制  40ms
    {
      M1_Target >= 0 ? Rzhl_Motor1_SetPwm(0, M1_Target) : Rzhl_Motor1_SetPwm(0 - M1_Target, 0);
      M2_Target >= 0 ? Rzhl_Motor2_SetPwm(0, M2_Target) : Rzhl_Motor2_SetPwm(0 - M2_Target, 0);
      M3_Target >= 0 ? Rzhl_Motor3_SetPwm(0, M3_Target) : Rzhl_Motor3_SetPwm(0 - M3_Target, 0);
      M4_Target >= 0 ? Rzhl_Motor4_SetPwm(0, M4_Target) : Rzhl_Motor4_SetPwm(0 - M4_Target, 0);
    }
    if ((Time + 1) % 10 == 0)   //尾灯控制  200ms
    {
      //车尾灯控制； 0：熄灭， 1：后退， 2：左转弯灯， 3：右转弯灯
      switch (Led_State)
      {
        case 0:
          {
            Rzhl_TailLeftLed_TurnOff();
            Rzhl_TailRightLed_TurnOff();
          } break;
        case 1:
          {
            Rzhl_TailLeftLed_TurnOn();
            Rzhl_TailRightLed_TurnOn();
          } break;
        case 2:
          {
            Rzhl_TailLeftLed_Turn();
            Rzhl_TailRightLed_TurnOff();
          } break;
        case 3:
          {
            Rzhl_TailLeftLed_TurnOff();
            Rzhl_TailRightLed_Turn();
          } break;
        default:
          {
            Rzhl_TailLeftLed_TurnOff();
            Rzhl_TailRightLed_TurnOff();
          } break;
      }
    }
    if ((Time + 1) % 50 == 0)		//指示灯   1s闪烁
    {
      if (Current_Mode == MODE_WIFI) {
        Led_Turn();  // WiFi模式：LED闪烁
      } else if (Current_Mode == MODE_REMOTE) {
        Led_Off();   // 遥控模式：LED熄灭
      }
      // 巡线模式：LED保持常亮，不闪烁
      Time = 0;
    }
  }
}

