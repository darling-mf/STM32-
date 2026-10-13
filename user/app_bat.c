#include "app_bat.h"
#include "delay.h"

static volatile float vbat = 0.0f;

static void TIM2_TRGO_Init(void);
static void ADC1_Init (void);
static void LED_Init(void);

static uint32_t lastTime = 0; //上次切片时间
static uint8_t stage = 0; //当前状态  0：灭  1：亮

//
//  @简介：对电池电压模块进行初始化
//
void App_Bat_Init(void)
{
	TIM2_TRGO_Init();
	ADC1_Init();
	LED_Init();
}

//
// @简介：电池电压监控模块的任务切片
//
void App_Bat_Proc(void)
{
	if(vbat > 7.9)  // 100%
	{
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_SET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_6,Bit_SET);
	}
	else if(vbat > 7.4)  // 75%
	{
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_SET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_6,Bit_RESET);
	}
	else if(vbat > 7)  // 50%
	{
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_6,Bit_RESET);
	}
	else if(vbat > 6.5)  // 20%
	{
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_6,Bit_RESET);
	}
	else  // 低于20%
	{
		uint32_t now = GetTick();
		if(now - lastTime > 100)
		{
			switch(stage)
			{
				case 0:
					GPIO_WriteBit(GPIOA,GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6,Bit_SET);
					lastTime = now;
					stage =1;
					break;
				case 1:
					GPIO_WriteBit(GPIOA,GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6,Bit_RESET);
					lastTime = now;
					stage = 0;
					break;
			}
		}
	}	
}

//
// @简介：对电源指示灯进行初始化
//				PA4 PA5 PA6 Out_PP
//
static void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
}
static void TIM2_TRGO_Init(void)
{
		// #1.初始化TIM2_TRGO,每10ms产生一个脉冲(100Hz)
	  // 1.①开启TIM2时钟 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	  // 1.②配置时基单元
	TIM_TimeBaseInitTypeDef  TIM2_TimeBaseInitStruct = {0};
	
	TIM2_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM2_TimeBaseInitStruct.TIM_Period = 10000-1;  // 1MHz/10000 = 100Hz
	TIM2_TimeBaseInitStruct.TIM_Prescaler = 72-1;
	TIM2_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	 
	TIM_TimeBaseInit(TIM2,&TIM2_TimeBaseInitStruct);
	  // 1.③TGRO = update
	TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_Update); //选择主机模式为update
	  // 1.④闭合TIM2开关
	TIM_Cmd(TIM2,ENABLE);
}

static void ADC1_Init (void)
{
	// #2.初始化ADC
	  // 2.①分频系数
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); //ADC <14MHz
		// 2.②设置引脚模式
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN; //模拟模式
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	  // 2.③配置ADC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	ADC_InitTypeDef ADC_InitStruct = {0};
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //不使用常规序列，所以设置软件触发
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent; // 单ADC模式
	ADC_InitStruct.ADC_NbrOfChannel = 1;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE; //多个通道才需要扫描模式
	ADC_Init(ADC1,&ADC_InitStruct);
	  // 2.④设置注入序列
			//选择注入序列外部触发源
	ADC_ExternalTrigInjectedConvConfig(ADC1,ADC_ExternalTrigInjecConv_T2_TRGO);
			//闭合外部触发开关
	ADC_ExternalTrigInjectedConvCmd(ADC1,ENABLE);
			//选择通道8
	ADC_InjectedChannelConfig(ADC1,ADC_Channel_8,1,ADC_SampleTime_7Cycles5);
	
		// 2.⑤配置ADC的JEOC中断
			//闭合JEOC的开关
	ADC_ITConfig(ADC1,ADC_IT_JEOC,ENABLE);
			//NVIC中断优先级分组
	NVIC_InitTypeDef NVIC_InitStruct ={0};
	NVIC_InitStruct.NVIC_IRQChannel =ADC1_2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;  //子优先级

	NVIC_Init(&NVIC_InitStruct);
	
		// 2.⑥闭合开关
	ADC_Cmd(ADC1,ENABLE);
}

//
// @简介：获取电池的当前电压，符号是V
//
float App_Bat_Get(void)
{
	return vbat;
}

//
// @简介：ADC1,ADC2的中断显示函数 
//				会自己执行，无需调用
//
void ADC1_2_IRQHandler (void)
{
	if(ADC_GetFlagStatus(ADC1,ADC_FLAG_JEOC) == SET)
	{
		ADC_ClearFlag(ADC1,ADC_FLAG_JEOC);
		
		uint16_t jdr1 = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_1);
		
		//具体看word
		vbat = (jdr1 / 4095.0f * 3.3) * 8.4f/3.3;
	}
}
