#include "LED.h"
#include <Arduino.h>
const int led = 13;
void Led_Init(){
	pinMode(led,OUTPUT);
}
void Led_On(){
	digitalWrite(led,HIGH);
}
void Led_Off(){
	digitalWrite(led,LOW);	
}
void Led_Turn(){
	if(digitalRead(led)){
		digitalWrite(led,LOW);
	}else{
		digitalWrite(led,HIGH);	
	}		
}