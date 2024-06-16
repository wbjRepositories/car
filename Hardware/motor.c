#include "stm32f10x.h"                  // Device header
#include "motor.h"

void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(MOTOR_RCC_GPIO,ENABLE);
	RCC_APB1PeriphClockCmd(MOTOR_RCC_TIM,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	//GPIO初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = MOTOR_PWMA|MOTOR_PWMB;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = MOTOR_AIN1|MOTOR_AIN2|MOTOR_STBY;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = MOTOR_BIN1|MOTOR_BIN2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	//初始化方向
	GPIO_SetBits(GPIOA,MOTOR_AIN2);
	GPIO_ResetBits(GPIOA,MOTOR_AIN1);
	GPIO_SetBits(GPIOB,MOTOR_BIN2);
	GPIO_ResetBits(GPIOB,MOTOR_BIN1);
	GPIO_SetBits(GPIOA,MOTOR_STBY);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	//选择内部时钟作为时钟源
	TIM_InternalClockConfig(MOTOR_TIM);
	//时基单元配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = MOTOR_Period;
	TIM_TimeBaseInitStructure.TIM_Prescaler = MOTOR_Prescaler;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(MOTOR_TIM,&TIM_TimeBaseInitStructure);
	
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = MOTOR_Pulse;
	TIM_OC1Init(MOTOR_TIM,&TIM_OCInitStructure);
	TIM_OC2Init(MOTOR_TIM,&TIM_OCInitStructure);
	
	TIM_Cmd(MOTOR_TIM,ENABLE);
}

//speed 范围在-2000到+2000
void Motor_SetSpeedMotor1(int16_t speed)
{
	if(speed > 2000)
	{
		speed = 2000;
	}
	if(speed < -2000)
	{
		speed = -2000;
	}
	
	
	if(speed<0)
	{
		GPIO_SetBits(GPIOB,MOTOR_BIN2);
		GPIO_ResetBits(GPIOB,MOTOR_BIN1);
		TIM_SetCompare1(MOTOR_TIM,-speed);
	}
	else
	{
		GPIO_SetBits(GPIOB,MOTOR_BIN1);
		GPIO_ResetBits(GPIOB,MOTOR_BIN2);
		TIM_SetCompare1(MOTOR_TIM,speed);
	}
	
}
//speed 范围在-2000到+2000
void Motor_SetSpeedMotor2(int16_t speed)
{
	if(speed > 2000)
	{
		speed = 2000;
	}
	if(speed < -2000)
	{
		speed = -2000;
	}
	
	if(speed<0)
	{
		GPIO_SetBits(GPIOA,MOTOR_AIN2);
		GPIO_ResetBits(GPIOA,MOTOR_AIN1);
		TIM_SetCompare2(MOTOR_TIM,-speed);
	}
	else
	{
		GPIO_SetBits(GPIOA,MOTOR_AIN1);
		GPIO_ResetBits(GPIOA,MOTOR_AIN2);
		TIM_SetCompare2(MOTOR_TIM,speed);
	}
	
}

