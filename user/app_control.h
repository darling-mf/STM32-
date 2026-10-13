#ifndef APP_CONTROL_H__
#define APP_CONTROL_H__

#include "stm32f10x.h"

void App_Control_Init(void);
void App_Control_Proc(void);
void App_Control_Reset(void);
void App_Control_SetMoveSpeed(float MoveSpeed);
void App_Control_SetTurnSpeed(float TurnSpeed);

#endif
