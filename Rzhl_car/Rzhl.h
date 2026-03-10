
#include <stdint.h>

//Rzhl指令
#define RZHL_ADDR				0x80

#define RZHL_CMD_MOTOR_WAIT		0x00
#define RZHL_CMD_MOTOR_INIT		0x01
#define RZHL_CMD_MOTOR1_SET		0x05
#define RZHL_CMD_MOTOR2_SET		0x09
#define RZHL_CMD_MOTOR3_SET		0x0D
#define RZHL_CMD_MOTOR4_SET		0x11
#define RZHL_CMD_ENCODER1_INIT		0x15
#define RZHL_CMD_ENCODER1_READ		0x16
#define RZHL_CMD_ENCODER2_INIT		0x18
#define RZHL_CMD_ENCODER2_READ		0x19
#define RZHL_CMD_ENCODER3_INIT		0x1B
#define RZHL_CMD_ENCODER3_READ		0x1C
#define RZHL_CMD_ENCODER4_INIT		0x1E
#define RZHL_CMD_ENCODER4_READ		0x1F
#define RZHL_CMD_SERVO1_INIT		0x21
#define RZHL_CMD_SERVO1_SET		0x22
#define RZHL_CMD_SERVO2_INIT		0x24
#define RZHL_CMD_SERVO2_SET		0x25
#define RZHL_CMD_SERVO3_INIT		0x27
#define RZHL_CMD_SERVO3_SET		0x28
#define RZHL_CMD_SERVO4_INIT		0x2A
#define RZHL_CMD_SERVO4_SET		0x2B
#define RZHL_CMD_HEADLED_ON		0x2D
#define RZHL_CMD_HEADLED_OFF		0x2E
#define RZHL_CMD_HEADLED_TURN		0x2F
#define RZHL_CMD_TAILLEFTLED_ON	0x30
#define RZHL_CMD_TAILLEFTLED_OFF	0x31
#define RZHL_CMD_TAILLEFTLED_TURN	0x32
#define RZHL_CMD_TAILRIGHTLED_ON	0x33
#define RZHL_CMD_TAILRIGHTLED_OFF	0x34
#define RZHL_CMD_TAILRIGHTLED_TURN	0x35
#define RZHL_CMD_AMBIENTLED_ON		0x36
#define RZHL_CMD_AMBIENTLED_OFF	0x37
#define RZHL_CMD_AMBIENTLED_TURN   0X38
#define RZHL_CMD_RESET				0XFF

void Rzhl_Init(void);
void Rzhl_Reset(void);
void Rzhl_Motor_Init(void);
void Rzhl_Forward(uint16_t Speed);
void Rzhl_Backward(uint16_t Speed);
void Rzhl_TurnLeft(uint16_t Speed);
void Rzhl_TurnRight(uint16_t Speed);
void Rzhl_TransLeft(uint16_t Speed);
void Rzhl_TransRight(uint16_t Speed);
void Rzhl_Motor1_SetPwm(uint16_t motor_a,uint16_t motor_b);
void Rzhl_Motor2_SetPwm(uint16_t motor_a,uint16_t motor_b);
void Rzhl_Motor3_SetPwm(uint16_t motor_a,uint16_t motor_b);
void Rzhl_Motor4_SetPwm(uint16_t motor_a,uint16_t motor_b);

void Rzhl_Encoder1_Init(void);
void Rzhl_Encoder2_Init(void);
void Rzhl_Encoder3_Init(void);
void Rzhl_Encoder4_Init(void);
int16_t Rzhl_Encoder1_Read(void);
int16_t Rzhl_Encoder2_Read(void);
int16_t Rzhl_Encoder3_Read(void);
int16_t Rzhl_Encoder4_Read(void);

void Rzhl_Servo1_Init(void);
void Rzhl_Servo2_Init(void);
void Rzhl_Servo3_Init(void);
void Rzhl_Servo4_Init(void);
void Rzhl_Servo1_SetPwm(uint16_t pwm);
void Rzhl_Servo2_SetPwm(uint16_t pwm);
void Rzhl_Servo3_SetPwm(uint16_t pwm);
void Rzhl_Servo4_SetPwm(uint16_t pwm);

void Rzhl_HeadLed_TurnOn(void);
void Rzhl_HeadLed_TurnOff(void);
void Rzhl_HeadLed_Turn(void);
void Rzhl_AmbientLed_TurnOn(void);
void Rzhl_AmbientLed_TurnOff(void);
void Rzhl_AmbientLed_Turn(void);
void Rzhl_TailLeftLed_TurnOn(void);
void Rzhl_TailLeftLed_TurnOff(void);
void Rzhl_TailLeftLed_Turn(void);
void Rzhl_TailRightLed_TurnOn(void);
void Rzhl_TailRightLed_TurnOff(void);
void Rzhl_TailRightLed_Turn(void);

void Rzhl_WriteCommand(uint8_t Command);
