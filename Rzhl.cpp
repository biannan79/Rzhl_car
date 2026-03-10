/******************************************************************
*  	@张蕙斓：Arduino-Rzhl驱动板库V1.0
*	有关驱动板的详细使用请见使用手册
*
*	Rzhl驱动板库共4个文件：
*	1. Rzhl.c   	&	Rzhl.h
*		功能源代码文件：这两个文件内定义了驱动板的功能函数，
*						用户可调用这些函数控制驱动板驱动直流电机以及舵机
*	2. Rzhl_I2C.c	&	Rzhl_I2C.h
*		I2C接口硬件配置文件：这两个文件内是关于I2C总线硬件的配置
*
*	移植Rzhl驱动板库步骤说明：
*   1. 修改Rzhl_I2C.c 文件中关于I2C的引脚配置
*		1.1 修改 Rzhl_I2C_Init()函数中的配置，修改SCL、SDA引脚为自定义的引脚
*		1.3 修改 Rzhl_I2C_SdaDir()函数中关于SDA引脚的配置
******************************************************************/

#include "Rzhl.h"
#include "Rzhl_I2c.h"

/**
  * @brief  NeZhen驱动板初始化
  * @param 	无
  * @retval 无
  */
void Rzhl_Init()
{
	Rzhl_I2C_Init();
	Rzhl_Reset();
}

/**
  * @brief  NeZhen驱动板复位
  * @param 	无
  * @retval 无
  */
void Rzhl_Reset()
{
	Rzhl_WriteCommand(RZHL_CMD_RESET);
	Rzhl_Delay_ms(100);	//上电延时,不可去掉!!!
}

/**
  * @brief  Rzhl驱动板电机初始化
  * @param 	无
  * @retval 无
  */
void Rzhl_Motor_Init()
{
	Rzhl_WriteCommand(RZHL_CMD_MOTOR_INIT);
}

/**
  * @brief  Rzhl驱动板电机1的PWM设置
  * @param 	motor_a：电机反转，范围0-1000
			motor_b：电机正转，范围0-1000
  * @retval 无
  */
void Rzhl_Motor1_SetPwm(uint16_t motor_a,uint16_t motor_b)
{
	uint8_t motor_ah,motor_al,motor_bh,motor_bl;

	motor_ah = (uint8_t)(motor_a >> 8);
	motor_al = (uint8_t)(motor_a & 0x00FF);
	motor_bh = (uint8_t)(motor_b >> 8);
	motor_bl = (uint8_t)(motor_b & 0x00FF);
	
	Rzhl_WriteCommand(RZHL_CMD_MOTOR1_SET);
	
	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR);
	Rzhl_I2C_SendByte(RZHL_CMD_MOTOR1_SET);
	Rzhl_I2C_SendByte(motor_ah);
	Rzhl_I2C_SendByte(motor_al);
	Rzhl_I2C_SendByte(motor_bh);
	Rzhl_I2C_SendByte(motor_bl);
	Rzhl_I2C_Stop();
}

/**
  * @brief  Rzhl驱动板电机2的PWM设置
  * @param 	motor_a：电机反转，范围0-1000
			motor_b：电机正转，范围0-1000
  * @retval 无
  */
void Rzhl_Motor2_SetPwm(uint16_t motor_a,uint16_t motor_b)
{
	uint8_t motor_ah,motor_al,motor_bh,motor_bl;

	motor_ah = (uint8_t)(motor_a >> 8);
	motor_al = (uint8_t)(motor_a & 0x00FF);
	motor_bh = (uint8_t)(motor_b >> 8);
	motor_bl = (uint8_t)(motor_b & 0x00FF);
	
	Rzhl_WriteCommand(RZHL_CMD_MOTOR2_SET);
	
	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR);
	Rzhl_I2C_SendByte(RZHL_CMD_MOTOR2_SET);
	Rzhl_I2C_SendByte(motor_ah);
	Rzhl_I2C_SendByte(motor_al);
	Rzhl_I2C_SendByte(motor_bh);
	Rzhl_I2C_SendByte(motor_bl);
	Rzhl_I2C_Stop();
}

/**
  * @brief  Rzhl驱动板电机3的PWM设置
  * @param 	motor_a：电机反转，范围0-1000
			motor_b：电机正转，范围0-1000
  * @retval 无
  */
void Rzhl_Motor3_SetPwm(uint16_t motor_a,uint16_t motor_b)
{
	uint8_t motor_ah,motor_al,motor_bh,motor_bl;

	motor_ah = (uint8_t)(motor_a >> 8);
	motor_al = (uint8_t)(motor_a & 0x00FF);
	motor_bh = (uint8_t)(motor_b >> 8);
	motor_bl = (uint8_t)(motor_b & 0x00FF);

	Rzhl_WriteCommand(RZHL_CMD_MOTOR3_SET);
	
	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR);
	Rzhl_I2C_SendByte(RZHL_CMD_MOTOR3_SET);
	Rzhl_I2C_SendByte(motor_ah);
	Rzhl_I2C_SendByte(motor_al);
	Rzhl_I2C_SendByte(motor_bh);
	Rzhl_I2C_SendByte(motor_bl);
	Rzhl_I2C_Stop();
}

/**
  * @brief  Rzhl驱动板电机4的PWM设置
  * @param 	motor_a：电机反转，范围0-1000
			motor_b：电机正转，范围0-1000
  * @retval 无
  */
void Rzhl_Motor4_SetPwm(uint16_t motor_a,uint16_t motor_b)
{
	uint8_t motor_ah,motor_al,motor_bh,motor_bl;

	motor_ah = (uint8_t)(motor_a >> 8);
	motor_al = (uint8_t)(motor_a & 0x00FF);
	motor_bh = (uint8_t)(motor_b >> 8);
	motor_bl = (uint8_t)(motor_b & 0x00FF);
	
	Rzhl_WriteCommand(RZHL_CMD_MOTOR4_SET);
	
	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR);
	Rzhl_I2C_SendByte(RZHL_CMD_MOTOR4_SET);
	Rzhl_I2C_SendByte(motor_ah);
	Rzhl_I2C_SendByte(motor_al);
	Rzhl_I2C_SendByte(motor_bh);
	Rzhl_I2C_SendByte(motor_bl);
	Rzhl_I2C_Stop();
}

/**
  * @brief  Rzhl驱动板电机1编码器初始化
  * @param 	无
  * @retval 无
  */
void Rzhl_Encoder1_Init()
{
	Rzhl_WriteCommand(RZHL_CMD_ENCODER1_INIT);	
}

/**
  * @brief  Rzhl驱动板电机2编码器初始化
  * @param 	无
  * @retval 无
  */
void Rzhl_Encoder2_Init()
{
	Rzhl_WriteCommand(RZHL_CMD_ENCODER2_INIT);	
}

/**
  * @brief  Rzhl驱动板电机3编码器初始化
  * @param 	无
  * @retval 无
  */
void Rzhl_Encoder3_Init()
{
	Rzhl_WriteCommand(RZHL_CMD_ENCODER3_INIT);	
}

/**
  * @brief  Rzhl驱动板电机4编码器初始化
  * @param 	无
  * @retval 无
  */
void Rzhl_Encoder4_Init()
{
	Rzhl_WriteCommand(RZHL_CMD_ENCODER4_INIT);	
}

/**
  * @brief  Rzhl驱动板电机1编码器数据读取
  * @param 	无
  * @retval value：电机1的转速(编码器数据采集周期为20ms)
			value>=0,表示电机正转
			value<0,表示电机反转
  */
int16_t Rzhl_Encoder1_Read()
{
	uint8_t value_h = 0, value_l = 0;
	int16_t value = 0;

	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR);
	Rzhl_I2C_SendByte(RZHL_CMD_ENCODER1_READ);
	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR|0x01);
	
	value_h = Rzhl_I2C_ReadByte();
	Rzhl_I2C_ACK();
	value_l = Rzhl_I2C_ReadByte();
	Rzhl_I2C_ACK();
	
	Rzhl_I2C_Stop();
		
	value = (value_h << 8) |  value_l;
	return value;
}

/**
  * @brief  Rzhl驱动板电机2编码器数据读取
  * @param 	无
  * @retval value：电机2的转速(编码器数据采集周期为20ms)
			value>=0,表示电机正转
			value<0,表示电机反转
  */
int16_t Rzhl_Encoder2_Read()
{
	uint8_t value_h = 0, value_l = 0;
	int16_t value = 0;

	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR);
	Rzhl_I2C_SendByte(RZHL_CMD_ENCODER2_READ);
	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR|0x01);
	value_h = Rzhl_I2C_ReadByte();
	Rzhl_I2C_ACK();
	value_l = Rzhl_I2C_ReadByte();
	Rzhl_I2C_ACK();
	Rzhl_I2C_Stop();
		
	value = (value_h << 8) |  value_l;

	return value;
}

/**
  * @brief  Rzhl驱动板电机3编码器数据读取
  * @param 	无
  * @retval value：电机3的转速(编码器数据采集周期为20ms)
			value>=0,表示电机正转
			value<0,表示电机反转
  */
int16_t Rzhl_Encoder3_Read()
{
	uint8_t value_h = 0, value_l = 0;
	int16_t value = 0;

	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR);
	Rzhl_I2C_SendByte(RZHL_CMD_ENCODER3_READ);
	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR|0x01);
	value_h = Rzhl_I2C_ReadByte();
	Rzhl_I2C_ACK();
	value_l = Rzhl_I2C_ReadByte();
	Rzhl_I2C_ACK();
	Rzhl_I2C_Stop();
		
	value = (value_h << 8) |  value_l;

	return value;
}

/**
  * @brief  Rzhl驱动板电机4编码器数据读取
  * @param 	无
  * @retval value：电机4的转速(编码器数据采集周期为20ms)
			value>=0,表示电机正转
			value<0,表示电机反转
  */
int16_t Rzhl_Encoder4_Read()
{
	uint8_t value_h = 0, value_l = 0;
	int16_t value = 0;

	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR);
	Rzhl_I2C_SendByte(RZHL_CMD_ENCODER4_READ);
	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR|0x01);
	value_h = Rzhl_I2C_ReadByte();
	Rzhl_I2C_ACK();
	value_l = Rzhl_I2C_ReadByte();
	Rzhl_I2C_ACK();
	Rzhl_I2C_Stop();
		
	value = (value_h << 8) |  value_l;

	return value;
}

/**
  * @brief  Rzhl驱动板舵机1初始化
  * @param 	无
  * @retval 无
  */
void Rzhl_Servo1_Init()
{
	Rzhl_WriteCommand(RZHL_CMD_SERVO1_INIT);	
}

/**
  * @brief  Rzhl驱动板舵机2初始化
  * @param 	无
  * @retval 无
  */
void Rzhl_Servo2_Init()
{
	Rzhl_WriteCommand(RZHL_CMD_SERVO2_INIT);	
}

/**
  * @brief  Rzhl驱动板舵机3初始化
  * @param 	无
  * @retval 无
  */
void Rzhl_Servo3_Init()
{
	Rzhl_WriteCommand(RZHL_CMD_SERVO3_INIT);	
}

/**
  * @brief  Rzhl驱动板舵机4初始化
  * @param 	无
  * @retval 无
  */
void Rzhl_Servo4_Init()
{
	Rzhl_WriteCommand(RZHL_CMD_SERVO4_INIT);	
}

/**
  * @brief  Rzhl驱动板舵机1的PWM设置
  * @param 	pwm 范围：50-250(对应舵机角度0°- 180°)
  * @retval 无
  */
void Rzhl_Servo1_SetPwm(uint16_t pwm)
{
	uint8_t servo1_h,servo1_l;

	servo1_h = (uint8_t)(pwm >> 8);
	servo1_l = (uint8_t)(pwm & 0x00FF);
	
	Rzhl_WriteCommand(RZHL_CMD_SERVO1_SET);
	
	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR);
	Rzhl_I2C_SendByte(RZHL_CMD_SERVO1_SET);
	Rzhl_I2C_SendByte(servo1_h);
	Rzhl_I2C_SendByte(servo1_l);
	Rzhl_I2C_Stop();
}

/**
  * @brief  Rzhl驱动板舵机2的PWM设置
  * @param 	pwm 范围：50-250(对应舵机角度0°- 180°)
  * @retval 无
  */
void Rzhl_Servo2_SetPwm(uint16_t pwm)
{
	uint8_t servo2_h,servo2_l;

	servo2_h = (uint8_t)(pwm >> 8);
	servo2_l = (uint8_t)(pwm & 0x00FF);
	
	Rzhl_WriteCommand(RZHL_CMD_SERVO2_SET);
	
	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR);
	Rzhl_I2C_SendByte(RZHL_CMD_SERVO2_SET);
	Rzhl_I2C_SendByte(servo2_h);
	Rzhl_I2C_SendByte(servo2_l);
	Rzhl_I2C_Stop();
}

/**
  * @brief  Rzhl驱动板舵机3的PWM设置
  * @param 	pwm 范围：50-250(对应舵机角度0°- 180°)
  * @retval 无
  */
void Rzhl_Servo3_SetPwm(uint16_t pwm)
{
	uint8_t servo3_h,servo3_l;

	servo3_h = (uint8_t)(pwm >> 8);
	servo3_l = (uint8_t)(pwm & 0x00FF);
	
	Rzhl_WriteCommand(RZHL_CMD_SERVO3_SET);
	
	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR);
	Rzhl_I2C_SendByte(RZHL_CMD_SERVO3_SET);
	Rzhl_I2C_SendByte(servo3_h);
	Rzhl_I2C_SendByte(servo3_l);
	Rzhl_I2C_Stop();
}

/**
  * @brief  Rzhl驱动板舵机4的PWM设置
  * @param 	pwm 范围：50-250(对应舵机角度0°- 180°)
  * @retval 无
  */
void Rzhl_Servo4_SetPwm(uint16_t pwm)
{
	uint8_t servo4_h,servo4_l;

	servo4_h = (uint8_t)(pwm >> 8);
	servo4_l = (uint8_t)(pwm & 0x00FF);
	
	Rzhl_WriteCommand(RZHL_CMD_SERVO4_SET);
	
	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR);
	Rzhl_I2C_SendByte(RZHL_CMD_SERVO4_SET);
	Rzhl_I2C_SendByte(servo4_h);
	Rzhl_I2C_SendByte(servo4_l);
	Rzhl_I2C_Stop();
}
/**
  * @brief  Rzhl驱动板前灯打开
  * @param  无
  * @retval 无
  */
void Rzhl_HeadLed_TurnOn()
{
	Rzhl_WriteCommand(RZHL_CMD_HEADLED_ON);
}

/**
  * @brief  Rzhl驱动板前灯关闭
  * @param  无
  * @retval 无
  */
void Rzhl_HeadLed_TurnOff()
{
	Rzhl_WriteCommand(RZHL_CMD_HEADLED_OFF);
}

/**
  * @brief  Rzhl驱动板前灯翻转
  * @param  无
  * @retval 无
  */
void Rzhl_HeadLed_Turn()
{
	Rzhl_WriteCommand(RZHL_CMD_HEADLED_TURN);
}

/**
  * @brief  Rzhl驱动板氛围灯打开
  * @param  无
  * @retval 无
  */
void Rzhl_AmbientLed_TurnOn()
{
	Rzhl_WriteCommand(RZHL_CMD_AMBIENTLED_ON);
}

/**
  * @brief  Rzhl驱动板氛围灯关闭
  * @param  无
  * @retval 无
  */
void Rzhl_AmbientLed_TurnOff()
{
	Rzhl_WriteCommand(RZHL_CMD_AMBIENTLED_OFF);
}

/**
  * @brief  Rzhl驱动板氛围灯翻转
  * @param  无
  * @retval 无
  */
void Rzhl_AmbientLed_Turn()
{
	Rzhl_WriteCommand(RZHL_CMD_AMBIENTLED_TURN);
}

/**
  * @brief  Rzhl驱动板左尾灯打开
  * @param  无
  * @retval 无
  */
void Rzhl_TailLeftLed_TurnOn()
{
	Rzhl_WriteCommand(RZHL_CMD_TAILLEFTLED_ON);
}

/**
  * @brief  Rzhl驱动板左尾灯关闭
  * @param  无
  * @retval 无
  */
void Rzhl_TailLeftLed_TurnOff()
{
	Rzhl_WriteCommand(RZHL_CMD_TAILLEFTLED_OFF);
}

/**
  * @brief  Rzhl驱动板左尾灯翻转
  * @param  无
  * @retval 无
  */
void Rzhl_TailLeftLed_Turn()
{
	Rzhl_WriteCommand(RZHL_CMD_TAILLEFTLED_TURN);
}

/**
  * @brief  Rzhl驱动板右尾灯打开
  * @param  无
  * @retval 无
  */
void Rzhl_TailRightLed_TurnOn()
{
	Rzhl_WriteCommand(RZHL_CMD_TAILRIGHTLED_ON);
}

/**
  * @brief  Rzhl驱动板右尾灯关闭
  * @param  无
  * @retval 无
  */
void Rzhl_TailRightLed_TurnOff()
{
	Rzhl_WriteCommand(RZHL_CMD_TAILRIGHTLED_OFF);
}

/**
  * @brief  Rzhl驱动板右尾灯翻转
  * @param  无
  * @retval 无
  */
void Rzhl_TailRightLed_Turn()
{
	Rzhl_WriteCommand(RZHL_CMD_TAILRIGHTLED_TURN);
}

/**
  * @brief  Rzhl驱动板发送一个命令
  * @param 	Command 范围：Rzhl指令
  * @retval 无
  */
void Rzhl_WriteCommand(uint8_t Command)
{
	Rzhl_I2C_Start();
	Rzhl_I2C_SendByte(RZHL_ADDR);
	Rzhl_I2C_SendByte(0x00);
	Rzhl_I2C_SendByte(Command);
	Rzhl_I2C_Stop();	
}
