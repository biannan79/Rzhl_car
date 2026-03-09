/******************************************************************
*  	有方机器人：Arduino-NeZha(哪吒)驱动板库V1.0
*	有关驱动板的详细使用请见使用手册
*	官方指定购买渠道：有方机器人（淘宝店铺）
*	官方淘宝地址：https://shop479988600.taobao.com/
*
*	NeZha(哪吒)驱动板库共4个文件：
*	1. NeZha.c   	&	NeZhe.h			
*		功能源代码文件：这两个文件内定义了驱动板的功能函数，
*						用户可调用这些函数控制驱动板驱动直流电机以及舵机
*	2. NeZha_I2C.c	&	NeZhen_I2C.h 	
*		I2C接口硬件配置文件：这两个文件内是关于I2C总线硬件的配置
*
*	移植NeZha(哪吒)驱动板库步骤说明：
*   1. 修改NeZha_I2C.c 文件中关于I2C的引脚配置
*		1.1 修改 NeZha_I2C_Init()函数中的配置，修改SCL、SDA引脚为自定义的引脚
*		1.3 修改 NeZha_I2C_SdaDir()函数中关于SDA引脚的配置
******************************************************************/

#include "NeZha_I2C.h"
#include <Arduino.h>

/*******************************************************************************************
*	UNIT_T:	I2C通信半周期参数(单位us)，例：当其为2时，对应I2C通信周期为2*2us = 4us
			对应I2C通信速率为 1 / 4us = 250KHz（极限）
			
	注：NeZha(哪吒)驱动板I2C通信速率不得高于200KHz，即UNIT_T不得小于2
		过高的通信速率可能会导致主控系统与NeZha(哪吒)驱动板通信失败
		若通信失败请适当降低速率。
*******************************************************************************************/
#define UNIT_T 2    //不得低于2us

#define NeZha_SCL(x) digitalWrite(6, x)
#define NeZha_SDA(x) digitalWrite(7, x)

/**
  * @brief  NeZha(哪吒)驱动板I2C初始化
  * @param  无
  * @retval 无
  */
void NeZha_I2C_Init()
{
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
}

/**
  * @brief  SDA引脚方向设置
  * @param  dir：dir = 1，SDA为输入；dir = 0，SDA为输出
  * @retval 无
  */
void NeZha_I2C_SdaDir(unsigned char dir)
{
    if (!dir) {
       pinMode(7, OUTPUT);
    } else {
       pinMode(7, INPUT);
    }
}

/**
  * @brief  I2C开始信号
  * @param  无
  * @retval 无
  */
void NeZha_I2C_Start(void)
{
    NeZha_SDA(1);
    NeZha_SCL(1);
    NeZha_SDA(0);
    NeZha_Delay_us(UNIT_T);
    NeZha_SCL(0);
}

/**
  * @brief  I2C停止信号
  * @param  无
  * @retval 无
  */
void NeZha_I2C_Stop(void)
{
    NeZha_SDA(0);
    NeZha_SCL(1);
    NeZha_Delay_us(UNIT_T);
    NeZha_SDA(1);
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte：要发送的一个字节
  * @retval 无
  */
void NeZha_I2C_SendByte(unsigned char Byte)
{
    NeZha_I2C_SdaDir(0);
    NeZha_SCL(0);
    NeZha_Delay_us(UNIT_T);

    for (unsigned char i = 0; i < 8; i++)
    {
        NeZha_SDA(Byte & (0x80 >> i));
        NeZha_SCL(1);
        NeZha_Delay_us(UNIT_T);
        NeZha_SCL(0);
        NeZha_Delay_us(UNIT_T);
    }

    NeZha_SCL(1);
    NeZha_Delay_us(UNIT_T);
    NeZha_SCL(0);
    NeZha_Delay_us(UNIT_T);
}

/**
  * @brief  I2C读取一个字节
  * @param  Addr：要读出字节的地址
  * @retval Byte：读出的数据
  */
unsigned char NeZha_I2C_ReadByte()
{
    unsigned char Byte = 0x00;
    NeZha_I2C_SdaDir(1);

    for (unsigned char i = 0; i < 8; i++)
    {
        NeZha_SCL(0);
        NeZha_Delay_us(UNIT_T);
        NeZha_SCL(1);

        if (digitalRead(7))
        {
          Byte |= (0X80 >> i);
        }

        NeZha_Delay_us(UNIT_T);
    }

return Byte;
}

/**
  * @brief  I2C应答信号
  * @param  无
  * @retval 无
  */
void NeZha_I2C_ACK()
{
    NeZha_SCL(0);
    NeZha_I2C_SdaDir(0);
    NeZha_SDA(0);
    NeZha_Delay_us(UNIT_T);
    NeZha_SCL(1);
    NeZha_Delay_us(UNIT_T);
    NeZha_SCL(0);
}

/**
  * @brief  I2C不应答信号
  * @param  无
  * @retval 无
  */
void NeZha_I2C_NACK()
{
    NeZha_SCL(0);
    NeZha_I2C_SdaDir(0);
    NeZha_SDA(1);
    NeZha_Delay_us(UNIT_T);
    NeZha_SCL(1);
    NeZha_Delay_us(UNIT_T);
    NeZha_SCL(0);
}

void NeZha_Delay_ms(unsigned int xms) 
{
    delay(xms);
}

void NeZha_Delay_us(unsigned int xus) 
{
    delayMicroseconds(xus);
}