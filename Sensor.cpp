#include <Arduino.h>

/*
	SENSOR0(2)  SENSOR1(3)  SENSOR2(4)  SENSOR3(5)
*/
#define SENSOR0 2
#define SENSOR1 3
#define SENSOR2 4
#define SENSOR3 5

void Sensor_Init()
{
  pinMode(SENSOR0, INPUT); 
  pinMode(SENSOR1, INPUT); 
  pinMode(SENSOR2, INPUT);
  pinMode(SENSOR3, INPUT);
}

uint8_t Sensor0_Get_State()
{
	return digitalRead(SENSOR0);
}

uint8_t Sensor1_Get_State()
{
	return digitalRead(SENSOR1);
}

uint8_t Sensor2_Get_State()
{
	return digitalRead(SENSOR2);
}

uint8_t Sensor3_Get_State()
{
	return digitalRead(SENSOR3);
}

