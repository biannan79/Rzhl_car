
#include "RobotArm.h"
#include "Rzhl.h"
#include "type_servo.h"

/******************************************************************
*  	@张蕙斓：大圣机械臂（Arduino）驱动库V1.0
*
*	本文件是基于Rzhl驱动板编写的机械臂驱动库文件。
*	Rzhl驱动板的四个舵机接口对应连接：
*	Servo2 ------------> 大圣机械臂左舵机
*	Servo3 ------------> 大圣机械臂前舵机
*	Servo4 ------------> 大圣机械臂右舵机
******************************************************************/



/*****************************************************************
舵机结构体：
*	1.舵机基本参数，从左到右对应舵机当前pwm值，左极限值，中值，右极限值
*	2.该值代表当前PWM高电平时间，例  150 = 1.5ms，对应舵机角度90°
******************************************************************/
struct SERVO_PARAM robotarm_left = {200, 186, 200, 250};
struct SERVO_PARAM robotarm_up = {150, 100, 150, 250};
struct SERVO_PARAM robotarm_right = {180, 163, 180, 250};

/**
  * @brief  机械臂初始化
  * @param 	无
  * @retval 无
  */
void RobotArm_Init()
{
	Rzhl_Servo2_Init();		//Rzhl驱动板舵机2初始化（Rzhl舵机2接机械臂左舵机）
	Rzhl_Servo3_Init();		//Rzhl驱动板舵机3初始化（Rzhl舵机3接机械臂前舵机）
	Rzhl_Servo4_Init();		//Rzhl驱动板舵机4初始化（Rzhl舵机4接机械臂右舵机）
	Robot_Arm_Reset();			//机械臂复位
}


/**
  * @brief  机械臂
  * @param 	无
  * @retval 无
  */
void Robot_Arm_Reset()
{
	robotarm_left.pwm  =  robotarm_left.middle;
	robotarm_up.pwm    =  robotarm_up.middle;
	robotarm_right.pwm =  robotarm_right.middle;

	Rzhl_Servo2_SetPwm(robotarm_left.middle);
	Rzhl_Servo3_SetPwm(robotarm_up.middle);
	Rzhl_Servo4_SetPwm(robotarm_right.middle);
}

/**
  * @brief  机械臂伸手
  * @param 	unit_pwm，舵机单位转动角度
  * @retval 无
  */
void RobotArm_StretchHand(unsigned char unit_pwm)
{
	unit_pwm = robotarm_right.pwm + unit_pwm;
	
		//限幅
	if (unit_pwm > robotarm_right.right_limit)
	{
		unit_pwm = robotarm_right.right_limit;
	}

	robotarm_right.pwm = unit_pwm;
	Rzhl_Servo4_SetPwm(unit_pwm);
}

/**
  * @brief  机械臂缩手
  * @param 	unit_pwm，舵机单位转动角度
  * @retval 无
  */
void RobotArm_ShrinkHand(unsigned char unit_pwm)
{
	unit_pwm = robotarm_right.pwm - unit_pwm;
	
		//限幅
	if (unit_pwm < robotarm_right.left_limit)
	{
		unit_pwm = robotarm_right.left_limit;
	}
	robotarm_right.pwm = unit_pwm;
	Rzhl_Servo4_SetPwm(unit_pwm);
}

/**
  * @brief  机械臂抬手
  * @param 	unit_pwm，舵机单位转动角度
  * @retval 无
  */
void RobotArm_RaiseHand(unsigned char unit_pwm)
{
	unit_pwm = robotarm_left.pwm + unit_pwm;
	
		//限幅
	if (unit_pwm > robotarm_left.right_limit)
	{
		unit_pwm = robotarm_left.right_limit;
	}
	robotarm_left.pwm = unit_pwm;	
	Rzhl_Servo2_SetPwm(unit_pwm);
}

/**
  * @brief  机械臂放下手
  * @param 	unit_pwm，舵机单位转动角度
  * @retval 无
  */
void RobotArm_DropHand(unsigned char unit_pwm)
{
	unit_pwm = robotarm_left.pwm - unit_pwm;

		//限幅
	if (unit_pwm < robotarm_left.left_limit)
	{
		unit_pwm = robotarm_left.left_limit;
	}
	robotarm_left.pwm = unit_pwm;
	Rzhl_Servo2_SetPwm(unit_pwm);
		
}

/**
  * @brief  机械臂握手
  * @param 	unit_pwm，舵机单位转动角度
  * @retval 无
  */
void RobotArm_ShakeHand(unsigned char unit_pwm)
{
	unit_pwm = robotarm_up.pwm + unit_pwm;
	
			//限幅
	if (unit_pwm > robotarm_up.right_limit)
	{
		unit_pwm = robotarm_up.right_limit;
	}
	robotarm_up.pwm = unit_pwm;	
	Rzhl_Servo3_SetPwm(unit_pwm);
}

/**
  * @brief  机械臂松手
  * @param 	unit_pwm，舵机单位转动角度
  * @retval 无
  */
void RobotArm_LetHand(unsigned char unit_pwm)
{
	unit_pwm = robotarm_up.pwm - unit_pwm;

		//限幅
	if (unit_pwm < robotarm_up.left_limit)
	{
		unit_pwm = robotarm_up.left_limit;
	}
	robotarm_up.pwm = unit_pwm;	
	Rzhl_Servo3_SetPwm(unit_pwm);
}



