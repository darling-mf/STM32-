#include "stm32f10x.h"

#include "bat_test.h"
#include "pwm_test.h"
#include "encoder_test.h"
#include "mpu6050_test.h"
#include "qmath_test.h"

#include "delay.h"
#include "task.h"
#include "app_bat.h"
#include "app_button.h"
#include "app_encoder.h"
#include "app_pwm.h"
#include "app_motor.h"
#include "app_usart2.h"
#include "app_mpu6050.h"
#include "app_control.h"
#include "app_rc.h"

//设定转速
//static float targetOmega = 0.0f;
//static void USART2_Proc(void);

int main(void)
{
	//配置NIVC中断优先级(JEOC涉及中断，需要配置NIVC)
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	//QMath_Test();
	//MPU6050_EularAngleTest();
	//MPU6050_Test();
	//Encoder_T_Method_Test();
	//PWM_Test();
	//Encoder_Test();
	
	App_Button_Init();
	App_Encoder_Init();
	App_PWM_Init();
	App_Bat_Init();
	App_Motor_Init();
	App_USART2_Init();
	App_MPU6050_Init();
	App_Control_Init();
	App_RC_Init();
	
	while(1)
	{
		//随着时间变化的转速，每秒增加2rad/s，每10秒一个循环	
//		targetOmega = (GetTick()/1000) % 10 * 2.0f;
//		App_Motor_SetOmega_L(targetOmega);
//		App_Motor_SetOmega_R(targetOmega);
//		
		App_Bat_Proc();										
		App_Button_Proc();
		App_Motor_Proc();
//		USART2_Proc();
		App_MPU6050_Proc();
		App_Control_Proc();
		App_RC_Proc();
	}
}

//static void USART2_Proc(void)
//{
//	PERIODIC(10);
//	float omega_l = App_Encoder_GetSpeed_L();
//	float omega_r = App_Encoder_GetSpeed_R();
//	
//	My_USART_Printf(USART2,"%.3f,%.3f,%.3f\n",targetOmega,omega_l,omega_r);
//}
