#ifndef __MOTOR_H
#define __MOTOR_H
#endif

#define MOTOR_RCC_GPIO		RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB
#define MOTOR_RCC_TIM		RCC_APB1Periph_TIM3
#define MOTOR_TIM			TIM3
#define MOTOR_AIN1			GPIO_Pin_4		//GPIOA口
#define MOTOR_AIN2			GPIO_Pin_1		//GPIOA口
#define MOTOR_PWMA			GPIO_Pin_7		//GPIOA口
#define MOTOR_STBY			GPIO_Pin_5		//GPIOA口
#define MOTOR_BIN1			GPIO_Pin_4		//GPIOB口
#define MOTOR_BIN2			GPIO_Pin_3		//GPIOB口
#define MOTOR_PWMB 			GPIO_Pin_6		//GPIOA口
#define MOTOR_Period		2000-1			//自动重装值
#define MOTOR_Prescaler		720-1			//预分频值
#define MOTOR_Pulse			0				//输出比较值

void Motor_Init(void);
void Motor_SetSpeedMotor1(int16_t speed);
void Motor_SetSpeedMotor2(int16_t speed);
