#include "bat_test.h"
#include "app_usart2.h"
#include "app_bat.h"
#include "delay.h"

//
// @简介：电池电压监控模块的测试程序
//				通过串口2把电压发送给电脑
//        用vofa显示函数
//

void Bat_Test(void) //相当于main
{
	App_USART2_Init();
	App_Bat_Init();
	
	while(1)
	{
		//每间隔10ms显示一个数据点
		float volt = App_Bat_Get();
		
		My_USART_Printf(USART2,"%.3f\n",volt);
		
		Delay(10);

	}
}
