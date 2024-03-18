/*
 * user_sl_platform.c
 *
 * Example of realizations of platform dependent functions
 */


#include "Platform/sl_platform.h"
#include "main.h"


uint32_t SL_GetTick()
{
	return HAL_GetTick();
}


void SL_Delay(uint32_t ms)
{
	HAL_Delay(ms);
}


void FOS_Platform_MainTim_Start()
{
	HAL_TIM_Base_Start_IT(&htim14);
}


void FOS_Platform_MainTim_Enable()
{
	__HAL_TIM_ENABLE(&htim14);
}


void FOS_Platform_MainTim_Disable()
{
	__HAL_TIM_DISABLE(&htim14);
}


void FOS_Platform_MainTim_SetCounter(uint32_t val)
{
	__HAL_TIM_SET_COUNTER(&htim14, val);
}


uint32_t FOS_Platform_MainTim_GetCounter()
{
	return __HAL_TIM_GET_COUNTER(&htim14);
}


void FOS_Platform_MainTim_SetARR(uint32_t val)
{
	__HAL_TIM_SET_AUTORELOAD(&htim14, val - 1);
}


void CallPendSV()
{
	SCB->ICSR |= (0x1 << SCB_ICSR_PENDSVSET_Pos);
}










