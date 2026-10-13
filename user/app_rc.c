#include "app_rc.h"
#include <string.h>
#include <stdio.h>
#include "app_control.h"

#define CMD_MAX_LEN 64
static char intBuf[CMD_MAX_LEN]; //专用于中断程序的缓冲区
static char transBuf[CMD_MAX_LEN]; //在中断程序和进程函数间转运数据的缓冲区
static char procBuf[CMD_MAX_LEN]; //专用于进程函数的缓冲区

static volatile uint8_t lineReceiveFlag = 0;//一行字符串接收完成的标志位
static uint16_t intBufCursor = 0; //intBuf的游标，指向数值的下一个空白位置
//
// @简介：对遥控器模块进行初始化
//
void App_RC_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	//初始化PB10 - AFPP - USART_Tx
	GPIO_InitTypeDef GPIO_InitStruct ={0};
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//初始化PB11 - IPU - USART_Rx
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//开启USART3的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	USART_InitTypeDef USART_InitStruct = {0};
	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_Mode = USART_Mode_Rx  | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3,&USART_InitStruct);
	
	//开启USART3的RXNE中断
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStruct ={0};
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	
	NVIC_Init(&NVIC_InitStruct);
	
	//开启总开关
	USART_Cmd(USART3,ENABLE);
}
	
//
// @简介：遥控器的进程函数
//
void App_RC_Proc(void)
{
	if(lineReceiveFlag)
	{
		strcpy(procBuf,transBuf);
		lineReceiveFlag = 0;
		
		//解析
			//@简介：判断前n个字符是否相等
			//@返回：0 - 相等 非零 - 不相等
		if(strncasecmp(procBuf,"move ",5) == 0)
		{
			int turnSpeed,moveSpeed;
			//@简介:读取格式化字符串中的数据
			//@参数:str- 被读取的字符串 [procBuf]
			//@参数:format-字符串的格式 [turnSpeed、moveSpeed]
			//@返回:成功解析的参数个数
			if(sscanf(procBuf,"move %d %d",&turnSpeed,&moveSpeed) == 2)
			{
				//执行
					//人为设定了摇杆往前是-100%，因此要加负号，无其他含义
					//限制了车速最高为0.7m/s	
				App_Control_SetMoveSpeed(-moveSpeed *0.01f*0.7f);	
				App_Control_SetTurnSpeed(-turnSpeed *0.01f*15.0f);
			}
		}
	}
}

//
// @简介：USART3的中断响应函数
//
void USART3_IRQHandle(void)
{
	if(USART_GetFlagStatus(USART3,USART_FLAG_RXNE) == SET)
	{
		uint8_t data = USART_ReceiveData(USART3);
		if(data != '\n')
		{
			intBuf[intBufCursor++] = data;
		}
		else
		{
			intBuf[intBufCursor] = '\0';
			intBufCursor = 0; //重新归位
			strcpy(transBuf,intBuf);
			lineReceiveFlag = 1;
		}
	}
}