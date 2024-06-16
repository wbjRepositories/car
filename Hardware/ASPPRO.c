#include "stm32f10x.h"                  // Device header
#include "motor.h"
#include "OLED.h"

static uint16_t speed = 1000;
int16_t statu = 0; //0停止；1直线前进；2后退；3左转；4右转；5原地左转；6原地右转;7自动避障模式
static int16_t tmpSpeed = 0;


void ASPPRO_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2,&USART_InitStructure);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
	USART_Cmd(USART2,ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
}

//0x00	停下
//0x10	前进
//0x11	后退 
//0x20	左转
//0x21	右转 
//0x22	原地左转
//0x23	原地右转 
//0x24	停止左转或右转
//0x30	一档速度 10
//0x31	二档速度 20
//0x32	三档速度 30
//0x33	四档速度 40
void USART2_IRQHandler(void)
{
	uint16_t data = USART_ReceiveData(USART2);
	int16_t LeftSpeed = 0;
	int16_t RightSpeed = 0;
	statu = 0; //0停止；1直线前进；2后退；3左转；4右转；5原地左转；6原地右转；7自动避障模式
//	tmpSpeed = 0;
	
	if(data==0x40)	//0x40	关闭自动避障模式
	{
		statu = 0;
	}
	if(data==0x41)	//0x41	开启自动避障模式
	{
		statu = 7;
	}
	
	
	
	if(data==0x30)				//0x30	一档速度 10
	{
		speed = 500;
	}
	else if(data==0x31)				//0x31	二档速度 20
	{
		speed = 1000;
	}
	else if(data==0x32)					//0x32	三档速度 30
	{
		speed = 1500;
	}
	else if(data==0x33)					//0x33	四档速度 40
	{
		speed = 2000;
	}
	LeftSpeed = speed;
	RightSpeed = speed;
	
	if(data==0x00)			//0x00	停下
	{
		statu = 0;
	}
	else if(data==0x10)		//0x10	前进
	{
		statu = 1;
	}
	else if(data==0x11)		//0x11	后退 
	{
		statu = 2;
		LeftSpeed = -speed;
		RightSpeed = -speed;
	}
	else if(data==0x20)			//0x20	左转
	{
		statu = 3;
		
		if(LeftSpeed == RightSpeed)
		{
			tmpSpeed = LeftSpeed;
			LeftSpeed = LeftSpeed - 1.0*LeftSpeed/2000*800;
			RightSpeed = RightSpeed + 1.0*LeftSpeed/2000*800;
		}
		else if(LeftSpeed > RightSpeed)
		{
			LeftSpeed = tmpSpeed;
			RightSpeed = tmpSpeed;
			LeftSpeed = LeftSpeed - 1.0*LeftSpeed/2000*800;
			RightSpeed = RightSpeed + 1.0*LeftSpeed/2000*800;
		}
		
	}
	else if(data==0x21)			//0x21	右转 
	{
		statu = 4;
		
		if(LeftSpeed == RightSpeed)
		{
			tmpSpeed = LeftSpeed;
			LeftSpeed = LeftSpeed + 1.0*LeftSpeed/2000*800;
			RightSpeed = RightSpeed - 1.0*LeftSpeed/2000*800;
		}
		else if(LeftSpeed < RightSpeed)
		{
			LeftSpeed = tmpSpeed;
			RightSpeed = tmpSpeed;
			LeftSpeed = LeftSpeed + 1.0*LeftSpeed/2000*800;
			RightSpeed = RightSpeed - 1.0*LeftSpeed/2000*800;
		}
		
	}
	else if(data==0x22 && (statu == 0 || statu == 5 || statu == 6))	//0x22	原地左转
	{
		statu = 5;
		LeftSpeed = -500;
		RightSpeed = 500;
	}
	else if(data==0x23 && (statu == 0 || statu == 6 || statu == 6))	//0x23	原地右转 
	{
		statu = 6;
		LeftSpeed = 500;
		RightSpeed = -500;
	}
	if(data==0x24)		//停止左右转
	{
		if(statu == 3 && statu == 4)
		{
			statu = 1;
			LeftSpeed = tmpSpeed;
			RightSpeed = tmpSpeed;
		}
		if(statu == 5 && statu == 6)
		{
			statu = 0;
			LeftSpeed = 0;
			RightSpeed = 0;
		}
	}

	if(statu == 0 || statu == 7)
	{
		Motor_SetSpeedMotor1(0);
		Motor_SetSpeedMotor2(0);
	}
	else
	{
		Motor_SetSpeedMotor1(RightSpeed);
		Motor_SetSpeedMotor2(LeftSpeed);
	}
	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
}

