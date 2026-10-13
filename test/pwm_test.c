#include "pwm_test.h"
#include "app_pwm.h"
#include "delay.h"
//
// @简介：对PWM模块进行测试
//				先使能电机
//				让电机正转两秒(30%、60%、90%)
//				关闭电机
//
void PWM_Test(void)
{
	App_PWM_Init();
	App_PWM_Cmd(1);	//唤醒电机 

	App_PWM_Set_L(30);
	App_PWM_Set_R(30);
	Delay(2000);
	
	App_PWM_Set_L(60);
	App_PWM_Set_R(60);
	Delay(2000);
	
	App_PWM_Set_L(90);
	App_PWM_Set_R(90);
	Delay(2000);
	
	App_PWM_Cmd(0);
	
	while(1)
	{
	}
	
}
