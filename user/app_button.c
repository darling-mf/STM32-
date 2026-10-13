#include "app_button.h"
#include "button.h"
#include "app_pwm.h"
#include "app_motor.h"
#include "app_control.h"
// 配置按钮名称，用于初始化(相当于ADC1、USART1)
static Button_TypeDef userkey; //用户按钮

static void OnUserKey_Clicked(uint8_t clicks);
//
// @简介：按钮初始化
//
void App_Button_Init(void)
{
	Button_InitTypeDef Button_InitStruct ={0};
	Button_InitStruct.GPIOx = GPIOA;
	Button_InitStruct.GPIO_Pin = GPIO_Pin_11;
	
	My_Button_Init(&userkey,&Button_InitStruct);
	
	//配置回调函数
	My_Button_SetClickCb(&userkey,OnUserKey_Clicked);
}

void App_Button_Proc(void)
{
	My_Button_Proc(&userkey);
}

static uint8_t pwm_on =0; //0 - 休眠  1 - 活动
//
// @简介：按钮点击的回调函数
//
static void OnUserKey_Clicked(uint8_t clicks)
{
	App_Control_Reset();
	if(clicks  == 1)
	{
		//翻转电机状态
		if(pwm_on == 0)
		{
			pwm_on = 1;
		}
		else
		{
			pwm_on = 0;
		}
		
		App_Motor_Cmd(pwm_on);
	}
}

