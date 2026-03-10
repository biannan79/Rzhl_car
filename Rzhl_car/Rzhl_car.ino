#include "Rzhl.h"
#include <stdint.h>
#include "RobotArm.h"
#include "Board_Timer.h"
#include "ps2.h"
#include "LED.h"
#include "Sensor.h"
#include "Vehicle_Chassis.h"

#define   UNIT_PWM	2				//舵机单位转动值
#define   PS2_LSPEED    1000		//左遥杆拨到底对应电机最大目标值（电机最大速度为1000）
#define   PS2_RSPEED    1000		//右遥杆拨到底对应电机最大目标值（电机最大速度为1000）
#define   RECIPROCAL	0.0078f     //128的倒数，此处为方便摇杆值映射目标值的计算。不可更改

// 模式定义
#define MODE_REMOTE     0  // 遥控模式
#define MODE_LINE_TRACK 1  // 巡线模式

uint8_t KeyNum;
uint8_t PS2_Mode, Last_PS2_Mode;
int16_t M1_Target, M2_Target, M3_Target, M4_Target;
uint8_t Time;
uint8_t Led_State;  //车尾灯灯状态标志位； 0：熄灭， 1：后退， 2：左转弯灯， 3：右转弯灯

// 巡线相关变量
uint8_t SensorState[4];
uint8_t Area, Last_Area;

// 模式控制变量
uint8_t Current_Mode = MODE_REMOTE;  // 当前模式，默认遥控模式
uint8_t Last_Start_State = 0;        // 上次START键状态，用于检测按键按下
uint8_t Last_Select_State = 0;       // 上次SELECT键状态
uint8_t Debug_Mode = 0;              // 调试模式：0=正常，1=显示所有按键

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
  } else {
    // 遥控模式：LED按原来的1秒闪烁
  }
}

void setup() {
  Serial.begin(9600);  // 启动串口调试
  PS2_Init();
  Rzhl_Init();
  Rzhl_Motor_Init();
  RobotArm_Init();
  Sensor_Init();           // 初始化传感器
  Vehicle_Chassis_Init();  // 初始化底盘
  Board_Timer_Init();
  Led_Init();
  Serial.println("System Initialized!");
  Serial.println("Commands:");
  Serial.println("  1 - Enter Line Track Mode");
  Serial.println("  0 - Enter Remote Mode");
  Serial.println("  s - Show Sensor Status");
  Serial.println("  m - Show Current Mode");
  Serial.println("  d - Toggle Debug Mode (show all key presses)");
}

void loop() {
  // 处理串口命令
  if (Serial.available() > 0) {
    char cmd = Serial.read();

    if (cmd == '1') {
      // 进入巡线模式
      Current_Mode = MODE_LINE_TRACK;
      FlashAllLights(1);
      Serial.println(">>> Switched to LINE TRACK Mode");
    }
    else if (cmd == '0') {
      // 进入遥控模式
      Current_Mode = MODE_REMOTE;
      // 立即停止电机
      Rzhl_Motor1_SetPwm(0, 0);
      Rzhl_Motor2_SetPwm(0, 0);
      Rzhl_Motor3_SetPwm(0, 0);
      Rzhl_Motor4_SetPwm(0, 0);
      M1_Target = 0;
      M2_Target = 0;
      M3_Target = 0;
      M4_Target = 0;
      FlashAllLights(2);
      Serial.println(">>> Switched to REMOTE Mode");
    }
    else if (cmd == 's') {
      // 显示传感器状态
      SensorState[0] = Sensor0_Get_State();
      SensorState[1] = Sensor1_Get_State();
      SensorState[2] = Sensor2_Get_State();
      SensorState[3] = Sensor3_Get_State();
      Serial.print("Sensors: ");
      Serial.print(SensorState[0]);
      Serial.print("-");
      Serial.print(SensorState[1]);
      Serial.print("-");
      Serial.print(SensorState[2]);
      Serial.print("-");
      Serial.println(SensorState[3]);
    }
    else if (cmd == 'm') {
      // 显示当前模式
      if (Current_Mode == MODE_REMOTE) {
        Serial.println("Current Mode: REMOTE");
      } else {
        Serial.println("Current Mode: LINE TRACK");
      }
    }
    else if (cmd == 'd') {
      // 切换调试模式
      Debug_Mode = !Debug_Mode;
      if (Debug_Mode) {
        Serial.println(">>> Debug Mode ON - Will show all key presses");
      } else {
        Serial.println(">>> Debug Mode OFF");
      }
    }
  }

  if (Board_Timer_Flag_Get())    //定时器定时20ms周期
  {
    Time++;

    if ((Time + 1) % 1 == 0)	//遥控控制周期20ms
    {
      KeyNum = ps2_key_serch();
      PS2_Mode = ps2_mode_get();

      // 调试模式：显示所有按键
      if (Debug_Mode) {
        // 检测所有按键（PSB_SELECT=1 到 PSB_BLUE=16）
        for (uint8_t btn = 1; btn <= 16; btn++) {
          if (ps2_get_key_state(btn)) {
            Serial.print(">>> Button pressed: ");
            Serial.print(btn);
            Serial.print(" (");
            // 显示按键名称
            switch(btn) {
              case 1: Serial.print("SELECT"); break;
              case 2: Serial.print("L3"); break;
              case 3: Serial.print("R3"); break;
              case 4: Serial.print("START"); break;
              case 5: Serial.print("PAD_UP"); break;
              case 6: Serial.print("PAD_RIGHT"); break;
              case 7: Serial.print("PAD_DOWN"); break;
              case 8: Serial.print("PAD_LEFT"); break;
              case 9: Serial.print("L2"); break;
              case 10: Serial.print("R2"); break;
              case 11: Serial.print("L1"); break;
              case 12: Serial.print("R1"); break;
              case 13: Serial.print("GREEN"); break;
              case 14: Serial.print("RED"); break;
              case 15: Serial.print("BLUE"); break;
              case 16: Serial.print("PINK"); break;
              default: Serial.print("UNKNOWN"); break;
            }
            Serial.println(")");
          }
        }
      }

      // 检测START键或SELECT键切换模式（两个键都可以）
      uint8_t start_state = ps2_get_key_state(PSB_START);
      uint8_t select_state = ps2_get_key_state(PSB_SELECT);

      // START键切换
      if (start_state && !Last_Start_State)
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
          FlashAllLights(2);  // 闪烁2次表示退出巡线模式
        }
      }
      Last_Start_State = start_state;

      // SELECT键也可以切换（备用）
      if (select_state && !Last_Select_State)
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
          FlashAllLights(2);  // 闪烁2次表示退出巡线模式
        }
      }
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

        // 调试输出传感器状态
        if ((Time + 1) % 25 == 0) {  // 每500ms输出一次
          Serial.print("Sensors: ");
          Serial.print(SensorState[0]);
          Serial.print("-");
          Serial.print(SensorState[1]);
          Serial.print("-");
          Serial.print(SensorState[2]);
          Serial.print("-");
          Serial.println(SensorState[3]);
        }

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
      else
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
      if (Current_Mode == MODE_REMOTE) {
        Led_Turn();  // 遥控模式：LED闪烁
      }
      // 巡线模式：LED保持常亮，不闪烁
      Time = 0;
    }
  }
}

