#include "NeZha.h"
#include <stdint.h>
#include "RobotArm.h"
#include "Board_Timer.h"
#include "ps2.h"
#include "LED.h"


#define   UNIT_PWM	2				//舵机单位转动值	
#define   PS2_LSPEED    1000		//左遥杆拨到底对应电机最大目标值（电机最大速度为1000）
#define   PS2_RSPEED    1000			//右遥杆拨到底对应电机最大目标值（电机最大速度为1000）
#define   RECIPROCAL	0.0078f     //128的倒数，此处为方便摇杆值映射目标值的计算。不可更改


uint8_t KeyNum;
uint8_t PS2_Mode,Last_PS2_Mode;
int16_t M1_Target, M2_Target, M3_Target, M4_Target;
uint8_t Time;
uint8_t Led_State;  //车尾灯灯状态标志位； 0：熄灭， 1：后退， 2：左转弯灯， 3：右转弯灯

void setup() {
  // put your setup code here, to run once:
	PS2_Init();
	NeZha_Init();
	NeZha_Motor_Init();
	RobotArm_Init(); 
	Board_Timer_Init();
  Led_Init();
}

void loop() {
  // put your main code here, to run repeatedly:
		if (Board_Timer_Flag_Get())    //定时器定时20ms周期
		{
			Time++;

			if ((Time + 1)%1 == 0)	//遥控控制周期20ms
			{
				KeyNum = ps2_key_serch();
				PS2_Mode = ps2_mode_get();				

				/**************小车底盘控制*****************/							
				if(PS2_Mode == PSB_REDLIGHT_MODE)     
				{
					static unsigned char ps2_lx,ps2_ly,ps2_rx,ps2_ry;
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
            Led_State =0;
					}
					else   //小车底盘控制控制
					{
						//遥感数据处理
						int lx_adjust,rx_adjust,ly_adjust;
						long int lx_target = 0, ly_target = 0, rx_target = 0;

						lx_adjust = ps2_lx - 128;
						rx_adjust = ps2_rx - 128;
						ly_adjust = ps2_ly - 127;

          /*************************************************************
					 *摇杆的范围为0-255，当摇杆位于中值时为128
					 *电机满速为1000，即电机速度映射到摇杆值就是 1000/128 = 7.8f
           *电机速度设置为800，即电机速度映射到摇杆值就是800/128 = 6.24f
					 *代表摇杆值每变化1，速度改变7.8
					**************************************************************/
						ly_target = 7.8f * ly_adjust;//PS2_LSPEED  * ly_adjust * RECIPROCAL;  1 4同向，2 3反向
						lx_target = 7.8f * lx_adjust;//PS2_LSPEED  * lx_adjust * RECIPROCAL;  1 2同向，3 4反向
						rx_target = 6.24f * rx_adjust;//PS2_RSPEED  * rx_adjust * RECIPROCAL;  //1 2同向，3 4反向

            //各电机转速计算
						M1_Target =  ly_target + lx_target - rx_target;
						M2_Target = -ly_target + lx_target - rx_target; 
						M3_Target = -ly_target - lx_target - rx_target;
						M4_Target =  ly_target - lx_target - rx_target;	

           //尾灯状态判断； 0：熄灭， 1：后退， 2：左转弯灯， 3：右转弯灯
						if(ly_target > 0)    //倒车
						{
						  Led_State = 1;
						}
						else
						{
						  if (lx_target < 0 || rx_target < 0)  //左转弯灯
						  {
							Led_State = 2;
						  }
						  else if(lx_target > 0  || rx_target > 0) //右转弯灯
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
				else if(PS2_Mode == PSB_GREENLIGHT_MODE)
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
						unsigned char up_state,down_state;
						unsigned char left_state,right_state;
						unsigned char pink_state,red_state;
						int lx_target = 0, ly_target = 0, rx_target = 0;

						up_state = ps2_get_key_state(PSB_PAD_UP);
						down_state = ps2_get_key_state(PSB_PAD_DOWN);
						left_state = ps2_get_key_state(PSB_PAD_LEFT);
						right_state = ps2_get_key_state(PSB_PAD_RIGHT);
						pink_state = ps2_get_key_state(PSB_PINK);
						red_state = ps2_get_key_state(PSB_RED);

						up_state == 1?   ly_target = PS2_LSPEED : (down_state == 1? (ly_target = 0 - PS2_LSPEED) : (ly_target = 0));
						left_state == 1? lx_target = PS2_LSPEED : (right_state == 1? (lx_target = 0 - PS2_LSPEED) : (lx_target = 0));
						pink_state == 1? rx_target = PS2_RSPEED : (red_state == 1? (rx_target = 0 - PS2_RSPEED) : (rx_target = 0));

            //电机转速
						M1_Target = -ly_target - lx_target + rx_target;
						M2_Target =  ly_target - lx_target + rx_target; 
						M3_Target =  ly_target + lx_target + rx_target;
						M4_Target = -ly_target + lx_target + rx_target;	

            //尾灯状态判断； 0：熄灭， 1：后退， 2：左转弯灯， 3：右转弯灯
            if(down_state){
              Led_State = 1;
            }else if(left_state||pink_state){
              Led_State = 2;
            }else if(right_state||red_state){
              Led_State = 3;
            }else{
              Led_State = 0;
            }
					}
				}
				Last_PS2_Mode = PS2_Mode;
			}		
			if ((Time + 1)%2 == 0)        //电机控制  40ms
			{
				M1_Target >= 0? NeZha_Motor1_SetPwm(0,M1_Target) : NeZha_Motor1_SetPwm(0 - M1_Target,0);
				M2_Target >= 0? NeZha_Motor2_SetPwm(0,M2_Target) : NeZha_Motor2_SetPwm(0 - M2_Target,0);
				M3_Target >= 0? NeZha_Motor3_SetPwm(0,M3_Target) : NeZha_Motor3_SetPwm(0 - M3_Target,0);
				M4_Target >= 0? NeZha_Motor4_SetPwm(0,M4_Target) : NeZha_Motor4_SetPwm(0 - M4_Target,0);
			}
      if ((Time + 1)%10 == 0)   //尾灯控制  200ms
      {
        //车尾灯控制； 0：熄灭， 1：后退， 2：左转弯灯， 3：右转弯灯
        switch(Led_State)
        {
          case 0:{
            NeZha_TailLeftLed_TurnOff();
            NeZha_TailRightLed_TurnOff();
          }break;
          case 1:{
            NeZha_TailLeftLed_TurnOn();
            NeZha_TailRightLed_TurnOn();
          }break;
          case 2:{
            NeZha_TailLeftLed_Turn();
            NeZha_TailRightLed_TurnOff();
          }break;     
          case 3:{
            NeZha_TailLeftLed_TurnOff();
            NeZha_TailRightLed_Turn();
          }break;  
          default:{
            NeZha_TailLeftLed_TurnOff();
            NeZha_TailRightLed_TurnOff();
          }break;     
        }
      }
      if((Time + 1)%50 == 0)		//指示灯   1s闪烁
			{
				Led_Turn();
				Time = 0;
			}
		}	
}
