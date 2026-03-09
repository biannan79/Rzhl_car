#include "Board_Timer.h"
#include <TimerOne.h>

uint8_t Board_Timer_Flag = 0; 

void Board_Timer_Init()
{
  Timer1.initialize(20000);
  Timer1.attachInterrupt(Board_Timer_Irq);
	Board_Timer_Flag = 0;
}

uint8_t Board_Timer_Flag_Get()
{
	if (Board_Timer_Flag == 1)
	{
		Board_Timer_Flag = 0;
		return 1;
	}
	
	return 0;
}
void Board_Timer_Irq(void)
{
  // 定时器中断处理函数
  // 在这里执行需要定时执行的操作
		Board_Timer_Flag = 1;
}
